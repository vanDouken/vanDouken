//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SERVER_UPDATEGROUP_IMPL_HPP
#define HPX_SERVER_UPDATEGROUP_IMPL_HPP

#include <server/hpx_updategroup.hpp>
#include <server/collector.hpp>
#include <hpx_updategroup.hpp>
#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/parallelization/hiparsimulator/parallelwriteradapter.h>
#include <libgeodecomp/parallelization/hiparsimulator/steereradapter.h>
#include <boost/serialization/shared_ptr.hpp>

#include <model_test/particle.h>
#include "canvascell.h"
#include "forceprimitives.h"


namespace LibGeoDecomp { namespace HiParSimulator { namespace server {
    namespace detail
    {
        template <typename CellType>
        struct HPX_COMPONENT_EXPORT distributor
            : ParallelWriter<CellType>
            , boost::noncopyable
        {
            typedef hpx::lcos::local::spinlock mutex_type;
            typedef typename ParallelWriter<CellType>::GridType GridType;
            typedef typename ParallelWriter<CellType>::RegionType RegionType;
            typedef typename ParallelWriter<CellType>::CoordType CoordType;

            typedef typename CellType::Topology topology;
            static const int dim = topology::DIMENSIONS;
        
            typedef 
                Particles
                buffer_type;

            distributor(Coord<dim> const & dim, mutex_type & m)
                : ParallelWriter<CanvasCell>("distributor")
                , mtx(m)
                //, sent_future(hpx::lcos::make_future())
            {}

            void initialized(GridType const & grid, RegionType const & region)
            {
            }
            void stepFinished(
                GridType const & grid
              , RegionType const & region
              , CoordType const & globalDimensions
              , unsigned step
              , WriterEvent event
              , bool lastCall)
            {
                boost::shared_ptr<buffer_type> buffer(new buffer_type());
                buffer->resize(region.size());
                {
                    mutex_type::scoped_lock lk(mtx);
                    
                    int cursor = 0;
                    for (Region<2>::Iterator i = region.begin();
                         i != region.end();
                         ++i) {
                        const LibGeoDecomp::CanvasCell& cell = grid.at(*i);
                        std::size_t nextSize = cursor + cell.numParticles;
                        buffer->resize(nextSize);
                        if (buffer->size() < nextSize) {
                            buffer->resize(nextSize);
                        }
                        for (int j = 0; j < cell.numParticles; ++j) {
                            particleToParticle(cursor + j, cell.particles[j], *buffer);
                        }
                        cursor += cell.numParticles;
                    }
                }
        
                //std::cout << "distributor: " << step << "\n";

                stepFinished_async(buffer, step);
            }
            
            void particleToParticle(int i, const LibGeoDecomp::CanvasCell::Particle& particle, Particles & particles)
            {
                int halfWidth = 320/2;
                int halfHeight = 240/2;
                float posX = (0.20 / 1.5) * (particle.pos[0] - halfWidth);
                float posY = (0.20 / 1.5) * (particle.pos[1] - halfHeight);
                        
                float depthOffset = 0;
                if (particle.lifetime < FADE_IN_OUT) {
                    depthOffset = FADE_IN_OUT - particle.lifetime;
                }

                if ((DEFAULT_PARTICLE_LIFETIME - particle.lifetime) < FADE_IN_OUT) {
                    depthOffset = particle.lifetime - DEFAULT_PARTICLE_LIFETIME + FADE_IN_OUT;
                }

                float posZ = -100 + particle.pos[2] - 0.01 * depthOffset;;
                float angle = ForcePrimitives::angle(particle.vel[0], particle.vel[1]);
                particles.setParticle(i, posX, posY, posZ, angle, particle.color);
                //return Particle(posX, posY, posZ, angle, particle.color);
            }

            typedef
                boost::shared_ptr<buffer_type>
                saved_buffer_type;
            typedef
                std::vector<saved_buffer_type>
                buffer_vector_type;
            typedef
                std::map<std::size_t, buffer_vector_type>
                buffer_map_type;
            typedef
                std::map<std::size_t, buffer_map_type>
                id_map_type;

            id_map_type id_map;

            void stepFinished_async(boost::shared_ptr<buffer_type> buffer, std::size_t step)
            {
                mutex_type::scoped_lock lk(collector_mtx);
                BOOST_FOREACH(std::size_t & id, collectors)
                {
                    mutex_type::scoped_lock lk(buffer_mtx);
                    typename id_map_type::iterator it = id_map.find(id);
                    if(it == id_map.end())
                    {
                        it = id_map.insert(it, std::make_pair(id, buffer_map_type()));
                    }

                    typename buffer_map_type::iterator jt = it->second.find(step);
                    if(jt == it->second.end())
                    {
                        jt = it->second.insert(
                            jt, std::make_pair(step, buffer_vector_type()));
                    }

                    jt->second.push_back(buffer);
                }
                //std::cout << "step finished " << step << "\n";
            }

            buffer_vector_type collect_finished(std::size_t id)
            {
                typename id_map_type::iterator it;
                typename buffer_map_type::iterator jt;
                buffer_vector_type res;
                {
                    mutex_type::scoped_lock lk(buffer_mtx);
                    //std::cout << "collect_finished: " << id << " " << step << "\n";

                    it = id_map.find(id);
                    //BOOST_ASSERT(it != id_map.end());
                    if(it == id_map.end())
                    {
                        //std::cout << "collect_finished: couldn't find id " << id << "\n";
                        return buffer_vector_type();
                    }
                    jt = it->second.begin();
                    //BOOST_ASSERT(jt != it->second.end());
                    if(jt == it->second.end())
                    {
                        /*
                        std::cout
                            << " collect_finished: couldn't find step " << step
                            << " collect_finished: " << it->second.size()
                            << "\n";

                        std::cout << "collect_finished: steps completed:\n";
                        for(auto & p : it->second)
                        {
                            std::cout << "    " << p.first << "\n";
                        }
                            */
                        return buffer_vector_type();
                    }
                    std::swap(res, jt->second);
                    it->second.erase(jt);
                    if(it->second.empty())
                    {
                        id_map.erase(it);
                    }
                    if(it->second.size() > 10)
                    {
                        //std::cout << "Skipping frames ...\n";
                        it->second.clear();
                    }
                }
                return res;
            }

            void allDone(GridType const & grid, RegionType const & region)
            {
            }

            void connect_writer(std::size_t id)
            {
                mutex_type::scoped_lock lk(collector_mtx);
                collectors.push_back(id);
            }

            void disconnect_writer(std::size_t id)
            {
                mutex_type::scoped_lock lk(collector_mtx);
                std::vector<std::size_t>::iterator
                    it = std::find(collectors.begin(), collectors.end(), id);
                if(it != collectors.end())
                {
                    collectors.erase(it);
                }
            }

            mutex_type collector_mtx;
            mutex_type buffer_mtx;
            mutex_type & mtx;

            //hpx::lcos::future<void> sent_future;
            std::vector<std::size_t> collectors;
        };

