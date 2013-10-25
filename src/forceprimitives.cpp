
#include "forceprimitives.hpp"

#include <libgeodecomp/communication/serialization.h>

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(vandouken::ForcePrimitives::Circle, "ForcePrimitiveCircle");
BOOST_CLASS_EXPORT_GUID(vandouken::ForcePrimitives::Line, "ForcePrimitiveLine");
