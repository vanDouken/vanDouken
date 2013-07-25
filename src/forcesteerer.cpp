//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "forcesteerer.hpp"

namespace vandouken {
    ForceSteerer::ForceSteere(unsigned period) :
        LibGeoDecomp::Steerer<Cell>(period)
    {
        std::size_t retry = 0;
        while(providerId == hpx::naming::invalid_id)
        {
            hpx::agas::resolve_name(VANDOUKEN_GRIDCOLLECTOR_NAME, Id);
            if(retry > 10) {
                throw std::logic_error("Could not connect to simulation");
            }
            ++retry;
        }
        consumerId = GridCollectorServer::AddGridConsumerAction()(collectorId);
        collectingFuture =
            hpx::async<GridCollectorServer::GetNextGridAction>(collectorId, consumerId).then(
                HPX_STD_BIND(
                    &GridProvider::setNextGrid,
                    this,
                    HPX_STD_PLACEHOLDERS::_1
                )
            );
    }
        
    ~ForceSteerer::ForceSteerer()
    {
    }
        
    void addSteerer(hpx::id_type id)
    {
        providerIds.push_back(id);
        //collectingFutures.push_back(
            hpx::async<ForceProviderServer::GetNextForceAction>(id).then(
                HPX_STD_BIND(
                    &GridProvider::setNextForces,
                    this,
                    HPX_STD_PLACEHOLDERS::_1
                )
            );
        //);
    }

    void removeSteerer(hpx::id_type id)
    {
        std::vector<hpx::id_type>::iterator it = providerIds.find(it);
        if(it != providerIds.end())
        {
            providerIds.erase(it);
        }
    }

    void ForceSteerer::nextStep(
        ForceSteerer::GridType *grid,
        const ForceSteerer::RegionType& validRegion,
        const ForceSteerer::CoordType& globalDimensions,
        unsigned step,
        LibGeoDecomp::SteererEvent event,
        std::size_t rank,
        bool lastCall)
    {
        if (origins.size() > 0) {
            Region lastRenderedRegion;
            for (int i = 0; i < origins.size(); ++i) {
                Coord<2> origin = origins[i];
                FloatCoord<2> delta = deltas[i];
                double length = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
                FloatCoord<2> force = delta * (1.0 / length);
                
                if (abs(delta[0]) > abs(delta[1])) {
                    if (delta[0] < 0) {
                        origin = origin + Coord<2>(delta[0], delta[1]);
                        delta = delta * -1;
                    }
                    for (int x = 0; x <= delta[0]; ++x) {
                        int y = origin[1] + x * delta[1] / (delta[0] + 1);
                        renderForce(origin[0] + x, y, force, grid, validRegion, &lastRenderedRegion);
                    }
                } else {
                    if (delta[1] < 0) {
                        origin = origin + Coord<2>(delta[0], delta[1]);
                        delta = delta * -1;
                    }
                    for (int y = 0; y <= delta[1]; ++y) {
                        int x = origin[0] + y * delta[0] / (delta[1] + 1);
                        renderForce(x, origin[1] + y, force, grid, validRegion, &lastRenderedRegion);
                    }
                }
            }

            origins.clear();
            deltas.clear();
    }
    
    void setNextForces(
        hpx::future<
            std::pair<
                std::vector<CoordType>,
                std::vector<LibGeoDecomp::FloatCoord<2> >
            >
        > forceFuture)
    {
        Mutex::scoped_lock l(mtx);
        std::pair<
            std::vector<CoordType>,
            std::vector<LibGeoDecomp::FloatCoord<2> >
        > res = forceFuture.move();

        origins.insert(origins.end(), res.first.begin(), res.first.end());
        forces.insert(forces.end(), res.second.begin(), res.second.end());

        hpx::async<ForceProviderServer::GetNextForceAction>(id).then(
            HPX_STD_BIND(
                &GridProvider::setNextForces,
                this,
                HPX_STD_PLACEHOLDERS::_1
            )
        );
    }
}
