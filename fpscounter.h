#ifndef _libgeodecomp_examples_latticegas_framecounter_h_
#define _libgeodecomp_examples_latticegas_framecounter_h_

#include <hpx/util/high_resolution_timer.hpp>

// fixme: move this class and chronometer to libgeodecomp/misc
class FPSCounter
{
public:
    FPSCounter() :
        frames(0)
    {}

    void incFrames()
    {
        ++frames;
    }

    long long getFrames()
    {
        return frames;
    }

    double fps()
    {
        double elapsed = timer.elapsed();
        // keep two digits
        double buf = 100 * frames / elapsed;
        return buf * 0.01;
    }

private:
    hpx::util::high_resolution_timer timer;
    long long frames;
};

#endif
