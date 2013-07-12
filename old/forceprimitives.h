#ifndef _steeringprimitives_h_
#define _steeringprimitives_h_

#include <libgeodecomp/misc/coord.h>

using LibGeoDecomp::Coord;
using LibGeoDecomp::FloatCoord;

#if defined(_MSC_VER) && !defined(M_PI)
#define M_PI 3.141592653589793238462643383279502884
#endif

/**
 * A collection of objects to describe basic shapes which drive the
 * advection of particles.
 */
class ForcePrimitives
{
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
            FloatCoord<2> *result, 
            bool *setForce, 
            const Coord<2>& pos) = 0;
    };

    class Circle : public Base
    {
    public:
        Circle(const Coord<2>& center, int radius, double force) :
            center(center),
            radius(radius),
            force(force)
        {}

        virtual void initCell(
            FloatCoord<2> *result, 
            bool *setForce, 
            const Coord<2>& pos) 
        {
            int x = pos.x() - center.x();

            if ((x < -radius) || (x > radius)) {
                return;
            }

            int x1 = center.x() + x;
            int y = sqrt(double(radius) * radius - x * x);
            int y1 = center.y() + y;
            int y2 = center.y() - y;
            if (Coord<2>(x1, y1) == pos) {
                double forceAngle = angle(x, y) + 90;
                (*result)[0] = force * cos(2 * M_PI * forceAngle / 360);
                (*result)[1] = force * sin(2 * M_PI * forceAngle / 360);
                *setForce = true;
            }
            if (Coord<2>(x1, y2) == pos) {
                double forceAngle = angle(x, -y) + 90;
                (*result)[0] = force * cos(2 * M_PI * forceAngle / 360);
                (*result)[1] = force * sin(2 * M_PI * forceAngle / 360);
                *setForce = true;
            }
        }

    private:
        Coord<2> center;
        int radius;
        double force;
    };

    class Line : public Base
    {
    public:
        Line(const Coord<2>& start, const Coord<2>& end, const FloatCoord<2>& force) :
            start(start),
            end(end),
            force(force)
        {

        }

        virtual void initCell(
            FloatCoord<2> *result, 
            bool *setForce, 
            const Coord<2>& pos) 
        {}

    private:
        Coord<2> start;
        Coord<2> end; 
        FloatCoord<2> force;
    };
};

#endif
