//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_SOUND_H
#define EMP_SND_SOUND_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_sound_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { class nurbs_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,sound_c, public, sound_i)
private:
    astring m_sName;
    nurbs_c* m_pNurbs;
    size_t m_stChannelCount;

public:
    sound_c(void);
    ~sound_c(void);

    void create(void);
    void destroy(void);

    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }
    EMP_RETURN c_astring& name(void) const { return m_sName; }

    void set_nurbs(nurbs_c* const a_pValue) { m_pNurbs = a_pValue; }

    EMP_RETURN nurbs_c const& nurbs(void) const { return *m_pNurbs; }
    EMP_RETURN nurbs_c& nurbs(void) { return *m_pNurbs; }

    EMP_RETURN float begin(void) const;
    EMP_RETURN float end(void) const;

    //>>sound_i
    void prepare_samples(c_size) {}
    //EMP_RETURN bool pitched(void) const { return false; }
    EMP_RETURN size_t sample_count(void) const;
    EMP_RETURN size_t channel_count(void) const { return 1; }
    EMP_RETURN float solve(c_size a_stIndex, c_size a_stSample) const;
    EMP_RETURN bool load(void) { return true; }
    void acquire(void) {}
    void release(void) {}
    EMP_RETURN bool released(void) const { return true; }
    EMP_RETURN soft_synth_data_c* data(void) { return nullptr; }
    //<<sound_i

private:
    EMP_RETURN float duration(void) const;
    EMP_RETURN float solve(c_size a_stIndex, c_float a_fTime) const;
    EMP_RETURN float sample(c_float a_fParameter) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

