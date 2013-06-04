
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SERVER_SIMULATOR_IMPL_HPP
#define HPX_SERVER_SIMULATOR_IMPL_HPP

#include <hpx/hpx_fwd.hpp>
#include <hpx/hpx.hpp>
#include <hpx/util/static.hpp>
#include <hpx/components/distributing_factory/distributing_factory.hpp>

#include <server/hpx_simulator.hpp>
#include <serialize/supervector.hpp>

#include <boost/range/algorithm.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <hpx_updategroup.hpp>

namespace LibGeoDecomp { namespace HiParSimulator { namespace server {

    template <typename CellType, typename Partition>
    hpx_simulator<CellType, Partition>::hpx_simulator(
        hpx::components::managed_component<
            hpx_simulator<CellType, Partition>
        > * back_ptr
      , std::size_t num_partitions, std::string const & name)
        : base_type(back_ptr)
        , wait_future(wait_promise.get_future())
        , name_(name)
        , update_groups(num_partitions)
    {
        hpx::agas::register_name(name_, this->get_gid());
    }
            
    template <typename CellType, typename Partition>
    hpx_simulator<CellType, Partition>::~hpx_simulator()
    {
        hpx::wait(init_futures);
        hpx::naming::id_type id = this->get_gid();
        hpx::agas::unregister_name(name_, id);
    }

    template <typename CellType, typename Partition>
    boost::shared_ptr<LibGeoDecomp::Initializer<CellType> > hpx_simulator<CellType, Partition>::getInitializer()
    {
        mutex_type::scoped_lock lk(mtx);
        return init_;
    }

