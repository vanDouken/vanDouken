//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCEVIEW_HPP
#define VANDOUKEN_FORCEVIEW_HPP

#include <QWidget>

namespace vandouken {
    struct ForceView : QWidget
    {
        void paintEvent(QPaintEvent *);
    };
}

#endif

