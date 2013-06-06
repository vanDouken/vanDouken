
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/lcos/async.hpp>
#include <hpx/include/actions.hpp>

#include <flowingcanvas_updategroup.hpp>
#include <flowingcanvas_simulator.hpp>
#include <flowingcanvas_collector.hpp>
#include <flowingcanvas_gui.hpp>

#include <hpx/util/high_resolution_timer.hpp>

#include <hpx/android/android.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>

#include <hpiif.hpp>

#include <jni.h>
#include <android/log.h>


#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>

#if !defined(LOG)
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
#endif

/*
static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOG("after %s() glError (0x%x)\n", op, error);
    }
}
*/

detail::hpiif & hpiif()
{
    hpx::util::static_<detail::hpiif> h;
    return h.get();
}

void hpiif_start()
{
    std::string name = "flowingcanvas_simulator";
    hpiif().gui = hpx::components::new_<
        ::server::flowingcanvas_gui
    >(
        hpx::find_here()
     );
    //hpiif().gui.create(hpx::find_here(), 320, 180);
    LOG("starting gui ...");

    hpiif().gui.start(name, false);
    LOG("gui started ...");
}

void new_force(float originX, float originY, float destX, float destY)
{
    hpiif().gui.new_force(originX, originY, destX, destY);
}

void new_image(std::vector<jbyte> const & data, int width, int height)
{
    hpiif().gui.new_image(data, width, height);
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_stellar_hpx_VisualizationRenderer_init(JNIEnv * env, jobject thiz, int texture)
{
    if(!hpiif().flow)
    {
        hpx::android::register_callback<void>(
            "hpiif_start"
          , HPX_STD_BIND(
                hpiif_start
            )
        );
        hpx::android::register_callback<void(float, float, float, float)>(
            "new_force"
          , HPX_STD_BIND(
                new_force
              , HPX_STD_PLACEHOLDERS::_1
              , HPX_STD_PLACEHOLDERS::_2
              , HPX_STD_PLACEHOLDERS::_3
              , HPX_STD_PLACEHOLDERS::_4
            )
        );
        hpx::android::register_callback<void(std::vector<jbyte> const &, int, int)>(
            "new_image"
          , HPX_STD_BIND(
                new_image
              , HPX_STD_PLACEHOLDERS::_1
              , HPX_STD_PLACEHOLDERS::_2
              , HPX_STD_PLACEHOLDERS::_3
            )
        );
        hpx::android::new_action("hpiif_start");
        while(!hpiif().flow)
        {
            sleep(1);
        }
        hpiif().flow->initializeGL(texture);
    }
}

JNIEXPORT void JNICALL Java_com_stellar_hpx_VisualizationRenderer_changed(JNIEnv * env, jobject thiz, jint width, jint height, jfloatArray jmat)
{
    while(!hpiif().flow)
    {
        sleep(1);
    }
    float mat[16] = {0.0f};
    jfloat * data = env->GetFloatArrayElements(jmat, NULL);
    if(data)
    {
        std::copy(data, data + 16, mat);
        hpiif().flow->resizeGL(width, height, mat);
        env->ReleaseFloatArrayElements(jmat, data, JNI_ABORT);
    }
}
JNIEXPORT void JNICALL Java_com_stellar_hpx_VisualizationRenderer_renderParticles(JNIEnv * env, jobject thiz)
{
    if(!hpiif().flow)
        return;
    /*
    while(!hpiif().flow)
    {
        sleep(1);
    }
    */
    hpiif().flow->paintGL();
}

JNIEXPORT void JNICALL
Java_com_stellar_hpx_VisualizationRenderer_newForce(JNIEnv * env, jobject thiz, float originX, float originY, float destX, float destY)
{
    if(!hpiif().flow)
        return;
    hpx::android::new_action("new_force", originX, originY, destX, destY);
}

#ifdef __cplusplus
}
#endif
