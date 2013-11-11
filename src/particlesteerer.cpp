//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "config.hpp"

#include "particlesteerer.hpp"
#include "particlesteererserver.hpp"
#include <libgeodecomp/communication/serialization.h>
#include <hpx/hpx.hpp>

BOOST_CLASS_EXPORT_GUID(vandouken::ParticleSteerer, "vandoukenParticleSteerer");

namespace vandouken {
    void ParticleSteerer::nextStep(
        GridType *grid,
        const LibGeoDecomp::Region<2>& validRegion,
        const CoordType& globalDimensions,
        unsigned step,
        LibGeoDecomp::SteererEvent,
        std::size_t rank,
        bool lastCall,
        SteererFeedback *feedback)
    {
        Mutex::scoped_lock l(mutex);
        if(!steererServerId)
        {
            std::string name(VANDOUKEN_PARTICLESTEERER_NAME);
            name += "/";
            name += boost::lexical_cast<std::string>(rank);
            steererServerId = hpx::components::new_<ParticleSteererServer>(hpx::find_here(), this).get();
            hpx::agas::register_name_sync(name, steererServerId);
            std::cout << "registered: " << name << "\n";
        }


        BOOST_FOREACH(const SteererFunctor& f, steererFunctors)
        {
            f(grid, validRegion, globalDimensions, step);
        }
        
        steererFunctors.clear();
        updatedRegion.clear();

        /*
        else
        {
            std::cout << rank << " complete region " << region << "\n";
            std::cout << rank << " valid region " << validRegion << "\n";
            std::cout << rank << " missing region " << (updatedRegion - region) << "\n";
        }
        */
    }

    void ParticleSteerer::steer(const SteererFunctor& f)
    {
        Mutex::scoped_lock l(mutex);
        steererFunctors.push_back(f);
    }
}

