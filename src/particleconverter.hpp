//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_PARTICLECONVERTER_HPP
#define VANDOUKEN_PARTICLECONVERTER_HPP

#include "cell.hpp"
#include "forceprimitives.hpp"
#include "particle.hpp"

#include <libgeodecomp/misc/supervector.h>

namespace vandouken {
    class ParticleConverter
    {
    public:
        template <typename BUFFER>
        void operator()(const Cell& cell,
            const LibGeoDecomp::Coord<2>& globalDimensions, BUFFER& buffer)
        {
            int halfWidth(globalDimensions.x() / 2);
            int halfHeight(globalDimensions.y() / 2);

            if(buffer.capacity() < buffer.size() + cell.numParticles)
            {
                buffer.reserve(buffer.size() + cell.numParticles);
            }

            for(int i = 0; i < cell.numParticles; ++i)
            {
                const Cell::Particle& particle = cell.particles[i];
                float posX = (0.20 / 1.5) * (particle.pos[0] - halfWidth);
                float posY = (0.20 / 1.5) * (particle.pos[1] - halfHeight);

                float depthOffset = 0;
                if (particle.lifetime < FADE_IN_OUT) {
                    depthOffset = FADE_IN_OUT - particle.lifetime;
                }

                if ((DEFAULT_PARTICLE_LIFETIME - particle.lifetime) < FADE_IN_OUT) {
                    depthOffset
                        = particle.lifetime - DEFAULT_PARTICLE_LIFETIME + FADE_IN_OUT;
                }

                float posZ = -100 + particle.pos[2] - 0.01 * depthOffset;;
                float angle = ForcePrimitives::angle(particle.vel[0], particle.vel[1]);
                buffer.addParticle(
                    Particle(posX, posY, posZ, angle, particle.color));
            }
        }
    private:
    };
}

#endif
