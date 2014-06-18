//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(__MIC)

#include "gridprovider.hpp"
#include "steeringprovider.hpp"
#include "particlewidget.hpp"
#include "forcesteerer.hpp"

#if !defined(ANDROID)
#include <GL/glu.h>
#endif

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

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);

        QPixmap map(VANDOUKEN_DATA_DIR "/brushstrokes.png");
        texture = bindTexture(map, GL_TEXTURE_2D);

        program = createProgram();
        if(program == 0) return;

        const float scale = 0.4f;
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
    }

    void ParticleWidget::resizeGL(int width, int height)
    {
        float screenRatio = float(width)/(height);
        float worldRatio = float(dimensions.x())/dimensions.y();
        if(worldRatio > screenRatio)
        {
            int viewportHeight = width/worldRatio;
            viewport[0] = 0;
            viewport[1] = (height - viewportHeight)/2.0;
            viewport[2] = width;
            viewport[3] = viewportHeight;
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        else
        {
            int viewportWidth = height*worldRatio;
            viewport[0] = (width - viewportWidth)/2.0;
            viewport[1] = 0;
            viewport[2] = viewportWidth;
            viewport[3] = height;
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }

        std::cout
            << dimensions.x() << " "
            << dimensions.y() << " "
            << "\n";

        w = viewport[2] - viewport[0];
        h = viewport[3] - viewport[1];

        resetProjection();
    }

    void ParticleWidget::paintEvent(QPaintEvent *event)
    {
        hpx::util::spinlock::scoped_lock lk(mutex);
        QGLWidget::paintEvent(event);
    }

    void ParticleWidget::paintGL()
    {
        if(frames == 0) timer.restart();
        if((frames % 20 == 0))
        {
            std::cout << "View FPS: " << frames/timer.elapsed() << "\n";
        }
        ++frames;

        boost::shared_ptr<Particles> newParticles = gridProvider->nextGrid();
        if(newParticles && newParticles->size()) {
            particles = newParticles;
        }
        if(!particles) return;

        qglClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &matrix[0]);

        float * data_ptr = &particles->posAngle[0];
        float * color_ptr = &particles->colors[0];

        for (std::size_t i = 0; i < particles->size(); i += N) {
            std::size_t n = 0;
            if(i + N > particles->size() - 1)
            {
                n = (particles->size() - i);
            }
            else
            {
                n = N;
            }
            const std::size_t n_data = n * 4;

            glUniform4fv(dataLocation,  n_data, data_ptr);
            glUniform4fv(colorLocation, n_data, color_ptr);

            glDrawArrays(GL_TRIANGLES, 0, 6 * n);
            data_ptr += n_data;
            color_ptr += n_data;
        }
    }

    void ParticleWidget::resetProjection()
    {
        QMatrix4x4 pmvMatrix;

        pmvMatrix.setToIdentity();
        // FIXME: where do these magic numbers come from?
        pmvMatrix.ortho(
            -0.068f * dimensions.x(),
            0.068f * dimensions.x(),
            0.068f * dimensions.y(),
            -0.068f * dimensions.y(),
            -100.f, 280.f);
        pmvMatrix.translate(
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
            "uniform vec4 data[240];\n"
            "uniform vec4 color[240];\n"
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
            "    if(baseColor.a < 0.9) discard;\n"
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
                MSG("Could not link program\n");
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
            MSG("Shader compile log:\n");
            const int bufLen = 2048;
            char buf[bufLen] = {0};
            glGetShaderInfoLog(shader, 2048, &len, buf);
            std::string str(buf, buf + len);
            MSG(str);

            if (!compiled) {
                MSG("Could not compile shader" << type << "!\n");
                glDeleteShader(shader);
                shader = 0;
            }

        }
        return shader;
    }
}

#endif
