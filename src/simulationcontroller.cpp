//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "simulationcontroller.hpp"
#include "simulationcontrollerserver.hpp"

#include <hpx/runtime/components/new.hpp>

namespace vandouken {
    SimulationController::SimulationController()
    {
        std::size_t retry = 0;
        while(thisId == hpx::naming::invalid_id)
        {
            hpx::agas::resolve_name(VANDOUKEN_SIMULATION_CONTROLLER_NAME, thisId);
            if(retry > 10) {
                throw std::logic_error("Could not connect to simulation");
            }
            ++retry;
        }
    }

    SimulationController::SimulationController(const LibGeoDecomp::Coord<2>& simulationDim)
    {
        thisId = hpx::components::new_<ComponentType>(hpx::find_here(), simulationDim).get();
        hpx::agas::register_name(VANDOUKEN_SIMULATION_CONTROLLER_NAME, thisId);
    }

    SimulationController::~SimulationController()
    {
        thisId = hpx::naming::invalid_id;
    }

    boost::shared_ptr<LibGeoDecomp::Initializer<Cell> > SimulationController::getInitializer() const
    {
        return ComponentType::GetInitializerAction()(thisId);
    }

    hpx::future<void> SimulationController::run() const
    {
        return hpx::async<ComponentType::RunAction>(thisId);
    }

    void SimulationController::stop() const
    {
        ComponentType::StopAction()(thisId);
    }
        
    std::size_t SimulationController::numUpdateGroups() const
    {
        return ComponentType::NumUpdateGroupsAction()(thisId);
    }
}
