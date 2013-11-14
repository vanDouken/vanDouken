//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_IMAGESTEERER_HPP
#define VANDOUKEN_IMAGESTEERER_HPP

#include "config.hpp"
#include "cell.hpp"

#include <hpx/hpx_fwd.hpp>
#include <hpx/util/function.hpp>

#include <libgeodecomp/geometry/floatcoord.h>
#include <libgeodecomp/geometry/region.h>
#include <libgeodecomp/storage/gridbase.h>

#if !defined(__MIC)
#include <QImage>
#include <QBuffer>
#else
#include <boost/gil/extension/io_new/png_read.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#endif

namespace vandouken {

    using LibGeoDecomp::Coord;
    using LibGeoDecomp::FloatCoord;
    using LibGeoDecomp::Region;
    using LibGeoDecomp::Streak;

    struct ImageSteerer
    {
        ImageSteerer() {}

#if !defined(__MIC)
        ImageSteerer(boost::shared_ptr<QImage> image, bool clear = true)
          : image(image)
          , clear(clear)
        {}
#endif

        void operator()(
                LibGeoDecomp::GridBase<Cell, 2> *grid,
                const LibGeoDecomp::Region<2>& validRegion,
                const LibGeoDecomp::Coord<2>& globalDimensions,
                unsigned)
        {
#if !defined(__MIC)
            QImage tmp = image->scaled(globalDimensions.x(), globalDimensions.y());
#else
            boost::gil::rgb8_image_t scaledImage(globalDimensions.x(), globalDimensions.y());
            boost::gil::resize_view(
                boost::gil::const_view(image)
              , boost::gil::view(scaledImage)
              , boost::gil::bilinear_sampler()
            );
            boost::gil::rgb8_image_t::const_view_t tmp = boost::gil::const_view(scaledImage);
#endif

            FloatCoord<2> force(0.0f, 0.0f);
            LibGeoDecomp::CoordBox<2> box = grid->boundingBox();
            for (LibGeoDecomp::CoordBox<2>::Iterator i = box.begin(); i != box.end(); ++i) {
            /*
            std::cout << "grid bounding box: " << grid->boundingBox() << "\n";
            for (LibGeoDecomp::Region<2>::Iterator i = validRegion.begin(); i != validRegion.end(); ++i) {
            */
#if !defined(__MIC)
                unsigned color = 0xff000000 + tmp.pixel(i->x(), i->y());
#else
                boost::gil::rgb8_image_t::value_type pixel = tmp(i->x(), i->y());
                unsigned color = 0xff000000 +
                    (pixel[0] << 16) +
                    (pixel[1] << 8) +
                    (pixel[2] << 0);
#endif
                if(clear)
                {
                    grid->set(*i,
                        Cell(
                            color
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
                    cell.backgroundPixel = color;
                    for(int j = 0; j < cell.numParticles; ++j)
                    {
                        if(cell.particles[j].lifetime > DEFAULT_PARTICLE_LIFETIME/2)
                        {
                            cell.particles[j].color = color;
                        }
                    }
                    grid->set(*i, cell);
                }
            }
        }

#if !defined(__MIC)
        boost::shared_ptr<QImage> image;
#else
        boost::gil::rgb8_image_t image;
#endif
        bool clear;

        template <typename ARCHIVE>
        void save(ARCHIVE& ar, unsigned) const
        {
#if !defined(__MIC)
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            image->save(&buffer, "PNG");
            int size = ba.size();
            ar & size;
            ar & boost::serialization::make_array(ba.data(), ba.size());
#endif
            ar & clear;
        }

        template <typename ARCHIVE>
        void load(ARCHIVE& ar, unsigned)
        {
            int size = 0;
            ar & size;
            if(size > 0)
            {
#if !defined(__MIC)
                QByteArray ba(size, 0);
                ar & boost::serialization::make_array(ba.data(), size);
                QBuffer buffer(&ba);
                buffer.open(QIODevice::ReadOnly);
                image.reset(new QImage);
                image->load(&buffer, "PNG");
#else
                std::vector<char> data(size);
                ar & boost::serialization::make_array(&data[0], size);
                using boost::iostreams::basic_array_source;
                using boost::iostreams::stream;
                basic_array_source<char> input_source(&data[0], data.size());
                stream<basic_array_source<char> > input_stream(input_source);
                boost::gil::read_image(input_stream, image, boost::gil::png_tag());
#endif
            }
            ar & clear;
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}

HPX_UTIL_REGISTER_FUNCTION_DECLARATION(
    void(
        LibGeoDecomp::GridBase<vandouken::Cell, 2> *
      , const LibGeoDecomp::Region<2>&
      , const LibGeoDecomp::Coord<2>&
      , unsigned
    )
  , vandouken::ImageSteerer
  , vandoukenImageSteerer
)

#endif
