//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "gridprovider.hpp"
#include "particlewidget.hpp"

#include <GL/glu.h>

namespace vandouken {
    void ParticleWidget::initializeGL()
    {
        initializeGLFunctions();

        modelview[0] = 1.0;
        modelview[1] = 0.0;
        modelview[2] = 0.0;
        modelview[3] = 0.0;

        modelview[4] = 0.0;
        modelview[5] = 1.0;
        modelview[6] = 0.0;
        modelview[7] = 0.0;

        modelview[8] = 0.0;
        modelview[9] = 0.0;
        modelview[10] = 1.0;
        modelview[11] = 0.0;

        modelview[12] = 0.0;
        modelview[13] = 0.0;
        modelview[14] = 0.0;
        modelview[15] = 1.0;

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);

        QPixmap map(VANDOUKEN_DATA_DIR "/brushstrokes.png");
        texture = bindTexture(map, GL_TEXTURE_2D);

        program = createProgram();
        if(program == 0) return;

        const double scale = 0.4f;
            // 0->1->2, 0->2->3
        /*
        static const float
            coords[] = {
                     scale * +3, scale * -1, scale * -1,
                     scale * -3, scale * -1, scale * -1,
                     scale * -3, scale * +1, scale * -1,
                     scale * +3, scale * +1, scale * -1
                    };
        */

        static const float
            triangle[] = {
                scale * +3, scale * -1, scale * -1,
                scale * -3, scale * -1, scale * -1,
                scale * -3, scale * +1, scale * -1,

                scale * +3, scale * -1, scale * -1,
                scale * -3, scale * +1, scale * -1,
                scale * +3, scale * +1, scale * -1,
            };

        static const float
            triangleTexCoords[] = {
                1.0f, 1.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,

                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
            };

        glGenBuffers(3, buffers);

        coords.reserve(N * 6 * 3);
        texCoords.reserve(N * 6 * 2);
        indices.reserve(N * 6);

