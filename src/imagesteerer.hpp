#ifndef VANDOUKEN_IMAGESTEERER_HPP
#define VANDOUKEN_IMAGESTEERER_HPP

#include "config.hpp"
#include "cell.hpp"

#include <libgeodecomp/misc/floatcoord.h>
#include <libgeodecomp/misc/region.h>
#include <libgeodecomp/misc/gridbase.h>

#include <QImage>

namespace vandouken {

    using LibGeoDecomp::Coord;
    using LibGeoDecomp::FloatCoord;
    using LibGeoDecomp::Region;
    using LibGeoDecomp::Streak;

    struct ImageSteerer
    {
        ImageSteerer() {}

        ImageSteerer(boost::shared_ptr<QImage> image, bool clear = true)
          : image(image)
          , clear(clear)
        {}

        void operator()(
                LibGeoDecomp::GridBase<Cell, 2> *grid,
                const LibGeoDecomp::Region<2>& validRegion,
                const LibGeoDecomp::Coord<2>& globalDimensions,
                unsigned)
        {
            QImage tmp = image->scaled(globalDimensions.x(), globalDimensions.y());

            FloatCoord<2> force(0.0f, 0.0f);
            for(Region<2>::Iterator i = validRegion.begin();
                i != validRegion.end(); ++i) {
                if(clear)
                {
                    grid->set(*i,
                        Cell(
                            0xff000000 + tmp.pixel(i->x(), i->y())
                          , *i
                          , false
                          , force
                          , rand() % Cell::MAX_SPAWN_COUNTDOWN
                        )
                    );
                }
                else
                {
                    Cell cell = grid->get(*i);
                    cell.backgroundPixel
                        = 0xff000000 + tmp.pixel(i->x(), i->y());
                    for(int j = 0; j < cell.numParticles; ++j)
                    {
                        if(cell.particles[j].lifetime > DEFAULT_PARTICLE_LIFETIME/2)
                        {
                            cell.particles[j].color = 0xff000000 + tmp.pixel(i->x(), i->y());
                        }
                    }
                    grid->set(*i, cell);
                }
            }
        }

        boost::shared_ptr<QImage> image;
        bool clear;

        template <typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
            throw "implement me!";
        }
    };
}

#endif
