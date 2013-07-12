#ifndef _forcesteerer_h_
#define _forcesteerer_h_

#include <boost/thread.hpp>
#include <Qt/QtCore>
#include <Qt/QtGui>
#include <QVector2D>
#include <libgeodecomp/io/steerer.h>
#include <libgeodecomp/misc/supervector.h>
#include <libgeodecomp/mpilayer/mpilayer.h>

#include "canvascell.h"

using LibGeoDecomp::Coord;
using LibGeoDecomp::CanvasCell;
using LibGeoDecomp::FloatCoord;
using LibGeoDecomp::Steerer;
using LibGeoDecomp::FloatCoord;
using LibGeoDecomp::Streak;
using LibGeoDecomp::SuperVector;

class ForceSteerer : public QObject, public Steerer<CanvasCell>
{
    Q_OBJECT
public:
    using Steerer<CanvasCell>::Topology;
    using Steerer<CanvasCell>::GridType;
    typedef LibGeoDecomp::Region<Topology::DIMENSIONS> Region;

    ForceSteerer(const unsigned period) :
        LibGeoDecomp::Steerer<CanvasCell>(period)
    {}

    virtual void nextStep(
        GridType *grid, 
        const Region& validRegion, 
        const unsigned& step) 
    {
        LibGeoDecomp::MPILayer layer;
        boost::mutex::scoped_lock lock(mutex);

        if (layer.broadcast(origins.size(), 0)) {
            origins = layer.broadcastVector(origins, 0);
            deltas  = layer.broadcastVector(deltas,  0);
        }

        if (origins.size() > 0) {
            Region lastRenderedRegion;
            for (int i = 0; i < origins.size(); ++i) {
                Coord<2> origin = origins[i];
                FloatCoord<2> delta = deltas[i];
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

            origins.clear();
            deltas.clear();
        }
    }

public slots:
    void addForce(const QVector2D& origin, const QVector2D& delta)
    {
        boost::mutex::scoped_lock lock(mutex);
        origins << Coord<2>(origin.x(), origin.y());
        deltas << FloatCoord<2>(delta.x(), delta.y());
    }

    void renderForce(int posX, int posY, const FloatCoord<2>& force, GridType *grid, const Region& validRegion, Region *lastRenderedRegion)
    {
        int size = 10;
        Region newRegion;
        for (int y = -size; y < size; ++y) {
            newRegion << Streak<2>(Coord<2>(posX - size, posY + y), posX + size);
        }

        Region relevantRegion = newRegion & *lastRenderedRegion & validRegion;

        for (Region::StreakIterator i = relevantRegion.beginStreak(); 
             i != relevantRegion.endStreak(); 
             ++i) {
            for (Coord<2> c = i->origin; c.x() < i->endX; ++c.x()) {
                grid->at(c).setForceVario(force[0], force[1]);
            }
        }
        
        *lastRenderedRegion = newRegion;
    }


private:
    SuperVector<Coord<2> > origins;
    SuperVector<FloatCoord<2> > deltas;
    boost::mutex mutex;
};

#endif
