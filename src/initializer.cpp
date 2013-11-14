//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "initializer.hpp"
#include <libgeodecomp/communication/serialization.h>

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>


#include <boost/shared_ptr.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <fstream>

namespace vandouken {

    Initializer::Initializer(const CoordType& dim) :
        LibGeoDecomp::SimpleInitializer<Cell>(
            dim,
            (std::numeric_limits<unsigned>::max)())
    {
    }

    void Initializer::grid(LibGeoDecomp::GridBase<Cell, 2> *ret)
    {
        using LibGeoDecomp::CoordBox;
        using LibGeoDecomp::FloatCoord;

        boost::gil::rgb8_image_t img;
        boost::gil::png_read_image(VANDOUKEN_DATA_DIR VANDOUKEN_INITIALIZER_IMG, img);
        boost::gil::rgb8_image_t scaledImg(gridDimensions().x(), gridDimensions().y());
        boost::gil::resize_view(
            boost::gil::const_view(img)
          , boost::gil::view(scaledImg)
          , boost::gil::bilinear_sampler()
        );

        boost::gil::rgb8_image_t::const_view_t imgView = boost::gil::const_view(scaledImg);
        CoordBox<2> box = ret->boundingBox();
        for (CoordBox<2>::Iterator i = box.begin(); i != box.end(); ++i) {
            bool setForce = false;
            FloatCoord<2> force;

            for (std::size_t j = 0; j < shapes.size(); ++j) {
                shapes[j]->initCell(&force, &setForce, *i);
            }

            // force alpha channel to 0xff to ensure all particles are opaque
            boost::gil::rgb8_image_t::value_type pixel = imgView(i->x(), i->y());
            unsigned color = 0xff000000 +
                (pixel[0] << 16) +
                (pixel[1] << 8) +
                (pixel[2] << 0);
            ret->set(*i, Cell(color, *i, setForce, force, rand() % Cell::MAX_SPAWN_COUNTDOWN));
        }
        std::cout << "done ...\n";
    }

    Initializer *createInitializer(const Initializer::CoordType& dim)
    {
        Initializer *init(new Initializer(dim));
        using LibGeoDecomp::Coord;

        boost::gil::point2<std::ptrdiff_t> imgDims
            = boost::gil::png_read_dimensions(VANDOUKEN_DATA_DIR VANDOUKEN_INITIALIZER_IMG);

        double scaleFactorX = double(dim.x())/imgDims[0];
        double scaleFactorY = double(dim.y())/imgDims[1];

        try {
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(scaleFactorX * 1160, scaleFactorY * 188), scaleFactorX * 90, 0.4 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(scaleFactorX * 904, scaleFactorY * 240) , scaleFactorX * 44, 0.3 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(scaleFactorX * 778, scaleFactorY * 92)  , scaleFactorX * 30, 0.2 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(scaleFactorX * 526, scaleFactorY * 72)  , scaleFactorX * 20, 0.15));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(scaleFactorX * 444, scaleFactorY * 44)  , scaleFactorX * 10, 0.15 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(scaleFactorX * 296, scaleFactorY * 34)  , scaleFactorX * 6 , 0.1 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(scaleFactorX * 302, scaleFactorY * 180) , scaleFactorX * 20, 0.25 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(scaleFactorX * 138, scaleFactorY * 46)  , scaleFactorX * 10, 0.2 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(scaleFactorX * 416, scaleFactorY * 328) , scaleFactorX *  4, 0.1 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(scaleFactorX * 456, scaleFactorY * 538) , scaleFactorX * 68, 0.4 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(scaleFactorX * 170, scaleFactorY * 492) , scaleFactorX *  6, 0.1 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(scaleFactorX * 58 , scaleFactorY * 460) , scaleFactorX *  4, 0.1 ));
        }
        catch(...)
        {
            delete init;
            init = 0;
        }
        return init;
    }
}

BOOST_CLASS_EXPORT_GUID(vandouken::Initializer, "vandoukenInitializer");
