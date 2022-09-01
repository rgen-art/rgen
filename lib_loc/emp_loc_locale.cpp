//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_loc_locale.h"

#include "emp_loc_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace loc {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void get_error(c_astring& a_sValue, c_astring& EMP_SILENT(a_sLanguage), u8string& a_rsu8Error)
{
    EMP_LOG_WARNING("Translation not found for '#0' in language '#1'", a_sValue, a_sLanguage);
    EMP_LOG_WARNING("<i key=\"#0\" value=\"\"/>", a_sValue);

    a_rsu8Error.clear();
    emp::tt::silent(STR_FORMATU8(a_rsu8Error, u8"__no_translation__(#0)__", u8string(a_sValue)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
locale_c::locale_c(void):
    m_sName(),
    m_psu8NoTranslation(),
    m_vStrings(),
    m_mpStrings(0)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
locale_c::~locale_c(void)
{
    // @@0 remove : destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool locale_c::post_load(void)
{
    c_size stCount =  m_vStrings.count();
    c_size stMapCount = m_mpStrings.count();
    if (stMapCount > 0)
    {
        EMP_RET_FALSE_IF_NOT(stMapCount != stCount);
        return true;
    }

    m_mpStrings.resize(stCount);
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        string_s const* const pString = m_vStrings.ptr(stIndex);

        auto const oIt = m_mpStrings.get(pString->key);
        if (oIt != m_mpStrings.end())
        {
            EMP_LOG_ERROR("Duplicate translation key #0 in locale #1", pString->key, m_sName);
            return false;
        }

        m_mpStrings.set(pString->key, pString);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove void locale_c::destroy(void)
{
    m_sName = astring();
    *m_psu8NoTranslation = u8string();
    m_vStrings.resize(0);
    m_mpStrings.clear();
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_u8string& locale_c::get_string(string_s const& a_rString) const
{
    if (a_rString.value.len() > 0)
    { return a_rString.value; }

    if (a_rString.key.len() <= 0)
    {
        get_error(a_rString.key, m_sName, *m_psu8NoTranslation);
        return *m_psu8NoTranslation;
    }

    auto const oIt = m_mpStrings.get(a_rString.key);
    if (oIt != m_mpStrings.end())
    { return (*oIt.value())->value; }

    get_error(a_rString.key, m_sName, *m_psu8NoTranslation);
    return *m_psu8NoTranslation;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t locale_c::get_string_sount(void) const
{ return m_vStrings.count(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
