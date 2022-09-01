//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_xml_reader.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <libxml/xmlreader.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#define EMP_XML_LIBMEM

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace xml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

#if defined EMP_XML_LIBMEM
#else
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void free_func(void* a_pPointer);
void free_func(void* a_pPointer)
{
    EMP_DEALLOCATE_ARRAY(reinterpret_cast<char*>(a_pPointer));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN void* malloc_func(c_size a_stSize);
EMP_RETURN void* malloc_func(c_size a_stSize)
{
    return EMP_ALLOCATE_ARRAY(char, a_stSize);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN void* malloc_atomic_func(c_size a_stSize);
EMP_RETURN void* malloc_atomic_func(c_size a_stSize)
{
    return EMP_ALLOCATE_ARRAY(char, a_stSize);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN void* realloc_func(void* a_pPointer, c_size a_stSize);
EMP_RETURN void* realloc_func(void* a_pPointer, c_size a_stSize)
{
    c_size stSize = EMP_ALLOCATOR.requested_size(a_pPointer);
    emp::mem::bulk_t<char> oBulk(reinterpret_cast<char*>(a_pPointer), stSize, true);
    emp::mem::reallocable_grow(oBulk, a_stSize, a_stSize);
    return oBulk.first_ptr();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN char* strdup_func(cpc_char a_szSource);
EMP_RETURN char* strdup_func(cpc_char a_szSource)
{
    cpc_ui8 pSource = reinterpret_cast<ui8 const*>(a_szSource);
    size_t stLen = 0;
    bool bAscii = false;
    size_t stSize = 0;
    EMP_RET_NULL_IF_NOT(u8string::len(pSource, stLen, bAscii, stSize));
    ui8* const pDest = EMP_ALLOCATE_ARRAY(ui8, stSize + 1);

    emp::mem::bulk_t<c_ui8> oSource(pSource, stSize, true);
    emp::mem::bulk_t<ui8> oDest(pDest, stSize + 1, true);
    emp::mem::bulk_copy_from_start(oSource, oDest, stSize);
    oDest.set(stSize, 0);
    return reinterpret_cast<char*>(oDest.first_ptr());
}
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
parser_c* parser_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool parser_c::create(void)
{
#if defined EMP_XML_LIBMEM
#else
    xmlGcMemGet(&pfctFreeFunc, &pfctMallocFunc, &pfctMallocAtomicFunc, &pfctReallocFunc, &pfctStrdupFunc);
    xmlGcMemSetup(&free_func, &malloc_func, &malloc_atomic_func, &realloc_func, &strdup_func);
#endif

    //xmlInitParser();
    LIBXML_TEST_VERSION

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void parser_c::destroy(void)
{
    xmlCleanupParser();

#if defined EMP_XML_LIBMEM
#else
    if (pfctFreeFunc != nullptr)
    {
        xmlGcMemSetup(pfctFreeFunc, pfctMallocFunc, pfctMallocAtomicFunc, pfctReallocFunc, pfctStrdupFunc);
    }
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
reader_c::reader_c(void):
m_pReader(nullptr),
m_bUnusedAttribute()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
reader_c::~reader_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::create(c_astring& a_sFilename) // @@0 utf8
{
    destroy();

    astring sFilename(a_sFilename);
    EMP_RET_FALSE_IF(sFilename.is_empty());

    m_pReader = xmlNewTextReaderFilename(sFilename.cstr());
    EMP_RET_FALSE_IF(m_pReader == nullptr, "failed to xmlNewTextReaderFilename : '#0'", a_sFilename);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void reader_c::destroy(void)
{
    if (m_pReader != nullptr)
    {
        emp::tt::silent(next_element());
        xmlMemoryDump();
        xmlFreeTextReader(m_pReader);
        m_pReader = nullptr;
    }
    m_bUnusedAttribute = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::parse(tfct_parse_element const& a_rfctParseElement, tfct_parse_attribute const& a_rfctParseAttribute)
{
    while (true)
    {
        c_read_result_e eReadElementResult = next_element();
        if (eReadElementResult == read_result_e::Continue)
        {
            if (read_type() != node_e::Element)
            { continue; }

            c_parse_element_result_e eParseElementResult = a_rfctParseElement(*this);
            if (eParseElementResult == parse_element_result_e::Continue)
            {
                while (true)
                {
                    c_read_result_e eReadAttributeResult = next_attribute();
                    if (eReadAttributeResult == read_result_e::Continue)
                    {
                        c_parse_attribute_result_e eParseAttributeResult = a_rfctParseAttribute(*this);
                        if (eParseAttributeResult == parse_attribute_result_e::Continue)
                        { }
                        else if (eParseAttributeResult == parse_attribute_result_e::Break)
                        { break; }
                        else
                        {
                            // @@0 utf8 EMP_LOG_ERROR("error while parsing attribute : '#0'", read_name());
                            return false;
                        }
                    }
                    else if (eReadAttributeResult == read_result_e::Break)
                    { break; }
                    else
                    {
                        // @@0 utf8 EMP_LOG_ERROR("error while reading attribute : #0", read_name());
                        return false;
                    }
                }
            }
            else if (eParseElementResult == parse_element_result_e::Skip)
            {}
            else if (eParseElementResult == parse_element_result_e::Break)
            { break; }
            else
            {
                // @@0 utf8 EMP_LOG_ERROR("error while parsing element : #0", read_name());
                return false;
            }
        }
        else if (eReadElementResult == read_result_e::Break)
        { break; }
        else
        {
            // @@0 utf8 EMP_LOG_ERROR("error while parsing element : #0", read_name());
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN reader_c::read_result_e reader_c::next_element(void) const
{
    EMP_ASSERT(m_pReader != nullptr);
    c_int siReturn = xmlTextReaderRead(m_pReader);
    EMP_ASSERT(siReturn == -1 || siReturn == 0 || siReturn == 1);
    if (siReturn == 0) { return read_result_e::Break; }
    if (siReturn == 1) { return read_result_e::Continue; }
    return read_result_e::Error;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN reader_c::read_result_e reader_c::next_valid_element(void) const
{
    EMP_ASSERT(m_pReader != nullptr);
    while (true)
    {
        read_result_e eResult = next_element();
        if (eResult == read_result_e::Continue)
        {
            if (read_type() == node_e::Element)
            { return read_result_e::Continue; }
        }
        else
        { return eResult; }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN reader_c::read_result_e reader_c::next_attribute(void)
{
    if (m_bUnusedAttribute)
    {
        m_bUnusedAttribute = false;
        return read_result_e::Continue;
    }

    EMP_ASSERT(m_pReader != nullptr);
    c_int siReturn = xmlTextReaderMoveToNextAttribute(m_pReader);
    EMP_ASSERT(siReturn == -1 || siReturn == 0 || siReturn == 1);
    if (siReturn == 0) { return read_result_e::Break; }
    if (siReturn == 1) { return read_result_e::Continue; }
    return read_result_e::Error;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::next_valid_attribute(void)
{
    EMP_ASSERT(m_pReader != nullptr);
    return next_attribute() == read_result_e::Continue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_e reader_c::read_type(void) const
{
    EMP_ASSERT(m_pReader != nullptr);
    c_int siType = xmlTextReaderNodeType(m_pReader);
    c_node_e eType = enum_from(node_e, siType);
    if (enum_is_valid(eType)) { return eType; }
    return node_e::Unknown;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_const_name(u8string& a_rsOut) const
{
    EMP_ASSERT(m_pReader != nullptr);
    xmlChar const* const xszName = xmlTextReaderConstName(m_pReader);
    EMP_RET_FALSE_IF_NOT(u8string::ref(xszName, a_rsOut));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_const_name(astring& a_rsOut) const
{
    u8string su8Name;
    EMP_RET_FALSE_IF_NOT(read_const_name(su8Name));
    EMP_RET_FALSE_IF_NOT(astring::from_u8string(a_rsOut, su8Name));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_const_value(u8string& a_rsOut) const
{
    EMP_ASSERT(m_pReader != nullptr);
    xmlChar const* const xszValue = xmlTextReaderConstValue(m_pReader);
    EMP_RET_FALSE_IF_NOT(u8string::ref(xszValue, a_rsOut));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_const_value(astring& a_rsOut) const
{
    u8string su8Value;
    EMP_RET_FALSE_IF_NOT(read_const_value(su8Value));
    EMP_RET_FALSE_IF_NOT(astring::from_u8string(a_rsOut, su8Value));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_owned_value(u8string& a_rsOut) const
{
    EMP_ASSERT(m_pReader != nullptr);
#if defined EMP_XML_LIBMEM
    xmlChar const* const xszValue = xmlTextReaderConstValue(m_pReader);
    EMP_RET_FALSE_IF_NOT(u8string::dup(reinterpret_cast<char const*>(xszValue), a_rsOut));
#else
    xmlChar* const xszValue = xmlTextReaderValue(m_pReader);
    EMP_RET_FALSE_IF_NOT(u8string::own(xszValue, a_rsOut));
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_owned_value(astring& a_rsOut) const
{
    u8string su8Value;
    EMP_RET_FALSE_IF_NOT(read_owned_value(su8Value));
    EMP_RET_FALSE_IF_NOT(astring::from_u8string(a_rsOut, su8Value));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_depth(size_t& a_rstDepth) const
{
    EMP_ASSERT(m_pReader != nullptr);
    int siDepth = xmlTextReaderDepth(m_pReader);
    if (siDepth < 0)
    { return false; }

    a_rstDepth = emp::ast::cast<uint>(siDepth);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::log_error(void) const
{
    //@@0 string
    //EMP_LOG_ERROR("Failed while parsing element : #0, #1, #2",
      //  emp::ast::cast<size_t>(read_type()), astring(read_name()), astring(read_value()));
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
EMP_RETURN bool reader_c::read_element(T const& a_sName)
{ return read_element(astring::make(a_sName)); }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_element(c_astring& a_sName)
{
    astring sName;
    EMP_RET_FALSE_IF_NOT(get_next_element(sName), "failed to read_element : #0", a_sName);
    EMP_RET_FALSE_IF_NOT(cmp(sName, a_sName), "failed to read element : #0 (found : #1)", a_sName, sName);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::get_next_element(astring& a_rOut)
{
    if (next_valid_element() != read_result_e::Continue)
    { return false; }
    EMP_RET_FALSE_IF_NOT(read_const_name(a_rOut));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename V>
EMP_RETURN bool reader_c::read_const_attribute(c_astring& a_sName, V& a_rValue, c_bool a_bLog)
{
    astring sValue;
    if (read_const_attribute(a_sName, sValue, a_bLog) == false)
    { return false; }
    return to(astring(sValue), a_rValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename V>
EMP_RETURN bool reader_c::read_const_attribute_value(V& a_rValue, c_bool EMP_SILENT(a_bLog))
{
    astring sValue;
    EMP_RET_FALSE_IF_NOT(read_const_value(sValue));
    return to(sValue, a_rValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_const_attribute(c_astring& a_sName, astring& a_rsValue, c_bool a_bLog)
{
    if (next_valid_attribute() == false)
    {
        if (a_bLog)
        { EMP_LOG_ERROR("failed to read next valid attribute"); }
        return false;
    }

    astring sName;
    EMP_RET_FALSE_IF_NOT(read_const_name(sName));

    if (cmp(sName, a_sName) == false)
    {
        if (a_bLog)
        { EMP_LOG_ERROR("Failed to read attribute : #0 (found : #1)", a_sName, sName); }
        return false;
    }

    EMP_RET_FALSE_IF_NOT(read_const_value(a_rsValue));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool reader_c::read_owned_attribute(c_astring& a_sName, astring& a_rsValue, c_bool a_bLog)
{
    if (next_valid_attribute() == false)
    {
        if (a_bLog)
        { EMP_LOG_ERROR("failed to read next valid attribute"); }
        return false;
    }

    astring sName;
    EMP_RET_FALSE_IF_NOT(read_const_name(sName));

    if (cmp(sName, a_sName) == false)
    {
        if (a_bLog)
        { EMP_LOG_ERROR("Failed to read attribute : #0 (found : #1)", a_sName, sName); }
        return false;
    }

    EMP_RET_FALSE_IF_NOT(read_owned_value(a_rsValue));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template bool reader_c::read_const_attribute(c_astring&, bool&               , c_bool);

template bool reader_c::read_const_attribute(c_astring&, emp::tim::time_c&   , c_bool);

template bool reader_c::read_const_attribute(c_astring&, float&              , c_bool);

template bool reader_c::read_const_attribute(c_astring&, char&               , c_bool);
template bool reader_c::read_const_attribute(c_astring&, short&              , c_bool);
template bool reader_c::read_const_attribute(c_astring&, int &               , c_bool);
template bool reader_c::read_const_attribute(c_astring&, long int &          , c_bool);
template bool reader_c::read_const_attribute(c_astring&, long long int &     , c_bool);

template bool reader_c::read_const_attribute(c_astring&, uchar&              , c_bool);
template bool reader_c::read_const_attribute(c_astring&, unsigned short&     , c_bool);
template bool reader_c::read_const_attribute(c_astring&, uint&               , c_bool);
template bool reader_c::read_const_attribute(c_astring&, unsigned long&      , c_bool);
template bool reader_c::read_const_attribute(c_astring&, unsigned long long& , c_bool);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template bool reader_c::read_const_attribute_value(bool&               , c_bool);
template bool reader_c::read_const_attribute_value(emp::tim::time_c&   , c_bool);
template bool reader_c::read_const_attribute_value(char&               , c_bool);
template bool reader_c::read_const_attribute_value(ui8&                , c_bool);
template bool reader_c::read_const_attribute_value(ui16&               , c_bool);
template bool reader_c::read_const_attribute_value(float&              , c_bool);
template bool reader_c::read_const_attribute_value(unsigned long&      , c_bool);
template bool reader_c::read_const_attribute_value(unsigned long long& , c_bool);
template bool reader_c::read_const_attribute_value(uint&               , c_bool);
template bool reader_c::read_const_attribute_value(long long&          , c_bool);
template bool reader_c::read_const_attribute_value(int&                , c_bool);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

