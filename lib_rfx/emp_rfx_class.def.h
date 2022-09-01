//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_CLASS_DEF_H
#define EMP_RFX_CLASS_DEF_H
#include "emp_rfx_class.h"
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef type_name
#error type_name NOT defined
#endif

#ifndef type_fields
#error type_fields NOT defined
#endif

#ifdef type_interface
#define type_interface_impl true
#else
#define type_interface_impl false
#endif

#ifdef type_pointer
#define type_pointer_impl true
#else
#define type_pointer_impl false
#endif

#ifdef type_pod
#define type_pod_impl true
#else
#define type_pod_impl false
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_NOCOPY_BASE(class, (type_t<type_name>), public, class_c)
private:
    friend class factory_c;
public:
    static constexpr c_bool pod = type_pod_impl;

private:
    bases<type_name> m_tuBases;
    link_t<type_name> m_Link;

public:
    type_t(void):
        m_tuBases(),
        m_Link()
    {}

    type_t(binder_t<type_name> const& a_Binder):
        m_tuBases(),
        m_Link(a_Binder)
    {}

    type_t(getter_t<type_name> const& a_Getter):
        m_tuBases(),
        m_Link(a_Getter)
    {}

    type_t(getter_t<type_name> const& a_Getter, setter_t<type_name> const& a_Setter):
        m_tuBases(),
        m_Link(a_Getter, a_Setter)
    {}

    virtual ~type_t(void)
    { }

public:
    // >> type_i
    EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<type_name>(); }
    EMP_RETURN link_t<type_name> const& link(void) const { return m_Link; }
    EMP_RETURN link_t<type_name>& link(void) { return m_Link; }
    EMP_RETURN bool bind(void) { return m_Link.bind(); }
    EMP_RETURN bool from_string(c_astring&) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool from_string(c_u8string&) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), astring& EMP_SILENT(a_rsOut)) const { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), u8string& a_rsOut) const
    { a_rsOut = u8string(U8LITERAL("...")); return true; }
    EMP_RETURN bool settable(void) const { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool bindable(void) const { return m_Link.binder().is_valid() && bindable_base<type_name>(); }

    EMP_RETURN bool reflect(factory_c& a_rFactory)
    {
        EMP_RET_FALSE_IF_NOT((reflect_base<type_name>(a_rFactory)));
#define enum_values type_fields
#define enum_value_ex_impl(x_Type, x_Bind, x_Get, x_Set, x_Name, x_Field)\
    EMP_RFX_FIELD_REF(x_Bind, x_Get, x_Set, x_Type, x_Name, x_Field)
#include "emp_tt_enum_ex.def.h"
#undef enum_value_ex_impl
        EMP_RET_FALSE_IF_NOT(reflect_bases());
        return true;
    }
    // << type_i

    template <typename U, size_t t_stCount> friend struct bindable_base_t;
    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, bindable_base_t)
        static EMP_RETURN bool apply(type_t<U> const& a_rClass)
        { return std::get<t_stCount - 1>(a_rClass.m_tuBases)->bindable() && bindable_base_t<U, t_stCount - 1>::apply(a_rClass); }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (bindable_base_t<U, 0>))
        static EMP_RETURN bool apply(type_t<U> const&) { return true; }
    };

    ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool bindable_base(void) const
    { return bindable_base_t<U, std::tuple_size<typename U::tbases>::value>::apply(*this); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool bindable_base(void) const
    { return true; }


    template <typename U, size_t t_stCount> friend struct reflect_base_t;
    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, reflect_base_t)
        static EMP_RETURN bool apply(factory_c& a_rFactory, type_t<U>& a_rClass)
        {
            EMP_RET_FALSE_IF_NOT((reflect_base_t<U, t_stCount - 1>::apply(a_rFactory, a_rClass)));
            typedef typename std::tuple_element<t_stCount - 1, typename U::tbases>::type tbase;
            auto* pBase = EMP_ALLOCATE(type_t<tbase>); // @@0 shared_ptr
            a_rClass.m_vpBases.add(pBase);
            std::get<t_stCount - 1>(a_rClass.m_tuBases) = pBase;
            EMP_RET_FALSE_IF_NOT(a_rFactory.reflect(*pBase));
            return true;
        }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (reflect_base_t<U, 0>))
        static EMP_RETURN bool apply(factory_c&, type_t<U>&) { return true; }
    };

    ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool reflect_base(factory_c& a_rFactory)
    { return reflect_base_t<U, std::tuple_size<typename U::tbases>::value>::apply(a_rFactory, *this); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool reflect_base(factory_c&)
    { return true; }

    EMP_RETURN bool post_load(void)
    { return call_post_load<emp::tt::has_function_post_load<type_name, bool(void)>::value>::apply(m_Link.value()); }

/*    ENABLE_IF_(U, emp::tt::has_function_post_load<U, bool(void)>::value)
    EMP_RETURN bool post_load_impl(void)
    {
        static_assert(emp::tt::has_function_post_load<U, bool(void)>::value, "");
        return m_Link.value().post_load();
    }

    ENABLE_IF_(U, false == emp::tt::has_function_post_load<U, bool(void)>::value)
    EMP_RETURN bool post_load_impl(void)
    { return true; }*/


    EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    {
        EMP_RET_FALSE_IF_NOT(emp::rfx::read_object(a_rReader, a_stDepth, a_sDirectory, *this));
        return add_post_load<emp::tt::has_function_post_load<type_name, bool(void)>::value>::apply(m_Link.value());
    }

    EMP_RETURN bool eat_object(astring& a_rsReader)
    {
        EMP_RET_FALSE_IF_NOT(emp::rfx::eat_object(a_rsReader, *this));
        return add_post_load<emp::tt::has_function_post_load<type_name, bool(void)>::value>::apply(m_Link.value());
    }

    EMP_RETURN bool write(xml::writer_c& a_rWriter)
    { return emp::rfx::write_object(a_rWriter, *this); }

    EMP_RETURN bool write_object(xml::writer_c& a_rWriter)
    { return emp::rfx::write_object(a_rWriter, *this); }

    EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, c_astring& a_sName)
    { return emp::rfx::write_attribute(a_rWriter, *this, a_sName); }

    EMP_RETURN bool write_element(xml::writer_c& a_rWriter)
    { return write_element_impl<type_name>(a_rWriter); }

    ENABLE_IF_(U, is_data<U>::value)
    EMP_RETURN bool write_element_impl(xml::writer_c& a_rWriter)
    { return emp::rfx::write_element(a_rWriter, *this); }

    ENABLE_IF_(U, is_data<U>::value == false)
    EMP_RETURN bool write_element_impl(xml::writer_c& a_rWriter)
    { return emp::rfx::write_object(a_rWriter, *this); }

    EMP_RETURN bool is_attribute(void) const { return false; }
    EMP_RETURN bool is_writable(void) const { return true; }

