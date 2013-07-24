//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_GRIDCOLLECTOR_HPP
#define VANDOUKEN_GRIDCOLLECTOR_HPP

#include "config.hpp"
#include "particle.hpp"

#include <hpx/runtime/naming/name.hpp>
#include <hpx/lcos/local/spinlock.hpp>
#include <hpx/util/high_resolution_timer.hpp>

#include <boost/serialization/access.hpp>
#include <libgeodecomp/io/writer.h>

namespace vandouken {
    class GridCollector : public LibGeoDecomp::Writer<Particles>
    {
        friend class boost::serialization::access;

        typedef
        std::map<
            std::size_t,
            std::map<
                unsigned,
                boost::shared_ptr<GridType>
            >
        > GridMap;

        typedef hpx::lcos::local::spinlock Mutex;
    public:
        GridCollector(unsigned period);

        void stepFinished(const GridType& grid, unsigned step, LibGeoDecomp::WriterEvent event);

        std::size_t addGridConsumer();
        void removeGridConsumer(std::size_t);

        boost::shared_ptr<GridType> getNextGrid(std::size_t id);
    private:
        GridCollector()
        {}

        template<typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
            ar & boost::serialization::base_object<LibGeoDecomp::Writer<Particles> >(*this);
        }

        std::size_t nextId;
        std::set<std::size_t> ids;
        GridMap grids;

        hpx::util::high_resolution_timer timer;

        hpx::naming::id_type collectorServerId;
        Mutex mutex;
    };
}

#endif
