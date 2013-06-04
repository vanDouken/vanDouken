
#ifndef HPX_SERIALIZE_SUPERMAP_HPP
#define HPX_SERIALIZE_SUPERMAP_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/serialization/map.hpp>

#include <libgeodecomp/misc/supermap.h>

namespace boost { namespace serialization {
    template <typename Archive, typename Key, typename Value>
    inline void serialize(Archive & ar, LibGeoDecomp::SuperMap<Key, Value> & c, const unsigned)
    {
        ar & static_cast<std::map<Key, Value> &>(c);
    }

}}

#endif
