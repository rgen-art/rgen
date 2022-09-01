//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_SOUND_I_H
#define EMP_SND_SOUND_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd { class soft_synth_data_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, sound_i)
public:
    sound_i(void) {}
    virtual ~sound_i(void) {}

public:
    //>> sound_i
    virtual void prepare_samples(c_size a_stIndex) = 0;
    virtual EMP_RETURN size_t sample_count(void) const = 0;
    virtual EMP_RETURN size_t channel_count(void) const = 0;
    virtual EMP_RETURN float solve(c_size a_stChannel, c_size a_stSample) const = 0;
    virtual void acquire(void) = 0;
    virtual void release(void) = 0;
    virtual EMP_RETURN bool released(void) const = 0;
    virtual EMP_RETURN soft_synth_data_c* data(void) = 0;
    //<< sound_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