private:
#define enum_values type_fields
#define enum_value_ex_impl(x_Type, x_Bind, x_Get, x_Set, x_Name, x_Field)\
    EMP_RFX_FIELD_IMP(x_Bind, x_Get, x_Set, x_Type, x_Name, x_Field)
#include "emp_tt_enum_ex.def.h"
#undef enum_value_ex_impl

    static EMP_RETURN EMP_INLINE type_s* new_id(factory_c& a_rFactory)
    { return a_rFactory.new_id<type_name>(ALITERAL(EMP_PP_STRING(type_name)), type_e::Class); }

private:
    /*EMP_RETURN bool add(c_astring const& a_sField, string_s const& a_rName, string_s const& a_rDescription,
        type_i* const a_pReflection)
    {
        EMP_RET_FALSE_IF_NOT(add(a_sField, a_pReflection));
        m_vsNames.add(a_rName);
        m_vsDescriptions.add(a_rDescription);
        return true;
    }

    EMP_RETURN bool add(c_astring const& a_sField, type_i* const a_pReflection)
    {
        EMP_ASSERT(a_pReflection);
        m_vsFields.add(a_sField);
        m_vpFields.add(a_pReflection);
        return true;
    }*/

public:
    void set_value(type_name* const a_pValue)
    {
        m_Link.set_value(a_pValue);
        set_value_base<type_name>(a_pValue);
    }

private:
    template <typename U, size_t t_stCount> friend struct set_value_base_t;

    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, set_value_base_t)
        static void apply(type_t<U>& a_rClass, type_name* const a_pValue)
        {
            std::get<t_stCount - 1>(a_rClass.m_tuBases)->set_value(a_pValue);
            set_value_base_t<U, t_stCount - 1>::apply(a_rClass, a_pValue);
        }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (set_value_base_t<U, 0>))
        static void apply(type_t<U>&, type_name* const)
        { }
    };

    ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    void set_value_base(type_name* const a_pValue)
    { return set_value_base_t<U, std::tuple_size<typename U::tbases>::value>::apply(*this, a_pValue); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    void set_value_base(type_name* const)
    { }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef type_to_string // @@0 factor
template <>
EMP_INLINE EMP_RETURN bool to_string<type_name>(type_name const& a_rValue, formater_i* const EMP_SILENT(a_pFormater), u8string& a_rsOut)
{ a_rsOut = u8string(type_to_string); return true; }
#else
#ifdef type_to_string_impl
template <>
EMP_INLINE EMP_RETURN bool to_string<type_name>(type_name const& a_rValue, formater_i* const a_pFormater, u8string& a_rsOut)
{ type_to_string_impl; }
#else
#define type_to_string_impl { }
template <>
EMP_INLINE EMP_RETURN bool to_string<type_name>(type_name const& EMP_SILENT(a_rValue), formater_i* const EMP_SILENT(a_pFormater), u8string& a_rsOut)
{ a_rsOut = u8string(EMP_FACTORY.id<type_name>()->m_sName); return true; }
#endif
#endif


} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef type_name
#undef type_fields
#undef type_interface
#undef type_interface_impl
#undef type_pointer
#undef type_pointer_impl
#undef type_to_string
#undef type_to_string_impl
#undef type_pod
#undef type_pod_impl

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
