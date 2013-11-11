//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_PARTICLESTEERERSERVER_HPP
#define VANDOUKEN_PARTICLESTEERERSERVER_HPP

#include "config.hpp"
#include "cell.hpp"

#include <libgeodecomp/storage/grid.h>
#include <libgeodecomp/geometry/region.h>
#include <libgeodecomp/communication/serialization.h>

#include <hpx/include/components.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace vandouken
{
    class ParticleSteerer;

    class ParticleSteererServer :
        public hpx::components::managed_component_base<ParticleSteererServer>
    {
    public:
        typedef
            hpx::util::function<void(
                LibGeoDecomp::GridBase<Cell, 2> *,
                const LibGeoDecomp::Region<2>&,
                const LibGeoDecomp::Coord<2>&,
                unsigned step)
            >
            SteererFunctor;

        ParticleSteererServer()
        {
            BOOST_ASSERT(false);
        }

        typedef 
            hpx::components::server::create_component_action1<
                vandouken::ParticleSteererServer
              , vandouken::ParticleSteerer *
            >
            CreateAction;

        typedef 
            hpx::components::server::create_component_action1<
                vandouken::ParticleSteererServer
              , vandouken::ParticleSteerer * const
            >
            ConstCreateAction;

        ParticleSteererServer(ParticleSteerer *steerer) :
            collector(steerer)
        {}

        void steer(const SteererFunctor& f);
        HPX_DEFINE_COMPONENT_ACTION(ParticleSteererServer, steer, SteerAction);

    private:
        ParticleSteerer *collector;
    };
}

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::ParticleSteererServer::CreateAction
  , vandoukenParticleSteererServerCreateAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::ParticleSteererServer::ConstCreateAction
  , vandoukenParticleSteererServerConstCreateAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::ParticleSteererServer::SteerAction,
    vandoukenParticleSteererServerSteerAction)

#endif
