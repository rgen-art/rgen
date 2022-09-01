//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_cor_selection.h"

#include "ase_cor_reflection.rfx.h"

#include "emp_ose_compiler.h"
#include "emp_ose_renderer.h"
#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool pool_c::post_load(void)
{
    c_size stCount = m_vsOse.count();
    files.resize(emp::tt::maxof<size_t>(stCount / (stCount > 4u ? 4u : 1u), 4u));
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_astring& sOse = m_vsOse.ref(stIndex);
        if (files.get(sOse).not_end())
        { return false; }
        files.set(sOse, stIndex);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void pool_c::add_source(c_astring& a_sValue)
{
    if (files.get(a_sValue).not_end())
    { return; }

    c_size stCount = m_vsOse.count();
    files.set(a_sValue, stCount);
    m_vsOse.add(a_sValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void pool_c::clear(void)
{
    m_vsOse.resize(0);
    files.clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t pool_c::remove_source(map_t<astring, size_t>::iterator_c const& a_rIt)
{
    c_size stCount = m_vsOse.count();

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (emp::str::cmp(m_vsOse.ref(stIndex), *a_rIt.key()))
        {
            files.remove(a_rIt);
            m_vsOse.remove(stIndex);
            return stIndex;
        }
    }

    EMP_ASSERT(false);
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t pool_c::remove_source(c_astring& a_rsFile)
{
    auto const it = files.get(a_rsFile);
    EMP_ASSERT(it.not_end());
    if (it.not_end())
    { return remove_source(it); }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void pool_c::remove_source(c_size a_stIndex)
{
    c_astring& sOse = m_vsOse.ref(a_stIndex);
    files.remove(files.get(sOse));
    m_vsOse.remove(a_stIndex);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void pool_c::insert_source(c_size a_stIndex, c_astring& a_sSource)
{
    if (files.get(a_sSource).not_end())
    { return; }

    m_vsOse.insert(a_stIndex, a_sSource);
    files.set(a_sSource, a_stIndex);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
