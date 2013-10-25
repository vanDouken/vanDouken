//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_GRIDPROVIDER_HPP
#define VANDOUKEN_GRIDPROVIDER_HPP

#include "config.hpp"
#include "cell.hpp"
#include "particle.hpp"

#include <hpx/lcos/future.hpp>
#include <hpx/util/high_resolution_timer.hpp>

#include <libgeodecomp/storage/grid.h>
#include <libgeodecomp/geometry/region.h>

#include <boost/serialization/shared_ptr.hpp>

#include <deque>

namespace vandouken {
    class GridProvider
    {
    public:
        typedef
            boost::shared_ptr<Particles>
            BufferType;

        GridProvider(std::size_t numUpdateGroups, const LibGeoDecomp::Coord<2>& dim);

        ~GridProvider();

        BufferType nextGrid();

    private:

        void setNextGrid(hpx::future<std::vector<hpx::future<BufferType> > >& buffersFuture);

        typedef hpx::lcos::local::spinlock MutexType;
        MutexType mutex;
        hpx::util::high_resolution_timer timer;

        LibGeoDecomp::Coord<2> dim;

        hpx::future<void> collectingFuture;
        boost::atomic<bool> stopped;
        BufferType grid;
        std::vector<std::size_t> consumerIds;
        std::vector<hpx::id_type> collectorIds;
    };
}

#endif
