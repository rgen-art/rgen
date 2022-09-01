//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_ENVELOPE_H
#define EMP_SND_ENVELOPE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { class nurbs_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, envelope_c)
private:
    astring m_sName;
    emp::mat::nurbs_c* m_pNurbs;
    vector<float> m_vtSamples;
    float m_fBegin;
    float m_fEnd;
    float m_fDuration;
    float m_fSustain;

public:
    envelope_c(void);
    explicit envelope_c(emp::mat::nurbs_c* const a_pNurbs);
    ~envelope_c(void);

    void create(void);
    void destroy(void);

    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }
    EMP_RETURN c_astring& name(void) const { return m_sName; }

    void set_nurbs(emp::mat::nurbs_c* const a_pValue) { m_pNurbs = a_pValue; }

    EMP_RETURN emp::mat::nurbs_c const& nurbs(void) const { return *m_pNurbs; }
    EMP_RETURN emp::mat::nurbs_c& nurbs(void) { return *m_pNurbs; }

    EMP_RETURN float begin(void) const { return m_fBegin; }
    EMP_RETURN float end(void) const { return m_fEnd; }
    EMP_RETURN float duration(void) const { return m_fDuration; }
    EMP_RETURN float sustain(void) const { return m_fSustain; }

    EMP_RETURN float solve(float const a_fTime, c_bool a_bFromCache) const;

    EMP_RETURN bool ended(void) const;

public:
    void build(void);

private:
    EMP_RETURN emp::mat::v4f::tret sample(c_float a_fParameter) const;

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

