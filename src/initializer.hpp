//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_INITIALIZER_HPP
#define VANDOUKEN_INITIALIZER_HPP

#include "config.hpp"
#include "cell.hpp"
#include "forceprimitives.hpp"

#include <boost/serialization/export.hpp>
#include <libgeodecomp/io/simpleinitializer.h>
#include <libgeodecomp/misc/supervector.h>

namespace vandouken {
    class Initializer : public LibGeoDecomp::SimpleInitializer<Cell>
    {
        friend class boost::serialization::access;

    public:
        typedef LibGeoDecomp::GridBase<Cell, 2> GridType;
        typedef LibGeoDecomp::Coord<2> CoordType;

        Initializer(const CoordType& dim) :
            SimpleInitializer<Cell>(
                dim,
                (std::numeric_limits<unsigned>::max)())
        {}

        void grid(GridType *ret);

        template<class SHAPE>
        void addShape(const SHAPE& shape)
        {
            shapes << boost::shared_ptr<ForcePrimitives::Base>(new SHAPE(shape));
        }

    private:
        LibGeoDecomp::SuperVector<boost::shared_ptr<ForcePrimitives::Base> > shapes;

        template<class ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
            ar & boost::serialization::base_object<SimpleInitializer<Cell> >(*this);
            ar & shapes;
        }

        Initializer() :
            SimpleInitializer<Cell>(
                CoordType(0, 0),
                (std::numeric_limits<unsigned>::max)())
        {}
    };
}

BOOST_CLASS_EXPORT_KEY2(vandouken::Initializer, "vandoukenInitializer");

#endif
