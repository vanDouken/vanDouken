#include <cv.h>
#include <highgui.h>
#include <framegrabber.h>
#include <boost/assert.hpp>
#include <vector>

FrameGrabber::FrameGrabber(bool _fakeCam, int width, int height) :
    capture(cvCaptureFromCAM(CV_CAP_ANY)),
    fakeCam(_fakeCam),
    time(0)
{
    std::cout << "fakeCam: " << fakeCam << "\n";
    if(capture && !fakeCam)
    {
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);
    }
    if (!fakeCam && !capture)
        throw std::runtime_error("could not access any capture devices");
}

FrameGrabber::~FrameGrabber()
{
    cvReleaseCapture(&capture);
}

void FrameGrabber::grab(std::vector<char> & frame, std::size_t width, std::size_t height)
{
    incFrames();

    if (fakeCam) {
        ++time;
        if (time > 50) {
            time = 0;
        }
        if(frame.size() < width * height * 3)
        {
            std::vector<char> tmp(width * height * 3);
            std::swap(frame, tmp);
        }


        std::size_t offsetX = time * 10 - 50;
        std::size_t offsetY = 100;
        if (time > 25)
            offsetY += (time - 25) * 10;

        //std::vector<char> frame(MAX_X * MAX_Y * 3);
        for (std::size_t y = 0; y < height; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                int r = 0xff;
                int g = 0xff;
                int b = 0xff;
                
                if ((y >= offsetY) &&(y <= (offsetY + 100)) && (x >= offsetX) && (x <= (offsetX + 50)) ) {
                    r = 1;
                    g = 0x8f;
                    b = 0x00;
                }
                
                frame[(y * width + x) * 3 + 0] = r;
                frame[(y * width + x) * 3 + 1] = g;
                frame[(y * width + x) * 3 + 2] = b;
            }
        }
                
        return;
    }

    IplImage *cv_frame = cvQueryFrame(capture);
    if (!cv_frame) 
        throw std::runtime_error("could not capture frame");

    if (cv_frame->depth != 8)
        throw std::runtime_error("unexpected color depth");

    if (cv_frame->nChannels != 3)
        throw std::runtime_error("unexpected number of channels");

    // fixme:
    // if (simParamsHost.debug)
    //   std::cerr << "Capture:\n"
	// 	<< "  nChannels: " << frame->nChannels << "\n"
	// 	<< "  depth: " << frame->depth << "\n"
	// 	<< "  IPL_DEPTH_8U:  " << IPL_DEPTH_8U  << "\n"
	// 	<< "  IPL_DEPTH_8S:  " << IPL_DEPTH_8S << "\n"
	// 	<< "  IPL_DEPTH_16U: " << IPL_DEPTH_16U  << "\n"
	// 	<< "  IPL_DEPTH_16S: " << IPL_DEPTH_16S << "\n"
	// 	<< "  IPL_DEPTH_32S: " << IPL_DEPTH_32S << "\n"
	// 	<< "  IPL_DEPTH_32F: " << IPL_DEPTH_32F  << "\n"
	// 	<< "  IPL_DEPTH_64F: " << IPL_DEPTH_64F << "\n"
	// 	<< "  dataOrder: " << frame->dataOrder << "\n"
	// 	<< "  width: " << frame->width << "\n"
	// 	<< "  height: " << frame->height << "\n"
	// 	<< "  imageSize: " << frame->imageSize << "\n"
	// 	<< "  widthStep: " << frame->widthStep << "\n\n";
    if(frame.size() < std::size_t(cv_frame->width * cv_frame->height * 3))
    {
        std::vector<char> tmp(cv_frame->width * cv_frame->height * 3);
        std::swap(frame, tmp);
    }
    
    std::copy(cv_frame->imageData, cv_frame->imageData + cv_frame->width * cv_frame->height * 3, frame.begin());

    //Q_EMIT newFrame(frame->imageData, frame->width, frame->height);
}

