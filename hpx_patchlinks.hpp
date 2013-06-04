//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PATCHLINKS_HPP
#define HPX_PATCHLINKS_HPP

#include <hpx/hpx.hpp>

//#include <hpx_updategroup.hpp>

namespace LibGeoDecomp { namespace HiParSimulator {
    template <typename GridType, typename UpdateGroup>
    struct hpx_patchlink_accepter
        : PatchAccepter<GridType>
    {
        using PatchAccepter<GridType>::checkNanoStepPut;
        using PatchAccepter<GridType>::pushRequest;
        using PatchAccepter<GridType>::requestedNanoSteps;
        typedef hpx::lcos::local::spinlock mutex_type;


        hpx_patchlink_accepter(
            Region<GridType::DIM> const & region
          , std::size_t this_rank
          , UpdateGroup const & dest_group
          , mutex_type & m
        )
            : lastNanoStep(0)
            , stride(1)
            , region_(region)
            , this_rank_(this_rank)
            , dest_group_(dest_group)
            , sent_future(hpx::lcos::make_future())
            , mtx(m)
        {}

        void charge(long next, long last, long newStride)
        {
            lastNanoStep = last;
            stride = newStride;
            pushRequest(next);
        }
        
        typedef 
            SuperVector<typename GridType::CellType>
            buffer_type;

        virtual void put(
            GridType const & grid
          , Region<GridType::DIM> const & /*validRegion*/
          , long const & nanoStep
        )
        {
            if(!checkNanoStepPut(nanoStep))
                return;
            boost::shared_ptr<buffer_type> buffer;
            {
                mutex_type::scoped_lock lk(mtx);
                buffer.reset(new buffer_type(region_.size()));
                GridVecConv::gridToVector(grid, buffer.get(), region_);
            }

            hpx::wait(sent_future);
            sent_future = dest_group_.set_outer_ghostzone(
                this_rank_
              , buffer
              , nanoStep
            );

            long nextNanoStep = (requestedNanoSteps.min)() + stride;
            if((lastNanoStep == -1) ||
               (nextNanoStep < lastNanoStep))
            {
                requestedNanoSteps << nextNanoStep;
            }
            requestedNanoSteps.erase_min();
        }
        
        long lastNanoStep;
        long stride;
        Region<GridType::DIM> region_;
        std::size_t this_rank_;
        UpdateGroup dest_group_;
        hpx::lcos::future<void> sent_future;
        mutex_type & mtx;
    };

    template <typename GridType>
    struct hpx_patchlink_provider
        : PatchProvider<GridType>
    {
        typedef hpx::lcos::local::spinlock mutex_type;
        using PatchProvider<GridType>::checkNanoStepGet;
        using PatchProvider<GridType>::storedNanoSteps;


        hpx_patchlink_provider(
            Region<GridType::DIM> const & region
          , mutex_type & m
        )
            : lastNanoStep(0)
            , stride(1)
            , region_(region)
            , buffer_future_(buffer_promise_.get_future())
            , recv_future_(recv_promise_.get_future())
            , get_future_(hpx::lcos::make_future())
            , mtx(m)
        {
            BOOST_ASSERT(!buffer_promise_.is_ready());
        }

        void charge(long next, long last, long newStride)
        {
            lastNanoStep = last;
            stride = newStride;
            recv(next);
        }

        virtual void get(
            GridType * grid
          , Region<GridType::DIM> const &
          , long const & nanoStep
          , bool const & remove=true
        )
        {
            hpx::wait(get_future_);
            get_future_ = hpx::async(
                HPX_STD_BIND(
                    &hpx_patchlink_provider<GridType>::get_async
                  , this
                  , grid
                  , nanoStep
                )
            );
        }
        
        void get_async(
            GridType * grid
          , long const & nanoStep
        )
        {
            boost::shared_ptr<buffer_type> buf = buffer_future_.get();
            {
                mutex_type::scoped_lock lk(mtx);
                checkNanoStepGet(nanoStep);
                GridVecConv::vectorToGrid(*buf, grid, region_);
            }

            long nextNanoStep = (storedNanoSteps.min)() + stride;
            if ((lastNanoStep == -1) ||
                (nextNanoStep < lastNanoStep))
            {
                recv(nextNanoStep);
            }
            storedNanoSteps.erase_min();
        }

        void recv(long nanoStep)
        {
            storedNanoSteps << nanoStep;
            buffer_promise_.reset();
            buffer_future_ = buffer_promise_.get_future();
            recv_promise_.set_value(nanoStep);
        }
        
        typedef 
            SuperVector<typename GridType::CellType>
            buffer_type;

        void set_buffer(boost::shared_ptr<buffer_type> buffer, long nanoStep)
        {
            hpx::wait(recv_future_);
            /*
            long exp_nanoStep = recv_future_.get();
            BOOST_ASSERT(exp_nanoStep == nanoStep);
            */
            BOOST_ASSERT(!buffer_promise_.is_ready());
            recv_promise_.reset();
            recv_future_ = recv_promise_.get_future();
            buffer_promise_.set_value(buffer);
        }

        long lastNanoStep;
        long stride;
        Region<GridType::DIM> region_;
        hpx::lcos::promise<boost::shared_ptr<buffer_type> > buffer_promise_;
        hpx::lcos::future<boost::shared_ptr<buffer_type> > buffer_future_;
        hpx::lcos::promise<long> recv_promise_;
        hpx::lcos::future<long> recv_future_;
        hpx::lcos::future<void> get_future_;
        mutex_type & mtx;
    };
}}

#endif

