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

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QImage>
#endif

#include <fstream>

namespace vandouken {

    Initializer::Initializer(const CoordType& dim) :
        LibGeoDecomp::SimpleInitializer<Cell>(
            dim,
            (std::numeric_limits<unsigned>::max)())
    {}

    void Initializer::grid(LibGeoDecomp::GridBase<Cell, 2> *ret)
    {
        using LibGeoDecomp::CoordBox;
        using LibGeoDecomp::FloatCoord;
#if !defined(__ANDROID__) || !defined(ANDROID)
        const char * filename = VANDOUKEN_DATA_DIR VANDOUKEN_INITIALIZER_IMG;
        QImage img = QImage(filename).scaled(gridDimensions().x(), gridDimensions().y());

        std::cout << "opening " << filename << "\n";
        std::cout << "initializer dimensions: " << gridDimensions().x() << " " << gridDimensions().y() << "\n";
        if(img.isNull())
        {
            std::cout << "couldn't load image " << filename << "\n";
            return;
        }

        CoordBox<2> box = ret->boundingBox();
        for (CoordBox<2>::Iterator i = ret->boundingBox().begin(); i != box.end(); ++i) {
            bool setForce = false;
            FloatCoord<2> force;

            for (std::size_t j = 0; j < shapes.size(); ++j) {
                shapes[j]->initCell(&force, &setForce, *i);
            }

            // force alpha channel to 0xff to ensure all particles are opaque
            unsigned pixel = 0xff000000 + img.pixel(i->x(), i->y());
            ret->set(*i, Cell(pixel, *i, setForce, force, rand() % Cell::MAX_SPAWN_COUNTDOWN));
        }
        std::cout << "done ...\n";
#endif
    }

    Initializer *createInitializer(const Initializer::CoordType& dim)
    {
        Initializer *init(new Initializer(dim));
        using LibGeoDecomp::Coord;

        try {
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(0.25 * 1160, 0.25 * 188), 0.25 * 90, 0.4 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(0.25 * 904, 0.25 * 240) , 0.25 * 44, 0.3 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(0.25 * 778, 0.25 * 92)  , 0.25 * 30, 0.2 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(0.25 * 526, 0.25 * 72)  , 0.25 * 20, 0.15));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(0.25 * 444, 0.25 * 44)  , 0.25 * 10, 0.15 ));
            init->addShape(
                ForcePrimitives::Circle(
                    Coord<2>(0.25 * 296, 0.25 * 34)  , 0.25 * 6 , 0.1 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(0.25 * 302, 0.25 * 180) , 0.25 * 20, 0.25 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(0.25 * 138, 0.25 * 46)  , 0.25 * 10, 0.2 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(0.25 * 416, 0.25 * 328) , 0.25 *  4, 0.1 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(0.25 * 456, 0.25 * 538) , 0.25 * 68, 0.4 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(0.25 * 170, 0.25 * 492) , 0.25 *  6, 0.1 ));
            init->addShape(
                    ForcePrimitives::Circle(
                        Coord<2>(0.25 * 58 , 0.25 * 460) , 0.25 *  4, 0.1 ));
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
