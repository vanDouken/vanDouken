//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridcollectorserver.hpp"
#include "gridcollector.hpp"

namespace vandouken {
    std::size_t GridCollectorServer::addGridConsumer()
    {
        return collector->addGridConsumer();
    }

    void GridCollectorServer::removeGridConsumer(std::size_t id)
    {
        collector->removeGridConsumer(id);
    }

    std::pair<unsigned, RegionBuffer>
    GridCollectorServer::getNextBuffer(std::size_t id)
    {
        return collector->getNextBuffer(id);
    }
}

HPX_REGISTER_ACTION(
    vandouken::GridCollectorServer::AddGridConsumerAction,
    vandoukenGridCollectorServerAddGridConsumerAction)

HPX_REGISTER_ACTION(
    vandouken::GridCollectorServer::RemoveGridConsumerAction,
    vandoukenGridCollectorServerRemoveGridConsumerAction)

HPX_REGISTER_ACTION(
    vandouken::GridCollectorServer::GetNextBufferAction,
    vandoukenGridCollectorServerGetNextBufferAction)


HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::managed_component<vandouken::GridCollectorServer>,
    vandoukenGridCollectorServerServer);
