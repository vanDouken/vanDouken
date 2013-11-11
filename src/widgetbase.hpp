//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_WIDGETBASE_HPP
#define VANDOUKEN_WIDGETBASE_HPP

#include "config.hpp"
#include "steeringprovider.hpp"
#include "forcesteerer.hpp"

#include <libgeodecomp/geometry/coord.h>

#include <QtGui>

#include <iostream>

namespace vandouken {

    template <typename QBase>
    class WidgetBase : public QBase
    {
    public:
        WidgetBase(QWidget *parent,
            LibGeoDecomp::Coord<2> dimensions,
            SteeringProvider *steeringProvider)
          : QBase(parent),
            dimensions(dimensions),
            globalOffset(0, 0, 0),
            steeringProvider(steeringProvider)
        {}

        virtual ~WidgetBase() {}

        QVector2D getModelPos(const QPoint& pos);
    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        
        LibGeoDecomp::Coord<2> dimensions;
        QVector3D globalOffset;
    private:
        virtual void resetProjection() = 0;
        
        SteeringProvider *steeringProvider;
        
        QPoint lastSweepPos;
        QPoint lastPanPos;

    };
   
    template <typename QBase>
    QVector2D WidgetBase<QBase>::getModelPos(const QPoint& pos)
    {
        //double zoomFactor = -globalOffset.z() * 0.01 + 1;
        // account for zooming
        QVector2D center(this->width() * 0.5, this->height() * 0.5);
        QVector2D distanceFromCenter(pos.x() - center.x(),
                                     pos.y() - center.y());

        QVector2D zoomedPos = distanceFromCenter/* * zoomFactor*/ + center;

        // convert screen coordinates to model coordinates
        double factor = 1.0;
        double factorX = static_cast<double>(dimensions.x()) / this->width();// * factor;
        double factorY = static_cast<double>(dimensions.y()) / this->height();// * factor;

        QVector2D modelPos(zoomedPos.x() * factorX,
                           zoomedPos.y() * factorY);

        // account for panning
        factor = 150.0 / 20.0;

        QVector2D panOffset(globalOffset.x() * factor,
                            globalOffset.y() * factor);
        modelPos -= panOffset;

        return modelPos;
    }

    template <typename QBase>
    void WidgetBase<QBase>::mousePressEvent(QMouseEvent *event)
    {
        if (event->buttons() & Qt::LeftButton) {
            std::cout << event->pos().x() <<  " " << event->pos().y() << "\n";
            lastSweepPos = event->pos();// - QPoint(viewport[0], viewport[1]);
        }
        if (event->buttons() & Qt::RightButton) {
            lastPanPos = event->pos();// - QPoint(viewport[0], viewport[1]);
        }
    }
    
    template <typename QBase>
    void WidgetBase<QBase>::mouseReleaseEvent(QMouseEvent *event)
    {
    }
    
    template <typename QBase>
    void WidgetBase<QBase>::mouseMoveEvent(QMouseEvent *event)
    {
        double zoomFactor = -globalOffset.z() * 0.01 + 1;
        if(event->buttons() & Qt::LeftButton) {
            QVector2D modelPos = getModelPos(lastSweepPos);

            QPoint eventPos = event->pos();//- QPoint(viewport[0], viewport[1]);

            QPoint delta = eventPos - lastSweepPos;

            double factorX = static_cast<double>(dimensions.x()) / this->width();
            double factorY = static_cast<double>(dimensions.y()) / this->height();
            QVector2D modelDelta(delta.x() * zoomFactor * factorX,
                                 delta.y() * zoomFactor * factorY);

            std::cout << "add force (" << modelPos.x() << ", " << modelPos.y()
                       << ") -> "       << modelDelta.x() << ", " << modelDelta.y() << "\n";
            steeringProvider->steer(ForceSteerer(modelPos, modelDelta));
            lastSweepPos = eventPos;

        }

        if (event->buttons() & Qt::RightButton) {
            double factor = 0.0403 * zoomFactor;
            double factorX = factor * 1000.0 / this->width();
            double factorY = factor * 500.0 / this->height();

            QPoint eventPos = event->pos();// - QPoint(viewport[0], viewport[1]);
            QPoint delta = eventPos - lastPanPos;

            globalOffset.setX(globalOffset.x() + factorX * delta.x());
            globalOffset.setY(globalOffset.y() + factorY * delta.y());
            // std::cout << "globalOffset(" << globalOffset.x() << ", " << globalOffset.y() << ")\n";
            lastPanPos = eventPos;
            resetProjection();
        }
    }

    template <typename QBase>
    void WidgetBase<QBase>::wheelEvent(QWheelEvent *event)
    {
        double newZ = globalOffset.z() - event->delta() * 0.03;
        if ((newZ < 90) && (newZ > -1000)) {
            globalOffset.setZ(newZ);
            resetProjection();
        }
    }
}

#endif
