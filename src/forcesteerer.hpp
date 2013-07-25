//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCESTEERER_HPP
#define VNADOUKEN_FORCESTEERER_HPP

#include "cell.hpp"

namespace vandouken {

    class ForceSteerer : public LibGeoDecomp::Steerer<Cell>
    {
    public:
        ForceSteerer(unsigned period);

        ~ForceSteerer();

        void ForceSteerer::nextStep(
            ForceSteerer::GridType *grid,
            const ForceSteerer::RegionType& validRegion,
            const ForceSteerer::CoordType& globalDimensions,
            unsigned step,
            LibGeoDecomp::SteererEvent event,
            std::size_t rank,
            bool lastCall);

        void addForce(std::vector<CoordType> const &o, );
        void removeSteerer(hpx::id_type id);
    private:
        void setNextForces(
            hpx::future<
                std::pair<
                    std::vector<CoordType>,
                    std::vector<LibGeoDecomp::FloatCoord<2> >
                >
            > forceFuture);

        std::vector<hpx::id_type> providerIds;
    };
}

#endif
