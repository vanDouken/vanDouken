//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCESTEERER_HPP
#define VANDOUKEN_FORCESTEERER_HPP

#include "cell.hpp"

#include <hpx/hpx_fwd.hpp>
#include <hpx/util/function.hpp>

#include <libgeodecomp/geometry/floatcoord.h>
#include <libgeodecomp/geometry/region.h>
#include <libgeodecomp/storage/gridbase.h>

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
                    renderForce(origin[0] + x, y, force, grid, validRegion);
                }
            } else {
                if (delta[1] < 0) {
                    origin = origin + Coord<2>(delta[0], delta[1]);
                    delta = delta * -1;
                }
                for (int y = 0; y <= delta[1]; ++y) {
                    int x = origin[0] + y * delta[0] / (delta[1] + 1);
                    renderForce(x, origin[1] + y, force, grid, validRegion);
                }
            }
        }

        void renderForce(int posX, int posY, const FloatCoord<2>& force, LibGeoDecomp::GridBase<Cell, 2> *grid, const Region<2>& validRegion)
        {
            int size = 10;
            Region<2> newRegion;
            for (int y = -size; y < size; ++y) {
                newRegion << Streak<2>(Coord<2>(posX - size, posY + y), posX + size);
            }

            Region<2> relevantRegion = newRegion & validRegion;

            for (Region<2>::StreakIterator i = relevantRegion.beginStreak();
                 i != relevantRegion.endStreak();
                 ++i) {
                for (Coord<2> c = i->origin; c.x() < i->endX; ++c.x()) {
                    Cell cell = grid->get(c);
                    cell.setForceVario(force[0], force[1]);
                    grid->set(c, cell);
                }
            }
        }

        template <typename ARCHIVE>
        void load(ARCHIVE& ar, unsigned)
        {
            float coord[2] = {0.0};
            ar & coord;
            qOrigin = QVector2D(coord[0], coord[1]);
            ar & coord;
            qDelta = QVector2D(coord[0], coord[1]);
        }
        
        template <typename ARCHIVE>
        void save(ARCHIVE& ar, unsigned) const
        {
            float coord[2] = {0.0};
            coord[0] = qOrigin.x();
            coord[1] = qOrigin.y();
            ar & coord;
            coord[0] = qDelta.x();
            coord[1] = qDelta.y();
            ar & coord;
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()

        QVector2D qOrigin;
        QVector2D qDelta;
    };
}

HPX_UTIL_REGISTER_FUNCTION_DECLARATION(
    void(
        LibGeoDecomp::GridBase<vandouken::Cell, 2> *
      , const LibGeoDecomp::Region<2>&
      , const LibGeoDecomp::Coord<2>&
      , unsigned
    )
  , vandouken::ForceSteerer
  , vandoukenForceSteerer
)

#endif
