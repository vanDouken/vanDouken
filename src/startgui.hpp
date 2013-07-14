//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_STARTGUI_HPP
#define VANDOUKEN_STARTGUI_HPP

#include "config.hpp"
//#include "simulationcontroller.hpp"

#include <libgeodecomp/config.h>
#include <hpx/config.hpp>

#include <boost/program_options/variables_map.hpp>

namespace vanDouken
{
    void startGUI(boost::program_options::variables_map& vm);
}

#endif
