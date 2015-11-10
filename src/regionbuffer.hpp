//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_REGIONBUFFER_HPP
#define VANDOUKEN_REGIONBUFFER_HPP

#include "particle.hpp"

#include <libgeodecomp/geometry/region.h>
#include <libgeodecomp/communication/hpxserializationwrapper.h>

#include <boost/shared_ptr.hpp>

namespace vandouken {
    struct RegionBuffer
    {
        typedef boost::shared_ptr<Particles> BufferType;
        typedef LibGeoDecomp::Region<2> RegionType;

        RegionBuffer() {}
        explicit RegionBuffer(RegionType const & region) :
            region(region)
        {}
        
        RegionBuffer(BufferType const & buffer, RegionType const & region) :
            buffer(buffer),
            region(region)
        {}
        BufferType buffer;
        RegionType region;

        template <typename ARCHIVE>
        void serialize(ARCHIVE & ar, unsigned)
        {
            ar & buffer;
            ar & region;
        }
    };
}

#endif