    template <typename CellType, typename Partition>
    std::size_t hpx_simulator<CellType, Partition>::num_partitions()
    {
        mutex_type::scoped_lock lk(mtx);
        return update_groups.size();
    }
        
    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::init(boost::shared_ptr<LibGeoDecomp::Initializer<CellType> > ini)
    {
        init_ = ini;
        
        LibGeoDecomp::CoordBox<dim> box = ini->gridBox();

        std::cout << box << "\n";

        hpx::components::distributing_factory factory;
        factory.create(hpx::find_here());

        hpx::components::component_type
            type = hpx::components::get_component_type<updategroup_server_type>();

        boost::copy(
            hpx::util::locality_results(factory.create_components(type, update_groups.size()))
          , update_groups.begin()
        );

        std::cout << "init simulator ...\n";
        init_futures.reserve(update_groups.size());
        std::size_t i = 0;
        BOOST_FOREACH(updategroup_type const & ug, update_groups)
        {
            init_futures.push_back(
                ug.init(i, this->get_gid())
            );
            ++i;
        }
        hpx::wait(init_futures);
        std::cout << "init simulator ... done\n";

        init_events();
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::init_events()
    {
        events.clear();
        long last_nano_step = init_->maxSteps() * CellType::nanoSteps();
        events[last_nano_step] << END;
        
        // TODO: insert load balancing event
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::handle_events()
    {
        if(current_nano_step() > events.begin()->first)
        {
            throw std::logic_error("stale event found, should have been handled previously");
        }
        if(current_nano_step() < events.begin()->first)
        {
            // Don't need to handle future events now.
            return;
        }

        const EventSet& cur_events = events.begin()->second;
        for(EventSet::const_iterator i = cur_events.begin(); i != cur_events.end(); ++i)
        {
            if(*i == LOAD_BALANCE)
            {
                // TODO: handle load balancing event
                // TODO: insert next load balancing event
            }
        }
        events.erase(events.begin());
    }

    template <typename CellType, typename Partition>
    long hpx_simulator<CellType, Partition>::current_nano_step()
    {
        std::pair<int, int> now = update_groups[0].current_step().get();
        
        return long(now.first) * CellType::nanoSteps() + now.second;
        return 0;
    }

    template <typename CellType, typename Partition>
    long hpx_simulator<CellType, Partition>::time_to_next_event()
    {
        if(events.begin()->first == -1)
        {
            return current_nano_step();
        }
        else
        {
            return events.begin()->first - current_nano_step();
        }
    }

    template <typename CellType, typename Partition>
    long hpx_simulator<CellType, Partition>::time_to_last_event()
    {
        return events.rbegin()->first - current_nano_step();
    }

    template <typename CellType, typename Partition>
    LibGeoDecomp::SuperVector<long> hpx_simulator<CellType, Partition>::initial_weights(std::size_t items, std::size_t num_partitions)
    {
        LibGeoDecomp::SuperVector<long> res(num_partitions);
        long last_pos = 0;
        for(std::size_t i = 0; i < num_partitions; ++i)
        {
            long current_pos = items * (i + 1) / num_partitions;
            res[i] = current_pos - last_pos;
            last_pos = current_pos;
        }

        return res;
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::registerWriter(boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > writer)
    {
        mutex_type::scoped_lock lk(mtx);
        writers_.push_back(writer);
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::getGridFragment(const grid_type ** grid, const LibGeoDecomp::Region<dim> **valid_region)
    {
        BOOST_ASSERT(false);
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::step()
    {
        std::size_t step = 0;
        {
            mutex_type::scoped_lock lk(mtx);
            nano_step(CellType::nanoSteps());
            step = getStep() * CellType::nanoSteps();
        }
        /*
        std::vector<hpx::future<void> > futures;
        futures.reserve(collectors_.size());
        */
        typedef std::pair<std::size_t, hpx::id_type> pair_type;
        BOOST_FOREACH(pair_type const & id, collectors_)
        {
            //futures.push_back(
                hpx::apply<
                    typename server::collector<CellType>::step_finished_action>(
                        id.second);
                //);
        }
        //hpx::wait(futures);
        //std::cout << "simulator: " << getStep() * CellType::nanoSteps() << " " << step << "\n";
    }

    template <typename CellType, typename Partition>
    unsigned hpx_simulator<CellType, Partition>::getStep() const
    {
        unsigned step = init_->startStep();
        for(std::size_t i = 0; i < update_groups.size(); ++i)
        {
            if(init_futures[i].is_ready())
            {
                unsigned cur_step = update_groups[i].current_step().get().first;
                step = (std::max)(cur_step, step);
            }
        }
        return step;
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::nano_step(long nano_steps)
    {
        hpx::wait(init_futures);
        while(nano_steps > 0)
        {
            long hop = (std::min)(nano_steps, time_to_next_event());
            std::vector<hpx::lcos::future<void> > update_futures;
            update_futures.reserve(update_groups.size());
            BOOST_FOREACH(updategroup_type const & ug, update_groups)
            {
                update_futures.push_back(
                    ug.update(hop)
                );
            }
            hpx::wait(update_futures);
            handle_events();
            nano_steps -= hop;
        }
    }

    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::run()
    {
        hpx::wait(init_futures);

        {
            std::vector<hpx::lcos::future<void> > update_futures;
            update_futures.reserve(update_groups.size());
            BOOST_FOREACH(updategroup_type const & ug, update_groups)
            {
                update_futures.push_back(
                    ug.notify_initialized()
                );
            }
            hpx::wait(update_futures);
        }

        nano_step(time_to_last_event());
    }
    
    template <typename CellType, typename Partition>
    std::vector<LibGeoDecomp::HiParSimulator::hpx_updategroup<CellType, Partition> > hpx_simulator<CellType, Partition>::updategroups()
    {
        mutex_type::scoped_lock lk(mtx);
        return update_groups;
    }

    template <typename CellType, typename Partition>
    LibGeoDecomp::SuperVector<boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > >
    hpx_simulator<CellType, Partition>::writers()
    {
        mutex_type::scoped_lock lk(mtx);
        return writers_;
    }
    
    template <typename CellType, typename Partition>
    std::size_t hpx_simulator<CellType, Partition>::connect_writer(hpx::naming::id_type const & id)
    {
        mutex_type::scoped_lock lk(mtx);
        hpx::util::static_<std::size_t> counter(0);
        hpx::wait(init_futures);
        std::size_t count = counter.get()++;
        {
            std::cout << "connect writer ...\n";
            std::vector<hpx::lcos::future<void> > connect_futures;
            BOOST_FOREACH(updategroup_type const & ug, update_groups)
            {
                connect_futures.push_back(
                    ug.connect_writer(count)
                );
            }
            collectors_.insert(std::pair<std::size_t, hpx::id_type>(count, id));
            hpx::wait(connect_futures);
            std::cout << "connect writer ... done\n";
        }
        return count;
    }
    
    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::disconnect_writer(std::size_t id)
    {
        mutex_type::scoped_lock lk(mtx);
        std::vector<hpx::lcos::future<void> > connect_futures;

        std::map<std::size_t, hpx::id_type>::iterator it = collectors_.find(id);
        BOOST_FOREACH(updategroup_type const & ug, update_groups)
        {
            connect_futures.push_back(
                ug.disconnect_writer(id)
            );
        }
        if(it != collectors_.end())
        {
            collectors_.erase(it);
        }
        hpx::wait(connect_futures);
    }
    
    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::connect_steerer(hpx::id_type const & id)
    {
        std::vector<hpx::lcos::future<void> > connect_futures;
        BOOST_FOREACH(updategroup_type const & ug, update_groups)
        {
            connect_futures.push_back(
                ug.connect_steerer(id)
            );
        }
        hpx::wait(connect_futures);
    }
    
    template <typename CellType, typename Partition>
    HPX_COMPONENT_EXPORT void hpx_simulator<CellType, Partition>::wait()
    {
        hpx::wait(wait_future);
    }
    
    template <typename CellType, typename Partition>
    void hpx_simulator<CellType, Partition>::stop()
    {
        wait_promise.set_value();
    }

}}}

#endif
