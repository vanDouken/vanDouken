//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_SIMULATIONCONTROLLER_HPP
#define VANDOUKEN_SIMULATIONCONTROLLER_HPP

#include "config.hpp"

#include <hpx/runtime/naming/name.hpp>
#include <libgeodecomp/misc/coord.h>

namespace vandouken {
    class SimulationControllerServer;
    class SimulationController
    {
    public:
        typedef
            SimulationControllerServer
            ComponentType;

        SimulationController();

        SimulationController(const LibGeoDecomp::Coord<2>& simulationDim, std::size_t overcommitFactor);

    private:
        hpx::naming::id_type thisId;
    };
}

#endif
