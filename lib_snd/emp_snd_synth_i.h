//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_SYNTH_I_H
#define EMP_SND_SYNTH_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_layout_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class quality_c;
class scale_i;
class sound_i;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,synth_i, public, layout_i)
public:
    synth_i(void) {}
    virtual ~synth_i(void) = 0;

public:
    //>> synth_i
    virtual EMP_RETURN bool load(c_size a_stIndex) = 0;
    virtual EMP_RETURN sound_i* sound(float const a_fFrequency) = 0;
    virtual EMP_RETURN quality_c const* quality(void) const = 0;
    //virtual EMP_RETURN bool scale(void) const = 0;
    //<< synth_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline synth_i::~synth_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

