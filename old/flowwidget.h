#ifndef _libgeodecomp_examples_flowingcanvas_flowwidget_h_
#define _libgeodecomp_examples_flowingcanvas_flowwidget_h_

#include <iostream>
#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QKeyEvent>
#include <QPainter>
#include <QWidget>
#include <QMutex>
#include <QtOpenGL/QGLWidget>
#include <QVector3D>
#include <QDebug>
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#endif
#include <flowingcanvas_gui.hpp>
#include <libgeodecomp/misc/grid.h>

#include <grid_storage.hpp>
#include <canvascell.h>

#if !defined(__ANDROID__) || !defined(ANDROID)
#define LOG(x...) std::cout << x;
#else
#define LOG(x...)                                                               \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "hpiif_android", "%s", sstr.str().c_str());            \
}                                                                               \
/**/
#endif

class FlowWidget
#if !defined(__ANDROID__) || !defined(ANDROID)
: public QGLWidget//, FPSCounter
{
    Q_OBJECT
#else
{
#endif

public:
    FlowWidget(int width, int height, grid_storage & g
#if !defined(__ANDROID__) || !defined(ANDROID)
            , QWidget *parent =0) :
        QGLWidget(parent)
      , w(width)
      , h(height)
      , gui(g)

    {
        setFocusPolicy(Qt::StrongFocus);
    }

    QSize minimumSizeHint() const
    {
        return QSize(320, 180);
    }

    QSize sizeHint() const
    {
        return QSize(320, 180);
    }
#else
        ) : 
        w(width)
      , h(height)
      , gui(g)
    {}
#endif

#if !defined(__ANDROID__) || !defined(ANDROID)
protected:
#endif

    void initializeGL()
    {
        LOG("OGL initialized ...\n");
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }

    void paintGL()
    {
        typedef LibGeoDecomp::Grid<LibGeoDecomp::CanvasCell> grid_type;
        boost::shared_ptr<grid_type> grid = gui.next_grid();

        if(!grid) return;

        LOG("printing new grid\n!");

#if !defined(__ANDROID__) || !defined(ANDROID)
        qglClearColor(Qt::black);
#endif
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        const float coords[] = {
            // Left Bottom triangle
            -0.5f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            // Right top triangle
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f
        };

        glLoadIdentity();
        glVertexPointer(3, GL_FLOAT, 12, coords);
        glEnableClientState(GL_VERTEX_ARRAY);

        //std::cout << vertices.size() << "\n";


        for(int y = 0; y < grid->getDimensions().y(); ++y)
        {
            for(int x = 0; x < grid->getDimensions().x(); ++x)
            {
                //float z = -30 + (y * w + x) * 0.000001;
                float z = 0.0f;
                LibGeoDecomp::CanvasCell const & c = (*grid)[LibGeoDecomp::Coord<2>(x, y)];
                glLoadIdentity();
                glTranslatef(x, y, z);
                glColor4ub(red(c.backgroundPixel), green(c.backgroundPixel), blue(c.backgroundPixel), 255);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);

                for (int i = 0; i < c.numParticles; ++i) {
                    glLoadIdentity();
                    LibGeoDecomp::CanvasCell::Particle const & p = c.particles[i];
                    glTranslatef(p.pos[0], p.pos[1], 1.0);
                    // fixme: hardcode alpha or deduce it from z
                    glColor4ub(red(p.color), green(p.color), blue(p.color), 127);
                    
                    
                    // fixme: use alpha here!
                    for (int j = 0; j < 1; ++j) {
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                    }
                }
            }
        }
    }

    void resizeGL(int width, int height)
    {

        // int side = qMin(width, height);
        // glViewport((width - side) / 2, (height - side) / 2, side, side);

        glViewport(0, 0, width, height);

        /*
        float width_ratio = (float)width / w;
        float height_ratio = (float)height / h;
        */
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //glFrustum(-ratio, ratio, 0.5, -0.5, 5, 1500);
#if !defined(__ANDROID__) || !defined(ANDROID)
        glOrtho(w, 0.0f, h, 0.0f, -10.0f, 10.0f);
#else
        glOrthox(w, 0.0f, h, 0.0f, -10.0f, 10.0f);
#endif
        //glTranslatef(0.0f, 0.0f, 4.0f);
        //glScalef(width_ratio, height_ratio, 1.0f);
        glMatrixMode(GL_MODELVIEW);
    }

private:
    int w;
    int h;
    // std::vector<float> vertices;
    grid_storage & gui;
};

#endif
