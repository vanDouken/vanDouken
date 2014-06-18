//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_RESETSIMULATION_HPP
#define VANDOUKEN_RESETSIMULATION_HPP

#include "config.hpp"
#include "initializer.hpp"

#include <hpx/hpx_fwd.hpp>
#include <hpx/util/function.hpp>

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

HPX_UTIL_REGISTER_FUNCTION_DECLARATION(
    void(
        LibGeoDecomp::GridBase<vandouken::Cell, 2> *
      , const LibGeoDecomp::Region<2>&
      , const LibGeoDecomp::Coord<2>&
      , unsigned
    )
  , vandouken::ResetSimulation
  , vandoukenResetSimulation
)

#endif
