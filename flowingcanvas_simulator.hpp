//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef FLOWINGCANVAS_SIMULATOR_HPP
#define FLOWINGCANVAS_SIMULATOR_HPP

#include <canvascell.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitions/recursivebisectionpartition.h>
#include <server/hpx_simulator.hpp>

namespace LibGeoDecomp { namespace HiParSimulator {
    namespace server {
        typedef
            LibGeoDecomp::HiParSimulator::server::hpx_simulator<
                LibGeoDecomp::CanvasCell
              , LibGeoDecomp::HiParSimulator::RecursiveBisectionPartition<2>
            >
            flowingcanvas_simulator;
    }
}}

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::create_component_action
  , flowingcanvas_simulator_create_component_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::init_action
  , flowingcanvas_simulator_init_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::num_partitions_action
  , flowingcanvas_simulator_num_partitions_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::getInitializer_action
  , flowingcanvas_simulator_getInitializer_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::updategroups_action
  , flowingcanvas_simulator_updategroups_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::writers_action
  , flowingcanvas_simulator_writers_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::registerWriter_action
  , flowingcanvas_simulator_registerWriter_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::step_action
  , flowingcanvas_simulator_step_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::connect_writer_action
  , flowingcanvas_simulator_connect_writer_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::disconnect_writer_action
  , flowingcanvas_simulator_disconnect_writer_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::connect_steerer_action
  , flowingcanvas_simulator_connect_steerer_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::wait_action
  , flowingcanvas_simulator_wait_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::stop_action
  , flowingcanvas_simulator_stop_action
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    boost::shared_ptr<LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::initializer_type>
  , flowingcanvas_initializer
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    std::vector<LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::updategroup_type>
  , flowingcanvas_updategroups
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_simulator::writer_vector
  , flowingcanvas_writers
)

#endif
