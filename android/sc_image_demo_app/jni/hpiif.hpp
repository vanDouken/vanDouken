//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPIIF_HPP
#define HPIIF_HPP

#include <flowingcanvas_gui_client.hpp>
#include <model_test/particlewidget.h>

namespace detail {
    struct hpiif
    {
        hpiif()
        {
        }
        
        flowingcanvas_gui gui;
        ParticleWidget *flow;

        boost::shared_ptr<hpx::lcos::promise<void> > finished_promise;
    };
}

detail::hpiif & hpiif();

#endif
