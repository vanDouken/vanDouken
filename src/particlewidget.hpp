//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_PARTICLEWIDGET_HPP
#define VANDOUKEN_PARTICLEWIDGET_HPP

#include "config.hpp"
#include "particle.hpp"
#include "widgetbase.hpp"

#include <libgeodecomp/geometry/coord.h>
#include <libgeodecomp/storage/grid.h>

#include <boost/shared_ptr.hpp>

#include <hpx/util/high_resolution_timer.hpp>
#include <hpx/util/spinlock.hpp>

#include <QtGui>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>


#ifndef LOG
#if !defined(ANDROID)
#define LOG(x,...) std::cout << x;
#else
#include <android/log.h>
#define LOG(x,...)                                                               \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "hpiif_android", "%s", sstr.str().c_str());            \
}                                                                               \
/**/
#endif
#endif

namespace vandouken {
    class GridProvider;
    class SteeringProvider;

    class ParticleWidget
        : public WidgetBase<QGLWidget>, protected QGLFunctions
    {
        Q_OBJECT

        typedef WidgetBase<QGLWidget> BaseType;
    public:
        static const std::size_t N = 240;

        ParticleWidget(
            GridProvider *gridProvider,
            SteeringProvider *steeringProvider,
            LibGeoDecomp::Coord<2> dimensions,
            QColor backgroundColor = Qt::black, QWidget *parent = 0) :
            BaseType(parent, dimensions, steeringProvider),
            recordedForces(new std::vector<std::pair<QVector2D, QVector2D> >()),
            gridProvider(gridProvider),
            backgroundColor(backgroundColor),
            frames(0)
        {
        }

        ~ParticleWidget()
        {}

        QSize minimumSizeHint() const
        {
            return QSize(dimensions.x(), dimensions.y());
        }

        QSize sizeHint() const
        {
            return QSize(dimensions.x(), dimensions.y());
        }

        void initializeGL();

    protected:
        void resizeGL(int width, int height);
        void paintEvent(QPaintEvent*);
        void paintGL();

    private:
        boost::shared_ptr<std::vector<std::pair<QVector2D, QVector2D> > >
            recordedForces;
        boost::shared_ptr<Particles> particles;
        GridProvider *gridProvider;

        boost::array<float, 16> matrix;
        GLuint program;
        GLint texture;

        std::vector<float> coords;
        std::vector<float> texCoords;
        std::vector<float> indices;
        GLuint dataLocation;
        GLuint matrixLocation;
        GLuint colorLocation;
        GLuint textureLocation;
        GLuint buffers[3];

        double modelview[16];
        int viewport[4];
        QColor backgroundColor;

        int h;
        int w;

        std::size_t frames;
        hpx::util::high_resolution_timer timer;
        hpx::util::spinlock mutex;

        void resetProjection();
        GLuint createProgram();
        GLuint loadShader(GLenum type, const char * src);
    };

}

#endif
