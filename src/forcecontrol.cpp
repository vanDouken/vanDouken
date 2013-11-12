//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(__MIC)

#include "forcecontrol.hpp"

#include <iostream>
#include <cmath>

namespace vandouken {
    ForceControl::ForceControl(QWidget *parent)
      : QWidget(parent)
      , background(QString(VANDOUKEN_DATA_DIR "background.svg"))
      , sliderLine(QString(VANDOUKEN_DATA_DIR "sliderLine.svg"))
      , sliderSelect(QString(VANDOUKEN_DATA_DIR "sliderSelect.svg"))
      , sliderLength(3 * LENGTH + 2 * MARGIN)
      , sliderPos(0.5)
    {
        boxes[center].load(QString(VANDOUKEN_DATA_DIR "center.svg"));
        boxes[north].load(QString(VANDOUKEN_DATA_DIR "north.svg"));
        boxes[south].load(QString(VANDOUKEN_DATA_DIR "south.svg"));
        boxes[east].load(QString(VANDOUKEN_DATA_DIR "east.svg"));
        boxes[west].load(QString(VANDOUKEN_DATA_DIR "west.svg"));
        boxes[northEast].load(QString(VANDOUKEN_DATA_DIR "northEast.svg"));
        boxes[northWest].load(QString(VANDOUKEN_DATA_DIR "northWest.svg"));
        boxes[southEast].load(QString(VANDOUKEN_DATA_DIR "southEast.svg"));
        boxes[southWest].load(QString(VANDOUKEN_DATA_DIR "southWest.svg"));

        boxesSelected[center].load(QString(VANDOUKEN_DATA_DIR "centerSelected.svg"));
        boxesSelected[north].load(QString(VANDOUKEN_DATA_DIR "northSelected.svg"));
        boxesSelected[south].load(QString(VANDOUKEN_DATA_DIR "southSelected.svg"));
        boxesSelected[east].load(QString(VANDOUKEN_DATA_DIR "eastSelected.svg"));
        boxesSelected[west].load(QString(VANDOUKEN_DATA_DIR "westSelected.svg"));
        boxesSelected[northEast].load(QString(VANDOUKEN_DATA_DIR "northEastSelected.svg"));
        boxesSelected[northWest].load(QString(VANDOUKEN_DATA_DIR "northWestSelected.svg"));
        boxesSelected[southEast].load(QString(VANDOUKEN_DATA_DIR "southEastSelected.svg"));
        boxesSelected[southWest].load(QString(VANDOUKEN_DATA_DIR "southWestSelected.svg"));

        selected.reset();
        selected[center] = true;
        Q_EMIT(forceChanged(sliderPos));
    }

    QSize ForceControl::size()
    {
        return
            QSize(
                sliderLength + 2 * MARGIN
              , 4.5 * MARGIN + (3 * (LENGTH + MARGIN))
            );
    }

    void ForceControl::mousePressEvent(QMouseEvent * event)
    {
        int x = event->pos().x() - OFFSET;
        int y = event->pos().y() - OFFSET;

        for(std::size_t yy = 0; yy < 3; ++yy)
        {
            for(std::size_t xx = 0; xx < 3; ++xx)
            {
                int xLeft = LENGTH * xx + xx * MARGIN;
                int yLeft = LENGTH * yy + yy * MARGIN;
                int xRight = xLeft + LENGTH;
                int yRight = yLeft + LENGTH;

                if(x >= xLeft && x <= xRight && y >= yLeft && y <= yRight)
                {
                    selected.reset();
                    selected[yy*3+xx] = true;
                    repaint(rect());
                    Q_EMIT(directionChanged(yy*3+xx));
                    return;
                }
            }
        }

        setSliderPos(x, y);
    }

    void ForceControl::mouseMoveEvent(QMouseEvent *event)
    {
        int x = event->pos().x() - OFFSET;
        int y = event->pos().y() - OFFSET;
        setSliderPos(x, y);
    }

    void ForceControl::setSliderPos(int x, int y)
    {
        int directionLength = 3 * (LENGTH + MARGIN);
        if(y >= directionLength && y <= directionLength + MARGIN && x >= 0 && x <= sliderLength)
        {
            sliderPos = std::ceil(x/sliderLength * 100) / 100;
            repaint(rect());
            Q_EMIT(forceChanged(sliderPos));
        }
    }

    void ForceControl::paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.fillRect(rect(), QBrush(Qt::black));
        QRectF boxesRect(0, 0, LENGTH, LENGTH);

        background.render(&painter, QRectF(0, 0, sliderLength + 2 * MARGIN, 4.5 * MARGIN + (3 * (LENGTH + MARGIN))));

        painter.translate(OFFSET, OFFSET);
        for(std::size_t i = 0; i < 9; ++i)
        {
            if(selected[i])
                boxesSelected[i].render(&painter, boxesRect);
            else
                boxes[i].render(&painter, boxesRect);
            painter.translate(boxesRect.width() + MARGIN, 0);
            if((i+1)%3 == 0) {
                painter.translate(-3*(boxesRect.width() + MARGIN), boxesRect.height() + MARGIN);
            }
        }

        painter.translate(0, MARGIN*0.5);

        sliderLine.render(&painter, QRectF(0, 0, sliderLength, LENGTH * 0.1));

        painter.translate(0, MARGIN);
        painter.setPen(QColor(35, 69, 124));
        painter.setFont(QFont("Sans", 24));
        painter.drawText(QRect(0,0,sliderLength,40), Qt::AlignLeft, QString("Kraftbetrag: ") + QString::number(sliderPos));
        painter.translate(0, -MARGIN);

        painter.translate(sliderPos * sliderLength - MARGIN*0.5, -MARGIN*0.5);
        sliderSelect.render(&painter, QRectF(0, 0, MARGIN*0.5, sliderLength * 0.1));
    }

    /*
    ForceControl::Direction ForceControl::getDirection()
    {
        for(std::size_t i = 0; i < 9; ++i)
        {
            if(selected[i])
            {
                switch (i)
                {
                    case 0:
                        return northWest;
                    case 1:
                        return north;
                    case 2:
                        return northEast;
                    case 3:
                        return west;
                    case 4:
                        return center;
                    case 5:
                        return east;
                    case 6:
                        return southWest;
                    case 7:
                        return south;
                    case 8:
                        return southEast;
                }
            }
        }
        return center;
    }
    */
}
#endif
