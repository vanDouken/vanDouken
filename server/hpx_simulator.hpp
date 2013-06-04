//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SERVER_SIMULATOR_HPP
#define HPX_SERVER_SIMULATOR_HPP

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>

#include <libgeodecomp/io/simpleinitializer.h>
#include <libgeodecomp/misc/floatcoord.h>
#include <libgeodecomp/misc/superset.h>
#include <libgeodecomp/misc/supermap.h>
#include <libgeodecomp/parallelization/distributedsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitions/recursivebisectionpartition.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitionmanager.h>

//#include <hpx_updategroup.hpp>

#include <vector>

namespace LibGeoDecomp { namespace HiParSimulator {
    
    template <typename CellType, typename Partition>
    struct hpx_updategroup;


    namespace server {

    // FIXME: code duplication! merge this with the defintion in hiparsimulator.h

    enum EventPoint {LOAD_BALANCE, END};
    typedef LibGeoDecomp::SuperSet<EventPoint> EventSet;
    typedef LibGeoDecomp::SuperMap<long, EventSet> EventMap;
    
    template <typename CellType, typename Partition>
    struct hpx_updategroup;

    inline std::string eventToStr(EventPoint event)
    {
        switch (event)
        {
            case LOAD_BALANCE:
                return "LOAD_BALANCING";
            case END:
                return "END";
            default:
                return "invalid";
        }
    }

    template <typename CellType, typename Partition>
    struct HPX_COMPONENT_EXPORT hpx_simulator
      : hpx::components::managed_component_base<
            hpx_simulator<CellType, Partition>
          , hpx::components::detail::this_type
          , hpx::traits::construct_with_back_ptr
      >
    {

        typedef hpx::lcos::local::spinlock mutex_type;

        mutex_type mtx;

        typedef 
            hpx::components::managed_component_base<
                hpx_simulator<CellType, Partition>
              , hpx::components::detail::this_type
              , hpx::traits::construct_with_back_ptr
            >
            base_type;

        typedef typename CellType::Topology topology;
        typedef LibGeoDecomp::GridBase<CellType, topology::DIMENSIONS> grid_type;
        typedef LibGeoDecomp::Initializer<CellType> initializer_type;

        typedef boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > writer_type;
        typedef LibGeoDecomp::SuperVector<writer_type> writer_vector;
        
        boost::shared_ptr<initializer_type> init_;

        typedef
            LibGeoDecomp::HiParSimulator::server::hpx_updategroup<CellType, Partition> 
            updategroup_server_type;
        typedef
            LibGeoDecomp::HiParSimulator::hpx_updategroup<CellType, Partition> 
            updategroup_type;

        static const int dim = topology::DIMENSIONS;

        hpx_simulator(
                hpx::components::managed_component<
                    hpx_simulator<CellType, Partition>
                > * back_ptr
            )
            : base_type(back_ptr)
        {}

        hpx_simulator(
                hpx::components::managed_component<
                    hpx_simulator<CellType, Partition>
                > *
                , std::size_t num_partitions
                , std::string const & name);

        typedef
            hpx::components::server::create_component_action2<
                hpx_simulator<CellType, Partition>
              , std::size_t
              , std::string
            >
            create_component_action;

        ~hpx_simulator();

        void init(boost::shared_ptr<LibGeoDecomp::Initializer<CellType> > ini);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, init, init_action);

        std::size_t num_partitions();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, num_partitions, num_partitions_action);

        boost::shared_ptr<initializer_type> getInitializer();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, getInitializer, getInitializer_action);

        void init_events();

        void handle_events();

        long current_nano_step();

        long time_to_next_event();

        long time_to_last_event();

        LibGeoDecomp::SuperVector<long> initial_weights(
            std::size_t items, std::size_t num_partitions);

        void registerWriter(boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > writer);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, registerWriter, registerWriter_action);

        void getGridFragment(const grid_type ** grid, const LibGeoDecomp::Region<dim> **valid_region);

        void step();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, step, step_action);
        
        unsigned getStep() const;

        void nano_step(long nano_steps);

        void run();
        
        std::vector<updategroup_type> updategroups();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, updategroups, updategroups_action);
        
        writer_vector writers();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, writers, writers_action);
        
        std::size_t connect_writer(hpx::naming::id_type const & id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, connect_writer, connect_writer_action);
        
        void disconnect_writer(std::size_t id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, disconnect_writer, disconnect_writer_action);
        
        void connect_steerer(hpx::id_type const & id);
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, connect_steerer, connect_steerer_action);

        void wait();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, wait, wait_action);

        void stop();
        HPX_DEFINE_COMPONENT_ACTION_TPL(hpx_simulator, stop, stop_action);
        
        std::size_t step_num;
        EventMap events;
        writer_vector writers_;
        hpx::promise<void> wait_promise;
        hpx::future<void> wait_future;
        std::string name_;
        std::vector<updategroup_type> update_groups;
        std::vector<hpx::lcos::future<void> > init_futures;
        std::vector<hpx::lcos::future<void> > register_writer_futures;
        std::vector<LibGeoDecomp::CoordBox<dim> > bboxes;
        std::vector<hpx::lcos::future<void> > step_futures;
        std::map<std::size_t, hpx::id_type> collectors_;
    };
}}}

#endif
