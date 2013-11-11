//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "steeringprovider.hpp"
#include "resetsimulation.hpp"
#include "mainwindowserver.hpp"
#include "mainwindow.hpp"
#include "maincontrol.hpp"
#include "forcecontrol.hpp"
#include "forceview.hpp"
#include "camerapreview.hpp"
#include "particlewidget.hpp"
#include "imagewidget.hpp"

#include <hpx/hpx.hpp>
#include <hpx/hpx.hpp>

#include <QResizeEvent>

namespace vandouken {
    MainWindow::MainWindow(
        LibGeoDecomp::Coord<2> dim,
        GridProvider *gridProvider,
        SteeringProvider *steeringProvider,
        Mode guiMode,
        QWidget *parent) :
        QWidget(parent),
        dim(dim),
        mainControl(0),
        forceControl(0),
        forceView(0),
        cameraPreview(0),
        particleWidget(0),
        steeringProvider(steeringProvider),
        imageWidget(0),
        guiMode(guiMode),
        state(MainControl::free)
    {
        if(gridProvider)
        {
            particleWidget =
                new ParticleWidget(
                    gridProvider,
                    steeringProvider,
                    dim,
                    Qt::black,
                    this);
        }

        if(guiMode == Mode::picturesOnly)
        {
            imageWidget = new ImageWidget(this,
                    steeringProvider,
                    dim
                    );

            QObject::connect(
                imageWidget, SIGNAL(stateChanged(int, bool))
              , this, SLOT(stateChanged(int, bool))
            );
        }

        if(guiMode != Mode::control)
        {
            mainControl = new MainControl(this);
            forceControl = new ForceControl(this);
            forceControl->hide();
            forceView =
                new ForceView(
                    particleWidget
                  , steeringProvider
                  , this);

            forceView->hide();
        }
        cameraPreview = new CameraPreview(steeringProvider, this);
        cameraPreview->hide();

        if(particleWidget)
        {
            QObject::connect(&paintTimer, SIGNAL(timeout()), particleWidget, SLOT(updateGL()));
        }

        QObject::connect(&grabTimer, SIGNAL(timeout()), cameraPreview, SLOT(grabFrame()));

        if(guiMode != Mode::picturesOnly)
        {
            QObject::connect(cameraPreview, SIGNAL(setImage(QImage)), this, SLOT(setImage(QImage)));
        }


        if(guiMode != Mode::control)
        {
            QObject::connect(&paintTimer, SIGNAL(timeout()), forceView, SLOT(repaint()));
            QObject::connect(
                forceControl, SIGNAL(directionChanged(int))
              , forceView, SLOT(directionChanged(int))
            );

            QObject::connect(
                forceControl, SIGNAL(forceChanged(double))
              , forceView, SLOT(forceChanged(double))
            );
        }

        if(mainControl)
        {
            QObject::connect(
                mainControl, SIGNAL(stateChanged(int, bool))
              , this, SLOT(stateChanged(int, bool))
            );
        }

        if(cameraPreview)
        {
            QObject::connect(
                cameraPreview, SIGNAL(stateChanged(int, bool))
              , this, SLOT(stateChanged(int, bool))
            );
        }

        paintTimer.start(20);
        grabTimer.start(20);

        showFullScreen();
        resetImage();
    }

