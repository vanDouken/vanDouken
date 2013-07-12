#ifndef _libgeodecomp_examples_flowingcanvas_canvasinitializer_h_
#define _libgeodecomp_examples_flowingcanvas_canvasinitializer_h_

#include <libgeodecomp/io/simpleinitializer.h>
#include <libgeodecomp/io/simpleinitializer.h>
#include <fstream>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "canvascell.h"

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QImage>
#endif

#include "canvascell.h"
#include "forceprimitives.h"
#include <serialize/coordbox.hpp>
#include <serialize/coord.hpp>

namespace LibGeoDecomp {

class CanvasInitializer : public SimpleInitializer<CanvasCell>
{
public:
    CanvasInitializer() :
        SimpleInitializer<CanvasCell>(
            Coord<2>(0, 0), 
            (std::numeric_limits<unsigned>::max)())
    { }

    CanvasInitializer(const std::string& filename, const Coord<2>& dim) :
        SimpleInitializer<CanvasCell>(
            dim, 
            (std::numeric_limits<unsigned>::max)()),
        filename(filename)
    { }

    virtual void grid(GridBase<CanvasCell, 2> *ret)
    {
#if !defined(__ANDROID__) || !defined(ANDROID)
        QImage img = QImage(filename.c_str()).scaled(gridDimensions().x(), gridDimensions().y());

        std::cout << "initializer dimensions: " << gridDimensions().x() << " " << gridDimensions().y() << "\n";
        if(img.isNull())
        {
            std::cout << "couldn't load image " << filename << "\n";
        }

        CoordBox<2> box = ret->boundingBox();
        for (CoordBox<2>::Iterator i = ret->boundingBox().begin(); i != box.end(); ++i) {
            bool setForce = false;
            FloatCoord<2> force;

            for (std::size_t j = 0; j < shapes.size(); ++j) {
                shapes[j]->initCell(&force, &setForce, *i);
            }

            // force alpha channel to 0xff to ensure all particles are opaque
            unsigned pixel = 0xff000000 + img.pixel(i->x(), i->y());
            ret->at(*i) = CanvasCell(pixel, *i, setForce, force, rand() % CanvasCell::MAX_SPAWN_COUNTDOWN);
        }
#endif
    }

    template <typename Archive>
    void serialize(Archive & ar, unsigned)
    {
        ar & boost::serialization::base_object<SimpleInitializer<CanvasCell> >(*this);
        ar & filename;
    }

    static Coord<2> getPPMDim(const std::string& filename)
    {
        std::ifstream infile(filename.c_str());
        if (!infile) {
            throw std::logic_error("unable to open input file");
        }
        std::string format;
        int infileWidth;
        int infileHeight;
        infile >> format;
        infile >> infileWidth;
        infile >> infileHeight;

        return Coord<2>(infileWidth, infileHeight);
    }

    template<class SHAPE>
    void addShape(const SHAPE& shape)
    {
        shapes << boost::shared_ptr<ForcePrimitives::Base>(new SHAPE(shape));
    }
    
    std::string filename;
    SuperVector<boost::shared_ptr<ForcePrimitives::Base> > shapes;

};

}

#endif
