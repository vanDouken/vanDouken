
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <canvascell.h>
#include <libgeodecomp/misc/region.h>
#include <serialize/supermap.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/async.hpp>
#include <hpx/apply.hpp>
#include <hpx/components/remote_object/new.hpp>
#include <flowingcanvas_collector.hpp>
#include <flowingcanvas_updategroup.hpp>
#include <flowingcanvas_simulator.hpp>
#include <hpx_simulator.hpp>
#include <fpscounter.h>
#include <flowingcanvas_gui.hpp>
#include <canvasinitializer.h>

#include <boost/range/algorithm.hpp>
#include <vector>

using namespace LibGeoDecomp;
using namespace LibGeoDecomp::HiParSimulator;

// class SimParams
// {
// public:
//     int maxCameraFrames;
//     float gradientCutoff;
// };

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
boost::shared_ptr<CanvasInitializer> getInit1(variables_map & vm)
{
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
    init->addShape(ForcePrimitives::Circle(Coord<2>(260,  62), 15, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(260,  62), 20, 0.0 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>( 67, 120), 70, 0.2 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>( 45,  62), 90, 0.15));
    init->addShape(ForcePrimitives::Circle(Coord<2>(125,  30), 10, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>( 35,  22),  5, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(167,  82), 10, 0.4 ));
    return init;
}

int hpx_main(variables_map & vm)
{
    {
        boost::shared_ptr<CanvasInitializer>
            init(
                getInit1(vm)
            );

        typedef
            hpx_simulator<CanvasCell, RecursiveBisectionPartition<2> >
            simulator_type;

        std::size_t num_partitions = vm["num_partitions"].as<std::size_t>();

        std::string name = "flowingcanvas_simulator";

        simulator_type simulator;
        simulator.create(hpx::find_here(), num_partitions, name);
            
        //hpx::agas::register_name(name, simulator.get_gid());
     
        simulator.init(init);

        hpx::apply(HPX_STD_BIND(print_info));

        //simulator.wait();
        while(true)//flow->is_running)
        {
            simulator.step();
            //(canvas_writer <= updateImageFun(flow)).get();
            fps.incFrames();
        }
    }

    return hpx::finalize();
}

int main(int argc, char *argv[])
{
    using boost::program_options::options_description;
    options_description
        desc_commandline("usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
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
    
    hpx::init(
        desc_commandline
      , argc
      , argv
    );
}
