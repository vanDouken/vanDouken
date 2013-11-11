//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_SIMULATIONCONTROLLERSERVER_HPP
#define VANDOUKEN_SIMULATIONCONTROLLERSERVER_HPP

#include "simulator.hpp"

#include <libgeodecomp/geometry/coord.h>
#include <libgeodecomp/io/hpxwritercollector.h>
#include <libgeodecomp/io/initializer.h>

namespace vandouken {
    class SimulationControllerServer :
        public hpx::components::managed_component_base<SimulationControllerServer>
    {
    public:
        typedef LibGeoDecomp::Coord<2> CoordType;

        typedef
            hpx::components::server::create_component_action1<
                SimulationControllerServer,
                const CoordType>
            CreateComponentAction;

        SimulationControllerServer();

        SimulationControllerServer(const CoordType& simulationDim);

        void run()
        {
            simulator.run();
        }

        HPX_DEFINE_COMPONENT_ACTION(
            SimulationControllerServer,
            run,
            RunAction);

        void stop()
        {
            simulator.stop();
        }

        HPX_DEFINE_COMPONENT_ACTION(
            SimulationControllerServer,
            stop,
            StopAction);

        boost::shared_ptr<LibGeoDecomp::Initializer<Cell> > getInitializer();

        HPX_DEFINE_COMPONENT_ACTION(
            SimulationControllerServer,
            getInitializer,
            GetInitializerAction);

        std::size_t numUpdateGroups()
        {
            return simulator.numUpdateGroups();
        }

        HPX_DEFINE_COMPONENT_ACTION(
            SimulationControllerServer,
            numUpdateGroups,
            NumUpdateGroupsAction);

    private:
        Simulator simulator;
    };
}

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    boost::shared_ptr<LibGeoDecomp::Initializer<vandouken::Cell> >
  , LibGeoDecompInitializerVandoukenCellLCO
)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::SimulationControllerServer::CreateComponentAction,
    vandoukenSimulationControllerServerCreateComponentAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::SimulationControllerServer::GetInitializerAction,
    vandoukenSimulationControllerServerGetInitializerAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::SimulationControllerServer::RunAction,
    vandoukenSimulationControllerServerRunAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::SimulationControllerServer::StopAction,
    vandoukenSimulationControllerServerStopAction)

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::SimulationControllerServer::NumUpdateGroupsAction,
    vandoukenSimulationControllerNumUpdateGroupsAction)

/*
LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR_DECLARATION(
    vandouken::ConverterSinkType,
    vandoukenHpxWriterConverterCollector)
*/

#endif
