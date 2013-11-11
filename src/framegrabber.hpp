#ifndef VANDOUKEN_FRAMEGRABBER_HPP
#define VANDOUKEN_FRAMEGRABBER_HPP

#if !defined(ANDROID)
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <boost/shared_ptr.hpp>

#include <QImage>

namespace vandouken {

    class FrameGrabber
    {
    public:
        FrameGrabber();

        ~FrameGrabber();

        boost::shared_ptr<QImage> grab();

    private:
        bool fake;
#if !defined(ANDROID)
        CvCapture *capture;
        QImage testImage;
#endif
        int width;
        int height;
    };
}

#endif
