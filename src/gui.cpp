//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "steeringprovider.hpp"
#include "startgui.hpp"
#include "simulation.hpp"
#include "simulationcontroller.hpp"
#include "parameters.hpp"

#include <boost/assign.hpp>

#include <hpx/hpx_fwd.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include <QApplication>
#include <QDialog>
#include "ui_startdialog.h"

bool standalone = false;

int hpx_main(boost::program_options::variables_map& vm)
{
    if(standalone) {
        vandouken::SimulationController simulation = vandouken::runSimulation(vm);
        hpx::future<void> runFuture = simulation.run();
        std::cout << "simulation started\n";
        vandouken::GridProvider gridProvider(simulation.numUpdateGroups(), simulation.getInitializer()->gridBox());
        vandouken::SteeringProvider steererProvider(simulation.numUpdateGroups(), simulation.getInitializer()->gridDimensions());
        vandouken::startGUI(vm, simulation, &gridProvider, &steererProvider, vandouken::MainWindow::control);
        simulation.stop();
        hpx::wait(runFuture);
        return hpx::finalize();
    }
    else {
        vandouken::SimulationController simulation;
        vandouken::GridProvider gridProvider(simulation.numUpdateGroups(), simulation.getInitializer()->gridBox());
        vandouken::SteeringProvider steererProvider(simulation.numUpdateGroups(), simulation.getInitializer()->gridDimensions());
        vandouken::startGUI(vm, simulation, &gridProvider, &steererProvider, vandouken::MainWindow::control);
        return hpx::disconnect();
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setOverrideCursor(QCursor(Qt::BlankCursor));
    /*
    QDialog qDialog;
    Ui_Dialog dialog;
    dialog.setupUi(&qDialog);
    dialog.numThreads->setMaximum(hpx::threads::hardware_concurrency());
    if(qDialog.exec() == QDialog::Rejected)
        return -1;

    standalone = dialog.runLocalCheck->isChecked();

    std::string agasHost = dialog.host->displayText().toStdString();
    std::string agasPort = dialog.port->displayText().toStdString();
    std::string numThreads = boost::lexical_cast<std::string>(dialog.numThreads->value());
    std::string overcommitFactor = boost::lexical_cast<std::string>(dialog.overcommitFactor->value());
    */

    boost::program_options::options_description
        commandLineParameters("Usage: " HPX_APPLICATION_STRING " [options]");

    /*
    using namespace boost::assign;
    std::vector<std::string> cfg;
    cfg += "vandouken.overcommitfactor!=" + overcommitFactor;
    cfg += "hpx.os_threads=" + numThreads;
    */
    standalone = true;
    //if(standalone) 
    {
        vandouken::setupParameters(commandLineParameters, "standalone");
        /*
        cfg += "hpx.agas.port=" + agasPort;
        return hpx::init(commandLineParameters, argc, argv, cfg);
        */
        return hpx::init(commandLineParameters, argc, argv);
    }
    /*
    else {
        vandouken::setupParameters(commandLineParameters, "gui");
        cfg += "hpx.runtime_mode=connect";
        cfg += "hpx.agas.address=" + agasHost;
        return hpx::init(
            commandLineParameters,
            argc,
            argv,
            cfg,
            HPX_STD_FUNCTION<void()>(),
            HPX_STD_FUNCTION<void()>(),
            hpx::runtime_mode_connect);
    }
    */
}
