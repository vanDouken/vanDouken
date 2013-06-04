#ifndef _nonhpxframecounter_h_
#define _nonhpxframecounter_h_

#include <sys/time.h>

// fixme: move this class and chronometer to libgeodecomp/misc
class NonHPXFPSCounter
{
public:
    NonHPXFPSCounter() :
        frames(0),
        tStart(getUTtime())
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
        double seconds = (getUTtime() - tStart) * 1e-6;
        return frames / seconds;
    }

private:
    long long frames;
    long long tStart;

    long long getUTtime()
    {
        timeval t;
        gettimeofday(&t, 0);
        return (long long)t.tv_sec * 1000000 + t.tv_usec;
    }
};

#endif
