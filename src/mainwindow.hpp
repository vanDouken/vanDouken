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
    class ParticleWidget;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT;

    public:
        MainWindow(
            LibGeoDecomp::Coord<2> dim,
            GridProvider *gridProvider,
            QWidget *parent=0);

    protected:
        void keyPressEvent(QKeyEvent * event);

    private:
        Ui_MainWindow content;
        QTimer paintTimer;
        ParticleWidget * particleWidget;
    };
}

#endif
