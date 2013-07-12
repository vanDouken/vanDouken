//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SIMULATOR_HPP
#define HPX_SIMULATOR_HPP

#include <libgeodecomp/misc/region.h>
#include <server/hpx_simulator.hpp>
//#include <hpx_updategroup.hpp>

namespace LibGeoDecomp { namespace HiParSimulator {
    
    template <typename CellType, typename Partition>
    struct hpx_updategroup;

    template <typename CellType, typename Partition>
    struct hpx_simulator
        : hpx::components::client_base<
            hpx_simulator<CellType, Partition>
          , hpx::components::stub_base<LibGeoDecomp::HiParSimulator::server::hpx_simulator<CellType, Partition> >
        >
    {
        typedef server::hpx_simulator<CellType, Partition> server_type;
        typedef hpx::components::stub_base<server_type> stub_type;
        typedef hpx_simulator<CellType, Partition> this_type;
        typedef hpx::components::client_base<this_type, stub_type> base_type;

        typedef boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > writer_type;
        typedef LibGeoDecomp::SuperVector<writer_type> writer_vector;
        
        typedef LibGeoDecomp::Initializer<CellType> initializer_type;
        
        typedef
            LibGeoDecomp::HiParSimulator::hpx_updategroup<CellType, Partition> 
            updategroup_type;

        typedef typename CellType::Topology topology;
        static const int dim = topology::DIMENSIONS;
        
        hpx_simulator()
        {}

        hpx_simulator(hpx::naming::id_type this_id)
            : base_type(this_id)
        {}

        void init(boost::shared_ptr<LibGeoDecomp::Initializer<CellType> > ini)
        {
            hpx::async<typename server_type::init_action>(
                this->get_gid(), ini).get();
        }

        hpx::lcos::future<std::size_t> num_partitions()
        {
            return
                hpx::async<typename server_type::num_partitions_action>(
                    this->get_gid());
        }

        hpx::lcos::future<boost::shared_ptr<initializer_type> > getInitializer()
        {
            return
                hpx::async<typename server_type::getInitializer_action>(
                    this->get_gid());
        }

        hpx::lcos::future<std::vector<updategroup_type> > updategroups()
        {
            return
                hpx::async<typename server_type::updategroups_action>(
                    this->get_gid());
        }

        hpx::lcos::future<void> registerWriter(writer_type w)
        {
            return
                hpx::async<typename server_type::registerWriter_action>(
                    this->get_gid(), w);
        }

        void step()
        {
            //return
            typename server_type::step_action act;
            act(this->get_gid());
        }
        
        hpx::lcos::future<writer_vector> writers()
        {
            return
                hpx::async<typename server_type::writers_action>(
                    this->get_gid());
        }
        
        std::size_t connect_writer(hpx::naming::id_type const & id)
        {
            return hpx::async<typename server_type::connect_writer_action>(
                this->get_gid(), id).get();
        }
        
        std::size_t connect_steerer(hpx::naming::id_type const & id)
        {
            hpx::async<typename server_type::connect_steerer_action>(
                this->get_gid(), id).get();
            return 0;
        }
        
        void disconnect_writer(std::size_t id)
        {
            hpx::async<typename server_type::disconnect_writer_action>(
                this->get_gid(), id).get();
        }

        void wait()
        {
            typename server_type::wait_action act;
            act(this->get_gid());
        }

        void stop()
        {
            typename server_type::stop_action act;
            act(this->get_gid());
        }
    };
}}

#endif
