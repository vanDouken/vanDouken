//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "gridcollectorserver.hpp"

namespace vandouken {
    GridProvider::GridProvider() :
        stopped(false)
    {
        std::size_t retry = 0;
        while(collectorId == hpx::naming::invalid_id)
        {
            hpx::agas::resolve_name(VANDOUKEN_GRIDCOLLECTOR_NAME, collectorId);
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

    GridProvider::~GridProvider()
    {
        stopped = true;
        hpx::wait(collectingFuture);
        GridCollectorServer::RemoveGridConsumerAction()(collectorId, consumerId);
    }

    void GridProvider::setNextGrid(hpx::future<boost::shared_ptr<GridType> > gridFuture)
    {
        grid = gridFuture.move();
        if(!stopped)
        {
            collectingFuture =
                hpx::async<GridCollectorServer::GetNextGridAction>(collectorId, consumerId).then(
                    HPX_STD_BIND(
                        &GridProvider::setNextGrid,
                        this,
                        HPX_STD_PLACEHOLDERS::_1
                    )
                );
        }
    }

    boost::shared_ptr<GridProvider::GridType> GridProvider::nextGrid()
    {
        return grid;
    }
}
