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

#include <libgeodecomp/misc/grid.h>

#include <boost/serialization/shared_ptr.hpp>

#include <deque>

namespace vandouken {
    class GridProvider
    {
    public:
        typedef LibGeoDecomp::Grid<Particles> GridType;

        GridProvider();

        ~GridProvider();

        boost::shared_ptr<GridType> nextGrid();

    private:

        void setNextGrid(hpx::future<boost::shared_ptr<GridType> > gridFuture);

        hpx::future<void> collectingFuture;
        boost::atomic<bool> stopped;
        boost::shared_ptr<GridType> grid;
        std::size_t consumerId;
        hpx::id_type collectorId;
    };
}

#endif
