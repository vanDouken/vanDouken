
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <flowingcanvas_updategroup.hpp>
#include <flowingcanvas_simulator.hpp>
#include <flowingcanvas_collector.hpp>
#include <flowingcanvas_gui.hpp>

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QTimer>
#include <QApplication>
#include <QThreadPool>
#else
#include <hpiif.hpp>
#include <android/log.h>
#endif
#include <model_test/particlewidget.h>

#include <serialize/coord.hpp>

namespace server {
    void start_gui(
        flowingcanvas_gui * gui
      , hpx::promise<ParticleWidget/*FlowWidget*/ *> flow_promise
      , hpx::promise<void> finished_promise
      )
    {
#if !defined(__ANDROID__) || !defined(ANDROID)
        QCoreApplication *app = QApplication::instance();
        ParticleWidget *flow = new ParticleWidget(*gui, LibGeoDecomp::Coord<2>(320, 240));

        QTimer *timerFlow = new QTimer(flow);

        QObject::connect(timerFlow, SIGNAL(timeout()), flow, SLOT(updateGL()));
        /*
        QObject::connect(flow,     SIGNAL(forceRecorded(QVector2D, QVector2D)),
                         gui ,    SLOT(addForce(QVector2D, QVector2D))); 
        QObject::connect(flow,     SIGNAL(reset()),
                         gui ,    SLOT(resetNonVirt())); 
        */

        QObject::connect(app,      SIGNAL(lastWindowClosed()),  timerFlow, SLOT(stop()));

        timerFlow->start(10);
        flow->resize(1000, 500);
        flow->show();
        flow_promise.set_value(flow);
        app->exec();
        std::cout << "Window closed\n";
        finished_promise.set_value();
#else
#endif
    }

    flowingcanvas_gui::flowingcanvas_gui()
        : step_finished_future(hpx::lcos::make_ready_future())
        , steps_finished(0)
    {
        LOG("flowingcanvas_gui construction...\n");
#if !defined(__ANDROID__) || !defined(ANDROID)
        hpx::util::io_service_pool *main_pool =
            hpx::get_runtime().get_thread_pool("main_pool");

        reset_ = false;
        {
            //hpx::promise<FlowWidget *> flow_promise;
            hpx::promise<ParticleWidget *> flow_promise;
            hpx::promise<void> finished_promise;
            main_pool->get_io_service().post(

                HPX_STD_BIND(
                    &start_gui
                  , this
                  , flow_promise
                  , finished_promise
                )
            );
            flow_future = flow_promise.get_future();
            finished_future = finished_promise.get_future();
        }
#else
        ParticleWidget *flow = new ParticleWidget(*this, LibGeoDecomp::Coord<2>(320, 240));
        hpiif().flow = flow;
        flow_future = hpx::lcos::make_ready_future(flow);
        hpiif().finished_promise = boost::make_shared<hpx::lcos::promise<void> >();
        finished_future = hpiif().finished_promise->get_future();
#endif
        LOG("flowingcanvas_gui construction completed ...\n");
    }

    void flowingcanvas_gui::start(std::string const & name, bool fake_cam)
    {
        hpx::naming::id_type sim_id;
        hpx::agas::resolve_name(name, sim_id);
        sim = sim_id;

        BOOST_ASSERT(sim_id);
        {
            //mutex_type::scoped_lock lk(mtx);
            LOG("flowingcanvas_gui start...\n");
            BOOST_ASSERT(sim.get_gid());
            update_groups = sim.updategroups().get();
        }
        /*
        BOOST_FOREACH(updategroup_type & ug, update_groups)
        {
            bboxes.push_back(ug.bbox());
        }
        */
        hpx::naming::id_type this_id = this->get_gid();
        collection_id = sim.connect_writer(this_id);
        sim.connect_steerer(this_id);
        hpx::apply(HPX_STD_BIND(&flowingcanvas_gui::print_info, this));
        LOG("flowingcanvas_gui start completed...\n");

        hpx::wait(finished_future);
        hpx::wait(step_finished_future);
        sim.disconnect_writer(collection_id);
        hpx::wait(step_finished_future);
    }


