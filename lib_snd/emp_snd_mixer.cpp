//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_mixer.h"

#include "emp_snd_al_device.h"

#include "emp_snd_quality.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

#include "emp_mat_algebra.h"
#include "emp_mat_evaluation.h"

#include "emp_mem_pointer.h"

#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mixer_c::mixer_c(void):
    m_pQuality(),
    m_pSource()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mixer_c::~mixer_c(void)
{ destroy(); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN si16 f_to_si16(float const a_fValue)
{
    //EMP_ASSERT(a_fValue <= 1);
    //EMP_ASSERT(a_fValue >= -1);

    float const fValue = emp::tt::minof(emp::tt::maxof(-1.0f, a_fValue), 1.0f);
    c_si16 si16Value = emp::ast::cast<si16>(emp::ast::cast<float>(emp::tt::max<si16>::value - 10) * fValue);
    return si16Value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mixer_c::create(
    quality_c const& a_rQuality,
    c_size a_stBufferCount, float const a_fBufferDuration)
{
    *m_pQuality = a_rQuality;
    m_stBufferCount = a_stBufferCount / emp::mat::precision_c::array_stride;
    // @@0  lib_snd hc
    m_stBufferCount = emp::tt::maxof<size_t>(m_stBufferCount, 16);
    if (m_stBufferCount <= 1)
    { return false; }

    c_size stDataStride = emp::mat::precision_c::data_stride;
    //c_size stArrayStride = emp::mat::precision_c::array_stride;

    m_fBufferDuration = a_fBufferDuration;///emp::ast::cast<float>(stArrayStride);
    if (m_fBufferDuration <= 0)
    { return false; }

    emp::snd::quality_c& rQuality = *m_pQuality;

    c_size stRate = rQuality.rate();
    c_size stChannelCount = rQuality.channels();
    c_size stDepthBit = rQuality.depth();
    c_size stDepthByte = stDepthBit / 8;
    m_stSamplePerChannel = emp::tt::maxof(
        emp::ast::cast<size_t>(
            emp::ast::cast<float>(stRate) * m_fBufferDuration),// * stArrayStride,
        stDataStride);

    m_fSampleRate = emp::ast::cast<float>(stRate);
    m_fSampleDuration = 1 / m_fSampleRate;

    m_vpafSamples.resize(stChannelCount);
    for (size_t stIndex = 0; stIndex < stChannelCount; ++stIndex)
    {
        emp::cnt::vector<float>* pvfSamples = EMP_ALLOCATE(emp::cnt::vector<float>);
        m_vpafSamples.set(stIndex, pvfSamples);
        pvfSamples->resize(m_stSamplePerChannel);
        for (size_t stSample = 0; stSample < m_stSamplePerChannel; ++stSample)
        { pvfSamples->set(stSample, 0); }
    }

    EMP_ASSERT((stDepthBit % 8) == 0);

    m_vsi16Samples.resize(m_stSamplePerChannel * stChannelCount);
    for (size_t stIndex = 0; stIndex < m_stSamplePerChannel; ++stIndex)
    {
        for (size_t stChannel = 0; stChannel < stChannelCount; ++stChannel)
        {
            emp::cnt::vector<float>* pvfSamples = m_vpafSamples.val(stChannel);

            c_si16 si16Value = f_to_si16(pvfSamples->val(stIndex));
            m_vsi16Samples.set(stIndex * 2 + stChannel, si16Value);
        }
    }

    m_pSource->create();

    m_vpBuffers.resize(m_stBufferCount);
    for (size_t stIndex = 0; stIndex < m_vpBuffers.count(); ++stIndex)
    {
        al_buffer_c* const pBuffer = EMP_ALLOCATE(al_buffer_c);
        pBuffer->create(m_vsi16Samples.first_ptr(),
            emp::ast::cast<ALsizei>(m_vsi16Samples.size() * stDepthByte),
            stChannelCount, emp::ast::cast<ALsizei>(stRate), m_stSamplePerChannel,
            stDepthBit);
        m_pSource->queue(pBuffer);
        m_vpBuffers.set(stIndex, pBuffer);
    }

    // @@0  : play after
    m_pSource->play();

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mixer_c::destroy(void)
{
    *m_pQuality = quality_c();

    m_pSource->destroy();

    emp::cnt::foreach(m_vpBuffers, emp::mem::safe_destroy_delete_reset_t());
    m_vpBuffers.destroy();
    m_stBufferCount = 0;


    m_vpFree.resize(0);

    emp::cnt::foreach(m_vpafSamples, emp::mem::safe_delete_reset_t());
    m_vpafSamples.resize(0);

    m_vsi16Samples.resize(0);

    m_stSamplePerChannel = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mixer_c::update(void)
{
    while (true)
    {
        al_buffer_c* const pBuffer = m_pSource->unqueue_processed();
        if (pBuffer == nullptr)
        { break; }
        m_vpFree.add(pBuffer);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mixer_c::render(void)
{
    static si16 si16LastSample[8];
    static float fLastSample[8];
    if (free_samples() > 0)
    {
        al_buffer_c* const pBuffer = m_vpFree.first_val();
        EMP_ASSERT(pBuffer != nullptr);

        c_size stChannelCount = m_pQuality->channels();

        c_size stSamplePerChannel = m_stSamplePerChannel;

        m_vsi16Samples.resize(0);
        for (size_t stIndex = 0; stIndex < stSamplePerChannel; ++stIndex)
        {
            //float fSum = 0.0f;
            for (size_t stChannel = 0; stChannel < stChannelCount; ++stChannel)
            {
                emp::cnt::vector<float>* pvfSamples = m_vpafSamples.val(stChannel);

                c_float fSample = pvfSamples->val(stIndex);
                if (emp::mat::abs(fSample) > 0.9999999f)
                {
                    //EMP_LOG_WARNING("BURNING #0", fSample);
                }
                c_float fNormValue = fSample;
                c_si16 si16Value = f_to_si16(fNormValue);
                m_vsi16Samples.add(si16Value);
                c_si16 si16Last = si16LastSample[stChannel];
                c_si32 si32Delta = si16Last - si16Value;
                if (emp::mat::abs(si32Delta) > 2000)
                {
                    //EMP_LOG_WARNING("CLIPING : #0", si32Delta);
                }
                c_float fDelta = fLastSample[stChannel] - fSample;
                if (emp::mat::abs(fDelta) >
                    (2000.0f / emp::tt::max<si16>::value))
                {
                    //EMP_LOG_WARNING("FLIPPING : #0", fDelta);
                }
                si16LastSample[stChannel] = si16Value;
                fLastSample[stChannel] = fSample;
                pvfSamples->set(stIndex, 0);

                //fSum += fSample;
            }
        }

        pBuffer->update(m_vsi16Samples.first_ptr());
        m_pSource->queue(pBuffer);
        m_vpFree.remove(0);

        if (m_pSource->is_playing() == false)
        { m_pSource->play(); }

    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t mixer_c::free_samples(void) const
{
    if (m_vpFree.count() > 0)
    { return m_vpafSamples.val(0)->count(); }
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t mixer_c::position_to_sample(float const a_fValue) const
{ return emp::ast::cast<size_t>(m_fSampleRate * a_fValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float mixer_c::sample_duration(void) const
{ return m_fSampleDuration; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN float waveshape_distort( float in ) {
  if(in <= -1.25f) {
    return -0.984375;
  } else if(in >= 1.25f) {
    return 0.984375;
  } else {
    return 1.1f * in - 0.2f * in * in * in;
  }
}*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void mixer_c::add_to_sample(c_size a_stChannel,
    c_size a_stIndex, float const a_fValue)
{
    EMP_ASSERT(a_fValue >= -1);
    EMP_ASSERT(a_fValue <= 1);
    EMP_ASSERT(free_samples() > 0);
    //if (free_samples() > 0)
    //{
    emp::cnt::array<float>* pvfSamples = m_vpafSamples.val(a_stChannel);

    float const fCurrent = pvfSamples->val(a_stIndex);
    EMP_ASSERT(fCurrent >= -1);
    EMP_ASSERT(fCurrent <= 1);

    //float const fNext =
    //  (fCurrent + a_fValue) * (1.0f - 0.5f * emp::mat::abs(a_fEnvelope));
    //float const fNext = (fCurrent + a_fValue) - (fCurrent * a_fValue);
    //float const fNext = waveshape_distort(fCurrent + a_fValue);

    float const fNext = fCurrent + a_fValue;
    float const fClamped = emp::tt::maxof(emp::tt::minof(fNext, 1.0f), -1.0f);
    pvfSamples->set(a_stIndex, fClamped);

    //}
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

