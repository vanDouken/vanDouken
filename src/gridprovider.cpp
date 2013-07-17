//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"

#include <libgeodecomp/io/hpxwritercollector.h>

LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR(
    LibGeoDecomp::HpxWriterCollector<vandouken::Particles>,
    vandoukenHpxWriterParticleCollector)

namespace vandouken {
    GridProvider::GridProvider() :
        guiSink(VANDOUKEN_GUI_SINK_NAME)
    {
        LibGeoDecomp::HpxWriterCollector<Particles>::SinkType
            thisSink(
                new GridProvider::Writer(this), guiSink.numUpdateGroups());

        writerId = guiSink.connectWriter(new LibGeoDecomp::HpxWriterCollector<Particles>(
                    thisSink));
    }

    GridProvider::~GridProvider()
    {
        guiSink.disconnectWriter(writerId.get());
    }

    boost::shared_ptr<GridProvider::GridType> GridProvider::nextGrid()
    {
        MutexType::scoped_lock lk(mtx);
        boost::shared_ptr<GridProvider::GridType> grid;
        if(grids.empty()) return grid;

        grid = grids.back();
        grids.clear();
        return grid;
    }
}
