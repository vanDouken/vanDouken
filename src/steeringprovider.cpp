//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "steeringprovider.hpp"
#include "particlesteererserver.hpp"
#include "log.hpp"
#include <libgeodecomp/storage/gridvecconv.h>

#include <hpx/lcos/broadcast.hpp>

namespace vandouken {
    SteeringProvider::SteeringProvider(std::size_t numUpdateGroups, const LibGeoDecomp::Coord<2>& dim) :
        serverIds(numUpdateGroups)
    {
        for(std::size_t i = 0; i < numUpdateGroups; ++i)
        {
            std::string name(VANDOUKEN_PARTICLESTEERER_NAME);
            name += "/";
            name += boost::lexical_cast<std::string>(i);

            LOG("trying to resolve " << name << "\n");
            
            while(serverIds[i] == hpx::naming::invalid_id)
            {
                hpx::naming::id_type id;
                hpx::agas::resolve_name_sync(name, id);
                if(!id)
                {
                    hpx::this_thread::suspend(boost::posix_time::seconds(1));
                    continue;
                }
                hpx::naming::gid_type gid = id.get_gid();
                hpx::naming::detail::strip_credit_from_gid(gid);
                serverIds[i] = hpx::id_type(gid, hpx::id_type::unmanaged);
            }
            LOG("resolved: " << name << "\n");
        }
    }

    SteeringProvider::~SteeringProvider()
    {
    }

    void SteeringProvider::steer(const SteererFunctor& f)
    {
        for(std::size_t i = 0; i < serverIds.size(); ++i)
        {
            LOG("steering ... " << serverIds[i] << "\n");
            hpx::apply<ParticleSteererServer::SteerAction>(serverIds[i], f);
        }
    }
}
