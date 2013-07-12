#ifndef _particlewriter_h_
#define _particlewriter_h_

#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/misc/supervector.h>
#include <libgeodecomp/mpilayer/mpilayer.h>

#include "canvascell.h"
#include "forceprimitives.h"
#include "particle.h"
#include "particlewidget.h"

using LibGeoDecomp::Coord;
using LibGeoDecomp::CanvasCell;
using LibGeoDecomp::FloatCoord;
using LibGeoDecomp::Streak;
using LibGeoDecomp::SuperVector;
using LibGeoDecomp::ParallelWriter;

class ParticleWriter : public ParallelWriter<CanvasCell>
{
public:
    typedef LibGeoDecomp::Region<2> MyRegion;
    typedef typename ParallelWriter<CanvasCell>::GridType GridType;

    ParticleWriter(
        grid_storage *widget,
        const Coord<2>& dimensions,
        const unsigned& period,
        MPI::Datatype mpiDatatype) :
        ParallelWriter<CanvasCell>("foo", period),
        widget(widget),
        datatype(mpiDatatype),
        halfWidth(dimensions.x() / 2),
        halfHeight(dimensions.y() / 2)
    {}

    virtual void stepFinished(
        const GridType& grid, 
        const MyRegion& validRegion, 
        const Coord<2>& globalDimensions,
        unsigned step, 
        LibGeoDecomp::WriterEvent event, 
        bool lastCall) 
    {
        if (step % getPeriod() != 0) {
            return;
        }

        int cursor = 0;
        for (MyRegion::Iterator i = validRegion.begin();
             i != validRegion.end();
             ++i) {
            Coord<2> c = *i;
            const CanvasCell& cell = grid.at(c);
            int nextSize = cursor + cell.numParticles;
            if (particles.size() < nextSize) {
                particles.resize(nextSize);
            }
            for (int j = 0; j < cell.numParticles; ++j) {
                particleToParticle(cursor + j, cell.particles[j]);
            }
            cursor += cell.numParticles;
        }

        LibGeoDecomp::MPILayer mpiLayer;
        SuperVector<int> particleCounts = mpiLayer.gather(cursor, 0);
        SuperVector<int> colorCounts = particleCounts;

        int totalParticleCount = 0;
        if (mpiLayer.rank() == 0) {
            for (int i = 0; i < particleCounts.size(); ++i) {
                totalParticleCount += particleCounts[i];
                particleCounts[i] *= 4;
            }
            if (allParticles.size() != totalParticleCount + (totalParticleCount % 140)) {
                allParticles.resize(totalParticleCount + (totalParticleCount % 140));
            }
        }

        // fixme: gather inner region AND ghost 
        mpiLayer.gatherV(&particles.posAngle[0], cursor * 4, particleCounts, 0,
                      &allParticles.posAngle, MPI_FLOAT);
        mpiLayer.gatherV(&particles.colors[0], cursor, colorCounts, 0,
                      &allParticles.colors, MPI_INT);

        if (mpiLayer.rank() == 0) {
            if (allParticles.size() > 0) {
                if (widget) {
                    if (lastCall)
                        widget->updateParticles(&allParticles);
                }                                  
            }
        }
    }

private:
    grid_storage *widget;
    //SuperVector<Particle> particles;
    //SuperVector<Particle> allParticles;
    Particles particles;
    Particles allParticles;
    MPI::Datatype datatype;
    int halfWidth;
    int halfHeight;

    void particleToParticle(int i, const CanvasCell::Particle& particle)
    {
        float posX = (0.20 / 1.5) * (particle.pos[0] - halfWidth);
        float posY = (0.20 / 1.5) * (particle.pos[1] - halfHeight);
                
        float depthOffset = 0;
        if (particle.lifetime < FADE_IN_OUT) {
            depthOffset = FADE_IN_OUT - particle.lifetime;
        }

        if ((DEFAULT_PARTICLE_LIFETIME - particle.lifetime) < FADE_IN_OUT) {
            depthOffset = particle.lifetime - DEFAULT_PARTICLE_LIFETIME + FADE_IN_OUT;
        }

        float posZ = -100 + particle.pos[2] - 0.01 * depthOffset;;
        float angle = ForcePrimitives::angle(particle.vel[0], particle.vel[1]);
        particles.setParticle(i, posX, posY, posZ, angle, particle.color);
        //return Particle(posX, posY, posZ, angle, particle.color);
    }

    char alpha(unsigned color)
    {
        return color >> 24;
    }
};

#endif