        float index = 0.0;
        for(std::size_t i = 0; i < N; ++i)
        {
            for(std::size_t j = 0; j < sizeof(triangle)/sizeof(float); ++j)
            {
                coords.push_back(triangle[j]);
            }

            for(std::size_t j = 0; j < sizeof(triangleTexCoords)/sizeof(float); ++j)
            {
                texCoords.push_back(triangleTexCoords[j]);
            }
            for(std::size_t j = 0; j < 6; ++j)
            {
                indices.push_back(index);
            }
            index += 1.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), &coords[0], GL_STREAM_DRAW);

        GLuint vertexLocation = glGetAttribLocation(program, "vertex");
        glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexLocation);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STREAM_DRAW);

        GLuint texCoordLocation = glGetAttribLocation(program, "texCoord");
        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(texCoordLocation);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
        glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STREAM_DRAW);

        GLuint indexLocation = glGetAttribLocation(program, "index");
        glVertexAttribPointer(indexLocation, 1, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(indexLocation);


        dataLocation = glGetUniformLocation(program, "data");
        matrixLocation = glGetUniformLocation(program, "matrix");
        colorLocation  = glGetUniformLocation(program, "color");
        textureLocation  = glGetUniformLocation(program, "texture");


        glUseProgram(program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1iv(textureLocation, 1, &texture);

        posAngle.reserve(Cell::MAX_PARTICLES * dimensions.prod() * 4);
        color.reserve(Cell::MAX_PARTICLES * dimensions.prod() * 4);
    }

    void ParticleWidget::resizeGL(int width, int height)
    {
        float screenRatio = float(width)/height;
        float worldRatio = float(dimensions.x())/dimensions.y();
        if(worldRatio > screenRatio)
        {
            int viewportHeight = width/worldRatio;
            viewport[0] = 0;
            viewport[1] = (height - viewportHeight)/2;
            viewport[2] = width;
            viewport[3] = viewportHeight;
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        else
        {
            int viewportWidth = height*worldRatio;
            viewport[0] = (width - viewportWidth)/2;
            viewport[1] = 0;
            viewport[2] = viewportWidth;
            viewport[3] = height;
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }


        w = viewport[2] - viewport[0];
        h = viewport[3] - viewport[1];

        resetProjection();
    }

    void ParticleWidget::paintGL()
    {
        if(frames == 0) timer.restart();
        if((frames % 20 == 0))
        {
            std::cout << "View FPS: " << frames/timer.elapsed() << "\n";
        }
        ++frames;
        //counter.incFrames();
        /*
        if ((counter.getFrames() % 20) == 0)
            std::cout << "view FPS: " << counter.fps() << "\n";
        */
        boost::shared_ptr<GridType> newGrid = gridProvider->nextGrid();
        if(newGrid) {
            grid = newGrid;
        }
        if(!grid) return;

        LibGeoDecomp::Coord<2> dimensions = grid->getDimensions();


        qglClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &matrix[0]);

        posAngle.clear();
        color.clear();
        std::size_t particles_size = 0;
        for(int x = 0; x < dimensions.x(); ++x) {
            for(int y = 0; y < dimensions.y(); ++y) {
                LibGeoDecomp::Coord<2> coord(x, y);
                Particles particles = (*grid)[coord];
                posAngle.insert(posAngle.end(), particles.posAngle->begin(), particles.posAngle->end());

                BOOST_FOREACH(boost::uint32_t c, *particles.colors) {
                    color.push_back(qRed  (c));
                    color.push_back(qGreen(c));
                    color.push_back(qBlue (c));
                    color.push_back(qAlpha(c));
                }
                particles_size += particles.size();
            }
        }

        float * data_ptr = &posAngle[0];
        float * color_ptr = &color[0];
        //particles_size = particles_size - (particles_size % N);

        for (std::size_t i = 0; i < particles_size; i = (std::min)(i + N, particles_size)) {
            /*
            std::size_t n = (std::min)(i + N, particles_size);
            std::size_t n = std::min(i + N, particles_size);
            std::size_t n_triangles = n -i;
            */
            const std::size_t n_data = N * 4;

            //LOG("paintGL " << i << " " << n_data << " " << particles->posAngle.size());

            glUniform4fv(dataLocation,  n_data, data_ptr);
            glUniform4fv(colorLocation, n_data, color_ptr);

            glDrawArrays(GL_TRIANGLES, 0, 6 * N);
            data_ptr += n_data;
            color_ptr += n_data;
        }

        // record/capture screen:
        // QPixmap shot = QPixmap::grabWindow(winId());
        // QString fileName = QString("foo%1.png").arg(++frameCounter);
        // std::cout << fileName.toStdString() << "\n";
        // shot.save(fileName);
    }

    void ParticleWidget::keyPressEvent(QKeyEvent * event)
    {
        switch(event->key())
        {
            case Qt::Key_F5:
                //gui.reset();
                break;
            case Qt::Key_F:
                if(isFullScreen())
                {
                    showNormal();
                }
                else
                {
                    std::cout << "got key ...\n";
                    showFullScreen();
                }
                break;
            default:
                break;
        }
    }

    void ParticleWidget::mousePressEvent(QMouseEvent *event)
    {
        if (event->buttons() & Qt::LeftButton) {
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
            gluUnProject(
                event->pos().x()
              , event->pos().y()
              , 0.0
              , modelview
              , mapMatrix.constData()
              , viewport
              , &x
              , &y
              , &z
            );
            lastSweepPos = QVector2D(x, y);
        }
        if (event->buttons() & Qt::RightButton) {
            lastPanPos = event->pos();
        }
    }

    void ParticleWidget::mouseMoveEvent(QMouseEvent *event)
    {
        /*
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;

        if (event->buttons() & Qt::LeftButton) {
            gluUnProject(
                event->pos().x()
              , event->pos().y()
              , 0.0
              , modelview
              , mapMatrix.constData()
              , viewport
              , &x
              , &y
              , &z
            );
            QVector2D modelPos(x, y);

            QVector2D modelDelta(modelPos.x() - lastSweepPos.x(), modelPos.y() - lastSweepPos.y());

            // std::cout << "add force (" << modelPos.x() << ", " << modelPos.y()
            //           << ") -> "       << modelDelta.x() << ", " << modelDelta.y() << "\n";

            gui.forceRecorded(modelPos, modelDelta);
            lastSweepPos = modelPos;
        }
        */

        if (event->buttons() & Qt::RightButton) {
            double zoomFactor = -globalOffset.z() * 0.01 + 1;
            double factor = 0.0403 * zoomFactor;
            double factorX = factor * 1000.0 / width();
            double factorY = factor *  500.0 / height();

            QPoint delta = event->pos() - lastPanPos;

            globalOffset.setX(globalOffset.x() + factorX * delta.x());
            globalOffset.setY(globalOffset.y() + factorY * delta.y());
            // std::cout << "globalOffset(" << globalOffset.x() << ", " << globalOffset.y() << ")\n";
            lastPanPos = event->pos();
            resetProjection();
        }
    }

    void ParticleWidget::wheelEvent(QWheelEvent *event)
    {
        double newZ = globalOffset.z() - event->delta() * 0.03;
        if ((newZ < 90) && (newZ > -1000)) {
            globalOffset.setZ(newZ);
            resetProjection();
        }
    }

    void ParticleWidget::resetProjection()
    {
        QMatrix4x4 pmvMatrix;

        const float ratio = float(h)/w;

        pmvMatrix.setToIdentity();
        pmvMatrix.frustum(-1.0, 1.0, ratio, -ratio, 5, 1000);
        pmvMatrix.translate(
            globalOffset.x(),
            globalOffset.y(),
            globalOffset.z());

        mapMatrix.setToIdentity();
        mapMatrix.ortho(0, dimensions.x(), 0, dimensions.y(), 0, 1);
        mapMatrix.translate(
            globalOffset.x(),
            globalOffset.y(),
            globalOffset.z());

        for(std::size_t i = 0; i < 16; ++i)
        {
            matrix[i] = pmvMatrix.constData()[i];
        }
    }

    GLuint ParticleWidget::createProgram()
    {
        static const char * vertexShaderSrc =
#if !defined(ANDROID) ||  !defined(__ANDROID__)
#else
            "precision mediump float;\n"
#endif
            "attribute vec3 vertex;\n"
            "attribute vec2 texCoord;\n"
            "attribute float index;\n"
            "varying vec2 tex;\n"
            "varying vec4 c;\n"
            "uniform vec4 data[140];\n"
            "uniform vec4 color[140];\n"
            "uniform mat4 matrix;\n"
            "const float fac = 0.01/180.0 * 3.14;\n"
            "void main(void)\n"
            "{\n"
            "    int i = int(index);\n"
            "    float rad_angle = data[i].w * fac;\n"
            "    float cos_angle = cos(rad_angle);\n"
            "    float sin_angle = sin(rad_angle);\n"
            "    mat4 model = mat4(\n"
            "           vec4( cos_angle,  sin_angle, 0.0, 0.0)\n"
            "         , vec4(-sin_angle,  cos_angle, 0.0, 0.0)\n"
            "         , vec4(       0.0,        0.0, 1.0, 0.0)\n"
            "         , vec4(data[i].xyz * 0.01, 1.0));\n"
            "    tex = texCoord;\n"
            "    c = color[i];\n"
            "    gl_Position = (matrix * model) * vec4(vertex, 1.0);\n"
            "}\n"
            ;

        static const char * fragmentShaderSrc =
#if !defined(ANDROID) ||  !defined(__ANDROID__)
#else
            "precision mediump float;\n"
#endif
            "varying vec4 c;\n"
            "varying vec2 tex;\n"
            "uniform sampler2D myTexture;\n"
            "const float fac = 1.0/255.0;\n"
            "void main(void)\n"
            "{\n"
            "    vec4 realColor = c;\n"
            "    vec2 delta = vec2(1.0/300.0, 1.0/4000.0);\n"
            "    vec2 realTex = vec2(tex.x, tex.y * (1.0 / 40.0)) + delta * vec2(0, c.w * 100.0);\n"
            "    realColor.w = 255.0;\n"
            "    vec4 baseColor = (realColor * fac) * texture2D(myTexture, realTex);\n"
            "    if(baseColor.a < 0.9) discard;\n"//baseColor = baseColor * vec4(0.0,1.0,1.0,1.0);\n"
            "    gl_FragColor = baseColor;\n"
            "}\n"
            ;

        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSrc);
        if(vertexShader == 0) return 0;
        GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
        if(fragmentShader == 0) return 0;

        GLuint program = glCreateProgram();

        if(program != 0)
        {
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);
            int linkStatus;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            if(linkStatus != GL_TRUE)
            {
                LOG("Could not link program\n");
                glDeleteProgram(program);
                program = 0;
            }
        }

        return program;
    }

    GLuint ParticleWidget::loadShader(GLenum type, const char * src)
    {
        GLuint shader = glCreateShader(type);
        if (shader != 0) {
            int len = strlen(src);
            glShaderSource(shader, 1, &src, &len);
            glCompileShader(shader);
            int compiled;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            LOG("Shader compile log:\n");
            const int bufLen = 2048;
            char buf[bufLen] = {0};
            glGetShaderInfoLog(shader, 2048, &len, buf);
            std::string str(buf, buf + len);
            LOG(str);

            if (!compiled) {
                LOG("Could not compile shader" << type << "!\n");
                glDeleteShader(shader);
                shader = 0;
            }

        }
        return shader;
    }
}
