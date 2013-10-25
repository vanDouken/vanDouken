//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_STEERINGPROVIDER_HPP
#define VANDOUKEN_STEERINGPROVIDER_HPP

#include "config.hpp"
#include "cell.hpp"

#include <hpx/lcos/future.hpp>
#include <hpx/util/high_resolution_timer.hpp>

#include <libgeodecomp/storage/grid.h>
#include <libgeodecomp/geometry/region.h>

#include <boost/serialization/shared_ptr.hpp>

#include <deque>

namespace vandouken {
    class SteeringProvider
    {
    public:
        typedef
            hpx::util::function<void(
                LibGeoDecomp::GridBase<Cell, 2> *,
                const LibGeoDecomp::Region<2>&,
                const LibGeoDecomp::Coord<2>&,
                unsigned step)
            >
            SteererFunctor;

        SteeringProvider(std::size_t numUpdateGroups, const LibGeoDecomp::Coord<2>& dim);

        ~SteeringProvider();

        void steer(const SteererFunctor& f);

    private:
        std::vector<hpx::id_type> serverIds;
    };
}

#endif
