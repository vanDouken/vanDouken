//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "config.hpp"

#include "particlesteerer.hpp"
#include "particlesteererserver.hpp"
#include <hpx/hpx.hpp>

namespace vandouken {
    void ParticleSteerer::nextStep(
        GridType *grid,
        const LibGeoDecomp::Region<2>& validRegion,
        const CoordType& globalDimensions,
        unsigned step,
        LibGeoDecomp::SteererEvent,
        std::size_t rank,
        bool lastCall)
    {
        Mutex::scoped_lock l(mutex);
        if(!steererServerId)
        {
            std::string name(VANDOUKEN_PARTICLESTEERER_NAME);
            name += "/";
            name += boost::lexical_cast<std::string>(rank);
            steererServerId = hpx::components::new_<ParticleSteererServer>(hpx::find_here(), this).get();
            hpx::agas::register_name(name, steererServerId);
            std::cout << "registered: " << name << "\n";
        }

        BOOST_FOREACH(const SteererFunctor& f, steererFunctors)
        {
            f(grid, validRegion, globalDimensions, step);
        }
        if(lastCall)
            steererFunctors.clear();
    }
    
    void ParticleSteerer::steer(const SteererFunctor& f)
    {
        Mutex::scoped_lock l(mutex);
        steererFunctors.push_back(f);
    }
}
