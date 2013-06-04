
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/async.hpp>
#include <hpx/include/components.hpp>
#include <flowingcanvas_collector.hpp>
#include <flowingcanvas_updategroup.hpp>
#include <flowingcanvas_simulator.hpp>
#include <server/collector.hpp>
#include <flowingcanvas_gui.hpp>
#include <flowingcanvas_gui_client.hpp>
#include <canvascell.h>
#include <serialize/supermap.hpp>
#include <hpx_simulator.hpp>
#include <fpscounter.h>
#include <canvasinitializer.h>

#include <boost/range/algorithm.hpp>
#include <vector>
#include <QApplication>

using namespace LibGeoDecomp;
using namespace LibGeoDecomp::HiParSimulator;

using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::value;

FPSCounter fps;

void print_info()
{
    while(true)
    {
        hpx::this_thread::suspend(5000);
        std::cout << "InteractiveSimulator @ " << fps.fps() << " FPS\n\n";
    }
}

typedef
    hpx_simulator<CanvasCell, RecursiveBisectionPartition<2> >
    simulator_type;

void run_simulation(simulator_type simulator)
{
    //simulator.wait();
    while(true)//flow->is_running)
    {
        simulator.step();
        //(canvas_writer <= updateImageFun(flow)).get();
        fps.incFrames();
    }
}

boost::shared_ptr<CanvasInitializer> getInit1(variables_map & vm)
{
    int width = vm["width"].as<int>();
    int height = vm["height"].as<int>();
    LibGeoDecomp::Coord<2>
        dim(
            vm["width"].as<int>()
          , vm["height"].as<int>()
        );
    boost::shared_ptr<CanvasInitializer>
        init(
            new CanvasInitializer(
                vm["path"].as<std::string>(), dim
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
    return init;
}

void start_simulation(variables_map & vm)
{
    {
        boost::shared_ptr<CanvasInitializer>
            init(
                getInit1(vm)
            );

        std::size_t num_partitions = vm["num_partitions"].as<std::size_t>();

        std::string name = "flowingcanvas_simulator";

        simulator_type simulator;
        simulator.create(hpx::find_here(), num_partitions, name);
            
        //hpx::agas::register_name(name, simulator.get_gid());
     
        simulator.init(init);

        hpx::apply(HPX_STD_BIND(print_info));

        hpx::apply(HPX_STD_BIND(run_simulation, simulator));
    }
}

int hpx_main(variables_map & vm)
{
    {
        std::cout << "in hpx main ...\n";
        if(vm["standalone"].as<bool>())
        {
            start_simulation(vm);
        }
        std::string name = "flowingcanvas_simulator";
        flowingcanvas_gui gui(
            hpx::components::new_<
                ::server::flowingcanvas_gui
            >(
                hpx::find_here()
            )
        );
        gui.start(name, false);
    }

    return hpx::finalize();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    options_description
        desc_commandline("usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        (
              "standalone"
            , value<bool>()->default_value(false)
            , "Run GUI and simulation"
        )
        (
              "num_partitions"
            , value<std::size_t>()->default_value(1)
            , "Number of partitions to create"
        )
        (
              "width"
            , value<int>()->default_value(320)
            , "Width"
        )
        (
              "height"
            , value<int>()->default_value(240)
            , "Width"
        )
        (
              "path"
            , value<std::string>()->default_value("./starry_night.png")
            , "Path to default image"
        )
        ;
    
    std::cout << "in main ...\n";
    hpx::init(
        desc_commandline
      , argc
      , argv
    );
    std::cout << "after init ...\n";
}
