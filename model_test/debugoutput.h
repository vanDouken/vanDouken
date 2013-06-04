#ifndef _debugoutput_h_
#define _debugoutput_h_

#include <QtCore/QTime>
#include <QtCore/QPoint>
#include <iostream>

class DebugOutput : public QObject
{
    Q_OBJECT
public:
    DebugOutput(int period=20) :
        frames(0),
        lastFrames(0),
        period(period)
    {
        clock.start();
    }
      
public slots:
    void newFrame()
    {
        frames += 1;
        if (frames % period == 0) {
            int ms = clock.restart();
            if (ms < 1) {
                ms = 1;
            }
            int delta = frames - lastFrames;
            double fps = delta * 1000.0 / ms;
            lastFrames = frames;
            std::cout << "fps: " << fps << "\n";
        }
    }

    void addForce(QPoint pos, QPoint force) 
    {
        std::cout << "new force (" << force.x() << ", " << force.y() 
                  << ") at (" << pos.x() << ", " << pos.y() << ")\n";
    }

private:
    int frames;
    int lastFrames;
    int period;
    QTime clock;
};
#endif
