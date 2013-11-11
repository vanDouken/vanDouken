//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "resetsimulation.hpp"

HPX_UTIL_REGISTER_FUNCTION(
    void(
        LibGeoDecomp::GridBase<vandouken::Cell, 2> *
      , const LibGeoDecomp::Region<2>&
      , const LibGeoDecomp::Coord<2>&
      , unsigned
    )
  , vandouken::ResetSimulation
  , vandoukenResetSimulation
)
