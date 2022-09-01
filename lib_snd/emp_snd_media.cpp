//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_media.h"

#include "emp_snd_envelope.h"
#include "emp_snd_soft_synth.h"
#include "emp_snd_sound.h"
#include "emp_snd_player.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
spectrum_t<T>::spectrum_t(void)
   #ifdef EMP_FFT
:
    m_stFftSampleCount(),
    m_stFftOutputCount(),
    m_stCurrentSample(),
    m_vdHamming(),
    m_vdSignal(),
    m_vfFrequencies(),
    m_pFftIn(),
    m_pFftOut(),
    m_FftPlan()
#endif
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
spectrum_t<T>::~spectrum_t(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void spectrum_t<T>::create(void)
{
#ifdef EMP_FFT
    m_stFftSampleCount = 2048 * 8;
    m_stFftOutputCount = 1 + m_stFftSampleCount / 2;

    m_pFftIn =
        (double*) fftw_malloc(sizeof(double) * m_stFftSampleCount);
    m_pFftOut =
        (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_stFftOutputCount);

    m_FftPlan = fftw_plan_dft_r2c_1d(
        emp::ast::cast<int>(m_stFftSampleCount),
        m_pFftIn, m_pFftOut,
        FFTW_ESTIMATE);
        //FFTW_DESTROY_INPUT | FFTW_EXHAUSTIVE);

    m_vdHamming.resize(m_stFftSampleCount);
    for (size_t stIndex = 0; stIndex < m_stFftSampleCount; ++stIndex)
    {
        T const dH =
            emp::ast::cast<T>(0.54 - 0.46) * emp::mat::cos(
                emp::ast::cast<T>(2.0) *
                emp::mat::pi<T>() *
                emp::ast::cast<T>(stIndex) /
                emp::ast::cast<T>(m_stFftSampleCount - 1));
        m_vdHamming.set(stIndex, dH);
    }

    m_vfFrequencies.resize(m_stFftOutputCount);
    for (size_t stIndex = 0; stIndex < m_stFftOutputCount; ++stIndex)
    { m_vfFrequencies.set(stIndex, 0.00001f); }

    m_vdSignal.resize(m_stFftSampleCount);
    for (size_t stIndex = 0; stIndex < m_stFftSampleCount; ++stIndex)
    { m_vdSignal.set(stIndex, 0); }
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void spectrum_t<T>::destroy(void)
{
#ifdef EMP_FFT
    m_vfFrequencies.resize(0);

    if (m_FftPlan)
    {
        fftw_destroy_plan(m_FftPlan);
        m_FftPlan = 0;
    }

    if (m_pFftIn)
    {
        fftw_free(m_pFftIn);
        m_pFftIn = nullptr;
    }

    if (m_pFftOut)
    {
        fftw_free(m_pFftOut);
        m_pFftOut = nullptr;
    }

    m_vdHamming.resize(0);
    m_vdSignal.resize(0);

    m_stCurrentSample = 0;
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN typename spectrum_t<T>::vector_d spectrum_t<T>::amplitude(
    vector_d const
    #ifdef EMP_FFT
    a_fFrequency
    #endif
    ) const
{
#ifdef EMP_FFT
    vector_d const tAbsFrequency = tool_d::abs(a_fFrequency);

    vector_d const tBinResolution = tool_d::set(
        emp::ast::cast<T>(44100) / emp::ast::cast<T>(m_stFftSampleCount));

    vector_d const tBinIndex = tool_d::div(tAbsFrequency, tBinResolution);

    vector_d const tBin = tool_d::min(tool_d::max(
        tool_d::zero(), tBinIndex),
            tool_d::set(emp::ast::cast<T>(m_vfFrequencies.count() - 1)));

    c_size stBin0 = emp::ast::cast<size_t>(tool_d::x(tBin));
    c_size stBin1 = emp::ast::cast<size_t>(tool_d::y(tBin));
    c_size stBin2 = emp::ast::cast<size_t>(tool_d::z(tBin));
    c_size stBin3 = emp::ast::cast<size_t>(tool_d::w(tBin));

    return tool_d::set(
        m_vfFrequencies.val(stBin0),
        m_vfFrequencies.val(stBin1),
        m_vfFrequencies.val(stBin2),
        m_vfFrequencies.val(stBin3));
#else
    return tool_d::set(0.00001f);
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void spectrum_t<T>::add_sample(vector_d const
#ifdef EMP_FFT
    a_fInput
#endif
    )
{
#ifdef EMP_FFT
    m_vdSignal.set(m_stCurrentSample, tool_d::x(a_fInput));
    m_stCurrentSample = (m_stCurrentSample + 1) % m_stFftSampleCount;

    m_vdSignal.set(m_stCurrentSample, tool_d::y(a_fInput));
    m_stCurrentSample = (m_stCurrentSample + 1) % m_stFftSampleCount;

    m_vdSignal.set(m_stCurrentSample, tool_d::z(a_fInput));
    m_stCurrentSample = (m_stCurrentSample + 1) % m_stFftSampleCount;

    m_vdSignal.set(m_stCurrentSample, tool_d::w(a_fInput));
    m_stCurrentSample = (m_stCurrentSample + 1) % m_stFftSampleCount;
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_FFT
template <typename T>
void spectrum_t<T>::compute_fft(void)
{
    { c_size stCount = m_stFftSampleCount;
    //EMP_LOG_INFO("___________________________________");
    //EMP_LOG_INFO("___________________________________");
    //EMP_LOG_INFO("___________________________________");
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = (m_stCurrentSample + stIndex) % stCount;
        c_double dHamming = m_vdHamming.val(stIndex);
        c_double dSignal = m_vdSignal.val(stOffset);
        //EMP_LOG_INFO("HAMMING #0, SIGNAL #1", dHamming, dSignal);
        m_pFftIn[stIndex] = dHamming * dSignal;
    } }

    fftw_execute(m_FftPlan);

    { c_size stCount = m_stFftOutputCount;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_double dR = m_pFftOut[stIndex][0];
        c_double dI = m_pFftOut[stIndex][1];
        c_double dsqM = dR * dR + dI * dI;
        //c_double dF = 10 * emp::mat::log10(dsqM);
        c_double dF = dsqM;
        m_vfFrequencies.set(stIndex, emp::ast::cast<float>(dF));
    } }
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
media_c::env_c::env_c(void):
    time(0.0f)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
media_c::media_c(void):
m_vEnvelopes(),
m_vpSpectrums(),
m_vpDelays(),
m_vpReverbs(),
m_pSound(),
m_fVolume(1.0f),
m_stSound(),
m_eState(state_e::Stopped)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void media_c::create(emp::snd::sound_i* const a_pSound, c_float a_fVolume)
{
    destroy();

    EMP_ASSERT(a_pSound->released());

    for (size_t stIndex = 0; stIndex < a_pSound->data()->envelopes().count();
        ++stIndex)
    { m_vEnvelopes.add(env_c()); }

    for (size_t stIndex = 0; stIndex < a_pSound->data()->spectrums().count();
        ++stIndex)
    {
        emp::snd::spectrum_t<precision_d>* const pSpectrum =
            EMP_ALLOCATE(emp::snd::spectrum_t<precision_d>);
        pSpectrum->create();
        m_vpSpectrums.add(pSpectrum);
    }

    for (size_t stIndex = 0; stIndex < a_pSound->data()->delays().count();
        ++stIndex)
    {
        emp::snd::delay_t<precision_d>* const pDelay =
            EMP_ALLOCATE(emp::snd::delay_t<precision_d>);
        m_vpDelays.add(pDelay);
    }

    for (size_t stIndex = 0; stIndex < a_pSound->data()->reverbs().count();
        ++stIndex)
    {
        emp::snd::reverb_t<precision_d>* const pReverb =
            EMP_ALLOCATE(emp::snd::reverb_t<precision_d>);
        m_vpReverbs.add(pReverb);
    }

    EMP_ASSERT(a_pSound != nullptr);

    m_pSound = a_pSound;
    m_pSound->acquire();
    m_fVolume = a_fVolume;

    m_stSound = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void media_c::destroy(void)
{
    m_stSound = 0;
    m_fVolume = 1.0f;
    m_eState = state_e::Stopped;
    m_vEnvelopes.resize(0);

    if (m_pSound)
    {
        { c_size stCount = m_pSound->data()->spectrums().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::mat::spectrum_t<precision_d>* const pSpectrum =
                m_pSound->data()->spectrums().val(stIndex);
            pSpectrum->set_data(nullptr);
        } }

        { c_size stCount = m_pSound->data()->delays().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::mat::delay_t<precision_d>* const pDelay =
                m_pSound->data()->delays().val(stIndex);
            pDelay->set_data(nullptr);
        } }

        { m_pSound->release(); }
        m_pSound = nullptr;
    }

    emp::cnt::foreach(m_vpSpectrums, emp::mem::safe_delete_reset_t());
    m_vpSpectrums.resize(0);

    emp::cnt::foreach(m_vpDelays, emp::mem::safe_delete_reset_t());
    m_vpDelays.resize(0);

    emp::cnt::foreach(m_vpReverbs, emp::mem::safe_delete_reset_t());
    m_vpReverbs.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void media_c::play(void)
{
    EMP_ASSERT(state() == state_e::Stopped);
    m_eState = state_e::Playing;
    m_stSound = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void media_c::stop(void)
{
    if (m_eState == state_e::Playing)
    { m_eState = state_e::Stopping; }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void media_c::pre_process(void)
{
#ifdef EMP_FFT
    { c_size stCount = m_vpSpectrums.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::snd::spectrum_t<precision_d>* const pSndSpectrum =
            m_vpSpectrums.val(stIndex);
        emp::mat::spectrum_t<precision_d>* const pEvlSpectrum =
            m_pSound->data().spectrums().val(stIndex);
        pEvlSpectrum->set_data(pSndSpectrum);
        pSndSpectrum->compute_fft();
    } }
#endif

    { c_size stCount = m_vpDelays.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::snd::delay_t<precision_d>* const pSndDelay =
            m_vpDelays.val(stIndex);
        emp::mat::delay_t<precision_d>* const pEvlDelay =
            m_pSound->data()->delays().val(stIndex);
        pEvlDelay->set_data(pSndDelay);
    } }

    { c_size stCount = m_vpReverbs.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::snd::reverb_t<precision_d>* const pSndReverb =
            m_vpReverbs.val(stIndex);
        emp::mat::reverb_t<precision_d>* const pEvlReverb =
            m_pSound->data()->reverbs().val(stIndex);
        pEvlReverb->set_data(pSndReverb);
    } }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool media_c::move(float const a_fDelta)
{
    emp::snd::c_state_e eState = state();
    //EMP_ASSERT(eState != state_e::Stopped);
    if (eState == state_e::Stopped)
    { return false; }

#ifdef EMP_FFT
    { c_size stCount = m_vpSpectrums.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::snd::spectrum_t<precision_d>* const pSndSpectrum =
            m_vpSpectrums.val(stIndex);
        emp::mat::spectrum_t<precision_d>* const pEvlSpectrum =
            m_pSound->data().spectrums().val(stIndex);
        pEvlSpectrum->set_data(pSndSpectrum);
    } }
#endif

    { c_size stCount = m_vpDelays.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::snd::delay_t<precision_d>* const pSndDelay =
            m_vpDelays.val(stIndex);
        emp::mat::delay_t<precision_d>* const pEvlDelay =
            m_pSound->data()->delays().val(stIndex);
        pEvlDelay->set_data(pSndDelay);
    } }

    { c_size stCount = m_vpReverbs.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::snd::reverb_t<precision_d>* const pSndReverb =
            m_vpReverbs.val(stIndex);
        emp::mat::reverb_t<precision_d>* const pEvlReverb =
            m_pSound->data()->reverbs().val(stIndex);
        pEvlReverb->set_data(pSndReverb);
    } }

    c_size stCount = m_pSound->data()->envelopes().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::envelope_t<precision_d>* const pEvlEnvelope =
            m_pSound->data()->envelopes().val(stIndex);
        emp::snd::envelope_c const* const pSndEnvelope =
            pEvlEnvelope->envelope();

        float const fSustain = pSndEnvelope->sustain();
        float const fDuration = pSndEnvelope->duration();

        env_c& rEnv = m_vEnvelopes.ref(stIndex);

        for (size_t stStride = 0;
            stStride < emp::mat::precision_c::array_stride;
            ++stStride)
        {
            float fTime = rEnv.time;
            vector_d const vTime = tool_d::set(fTime);
            vector_d const vSustain = tool_d::set(fSustain);
            vector_d const vDuration = tool_d::set(fDuration);
            vector_d const vNewTime = tool_d::add(vTime, tool_d::mul(tool_d::set(a_fDelta),tool_d::set(0,1,2,3)));
            vector_d const vMax = (eState == state_e::Playing ? vSustain : (eState == state_e::Stopping ? vDuration : vTime));
            vector_d const vClampedTime = tool_d::bit_select(tool_d::more_mask(vNewTime, vMax), vMax, vNewTime);

            c_float fX = tool_d::get_x(vClampedTime);
            c_float fY = tool_d::get_y(vClampedTime);
            c_float fZ = tool_d::get_z(vClampedTime);
            c_float fW = tool_d::get_w(vClampedTime);

            c_float fVX = pSndEnvelope->solve(fX, true);
            c_float fVY = pSndEnvelope->solve(fY, true);
            c_float fVZ = pSndEnvelope->solve(fZ, true);
            c_float fVW = pSndEnvelope->solve(fW, true);

            if (fW >= fDuration)
            { m_eState = state_e::Stopped; }
            rEnv.time = fW + a_fDelta;

            if (fVX < 0 || fVY < 0 || fVZ < 0 || fVW < 0)
            {
                //EMP_LOG_WARNING("negative volume");
            }

            vector_d const vVolume = tool_d::set(
                emp::tt::maxof(0.0f, fVX),
                emp::tt::maxof(0.0f, fVY),
                emp::tt::maxof(0.0f, fVZ),
                emp::tt::maxof(0.0f, fVW));

            pEvlEnvelope->set_volume(stStride, vVolume);
        }
    }

    c_size stSampleCount = m_pSound->sample_count();
    if (stSampleCount > 0)
    {
        m_pSound->prepare_samples(m_stSound);
        m_stSound += emp::mat::precision_c::data_stride;

        if (emp::tt::is_not_max(stSampleCount))
        {
            size_t stSoundLoop = 1;
            while (m_stSound >= stSampleCount * stSoundLoop)
            { ++stSoundLoop;}
            m_stSound -= (stSoundLoop - 1) * stSampleCount;
        }

        return true;

    }
    else
    {
        if (state() == state_e::Stopping)
        {
            m_stSound = 0;
            m_eState = state_e::Stopped;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN float media_c::sample(c_size a_stChannel)
{
    float const fSound = m_pSound->solve(a_stChannel, m_stSound);
    float const fClampedSound =
        emp::tt::minof(emp::tt::maxof(fSound, -1.0f), 1.0f);
    return emp::tt::minof(emp::tt::maxof(fClampedSound * m_fVolume, -1.0f),1.0f);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
size_t media_c::channel_count(void) const
{ return m_pSound->channel_count(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template class spectrum_t<precision_d>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

