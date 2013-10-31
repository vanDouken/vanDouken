//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_PARTICLE_HPP
#define VANDOUKEN_PARTICLE_HPP

#include <libgeodecomp/geometry/topologies.h>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

//#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QtGui/QColor>
/*
#else
inline int qRed(unsigned int rgb)
{
    return ((rgb >> 16) & 0xff);
}

inline int qGreen(unsigned int rgb)
{
    return ((rgb >> 8) & 0xff);
}

inline int qBlue(unsigned int rgb)
{
    return (rgb & 0xff);
}

inline int qAlpha(unsigned int rgb)
{
    return rgb >> 24;
}
#endif
*/


namespace vandouken {
    class Particle
    {
    public:
        inline Particle(float posX=0, float posY=0, float posZ=0, float angle=0, unsigned color=0) :
            posX (posX ),
            posY (posY ),
            posZ (posZ ),
            angle(angle),
            color(color)
        {}

        float posX;
        float posY;
        float posZ;
        float angle;
        // coding: AARRGGBB, use color for color multiply of texture and alpha for texture index?
        unsigned color;
    };

    class Particles
    {
    public:
        typedef LibGeoDecomp::Topologies::Cube<2>::Topology Topology;

        /*
        Particles() :
            posAngle(boost::make_shared<std::vector<float> >()),
            colors(boost::make_shared<std::vector<boost::uint32_t> >())
        {}
        */

        void addParticle(const Particle& particle)
        {
            posAngle.push_back(std::ceil(particle.posX * 100.f));
            posAngle.push_back(std::ceil(particle.posY * 100.f));
            posAngle.push_back(std::ceil(particle.posZ * 100.f));
            posAngle.push_back(std::ceil(particle.angle * 100.f));

            colors.push_back(qRed  (particle.color));
            colors.push_back(qGreen(particle.color));
            colors.push_back(qBlue (particle.color));
            colors.push_back(qAlpha(particle.color));
        }

        void resize(std::size_t size)
        {
            posAngle.resize(size * 4);
            colors.resize(size * 4);
        }

        void reserve(std::size_t size)
        {
            posAngle.reserve(size * 4);
            colors.reserve(size * 4);
        }

        std::size_t size()
        {
            return colors.size() / 4;
        }

        std::size_t capacity()
        {
            return colors.capacity() / 4;
        }

        template<typename ARCHIVE>
        void serialize(ARCHIVE& ar, unsigned)
        {
            ar & posAngle;
            ar & colors;
        }

        std::vector<float> posAngle;
        std::vector<float> colors;
    };
}
BOOST_IS_BITWISE_SERIALIZABLE(vandouken::Particle)

#endif
