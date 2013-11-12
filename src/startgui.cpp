//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(__MIC)

#include "startgui.hpp"
#include "mainwindow.hpp"
#include "mainwindowserver.hpp"

#include <hpx/hpx.hpp>

#include <QApplication>

#ifndef LOG
#if !defined(ANDROID)
#define LOG(x,...) std::cout << x;
#else
#include <android/log.h>
#define LOG(x,...)                                                               \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "vandouken", "%s", sstr.str().c_str());            \
}                                                                               \
/**/
#endif
#endif

namespace {
    void runWidget(
        boost::shared_ptr<hpx::lcos::local::promise<void> > finishedPromise,
        boost::shared_ptr<hpx::lcos::local::promise<vandouken::MainWindow *> > mainWindowPromise,
        const LibGeoDecomp::Coord<2>& simulationDim,
        vandouken::GridProvider *gridProvider,
        vandouken::SteeringProvider *steererProvider,
        vandouken::MainWindow::Mode guiMode)
    {
        QCoreApplication *app = QApplication::instance();
        LOG("runWidget" << simulationDim << "\n");
        vandouken::MainWindow
            *main = new vandouken::MainWindow(simulationDim, gridProvider, steererProvider, guiMode);
        mainWindowPromise->set_value(main);
        LOG("main window created\n");

        main->resize(1000, 500);
        main->show();

        app->exec();
        LOG("finished ...\n");
        finishedPromise->set_value();
    }
}

namespace vandouken {
    void startGUI(
        boost::program_options::variables_map& vm,
        const SimulationController& simulation,
        vandouken::GridProvider *gridProvider,
        vandouken::SteeringProvider *steererProvider,
        MainWindow::Mode guiMode)
    {
        boost::shared_ptr<hpx::lcos::local::promise<void> >
            finishedPromise(new hpx::lcos::local::promise<void>);
        hpx::future<void> finished(finishedPromise->get_future());
        
        boost::shared_ptr<hpx::lcos::local::promise<MainWindow*> >
            mainWindowPromise(new hpx::lcos::local::promise<MainWindow*>);
        hpx::future<MainWindow *> mainWindowFuture(mainWindowPromise->get_future());

        hpx::util::io_service_pool *main_pool =
            hpx::get_runtime().get_thread_pool("main_pool");

        main_pool->get_io_service().post(
            HPX_STD_BIND(
                &::runWidget,
                finishedPromise,
                mainWindowPromise,
                simulation.getInitializer()->gridDimensions(),
                gridProvider,
                steererProvider,
                guiMode
            )
        );
        
        MainWindow *mainWindow(mainWindowFuture.get());
        hpx::id_type serverId;
        if(guiMode & MainWindow::Mode::control)
        {
            std::string name(VANDOUKEN_MAIN_WINDOW_NAME);
            serverId = hpx::components::new_<MainWindowServer>(hpx::find_here(), mainWindow).get();
            hpx::agas::register_name_sync(name, serverId);
            LOG("registered: " << name << "\n");
        }
        else
        {
            std::string name(VANDOUKEN_MAIN_WINDOW_NAME);
            while(serverId == hpx::naming::invalid_id)
            {
                hpx::naming::id_type id;
                hpx::agas::resolve_name_sync(name, id);
                if(!id)
                {
                    hpx::this_thread::suspend(boost::posix_time::seconds(1));
                    continue;
                }
                hpx::naming::gid_type gid = id.get_gid();
                hpx::naming::detail::strip_credit_from_gid(gid);
                serverId = hpx::id_type(gid, hpx::id_type::unmanaged);
            }
            LOG("resolved: " << name << "\n");
        }
        mainWindow->setServerId(serverId);

        if(guiMode == MainWindow::Mode::picturesOnly)
        {
            LOG("starting image retrival ...\n");
            while(!finished.is_ready())
            {
                hpx::util::high_resolution_timer timer;
                QImage image = MainWindowServer::GetImageAction()(serverId);
                if(image.isNull()) continue;

                LOG("got image\n");

                QMetaObject::invokeMethod(
                    mainWindow,
                    "setImage",
                    Qt::AutoConnection,
                    Q_ARG(QImage, image)
                );
                double elapsed = timer.elapsed() * 1000;
                if(elapsed < 40.0)
                {
                    double remain = 40.0 - elapsed;
                    hpx::this_thread::suspend(boost::posix_time::milliseconds(remain));
                }
            }
        }
        else
        {
            hpx::wait(finished);
        }
    }
}

#endif
