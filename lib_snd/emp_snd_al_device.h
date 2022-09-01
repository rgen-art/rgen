//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_AL_H
#define EMP_SND_AL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(EMP_XX_OS_IOS) || defined(EMP_XX_OS_OSX)
#include <OpenAL/al.h>
#else
#include "AL/al.h"
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool al_success(void);
EMP_RETURN bool al_failure(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_ALCALL_ORDIE(x_Call)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
x_Call;\
if (al_failure())\
{\
    EMP_LOG_ERROR("failed to #0", ALITERAL(EMP_PP_STRING(x_Call)));\
    EMP_ASSERT(false);\
    return false;\
}\
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_ASSERT_ALCALL(x_Call)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
x_Call; EMP_ASSERT(al_success());\
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, al_buffer_c)
private:
    ui32 EMP_TT_MAX_VAR(m_ui32ID);
    ALenum m_eFormat = 0;
    ALsizei m_siSize = 0;
    size_t m_stChannelCount = 0;
    ALsizei m_siSampleRate = 0;
    size_t m_stFrameCount = 0;
    size_t m_stBitPerChannel = 0;

public:
    al_buffer_c(void);
    ~al_buffer_c(void);

    void create(void const* const a_pui8Data,
                ALsizei const a_stSize,
                c_size a_stChannelCount,
                ALsizei const a_stSampleRate,
                c_size a_stFrameCount,
                c_size a_stBitPerChannel);

    void update(void const* const a_pui8Data);

    void destroy(void);

    EMP_RETURN bool is_valid(void) const;
    EMP_RETURN ui32 id(void) const { return m_ui32ID; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, al_source_c)
private:
    uint EMP_TT_MAX_VAR(m_uiID);
    float m_fGain = 0.2f;
    float m_fPitch = 1.0f;
    emp::cnt::vector<al_buffer_c*> m_vpBuffers{};

public:
    al_source_c(void);
    ~al_source_c(void);

    void create(void);
    void destroy(void);

    void play(void);
    void stop(void);

    void queue(al_buffer_c* const a_pValue);
    EMP_RETURN al_buffer_c* unqueue(void);
    EMP_RETURN al_buffer_c* unqueue_processed(void);

    EMP_RETURN bool is_playing(void) const;

    EMP_RETURN size_t position(void) const;

    EMP_RETURN uint id(void) const { return m_uiID; }

    EMP_RETURN float gain(void) const { return m_fGain; }
    void set_gain(float const a_fValue);

    EMP_RETURN float pitch(void) const { return m_fPitch; }
    void set_pitch(float const a_fValue);

private:
    EMP_RETURN bool is_valid(void) const;

    EMP_RETURN int queued(void) const;
    EMP_RETURN int processed(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, al_listener_c)
public:
    al_listener_c(void);
    ~al_listener_c(void);

public:
    EMP_RETURN bool create(v3f::tparam a_v3Position, v3f::tparam a_v3At, v3f::tparam a_v3Up);
    void destroy(void);

private:
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

