//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_al_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_limit.h"
#include "emp_mem_bulk.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN astring alerror_to_string(c_int a_siError);
EMP_RETURN astring alerror_to_string(c_int a_siError)
{
    switch (a_siError)
    {
#define XTMP_CASE(x_Error) case x_Error: { return ALITERAL(EMP_PP_STRING(x_Error)); }
        XTMP_CASE(AL_NO_ERROR);
        XTMP_CASE(AL_INVALID_NAME);
        XTMP_CASE(AL_INVALID_ENUM);
        XTMP_CASE(AL_INVALID_VALUE);
        XTMP_CASE(AL_INVALID_OPERATION);
        XTMP_CASE(AL_OUT_OF_MEMORY);
        default:
        { EMP_ASSERT_UNREACHABLE(); return astring();}
#undef XTMP_CASE
    };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_success(void)
{
    c_int iAlError = alGetError();
    if (iAlError != AL_NO_ERROR)
    {
        EMP_LOG_ERROR("AlError : #0", alerror_to_string(iAlError));
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_failure(void)
{ return al_success() == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
al_buffer_c::al_buffer_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
al_buffer_c::~al_buffer_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_buffer_c::create(   void const* const a_pui8Data,
                            ALsizei const a_stSize,
                            c_size a_stChannelCount,
                            ALsizei const a_stSampleRate,
                            c_size a_stFrameCount,
                            c_size a_stBitPerChannel)
{
    destroy();

    m_siSize = a_stSize;
    m_stChannelCount = a_stChannelCount;
    m_siSampleRate = a_stSampleRate;
    m_stFrameCount = a_stFrameCount;
    m_stBitPerChannel = a_stBitPerChannel;

    c_size stBitPerChannel = m_stBitPerChannel;
    c_size stChannelCount = m_stChannelCount;

    if (stBitPerChannel == 16 &&
        stChannelCount == 2)
    { m_eFormat = AL_FORMAT_STEREO16; }
    else if (stBitPerChannel == 16 &&
             stChannelCount == 1)
    { m_eFormat = AL_FORMAT_MONO16; }
    else if (stBitPerChannel == 8 &&
             stChannelCount == 2)
    { m_eFormat = AL_FORMAT_STEREO8; }
    else if (stBitPerChannel == 8 &&
             stChannelCount == 1)
    { m_eFormat = AL_FORMAT_MONO8; }
    else
    { EMP_ASSERT_UNREACHABLE(); }

    EMP_ASSERT_ALCALL(alGenBuffers(1, &m_ui32ID));
    update(a_pui8Data);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_buffer_c::destroy(void)
{
    if (is_valid())
    { EMP_ASSERT_ALCALL(alDeleteBuffers(1, &m_ui32ID)); }

    m_ui32ID = emp::tt::max<ui32>::value;

    m_eFormat = 0;
    m_siSize = 0;
    m_stChannelCount = 0;
    m_siSampleRate = 0;
    m_stFrameCount = 0;
    m_stBitPerChannel = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_buffer_c::update(void const* const a_pui8Data)
{ EMP_ASSERT_ALCALL(alBufferData(m_ui32ID, m_eFormat, a_pui8Data, m_siSize, m_siSampleRate)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_buffer_c::is_valid(void) const
{ return m_ui32ID != emp::tt::max<ui32>::value; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
al_source_c::al_source_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
al_source_c::~al_source_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void set_gain_imp(c_uint a_uiID, c_float a_fValue)
{ EMP_ASSERT_ALCALL(alSourcef(a_uiID, AL_GAIN, a_fValue)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void set_pitch_imp(c_uint a_uiID, c_float a_fValue)
{ EMP_ASSERT_ALCALL(alSourcef(a_uiID, AL_PITCH, a_fValue)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::create(void)
{
    destroy();

    m_fGain = 1.0f;
    m_fPitch = 1;

    EMP_ASSERT_ALCALL(alGenSources(1, &m_uiID));

    //c_int siBufferID = emp::saf::cast<int>(a_rAlBuffer.id());
    //EMP_ASSERT_ALCALL(alSourcei(id(), AL_BUFFER, siBufferID));

    EMP_ASSERT_ALCALL(alSourcef(id(), AL_PITCH, 1.0f));
    EMP_ASSERT_ALCALL(alSourcef(id(), AL_MIN_GAIN, 0.0f));
    EMP_ASSERT_ALCALL(alSourcef(id(), AL_MAX_GAIN, 1.0f));

    float const afZero[] { 0, 0, 0 };
    EMP_ASSERT_ALCALL(alSourcefv(id(), AL_POSITION, afZero));
    EMP_ASSERT_ALCALL(alSourcefv(id(), AL_VELOCITY, afZero));
    EMP_ASSERT_ALCALL(alSourcei(id(), AL_LOOPING,  AL_FALSE));

    set_gain_imp(id(), gain());
    set_pitch_imp(id(), pitch());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::destroy(void)
{
    m_fGain = 0;

    if (is_valid())
    {
        stop();
        EMP_ASSERT_ALCALL(alDeleteSources(1, &m_uiID));
        m_uiID = emp::tt::max<ui32>::value;
    }
    m_vpBuffers.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::set_gain(float const a_fValue)
{
    if (emp::tt::equal_(a_fValue, gain()) == false)
    {
        m_fGain = a_fValue;
        set_gain_imp(id(), gain());
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::set_pitch(float const a_fValue)
{
    if (emp::tt::equal_(a_fValue, pitch()) == false)
    {
        m_fPitch = a_fValue;
        set_pitch_imp(id(), pitch());
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::play(void)
{
    EMP_ASSERT(is_playing() == false);
    { EMP_ASSERT_ALCALL(alSourcePlay(id())); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::stop(void)
{
//    EMP_LOG_INFO("al_source::stop : #0", id());
    if (is_playing())
    {
        EMP_ASSERT_ALCALL(alSourceStop(id()));
        int siQueued = queued();
        while (siQueued--)
        { EMP_ASSERT_BOOL_CALL(unqueue() != nullptr); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_source_c::is_playing(void) const
{
    if (is_valid())
    {
        int siState = 0;
        EMP_ASSERT_ALCALL(alGetSourcei(id(), AL_SOURCE_STATE, &siState));
        return siState == AL_PLAYING;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_source_c::queue(al_buffer_c* const a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    EMP_ASSERT(is_valid());

    EMP_ASSERT(m_vpBuffers.contains(a_pValue) == false);
    m_vpBuffers.add(a_pValue);

    uint uiBuffer = a_pValue->id();
    EMP_ASSERT_ALCALL(alSourceQueueBuffers(id(), 1, &uiBuffer));
    EMP_ASSERT(queued() > 0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN al_buffer_c* al_source_c::unqueue(void)
{
    EMP_ASSERT(is_valid());

    uint uiBuffer = 0;
    EMP_ASSERT_ALCALL(alSourceUnqueueBuffers(id(), 1, &uiBuffer));

    al_buffer_c* pFound = nullptr;
    c_size stCount = m_vpBuffers.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex) // @@0 map ?
    {
        c_size stOffset = stCount - stIndex - 1;
        al_buffer_c* pCurrent = m_vpBuffers.val(stOffset);
        if (pCurrent->id() == uiBuffer)
        {
            pFound = pCurrent;
            m_vpBuffers.remove_unordered(stOffset);
            break;
        }
    }

    EMP_ASSERT(pFound != nullptr);
    return pFound;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN al_buffer_c* al_source_c::unqueue_processed(void)
{
    EMP_ASSERT(is_valid());

    if (processed() > 0)
    {
        return unqueue();
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int al_source_c::processed(void) const
{
    int siProcessed = 0;
    EMP_ASSERT_ALCALL(alGetSourcei(id(), AL_BUFFERS_PROCESSED, &siProcessed));
    return siProcessed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int al_source_c::queued(void) const
{
    int siQueued = 0;
    EMP_ASSERT_ALCALL(alGetSourcei(id(), AL_BUFFERS_QUEUED, &siQueued));
    return siQueued;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t al_source_c::position(void) const
{
    EMP_ASSERT(is_valid());
    int siPosition = 0;
    EMP_ASSERT_ALCALL(alGetSourcei(id(), AL_SAMPLE_OFFSET, &siPosition));
    return emp::ast::cast<size_t>(siPosition);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_source_c::is_valid(void) const
{ return id() != emp::tt::max<ui32>::value; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
al_listener_c::al_listener_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
al_listener_c::~al_listener_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_listener_c::create( v3f::tparam a_v3Position,
                                        v3f::tparam a_v3At,
                                        v3f::tparam a_v3Up)
{
    EMP_ALCALL_ORDIE(alListenerf(AL_GAIN, 1.0f));
    EMP_ALCALL_ORDIE(alListenerfv(AL_POSITION, &a_v3Position.x));

    // @@0 wtf
    c_v3f v3Zero = v3f::zero();
    EMP_ALCALL_ORDIE(alListenerfv(AL_VELOCITY, &v3Zero.x));

    // @@0 wtf
    emp::cnt::array<float, 6, true> afOrientation({ a_v3At.x, a_v3At.y, a_v3At.z, a_v3Up.x, a_v3Up.y, a_v3Up.z });
    EMP_ALCALL_ORDIE(alListenerfv(AL_ORIENTATION, afOrientation.first_ptr()));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void al_listener_c::destroy(void)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

