//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_envelope.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_nurbs.h"

#include "emp_rfx_factory.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
envelope_c::envelope_c(void):
    m_sName(),
    m_pNurbs(),
    m_vtSamples(),
    m_fBegin(),
    m_fEnd(),
    m_fDuration(),
    m_fSustain()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
envelope_c::envelope_c(emp::mat::nurbs_c* const a_pNurbs):
    m_sName(),
    m_pNurbs(a_pNurbs),
    m_vtSamples(),
    m_fBegin(),
    m_fEnd(),
    m_fDuration(),
    m_fSustain()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
envelope_c::~envelope_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_c::create(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_c::destroy(void)
{
    m_sName = astring();
    m_pNurbs = nullptr;
    m_vtSamples.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::v4f::tret envelope_c::sample(c_float a_fParameter) const
{
    v4f v4Sample = emp::mat::v4f::all(0);
    c_size stCount = m_pNurbs->points().count();
    if (stCount >= 2)
    { v4Sample = m_pNurbs->sample(a_fParameter); }
    return v4Sample;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float envelope_c::solve(c_float a_fTime, c_bool a_bFromCache) const
{
    if (a_bFromCache)
    {
        c_float fTime = emp::tt::minof(emp::tt::maxof(0.0f, a_fTime - m_fBegin), m_fDuration);

        c_float fRate = 44100;
        c_size stCount = m_vtSamples.count();

        //>> @@0 bug stCount - 1 ?
        c_size stIndex = emp::tt::minof(emp::tt::maxof<size_t>(0, emp::ast::cast<size_t>(fTime * fRate)), stCount);
        //<<

        if (stIndex < stCount)
        { return m_vtSamples.val(stIndex); }
        return 0.0f;
    }
    else
    {
        c_float fTime = emp::tt::minof(emp::tt::maxof(m_fBegin, a_fTime), m_fEnd);

        float fParameter = 0.0f;
        c_size stCount = m_pNurbs->points().count();
        if (stCount >= 2)
        { fParameter = m_pNurbs->solve(fTime); } // @@0 : mixer1
        fParameter = emp::tt::minof(emp::tt::maxof(0.0f, fParameter), 1.0f);
        c_v4f v4Sample = sample(fParameter);
        return v4Sample.y;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_c::build(void)
{
    m_fBegin = m_pNurbs->points().count() > 1 ? m_pNurbs->points().first_val().x : 0;
    m_fEnd = m_pNurbs->points().count() > 1 ? m_pNurbs->points().last_ref().x : 0;

    m_fDuration = m_fEnd - m_fBegin;

    m_fSustain = 0.5f * m_fDuration;

    c_float fRate = 44100;
    c_size stCount = emp::ast::cast<size_t>(m_fDuration * fRate) + 1;

    m_vtSamples.resize(stCount);
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_float fTime = emp::ast::cast<float>(stIndex) / fRate;
        c_float fReturn = solve(fTime, false);
        m_vtSamples.set(stIndex, fReturn);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

