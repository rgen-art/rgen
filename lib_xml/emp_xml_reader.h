//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XML_READER_H
#define EMP_XML_READER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct _xmlTextReader;
typedef _xmlTextReader xmlTextReader;
typedef xmlTextReader* xmlTextReaderPtr;

typedef void (*pfct_free_func)(void*);
typedef void* (*pfct_malloc_func)(c_size);
typedef void* (*pfct_malloc_atomic_func)(c_size);
typedef void* (*pfct_realloc_func)(void*, c_size);
typedef char* (*pfct_strdup_func)(cpc_char);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//namespace emp { namespace fct { template <typename T> class function_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace xml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name node_e
#define enum_type
#define enum_values\
    enum_value( Unknown   , 0)\
    enum_value( Element   , 1)\
    enum_value( Attribute , 2)\
    enum_value( Text      , 3)
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, parser_c)
    pfct_free_func pfctFreeFunc = nullptr;
    pfct_malloc_func pfctMallocFunc = nullptr;
    pfct_malloc_atomic_func pfctMallocAtomicFunc = nullptr;
    pfct_realloc_func pfctReallocFunc = nullptr;
    pfct_strdup_func pfctStrdupFunc = nullptr;

private:
    static parser_c* s_pInstance;
public:
    #define EMP_XML_PARSER emp::xml::parser_c::instance()
    static void set_instance(parser_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN parser_c& instance(void) { return *s_pInstance; }

    parser_c(void) {}
    ~parser_c(void) { destroy(); }

    EMP_RETURN bool create(void);
    void destroy(void);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, reader_c)
private:
    xmlTextReaderPtr m_pReader;
    bool m_bUnusedAttribute;

public:
#define enum_name read_result_e
#define enum_values\
    enum_value( Break    )\
    enum_value( Continue )\
    enum_value( Error    )
#include "emp_tt_enum.def.h"

#define enum_name parse_element_result_e
#define enum_values\
    enum_value( Break    )\
    enum_value( Continue )\
    enum_value( Skip     )\
    enum_value( Error    )
#include "emp_tt_enum.def.h"

#define enum_name parse_attribute_result_e
#define enum_values\
    enum_value( Break    )\
    enum_value( Continue )\
    enum_value( Error    )
#include "emp_tt_enum.def.h"

public:
    reader_c(void);
    ~reader_c(void);

public:
    EMP_RETURN bool create(c_astring& a_sFilename);
    void destroy(void);

public:
    typedef parse_element_result_e (*PFnParseElement)(reader_c const& a_rReader);
    typedef parse_attribute_result_e (*PFnParseAttribute)(reader_c const& a_rReader);

    typedef emp::fct::function_t<PFnParseElement> tfct_parse_element;
    typedef emp::fct::function_t<PFnParseAttribute> tfct_parse_attribute;

    EMP_RETURN bool parse(tfct_parse_element const& a_rfctParseElement, tfct_parse_attribute const& a_rfctParseAttribute);

    EMP_RETURN bool read_const_name(u8string& a_rsOut) const;
    EMP_RETURN bool read_const_name(astring& a_rsOut) const;
    EMP_RETURN bool read_const_value(u8string& a_rsOut) const;
    EMP_RETURN bool read_const_value(astring& a_rsOut) const;
    EMP_RETURN bool read_owned_value(u8string& a_rsOut) const;
    EMP_RETURN bool read_owned_value(astring& a_rsOut) const;

    EMP_RETURN bool read_element(c_astring& a_sName);
    EMP_RETURN bool read_const_attribute(c_astring& a_sName, astring& a_rsValue, c_bool a_bLog);
    EMP_RETURN bool read_owned_attribute(c_astring& a_sName, astring& a_rsValue, c_bool a_bLog);
    EMP_RETURN bool read_depth(size_t& a_rstDepth) const;

    EMP_RETURN node_e read_type(void) const;

    template <typename V> EMP_RETURN bool read_const_attribute(c_astring& a_sName, V& a_rValue, c_bool a_bLog);
    template <typename V> EMP_RETURN bool read_const_attribute_value(V& a_rValue, c_bool a_bLog);

    EMP_RETURN bool read_const_attribute_value(u8string& a_rsu8Value, c_bool EMP_SILENT(a_bLog))
    {
        u8string su8Value;
        EMP_RET_FALSE_IF_NOT(read_const_value(su8Value));
        a_rsu8Value.append_string(su8Value);
        return true;
    }

    EMP_RETURN bool read_const_attribute_value(astring& a_rsValue, c_bool EMP_SILENT(a_bLog))
    { return read_const_value(a_rsValue); }

    EMP_RETURN bool read_owned_attribute_value(u8string& a_rsu8Value, c_bool EMP_SILENT(a_bLog))
    { return read_owned_value(a_rsu8Value); }

    EMP_RETURN bool read_owned_attribute_value(astring& a_rsValue, c_bool EMP_SILENT(a_bLog))
    { return read_owned_value(a_rsValue); }

    EMP_RETURN bool next_valid_attribute(void);
    EMP_RETURN read_result_e next_valid_element(void) const;

    EMP_RETURN read_result_e next_attribute(void);
    EMP_RETURN read_result_e next_element(void) const;

    void set_unused_attribute(c_bool a_bValue) { m_bUnusedAttribute = a_bValue; }

private:
    EMP_RETURN bool log_error(void) const;

    EMP_RETURN bool get_next_element(astring& a_rOut);

public:
    static void cleanup_parser(void);

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

