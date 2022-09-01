//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_QUALITY_H
#define EMP_SND_QUALITY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(class, quality_c)
private:
    astring m_sName;
    ui32 m_ui32Rate;
    ui32 m_ui32Depth;
    ui32 m_ui32Channels;
    float m_fVolume;

public:
    quality_c(void):
        m_sName(), m_ui32Rate(0), m_ui32Depth(0), m_ui32Channels(0), m_fVolume(0)
    {}

    quality_c(c_ui32 a_ui32Rate, c_ui32 a_ui32Depth, c_ui32 a_ui32Channel, c_float a_fVolume):
        m_sName(), m_ui32Rate(a_ui32Rate), m_ui32Depth(a_ui32Depth), m_ui32Channels(a_ui32Channel), m_fVolume(a_fVolume)
    {}

    quality_c(quality_c const& a_rFrom):
        m_sName(a_rFrom.m_sName), m_ui32Rate(a_rFrom.m_ui32Rate), m_ui32Depth(a_rFrom.m_ui32Depth),
        m_ui32Channels(a_rFrom.m_ui32Channels), m_fVolume(a_rFrom.m_fVolume)
    {}

    quality_c& operator=(quality_c const& a_rFrom)
    {
        m_sName = a_rFrom.m_sName;
        m_ui32Rate = a_rFrom.m_ui32Rate;
        m_ui32Depth = a_rFrom.m_ui32Depth;
        m_ui32Channels = a_rFrom.m_ui32Channels;
        m_fVolume = a_rFrom.m_fVolume;
        return *this;
    }

    EMP_RETURN c_astring& name(void) const { return m_sName; }
    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }

    EMP_RETURN ui32 rate(void) const { return m_ui32Rate; }
    void set_rate(c_ui32 a_ui32Value) { m_ui32Rate = a_ui32Value; }

    EMP_RETURN ui32 depth(void) const { return m_ui32Depth; }
    void set_depth(c_ui32 a_ui32Value) { m_ui32Depth = a_ui32Value; }

    EMP_RETURN ui32 channels(void) const { return m_ui32Channels; }
    void set_channels(c_ui32 a_ui32Value) { m_ui32Channels = a_ui32Value; }

    EMP_RETURN float volume(void) const { return m_fVolume; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

