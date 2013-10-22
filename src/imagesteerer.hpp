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
        void save(ARCHIVE& ar, unsigned) const
        {
            int w = image->width();
            int h = image->height();
            QImage::Format format = image->format();
            ar & w;
            ar & h;
            ar & format;
            const unsigned char *imageData = image->constBits();
            std::vector<unsigned char> buffer(imageData, imageData + w * h);
            ar & buffer;
            ar & clear;
        }

        template <typename ARCHIVE>
        void load(ARCHIVE& ar, unsigned)
        {
            int w;
            int h;
            QImage::Format format;
            ar & w;
            ar & h;
            ar & format;
            std::vector<unsigned char> buffer;
            buffer.reserve(w * h);
            ar & buffer;
            image.reset(new QImage(&buffer[0], w, h, format));
            ar & clear;
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}

#endif
