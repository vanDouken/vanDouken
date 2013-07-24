//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "config.hpp"

#include "gridcollector.hpp"
#include "gridcollectorserver.hpp"

#include <boost/serialization/export.hpp>

#include <hpx/hpx.hpp>
#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

namespace vandouken {
    GridCollector::GridCollector(unsigned period) :
        Writer<Particles>("", period),
        nextId(0)
    {
        collectorServerId = hpx::components::new_<GridCollectorServer>(hpx::find_here(), this).get();
        hpx::agas::register_name(VANDOUKEN_GRIDCOLLECTOR_NAME, collectorServerId);
    }

    void GridCollector::stepFinished(const GridType& grid, unsigned step, LibGeoDecomp::WriterEvent event)
    {
        if(step == 0) timer.restart();
        if(step % 10 == 0)
        {
            std::cout
                << "Simulation at step "
                << step
                << " (FPS: " << step/timer.elapsed() << ")"
                << "\n";
        }

        Mutex::scoped_lock l(mutex);
        if(!ids.empty())
        {
            boost::shared_ptr<GridType> g(new GridType(grid));
            BOOST_FOREACH(std::size_t id, ids)
            {
                GridMap::iterator it = grids.find(id);
                if(it == grids.end())
                {
                    it = grids.insert(
                        it,
                        std::make_pair(
                            id
                          , std::map<unsigned, boost::shared_ptr<GridType> >()
                        )
                    );
                }
                it->second.insert(
                    std::make_pair(
                        step
                      , g
                    )
                );
            }
        }
    }

    std::size_t GridCollector::addGridConsumer()
    {
        Mutex::scoped_lock l(mutex);
        std::size_t id = nextId++;
        std::set<size_t>::iterator it = ids.find(id);
        if(it == ids.end())
        {
            ids.insert(id);
        }
        return id;
    }

    void GridCollector::removeGridConsumer(std::size_t id)
    {
        Mutex::scoped_lock l(mutex);
        std::set<size_t>::iterator it = ids.find(id);
        BOOST_ASSERT(it != ids.end());
        ids.erase(it);
    }

    boost::shared_ptr<GridCollector::GridType> GridCollector::getNextGrid(std::size_t id)
    {
        Mutex::scoped_lock l(mutex);
        boost::shared_ptr<GridCollector::GridType> res;
        GridMap::iterator it = grids.find(id);

        if((it == grids.end()) || it->second.empty())
            return res;

        res = it->second.begin()->second;
        if(it->second.size() > 10)
            it->second.clear();

        return res;
    }
}

BOOST_CLASS_EXPORT_GUID(vandouken::GridCollector, "vandoukenGridCollector");
