#ifndef _libgeodecomp_examples_flowingcanvas_canvaswriter_h_
#define _libgeodecomp_examples_flowingcanvas_canvaswriter_h_

#include <QtCore/qmath.h>
#include <QImage>
#include <QPainter>
#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/misc/region.h>
#include <canvascell.h>

//#include <flowwidget.h>

/*
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
*/

namespace LibGeoDecomp {

class CanvasWriter// : public QObject
{
    CanvasWriter(int width, int height) :
        width_(width),
        height_(height),
        outputFrame(width, height, QImage::Format_ARGB32)
        //: ParallelWriter<CanvasCell>("foo")
    {}

    int width_;
    int height_;

    typedef Region<2> RegionType;
    typedef GridBase<CanvasCell, 2> GridType;

    virtual void initialized()
    {}

    virtual void stepFinished(GridType const & grid, std::size_t step)
    {
        {
            QPainter p(&outputFrame);
            p.setBrush(QBrush(Qt::red));
            p.drawRect(0, 0, outputFrame.width(), outputFrame.height());
        }
        /*
        switch (flow->cameraMode) {
        case 0:
            drawAttribute(grid, SelectOriginalPixel());
            break;
        case 1:
            drawAttribute(grid, SelectCameraPixel());
            break;
        case 2:
            drawAttribute(grid, SelectCameraLevel());
            break;
        default:
            break;
        }

        switch (flow->particleMode) {
        case 0:
            drawForce(grid, SelectForceVario());
            break;
        case 1:
            drawForce(grid, SelectForceFixed());
            break;
        case 2:
            drawForce(grid, SelectForceTotal());
            break;
        case 3:
            drawParticles(grid);
            break;
        default:
            break;
        }
        flow->updateImage(outputFrame);
        */
        QImage tmp(width_, height_, QImage::Format_ARGB32); tmp.swap(outputFrame);
    }

    virtual void allDone()
    {
    }
    QImage outputFrame;

    //FlowWidget * flow;

//public Q_SLOTS:
/*
    virtual void cycleViewModeParticle()
    {
        particleMode = (particleMode + 1) % 4;
    }

    virtual void cycleViewModeCamera()
    {
        cameraMode = (cameraMode + 1) % 3;
    }
    */

private:

    template<typename SELECTOR>
    void drawForce(GridType const & grid, const SELECTOR& selector)
    {
        // FIXME: get proper dimension ...
        Coord<2> dim(width_, height_);
        int spacingX = 10;
        int spacingY = 10;
        float factorX = 1.0 * outputFrame.width()  / dim.x();
        float factorY = 1.0 * outputFrame.height() / dim.y();

        QPainter p(&outputFrame);
        /*
        p.setBrush(QBrush(Qt::black));
        p.drawRect(0, 0, outputFrame.width(), outputFrame.height());
        */
        p.setBrush(QBrush(Qt::black));
        p.setPen(QPen(Qt::black));

        for(int y = 0; y < dim.y(); y += spacingY)
        {
            for(int x = 0; x < dim.x(); x+= spacingX)
            {
                int startX = (x + 0.5) * factorX;
                int startY = (y + 0.5) * factorY;
                const CanvasCell& cell = grid.at(Coord<2>(x, y));
                float force0 = selector(cell, 0);
                float force1 = selector(cell, 1);
                int offsetX = force0 * spacingX * factorX * 0.8;
                int offsetY = force1 * spacingY * factorY * 0.8;
                int endX = startX + offsetX;
                int endY = startY + offsetY;
                p.drawLine(startX, startY, endX, endY); 
                QRectF rect(endX - spacingX * 0.1, endY - spacingY * 0.1, spacingX * 0.2, spacingY * 0.2);
                p.drawEllipse(rect);
            }
        }
    }

    // fixme: erase background
    // fixme: scale to image size
    template<typename SELECTOR>
    void drawAttribute(GridType const & grid, const SELECTOR& selector)
    {
        // FIXME: get proper dimension ...
        Coord<2> dim(width_, height_);
            
        for(int y = 0; y < dim.y(); ++y)
        {
            for(int x = 0; x < dim.x(); ++x)
            {
                outputFrame.setPixel(x, y, selector(grid.at(Coord<2>(x, y))));
            }
        }
    }

    void drawParticles(GridType const & grid)
    {
        // FIXME: get proper dimension ...
        Coord<2> dim(width_, height_);

        float factorX = 1.0 * outputFrame.width()  / dim.x();
        float factorY = 1.0 * outputFrame.height() / dim.y();

        QPainter p(&outputFrame);
        /*
        p.setBrush(QBrush(Qt::black));
        p.drawRect(0, 0, outputFrame.width(), outputFrame.height());
        */
        p.setBrush(QBrush(Qt::black));
        p.setPen(QPen(Qt::black));

        QRect ellipse(-10, -3, 20, 6);
        p.setPen(QPen(Qt::transparent));
        
        for(int y = 0; y < dim.y(); ++y)
        {
            for(int x = 0; x < dim.x(); ++x)
            {
                const CanvasCell& cell = grid.at(Coord<2>(x, y));
                for (std::size_t i = 0; i < cell.particles.size(); ++i) {
                    const CanvasCell::Particle& particle = cell.particles[i];
                    QPoint origin(particle.pos[0] * factorX,
                                  particle.pos[1] * factorY);

                    // double length = particle.vel[0] * particle.vel[0] + particle.vel[1] * particle.vel[1];
                    // double angle = 0;

                    // if (length > 0) {
                    //     length = qSqrt(length);
                    //     angle = (360.0 / 2.0 / 3.14159) * qAsin(particle.vel[1] / length);
                    // }

                    // p.save();
                    // p.translate(origin);
                    // p.rotate(angle);
                    // p.setBrush(QBrush(particle.color));
                    // p.drawEllipse(ellipse);

                    // p.restore();

                    QPoint direction(particle.vel[0] * 20,
                                     particle.vel[1] * 20);
                    QPoint end = origin + direction;
                    QPoint offset(2, 2);
                    QSize size(4, 4);
                    QRectF rect(origin - offset, size);
                    p.drawEllipse(rect);
                    p.drawLine(origin, end);
                }
            }
        }
    }
};

}

#endif
