//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_SINGLETRACER_HPP
#define VANDOUKEN_SINGLETRACER_HPP

#include "cell.hpp"

#include <boost/serialization/access.hpp>
#include <libgeodecomp/io/writer.h>

namespace vandouken {
    class SingleTracer : public LibGeoDecomp::Writer<Cell>
    {
        friend class boost::serialization::access;
    public:
        SingleTracer(unsigned period) :
            LibGeoDecomp::Writer<Cell>("", period)
        {}

        void stepFinished(const GridType& grid, unsigned step, LibGeoDecomp::WriterEvent event)
        {
            std::cout << "Simulation at step " << step << "\n";
        }

    private:
        SingleTracer()
        {}
        template<typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
            ar & boost::serialization::base_object<LibGeoDecomp::Writer<Cell> >(*this);
        }
    };
}

#endif
