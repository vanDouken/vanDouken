//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_IMAGEWIDGET_HPP
#define VANDOUKEN_IMAGEWIDGET_HPP

#include "config.hpp"
#include "widgetbase.hpp"

#include <hpx/lcos/local/spinlock.hpp>

#include <libgeodecomp/geometry/coord.h>

#include <boost/shared_ptr.hpp>

#include <QWidget>

namespace vandouken {

    struct ImageWidget : WidgetBase<QWidget>
    {
        Q_OBJECT

        typedef WidgetBase<QWidget> BaseType;
    public:

        ImageWidget(QWidget *parent,
            SteeringProvider *steeringProvider,
            LibGeoDecomp::Coord<2> dimensions);

        void mousePressEvent(QMouseEvent *event);

        void setImage(QImage img);

        void paintEvent(QPaintEvent *);

        void resetOnClick(bool, bool);
    
    Q_SIGNALS:
        void stateChanged(int, bool);

    private:
        typedef hpx::lcos::local::spinlock MutexType;
        MutexType mutex;
        QImage image;
        bool reset;
        bool resetImage;

        void resetProjection() {}
    };
}

#endif

