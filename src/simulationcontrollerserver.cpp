//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "simulationcontrollerserver.hpp"
#include "initializer.hpp"
#include "gridcollector.hpp"
#include "particlesteerer.hpp"

#include <libgeodecomp/io/tracingwriter.h>

#include <hpx/hpx_fwd.hpp>

/*
LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR(
    vandouken::ConverterSinkType,
    vandoukenHpxWriterConverterCollector)
*/

namespace vandouken {
    SimulationControllerServer::SimulationControllerServer() :
        simulator(
            new Initializer(LibGeoDecomp::Coord<2>()),
            0,
            0, // Balancer
            1, // Balancing Period
            1  //ghostzoneWidth
        )
    {
        BOOST_ASSERT(false);
    }

    SimulationControllerServer::SimulationControllerServer(const LibGeoDecomp::Coord<2>& simulationDim) :
        simulator(
            createInitializer(simulationDim),
            boost::lexical_cast<float>(hpx::get_config_entry("vandouken.overcommitfactor", "1.0")),
            0, // Balancer
            1, // Balancing Period
            1  //ghostzoneWidth
        )
    {
        simulator.addWriter(new GridCollector(1));
        /*
        simulator.addSteerer(new ParticleSteerer());
        */

        /*
        simulator.addWriter(
            new LibGeoDecomp::TracingWriter<Cell>(10, (std::numeric_limits<unsigned>::max)()));
        */
    }

    boost::shared_ptr<LibGeoDecomp::Initializer<Cell> > SimulationControllerServer::getInitializer()
    {
        return simulator.getInitializer();
    }
}

HPX_REGISTER_ACTION(
    vandouken::SimulationControllerServer::CreateComponentAction,
    vandoukenSimulationControllerServerCreateComponentAction)

HPX_REGISTER_ACTION(
    vandouken::SimulationControllerServer::GetInitializerAction,
    vandoukenSimulationControllerServerGetInitializerAction)

HPX_REGISTER_ACTION(
    vandouken::SimulationControllerServer::RunAction,
    vandoukenSimulationControllerServerRunAction)

HPX_REGISTER_ACTION(
    vandouken::SimulationControllerServer::StopAction,
    vandoukenSimulationControllerServerStopAction)

HPX_REGISTER_ACTION(
    vandouken::SimulationControllerServer::NumUpdateGroupsAction,
    vandoukenSimulationControllerNumUpdateGroupsAction)

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::managed_component<vandouken::SimulationControllerServer>,
    vandoukenSimulationControllerServer);
