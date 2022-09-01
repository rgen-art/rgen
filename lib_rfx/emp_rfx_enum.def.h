//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_ENUM_DEF_H
#define EMP_RFX_ENUM_DEF_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_fct_function.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove ?
#define EMP_RFX_ENUM_REF(x_Type, x_Field)\
    m_vsShortNames.add(astring(EMP_PP_STRING(x_Field)));\
    m_vsNames.add(astring(EMP_PP_STRING(x_Type::x_Field)));\
    m_veValues.add(x_Type::x_Field);\
    m_vpValues.add(EMP_ALLOCATE(emp::loc::string_s, astring(EMP_PP_STRING(x_Type::x_Field))));*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_NOCOPY_BASE(class, enum_i, public, type_i)
//};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, enum_t, public, enum_i)
private:
    static emp::cnt::map_t<T, astring>* s_pmNames;
    static emp::cnt::map_t<astring, T>* s_pmValues;
    static emp::cnt::map_t<T, u8string>* s_pmLocalized;

private:
    link_t<T> m_Link;

private:
    static void global_allocation(void)
    {
        c_size stCount = enum_type_alias(T)::count;
        if (s_pmNames == nullptr &&
            s_pmValues == nullptr &&
            s_pmLocalized == nullptr)
        {
            s_pmNames = EMP_ALLOCATE((emp::cnt::map_t<T, astring>), 8u); //-V747
            s_pmValues = EMP_ALLOCATE((emp::cnt::map_t<astring, T>), 8u); //-V747
            s_pmLocalized = EMP_ALLOCATE((emp::cnt::map_t<T, u8string>), 8u); //-V747
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                s_pmNames->set(enum_from(T, stIndex), enum_index_to_astring(T, stIndex));
                s_pmValues->set(enum_index_to_astring(T, stIndex), enum_from(T, stIndex));
            }

            EMP_FACTORY.register_deallocation(emp::fct::function(&enum_t::global_deallocation));
        }
    }

    static void global_deallocation(void)
    {
        if (s_pmNames != nullptr)
        {
            EMP_DEALLOCATE(s_pmNames);
            s_pmNames = nullptr;
        }

        if (s_pmValues != nullptr)
        {
            EMP_DEALLOCATE(s_pmValues);
            s_pmValues = nullptr;
        }

        if (s_pmLocalized != nullptr)
        {
            EMP_DEALLOCATE(s_pmLocalized);
            s_pmLocalized = nullptr;
        }
    }

public:
    enum_t(void):
        m_Link()
    { global_allocation(); }

    explicit enum_t(link_t<T> const& a_rLink):
        m_Link(a_rLink)
    { global_allocation(); }

    virtual~enum_t(void) {}

public:
    // >> type_i
    EMP_RETURN bool bind(void) { return m_Link.bind(); }
    EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<T>(); }
    EMP_RETURN bool post_load(void) { return true; }
    EMP_RETURN bool from_string(c_astring& a_sValue)
    {
        auto const oIt = s_pmValues->get(a_sValue);
        EMP_RET_FALSE_IF(oIt == s_pmValues->end());
        m_Link.value() = *oIt.value();
        return true;
    }

    EMP_RETURN bool from_string(c_u8string& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }

    EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), u8string& a_rsOut) const
    {
        auto const itLoc = s_pmLocalized->get(m_Link.value());
        if (itLoc == s_pmLocalized->end())
        {
            a_rsOut = EMP_LOCALIZE(emp::loc::string_s(STR_AFORMAT("#0::#1", EMP_FACTORY.id<T>()->m_sName, enum_to_astring(m_Link.value()))));
            s_pmLocalized->set(m_Link.value(), a_rsOut);

        }
        else
        {
            a_rsOut = *itLoc.value();
        }

        return true;
    }

    EMP_RETURN size_t field_count(void) const { return 1; }
    EMP_RETURN bool settable(void) const { return m_Link.setter().is_valid(); }
    EMP_RETURN bool bindable(void) const { return m_Link.binder().is_valid(); }
    EMP_RETURN bool iterate(c_size EMP_SILENT(a_stDepth), reflect_f const& EMP_SILENT(a_rCallback)) { return true; }
    EMP_RETURN bool reflect(factory_c& EMP_SILENT(a_rFactory)) { return true; }
   // << type_i

    // xml_i
    EMP_RETURN bool is_attribute(void) const { return true; }
    EMP_RETURN bool is_writable(void) const { return true; }

    EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size EMP_SILENT(a_stDepth), c_astring& EMP_SILENT(a_sDirectory))
    { return read_attribute(a_rReader); }

    EMP_RETURN bool eat_object(astring& EMP_SILENT(a_rReader))
    { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }

    EMP_RETURN bool read_attribute(xml::reader_c& a_rReader)
    {
        astring sValue;
        EMP_RET_FALSE_IF_NOT(a_rReader.read_const_attribute_value(sValue, true));
        return from_string(sValue);
    }

    EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, c_astring& a_sName)
    {
        auto const it = s_pmNames->get(m_Link.value());
        EMP_RET_FALSE_IF(it.end());
        EMP_RET_FALSE_IF_NOT(a_rWriter.write_attribute(a_sName, *it.value()));
        return true;
    }
    // << xml_i
};