        template <typename CellType>
        struct HPX_COMPONENT_EXPORT steerer_collector
            : LibGeoDecomp::Steerer<CellType>
        {
            typedef hpx::lcos::local::spinlock mutex_type;

            typedef typename LibGeoDecomp::Steerer<CellType>::GridType GridType;

            typedef typename CellType::Topology topology;
            static const int dim = topology::DIMENSIONS;

            steerer_collector()
                : LibGeoDecomp::Steerer<CellType>(1)
            {}
        
            typedef 
                LibGeoDecomp::SuperVector<CellType>
                buffer_type;

            typedef
                boost::shared_ptr<LibGeoDecomp::Steerer<CellType> > 
                steerer_type;
            typedef
                hpx::future<steerer_type> 
                steer_future_type;

            steerer_type erase_id(steer_future_type f, hpx::id_type id)
            {
                mutex_type::scoped_lock lk(this->mtx);
                this->currently_steering.erase(id);
                return f.get();
            }

            void nextStep(
                GridType * grid
              , Region<dim> const & validRegion
              , const unsigned & step
            )
            {
                /*
                if(!steer_futures.empty())
                {
                    HPX_STD_TUPLE<int, steer_future_type> res = hpx::wait_any(steer_futures);
                    steerer_type s = HPX_STD_GET(1, res).get();
                    if(s)
                    {
                        s->nextStep(grid, validRegion, step);
                    }
                    steer_futures.erase(steer_futures.begin() + HPX_STD_GET(0, res));
                }
                */
                typedef
                    LibGeoDecomp::HiParSimulator::server::collector<CellType>
                    server_type;
                mutex_type::scoped_lock lk(mtx);

                for(typename std::vector<steer_future_type>::iterator i = steer_futures.begin(); i != steer_futures.end();)
                {
                    if(i->is_ready())
                    {
                        steerer_type s = i->get();
                        if(s)
                        {
                            s->nextStep(grid, validRegion, step);
                        }
                        i = steer_futures.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }

                BOOST_FOREACH(hpx::id_type id, steerer)
                {
                    bool found = currently_steering.find(id) != currently_steering.end();
                    if(!found)
                    {
                        currently_steering.insert(id);
                        {
                            hpx::util::unlock_the_lock<mutex_type::scoped_lock> ulk(lk);
                            steer_future_type f = 
                                hpx::async<typename server_type::steer_action>(id, validRegion);

                            steer_futures.push_back(
                                f.then(
                                    HPX_STD_BIND(
                                        &steerer_collector::erase_id
                                      , this
                                      , HPX_STD_PLACEHOLDERS::_1
                                      , id
                                    )
                                )
                            );
                        }
                    }
                }
            }

            void connect_steerer(hpx::id_type id)
            {
                mutex_type::scoped_lock lk(mtx);
                steerer.push_back(id);
            }

            mutex_type mtx;
            std::vector<steer_future_type> steer_futures;
            std::set<hpx::id_type> currently_steering;
            std::vector<hpx::id_type> steerer;
        };
    }
    
