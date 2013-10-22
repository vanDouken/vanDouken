#ifndef VANDOUKEN_FRAMEGRABBER_HPP
#define VANDOUKEN_FRAMEGRABBER_HPP

#include <opencv/cv.h>
#include <opencv/highgui.h>

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
        CvCapture *capture;
        QImage testImage;
        int width;
        int height;
    };
}

#endif
