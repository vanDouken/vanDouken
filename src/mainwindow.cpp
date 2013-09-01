//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "steeringprovider.hpp"
#include "resetsimulation.hpp"
#include "mainwindow.hpp"
#include "maincontrol.hpp"
#include "forcecontrol.hpp"
#include "forceview.hpp"
#include "camerapreview.hpp"
#include "particlewidget.hpp"

#include <QResizeEvent>

namespace vandouken {
    MainWindow::MainWindow(
        LibGeoDecomp::Coord<2> dim,
        GridProvider *gridProvider,
        SteeringProvider *steeringProvider,
        QWidget *parent) :
        QWidget(parent),
        steeringProvider(steeringProvider),
        state(MainControl::free)
    {
        particleWidget =
            new ParticleWidget(
                gridProvider,
                steeringProvider,
                dim,
                Qt::black,
                this);

        mainControl = new MainControl(this);
        forceControl = new ForceControl(this);
        forceControl->hide();
        forceView =
            new ForceView(
                particleWidget
              , steeringProvider
              , this);

        forceView->hide();
        cameraPreview = new CameraPreview(steeringProvider, this);
        cameraPreview->hide();

        QObject::connect(&paintTimer, SIGNAL(timeout()), particleWidget, SLOT(updateGL()));
        QObject::connect(&paintTimer, SIGNAL(timeout()), forceView, SLOT(repaint()));
        QObject::connect(&grabTimer, SIGNAL(timeout()), cameraPreview, SLOT(grabFrame()));

        QObject::connect(
            forceControl, SIGNAL(directionChanged(int))
          , forceView, SLOT(directionChanged(int))
        );

        QObject::connect(
            forceControl, SIGNAL(forceChanged(double))
          , forceView, SLOT(forceChanged(double))
        );

        QObject::connect(
            mainControl, SIGNAL(stateChanged(int))
          , this, SLOT(stateChanged(int))
        );

        QObject::connect(
            cameraPreview, SIGNAL(stateChanged(int))
          , this, SLOT(stateChanged(int))
        );

        paintTimer.start(20);
        grabTimer.start(20);

        showFullScreen();
    }

    void MainWindow::stateChanged(int s)
    {
        if(s == MainControl::resetAll)
        {
            switch(state)
            {
                case MainControl::free:
                    steeringProvider->steer(ResetSimulation());
                    mainControl->state = MainControl::free;
                    state = MainControl::free;
                    break;
                case MainControl::edit:
                    steeringProvider->steer(ResetSimulation());
                    mainControl->state = MainControl::edit;
                    state = MainControl::edit;
                    break;
                case MainControl::picture:
                    cameraPreview->steer();
                    mainControl->state = MainControl::free;
                    state = MainControl::free;
                    break;
                case MainControl::video:
                    cameraPreview->enableGrab(false);
                    steeringProvider->steer(ResetSimulation());
                    mainControl->state = MainControl::free;
                    state = MainControl::free;
                    break;
            }
        }
        else
        {
            state = s;
        }
        layoutWidget(size());
        mainControl->repaint();
    }

    void MainWindow::keyPressEvent(QKeyEvent * event)
    {
        switch(event->key())
        {
            case Qt::Key_F5:
                steeringProvider->steer(ResetSimulation());
                break;
            case Qt::Key_F:
                if(isFullScreen())
                {
                    showNormal();
                }
                else
                {
                    std::cout << "got key ...\n";
                    showFullScreen();
                }
                return;
            default:
                break;
        }
    }
        
    void MainWindow::mousePressEvent(QMouseEvent *event)
    {
        std::cout << "got it!\n";
    }

    void MainWindow::resizeEvent(QResizeEvent *event)
    {
        QSize size = event->size();
        layoutWidget(size);
    }

    void MainWindow::layoutWidget(QSize size)
    {
        QSize forceSize = forceControl->size();
        QSize mainSize = mainControl->size();
        int particleOffset = mainSize.width() + MainControl::OFFSET;

        mainControl->resize(mainSize);
        mainControl->move(0, rect().height()/2 - mainSize.height()/2);

        if(state == MainControl::edit)
        {
            forceControl->resize(forceSize);
            forceControl->move(
                mainSize.width() + MainControl::OFFSET
              , rect().height()/2 - forceSize.height()/2
            );
            if(!forceControl->isVisible())
                forceControl->show();
            particleOffset += forceSize.width() + ForceControl::OFFSET;
        
            forceView->resize(size - QSize(particleOffset, 0));
            forceView->move(
                particleOffset   
              , 0
            );
            forceView->show();
        }
        else
        {
            if(forceControl->isVisible())
                forceControl->hide();
            if(forceView->isVisible())
                forceView->hide();
        }

        if(state == MainControl::picture)
        {
            cameraPreview->resize(size - QSize(particleOffset, 0));
            cameraPreview->move(
                particleOffset   
              , 0
            );
            if(!cameraPreview->isVisible())
            {
                cameraPreview->show();
            }
        }
        else
        {
            if(cameraPreview->isVisible())
            {
                cameraPreview->hide();
            }
        }

        if(state == MainControl::free || state == MainControl::video || state == MainControl::edit)
        {
            particleWidget->resize(size - QSize(particleOffset, 0));

            particleWidget->move(
                particleOffset   
              , 0
            );
            if(!particleWidget->isVisible())
            {
                particleWidget->show();
            }
        }
        else
        {
            if(particleWidget->isVisible())
            {
                particleWidget->hide();
            }
        }

        if(state == MainControl::video || state == MainControl::picture)
        {
            cameraPreview->enableGrab(true);
        }
        if(state != MainControl::video && state != MainControl::picture)
        {
            cameraPreview->enableGrab(false);
        }
    }

    void MainWindow::paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.fillRect(rect(), QBrush(Qt::black));
    }
}
