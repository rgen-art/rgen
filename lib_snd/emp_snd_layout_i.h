//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_LAYOUT_I_H
#define EMP_SND_LAYOUT_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class synth_i;
class scale_i;
class layout_i;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(class, cell_c)
public:
    ui32 index;
    synth_i* synth;
    scale_i* scale;
    layout_i* layout;

public:
    cell_c(void):
        EMP_TT_MAX_CTOR(index),
        synth(nullptr),
        scale(nullptr),
        layout(nullptr)
    {}

    cell_c(c_ui32 a_ui32Index, synth_i* const a_pSynth):
        index(a_ui32Index),
        synth(a_pSynth),
        scale(nullptr),
        layout(nullptr)
    {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, layout_i)
private:
public:
    layout_i(void) {}
    virtual ~layout_i(void) = 0;

public:
    virtual EMP_RETURN c_astring& name(void) const = 0;

    virtual EMP_RETURN size_t cell_count(void) const = 0;
    virtual EMP_RETURN cell_c const cell(c_size a_stIndex) = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE layout_i::~layout_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
