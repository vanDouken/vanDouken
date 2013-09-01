//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_MAINWINDOW_HPP
#define VANDOUKEN_MAINWINDOW_HPP

#include "ui_main.h"

#include <libgeodecomp/misc/coord.h>

#include <QMainWindow>
#include <QTimer>

namespace vandouken {
    class GridProvider;
    class SteeringProvider;
    class ParticleWidget;
    class MainControl;
    class ForceControl;
    class ForceView;
    class CameraPreview;

    class MainWindow : public QWidget
    {
        Q_OBJECT;

    public:
        MainWindow(
            LibGeoDecomp::Coord<2> dim,
            GridProvider *gridProvider,
            SteeringProvider *steeringProvider,
            QWidget *parent=0);

    public Q_SLOTS:
        void stateChanged(int);

    protected:
        void keyPressEvent(QKeyEvent * event);
        void mousePressEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *event);
        void paintEvent(QPaintEvent *);

    private:

        void layoutWidget(QSize size);

        Ui_MainWindow content;
        QTimer paintTimer;
        QTimer grabTimer;
        MainControl *mainControl;
        ForceControl *forceControl;
        ForceView *forceView;
        CameraPreview *cameraPreview;
        ParticleWidget * particleWidget;
        SteeringProvider *steeringProvider;

        int state;
    };
}

#endif
