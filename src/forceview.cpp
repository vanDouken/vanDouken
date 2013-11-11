//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "forcecontrol.hpp"
#include "forcesteerer.hpp"
#include "forceview.hpp"
#include "particlewidget.hpp"
#include "mainwindow.hpp"
#include "steeringprovider.hpp"

#include <QPainter>
#include <QMouseEvent>

namespace {
    QVector2D getModelDelta(int direction, double force)
    {
        QVector2D modelDelta(0.0, 0.0);
        switch (direction)
        {
            case vandouken::ForceControl::northWest:
                modelDelta.setX(-1.0 * force);
                modelDelta.setY(-1.0 * force);
                break;
            case vandouken::ForceControl::north:
                modelDelta.setX(0.0 * force);
                modelDelta.setY(-1.0 * force);
                break;
            case vandouken::ForceControl::northEast:
                modelDelta.setX(1.0 * force);
                modelDelta.setY(-1.0 * force);
                break;
            case vandouken::ForceControl::east:
                modelDelta.setX(1.0 * force);
                modelDelta.setY(0.0 * force);
                break;
            case vandouken::ForceControl::west:
                modelDelta.setX(-1.0 * force);
                modelDelta.setY(0.0 * force);
                break;
            case vandouken::ForceControl::southEast:
                modelDelta.setX(1.0 * force);
                modelDelta.setY(1.0 * force);
                break;
            case vandouken::ForceControl::south:
                modelDelta.setX(0.0 * force);
                modelDelta.setY(1.0 * force);
                break;
            case vandouken::ForceControl::southWest:
                modelDelta.setX(-1.0 * force);
                modelDelta.setY(1.0 * force);
                break;
        }

        return modelDelta;
    }
}

namespace vandouken {

    ForceView::ForceView(
        ParticleWidget *particleWidget
      , SteeringProvider *steeringProvider
      , MainWindow *parent)
      : QWidget(parent)
      , direction(4)
      , forceValue(0.5)
      , mainWindow(parent)
      , particleWidget(particleWidget)
      , steeringProvider(steeringProvider)
    {
        arrows[ForceControl::northWest].load(
            QString(VANDOUKEN_DATA_DIR "arrowNorthWest.svg"));
        arrows[ForceControl::north].load(
            QString(VANDOUKEN_DATA_DIR "arrowNorth.svg"));
        arrows[ForceControl::northEast].load(
            QString(VANDOUKEN_DATA_DIR "arrowNorthEast.svg"));
        arrows[ForceControl::west].load(
            QString(VANDOUKEN_DATA_DIR "arrowWest.svg"));
        arrows[ForceControl::east].load(
            QString(VANDOUKEN_DATA_DIR "arrowEast.svg"));
        arrows[ForceControl::southWest].load(
            QString(VANDOUKEN_DATA_DIR "arrowSouthWest.svg"));
        arrows[ForceControl::south].load(
            QString(VANDOUKEN_DATA_DIR "arrowSouth.svg"));
        arrows[ForceControl::southEast].load(
            QString(VANDOUKEN_DATA_DIR "arrowSouthEast.svg"));
    }

    void ForceView::mousePressEvent(QMouseEvent *event)
    {
        if(direction == ForceControl::center)
        {
            return;
        }
        if(std::abs(forceValue) <= 1e-7) return;

        ForceArrow forceArrow = {
            event->pos()
          , direction
          , forceValue * 2.0};
        forces.push_back(forceArrow);
    }

    void ForceView::paintEvent(QPaintEvent *)
    {
        if(!isVisible()) return;

        QPainter painter(this);

        if(particleWidget)
        {
            QImage tmp(
                particleWidget->grabFrameBuffer());//.scaled(size(), Qt::KeepAspectRatio));
            painter.drawImage(
                0//size().width()/2 - tmp.size().width()/2
              , 0//size().height()/2 - tmp.size().height()/2
              , tmp);
        }

        BOOST_FOREACH(const ForceArrow& forceArrow, forces)
        {
            double width = forceArrow.force;
            double height = forceArrow.force;
            QPoint pos(forceArrow.pos);
            switch (forceArrow.direction)
            {
                case ForceControl::north:
                case ForceControl::south:
                    width *= 30.0;
                    height *= 45.0;
                    break;
                case ForceControl::east:
                case ForceControl::west:
                    width *= 45.0;
                    height *= 30.0;
                    break;
                default:
                    width *= 35.0;
                    height *= 35.0;
                    break;
            }
            pos.rx() -= width * 0.5;
            pos.ry() -= height * 0.5;

            arrows[forceArrow.direction].render(
                &painter
              , QRectF(
                    pos.x()
                  , pos.y()
                  , width, height
                )
            );
        }
    }

    void ForceView::directionChanged(int d)
    {
        direction = d;

        if(direction == ForceControl::center)
        {
            typedef std::vector<ForceArrow>::iterator iterator;
            for(iterator it = forces.begin(); it != forces.end();)
            {
                QVector2D modelPos(mainWindow->getModelPos(it->pos));
                QVector2D modelDelta(getModelDelta(it->direction, it->force));

                steeringProvider->steer(
                    ForceSteerer(
                        modelPos
                      , modelDelta
                    )
                );

                //Interpolate forces ...
                iterator next = ++it;
                if(next != forces.end())
                {
                    QVector2D nextModelPos(mainWindow->getModelPos(next->pos));
                    
                    /*
                    QVector2D diff(nextModelPos - modelPos);
                    double diffLength = diff.length();
                    if(diffLength >= 5.0)
                    {
                        QVector2D nextModelDelta(
                            getModelDelta(next->direction, next->force));

                        double angle
                            = std::acos(QVector2D::dotProduct(modelDelta, nextModelDelta));

                        std::cout << "\n\n" << angle << "\n\n";

                        for(double d = 0.2; d < 1.0; d += 0.2)
                        {
                            double sn = std::sin(d*angle);
                            double cn = std::cos(d*angle);

                            steeringProvider->steer(
                                ForceSteerer(
                                    modelPos + d * diff
                                  , QVector2D(
                                        modelDelta.x() * cn - modelDelta.y() * sn
                                      , modelDelta.x() * sn - modelDelta.y() * cn
                                    )
                                )
                            );
                        }
                    }
                    */
                }

            }
            forces.clear();
        }
    }

    void ForceView::forceChanged(double f)
    {
        forceValue = f;
    }
}
