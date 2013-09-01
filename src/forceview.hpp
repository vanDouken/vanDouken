//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCEVIEW_HPP
#define VANDOUKEN_FORCEVIEW_HPP

#include "config.hpp"

#include <QWidget>
#include <QtSvg/QSvgRenderer>

namespace vandouken {
    class ParticleWidget;
    class SteeringProvider;

    struct ForceView : QWidget
    {
        Q_OBJECT
    public:

        ForceView(
            ParticleWidget *particleWidget
          , SteeringProvider *steeringProvider
          , QWidget *parent);

        void mousePressEvent(QMouseEvent *event);

        void paintEvent(QPaintEvent *);

    public Q_SLOTS:
        void directionChanged(int);
        void forceChanged(double);

    private:
        QImage image;
        int direction;
        double forceValue;
        QSvgRenderer arrows[9];

        ParticleWidget *particleWidget;
        SteeringProvider *steeringProvider;

        struct ForceArrow
        {
            QPoint pos;
            int direction;
            double force;
        };
        std::vector<ForceArrow> forces;
    };
}

#endif

