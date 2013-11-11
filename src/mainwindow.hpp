//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_MAINWINDOW_HPP
#define VANDOUKEN_MAINWINDOW_HPP

#include "ui_main.h"

#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/naming/id_type.hpp>
#include <hpx/lcos/local/spinlock.hpp>

#include <libgeodecomp/geometry/coord.h>

#include <QMainWindow>
#include <QTimer>

namespace vandouken {
    class GridProvider;
    class SteeringProvider;
    class ParticleWidget;
    class ImageWidget;
    class MainControl;
    class ForceControl;
    class ForceView;
    class CameraPreview;

    class MainWindow : public QWidget
    {
        Q_OBJECT;

    public:

        enum Mode
        {
            default_ = 0x00,
            control = 0x01,
            showControls = 0x02,
            picturesOnly = 0x04
        };

        MainWindow(
            LibGeoDecomp::Coord<2> dim,
            GridProvider *gridProvider,
            SteeringProvider *steeringProvider,
            Mode guiMode,
            QWidget *parent=0);
        
        template <typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned) { BOOST_ASSERT(false); }
        MainWindow() { BOOST_ASSERT(false); }

        void setServerId(hpx::id_type id) { serverId = id; }

        QImage getImage();
        
        QVector2D getModelPos(const QPoint& pos);

    public Q_SLOTS:
        void stateChanged(int, bool);
        void setImage(QImage);

    protected:
        void keyPressEvent(QKeyEvent * event);
        void mousePressEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *event);
        void paintEvent(QPaintEvent *);

    private:
        typedef hpx::lcos::local::spinlock MutexType;
        MutexType mutex;

        void resetImage();
        void layoutWidget(QSize size);

        LibGeoDecomp::Coord<2> dim;
        Ui_MainWindow content;
        QTimer paintTimer;
        QTimer grabTimer;
        MainControl *mainControl;
        ForceControl *forceControl;
        ForceView *forceView;
        CameraPreview *cameraPreview;
        ParticleWidget * particleWidget;
        SteeringProvider *steeringProvider;
        ImageWidget * imageWidget;
        
        Mode guiMode;
        QImage image;
        hpx::id_type serverId;

        int state;

    };
}

#endif
