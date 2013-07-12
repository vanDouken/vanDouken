//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SERIALIZE_COORDBOX_HPP
#define HPX_SERIALIZE_COORDBOX_HPP

#include <libgeodecomp/misc/coordbox.h>

namespace boost { namespace serialization {
    template <typename Archive, int Dim>
    inline void serialize(Archive & ar, LibGeoDecomp::CoordBox<Dim> & cb, const unsigned)
    {
        ar & cb.origin;
        ar & cb.dimensions;
    }
}}

#endif
