//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCECONTROL_HPP
#define VANDOUKEN_FORCECONTROL_HPP

#if !defined(__MIC)

//#include <QtGui/QWidget>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
//#include <QtGui/QApplication>
#include <QtWidgets/QApplication>
#include <QtGui/QMouseEvent>

#include <bitset>

namespace vandouken {
    struct ForceControl : QWidget
    {
        Q_OBJECT
    public:

        ForceControl(QWidget *parent);

        static const int OFFSET = 25;
        static const int MARGIN = 25;
        static const int LENGTH = 100;
        
        QSize size();

        void mousePressEvent(QMouseEvent * event);

        void mouseMoveEvent(QMouseEvent * event);

        void setSliderPos(int x, int y);

        void paintEvent(QPaintEvent *);

        enum Direction {
            northWest,
            north,
            northEast,
            west,
            center,
            east,
            southWest,
            south,
            southEast,
        };
    
    Q_SIGNALS:
        void directionChanged(int);
        void forceChanged(double);

    private:

        QSvgRenderer boxes[9];
        QSvgRenderer boxesSelected[9];
        QSvgRenderer background;
        QSvgRenderer sliderLine;
        QSvgRenderer sliderSelect;
        double sliderLength;
        double sliderPos;
        std::bitset<9> selected;
    };
}

#endif
#endif