    template <typename CellType, typename Partition>
    /*CoordBox<hpx_updategroup<CellType, Partition>::dim>*/
    void hpx_updategroup<CellType, Partition>::init(
        std::size_t rank
      , hpx::naming::id_type server_id
    )
    {
        sim_ = server_id;
        boost::shared_ptr<initializer_type> init = sim_.getInitializer().get();
        this_rank = rank;
        std::size_t num_partitions = sim_.num_partitions().get();

        CoordBox<dim> box = init->gridBox();

        bboxes.resize(num_partitions);

        BOOST_ASSERT(rank < num_partitions);

        part_man->resetRegions(
            box
          , new Partition(
                box.origin
              , box.dimensions
              , 0
              , initial_weights(box.dimensions.prod(), num_partitions)
            )
          , rank
          , ghostzone_width
        );
        bboxes[rank] = part_man->ownRegion().boundingBox();

        typedef
            LibGeoDecomp::HiParSimulator::hpx_updategroup<CellType, Partition> 
            updategroup_type;

        std::vector<updategroup_type> update_groups = sim_.updategroups().get();
        ////////////////////////////////////////////////////////////////////////
        // All gather for bounding boxes
        hpx::lcos::future<void> rgg = reset_ghostzone_gate.get_future(num_partitions);
        //hpx::lcos::future<void> rgg = reset_ghostzone_gate.get_future(0);
        init_promise.set_value();
        std::size_t i = 0;
        BOOST_FOREACH(updategroup_type const & ug, update_groups)
        {
            BOOST_ASSERT(ug.get_gid());
            if(i != this_rank)
            {
                ug.reset_ghostzone(this_rank, bboxes[this_rank]);
            }
            ++i;
        }

        reset_ghostzone_gate.set(rank);
        //reset_ghostzone_gate.synchronize(1);
        rgg.get();
        part_man->resetGhostZones(bboxes);
        ////////////////////////////////////////////////////////////////////////
        
        long first_sync_point =
            init->startStep() * CellType::nanoSteps() + ghostzone_width;

        patch_accepter_vec patchaccepters_ghost;
        patch_accepter_vec patchaccepters_inner;

        LibGeoDecomp::SuperVector<boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > >
            writers = sim_.writers().get();
        patchaccepters_ghost.reserve(writers.size() + 1);
        patchaccepters_inner.reserve(writers.size() + 1);


        ////////////////////////////////////////////////////////////////////////
        // Create Writers
        distributor_ = boost::make_shared<detail::distributor<CellType> >(init->gridDimensions(), grid_mtx);
        typedef ParallelWriterAdapter<grid_type, CellType, hpx_updategroup> parallel_writer_adapter;
        patchacceptor_ptr
            distributor_adapter_inner(
                new parallel_writer_adapter(
                    this
                  , distributor_
                  , init->startStep()
                  , init->maxSteps()
                  , init->gridDimensions()
                  , true
                )
            );
        patchaccepters_inner.push_back(distributor_adapter_inner);
        patchacceptor_ptr
            distributor_adapter_ghost(
                new parallel_writer_adapter(
                    this
                  , distributor_
                  , init->startStep()
                  , init->maxSteps()
                  , init->gridDimensions()
                  , false
                )
            );
        patchaccepters_ghost.push_back(distributor_adapter_ghost);

        BOOST_FOREACH(boost::shared_ptr<LibGeoDecomp::ParallelWriter<CellType> > writer, writers)
        {
            patchacceptor_ptr adapterGhost(
                new parallel_writer_adapter(
                    this
                  , writer
                  , init->startStep()
                  , init->maxSteps()
                  , init->gridDimensions()
                  , false
                )
            );
            patchacceptor_ptr adapterInner(
                new parallel_writer_adapter(
                    this
                  , writer
                  , init->startStep()
                  , init->maxSteps()
                  , init->gridDimensions()
                  , true
                )
            );
            patchaccepters_ghost.push_back(adapterGhost);
            patchaccepters_inner.push_back(adapterInner);
        }
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Setup Patch Acceptors for inner ghost zones
        typedef std::pair<int, SuperVector<Region<dim> > > region_pair_type;
        
        BOOST_FOREACH(
            region_pair_type const & region_pair
          , part_man->getInnerGhostZoneFragments()
        )
        {
            if(!region_pair.second.back().empty())
            {
                typedef
                    hpx_patchlink_accepter<grid_type, updategroup_type>
                    patchlink_accepter_type;
                boost::shared_ptr<
                    patchlink_accepter_type
                > link(
                    new patchlink_accepter_type(
                        region_pair.second.back()
                      , this_rank
                      , update_groups[region_pair.first]
                      , grid_mtx
                    )
                );
                patchaccepters_ghost.push_back(link);
                link->charge(first_sync_point, -1, ghostzone_width);
            }
        }
        ////////////////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////////////////
        // Setup Vanilla Stepper
        stepper.reset(
            new stepper_type(
                part_man
              , init.get()
              , patchaccepters_ghost
              , patchaccepters_inner
            )
        );
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Setup Patch Providers for outer ghost zones
        BOOST_FOREACH(
            region_pair_type const & region_pair
          , part_man->getOuterGhostZoneFragments()
        )
        {
            if(!region_pair.second.back().empty())
            {
                mutex_type::scoped_lock lk(patchlink_provider_mtx);
                boost::shared_ptr<
                    patchlink_provider_type
                > link(
                    new patchlink_provider_type(
                        region_pair.second.back()
                      , grid_mtx
                    )
                );
                patchlink_provider_map.insert(
                    std::pair<std::size_t, boost::shared_ptr<patchlink_provider_type> >(
                        region_pair.first
                      , link
                    )
                );
                stepper->addPatchProvider(link, Stepper<CellType>::GHOST);
                link->charge(first_sync_point, -1, ghostzone_width);
            }
        }
        init_patchlink_provider_trigger.set();
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Setup PatchProviders (aka stepper)
        steerer_.reset(new detail::steerer_collector<CellType>);

        typedef
            LibGeoDecomp::HiParSimulator::SteererAdapter<grid_type, CellType>
            SteererAdapterType;
        patchprovider_ptr adapterGhost(
                new SteererAdapterType(steerer_));
        patchprovider_ptr adapterInnerSet(
                new SteererAdapterType(steerer_));

        stepper->addPatchProvider(adapterGhost, Stepper<CellType>::GHOST);
        stepper->addPatchProvider(adapterInnerSet, Stepper<CellType>::INNER_SET);
    }

    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::connect_writer(std::size_t id)
    {
        //BOOST_ASSERT(id);
        distributor_->connect_writer(id);
    }

    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::disconnect_writer(std::size_t id)
    {
        //BOOST_ASSERT(id);
        distributor_->disconnect_writer(id);
    }

    template <typename CellType, typename Partition>
    typename hpx_updategroup<CellType, Partition>::buffer_vector_type
    hpx_updategroup<CellType, Partition>::collect_finished(std::size_t id)
    {
        return distributor_->collect_finished(id);
    }
    

    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::register_writer(boost::shared_ptr<ParallelWriter<CellType> > writer)
    {
        BOOST_ASSERT(false);
    }
    
    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::update(unsigned nano_steps)
    {
        stepper->update(
            nano_steps
            /*
          , HPX_STD_BIND(
                &detail::distributor<CellType>::step_finished
              , distributor_.get()
              //, HPX_STD_PLACEHOLDERS::_1
            )
            */
        );
        //std::cout << "step done ...\n";
    }
    
    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::notify_initialized()
    {
        // TODO: notify writers
        BOOST_ASSERT(false);
    }
    
    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::notify_done()
    {
        // TODO: notify writers
        BOOST_ASSERT(false);
    }
   
    template <typename CellType, typename Partition>
    void hpx_updategroup<CellType, Partition>::connect_steerer(hpx::id_type id)
    {
        steerer_->connect_steerer(id);
    }
}}}

#endif
