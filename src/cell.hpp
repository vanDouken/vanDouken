//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_CANVASCELL_HPP
#define VANDOUKEN_CANVASCELL_HPP

#include <stdio.h>
#include <libgeodecomp/misc/cellapitraits.h>
#include <libgeodecomp/misc/floatcoord.h>
#include <libgeodecomp/misc/topologies.h>
#include <libgeodecomp/misc/stencils.h>
#include <libgeodecomp/misc/cellapitraits.h>

#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/array.hpp>
#include <boost/foreach.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#define DEFAULT_PARTICLE_LIFETIME 300
#define FADE_IN_OUT 40

namespace vandouken {

class Cell
{
    friend class CanvasWriter;
public:
    class Particle
    {
    public:
        Particle()
            : lifetime(DEFAULT_PARTICLE_LIFETIME)
            , color(0)
        {
            pos[0] = 0;
            pos[1] = 0;
            pos[2] = 0;
            vel[0] = 0;
            vel[1] = 0;
        }

        Particle(const float& pos0, const float& pos1, const float& pos2, const unsigned& _color = 0, const int& _lifetime = DEFAULT_PARTICLE_LIFETIME) :
            lifetime(_lifetime),
            color(_color)
        {
            pos[0] = pos0;
            pos[1] = pos1;
            pos[2] = pos2;
            vel[0] = 0;
            vel[1] = 0;
        }

        void update(const float& deltaT, const float& force0, const float& force1, const float& forceFactor, const float& friction)
        {
            vel[0] += deltaT * forceFactor * force0;
            vel[1] += deltaT * forceFactor * force1;
            vel[0] *= friction;
            vel[1] *= friction;
            pos[0] += deltaT * vel[0];
            pos[1] += deltaT * vel[1];
            --lifetime;
        }

        template <typename Archive>
        void serialize(Archive & ar, unsigned)
        {
            ar & pos;
            ar & vel;
            ar & lifetime;
            ar & color;
        }

        float pos[3];
        float vel[2];
        int lifetime;
        unsigned color;
    };

    static const int MAX_PARTICLES = 5;
    static const int MAX_SPAWN_COUNTDOWN = DEFAULT_PARTICLE_LIFETIME;
    typedef Topologies::Cube<2>::Topology Topology;
    typedef Stencils::Moore<2, 1> Stencil;
    class API : public CellAPITraits::Fixed
    {};

    static inline unsigned nanoSteps()
    {
        return 2;
    }
    // fixme: clean up constructors
    CanvasCell()
      : backgroundPixel(0x00ff0000),
        spawnCountdown(0),
        numParticles(0),
        dist(0, 1000)
    {
        pos[0] = 0;
        pos[1] = 0;
        forceFixed[0] = 0.0;
        forceFixed[1] = 0.0;
        forceVario[0] = 0.0;
        forceVario[1] = 0.0;
        forceSet = false;
    }

    CanvasCell(
        const unsigned& _backgroundPixel,
        const Coord<2>& _pos,
        const bool& _forceSet = false,
        const FloatCoord<2>& _forceFixed = FloatCoord<2>(),
        const int& _spawnCountdown = 0) :
        backgroundPixel(_backgroundPixel),
        spawnCountdown(_spawnCountdown),
        numParticles(0),
        dist(0, 1000)
    {
        pos[0] = _pos.x();
        pos[1] = _pos.y();
        forceFixed[0] = _forceFixed[0];
        forceFixed[1] = _forceFixed[1];
        forceVario[0] = 0.0;
        forceVario[1] = 0.0;
        forceSet = _forceSet;
    }

    template<typename COORD_MAP>
    void update(const COORD_MAP& hood, const unsigned& nanoStep)
    {
        //std::cout << numParticles << " start update\n";
        if (nanoStep == (nanoSteps() - 1)) {
            // fixme: can we avoid this?
            *this = hood[FixedCoord<0,0>()];
            moveParticles(hood);
            return;
        }

        updateForces(hood, nanoStep);

        if (nanoStep == (nanoSteps() - 2)) {
            spawnParticles();
            updateParticles();
        }

        //std::cout << numParticles << " end update\n";
    }

    void setForceVario(const float forceX, const float forceY)
    {
        forceVario[0] = forceX;
        forceVario[1] = forceY;
    }

private:
    unsigned backgroundPixel;
    unsigned spawnCountdown;
    float pos[2];
    bool forceSet;

    float forceVario[2];
    float forceFixed[2];
    float forceTotal[2];
    // fixme: make this private again
public:
    Particle particles[MAX_PARTICLES];
    int numParticles;
    boost::random::mt19937 gen;
    boost::random::uniform_int_distribution<> dist;

    template <typename Archive>
    void serialize(Archive & ar, unsigned)
    {
        ar & backgroundPixel;
        ar & spawnCountdown;
        ar & pos;
        ar & forceSet;
        ar & forceVario;
        if(forceSet)
        {
            ar & forceFixed;
            ar & forceTotal;
        }
        ar & numParticles;
        if(numParticles > 0)
        {
            ar & boost::serialization::make_array(particles, numParticles);
        }
    }

private:

