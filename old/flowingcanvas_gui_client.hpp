
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef FLOWINGCANVAS_GUI_CLIENT_HPP
#define FLOWINGCANVAS_GUI_CLIENT_HPP

#include <hpx/hpx_fwd.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <canvascell.h>
#include <serialize/supermap.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/async.hpp>
#include <hpx/include/components.hpp>
#include <server/collector.hpp>
#include <flowingcanvas_gui.hpp>

#if !defined(LOG)

#if !defined(__ANDROID__) || !defined(ANDROID)
#define LOG(x...) std::cout << x;
#else
#include <android/log.h>
#define LOG(x...)                                                               \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "hpiif_android", "%s", sstr.str().c_str());            \
}                                                                               \
/**/
#endif

#endif

struct flowingcanvas_gui
    : hpx::components::client_base<
        ::flowingcanvas_gui
      , LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell>
    >
{
    typedef
        LibGeoDecomp::HiParSimulator::server::collector<LibGeoDecomp::CanvasCell>
        server_type;
    typedef 
        hpx::components::client_base<
            flowingcanvas_gui
          , server_type
        >
        base_type;
    flowingcanvas_gui()
    {}
    flowingcanvas_gui(hpx::future<hpx::id_type> const & id)
        : base_type(id)
    {}

    void start(std::string const & name, bool fake_cam)
    {
        hpx::async< server_type::start_action>(this->get_gid(), name, fake_cam).get();
    }

    void start_apply(std::string const & name, bool fake_cam)
    {
        hpx::apply< server_type::start_action>(this->get_gid(), name, fake_cam);
    }
        
    void new_force(float origin_x, float origin_y, float dest_x, float dest_y)
    {
        hpx::async<server_type::new_force_action>(this->get_gid(), origin_x, origin_y, dest_x, dest_y).get();
    }

    void new_image(std::vector<signed char> const & data, int width, int height)
    {
        hpx::async<server_type::new_image_action>(this->get_gid(), data, width, height).get();
    }
};

#endif