    boost::shared_ptr<LibGeoDecomp::Steerer<LibGeoDecomp::CanvasCell> >
    flowingcanvas_gui::steer(LibGeoDecomp::Region<dim> const & validRegion)
    {
        //LOG("steer\n");
        std::vector<float> origin_x;
        std::vector<float> origin_y;

        std::vector<float> dest_x;
        std::vector<float> dest_y;
        {
            mutex_type::scoped_lock lk(force_mtx);
            
            std::swap(origin_x_, origin_x);
            std::swap(origin_y_, origin_y);
            std::swap(dest_x_, dest_x);
            std::swap(dest_y_, dest_y);
        }

        std::vector<signed char> image_data;
        int width = 0;
        int height = 0;
        {
            mutex_type::scoped_lock lk(image_mtx);
            std::swap(image_data_, image_data);
            width = width_;
            height = height_;
        }

#if !defined(__ANDROID__) || !defined(ANDROID)
        if(reset_)
        {
            LOG("reset!\n");
            reset_ = false;
            boost::shared_ptr<steerer> s = boost::make_shared<steerer>();

            s->op = steerer::STEER_RESET;
            return s;
        }
#endif

        if(!image_data.empty())
        {
            LOG("got new image!\n");
            boost::shared_ptr<steerer> s = boost::make_shared<steerer>();

            std::swap(image_data, s->image_data);
            s->width = width;
            s->height = height;

            s->op = steerer::STEER_IMAGE;
            return s;
        }

        if(
            !origin_x.empty()
         && !origin_y.empty()
         && !dest_x.empty()
         && !dest_y.empty()
        )
        {
            LOG("got new force!\n");
            boost::shared_ptr<steerer> s = boost::make_shared<steerer>();
            
            std::swap(s->origin_x_, origin_x);
            std::swap(s->origin_y_, origin_y);
            
            std::swap(s->dest_x_, dest_x);
            std::swap(s->dest_y_, dest_y);
            s->op = steerer::STEER_FORCE;
            return s;
        }
//#if !defined(__ANDROID__) || !defined(ANDROID)
        /*
        mutex_type::scoped_lock lk(data_mtx);
        std::size_t size = (validRegion.boundingBox().dimensions.y() * validRegion.boundingBox().dimensions.x()) * 3 + 3;
        LibGeoDecomp::Coord<2> origin = validRegion.boundingBox().origin;
        int wwidth = validRegion.boundingBox().dimensions.x();
        std::vector<char> part_data;
        part_data.resize(size);
        for(LibGeoDecomp::Region<2>::Iterator it = validRegion.begin(); it != validRegion.end(); ++it)
        {
            LibGeoDecomp::Coord<2> c = *it - origin;
            std::size_t ii = (c.y() * wwidth + c.x()) * 3;
            std::size_t jj = (it->y() * width + it->x()) * 3;

            part_data[ii + 0] = data[jj + 0];
            part_data[ii + 1] = data[jj + 1];
            part_data[ii + 2] = data[jj + 2];
        }

        return boost::make_shared<camera_steerer>(part_data, origin, wwidth);
        */
//#else
        return boost::shared_ptr<LibGeoDecomp::Steerer<LibGeoDecomp::CanvasCell> >();
//#endif
    }
    
    void flowingcanvas_gui::step_finished()
    {
//#if 0
        if(finished_future.ready()) return;
        //LOG("step finished " << step << " \n");


        steps_finished++;
        if(steps_finished != 1) return;
        //steps_finished.store(1);

        /*
        step_finished_future.then(
        */
        hpx::wait(step_finished_future);
        step_finished_future = hpx::async(
            HPX_STD_BIND(
                &flowingcanvas_gui::collect_finished
              , this
              //, HPX_STD_PLACEHOLDERS::_1
            )
        );
        //collect_finished();
//#endif
    }
        
    void flowingcanvas_gui::collect_finished()
    {
        while(steps_finished != 0)
        {
            std::vector<
                hpx::future<buffer_vector_type>
            > collection_futures;
            collection_futures.reserve(update_groups.size());
            
            BOOST_FOREACH(updategroup_type const & ug, update_groups)
            {
                collection_futures.push_back(
                    ug.collect_finished(collection_id)
                );
            }
            
            boost::shared_ptr<Particles> gs
                = boost::make_shared<Particles>();
            if(buffered_grid)
            {
                gs->posAngle.reserve(buffered_grid->posAngle.size());
                gs->colors.reserve(buffered_grid->colors.size());
            }

            while(!collection_futures.empty())
            {
                HPX_STD_TUPLE<
                    int
                  , hpx::future<buffer_vector_type>
                > res = hpx::wait_any(collection_futures);
                buffer_vector_type buffer = HPX_STD_GET(1, res).get();
                BOOST_FOREACH(saved_buffer_type const & sb, buffer)
                {
                    gs->posAngle.insert(
                        gs->posAngle.end()
                      , sb->posAngle.begin()
                      , sb->posAngle.end()
                    );
                    gs->colors.insert(
                        gs->colors.end()
                      , sb->colors.begin()
                      , sb->colors.end()
                    );
                }
                collection_futures.erase(collection_futures.begin() + HPX_STD_GET(0, res));
            }

            if(gs->size() > 0)
            {
                std::size_t remainder = gs->colors.size() % 140;
                if(remainder == 0)
                {
                    for(std::size_t i = 0; i < remainder; ++i)
                    {
                        gs->posAngle.push_back(0.0f);
                        gs->posAngle.push_back(0.0f);
                        gs->posAngle.push_back(0.0f);
                        gs->posAngle.push_back(0.0f);
                        gs->colors.push_back(0);
                    }
                }
                new_grid(gs);
            }
            fps.incFrames();
            --steps_finished;
        }
    }

}

BOOST_CLASS_EXPORT_GUID(server::steerer, "flowingcanvas_gui_steerer")

/*
HPX_REGISTER_ACTION(
    server::flowingcanvas_gui::create_component_action
  , flowingcanvas_gui_create_component_action
)

HPX_REGISTER_ACTION(
    server::flowingcanvas_gui::create_component_action_const
  , flowingcanvas_gui_create_component_action_const
)
*/
