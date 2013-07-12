//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef FLOWINGCANVAS_COLLECTOR_HPP
#define FLOWINGCANVAS_COLLECTOR_HPP

#include <serialize/coord.hpp>
#include <canvascell.h>
#include <server/collector.hpp>
#include "flowingcanvas_gui.hpp"

namespace LibGeoDecomp { namespace HiParSimulator {
    namespace server {
        typedef
            LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell>
            flowingcanvas_collector;
    }
}}

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::step_finished_action
  , flowingcanvas_collector_step_finished_action
)
/*
HPX_ACTION_HAS_CRITICAL_PRIORITY(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::step_finished_action
)
*/

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::start_action
  , flowingcanvas_collector_start_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::steer_action
  , flowingcanvas_collector_steer_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::new_force_action
  , flowingcanvas_collector_new_force_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_collector::new_image_action
  , flowingcanvas_collector_new_image_action
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    boost::shared_ptr<
        LibGeoDecomp::Steerer<
            LibGeoDecomp::CanvasCell
        >
    >
  , steerer_canvascell
)

#endif
