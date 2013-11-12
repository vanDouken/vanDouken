//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#if !defined(__MIC)

#include "imagewidget.hpp"
#include "maincontrol.hpp"

#include <QPainter>
#include <QMouseEvent>

#include <iostream>

#ifndef LOG
#if !defined(ANDROID)
#define LOG(x,...) std::cout << x;
#else
#include <android/log.h>
#define LOG(x,...)                                                               \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "vandouken", "%s", sstr.str().c_str());            \
}                                                                               \
/**/
#endif
#endif

namespace vandouken
{
    ImageWidget::ImageWidget(QWidget *parent,
        SteeringProvider *steeringProvider,
        LibGeoDecomp::Coord<2> dimensions)
      : BaseType(parent, dimensions, steeringProvider)
      , reset(false)
      , resetImage(false)
    {
        LOG("constructed ...\n");
    }

    void ImageWidget::mousePressEvent(QMouseEvent *event)
    {
        if(reset)
        {
            Q_EMIT(stateChanged(MainControl::resetAll, resetImage));
            reset = false;
        }
        else
        {
            BaseType::mousePressEvent(event);
        }
    }

    void ImageWidget::resetOnClick(bool r, bool resImage)
    {
        reset = r;
        resetImage = resImage;
    }
        
    void ImageWidget::setImage(QImage img)
    {
        MutexType::scoped_lock lock(mutex);
        image = img;
    }

    void ImageWidget::paintEvent(QPaintEvent *event)
    {
        MutexType::scoped_lock lock(mutex);
        if(!isVisible() || image.isNull()) return;
        QPainter painter(this);

        QImage tmp = image.scaled(width(), height(), Qt::KeepAspectRatio);
        painter.drawImage(
            width()/ 2.0 - tmp.width() / 2.0
          , height()/ 2.0 - tmp.height() / 2.0
          , tmp
        );
    }
}

#endif
