//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_RESETSIMULATION_HPP
#define VNADOUKEN_RESETSIMULATION_HPP

#include "initializer.hpp"

#include <libgeodecomp/geometry/floatcoord.h>
#include <libgeodecomp/geometry/region.h>

namespace vandouken {

    using LibGeoDecomp::Coord;
    using LibGeoDecomp::FloatCoord;
    using LibGeoDecomp::Region;
    using LibGeoDecomp::Streak;

    class ResetSimulation
    {
    public:
        ResetSimulation() {}

        void operator()(
                LibGeoDecomp::GridBase<Cell, 2> *grid,
                const LibGeoDecomp::Region<2>& validRegion,
                const LibGeoDecomp::Coord<2>& globalDimensions,
                unsigned)
        {
            Initializer * init = createInitializer(globalDimensions);
            init->grid(grid);
        }

        template <typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
        }
    };
}

#endif
