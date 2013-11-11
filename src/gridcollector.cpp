//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "config.hpp"

#include "gridcollector.hpp"
#include "gridcollectorserver.hpp"
#include "particleconverter.hpp"

#include <libgeodecomp/communication/serialization.h>
#include <boost/serialization/export.hpp>

#include <hpx/hpx.hpp>
#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

namespace vandouken {
    GridCollector::GridCollector(unsigned period) :
        LibGeoDecomp::ParallelWriter<Cell>("", period),
        nextId(0)
    {
    }

    void GridCollector::stepFinished(
        const GridCollector::GridType& grid,
        const GridCollector::RegionType& validRegion,
        const GridCollector::CoordType& globalDimensions,
        unsigned step,
        LibGeoDecomp::WriterEvent event,
        std::size_t rank,
        bool lastCall)
    {
        {
            Mutex::scoped_lock l(mutex);
            if(!collectorServerId)
            {
                std::string name(VANDOUKEN_GRIDCOLLECTOR_NAME);
                name += "/";
                name += boost::lexical_cast<std::string>(rank);
                collectorServerId = hpx::components::new_<GridCollectorServer>(hpx::find_here(), this).get();
                hpx::agas::register_name_sync(name, collectorServerId);
                std::cout << "registered: " << name << "\n";
            }
        }
        if(step == 0) timer.restart();
        if(lastCall && step % 10 == 0 && rank == 0)
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
            Mutex::scoped_lock bl(bufferMutex);
            BufferMap::iterator it = buffers.find(step);
            if(it == buffers.end())
            {
                it = buffers.insert(
                    it
                  , std::make_pair(
                        step
                      , RegionBuffer(
                            boost::make_shared<Particles>()
                          , validRegion
                        )
                    )
                );
                it->second.buffer->reserve(globalDimensions.prod());
            }
            else
            {
                it->second.region += validRegion;
            }

            for (RegionType::StreakIterator i = validRegion.beginStreak();
                 i != validRegion.endStreak(); ++i) {
                GridType::ConstIterator src = grid.at(i->origin);
                //it->second->resize(i->length());
                for(int j = 0; j < i->length(); ++j)
                {
                    ParticleConverter()(*src, globalDimensions, *it->second.buffer);
                    ++src;
                }
            }

            /*
            std::cout << it->second->size() << "\n";
            if(lastCall) std::cout << "\n";
            */
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

    std::pair<unsigned, RegionBuffer>
    GridCollector::getNextBuffer(std::size_t id)
    {
        Mutex::scoped_lock l(bufferMutex);
        std::pair<unsigned, RegionBuffer> res;

        BufferMap::iterator it = buffers.begin();
        if(it == buffers.end()) return res;

        res = *it;

        buffers.erase(it);
        if(buffers.size() > 10) buffers.clear();

        return res;

        /*
        BufferMap::iterator it = buffers.find(id);


        if(it == buffers.end())
            return res;

        if(it->second.empty())
            return res;


        res = it->second.begin()->second;
        if(it->second.size() > 10)
        {
            it->second.clear();
        }
        else
        {
            it->second.erase(it->second.begin());
        }

        return res;
        */
    }
}

BOOST_CLASS_EXPORT_GUID(vandouken::GridCollector, "vandoukenGridCollector");
