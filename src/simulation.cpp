//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "simulation.hpp"
#include <boost/program_options.hpp>

namespace vandouken {
    SimulationController runSimulation(boost::program_options::variables_map& vm)
    {
        LibGeoDecomp::Coord<2> simulationDim(
            vm["dimX"].as<int>(),
            vm["dimY"].as<int>());
        return SimulationController(simulationDim);
    }
}
