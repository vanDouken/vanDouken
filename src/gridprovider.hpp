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
#include "particleconverter.hpp"

#include <hpx/lcos/future.hpp>

#include <libgeodecomp/misc/grid.h>
#include <libgeodecomp/io/writer.h>
#include <libgeodecomp/io/hpxwritercollector.h>

#include <boost/serialization/shared_ptr.hpp>

#include <deque>

namespace vandouken {
    class GridProvider
    {
        class Writer;

    public:
        typedef LibGeoDecomp::Grid<Particles> GridType;
        typedef hpx::lcos::local::spinlock MutexType;

        GridProvider();

        ~GridProvider();

        boost::shared_ptr<GridProvider::GridType> nextGrid();

    private:
        LibGeoDecomp::HpxWriterSink<Cell, ParticleConverter> guiSink;
        std::deque<boost::shared_ptr<GridType> > grids;
        hpx::future<std::size_t> writerId;
        MutexType mtx;

        class Writer : public LibGeoDecomp::Writer<Particles>
        {
            friend class boost::serialization::access;
        public:
            Writer(GridProvider *gridProvider) :
                LibGeoDecomp::Writer<Particles>("", 1),
                gridProvider(gridProvider)
            {}

            LibGeoDecomp::Writer<Particles> *clone()
            {
                return new Writer(gridProvider);
            }

            void stepFinished(
                const GridType& grid,
                unsigned step,
                LibGeoDecomp::WriterEvent event)
            {
                MutexType::scoped_lock lk(gridProvider->mtx);
                gridProvider->grids.push_back(boost::make_shared<GridType>(grid));
            }

        private:
            GridProvider *gridProvider;

            template<class ARCHIVE>
            void serialize(ARCHIVE& ar, unsigned)
            {
                BOOST_ASSERT(false);
            }
        };
    };
}

LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR_DECLARATION(
    LibGeoDecomp::HpxWriterCollector<vandouken::Particles>,
    vandoukenHpxWriterParticleCollector)

#endif
