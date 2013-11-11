//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "particlesteererserver.hpp"
#include "particlesteerer.hpp"

namespace vandouken {
    void ParticleSteererServer::steer(const SteererFunctor& f)
    {
        return collector->steer(f);
    }
}

HPX_REGISTER_ACTION(
    vandouken::ParticleSteererServer::CreateAction
  , vandoukenParticleSteererServerSteerCreateAction)

HPX_REGISTER_ACTION(
    vandouken::ParticleSteererServer::ConstCreateAction
  , vandoukenParticleSteererServerSteerConstCreateAction)

HPX_REGISTER_ACTION(
    vandouken::ParticleSteererServer::SteerAction,
    vandoukenParticleSteererServerSteerAction)

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::managed_component<vandouken::ParticleSteererServer>,
    vandoukenParticleSteererServerServer);
