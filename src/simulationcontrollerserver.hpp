//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_SIMULATIONCONTROLLERSERVER_HPP
#define VANDOUKEN_SIMULATIONCONTROLLERSERVER_HPP

#include "simulator.hpp"

#include <libgeodecomp/misc/coord.h>
#include <libgeodecomp/io/hpxwritercollector.h>
#include <libgeodecomp/io/initializer.h>
#include "particleconverter.hpp"

namespace vandouken {
    class SimulationControllerServer :
        public hpx::components::managed_component_base<SimulationControllerServer>
    {
    public:
        typedef LibGeoDecomp::Coord<2> CoordType;

        typedef
            hpx::components::server::create_component_action2<
                SimulationControllerServer,
                const CoordType,
                std::size_t>
            CreateComponentAction;

        SimulationControllerServer();

        SimulationControllerServer(const CoordType& simulationDim, std::size_t overcommitFactor);

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

    private:
        Simulator simulator;
    };

    typedef LibGeoDecomp::HpxWriterCollector<vandouken::Cell, ParticleConverter> ConverterSinkType;
}

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

LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR_DECLARATION(
    LibGeoDecomp::HpxWriterCollector<vandouken::Cell>,
    vandoukenHpxWriterCollector)

LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR_DECLARATION(
    vandouken::ConverterSinkType,
    vandoukenHpxWriterConverterCollector)

#endif
