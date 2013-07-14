//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "simulation.hpp"
#include "simulationcontroller.hpp"
#include "parameters.hpp"

#include <hpx/hpx_fwd.hpp>
#include <hpx/hpx_init.hpp>

#include <QApplication>

int hpx_main(boost::program_options::variables_map& vm)
{
    return hpx::finalize();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    boost::program_options::options_description
        commandLineParameters("Usage: " HPX_APPLICATION_STRING " [options]");

    vandouken::setupParameters(commandLineParameters, "gui");

    return hpx::init(commandLineParameters, argc, argv);
}
