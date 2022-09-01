//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_SAMPLE_H
#define EMP_SND_SAMPLE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_sound.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,sample_c, public, sound_i)
private:
    astring m_sName;
    astring m_sFilename;
    float m_fDuration;
    size_t m_stSampleRate;
    vector<vector<float>*> m_vpvfSamples;

public:
    sample_c(void);

    EMP_RETURN bool post_load(void);
    EMP_RETURN bool load(void);

    void destroy(void);

    void set_name(astring& a_sValue) { m_sName = a_sValue; }
    EMP_RETURN c_astring& name(void) const { return m_sName; }

    void set_filename(astring& a_sValue) { m_sFilename = a_sValue; }
    EMP_RETURN c_astring& filename(void) const { return m_sFilename; }


public:
    //>>sound_i
    void prepare_samples(c_size)
    {}
    //EMP_RETURN bool pitched(void) const { return true; }
    //EMP_RETURN float duration(void) const;
    EMP_RETURN size_t sample_count(void) const;
    EMP_RETURN size_t channel_count(void) const;
    //EMP_RETURN emp::mat::v4f::tret solve(
    //    c_size a_stChannel, float const a_fTime) const;
    EMP_RETURN float solve(c_size a_stChannel, c_size a_stSample) const;
    void acquire(void) {}
    void release(void) {}
    EMP_RETURN bool released(void) const { return true; }
    EMP_RETURN soft_synth_data_c* data(void) { return nullptr; }
    //<<sound_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

