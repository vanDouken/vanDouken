//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "initializer.hpp"

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <boost/shared_ptr.hpp>

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QImage>
#endif

#include <fstream>

namespace vandouken {
    void Initializer::grid(LibGeoDecomp::GridBase<Cell, 2> *ret)
    {
        using LibGeoDecomp::CoordBox;
        using LibGeoDecomp::FloatCoord;
//#if 0
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
            ret->at(*i) = Cell(pixel, *i, setForce, force, rand() % Cell::MAX_SPAWN_COUNTDOWN);
        }
        std::cout << "done ...\n";
#endif
//#endif
    }
}

BOOST_CLASS_EXPORT_GUID(vandouken::Initializer, "vandoukenInitializer");
