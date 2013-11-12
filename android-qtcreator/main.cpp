#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "src/controlgui.hpp"

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


int main(int argc, char *argv[])
{
    try {
        controlGUI(argc, argv);
    }
    catch(std::exception e)
    {
        //LOG(e.what() << std::string(" ...\n");
        __android_log_print(ANDROID_LOG_INFO, "vandouken", e.what());
    }
}
