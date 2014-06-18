//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_SIMULATIONCONTROLLER_HPP
#define VANDOUKEN_SIMULATIONCONTROLLER_HPP

#include "config.hpp"
#include "cell.hpp"

#include <hpx/runtime/naming/name.hpp>
#include <libgeodecomp/geometry/coord.h>
#include <libgeodecomp/io/initializer.h>

namespace vandouken {
    class SimulationControllerServer;
    class SimulationController
    {
    public:
        typedef
            SimulationControllerServer
            ComponentType;

        SimulationController();

        SimulationController(const LibGeoDecomp::Coord<2>& simulationDim);

        ~SimulationController();

        boost::shared_ptr<LibGeoDecomp::Initializer<Cell> > getInitializer() const;

        hpx::lcos::future<void> run() const;
        void stop() const;

        std::size_t numUpdateGroups() const;
    private:
        hpx::naming::id_type thisId;
    };
}

#endif
