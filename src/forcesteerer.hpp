//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCESTEERER_HPP
#define VNADOUKEN_FORCESTEERER_HPP

#include "cell.hpp"

#include <libgeodecomp/misc/floatcoord.h>
#include <libgeodecomp/misc/region.h>
#include <libgeodecomp/misc/gridbase.h>

#include <QVector2D>

namespace vandouken {

    using LibGeoDecomp::Coord;
    using LibGeoDecomp::FloatCoord;
    using LibGeoDecomp::Region;
    using LibGeoDecomp::Streak;

    class ForceSteerer
    {
    public:
        ForceSteerer() {}

        ForceSteerer(QVector2D origin, QVector2D delta)
          : qOrigin(origin)
          , qDelta(delta)
        {}

        void operator()(
                LibGeoDecomp::GridBase<Cell, 2> *grid,
                const LibGeoDecomp::Region<2>& validRegion,
                const LibGeoDecomp::Coord<2>& globalDimensions,
                unsigned)
        {
            Region<2> lastRenderedRegion;
            Coord<2> origin(qOrigin.x(), qOrigin.y());
            FloatCoord<2> delta(qDelta.x(), qDelta.y());
            double length = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
            FloatCoord<2> force = delta * (1.0 / length);

            if (abs(delta[0]) > abs(delta[1])) {
                if (delta[0] < 0) {
                    origin = origin + Coord<2>(delta[0], delta[1]);
                    delta = delta * -1;
                }
                for (int x = 0; x <= delta[0]; ++x) {
                    int y = origin[1] + x * delta[1] / (delta[0] + 1);
                    renderForce(origin[0] + x, y, force, grid, validRegion, &lastRenderedRegion);
                }
            } else {
                if (delta[1] < 0) {
                    origin = origin + Coord<2>(delta[0], delta[1]);
                    delta = delta * -1;
                }
                for (int y = 0; y <= delta[1]; ++y) {
                    int x = origin[0] + y * delta[0] / (delta[1] + 1);
                    renderForce(x, origin[1] + y, force, grid, validRegion, &lastRenderedRegion);
                }
            }
        }

        void renderForce(int posX, int posY, const FloatCoord<2>& force, LibGeoDecomp::GridBase<Cell, 2> *grid, const Region<2>& validRegion, Region<2> *lastRenderedRegion)
        {
            int size = 10;
            Region<2> newRegion;
            for (int y = -size; y < size; ++y) {
                newRegion << Streak<2>(Coord<2>(posX - size, posY + y), posX + size);
            }

            Region<2> relevantRegion = newRegion & *lastRenderedRegion & validRegion;

            for (Region<2>::StreakIterator i = relevantRegion.beginStreak();
                 i != relevantRegion.endStreak();
                 ++i) {
                for (Coord<2> c = i->origin; c.x() < i->endX; ++c.x()) {
                    Cell cell = grid->get(c);
                    cell.setForceVario(force[0], force[1]);
                    grid->set(c, cell);
                }
            }

            *lastRenderedRegion = newRegion;
        }

        template <typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
            throw "implement me!";
        }

        QVector2D qOrigin;
        QVector2D qDelta;
    };
}

#endif
