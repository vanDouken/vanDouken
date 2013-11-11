//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_MAINCONTROL_HPP
#define VANDOUKEN_MAINCONTROL_HPP

//#include <QtGui/QWidget>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QMouseEvent>

namespace vandouken {
    struct MainControl : QWidget
    {
        Q_OBJECT
    public:

        enum State {
            free,
            edit,
            picture,
            video,
            resetAll
        };

        MainControl(QWidget *parent)
          : QWidget(parent)
          , state(free)
          , background(QString(VANDOUKEN_DATA_DIR "background.svg"))
          , reset(QString(VANDOUKEN_DATA_DIR "center.svg"))
          , freeHand(QString(VANDOUKEN_DATA_DIR "freeHand.svg"))
          , freeHandSelected(QString(VANDOUKEN_DATA_DIR "freeHandSelected.svg"))
          , forceSelect(QString(VANDOUKEN_DATA_DIR "force.svg"))
          , forceSelectSelected(QString(VANDOUKEN_DATA_DIR "forceSelected.svg"))
          , pictureSelect(QString(VANDOUKEN_DATA_DIR "picture.svg"))
          , pictureSelectSelected(QString(VANDOUKEN_DATA_DIR "pictureSelected.svg"))
          , videoSelect(QString(VANDOUKEN_DATA_DIR "video.svg"))
          , videoSelectSelected(QString(VANDOUKEN_DATA_DIR "videoSelected.svg"))
        {}

        static const int OFFSET = 10;
        static const int MARGIN = 25;
        static const int LENGTH = 100;

        QSize size();

        void mousePressEvent(QMouseEvent * event);

        void paintEvent(QPaintEvent *);

        State state;

    Q_SIGNALS:
        void stateChanged(int, bool);

    private:
        QSvgRenderer background;
        QSvgRenderer reset;
        QSvgRenderer freeHand;
        QSvgRenderer freeHandSelected;
        QSvgRenderer forceSelect;
        QSvgRenderer forceSelectSelected;
        QSvgRenderer pictureSelect;
        QSvgRenderer pictureSelectSelected;
        QSvgRenderer videoSelect;
        QSvgRenderer videoSelectSelected;
    };
}

#endif
