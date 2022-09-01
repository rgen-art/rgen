//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_xml_writer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <libxml/xmlwriter.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#define EMP_XML_LIBFLUSH

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace xml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
writer_c::writer_c(void):
m_pWriter(nullptr)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
writer_c::~writer_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::indent(void) const
{ return m_bIndent; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::indent(c_bool a_bValue)
{
    EMP_RET_FALSE_IF(xmlTextWriterSetIndent(m_pWriter, a_bValue) != 0);
    m_bIndent = a_bValue;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::create(c_astring& a_sFilename)
{
    destroy();


    EMP_RET_FALSE_IF(a_sFilename.is_empty(), "empty filename");

    astring sFilename(a_sFilename);
    m_pWriter = xmlNewTextWriterFilename(sFilename.cstr(), 0);
    EMP_RET_FALSE_IF(m_pWriter == nullptr, "failed to xmlNewTextWriterFilename : #0", a_sFilename);
    EMP_RET_FALSE_IF_NOT(indent(true));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void writer_c::destroy(void)
{
    if (m_pWriter != nullptr)
    {
        xmlFreeTextWriter(m_pWriter);
        m_pWriter = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::start_document(void)
{
    EMP_RET_FALSE_IF(xmlTextWriterStartDocument(m_pWriter, nullptr, "UTF-8", nullptr) < 0);
#if defined EMP_XML_LIBFLUSH
    EMP_RET_FALSE_IF(xmlTextWriterFlush(m_pWriter) < 0);
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::end_document(void)
{
#if defined EMP_XML_LIBFLUSH
    EMP_RET_FALSE_IF(xmlTextWriterFlush(m_pWriter) < 0);
#endif
    EMP_RET_FALSE_IF(xmlTextWriterEndDocument(m_pWriter) < 0);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::start_element(c_astring& a_sName)
{
    u8string sName(a_sName);
    EMP_ASSERT(sName.is_empty() == false);
    EMP_RET_FALSE_IF(xmlTextWriterStartElement(m_pWriter, sName.cstr()) < 0);
#if defined EMP_XML_LIBFLUSH
    EMP_RET_FALSE_IF(xmlTextWriterFlush(m_pWriter) < 0);
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::end_element(void)
{
    EMP_RET_FALSE_IF(xmlTextWriterEndElement(m_pWriter) < 0);
#if defined EMP_XML_LIBFLUSH
    EMP_RET_FALSE_IF(xmlTextWriterFlush(m_pWriter) < 0);
#endif

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::write_attribute(c_astring& a_sName, c_astring& a_sValue)
{ return write_attribute(a_sName, u8string(a_sValue)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::write_attribute(c_astring& a_sName, c_u8string& a_sValue)
{
    u8string sName(a_sName); // @@0 c_str
    u8string sValue(a_sValue); // @@0 c_str

    EMP_ASSERT(sName.is_empty() == false);

    EMP_RET_FALSE_IF(xmlTextWriterWriteAttribute(m_pWriter, sName.cstr(), sValue.cstr()) < 0);
#if defined EMP_XML_LIBFLUSH
    EMP_RET_FALSE_IF(xmlTextWriterFlush(m_pWriter) < 0);
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename V>
EMP_RETURN bool writer_c::write_attribute(c_astring& a_sName, V const& a_rValue)
{
    u8string sValue;
    return write_attribute(a_sName, u8string(emp::str::from(sValue, a_rValue)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool writer_c::write_element(c_u8string& a_sName, c_u8string& a_sValue)
{
    u8string sName(a_sName); // @@0 c_str
    u8string sValue(a_sValue); // @@) c_str

    EMP_ASSERT(sName.is_empty() == false);
    EMP_ASSERT(sValue.is_empty() == false);

    EMP_RET_FALSE_IF(xmlTextWriterWriteElement(m_pWriter, sName.c_str(), sValue.c_str()) < 0)

    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool writer_c::write_raw(c_u8string& a_sValue)
{
    u8string sValue(a_sValue); // @@0 c_str
    EMP_ASSERT(sValue.is_empty() == false);
    EMP_RET_FALSE_IF(xmlTextWriterWriteRaw(m_pWriter, sValue.cstr()) < 0);
#if defined EMP_XML_LIBFLUSH
    EMP_RET_FALSE_IF(xmlTextWriterFlush(m_pWriter) < 0);
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename V>
EMP_RETURN bool writer_c::write_raw(V const& a_rValue)
{ return write_raw(u8string(u8from(a_rValue))); } // @@0 u8 from*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template bool writer_c::write_attribute(c_astring&  , c_bool&);

template bool writer_c::write_attribute(c_astring&  , c_float&);

template bool writer_c::write_attribute(c_astring&  , c_char&);
template bool writer_c::write_attribute(c_astring&  , short const&);
template bool writer_c::write_attribute(c_astring&  , c_int&);
template bool writer_c::write_attribute(c_astring&  , long const&);
template bool writer_c::write_attribute(c_astring&  , long long const&);

template bool writer_c::write_attribute(c_astring&  , c_uchar&);
template bool writer_c::write_attribute(c_astring&  , unsigned short const&);
template bool writer_c::write_attribute(c_astring&  , c_uint&);
template bool writer_c::write_attribute(c_astring&  , unsigned long const&);
template bool writer_c::write_attribute(c_astring&  , unsigned long long const&);
template bool writer_c::write_attribute(c_astring&  , emp::tim::time_c const&);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

