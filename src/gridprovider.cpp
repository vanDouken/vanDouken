//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "gridcollectorserver.hpp"
#include <libgeodecomp/storage/gridvecconv.h>

#include <hpx/lcos/broadcast.hpp>

#include <utility>

namespace vandouken {
    GridProvider::GridProvider(std::size_t numUpdateGroups, const LibGeoDecomp::CoordBox<2>& boundingBox) :
        //region(region),
        stopped(false),
        consumerIds(numUpdateGroups),
        collectorIds(numUpdateGroups)
    {
        region << boundingBox;
        for(std::size_t i = 0; i < numUpdateGroups; ++i)
        {
            std::string name(VANDOUKEN_GRIDCOLLECTOR_NAME);
            name += "/";
            name += boost::lexical_cast<std::string>(i);
            while(collectorIds[i] == hpx::naming::invalid_id)
            {
                hpx::naming::id_type id;
                hpx::agas::resolve_name_sync(name, id);
                if(!id)
                {
                    hpx::this_thread::suspend(boost::posix_time::seconds(1));
                    continue;
                }
                hpx::naming::gid_type gid = id.get_gid();
                hpx::naming::detail::strip_credits_from_gid(gid);
                collectorIds[i] = hpx::id_type(gid, hpx::id_type::unmanaged);
            }
            std::cout << "resolved: " << name << "\n";
            consumerIds[i] = GridCollectorServer::AddGridConsumerAction()(collectorIds[i]);
        }

        timer.restart();
        
        // create list of futures
        std::vector<
            hpx::lcos::future<std::pair<unsigned, RegionBuffer> >
        > futures;

        // add all futures to list
        futures.reserve(collectorIds.size());
        for(std::size_t i = 0; i < collectorIds.size(); ++i)
        {
            futures.push_back(
                hpx::async<GridCollectorServer::GetNextBufferAction>
                                      (collectorIds[i], consumerIds[i])
            );
        }

        // write future to class
        collectingFuture = hpx::when_all(futures).then(
            HPX_STD_BIND(
                &GridProvider::setNextGrid,
                this,
                HPX_STD_PLACEHOLDERS::_1
            )
        );


    }

    GridProvider::~GridProvider()
    {
        stopped = true;
        collectingFuture.wait();
        for(std::size_t i = 0; i < collectorIds.size(); ++i)
        {
            GridCollectorServer::RemoveGridConsumerAction()(collectorIds[i], consumerIds[i]);
        }
    }

    void GridProvider::setNextGrid(
        hpx::lcos::future<
            std::vector<
                hpx::lcos::future<std::pair<unsigned, RegionBuffer> >
            >
        > && buffersFuture)
    {
        typedef std::pair<unsigned, RegionBuffer> pair_type;
        std::vector<hpx::lcos::future<pair_type> > buffers =
                                                            buffersFuture.get();
        BufferType res;


        BOOST_FOREACH(hpx::lcos::future<pair_type>& bufferFuture, buffers)
        {
            std::pair<unsigned, RegionBuffer> regionBuffer = bufferFuture.get();
            if(regionBuffer.second.region.empty())
            {
                continue;
            }

            {
                MutexType::scoped_lock l(mutex);
                BufferMap::iterator it = bufferMap.find(regionBuffer.first);

                if(it == bufferMap.end())
                {
                    it =
                        bufferMap.insert(it,
                            std::make_pair(
                                regionBuffer.first
                              , RegionBuffer(regionBuffer.second.buffer, region)
                            )
                        );
                }
                else
                {
                    std::vector<float> & posAngle = it->second.buffer->posAngle;
                    std::vector<float> & colors = it->second.buffer->colors;
                    posAngle.insert(
                        posAngle.end()
                      , regionBuffer.second.buffer->posAngle.begin()
                      , regionBuffer.second.buffer->posAngle.end());
                    colors.insert(
                        colors.end()
                      , regionBuffer.second.buffer->colors.begin()
                      , regionBuffer.second.buffer->colors.end());
                }

                it->second.region -= regionBuffer.second.region;

            }
        }

        if(!stopped)
        {
            timer.restart();

            // create list of futures
            std::vector<
                hpx::lcos::future<std::pair<unsigned, RegionBuffer> >
            > futures;

            // add all futures to list
            futures.reserve(collectorIds.size());
            for(std::size_t i = 0; i < collectorIds.size(); ++i)
            {
                futures.push_back(
                    hpx::async<GridCollectorServer::GetNextBufferAction>
                                      (collectorIds[i], consumerIds[i])
                );
            }

            // combine list of futures to one future
            collectingFuture = hpx::when_all(futures).then(
                HPX_STD_BIND(
                    &GridProvider::setNextGrid,
                    this,
                    HPX_STD_PLACEHOLDERS::_1
                )
            );

        }
    }

    GridProvider::BufferType GridProvider::nextGrid()
    {
        MutexType::scoped_lock l(mutex);
        BufferType res;
        BufferMap::iterator begin = bufferMap.begin();
        BufferMap::iterator it;
        bool erase = false;

        for(it = begin; it != bufferMap.end(); ++it)
        {
            if(it->second.region.empty())
            {
                res = it->second.buffer;
                erase = true;
                break;
            }
        }
        if(erase)
        {
            bufferMap.erase(begin, ++it);
        }
        if(bufferMap.size() > 10) bufferMap.clear();
        return res;
    }
}