    void MainWindow::stateChanged(int s, bool callResetImage)
    {
        if(!(guiMode & Mode::control) && serverId)
        {
            hpx::apply<MainWindowServer::StateChangedAction>(serverId, s, callResetImage);
        }

        if(s == MainControl::resetAll)
        {
            if(callResetImage)
                resetImage();

            switch(state)
            {
                case MainControl::free:
                    if(guiMode == Mode::control)
                    {
                        steeringProvider->steer(ResetSimulation());
                    }
                    else
                    {
                        mainControl->state = MainControl::free;
                    }
                    state = MainControl::free;
                    break;
                case MainControl::edit:
                    if(guiMode == Mode::control)
                    {
                        steeringProvider->steer(ResetSimulation());
                    }
                    else
                    {
                        mainControl->state = MainControl::edit;
                    }
                    state = MainControl::edit;
                    break;
                case MainControl::picture:
                    if(guiMode == Mode::control)
                    {
                        std::cout << "should steer camera picture\n";
                        cameraPreview->steer();
                        std::cout << "steering done\n";
                    }
                    else
                    {
                        mainControl->state = MainControl::free;
                    }
                    state = MainControl::free;
                    break;
                case MainControl::video:
                    if(guiMode == Mode::control)
                    {
                        cameraPreview->enableGrab(false, false);
                        steeringProvider->steer(ResetSimulation());
                    }
                    else
                    {
                        mainControl->state = MainControl::free;
                    }
                    state = MainControl::free;
                    break;
            }
        }
        else
        {
            state = s;
        }
        layoutWidget(size());
        if(guiMode != Mode::control)
        {
            mainControl->repaint();
        }
    }

    QVector2D MainWindow::getModelPos(const QPoint& pos)
    {
        if(particleWidget) return particleWidget->getModelPos(pos);
        return imageWidget->getModelPos(pos);
    }

    void MainWindow::setImage(QImage img)
    {
        if(guiMode & Mode::picturesOnly)
        {
            imageWidget->setImage(img);
            imageWidget->repaint();
        }
        else
        {
            MutexType::scoped_lock lock(mutex);
            image = img;
        }
    }

    void MainWindow::keyPressEvent(QKeyEvent * event)
    {
        switch(event->key())
        {
            case Qt::Key_F5:
                steeringProvider->steer(ResetSimulation());
                resetImage();
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

    QImage MainWindow::getImage()
    {
        MutexType::scoped_lock lock(mutex);
        if(image.isNull())
        {
            return image;
        }
        std::cout << "getting new image\n";
        QImage res = image;
        image = QImage();
        return res;
    }

    void MainWindow::resetImage()
    {
        if(guiMode != picturesOnly)
        {
            MutexType::scoped_lock lock(mutex);
            const char * filename = VANDOUKEN_DATA_DIR VANDOUKEN_INITIALIZER_IMG;
            std::cout << "resetting image " << filename << "\n";
            std::cout << image.isNull() << "\n";
            image.load(QString(filename));
            std::cout << image.isNull() << "\n";
        }
    }

    void MainWindow::layoutWidget(QSize size)
    {
        if(guiMode == Mode::control)
        {
            if(particleWidget)
            {
                particleWidget->resize(size);

                if(!particleWidget->isVisible())
                {
                    particleWidget->show();
                }
            }

            if(state == MainControl::video || state == MainControl::picture)
            {
                cameraPreview->enableGrab(true, state == MainControl::video);
            }
            if(state != MainControl::video && state != MainControl::picture)
            {
                cameraPreview->enableGrab(false, false);
            }
            return;
        }

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
            std::cout << "showing force view ...\n";
        }
        else
        {
            if(forceControl->isVisible())
                forceControl->hide();
            if(forceView->isVisible())
                forceView->hide();
        }

        if(guiMode == Mode::picturesOnly)
        {
            if(imageWidget)
            {
                imageWidget->resize(size - QSize(particleOffset, 0));

                imageWidget->move(
                    particleOffset   
                  , 0
                );
                if(!imageWidget->isVisible())
                {
                    imageWidget->show();
                }

                if(state == MainControl::picture)
                {
                    imageWidget->resetOnClick(true, false);
                }
            }
        }
        else
        {
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
                if(particleWidget)
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
            }
            else
            {
                if(particleWidget && particleWidget->isVisible())
                {
                    particleWidget->hide();
                }
            }

            if(state == MainControl::video || state == MainControl::picture)
            {
                cameraPreview->enableGrab(true, state == MainControl::video);
            }
            if(state != MainControl::video && state != MainControl::picture)
            {
                cameraPreview->enableGrab(false, false);
            }
        }
    }

    void MainWindow::paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.fillRect(rect(), QBrush(Qt::black));
    }
}
