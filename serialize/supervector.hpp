
#ifndef HPX_SERIALIZE_SUPERVECTOR_HPP
#define HPX_SERIALIZE_SUPERVECTOR_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/serialization/vector.hpp>

#include <libgeodecomp/misc/supervector.h>

namespace boost { namespace serialization {
    template <typename Archive, typename T>
    inline void serialize(Archive & ar, LibGeoDecomp::SuperVector<T> & c, const unsigned)
    {
        ar & static_cast<std::vector<T> &>(c);
    }

}}

#endif
