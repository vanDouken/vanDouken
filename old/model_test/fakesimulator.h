#ifndef _fakesimulator_h_
#define _fakesimulator_h_

#include <Qt/QtCore>
#include <QtGui/QImage>
#include <vector>
#include <libgeodecomp/parallelization/serialsimulator.h>
#include <libgeodecomp/parallelization/stripingsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitions/recursivebisectionpartition.h>

#include "canvascell.h"
#include "canvasinitializer.h"
#include "nonhpxfpscounter.h"
#include "particle.h"
#include "particlewidget.h"

class FakeSimulator : public QObject
{
    Q_OBJECT
public:
    // typedef LibGeoDecomp::SerialSimulator<LibGeoDecomp::CanvasCell> SimulatorType;
    typedef LibGeoDecomp::HiParSimulator::HiParSimulator<LibGeoDecomp::CanvasCell, LibGeoDecomp::HiParSimulator::RecursiveBisectionPartition<2> > SimulatorType;

    FakeSimulator(SimulatorType *sim) :
        sim(sim)
    {}

public slots:
    void step() 
    {
        counter.incFrames();
        sim->step();
        if ((counter.getFrames() % 20) == 0)
            std::cout << "calc FPS: " << counter.fps() << "\n";
    }

private:
    SimulatorType *sim;
    NonHPXFPSCounter counter;
};

#endif
