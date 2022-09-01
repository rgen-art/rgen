//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_MEDIA_H
#define EMP_SND_MEDIA_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_media_i.h"
#include "emp_snd_sound_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

#include "emp_mat_evaluation.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef EMP_FFT
//#define EMP_FFT
// @@0  : fftw
#endif

#ifdef EMP_FFT
// @@0  : remove inclusion
#include <fftw3.h>
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace evl { template <typename> class envelope_t; } }
namespace emp { namespace evl { template <typename> class spectrum_t; } }
namespace emp { namespace evl { template <typename> class delay_t; } }
namespace emp { namespace evl { template <typename> class reverb_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef emp::mat::precision_c::type precision_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class envelope_c;
class sound_i;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, spectrum_t)
private:
    using tool_d = emp::mat::simd_v4<precision_d>;
    using vector_d = typename tool_d::type;

private:
#ifdef EMP_FFT
    size_t m_stFftSampleCount;
    size_t m_stFftOutputCount;
    size_t m_stCurrentSample;
    emp::cnt::vector<T> m_vdHamming;
    emp::cnt::vector<T> m_vdSignal;
    emp::cnt::vector<T> m_vfFrequencies;
    double* m_pFftIn;
    fftw_complex* m_pFftOut;
    fftw_plan m_FftPlan;
#endif

public:
    spectrum_t(void);
    ~spectrum_t(void);

public:
    void create(void);
    void destroy(void);

public:
    EMP_RETURN vector_d amplitude(vector_d const a_fFrequency) const;

    void add_sample(vector_d const a_fInput);

#ifdef EMP_FFT
    void compute_fft(void);
#endif
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, delay_t)
private:
    emp::cnt::vector<T> m_vtSamples;
    size_t m_stIndex;

public:
    delay_t(void):
        m_vtSamples(),
        m_stIndex()
    {}

    ~delay_t(void)
    {}

public:
    EMP_RETURN T update(T const a_tSignal, T const a_tDelay, T const a_tDecay, c_bool a_bFeedBack)
    {
        c_size stDelayCount = emp::ast::cast<size_t>(a_tDelay * 44.1);
        if (stDelayCount != m_vtSamples.count())
        {
            m_vtSamples.resize(stDelayCount);
            m_stIndex = 0;
            //for (size_t stIndex = 0; stIndex < stDelayCount; ++stIndex)
            //{ m_vtSamples.set(stIndex, 0); }
        }

        if (stDelayCount > 0)
        {
            c_size stIndex = m_stIndex;
            m_stIndex = (stIndex + 1) % stDelayCount;
            T const tSample = m_vtSamples.val(m_stIndex);

            T const tOutput = a_tSignal + tSample * a_tDecay;
            m_vtSamples.set(stIndex, a_bFeedBack ? tOutput : a_tSignal);
            return tOutput;
        }
        return a_tSignal;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, reverb_t)
private:
    emp::cnt::vector<delay_t<T>*> m_vpDelays;
    emp::cnt::vector<T> m_vtDelays;

public:
    reverb_t(void):
        m_vpDelays(),
        m_vtDelays()
    {}

    ~reverb_t(void)
    {}

public:
    EMP_RETURN T update(T const a_tSignal, c_size a_stTap, T const a_tDelay, T const a_tDecay, c_bool a_bFeedBack)
    {
        T tOutput = a_tSignal;
        for (size_t stIndex = 0; stIndex < a_stTap; ++stIndex)
        {
            delay_t<T>* const pDelay = get_tap_delay(m_vpDelays, stIndex);
            tOutput = pDelay->update(tOutput, a_tDelay, a_tDecay, a_bFeedBack);
        }

        return tOutput;
    }

private:
    EMP_RETURN delay_t<T>* get_tap_delay(
        emp::cnt::vector<delay_t<T>*>& a_rvpDelays, c_size a_stIndex)
    {
        if (a_stIndex < a_rvpDelays.count())
        { return a_rvpDelays.val(a_stIndex); }
        delay_t<T>* const pDelay = EMP_ALLOCATE(delay_t<T>);
        a_rvpDelays.add(pDelay);
        return pDelay;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,media_c, public, media_i)
private:
    using tool_d = emp::mat::simd_v4<precision_d>;
    using vector_d = typename tool_d::type;

private:
    EMP_(class, env_c)
    public:
        float time;
        env_c(void);
    };

private:
    emp::cnt::vector<env_c> m_vEnvelopes;
    emp::cnt::vector<spectrum_t<precision_d>*> m_vpSpectrums;
    emp::cnt::vector<delay_t<precision_d>*> m_vpDelays;
    emp::cnt::vector<reverb_t<precision_d>*> m_vpReverbs;

    emp::snd::sound_i* m_pSound;
    float m_fVolume;
    size_t m_stSound;
    state_e m_eState;

public:
    media_c(void);

public:
    void create(emp::snd::sound_i* const a_pSound, float const a_fVolume);

    void destroy(void);

public:
    //>>media_i
    EMP_RETURN state_e state(void) const { return m_eState; }
    //<<media_i

private:
    //>>media_i
    void play(void);
    void stop(void);
    void pre_process(void);
    EMP_RETURN bool move(float const a_fDelta);
    EMP_RETURN size_t channel_count(void) const;
    EMP_RETURN float sample(c_size a_stChannel)
    { return m_pSound->solve(a_stChannel, m_stSound) * m_fVolume; }
    //<<media_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

