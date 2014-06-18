//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(__MIC)

#include "controlgui.hpp"
#include "gridprovider.hpp"
#include "steeringprovider.hpp"
#include "startgui.hpp"
#include "simulation.hpp"
#include "simulationcontroller.hpp"
#include "parameters.hpp"
#include "mainwindow.hpp"

#include <hpx/hpx_fwd.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include <boost/assign.hpp>
        
#include <QApplication>
#include <QDialog>
#include "ui_startdialog.h"

#include "log.hpp"

int hpx_main(boost::program_options::variables_map& vm)
{
    MSG("inside hpx_main\n")
    vandouken::SimulationController simulation;
    MSG("got " << simulation.numUpdateGroups());
    LibGeoDecomp::Coord<2> dim = simulation.getInitializer()->gridDimensions();
    MSG("got " << dim.x() << " " << dim.y());
    vandouken::SteeringProvider steererProvider(simulation.numUpdateGroups(), simulation.getInitializer()->gridDimensions());
    vandouken::startGUI(
        vm
      , simulation
      , 0
      , &steererProvider
      , vandouken::MainWindow::picturesOnly
    );
    MSG("inside hpx_main done\n")
    return hpx::disconnect();
}

#endif

void controlGUI(int argc, char **argv)
{
#if !defined(__MIC)
    QApplication app(argc, argv);
#if !defined(ANDROID)
    QDialog qDialog;
    Ui_Dialog dialog;
    dialog.setupUi(&qDialog);
    dialog.numThreads->setMaximum(hpx::threads::hardware_concurrency());
    if(qDialog.exec() == QDialog::Rejected)
        return;

    std::string agasHost = dialog.host->displayText().toStdString();
    std::string agasPort = dialog.port->displayText().toStdString();
    std::string numThreads = boost::lexical_cast<std::string>(dialog.numThreads->value());
#else
    std::string agasHost = "192.168.0.1";
    std::string agasPort = "7910";
    std::string numThreads = "2";
#endif
    
    MSG("starting HPX ... " << agasHost << " " << agasPort << " " << numThreads << "\n")
    std::vector<std::string> cfg;
    cfg.push_back("hpx.run_hpx_main!=1");
    cfg.push_back("hpx.os_threads=" + numThreads);
    cfg.push_back("hpx.agas.address=" + agasHost);
    cfg.push_back("hpx.agas.port=" + agasPort);
    //cfg.push_back("hpx.logging.level=5");
#if defined(ANDROID)
    cfg.push_back("hpx.parcel.address=192.168.0.2");
#endif

    boost::program_options::options_description
        commandLineParameters("Usage: " HPX_APPLICATION_STRING " [options]");

    hpx::init(
        commandLineParameters,
        argc,
        argv,
        cfg,
        HPX_STD_FUNCTION<void()>(),
        HPX_STD_FUNCTION<void()>(),
        hpx::runtime_mode_connect);
#endif
}
