
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef FLOWINGCANVAS_GUI_HPP
#define FLOWINGCANVAS_GUI_HPP

#include <hpx/hpx_fwd.hpp>
#include <hpx/hpx.hpp>
#include <server/collector.hpp>
#include <hpx_simulator.hpp>
#include <hpx_updategroup.hpp>
#include <canvascell.h>
#include <grid_storage.hpp>
#include <libgeodecomp/parallelization/hiparsimulator/gridvecconv.h>
#include <libgeodecomp/misc/region.h>
#include <libgeodecomp/misc/coordbox.h>
#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QImage>
#include <QVector2D>
#include <QtCore>
#include <canvasinitializer.h>
#endif

#include <model_test/particle.h>

#include <fpscounter.h>

namespace LibGeoDecomp {
    class CanvasInitializer;
}
//class FlowWidget;
class ParticleWidget;

namespace server {
struct steerer
    : LibGeoDecomp::Steerer<LibGeoDecomp::CanvasCell>
{
    std::vector<float> origin_x_;
    std::vector<float> origin_y_;

    std::vector<float> dest_x_;
    std::vector<float> dest_y_;

    std::vector<signed char> image_data;
    int width;
    int height;
    
    typedef LibGeoDecomp::Region<2> Region;

    enum {
        STEER_FORCE,
        STEER_IMAGE,
        STEER_RESET
    };
    int op;
    /*
    std::vector<char> data;
    LibGeoDecomp::Coord<2> origin;
    int wwidth;
    */

    steerer()
        : LibGeoDecomp::Steerer<LibGeoDecomp::CanvasCell>(1)
    {}
    
    void reset(
        GridType * grid
      , LibGeoDecomp::Region<2> const & validRegion
    )
    {
#if !defined(__ANDROID__) || !defined(ANDROID)
        LibGeoDecomp::Coord<2> dim(320, 240);
        boost::shared_ptr<LibGeoDecomp::CanvasInitializer>
            init(
                new LibGeoDecomp::CanvasInitializer(
                    "./starry_night.jpg", dim
                )
            );
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 1160, 0.25 * 188), 0.25 * 90, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 904, 0.25 * 240) , 0.25 * 44, 0.3 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 778, 0.25 * 92)  , 0.25 * 30, 0.2 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 526, 0.25 * 72)  , 0.25 * 20, 0.15));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 444, 0.25 * 44)  , 0.25 * 10, 0.15 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 296, 0.25 * 34)  , 0.25 * 6 , 0.1 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 302, 0.25 * 180) , 0.25 * 20, 0.25 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 138, 0.25 * 46)  , 0.25 * 10, 0.2 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 416, 0.25 * 328) , 0.25 *  4, 0.1 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 456, 0.25 * 538) , 0.25 * 68, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 170, 0.25 * 492) , 0.25 *  6, 0.1 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(0.25 * 58 , 0.25 * 460) , 0.25 *  4, 0.1 ));

        init->grid(grid);
