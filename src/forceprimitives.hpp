//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_FORCEPRIMITIVES_HPP
#define VANDOUKEN_FORCEPRIMITIVES_HPP

#include "config.hpp"

#include <libgeodecomp/geometry/coord.h>
#include <libgeodecomp/geometry/floatcoord.h>

#include <boost/serialization/base_object.hpp>

#if defined(_MSC_VER) && !defined(M_PI)
#define M_PI 3.141592653589793238462643383279502884
#endif

namespace vandouken {

    /**
     * A collection of objects to describe basic shapes which drive the
     * advection of particles.
     */
    class ForcePrimitives
    {
        typedef LibGeoDecomp::Coord<2> CoordType;
        typedef LibGeoDecomp::FloatCoord<2> FloatCoordType;
    public:
        static double angle(double velX, double velY)
        {
            if ((velX == 0) && (velY == 0)) {
                return 0;
            }

            double a = 0;
            a = atan(velY / velX);
            if (velX < 0) {
                a += 1.0 * M_PI;
            }
            return a * (360.0 / 2 / M_PI);
        }

        class Base
        {
        public:
            virtual void initCell(
                FloatCoordType *result,
                bool *setForce,
                const CoordType& pos) = 0;

            template<typename ARCHIVE>
            void serialize(ARCHIVE& ar, unsigned)
            {
            }
        };

        class Circle : public Base
        {
        public:
            Circle() {}

            Circle(const CoordType& center, int radius, double force) :
                center(center),
                radius(radius),
                force(force)
            {}

            virtual void initCell(
                FloatCoordType *result,
                bool *setForce,
                const CoordType& pos)
            {
                int x = pos.x() - center.x();

                if ((x < -radius) || (x > radius)) {
                    return;
                }

                int x1 = center.x() + x;
                int y = sqrt(double(radius) * radius - x * x);
                int y1 = center.y() + y;
                int y2 = center.y() - y;
                if (CoordType(x1, y1) == pos) {
                    double forceAngle = angle(x, y) + 90;
                    (*result)[0] = force * cos(2 * M_PI * forceAngle / 360);
                    (*result)[1] = force * sin(2 * M_PI * forceAngle / 360);
                    *setForce = true;
                }
                if (CoordType(x1, y2) == pos) {
                    double forceAngle = angle(x, -y) + 90;
                    (*result)[0] = force * cos(2 * M_PI * forceAngle / 360);
                    (*result)[1] = force * sin(2 * M_PI * forceAngle / 360);
                    *setForce = true;
                }
            }

            template<typename ARCHIVE>
            void serialize(ARCHIVE& ar, unsigned)
            {
                ar & boost::serialization::base_object<Base>(*this);
                ar & center;
                ar & radius;
                ar & force;
            }

        private:
            CoordType center;
            int radius;
            double force;
        };

        class Line : public Base
        {
        public:
            Line() {}

            Line(const CoordType& start, const CoordType& end, const FloatCoordType& force) :
                start(start),
                end(end),
                force(force)
            {

            }

            virtual void initCell(
                FloatCoordType *result,
                bool *setForce,
                const CoordType& pos)
            {}

            template<typename ARCHIVE>
            void serialize(ARCHIVE& ar, unsigned)
            {
                ar & boost::serialization::base_object<Base>(*this);
                ar & start;
                ar & end;
                ar & force;
            }

        private:
            CoordType start;
            CoordType end;
            FloatCoordType force;
        };
    };
}



#endif
