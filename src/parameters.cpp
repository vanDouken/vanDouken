//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "parameters.hpp"
#include <boost/program_options.hpp>

namespace vandouken {
    void setupParameters(boost::program_options::options_description& parameters, const std::string& type)
    {
        bool runsSimulation = (type == "server") || (type == "standalone");

        if(runsSimulation)
        {
            parameters.add_options()
                ("overcommitFactor",
                 boost::program_options::value<std::size_t>()->default_value(1),
                 "Number of update groups to create. On each locality: #threads * overcommitFactor (default: 1)")
                ("dimX",
                 boost::program_options::value<int>()->default_value(300),
                 "Dimension of Simulation in X direction (default: 300)")
                ("dimY",
                 boost::program_options::value<int>()->default_value(150),
                 "Dimension of Simulation in Y direction (default: 150)")
                ;
        }
    }
}
