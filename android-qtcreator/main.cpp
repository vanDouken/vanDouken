#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "src/controlgui.hpp"


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
