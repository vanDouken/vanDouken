
//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindowserver.hpp"
#include "mainwindow.hpp"

namespace vandouken
{
    void MainWindowServer::stateChanged(int state, bool callReset)
    {
        QMetaObject::invokeMethod(
            mainWindow,
            "stateChanged",
            Qt::AutoConnection,
            Q_ARG(int, state),
            Q_ARG(bool, callReset)
        );
    }
        
    QImage MainWindowServer::getImage()
    {
        return mainWindow->getImage();
    }
}

HPX_REGISTER_ACTION(
    vandouken::MainWindowServer::StateChangedAction,
    MainWindowServerStateChangedAction)

HPX_REGISTER_ACTION(
    vandouken::MainWindowServer::GetImageAction,
    MainWindowGetImageAction)

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::managed_component<vandouken::MainWindowServer>,
    vandoukenMainWindowServer);
