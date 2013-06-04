#include <QApplication>

#include "canvascell.h"
#include "canvasinitializer.h"
#include "fakesimulator.h"
#include "forcesteerer.h"
#include "particlewriter.h"
#include <model_test/storage.h>

#include <libgeodecomp/io/tracingwriter.h>
#include <libgeodecomp/loadbalancer/noopbalancer.h>
#include <libgeodecomp/parallelization/hiparsimulator.h>
#include <libgeodecomp/parallelization/stripingsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator/partitions/recursivebisectionpartition.h>

using LibGeoDecomp::HiParSimulator::HiParSimulator;
using LibGeoDecomp::HiParSimulator::RecursiveBisectionPartition;
using LibGeoDecomp::MPILayer;
using LibGeoDecomp::NoOpBalancer;
using LibGeoDecomp::StripingSimulator;
using LibGeoDecomp::TracingWriter;

using LibGeoDecomp::CanvasCell;
using LibGeoDecomp::CanvasInitializer;

CanvasInitializer *getInit1()
{
    std::string input_file = "starry_night_upper_right.ppm";
    QImage image(input_file.c_str());
    LibGeoDecomp::Coord<2> dim(image.width(), image.height());
    std::cout << "Image dimensions: " << image.width() << " " << image.height() << "\n";
    CanvasInitializer *init = new CanvasInitializer(input_file, dim);
    init->addShape(ForcePrimitives::Circle(Coord<2>(260,  62), 15, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(260,  62), 20, 0.0 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>( 67, 120), 70, 0.2 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>( 45,  62), 90, 0.15));
    init->addShape(ForcePrimitives::Circle(Coord<2>(125,  30), 10, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>( 35,  22),  5, 0.4 ));
    init->addShape(ForcePrimitives::Circle(Coord<2>(167,  82), 10, 0.4 ));
    return init;
}

CanvasInitializer *getInit2()
{
    std::string input_file = "starry_night.ppm";
    QImage image(input_file.c_str());
    LibGeoDecomp::Coord<2> dim(image.width(), image.height());
    CanvasInitializer *init = new CanvasInitializer(input_file, dim);
    // init->addShape(ForcePrimitives::Circle(Coord<2>(260,  62), 15, 0.4 ));
    // init->addShape(ForcePrimitives::Circle(Coord<2>(260,  62), 20, 0.0 ));
    // init->addShape(ForcePrimitives::Circle(Coord<2>( 67, 120), 70, 0.2 ));
    // init->addShape(ForcePrimitives::Circle(Coord<2>( 45,  62), 90, 0.15));
    // init->addShape(ForcePrimitives::Circle(Coord<2>(125,  30), 10, 0.4 ));
    // init->addShape(ForcePrimitives::Circle(Coord<2>( 35,  22),  5, 0.4 ));
    // init->addShape(ForcePrimitives::Circle(Coord<2>(167,  82), 10, 0.4 ));
    return init;
}

int main(int argc, char **argv)
{
    MPI::Init(argc, argv);
    QApplication app(argc, argv);
    LibGeoDecomp::Typemaps::initializeMaps();

    MPI::Aint displacements[] = {0};
    MPI::Datatype memberTypes[] = {MPI::CHAR};
    int lengths[] = {sizeof(CanvasCell)};
    MPI::Datatype objType;
    objType = 
        MPI::Datatype::Create_struct(1, lengths, displacements, memberTypes);
    objType.Commit();

    MPI::Datatype particleType;
    lengths[0] = sizeof(Particle);
    particleType = 
        MPI::Datatype::Create_struct(1, lengths, displacements, memberTypes);
    particleType.Commit();

    ParticleWidget *widget = 0;
    CanvasInitializer *init = getInit1();
    
    HiParSimulator<CanvasCell, RecursiveBisectionPartition<2> > *sim = new HiParSimulator<CanvasCell, RecursiveBisectionPartition<2> >(
        init,
        MPILayer().rank() ? 0 : new NoOpBalancer(), 
        1000,
        1,
        particleType);
    std::cout << "Simulator created ...\n";
    // HiParSimulator<CanvasCell, RecursiveBisectionPartition<2> > sim(
    //     new CanvasInitializer(dim.x(), dim.y()),
    //     MPILayer().rank() ? 0 : new NoOpBalancer(), 
    //     1000,
    //     1,
    //     particleType);

    ForceSteerer *steerer = new ForceSteerer(1);
    sim->addSteerer(steerer);
    std::cout << "Steerer added ...\n";

    Storage * storage = 0;
    if (MPILayer().rank() == 0) {
        storage = new Storage;
        widget = new ParticleWidget(*storage, init->gridDimensions(), Qt::black);
        widget->resize(1000, 500);
        widget->show();
        std::cout << "Widget created ...\n";
    }

    sim->addWriter(new TracingWriter<CanvasCell>(100, 100000));
    sim->addWriter(new ParticleWriter(storage, init->gridDimensions(), 1, particleType));

    FakeSimulator fakeSim(sim);

    // DebugOutput logger;
    QTimer timer;
    QTimer timerGL;

    QObject::connect(&timer, SIGNAL(timeout()),
                     &fakeSim, SLOT(step()));
    // QObject::connect(&widget, SIGNAL(newFrame()),
    //                  &logger, SLOT(newFrame()));
    // QObject::connect(&widget, SIGNAL(forceRecorded(QPoint, QPoint)),
    //                  &logger, SLOT(addForce(QPoint, QPoint)));
    QObject::connect(storage,     SIGNAL(forceRecorded(QVector2D, QVector2D)),
                     steerer,    SLOT(addForce(QVector2D, QVector2D))); 
    QObject::connect(&timerGL, SIGNAL(timeout()), widget, SLOT(updateGL()));

    timer.start(10);
    timerGL.start(10);
    app.exec();

    delete sim;
    MPI::Finalize();
}