#endif
    }

    void steerForce(
        GridType * grid
      , LibGeoDecomp::Region<2> const & validRegion
    )
    {
        if(
            !origin_x_.empty()
         && !origin_y_.empty()
         && !dest_x_.empty()
         && !dest_y_.empty()
        )
        {
            Region lastRenderedRegion;
            for(std::size_t i = 0; i < origin_x_.size(); ++i)
            {
                LibGeoDecomp::Coord<2>
                    origin(
                        origin_x_[i]
                      , origin_y_[i]
                    );
                LibGeoDecomp::Coord<2>
                    delta(
                        dest_x_[i]
                      , dest_y_[i]
                    );
                
                /*
                LibGeoDecomp::Coord<2>
                    delta = origin - dest;
                */

                double length = std::sqrt(double(delta.x()) * delta.x() + delta.y() * delta.y());
                LibGeoDecomp::FloatCoord<2> force(0.0f, 0.0f);
                if(length > 0.0)
                {
                    force[0] = delta.x() / length;
                    force[1] = delta.y() / length;
                }

                std::cout << " " << origin << " -> " << delta << ", force: " << force << "\n";

                if (abs(delta.x()) > abs(delta.y())) {
                    if (delta.x() < 0) {
                        origin = origin + delta;
                        delta = -delta;
                    }
                    for (int x = 0; x <= delta.x(); ++x) {
                        int y = origin.y() + x * delta.y() / (delta.x() + 1);
                        renderForce(origin.x() + x, y, force, grid, validRegion, &lastRenderedRegion);
                    }
                } else {
                    if (delta.y() < 0) {
                        origin = origin + delta;
                        delta = -delta;
                    }
                    for (int y = 0; y <= delta.y(); ++y) {
                        int x = origin.x() + y * delta.x() / (delta.y() + 1);
                        renderForce(x, origin.y() + y, force, grid, validRegion, &lastRenderedRegion);
                    }
                }
            }
        }
    }

    void renderForce(int posX, int posY, const LibGeoDecomp::FloatCoord<2>& force, GridType *grid, const Region& validRegion, Region *lastRenderedRegion)
    {
        // std::cout << "renderForce(" << posX << ", " << posY << ")\n";

        int size = 10;
        Region newRegion;
        for (int y = -size; y < size; ++y) {
            newRegion << LibGeoDecomp::Streak<2>(LibGeoDecomp::Coord<2>(posX - size, posY + y), posX + size);
        }

        Region relevantRegion = newRegion & *lastRenderedRegion & validRegion;

        for (Region::StreakIterator i = relevantRegion.beginStreak(); 
             i != relevantRegion.endStreak(); 
             ++i) {
            for (LibGeoDecomp::Coord<2> c = i->origin; c.x() < i->endX; ++c.x()) {
                grid->at(c).setForceVario(force[0], force[1]);
            }
        }
        
        *lastRenderedRegion = newRegion;

    }
    
    void steerImage(
        GridType * grid
      , LibGeoDecomp::Region<2> const & validRegion
    )
    {
#if !defined(__ANDROID__) || !defined(ANDROID)
        LibGeoDecomp::Coord<2> dim = grid->boundingBox().dimensions;

        std::cout << "new image: " << width << " " << height << " scaled to: " << dim.x() << " " << dim.y() << "\n";

        QImage
            img = QImage::fromData(
                reinterpret_cast<unsigned char *>(&image_data[0])
              , width*height
              , "jpg"
            ).scaled(320, 240);
        LibGeoDecomp::FloatCoord<2> force(0.0f, 0.0f);
        for(LibGeoDecomp::Region<2>::Iterator i = validRegion.begin(); i != validRegion.end(); ++i)
        {
            grid->at(*i)
                = LibGeoDecomp::CanvasCell(
                    0xff000000 + img.pixel(i->x(), i->y())
                  , *i
                  , false
                  , force
                  , rand() % LibGeoDecomp::CanvasCell::MAX_SPAWN_COUNTDOWN
                );
        }
#endif
    }

    void nextStep(
        GridType * grid
      , LibGeoDecomp::Region<2> const & validRegion
      , const unsigned & step
    )
    {
        switch(op)
        {
            case STEER_FORCE:
                steerForce(grid, validRegion);
                break;
            case STEER_IMAGE:
                steerImage(grid, validRegion);
                break;
            case STEER_RESET:
                reset(grid, validRegion);
                break;
        }
    }

    template <typename Archive>
    void serialize(Archive & ar, unsigned)
    {
        ar & boost::serialization::base_object<LibGeoDecomp::Steerer<LibGeoDecomp::CanvasCell> >(*this);

        ar & op;

        switch(op)
        {
            case STEER_FORCE:
                ar & origin_x_;
                ar & origin_y_;
                ar & dest_x_;
                ar & dest_y_;
                break;
            case STEER_IMAGE:
                ar & image_data;
                ar & width;
                ar & height;
                break;
            case STEER_RESET:
                break;
        }
    }
};

    struct HPX_COMPONENT_EXPORT flowingcanvas_gui
        : LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell>
        , hpx::components::managed_component_base< ::server::flowingcanvas_gui>
    {

        typedef LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell> base_type;
        typedef hpx::components::managed_component< ::server::flowingcanvas_gui> wrapping_type;

        typedef flowingcanvas_gui type_holder;
        typedef base_type base_type_holder;
        
        typedef
            LibGeoDecomp::HiParSimulator::hpx_updategroup<
                LibGeoDecomp::CanvasCell
              , LibGeoDecomp::HiParSimulator::RecursiveBisectionPartition<2>
            >
            updategroup_type;

        flowingcanvas_gui();

        /*
        typedef
            hpx::components::server::create_component_action2<
                flowingcanvas_gui
            >
            create_component_action_const;

        typedef
            hpx::components::server::create_component_action2<
                flowingcanvas_gui
              , int
              , int
            >
            create_component_action;
        */

        void start(std::string const & name, bool fake_cam);

        void do_step(std::vector<char> const & data);

        typedef hpx::lcos::local::spinlock mutex_type;

        mutex_type mtx;

        void step_finished();
        
        typedef 
            Particles
            buffer_type;
        typedef
            boost::shared_ptr<buffer_type>
            saved_buffer_type;
        typedef
            std::vector<saved_buffer_type>
            buffer_vector_type;

        void collect_finished(/*hpx::future<void> f,*/);
        
        typedef LibGeoDecomp::Grid<LibGeoDecomp::CanvasCell> grid_type;

        boost::shared_ptr<LibGeoDecomp::Steerer<LibGeoDecomp::CanvasCell> > steer(LibGeoDecomp::Region<dim> const & validRegion);

        typedef
            LibGeoDecomp::HiParSimulator::hpx_simulator<
                LibGeoDecomp::CanvasCell
              , LibGeoDecomp::HiParSimulator::RecursiveBisectionPartition<2>
            >
            simulator_type;
        simulator_type sim;
        hpx::future</*FlowWidget*/ParticleWidget *> flow_future;
        hpx::future<void> finished_future;

        std::vector<LibGeoDecomp::CoordBox<2> > bboxes;
        mutex_type buffered_grids_mtx;
        //std::deque<boost::shared_ptr<std::vector<Particle> > > buffered_grids;
        boost::shared_ptr<Particles> buffered_grid;
        void new_grid(boost::shared_ptr<Particles> g)
        {
            mutex_type::scoped_lock lk(buffered_grids_mtx);
            //buffered_grids.push_back(g);
            buffered_grid = g;
        }
        boost::shared_ptr<Particles> next_grid()
        {
            //boost::shared_ptr<std::vector<Particle> > res;
            
            mutex_type::scoped_lock lk(buffered_grids_mtx);
            /*
            if(buffered_grids.empty())
                return res;

            res = buffered_grids.front();
            buffered_grids.pop_front();
            return res;
            */
            return buffered_grid;
        }


        hpx::future<void> step_finished_future;
        boost::atomic<int> steps_finished;
        std::vector<updategroup_type> update_groups;
        std::size_t collection_id;
        std::vector<hpx::future<void> > do_step_futures;

        mutex_type grid_mtx;

        mutex_type data_mtx;
        std::vector<char> data;

        mutex_type force_mtx;
        std::vector<float> origin_x_;
        std::vector<float> origin_y_;

        std::vector<float> dest_x_;
        std::vector<float> dest_y_;

#if !defined(__ANDROID__) || !defined(ANDROID)
        boost::atomic<bool> reset_;
        void reset()
        {
            reset_ = true;
        }
#else
        void reset() {}
#endif
        public:
        void new_force(float origin_x, float origin_y, float dest_x, float dest_y)
        {
            mutex_type::scoped_lock lk(force_mtx);
            origin_x_.push_back(origin_x);
            origin_y_.push_back(origin_y);
            dest_x_.push_back(dest_x);
            dest_y_.push_back(dest_y);
        }

        mutex_type image_mtx;
        std::vector<signed char> image_data_;
        int width_;
        int height_;

        void new_image(std::vector<signed char> const & image_data, int width, int height)
        {
            mutex_type::scoped_lock lk(image_mtx);
            image_data_ = image_data;
            width_ = width;
            height_ = height;
        }
        

        void print_info()
        {
            while(!finished_future.ready())
            {
                hpx::this_thread::suspend(5000);
                LOG("GUI @ " << fps.fps() << " FPS\n");
            }
        }

        FPSCounter fps;
    };
}

/*
HPX_REGISTER_ACTION_DECLARATION(
    server::flowingcanvas_gui::create_component_action
  , flowingcanvas_gui_create_component_action
)

HPX_REGISTER_ACTION_DECLARATION(
    server::flowingcanvas_gui::create_component_action_const
  , flowingcanvas_gui_create_component_action_const
)
*/

#endif
