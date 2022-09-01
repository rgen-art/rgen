//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_RANDOM_H
#define EMP_MAT_RANDOM_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <random>
#include <chrono>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

struct random_float_s
{

public:

    void reset_seed(void)
    {
        reset_seed(static_cast<ui32>(std::chrono::system_clock::now().time_since_epoch().count()));
    }

    void reset_seed(uint32_t const a_iSeed)
    {
        seed = a_iSeed;
        gen = std::mt19937(seed);
        dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }

    void reset_dist()
    {
        gen = std::mt19937(seed);
        dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }

    EMP_INLINE EMP_RETURN float value()
    {
        return dist(gen);
    }

private:
    std::mt19937 gen;
    std::mt19937::result_type seed;
    std::uniform_real_distribution<float> dist;
};





} }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

