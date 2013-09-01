//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "steeringprovider.hpp"
#include "particlesteererserver.hpp"
#include <libgeodecomp/parallelization/hiparsimulator/gridvecconv.h>

#include <hpx/lcos/broadcast.hpp>

namespace vandouken {
    SteeringProvider::SteeringProvider(std::size_t numUpdateGroups, const LibGeoDecomp::Coord<2>& dim) :
        serverIds(numUpdateGroups)
    {
        for(std::size_t i = 0; i < numUpdateGroups; ++i)
        {
            std::size_t retry = 0;
            std::string name(VANDOUKEN_PARTICLESTEERER_NAME);
            name += "/";
            name += boost::lexical_cast<std::string>(i);
            while(serverIds[i] == hpx::naming::invalid_id)
            {
                hpx::agas::resolve_name(name, serverIds[i]);
                if(retry > 10) {
                    throw std::logic_error("Could not connect to simulation");
                }
                if(!serverIds[i])
                {
                    hpx::this_thread::suspend(boost::posix_time::seconds(1));
                }
                ++retry;
            }
            std::cout << "resolved: " << name << "\n";
        }
    }

    SteeringProvider::~SteeringProvider()
    {
    }

    void SteeringProvider::steer(const SteererFunctor& f)
    {
        for(std::size_t i = 0; i < serverIds.size(); ++i)
        {
            hpx::apply<ParticleSteererServer::SteerAction>(serverIds[i], f);
        }
    }
}
