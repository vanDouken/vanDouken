//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_SIMULATIONCONTROLLERSERVER_HPP
#define VANDOUKEN_SIMULATIONCONTROLLERSERVER_HPP

#include "simulator.hpp"

#include <libgeodecomp/misc/coord.h>

namespace vandouken {
    class SimulationControllerServer :
        public hpx::components::managed_component_base<SimulationControllerServer>
    {
    public:
        typedef LibGeoDecomp::Coord<2> CoordType;

        typedef
            hpx::components::server::create_component_action2<
                SimulationControllerServer,
                const CoordType,
                std::size_t>
            CreateComponentAction;

        SimulationControllerServer();

        SimulationControllerServer(const CoordType& simulationDim, std::size_t overcommitFactor);

    private:
        Simulator simulator;
    };
}

HPX_REGISTER_ACTION_DECLARATION(
    vandouken::SimulationControllerServer::CreateComponentAction,
    vandoukenSimulationControllerServerCreateComponentAction)

#endif
