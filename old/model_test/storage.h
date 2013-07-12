
#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include <QVector2D>
#include <grid_storage.hpp>
#include <libgeodecomp/misc/supervector.h>

#include <deque>

struct Storage : QObject, grid_storage
{
    Q_OBJECT

    public:

    Storage() {}

    void updateParticles(Particles * parts)
    {
        grid_ptr new_particles(new Particles());
        new_particles->resize(parts->size());

        std::copy(
            parts->posAngle.begin()
          , parts->posAngle.end()
          , new_particles->posAngle.begin()
        );

        std::copy(
            parts->colors.begin()
          , parts->colors.end()
          , new_particles->colors.begin()
        );
        particles.push_back(new_particles);
    }

    grid_ptr next_grid()
    {
        grid_ptr g;
        if(!particles.empty())
        {
            g = particles.front();
            particles.pop_front();
        }
        return g;
    }

    void reset()
    {
    }
    void new_force(float ox, float oy, float dx, float dy)
    {
        QVector2D origin(ox, oy);
        QVector2D delta(dx, dy);
        Q_EMIT forceRecorded(origin, delta);
    }

    Q_SIGNALS:
        void forceRecorded(QVector2D, QVector2D);

    public:
    std::deque<
        boost::shared_ptr<Particles>
    > particles;
};

#endif
