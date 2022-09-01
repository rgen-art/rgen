//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_SCALE_H
#define EMP_SND_SCALE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_scale_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,scale_c, public, scale_i)
private:
    astring m_sName;
    float m_fOrigin;
    float m_fTones;
    float m_fRatio;
    float m_fOffset;

public:
    scale_c(void):
        m_sName(),
        m_fOrigin(),
        m_fTones(),
        m_fRatio(),
        m_fOffset()
    { destroy(); }

    ~scale_c(void)
    {}

public:
    void destroy(void)
    {
        m_sName = astring();
        m_fOrigin = 0;
        m_fTones = 0;
        m_fRatio = 0;
        m_fOffset = 0;
    }

public:
    //>> scale_i
    EMP_RETURN c_astring& name(void) const { return m_sName; }
    EMP_RETURN bool frequency(c_ui32 a_ui32Index, float& a_rfOut) const;
    //<< scale_i

public:
    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }

    EMP_RETURN float origin(void) const { return m_fOrigin; }
    void set_origin(float const a_fValue) { m_fOrigin = a_fValue; }

    EMP_RETURN float tones(void) const { return m_fTones; }
    void set_tones(float const a_fValue) { m_fTones = a_fValue; }

    EMP_RETURN float ratio(void) const { return m_fRatio; }
    void set_ratio(float const a_fValue) { m_fRatio = a_fValue; }

    EMP_RETURN float offset(void) const { return m_fOffset; }
    void set_offset(float const a_fValue) { m_fOffset = a_fValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