template <typename T> emp::cnt::map_t<T, astring>* enum_t<T>::s_pmNames = nullptr;
template <typename T> emp::cnt::map_t<astring, T>* enum_t<T>::s_pmValues = nullptr;
template <typename T> emp::cnt::map_t<T, u8string>* enum_t<T>::s_pmLocalized = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef type_name
#error type_name NOT defined
#endif

#ifndef type_fields
#error type_fields NOT defined
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_NOCOPY_BASE(class,enum_c, public, enum_i)
    astring m_sXmlType;
    vector<astring> m_vsNames;
    vector<astring> m_vsShortNames;
    vector<string_s*> m_vpValues;

    EMP_RETURN c_astring& xml_type(void) const { return m_sXmlType; }

    EMP_RETURN bool reflect(factory_c* const a_pFactory)
    {
        m_vsNames.resize(0);
        m_vsShortNames.resize(0);
        m_veValues.resize(0);
#define enum_value_ex_impl(x_Field) EMP_RFX_ENUM_REF(EMP_RFX_META_NAME(), x_Field)
#include "emp_enu_enum_ex.def"
        return true;
    }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_NOCOPY_BASE(class, type_t<type_name>, public, enum_t<type_name>)
public:
    emp::loc::string_s m_Dummy;

public:
    type_t(void):
        m_Dummy()
    {}

    explicit type_t(link_t<type_name> const& a_rLink):
        enum_t(a_rLink),
        m_Dummy()
    {}

public:
    static EMP_RETURN EMP_INLINE type_s* new_id(factory_c& a_rFactory)
    { return a_rFactory.new_id<type_name>(ALITERAL(EMP_PP_STRING(type_name)), type_e::Enum); }

    //>> enum_i
    EMP_RETURN size_t enum_value_count(void) const { EMP_ASSERT_NOT_IMPLEMENTED(); return 0; }
    EMP_RETURN emp::loc::string_s const& enum_to_string(c_size EMP_SILENT(a_stValue)) const { EMP_ASSERT_NOT_IMPLEMENTED(); return m_Dummy; }
    
    void enum_from_index(c_size EMP_SILENT(a_stVlalue)) { EMP_ASSERT_NOT_IMPLEMENTED(); }
    EMP_RETURN size_t enum_current_index(void) const { EMP_ASSERT_NOT_IMPLEMENTED(); return 0; }
    //<< enum_i

    EMP_RETURN bool write(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }

    EMP_RETURN bool write_object(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }
    EMP_RETURN bool write_element(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }

public:
    // >> type_i
   /*EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<type_name>(); }
   EMP_RETURN bool from_string(c_astring& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN bool from_string(c_u8string& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), astring& EMP_SILENT(a_rsOut)) const { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), u8string& EMP_SILENT(a_rsOut)) const { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN size_t field_count(void) const { EMP_ASSERT_NOT_IMPLEMENTED(); return 0; }
   EMP_RETURN bool settable(void) const { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN bool bindable(void) const { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN bool iterate(c_size EMP_SILENT(a_stDepth), reflect_f const& EMP_SILENT(a_rCallback)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
   EMP_RETURN bool reflect(factory_c& EMP_SILENT(a_rFactory))
   { return true; }*/ // @@0 implement
    // << type_i


