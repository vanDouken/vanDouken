//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FRAMEGRABBER_HPP
#define VANDOUKEN_FRAMEGRABBER_HPP

#if !defined(__MIC)

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

#endif
