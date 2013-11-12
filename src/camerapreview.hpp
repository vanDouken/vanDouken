//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_CAMERA_PREVIEW_HPP
#define VANDOUKEN_CAMERA_PREVIEW_HPP

#if !defined(__MIC)

#include "config.hpp"
#include "framegrabber.hpp"

#include <QWidget>
#include <QImage>

namespace vandouken {
    class SteeringProvider;

    class CameraPreview : public QWidget
    {
        Q_OBJECT
    public:
        CameraPreview(SteeringProvider *steeringProvider, QWidget *parent);

        void paintEvent(QPaintEvent *);
        void resizeEvent(QResizeEvent *);
        void mousePressEvent(QMouseEvent * event);
        void enableGrab(bool, bool);
        void steer();

    public Q_SLOTS:
        void grabFrame();

    Q_SIGNALS:
        void stateChanged(int, bool);
        void setImage(QImage);

    private:
        boost::shared_ptr<QImage> image;
        FrameGrabber frameGrabber;
        SteeringProvider *steeringProvider;
        bool grabEnabled;
        bool clear;
        bool doSteer;
    };
}

#endif
#endif
