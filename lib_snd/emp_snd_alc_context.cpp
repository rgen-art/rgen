//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_alc_context.h"

#include "emp_snd_al_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(EMP_XX_OS_IOS) || defined(EMP_XX_OS_OSX)
#include <OpenAL/alc.h>
#else
#include <AL/alc.h>
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN astring alcerror_to_string(c_int a_siError);
EMP_RETURN astring alcerror_to_string(c_int a_siError)
{
    switch (a_siError)
    {
#define XTMP_CASE(x_Error)\
        case x_Error: { return ALITERAL(EMP_PP_STRING(x_Error)); }
        XTMP_CASE(ALC_NO_ERROR);
        XTMP_CASE(ALC_INVALID_DEVICE);
        XTMP_CASE(ALC_INVALID_CONTEXT);
        XTMP_CASE(ALC_INVALID_ENUM);
        XTMP_CASE(ALC_INVALID_VALUE);
        XTMP_CASE(ALC_OUT_OF_MEMORY);
        default:
        { EMP_ASSERT_UNREACHABLE();  return astring(); }
#undef XTMP_CASE
    };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool alc_success(ALCdevice* const a_pAlcDevice)
{
    c_int iAlError = alcGetError(a_pAlcDevice);
    if (iAlError != ALC_NO_ERROR)
    {
        EMP_LOG_ERROR("AlcError : #0", alcerror_to_string(iAlError));
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool alc_failed(ALCdevice* const a_pAlcDevice)
{ return alc_success(a_pAlcDevice) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
alc_context_c::alc_context_c(void) :
m_pAlcDevice(nullptr),
m_pAlcContext(nullptr)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
alc_context_c::~alc_context_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool alc_context_c::create(void)
{
    destroy();

    m_pAlcDevice = alcOpenDevice(nullptr);
    if (m_pAlcDevice == nullptr ||
        alc_failed(m_pAlcDevice))
    {
        EMP_LOG_ERROR("Libsnd failed to alcOpenDevice");
        return false;
    }

    // if a mixer output rate was specified, set it here
    // must be done before the alcCreateContext() call
    //if (mOutputRate)
    // { alcMacOSXMixerOutputRateProc(mOutputRate); }

    c_int siParams []=
    {
        ALC_FREQUENCY,
        44100,
        ALC_STEREO_SOURCES,
        8,
        ALC_MONO_SOURCES,
        8,
        ALC_SYNC,
        AL_TRUE,
        0
    };

    m_pAlcContext = alcCreateContext(m_pAlcDevice, siParams);
    if (alc_failed(m_pAlcDevice) ||
        m_pAlcContext == nullptr)
    {
        EMP_LOG_ERROR("Libsnd failed to alcCreateContext");
        return false;
    }

    alcMakeContextCurrent(m_pAlcContext);
    if (alc_failed(m_pAlcDevice))
    {
        EMP_LOG_ERROR("Libsnd failed to alcMakeContextCurrent");
        return false;
    }

    alDistanceModel(AL_NONE);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void alc_context_c::process(void)
{
    alcProcessContext(m_pAlcContext);
    EMP_ASSERT(alc_success(m_pAlcDevice));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void alc_context_c::destroy(void)
{
    if (m_pAlcContext)
    {
        alcMakeContextCurrent(nullptr);
        EMP_ASSERT(alc_success(m_pAlcDevice));

        alcDestroyContext(m_pAlcContext);
        EMP_ASSERT(alc_success(m_pAlcDevice));

        m_pAlcContext = nullptr;
    }

    if (m_pAlcDevice)
    {
        EMP_ASSERT_BOOL_CALL(alcCloseDevice(m_pAlcDevice) != 0);
        m_pAlcDevice = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

