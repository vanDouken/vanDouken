//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_PARTICLEWIDGET_HPP
#define VANDOUKEN_PARTICLEWIDGET_HPP

#include "particle.hpp"

#include <libgeodecomp/misc/coord.h>
#include <libgeodecomp/misc/grid.h>

#include <boost/shared_ptr.hpp>

#include <QtGui>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>

#ifndef LOG
#if !defined(__ANDROID__) || !defined(ANDROID)
#define LOG(x,...) std::cout << x;
#else
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

    class ParticleWidget
        : public QGLWidget, protected QGLFunctions
    {
        Q_OBJECT
    public:
        typedef LibGeoDecomp::Grid<Particles> GridType;
        static const std::size_t N = 140;

        ParticleWidget(
            GridProvider *gridProvider,
            LibGeoDecomp::Coord<2> dimensions,
            QColor backgroundColor = Qt::black, QWidget *parent = 0) :
            QGLWidget(parent),
            gridProvider(gridProvider),
            dimensions(dimensions),
            backgroundColor(backgroundColor),
            globalOffset(0, 0, 0),
            //frameCounter(0),
            colors(N*4)
        {}

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
        void keyPressEvent(QKeyEvent * event);
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        void paintGL();

    private:
        boost::shared_ptr<GridType> grid;
        GridProvider *gridProvider;

        LibGeoDecomp::Coord<2> dimensions;
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

        QMatrix4x4 mapMatrix;
        double modelview[16];
        int viewport[4];
        QColor backgroundColor;
        QVector2D lastSweepPos;
        QPoint lastPanPos;

        QVector3D globalOffset;

        int h;
        int w;

        std::vector<float> colors;

        LibGeoDecomp::SuperVector<float> posAngle;
        LibGeoDecomp::SuperVector<float> color;
        void resetProjection();
        GLuint createProgram();
        GLuint loadShader(GLenum type, const char * src);
    };

}

#endif
