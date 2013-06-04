//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SERVER_UPDATEGROUP_HPP
#define HPX_SERVER_UPDATEGROUP_HPP

#include <libgeodecomp/parallelization/distributedsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitionmanager.h>
#include <libgeodecomp/parallelization/hiparsimulator/offsethelper.h>
#include <libgeodecomp/parallelization/hiparsimulator/vanillastepper.h>
#include <libgeodecomp/misc/displacedgrid.h>

#include <serialize/supervector.hpp>

#include <hpx_patchlinks.hpp>
//#include <server/hpx_simulator.hpp>
#include <hpx_simulator.hpp>

#include <hpx/include/components.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/lcos/local/trigger.hpp>
#include <hpx/include/iostreams.hpp>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/serialization/detail/get_data.hpp>

#include <boost/smart_ptr/shared_ptr.hpp>

#include <serialize/coord.hpp>
#include <serialize/coordbox.hpp>

#include <model_test/particle.h>

#if !defined(LOG)
#if !defined(__ANDROID__) || !defined(ANDROID)
#define LOG(x,...) std::cout << x;
#else
#include <android/log.h>
#define LOG(x,...)                                                              \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "hpiif_android", "%s", sstr.str().c_str());            \
}                                                                               \
/**/
#endif
#endif

namespace LibGeoDecomp { namespace HiParSimulator {

    namespace server {

    namespace detail {
        template <typename CellType>
        struct distributor;
        
        template <typename CellType>
        struct steerer_collector;
    }

    template <typename CellType, typename Partition>
    struct hpx_simulator;

    template <typename CellType, typename Partition>
    struct HPX_EXPORT hpx_updategroup
        : hpx::components::managed_component_base<
            hpx_updategroup<CellType, Partition>
        >
    {
        typedef typename CellType::Topology topology;
        static const int dim = topology::DIMENSIONS;

        typedef LibGeoDecomp::DistributedSimulator<CellType> simulator_type;
        //typedef typename simulator_type::GridType grid_type;
        typedef LibGeoDecomp::DisplacedGrid<CellType, topology, true> grid_type;
        typedef LibGeoDecomp::Initializer<CellType> initializer_type;

        typedef VanillaStepper<CellType> stepper_type;
        typedef typename stepper_type::PatchType patch_type;
        typedef typename stepper_type::PatchProviderPtr patchprovider_ptr;
        typedef typename stepper_type::PatchAccepterPtr patchacceptor_ptr;
        
        typedef PartitionManager<dim, topology> partition_manager_type;
        typedef typename partition_manager_type::RegionVecMap region_vec_map_type;

        typedef typename stepper_type::PatchAccepterVec patch_accepter_vec;

        hpx_updategroup()
            : ghostzone_width(1)
            , part_man(new partition_manager_type)
            , init_future(init_promise.get_future())
            , init_patchlink_provider_future(init_patchlink_provider_trigger.get_future())
        {}

        void reset_ghostzone(std::size_t rank, CoordBox<dim> const & bbox)
        {
            init_future.get();
            BOOST_ASSERT(rank != this_rank);
            bboxes[rank] = bbox;
            reset_ghostzone_gate.set(rank);
        }
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, reset_ghostzone, reset_ghostzone_action);

        void set_outer_ghostzone(
            std::size_t src_rank, boost::shared_ptr<SuperVector<CellType> > buffer
          , long nanoStep
        )
        {
            hpx::wait(init_patchlink_provider_future);
            typename std::map<std::size_t, boost::shared_ptr<patchlink_provider_type> >::iterator
                patchlink_iter;
            {
                mutex_type::scoped_lock lk(patchlink_provider_mtx);
                patchlink_iter = patchlink_provider_map.find(src_rank);
                BOOST_ASSERT(patchlink_iter != patchlink_provider_map.end());
            }

            patchlink_iter->second->set_buffer(buffer, nanoStep);
        }

        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, set_outer_ghostzone, set_outer_ghostzone_action);

        void guess_offset(Coord<dim> *offset, Coord<dim> *dimensions, initializer_type * init, CoordBox<dim> const & bbox)
        {
            OffsetHelper<dim -1, dim, topology>()(
                offset
              , dimensions
              , bbox
              , init->gridBox()
              , ghostzone_width
            );
        }

        /*CoordBox<dim>*/void init(
            std::size_t rank
          , hpx::naming::id_type simulator_id
        );

        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, init, init_action);
        
        void register_writer(boost::shared_ptr<ParallelWriter<CellType> > writer);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, register_writer, register_writer_action);

        void update(unsigned nanoStep);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, update, update_action);
        
        void notify_initialized();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, notify_initialized, notify_initialized_action);
        
        void notify_done();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, notify_done, notify_done_action);
        
        std::pair<int, int> current_step()
        {
            if(stepper)
                return stepper->currentStep();
            else
                return std::pair<int, int>(sim_.getInitializer().get()->startStep(), 0);
        }
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, current_step, current_step_action);

        void connect_writer(std::size_t id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, connect_writer, connect_writer_action);

        void disconnect_writer(std::size_t id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, disconnect_writer, disconnect_writer_action);

        
        typedef 
            Particles
            buffer_type;
        typedef
            boost::shared_ptr<buffer_type>
            saved_buffer_type;
        typedef
            std::vector<saved_buffer_type>
            buffer_vector_type;
        buffer_vector_type collect_finished(std::size_t id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, collect_finished, collect_finished_action);

        CoordBox<dim> bbox()
        {
            return stepper->grid().boundingBox();
        }
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, bbox, bbox_action);

        void connect_steerer(hpx::id_type id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_updategroup, connect_steerer, connect_steerer_action);

        int getStep()
        {
            return current_step().first;
        }

        SuperVector<long> initial_weights(std::size_t items, std::size_t num_partitions)
        {
            SuperVector<long> res(num_partitions);
            long last_pos = 0;
            for(std::size_t i = 0; i < num_partitions; ++i)
            {
                long current_pos = items * (i + 1) / num_partitions;
                res[i] = current_pos - last_pos;
                last_pos = current_pos;
            }

            return res;
        }

        typedef hpx::lcos::local::spinlock mutex_type;
        std::size_t this_rank;
        boost::shared_ptr<stepper_type> stepper;
        std::size_t ghostzone_width;
        SuperVector<CoordBox<dim> > bboxes;
        boost::shared_ptr<partition_manager_type> part_man;
        hpx::lcos::local::base_and_gate<> reset_ghostzone_gate;
        hpx::lcos::promise<void> init_promise;
        hpx::lcos::future<void> init_future;
        
        hpx::lcos::local::base_trigger<> init_patchlink_provider_trigger;
        hpx::lcos::future<void> init_patchlink_provider_future;


        typedef hpx_patchlink_provider<grid_type> patchlink_provider_type;
        std::map<std::size_t, boost::shared_ptr<patchlink_provider_type> > patchlink_provider_map;
        mutex_type patchlink_provider_mtx;
        mutex_type grid_mtx;
        LibGeoDecomp::HiParSimulator::hpx_simulator<CellType, Partition> sim_;
        
        boost::shared_ptr<detail::distributor<CellType> > distributor_;
        boost::shared_ptr<detail::steerer_collector<CellType> > steerer_;
    };
}}}

#endif
