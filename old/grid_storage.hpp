#ifndef GRID_STORAGE_HPP
#define GRID_STORAGE_HPP

#include <boost/shared_ptr.hpp>
#include <libgeodecomp/misc/grid.h>
#include <canvascell.h>
#include <particle.h>
#include <model_test/particle.h>

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QtCore>
#include <QVector2D>
#endif

class grid_storage
{
public:
    typedef LibGeoDecomp::Grid<LibGeoDecomp::CanvasCell> grid_type;
    typedef boost::shared_ptr<Particles> grid_ptr;
    virtual void updateParticles(Particles * parts) {}
    virtual grid_ptr next_grid() = 0;
        
    virtual void reset() = 0;
    
    virtual void new_force(float origin_x, float origin_y, float dest_x, float dest_y) = 0;
    
#if !defined(__ANDROID__) || !defined(ANDROID)
    void forceRecorded(const QVector2D& origin, const QVector2D& delta)
    {
        this->new_force(origin.x(), origin.y(), delta.x(), delta.y());
    }
    
    void resetNonVirt()
    {
        this->reset();
    }
#endif
};

#endif
