//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UPDATEGROUP_HPP
#define HPX_UPDATEGROUP_HPP

#include <server/hpx_updategroup.hpp>

namespace LibGeoDecomp { namespace HiParSimulator {

    template <typename CellType, typename Partition>
    struct hpx_updategroup
        : hpx::components::client_base<
            hpx_updategroup<CellType, Partition>
          , hpx::components::stub_base<server::hpx_updategroup<CellType, Partition> >
        >
    {
        typedef server::hpx_updategroup<CellType, Partition> server_type;
        typedef hpx::components::stub_base<server_type> stub_type;
        typedef hpx_updategroup<CellType, Partition> this_type;
        typedef hpx::components::client_base<this_type, stub_type> base_type;

        typedef typename CellType::Topology topology;
        static const int dim = topology::DIMENSIONS;
        
        hpx_updategroup()
        {}

        hpx_updategroup(hpx::naming::id_type this_id)
            : base_type(this_id)
        {}

        void reset_ghostzone(std::size_t rank, CoordBox<dim> const & bbox) const
        {
            hpx::apply<typename server_type::reset_ghostzone_action>(
                this->get_gid(), rank, bbox);
        }

        hpx::lcos::future<void> set_outer_ghostzone(
            std::size_t src_rank, boost::shared_ptr<SuperVector<CellType> > buffer
          , long nanoStep
        ) const
        {
            return
                hpx::async<typename server_type::set_outer_ghostzone_action>(
                    this->get_gid(), src_rank, buffer, nanoStep);
        }

        //hpx::lcos::future<CoordBox<dim> >
        hpx::lcos::future<void> init(
            std::size_t rank
          , hpx::naming::id_type simulator_id
        ) const
        {
            return
                hpx::async<typename server_type::init_action>(
                    this->get_gid(), rank, simulator_id);
        }
        
        hpx::lcos::future<void> register_writer(boost::shared_ptr<ParallelWriter<CellType> > writer) const
        {
            return
                hpx::async<typename server_type::register_writer_action>(
                    this->get_gid(), writer);
        }
        
        hpx::lcos::future<void> update(unsigned nanoStep) const
        {
            return
                hpx::async<typename server_type::update_action>(
                    this->get_gid(), nanoStep);
        }
        
        hpx::lcos::future<void> notify_initialized() const
        {
            return
                hpx::async<typename server_type::notify_initialized_action>(
                    this->get_gid());
        }
        
        hpx::lcos::future<void> notify_done() const
        {
            return
                hpx::async<typename server_type::notify_done_action>(
                    this->get_gid());
        }
        
        hpx::lcos::future<std::pair<int, int> > current_step() const
        {
            return
                hpx::async<typename server_type::current_step_action>(
                    this->get_gid());
        }

        hpx::lcos::future<void> connect_writer(std::size_t id) const
        {
            return
                hpx::async<typename server_type::connect_writer_action>(
                    this->get_gid(), id);
        }
        
        hpx::lcos::future<void> disconnect_writer(std::size_t id) const
        {
            return
                hpx::async<typename server_type::disconnect_writer_action>(
                    this->get_gid(), id);
        }
        typedef 
            Particles
            buffer_type;
        typedef
            boost::shared_ptr<buffer_type>
            saved_buffer_type;
        typedef
            std::vector<saved_buffer_type>
            buffer_vector_type;
        hpx::future<buffer_vector_type> collect_finished(std::size_t id) const
        {
            return
                hpx::async<typename server_type::collect_finished_action>(
                    this->get_gid(), id);
        }

        CoordBox<dim> bbox()
        {
            typename server_type::bbox_action act;
            return act(this->get_gid());
        }

        hpx::lcos::future<void> connect_steerer(hpx::id_type id) const
        {
            return
                hpx::async<typename server_type::connect_steerer_action>(
                    this->get_gid(), id);
        }
    };
}}

#endif
