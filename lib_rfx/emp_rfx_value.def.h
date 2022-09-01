//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_VALUE_DEF_H
#define EMP_RFX_VALUE_DEF_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*#define type_name emp::tim::time_c
#define type_to_string emp::str::afrom(a_rValue) // @@0 alloc
#define type_fields\
   enum_value_ex(ui64, (EMP_RFX_B, usec, m_ui64usec))
#include "emp_rfx_class.def.h"*/

/*
    EMP_RETURN bool write_object(writer_c& a_rWriter)
    { return a_rWriter.write_attribute(EMP_RFX_TOKEN_V_VALUE_V1, to_xml_imp(get_value())); }
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,value_i, public, type_i)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool read_value(xml::reader_c& a_rReader, T& a_rtValue)
{
    astring sValue;
    EMP_RET_FALSE_IF_NOT(a_rReader.read_const_value(sValue));
    return emp::str::to(sValue, a_rtValue, nullptr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_INLINE EMP_RETURN bool read_value(xml::reader_c& a_rReader, astring& a_rtValue)
{ return a_rReader.read_owned_value(a_rtValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_INLINE EMP_RETURN bool read_value(xml::reader_c& a_rReader, u8string& a_rtValue)
{ return a_rReader.read_owned_value(a_rtValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_NOCOPY_BASE(class,value_c, public, type_i)
private:
    type_e m_eType;

public:
    value_c(c_type_e a_eType): m_eType(a_eType) {}
    ~value_c(void) {}
c_size
public:
    //>> type_i
    EMP_RETURN type_e e_type(void) const { return t_eEnum; }
    EMP_RETURN c_astring& s_type(void) const { return m_Bind.s_type(); }
    EMP_RETURN c_astring& xml_type(void) const { return m_Bind.s_type(); }

    EMP_RETURN bool reflect(factory_c* const) { return true; }
    EMP_RETURN bool iterate(c_size, reflect_f const&) { return true; }

    EMP_RETURN bool read(reader_c& a_rReader, c_size, c_astring&)
    { return read_attribute(a_rReader); }

    EMP_RETURN bool write(writer_c& a_rWriter)
    { return emp::rfx::write(this, a_rWriter); }

    EMP_RETURN astring to_string(formater_i* const a_pFormater) const
    { return to_string_impl(get_value(), a_pFormater); }

    EMP_RETURN size_t field_count(void) const { return 1; }
    //<< type_i
};*/



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_value_ex_impl(x_Enum, x_Type, x_Name, x_Value, x_Pod)\
EMP_PP_IF(x_Value, \
template <>\
EMP_NOCOPY_BASE(class, type_t<x_Type>, public, type_i)\
private:\
    link_t<x_Type> m_Link;\
public:\
    static constexpr c_bool pod = x_Pod;\
public:\
    type_t(void):\
        m_Link()\
    {}\
    type_t(binder_t<x_Type> const& a_Binder):\
        m_Link(a_Binder)\
    {}\
    type_t(getter_t<x_Type> const& a_Getter):\
        m_Link(a_Getter)\
    {}\
    type_t(getter_t<x_Type> const& a_Getter, setter_t<x_Type> const& a_Setter):\
        m_Link(a_Getter, a_Setter)\
    {}\
    ~type_t(void)\
    {}\
public:\
    EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<x_Type>(); }\
    EMP_RETURN link_t<x_Type> const& link(void) const { return m_Link; }\
    EMP_RETURN link_t<x_Type>& link(void) { return m_Link; }\
    EMP_RETURN bool bind(void) { return m_Link.bind(); }\
    EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& EMP_SILENT(a_sDirectory))\
    {\
        size_t EMP_TT_MAX_VAR(stCurrentDepth);\
        if ((stCurrentDepth + 1) == a_stDepth ||\
            a_rReader.read_type() != xml::node_e::Text)\
        {\
            xml::reader_c::read_result_e eReturn = a_rReader.next_element();\
            EMP_RET_FALSE_IF(eReturn != xml::reader_c::read_result_e::Continue);\
        }\
        if (m_Link.has_value())\
        { EMP_RET_FALSE_IF_NOT(read_value<x_Type>(a_rReader, m_Link.value())); }\
        else\
        {\
            if (m_Link.setter().is_valid())\
            {\
                x_Type oValue;\
                EMP_RET_FALSE_IF_NOT(read_value(a_rReader, oValue));\
                EMP_RET_FALSE_IF_NOT(m_Link.setter()(oValue));\
            }\
        }\
        return true;\
    }\
    EMP_RETURN bool eat_object(astring& a_rsValue)\
    {\
        astring sEnd;\
        if (m_Link.has_value())\
        { EMP_RET_FALSE_IF_NOT(emp::str::to(a_rsValue, m_Link.value(), &sEnd)); }\
        else\
        {\
            if (m_Link.setter().is_valid())\
            {\
                x_Type oValue;\
                EMP_RET_FALSE_IF_NOT(emp::str::to(a_rsValue, oValue, &sEnd));\
                EMP_RET_FALSE_IF_NOT(m_Link.setter()(oValue));\
            }\
            /* else { EMP_ASSERT(false); } // @@2 unused ?*/\
        }\
        a_rsValue = sEnd;\
        return true;\
    }\
    EMP_RETURN bool from_string(c_astring& a_sValue) { return m_Link.from_string(a_sValue);}\
    EMP_RETURN bool from_string(c_u8string& a_sValue) { return m_Link.from_string(a_sValue); }\
    EMP_RETURN bool post_load(void) { return true; }\
    EMP_RETURN bool to_string(str::formater_i* const a_pFormater, u8string& a_rsOut) const\
    { return to_string_impl(get_value(), a_pFormater, a_rsOut); }\
    EMP_RETURN size_t field_count(void) const { return 1; }\
    EMP_RETURN bool settable(void) const { return m_Link.setter().is_valid(); }\
    EMP_RETURN bool bindable(void) const { return m_Link.binder().is_valid(); }\
    EMP_RETURN bool iterate(c_size EMP_SILENT(a_stDepth), reflect_f const& EMP_SILENT(a_rCallback))\
    { return true; /*EMP_ASSERT_NOT_IMPLEMENTED(); return false;*/ }\
    EMP_RETURN bool reflect(factory_c& EMP_SILENT(a_rFactory))\
    { return true; }\
    static EMP_INLINE EMP_RETURN type_s* new_id(factory_c& a_rFactory)\
    { return a_rFactory.new_id<x_Type>(ALITERAL(EMP_PP_STRING(x_Type)), type_e::x_Enum); }\
    template <typename T>\
    EMP_RETURN bool read_attribute_value_impl(xml::reader_c& a_rReader, T& a_rValue)\
    { return a_rReader.read_const_attribute_value(a_rValue, true); }\
    EMP_RETURN bool read_attribute_value_impl(xml::reader_c& a_rReader, astring& a_rValue)\
    { return a_rReader.read_owned_attribute_value(a_rValue, true); }\
    EMP_RETURN bool read_attribute_value_impl(xml::reader_c& a_rReader, u8string& a_rValue)\
    { return a_rReader.read_owned_attribute_value(a_rValue, true); }\
    EMP_RETURN bool read_attribute(xml::reader_c& a_rReader)\
    {\
        /*EMP_RET_FALSE_IF_NOT(emp::rfx::read_type(a_rReader, m_Bind.s_type(), true));*/\
        if (m_Link.has_value())\
        { EMP_RET_FALSE_IF_NOT(read_attribute_value_impl(a_rReader, m_Link.value())); }\
        else\
        {\
            if (m_Link.setter().is_valid())\
            {\
                x_Type oValue;\
                EMP_RET_FALSE_IF_NOT(read_attribute_value_impl(a_rReader, oValue));\
                EMP_RET_FALSE_IF_NOT(m_Link.setter()(oValue));\
            }\
            /* else { EMP_ASSERT(false); } // @@2 unused ?*/\
        }\
        return true;\
    }\
    EMP_RETURN x_Type get_value(void) const\
    {\
        x_Type tValue;\
        if (m_Link.has_value())\
        { tValue = m_Link.value(); }\
        else\
        { EMP_ASSERT_BOOL_CALL(m_Link.getter()(tValue)); }\
        return tValue;\
    }\
    void set_value(x_Type* const a_pValue) { m_Link.set_value(a_pValue); }\
    EMP_RETURN bool write(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }\
    EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, c_astring& a_sName)\
    {\
        u8string sValue;\
        EMP_RET_FALSE_IF_NOT(to_string_impl(get_value(), nullptr, sValue));\
        return a_rWriter.write_attribute(a_sName, sValue);\
    }\
    EMP_RETURN bool write_object(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }\
    EMP_RETURN bool write_element(xml::writer_c& a_rWriter)\
    {\
        u8string sValue;\
        EMP_RET_FALSE_IF_NOT(to_string_impl(get_value(), nullptr, sValue));\
        return a_rWriter.write_raw(sValue);\
    }\
    EMP_RETURN bool is_attribute(void) const { return true; }\
    EMP_RETURN bool is_writable(void) const { return true; }\
};)
#include "emp_tt_enum_ex.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef enum_name
#undef enum_values

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
