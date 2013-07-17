//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "mainwindow.hpp"
#include "particlewidget.hpp"

namespace vandouken {
    MainWindow::MainWindow(
        LibGeoDecomp::Coord<2> dim,
        GridProvider *gridProvider,
        QWidget *parent) :
        QMainWindow(parent)
    {
        content.setupUi(this);
        QLayout *layout = content.visualizationTab->layout();
        ParticleWidget * widget =
            new ParticleWidget(
                gridProvider,
                dim,
                Qt::black,
                content.visualizationTab);

        layout->addWidget(widget);
        QObject::connect(&paintTimer, SIGNAL(timeout()), widget, SLOT(updateGL()));

        paintTimer.start(10);
    }
}
