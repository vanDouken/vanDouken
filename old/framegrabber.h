#ifndef _libgeodecomp_examples_flowingcanvas_framegrabber_h_
#define _libgeodecomp_examples_flowingcanvas_framegrabber_h_

#include <iostream>
#include <stdexcept>
#include <fpscounter.h>
#include <cv.h>
#include <highgui.h>

class FrameGrabber : public FPSCounter
{
public:
    FrameGrabber(bool _fakeCam, int width, int height);

    ~FrameGrabber();

    void grab(std::vector<char> & data, std::size_t width, std::size_t height);

    void info()
    {
        std::cout << "FrameGrabber @ " << fps() << " FPS\n";
    }

private:
    // ugly hack. we can't include cv.h here since nvcc won't compile it
    CvCapture *capture;
    bool fakeCam;
    std::size_t time;
};

#endif
