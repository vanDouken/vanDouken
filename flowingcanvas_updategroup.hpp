//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef FLOWINGCANVAS_UPDATEGROUP_HPP
#define FLOWINGCANVAS_UPDATEGROUP_HPP

#include <hpx/include/compression_zlib.hpp>
#include <hpx/include/compression_bzip2.hpp>
#include <hpx/include/compression_snappy.hpp>
#include <canvascell.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitions/recursivebisectionpartition.h>
#include <server/hpx_updategroup.hpp>

namespace LibGeoDecomp { namespace HiParSimulator {
    namespace server {
        typedef
            LibGeoDecomp::HiParSimulator::server::hpx_updategroup<
                LibGeoDecomp::CanvasCell
              , LibGeoDecomp::HiParSimulator::RecursiveBisectionPartition<2>
            >
            flowingcanvas_updategroup;
        typedef
            std::pair<int,int>
            int_pair_type;
    }
}}

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::init_action
  , flowingcanvas_updategroup_init_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::reset_ghostzone_action
  , flowingcanvas_updategroup_reset_ghostzone_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::set_outer_ghostzone_action
  , flowingcanvas_updategroup_set_outer_ghostzone_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::register_writer_action
  , flowingcanvas_updategroup_register_writer_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::update_action
  , flowingcanvas_updategroup_update_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::current_step_action
  , flowingcanvas_updategroup_current_step_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::notify_initialized_action
  , flowingcanvas_updategroup_notifiy_initialized_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::notify_done_action
  , flowingcanvas_updategroup_notifiy_done_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::connect_writer_action
  , flowingcanvas_updategroup_connect_writer_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::disconnect_writer_action
  , flowingcanvas_updategroup_disconnect_writer_action
)
HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::collect_finished_action
  , flowingcanvas_updategroup_collect_finished_action
)
/*
HPX_ACTION_HAS_CRITICAL_PRIORITY(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::collect_finished_action
)
*/

/*
HPX_ACTION_USES_ZLIB_COMPRESSION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::collect_finished_action
    )
*/
HPX_ACTION_USES_SNAPPY_COMPRESSION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::collect_finished_action
    )

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::bbox_action
  , flowingcanvas_updategroup_bbox_action
)

HPX_REGISTER_ACTION_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::connect_steerer_action
  , flowingcanvas_updategroup_connect_steerer_action
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::int_pair_type
  , std_pair_int_int
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    LibGeoDecomp::CoordBox<2>
  , flowingcanvas_coord_box
)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    LibGeoDecomp::HiParSimulator::server::flowingcanvas_updategroup::buffer_vector_type
  , flowingcanvas_buffer_vector
)

#endif
