//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "gridcollectorserver.hpp"
#include <libgeodecomp/parallelization/hiparsimulator/gridvecconv.h>

#include <hpx/lcos/broadcast.hpp>

namespace vandouken {
    GridProvider::GridProvider(std::size_t numUpdateGroups, const LibGeoDecomp::Coord<2>& dim) :
        dim(dim),
        stopped(false),
        consumerIds(numUpdateGroups),
        collectorIds(numUpdateGroups)
    {
        for(std::size_t i = 0; i < numUpdateGroups; ++i)
        {
            std::size_t retry = 0;
            std::string name(VANDOUKEN_GRIDCOLLECTOR_NAME);
            name += "/";
            name += boost::lexical_cast<std::string>(i);
            while(collectorIds[i] == hpx::naming::invalid_id)
            {
                hpx::agas::resolve_name_sync(name, collectorIds[i]);
                if(retry > 10) {
                    throw std::logic_error("Could not connect to simulation");
                }
                if(!collectorIds[i])
                {
                    hpx::this_thread::suspend(boost::posix_time::seconds(1));
                }
                ++retry;
            }
            std::cout << "resolved: " << name << "\n";
            consumerIds[i] = GridCollectorServer::AddGridConsumerAction()(collectorIds[i]);
        }

        timer.restart();
        std::vector<hpx::future<BufferType> > futures;
        futures.reserve(collectorIds.size());
        for(std::size_t i = 0; i < collectorIds.size(); ++i)
        {
            futures.push_back(
                hpx::async<GridCollectorServer::GetNextBufferAction>(collectorIds[i], consumerIds[i])
            );
        }

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
        hpx::wait(collectingFuture);
        for(std::size_t i = 0; i < collectorIds.size(); ++i)
        {
            GridCollectorServer::RemoveGridConsumerAction()(collectorIds[i], consumerIds[i]);
        }
    }

    void GridProvider::setNextGrid(
        hpx::future<std::vector<hpx::future<BufferType> > >& buffersFuture)
    {
        std::vector<hpx::future<BufferType> > buffers = buffersFuture.move();
        BufferType res;

        BOOST_FOREACH(hpx::future<BufferType>& bufferFuture, buffers)
        {
            BufferType buffer = bufferFuture.move();
            if(!res)
            {
                res = buffer;
                continue;
            }
            if(!buffer) continue;
            res->posAngle.insert(res->posAngle.end(), buffer->posAngle.begin(), buffer->posAngle.end());
            res->colors.insert(res->colors.end(), buffer->colors.begin(), buffer->colors.end());
        }

        {
            MutexType::scoped_lock l(mutex);
            if(res && res->size() > 0)
                grid = res;
        }

        if(!stopped)
        {
            timer.restart();
            std::vector<hpx::future<BufferType> > futures;
            futures.reserve(collectorIds.size());
            for(std::size_t i = 0; i < collectorIds.size(); ++i)
            {
                futures.push_back(
                    hpx::async<GridCollectorServer::GetNextBufferAction>(collectorIds[i], consumerIds[i])
                );
            }

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
        BufferType res = grid;
        grid.reset();
        return res;
    }
}