    /**
     * Transition particles between neighboring cells
     */
    template<typename COORD_MAP>
    void moveParticles(const COORD_MAP& hood)
    {
        numParticles = 0;
        addParticles(hood[FixedCoord<-1, -1>()]);
        addParticles(hood[FixedCoord< 0, -1>()]);
        addParticles(hood[FixedCoord< 1, -1>()]);

        addParticles(hood[FixedCoord<-1,  0>()]);
        addParticles(hood[FixedCoord< 0,  0>()]);
        addParticles(hood[FixedCoord< 1,  0>()]);

        addParticles(hood[FixedCoord<-1,  1>()]);
        addParticles(hood[FixedCoord< 0,  1>()]);
        addParticles(hood[FixedCoord< 1,  1>()]);
    }

    void addParticles(const CanvasCell& cell)
    {
        for (int i = 0; i < cell.numParticles; ++i) {
            const Particle& particle = cell.particles[i];
            // fixme: refactor via "extract method" for readability
            if ((((int)pos[0]) == ((int)particle.pos[0])) &&
                (((int)pos[1]) == ((int)particle.pos[1])) &&
                (particle.lifetime > 0)) {
                if (numParticles < MAX_PARTICLES) {
                    particles[numParticles] = particle;
                    ++numParticles;
                } else {
                    // printf("ignoring capacity overflow\n");
                }
            }
        }
    }

    void spawnParticles()
    {
        if (spawnCountdown <= 0) {
            spawnCountdown = MAX_SPAWN_COUNTDOWN;

            if (numParticles == 0) {
                numParticles = 1;
                // fixme: rand() is evil
                unsigned texture = (rand() % 10) << 24;
                unsigned color = backgroundPixel & 0x0ffffff;
                particles[0] = Particle(pos[0], pos[1], 0.001 * (rand() % 1000), texture | color);
            }

        } else {
            --spawnCountdown;
        }
    }

    void updateParticles()
    {
        for (int i = 0; i < numParticles; ++i) {
            // fixme: parameters
            particles[i].update(0.5, forceTotal[0], forceTotal[1], 1.0, 0.9);
        }
    }

    template<typename COORD_MAP>
    void updateForces(const COORD_MAP& hood, const unsigned& nanoStep)
    {
        const CanvasCell& oldSelf = hood[FixedCoord<0, 0>()];

        backgroundPixel = oldSelf.backgroundPixel;
        spawnCountdown = oldSelf.spawnCountdown;
        pos[0] = oldSelf.pos[0];
        pos[1] = oldSelf.pos[1];
        // forceVario[0] = oldSelf.forceVario[0];
        // forceVario[1] = oldSelf.forceVario[1];

        float forceVarioDecay = 0.985;
        forceVario[0] = (hood[FixedCoord< 0, -1>()].forceVario[0] +
                         hood[FixedCoord<-1,  0>()].forceVario[0] +
                         hood[FixedCoord< 1,  0>()].forceVario[0] +
                         hood[FixedCoord< 0,  1>()].forceVario[0]) * 0.25f * forceVarioDecay;
        forceVario[1] = (hood[FixedCoord< 0, -1>()].forceVario[1] +
                         hood[FixedCoord<-1,  0>()].forceVario[1] +
                         hood[FixedCoord< 1,  0>()].forceVario[1] +
                         hood[FixedCoord< 0,  1>()].forceVario[1]) * 0.25f * forceVarioDecay;

        forceSet = oldSelf.forceSet;
        if (forceSet) {
            forceFixed[0] = oldSelf.forceFixed[0];
            forceFixed[1] = oldSelf.forceFixed[1];
        } else {
            forceFixed[0] = (hood[FixedCoord< 0, -1>()].forceFixed[0] +
                             hood[FixedCoord<-1,  0>()].forceFixed[0] +
                             hood[FixedCoord< 1,  0>()].forceFixed[0] +
                             hood[FixedCoord< 0,  1>()].forceFixed[0]) * 0.25f;
            forceFixed[1] = (hood[FixedCoord< 0, -1>()].forceFixed[1] +
                             hood[FixedCoord<-1,  0>()].forceFixed[1] +
                             hood[FixedCoord< 1,  0>()].forceFixed[1] +
                             hood[FixedCoord< 0,  1>()].forceFixed[1]) * 0.25f;
        }

        numParticles = oldSelf.numParticles;
        for (int i = 0; i < numParticles; ++i) {
            particles[i] = oldSelf.particles[i];
        }

        // float gradientLimit = 0.001;

        // if ((gradientX > gradientLimit) || (gradientX < -gradientLimit)) {
        //     forceVario[0] = (std::min)(1.0, 0.01 / gradientX);
        // } else {
        //     forceVario[0] = 0;
        // }

        // if ((gradientY > gradientLimit) || (gradientY < -gradientLimit)) {
        //     forceVario[1] = (std::min)(1.0, 0.01 / gradientY);
        // } else {
        //     forceVario[1] = 0;
        // }

        forceTotal[0] = 0.5f * forceFixed[0] + 0.5f * forceVario[0];
        forceTotal[1] = 0.5f * forceFixed[1] + 0.5f * forceVario[1];
    }
};
}

//BOOST_IS_BITWISE_SERIALIZABLE(LibGeoDecomp::CanvasCell)
BOOST_IS_BITWISE_SERIALIZABLE(LibGeoDecomp::CanvasCell::Particle)

#endif