/*
public:
    type_t(
        getter_t<enum_name>* const a_pGetter,
        setter_t<enum_name>* const a_pSetter,
        binder_t<enum_name>* const a_pBinder):
        m_Bind(a_pGetter, a_pSetter, a_pBinder),
        m_sXmlType(),
        m_vsNames(),
        m_vsShortNames(),
        m_veValues(),
        m_vpValues()
    { compute_xml_type(m_Bind.s_type(), m_sXmlType); }


    ~type_t(void)
    { foreach(m_vpValues, safe_delete_reset_t()); }

public:
    //>> type_i

    EMP_RETURN bool reflect(factory_c* const a_pFactory)
    {
        m_vsNames.resize(0);
        m_vsShortNames.resize(0);
        m_veValues.resize(0);
#define enum_value_ex_impl(x_Field) EMP_RFX_ENUM_REF(EMP_RFX_META_NAME(), x_Field)
#include "emp_enu_enum_ex.def"
        return true;
    }

    EMP_RETURN bool write(writer_c& a_rWriter)
    { return emp::rfx::write(this, a_rWriter); }

    EMP_RETURN bool write_object(writer_c& a_rWriter)
    {
    c_size stIndex = get_index();
    c_astring& sValue = m_vsShortNames.ref(stIndex);
    EMP_RET_FALSE_IF_NOT(a_rWriter.write_attribute(EMP_RFX_TOKEN_V_VALUE_V1, sValue));
    return true;
}

    EMP_RETURN astring to_string(formater_i* const a_pFormater) const{
    c_size stIndex = get_index();
    if (emp::tt::is_max(stIndex))
    { return astring(); }
    // @@0  hc
    return EMP_LOCALIZE(*m_vpValues.ref(stIndex));
}

    EMP_RETURN bool from_string(c_astring& a_sValue)
    {
    size_t stValue = 0;
    if (emp::str::to(a_sValue, stValue))
    {
        EMP_RFX_META_NAME()::CValue eCastedValue =
            emp::ast::cast<EMP_RFX_META_NAME()::Value>(stValue);
        if (emp::enu::typeof_t<EMP_RFX_META_NAME()::Value>::type::is_valid(
                eCastedValue) == false)
        {
            EMP_LOG_ERROR(
                "Failed to cast enum #0 from #1", this->xml_type(), a_sValue);
            return false;
        }

        c_size stCount = m_veValues.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            EMP_RFX_META_NAME()::CValue eValue = m_veValues.val(stIndex);
            if (eValue == stValue &&
                eValue == eCastedValue)
            {
                m_Bind.value() = eValue;
                return true;
            }
        }
    }
    else
    {
        {
            c_size stCount = m_vsShortNames.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                if (emp::str::cmp(m_vsShortNames.ref(stIndex), a_sValue))
                {
                    m_Bind.value() = m_veValues.val(stIndex);
                    return true;
                }
            }
        }
        {
            c_size stCount = m_vsNames.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                if (emp::str::cmp(m_vsNames.ref(stIndex), a_sValue))
                {
                    m_Bind.value() = m_veValues.val(stIndex);
                    return true;
                }
            }
        }
    }

    EMP_LOG_ERROR(
        "Failed to read enum #0 from #1", this->xml_type(), a_sValue);
    return false;
}


    EMP_RETURN size_t field_count(void) const { return 1; }
    //<< type_i

    //>> class_i
    void set_value(enum_name* const a_pValue) { m_Bind.set_value(a_pValue); }
    //<< class_i

    //>> enum_i
    EMP_RETURN size_t enum_value_count(void) const { return m_vsNames.count(); }
    EMP_RETURN string_s const& enum_to_string(c_size a_stValue) const { return *m_vpValues.val(a_stValue); }
    void enum_from_index(c_size a_stIndex) { m_Bind.value() = m_veValues.val(a_stIndex); }

    EMP_RETURN size_t enum_current_index(void) const
    {
        c_size stCount = enum_value_count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (m_Bind.value() == m_veValues.val(stIndex))
            { return stIndex; }
        }
        return emp::tt::max<size_t>::value;
    }
    //<< enum_i

private:
    EMP_RETURN auto get_index(void) const
    {
        c_size stCount = m_veValues.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (m_veValues.val(stIndex) == m_Bind.value())
            { return stIndex; }
        }
        return emp::tt::max<size_t>::value;
    }
*/
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef type_name
#undef type_fields

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
