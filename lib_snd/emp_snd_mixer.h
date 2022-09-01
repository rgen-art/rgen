//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_MIXER_H
#define EMP_SND_MIXER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_array.h"
#include "emp_cnt_vector.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*#ifdef EMP_FFT
#include <fftw3.h>
#endif*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class quality_c;
class al_buffer_c;
class al_source_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, mixer_c)
private:
    EMP_POINTER(quality_c) m_pQuality;
    emp::cnt::vector<al_buffer_c*> m_vpBuffers{};
    emp::cnt::vector<al_buffer_c*> m_vpFree{};
    emp::mem::pointer_t<al_source_c> m_pSource;
    emp::cnt::vector<emp::cnt::vector<float>*> m_vpafSamples{};
    emp::cnt::vector<si16> m_vsi16Samples{};
    size_t m_stBufferCount = 0;
    float m_fBufferDuration = 0;
    float m_fSampleRate = 0;
    float m_fSampleDuration = 0;
    size_t m_stSamplePerChannel = 0;

public:
    mixer_c(void);
    ~mixer_c(void);

    EMP_RETURN bool create(quality_c const& a_rQuality, c_size a_stBufferCount, float const a_fBufferDuration);
    void destroy(void);

    void update(void);
    void render(void);

    EMP_RETURN size_t free_samples(void) const;

    EMP_RETURN float sample_duration(void) const;

    void add_to_sample(c_size a_stChannel, c_size a_stIndex, c_float a_fValue)
    {
        // @@0  1d buffer
        EMP_ASSERT_NUMBER(a_fValue);
        emp::cnt::vector<float>* pvfSamples = m_vpafSamples.val(a_stChannel);
        float& rCurrent = pvfSamples->ref(a_stIndex);
        rCurrent += a_fValue;
    }

private:
    EMP_RETURN size_t position_to_sample(float const a_fValue) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

