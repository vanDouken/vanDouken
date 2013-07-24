//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "simulationcontrollerserver.hpp"
#include "initializer.hpp"
#include "singletracer.hpp"

LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR(
    LibGeoDecomp::HpxWriterCollector<vandouken::Cell>,
    vandoukenHpxWriterCollector)


LIBGEODECOMP_REGISTER_HPX_WRITER_COLLECTOR(
    vandouken::ConverterSinkType,
    vandoukenHpxWriterConverterCollector)

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

    SimulationControllerServer::SimulationControllerServer(const LibGeoDecomp::Coord<2>& simulationDim, std::size_t overcommitFactor) :
        simulator(
            createInitializer(simulationDim),
            overcommitFactor,
            0, // Balancer
            1, // Balancing Period
            1  //ghostzoneWidth
        )
    {
        LibGeoDecomp::HpxWriterCollector<Cell>::SinkType sink(
            new SingleTracer(10 /*period*/),
            simulator.numUpdateGroups());

        simulator.addWriter(
            new LibGeoDecomp::HpxWriterCollector<Cell>(
                sink));

        LibGeoDecomp::HpxWriterCollector<Cell, ParticleConverter>::SinkType guiSink(
            1,
            simulator.numUpdateGroups(),
            VANDOUKEN_GUI_SINK_NAME);

        simulator.addWriter(
            new LibGeoDecomp::HpxWriterCollector<Cell, ParticleConverter>(
                guiSink));
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

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::managed_component<vandouken::SimulationControllerServer>,
    vandoukenSimulationControllerServer);
