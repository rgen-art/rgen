//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XML_WRITER_H
#define EMP_XML_WRITER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct _xmlTextWriter;
typedef _xmlTextWriter xmlTextWriter;
typedef xmlTextWriter* xmlTextWriterPtr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//@@string namespace emp { namespace str { class u8string; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace xml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, writer_c)
private:
    xmlTextWriter* m_pWriter;
    bool m_bIndent = false;

public:
    writer_c(void);
    ~writer_c(void);

    EMP_RETURN bool indent(void) const;
    EMP_RETURN bool indent(c_bool a_bValue);

    EMP_RETURN bool create(c_astring& a_sFilename);
    void destroy(void);

    EMP_RETURN bool start_document(void);
    EMP_RETURN bool end_document(void);

    EMP_RETURN bool start_element(c_astring& a_sName);
    EMP_RETURN bool end_element(void);

    template <typename V>
    EMP_RETURN bool write_element(c_astring& a_sName, V const& a_rtValue);
    EMP_RETURN bool write_element(c_astring& a_sName, c_astring& a_sValue);

    template <typename V>
    EMP_RETURN bool write_attribute(c_astring& a_sName, V const& a_rtValue);

    EMP_RETURN bool write_attribute(c_astring& a_sName, c_astring& a_sValue);
    EMP_RETURN bool write_attribute(c_astring& a_sName, c_u8string& a_sValue);

    // @@0 remove EMP_RETURN bool write_raw(cpc_char a_szValue) { return write_raw<c_char*>(a_szValue); }

    //template <typename V>
    //EMP_RETURN bool write_raw(V const& a_rtValue);
    EMP_RETURN bool write_raw(c_u8string& a_sValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

