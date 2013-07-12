#ifndef _particle_h_
#define _particle_h_

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <QColor>
#else
inline int qRed(unsigned int rgb)
{
    return ((rgb >> 16) & 0xff); 
}

inline int qGreen(unsigned int rgb)
{
    return ((rgb >> 8) & 0xff); 
}

inline int qBlue(unsigned int rgb)
{
    return (rgb & 0xff); 
}

inline int qAlpha(unsigned int rgb)
{
    return rgb >> 24; 
}
#endif


struct Particles
{
    void setParticle(std::size_t i, float posX, float posY, float posZ, float angle, boost::uint32_t color)
    {
        posAngle[i * 4 + 0] = /*static_cast<boost::uint32_t>*/std::ceil(posX * 100.f);
        posAngle[i * 4  + 1] = /*static_cast<boost::uint32_t>*/std::ceil(posY * 100.f);
        posAngle[i * 4  + 2] = /*static_cast<boost::uint32_t>*/std::ceil(posZ * 100.f);
        posAngle[i * 4  + 3] = /*static_cast<boost::uint32_t>*/std::ceil(angle * 100.f);

        colors[i] = color;
    }

    std::size_t size()
    {
        return posAngle.size() / 4;
    }

    void resize(std::size_t size)
    {
        posAngle.resize(size * 4);
        colors.resize(size * 4);
    }

    template <typename Archive>
    void serialize(Archive & ar, unsigned)
    {
        ar & posAngle;
        ar & colors;
    }

    LibGeoDecomp::SuperVector<float> posAngle;
    LibGeoDecomp::SuperVector<boost::uint32_t> colors;
};

class Particle
{
public:
    inline Particle(float posX=0, float posY=0, float posZ=0, float angle=0, unsigned color=0) :
        posX_ (posX ),
        posY_ (posY ),
        posZ_ (posZ ),
        angle_(angle),
        color(color)
    {}

    float posX_;
    float posY_;
    float posZ_;  
    float angle_;
    // coding: AARRGGBB, use color for color multiply of texture and alpha for texture index?
    unsigned color;

    float posX() const
    {
        return posX_ * 0.01f;
    }

    float posY() const
    {
        return posY_ * 0.01f;
    }

    float posZ() const
    {
        return posZ_ * 0.01f;
    }

    float angle() const
    {
        return angle_ * 0.01f;
    }
};
BOOST_IS_BITWISE_SERIALIZABLE(Particle)

#endif
