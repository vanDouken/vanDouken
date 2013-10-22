#include "config.hpp"
#include "framegrabber.hpp"
#include <boost/assert.hpp>
#include <boost/make_shared.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace vandouken {

    FrameGrabber::FrameGrabber()
    {

        capture = cvCaptureFromCAM(CV_CAP_ANY);
        if(capture && cvGetCaptureDomain(capture) != CV_CAP_ANY)
        {

            width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
            height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
        }
        else
        {
            std::cout << "Got fake cam ...\n";
            fake = true;
        }

        std::cout << width << " " << height << "\n";
    }

    FrameGrabber::~FrameGrabber()
    {
        if(!fake)
        {
            cvReleaseCapture(&capture);
        }
    }

    boost::shared_ptr<QImage> FrameGrabber::grab()
    {
        boost::shared_ptr<QImage> frame;
        if(fake)
        {
            frame.reset(new QImage(VANDOUKEN_DATA_DIR "test.png"));
        }
        else
        {
            IplImage *cv_frame = cvQueryFrame(capture);
            if (!cv_frame) 
                throw std::runtime_error("could not capture frame");

            if (cv_frame->depth != 8)
                throw std::runtime_error("unexpected color depth");
            
            int w = cv_frame->width;
            int h = cv_frame->height;
            int channels = cv_frame->nChannels;
            frame.reset(new QImage(w, h, QImage::Format_ARGB32));

            char *data = cv_frame->imageData;

            for (int y = 0; y < h; y++, data += cv_frame->widthStep)
            {
                for (int x = 0; x < w; x++)
                {
                    char r = 0, g = 0, b = 0, a = 0;
                    if (channels == 1)
                    {
                        r = data[x * channels];
                        g = data[x * channels];
                        b = data[x * channels];
                    }
                    else if (channels == 3 || channels == 4)
                    {
                        r = data[x * channels + 2];
                        g = data[x * channels + 1];
                        b = data[x * channels];
                    }
                    
                    if (channels == 4)
                    {
                        a = data[x * channels + 3];
                        frame->setPixel(width - 1 - x, y, qRgba(r, g, b, a));
                    }
                    else
                    {
                        frame->setPixel(width - 1 - x, y, qRgb(r, g, b));
                    }
                }
            }
        }
        return frame;
    }

}
