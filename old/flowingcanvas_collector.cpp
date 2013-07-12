//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/include/components.hpp>
#include <hpx/include/actions.hpp>
#include "flowingcanvas_gui.hpp"

template struct HPX_COMPONENT_EXPORT
    LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell>;

#include <flowingcanvas_updategroup.hpp>
#include <flowingcanvas_simulator.hpp>
#include <flowingcanvas_collector.hpp>

HPX_REGISTER_ACTION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::step_finished_action
  , flowingcanvas_collector_step_finished_action
)

HPX_REGISTER_ACTION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::start_action
  , flowingcanvas_collector_start_action
)

HPX_REGISTER_ACTION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::steer_action
  , flowingcanvas_collector_steer_action
)

HPX_REGISTER_ACTION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::new_force_action
  , flowingcanvas_collector_new_force_action
)

HPX_REGISTER_ACTION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::new_image_action
  , flowingcanvas_collector_new_image_action
)

HPX_REGISTER_BASE_LCO_WITH_VALUE(
    boost::shared_ptr<
        LibGeoDecomp::Steerer<
            LibGeoDecomp::CanvasCell
        >
    >
  , steerer_canvascell
)

HPX_DEFINE_GET_COMPONENT_TYPE(
    LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell>
)

typedef hpx::components::managed_component< ::server::flowingcanvas_gui>  flowingcanvas_gui_component_type;

HPX_REGISTER_DERIVED_COMPONENT_FACTORY(
    flowingcanvas_gui_component_type, flowingcanvas_gui, "flowingcanvas_collector"
)
