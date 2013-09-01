//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "camerapreview.hpp"
#include "imagesteerer.hpp"
#include "steeringprovider.hpp"
#include "maincontrol.hpp"

#include <QPainter>

namespace vandouken {
    CameraPreview::CameraPreview(SteeringProvider *steeringProvider, QWidget *parent)
      : QWidget(parent)
      , steeringProvider(steeringProvider)
      , grabEnabled(false)
      , clear(true)
    {}

    void CameraPreview::grabFrame()
    {
        if(grabEnabled)
        {
            image = frameGrabber.grab();
            if(isVisible())
            {
                repaint();
            }
            else
            {
                steer();
                if(clear) clear = false;
            }
        }
    }

    void CameraPreview::enableGrab(bool b)
    {
        grabEnabled = b;
        clear = grabEnabled;
    }

    void CameraPreview::steer()
    {
        steeringProvider->steer(ImageSteerer(image, clear));
    }

    void CameraPreview::paintEvent(QPaintEvent *event)
    {
        if(!isVisible()) return;

        QPainter painter(this);

        if(image)
        {
            QImage tmp(image->scaled(size(), Qt::KeepAspectRatio));
            painter.drawImage(
                size().width()/2 - tmp.size().width()/2
              , size().height()/2 - tmp.size().height()/2
              , tmp);
        }
    }

    void CameraPreview::resizeEvent(QResizeEvent *event)
    {}

    void CameraPreview::mousePressEvent(QMouseEvent *event)
    {
        if(isVisible())
        {
            Q_EMIT(stateChanged(MainControl::resetAll));
        }
    }
}
