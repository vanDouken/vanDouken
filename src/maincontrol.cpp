//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(__MIC)

#include "maincontrol.hpp"

#include <iostream>

namespace vandouken {
    void MainControl::mousePressEvent(QMouseEvent *event)
    {
        int x = event->pos().x() - OFFSET;
        int y = event->pos().y() - OFFSET;
        if(x < 0 || x > LENGTH) return;
        if(y < 0) return;
        if(y < LENGTH)
        {
            state = free;
            Q_EMIT(stateChanged(state, true));
            repaint();
            return;
        }
        y -= LENGTH + MARGIN;
        if(y < LENGTH)
        {
            state = edit;
            Q_EMIT(stateChanged(state, true));
            repaint();
            return;
        }
        y -= LENGTH + MARGIN;
        if(y < LENGTH)
        {
            state = picture;
            Q_EMIT(stateChanged(state, true));
            repaint();
            return;
        }
        y -= LENGTH + MARGIN;
        if(y < LENGTH)
        {
            state = video;
            Q_EMIT(stateChanged(state, true));
            repaint();
            return;
        }
        y -= LENGTH + MARGIN;
        if(y < LENGTH)
        {
            state = resetAll;
            Q_EMIT(stateChanged(state, true));
            repaint();
        }
    }
    
    QSize MainControl::size()
    {
        return
            QSize(
                LENGTH + 2*OFFSET + 2*MARGIN,
                4*OFFSET + 5* LENGTH + 6*MARGIN
            );
    }
        
    void MainControl::paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        QRectF boxesRect(0, 0, LENGTH, LENGTH);
        
        painter.translate(OFFSET, OFFSET);

        background.render(
            &painter,
            QRectF(
                0,
                0,
                LENGTH + 2*MARGIN,
                5* LENGTH + 6*MARGIN
            )
        );
        painter.translate(MARGIN, MARGIN);
        if(state == State::free)
        {
            freeHandSelected.render(&painter, boxesRect);
        }
        else
        {
            freeHand.render(&painter, boxesRect);
        }

        painter.translate(0, MARGIN + LENGTH);
        if(state == State::edit)
        {
            forceSelectSelected.render(&painter, boxesRect);
        }
        else
        {
            forceSelect.render(&painter, boxesRect);
        }

        painter.translate(0, MARGIN + LENGTH);
        if(state == State::picture)
        {
            pictureSelectSelected.render(&painter, boxesRect);
        }
        else
        {
            pictureSelect.render(&painter, boxesRect);
        }

        painter.translate(0, MARGIN + LENGTH);
        if(state == State::video)
        {
            videoSelectSelected.render(&painter, boxesRect);
        }
        else
        {
            videoSelect.render(&painter, boxesRect);
        }

        painter.translate(0, MARGIN + LENGTH);
        reset.render(&painter, boxesRect);
    }
}

#endif
