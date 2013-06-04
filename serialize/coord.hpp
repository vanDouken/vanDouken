//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SERIALIZE_COORD_HPP
#define HPX_SERIALIZE_COORD_HPP

#include <libgeodecomp/misc/coord.h>

namespace boost { namespace serialization {
    template <typename Archive, int Dim>
    inline void serialize(Archive & ar, LibGeoDecomp::Coord<Dim> & c, const unsigned)
    {
        for(std::size_t i = 0; i < Dim; ++i)
        {
            ar & c[i];
        }
    }
}}

#endif
