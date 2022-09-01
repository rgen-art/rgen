//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_CLASS_H
#define EMP_RFX_CLASS_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_type_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_cat.h"
#include "emp_pp_if.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RFX_B true, false, false
#define EMP_RFX_R false, true, false
#define EMP_RFX_RW false, true, true

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RFX_FIELD_REF(x_Bind, x_Get, x_Set, x_Type, x_Name, x_Field)\
{\
    emp::rfx::type_i* const pType = EMP_ALLOCATE((emp::rfx::type_t<EMP_PP_TRY_SOLVE(x_Type)>),\
        EMP_PP_IF(x_Bind, emp::fct::function(this, &type_t::EMP_PP_CAT(bind_, x_Name)))\
        EMP_PP_IF(x_Get, emp::fct::function(this, &type_t::EMP_PP_CAT(get_, x_Name))\
            EMP_PP_COMMA_IF(x_Set)\
        EMP_PP_IF(x_Set, emp::fct::function(this, &type_t::EMP_PP_CAT(set_, x_Name)))));\
    EMP_RET_FALSE_IF_NOT(a_rFactory.reflect(*pType));\
    add_field(field_s(pType, ALITERAL(EMP_PP_STRING(x_Name)),\
        emp::loc::string_s(ALITERAL(EMP_PP_STRING(type_name.x_Name::name))),\
        emp::loc::string_s(ALITERAL(EMP_PP_STRING(type_name.x_Name::desc)))));\
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RFX_FIELD_IMP(x_Bind, x_Get, x_Set, x_Type, x_Name, x_Field)\
    EMP_PP_IF(x_Bind, EMP_RETURN bool EMP_PP_CAT(bind_, x_Name)(EMP_PP_TRY_SOLVE(x_Type)*& a_prtValue)\
    {\
        a_prtValue = &link().value().x_Field;\
        return true;\
    })\
    EMP_PP_IF(x_Get , EMP_RETURN bool EMP_PP_CAT(get_ , x_Name)(EMP_PP_TRY_SOLVE(x_Type)& a_rtValue)\
    {\
        a_rtValue = link().value().x_Name();\
        return true;\
    }\
        EMP_PP_IF(x_Set , EMP_RETURN bool EMP_PP_CAT(set_ , x_Name)(EMP_PP_TRY_SOLVE(x_Type) const& a_rtValue)\
        {\
            link().value().EMP_PP_CAT(set_, x_Name)(a_rtValue);\
            return true;\
        }))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T>
//EMP_NOCOPY_BASE(class,EMP_PP_FORWARD(base_t<T, true>), public, base_i)
//private:
    /*base_t<T, false> m_NoBase;
    EMP_POINTER(type_t<typename T::tbase1>) m_pBase;

public:
    base_t(getter_t<T>* const a_pGetter, setter_t<T>* const a_pSetter, binder_t<T>* const a_pBinder):
    m_NoBase(a_pGetter, a_pSetter, a_pBinder),
    m_pBase(
        emp::ast::cast<getter_t<typename T::tbase1>*>(nullptr),
        emp::ast::cast<setter_t<typename T::tbase1>*>(nullptr),
        emp::ast::cast<binder_t<typename T::tbase1>*>(nullptr))
    {}

    ~base_t(void) {}

public:
    //>> type_i
    EMP_RETURN type_e e_type(void) const { return type_e::Class; }
    EMP_RETURN c_astring& s_type(void) const { return m_NoBase.s_type(); }
    EMP_RETURN c_astring& xml_type(void) const { return m_NoBase.xml_type(); }

    EMP_RETURN bool reflect(factory_c* const a_pFactory)
    { return m_pBase->reflect(a_pFactory); }

    EMP_RETURN bool read(reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    { return read(this, a_rReader, a_stDepth, a_sDirectory); }

    EMP_RETURN bool read_object(reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    { return read_object(this, a_rReader, a_stDepth, a_sDirectory); }

    EMP_RETURN bool write(writer_c& a_rWriter)
    { return write(this, a_rWriter); }

    EMP_RETURN bool write_object(writer_c& a_rWriter)
    {
        EMP_RET_FALSE_IF_NOT(m_pBase->write_object(a_rWriter));
        EMP_RET_FALSE_IF_NOT(m_NoBase.write_object(a_rWriter));
        return true;
    }

    EMP_RETURN bool settable(void) const
    { return m_NoBase.settable() && m_pBase->settable(); }

    EMP_RETURN bool bindable(void) const
    { return m_NoBase.bindable() && m_pBase->bindable(); }

    EMP_RETURN astring to_string(formater_i* const a_pFormater) const { return m_NoBase.to_string(a_pFormater); }
    EMP_RETURN bool from_string(c_astring& a_sValue) { return m_NoBase.from_string(a_sValue); }
    //<< type_i

    //>> base_i
    EMP_RETURN c_astring& ctor_type(void) const { return m_NoBase.ctor_type(); }

    EMP_RETURN bool pre_load(void) { return m_NoBase.pre_load(); }
    EMP_RETURN bool post_load(void) { return m_NoBase.post_load(); }

    EMP_RETURN bool read_base(c_astring& a_sDirectory)
    { return read_ctor(m_pBase, a_sDirectory); }

    void reset_all_read(void)
    {
        m_pBase->reset_all_read();
        m_NoBase.reset_all_read();
    }

    EMP_RETURN bool all_read(void) const
    { return m_pBase->all_read() && m_NoBase.all_read(); }

    void set_read(c_size a_stIndex, c_bool a_bValue)
    {
        c_size st0 = m_NoBase.field_count();
        c_size st1 = st0 + m_pBase->field_count();

        if (a_stIndex < st0)
        { m_NoBase.set_read(a_stIndex, a_bValue); }
        else if (a_stIndex < st1)
        { m_pBase->set_read(a_stIndex - st0, a_bValue); }
        else
        { EMP_ASSERT(false); }
    }

    EMP_RETURN size_t field_count(void) const
    { return m_NoBase.field_count() + m_pBase->field_count(); }

    EMP_RETURN c_astring& field_name(c_size a_stIndex) const
    {
        c_size st0 = m_NoBase.field_count();
        if (a_stIndex < st0)
        { return m_NoBase.field_name(a_stIndex); }
        return m_pBase->field_name(a_stIndex - st0);
    }

    EMP_RETURN type_i* field_reflection(c_size a_stIndex) const
    {
        c_size st0 = m_NoBase.field_count();
        c_size st1 = st0 + m_pBase->field_count();

        if (a_stIndex < st0)
        { return m_NoBase.field_reflection(a_stIndex); }
        else if (a_stIndex < st1)
        { return m_pBase->field_reflection(a_stIndex - st0); }
        EMP_ASSERT(false);
        return nullptr;
    }
    //<< base_i

    //>> class_i
    void set_value(T* const a_pValue)
    {
        m_NoBase.set_value(a_pValue);
        m_pBase->set_value(a_pValue);
    }
    //<< class_i

    void add(cpc_char a_szField,
    #ifdef EMP_XX_LOCALIZE
            string_s const& a_rName, string_s const& a_rDescription,
    #endif
            type_i* const a_pReflection)
    {
        m_NoBase.add(a_szField,
    #ifdef EMP_XX_LOCALIZE
            a_rName, a_rDescription,
    #endif
            a_pReflection);
    }

    EMP_RETURN T const& value(void) const { return m_NoBase.value(); }
    EMP_RETURN T& value(void) { return m_NoBase.value(); }*/
//};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T, bool t_bValue>
//EMP_NOCOPY_BASE(class,base_t, public, base_i)
    /*
    ~base_t(void)
    {
        foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
        m_vpReflections.resize(0);
    }

    //>> type_i
    EMP_RETURN bool reflect(factory_c* const) { return true; }

    EMP_RETURN bool write(writer_c& a_rWriter)
    { return write(this, a_rWriter); }

    EMP_RETURN bool write_object(writer_c& a_rWriter)
    {
        c_size stCount = m_vpReflections.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            type_i* const pReflection = m_vpReflections.val(stIndex);

            EMP_RET_FALSE_IF_NOT(pReflection->bind());

            if (pReflection->field_count() == 1 &&
                pReflection->e_type() != emp::rfx::type_e::Vector &&
                pReflection->e_type() != emp::rfx::type_e::HeapObject &&
                pReflection->e_type() != emp::rfx::type_e::Class)
            {
                EMP_RET_FALSE_IF_NOT(pReflection->write_attribute(m_vsFields.ref(stIndex), a_rWriter),
                    "Failed to write : #0 #1;", pReflection->xml_type(), m_vsFields.ref(stIndex));
            }
        }

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            type_i* const pReflection = m_vpReflections.val(stIndex);

            EMP_RET_FALSE_IF_NOT(pReflection->bind());

            if (pReflection->field_count() > 1 ||
                pReflection->e_type() == emp::rfx::type_e::Vector ||
                pReflection->e_type() == emp::rfx::type_e::HeapObject ||
                pReflection->e_type() == emp::rfx::type_e::Class)
            {
                EMP_RET_FALSE_IF_NOT(a_rWriter.start_element(emp::str::u8string(m_vsFields.ref(stIndex))));

                EMP_RET_FALSE_IF_NOT(pReflection->write(a_rWriter),
                    "Failed to write : #0 #1;", pReflection->xml_type(), m_vsFields.ref(stIndex))

                EMP_RET_FALSE_IF_NOT(a_rWriter.end_element());
            }
        }
        return true;
    }

    EMP_RETURN astring to_string(formater_i* const a_pFormater) const { return to_string_imp<T>(a_pFormater); }

    EMP_RETURN bool from_string(c_astring& EMP_SILENT(a_sValue) { return false; }

    //<< type_i

    //>> base_i
    EMP_RETURN c_astring& ctor_type(void) const { return m_sCtorType; }

    EMP_RETURN bool pre_load(void)
    { return call_pre_load<T, has_pre_load<T>::value>::apply(m_Bind.value()); }

    EMP_RETURN bool post_load(void)
    { return call_post_load<T, has_post_load<T>::value>::apply(m_Bind.value()); }

    EMP_RETURN bool read_base(c_astring& EMP_SILENT(a_sDirectory)) { return true; }

    EMP_RETURN size_t field_count(void) const
    { return m_vpReflections.count(); }

    EMP_RETURN c_astring& field_name(c_size a_stIndex) const
    { return m_vsFields.ref(a_stIndex); }

    EMP_RETURN type_i* field_reflection(c_size a_stIndex) const
    { return m_vpReflections.ref(a_stIndex); }
    //<< base_i

    //>> class_i
    void set_value(T* const a_pValue) { m_Bind.set_value(a_pValue); }
    //<< class_i


    #ifdef EMP_XX_LOCALIZE
    void add(cpc_char a_szField, string_s const& a_rName, string_s const& a_rDescription, type_i* const a_pReflection)
    {
        add(a_szField, a_pReflection);
        m_vsNames.add(a_rName);
        m_vsDescriptions.add(a_rDescription);
    }
    #endif

    void add(cpc_char a_szField, type_i* const a_pReflection)
    {
        EMP_ASSERT(a_pReflection);
        m_vsFields.add(astring(a_szField));
        m_vpReflections.add(a_pReflection);
    }

    EMP_RETURN T const& value(void) const { return m_Bind.value(); }
    EMP_RETURN T& value(void) { return m_Bind.value(); }

private:
    template <typename I>
    EMP_RETURN typename enable_if<tt::or_<type_t<I>::interface, type_t<I>::pointer>::value, astring>::type
        to_string_impl(formater_i* const a_pFormater) const
    {
        if (m_Bind.has_value())
        { return to_string(m_Bind.value(), a_pFormater); }
        return "__null_pointer__";
    }

    template <typename I>
    EMP_RETURN typename enable_if_not<tt::or_<type_t<I>::interface, type_t<I>::pointer>::value, astring>::type
        to_string_impl(formater_i* const a_pFormater) const
    {
        if (m_Bind.has_value())
        { return to_string(m_Bind.value(), a_pFormater); }
        else if (m_Bind.gettable())
        {
            T oValue;
            EMP_ASSERT_BOOL_CALL(m_Bind.getter()(oValue));
            return to_string(oValue, a_pFormater);
        }
        return "__null_pointer__";
    }*/
//};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

template <bool t_bFalse>
struct call_post_load
{
    template <typename U>
    static EMP_RETURN bool apply(U&)
    { return true; }
};

template <>
struct call_post_load<true>
{
    template <typename U>
    static EMP_RETURN bool apply(U& a_rObject)
    { return a_rObject.post_load(); }
};

template <bool t_bFalse>
struct add_post_load
{
    template <typename U>
    static EMP_RETURN bool apply(U&)
    { return true; }
};

template <>
struct add_post_load<true>
{
    template <typename U>
    static EMP_RETURN bool apply(U& a_rObject)
    { return emp::rfx::store<U>()->add_post_load(&a_rObject); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, base_t)
    using type = emp::tt::void_t<T>;
};

template <typename ...T>
EMP_NOINSTANCE(struct, base_t<std::tuple<T...>>)
    using type = std::tuple<type_t<T>*...>;
};

ENABLE_IF_T(emp::tt::has_typedef_tbases<T>::value)
typename T::tbases decl_base(void);

ENABLE_IF_T(false == emp::tt::has_typedef_tbases<T>::value)
void decl_base(void);

template <typename T>
using bases = typename base_t<decltype(decl_base<T>())>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_NOCOPY(class, member_i)
public:
};
template <typename t_Class>
EMP_NOCOPY_BASE(class,member_t, public, member_i)
public:
    template <typename t_Type, t_Type t_Class::*t_Member>
    EMP_RETURN t_Type& get(t_Class& a_rObject) { return a_rObject.*t_Member; }

    template <typename t_Type, t_Type t_Class::*t_Member>
    void set(t_Class& a_rObject, t_Type& a_rtValue) { a_rObject.*t_Member = a_rtValue; }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*#define enum_name access_e
#define enum_values\
    enum_value( Bind      )\
    enum_value( Read      )\
    enum_value( ReadWrite )
#include "emp_tt_enum.def.h"*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
EMP_(struct, field_s)
    type_i* m_pType;
    astring m_sName;
    string_s m_sLabel;
    string_s m_sDescription;
    bool m_bRead = false;

    //member_i* m_pMember; @@0 ouat...
    //size_t m_stOffset; @@0 ouat...

public:
    field_s(void):
        m_pType(),
        m_sName(),
        m_sLabel(),
        m_sDescription(),
        m_bRead(false)
    {}

    field_s(type_i* const a_pType):
        m_pType(a_pType),
        m_sName(),
        m_sLabel(),
        m_sDescription(),
        m_bRead(false)
    {}

    field_s(
        type_i* const a_pType,
        c_astring& a_sName,
        string_s const& a_sLabel,
        string_s const& a_sDescription):
        m_pType(a_pType),
        m_sName(a_sName),
        m_sLabel(a_sLabel),
        m_sDescription(a_sDescription),
        m_bRead(false)
    {}

    ~field_s(void)
    {}

   /* field_s(field_s const& a_rFrom):
        m_pType(a_rFrom.m_pType),
        m_sName(a_rFrom.m_sName),
        m_sName(a_rFrom.m_sName),
        m_stOffset(a_rFrom.m_stOffset),
        m_bRead(a_rFrom.m_bRead)
    {}

    field_s& operator=(field_s const& a_rFrom)
    {
        m_pType = a_rFrom.m_pType;
        m_sMember = a_rFrom.m_sMember;
        m_sName = a_rFrom.m_sName;
        m_stOffset = a_rFrom.m_stOffset;
        return *this;
    }*/
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, class_i, public, type_i)
public:
    class_i(void) {}
    virtual ~class_i(void) = 0;

public:
    //>> base_i
    /*virtual EMP_RETURN c_astring& ctor_type(void) const = 0;

    virtual EMP_RETURN bool pre_load(void) = 0;
    virtual EMP_RETURN bool post_load(void) = 0;

    virtual EMP_RETURN bool read_base(c_astring& a_rsDirectory) = 0;

    virtual void reset_all_read(void) = 0;
    virtual EMP_RETURN bool all_read(void) const = 0;
    virtual void set_read(c_size a_stIndex, c_bool a_bValue) = 0;

    virtual EMP_RETURN size_t field_count(void) const = 0;
    virtual EMP_RETURN c_astring& field_name(c_size a_stIndex) const = 0;
    virtual EMP_RETURN type_i* field_reflection(c_size a_stIndex) const = 0;*/
    //<< base_i
};
inline class_i::~class_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, class_c, public, class_i)
private:
    template <typename U, size_t t_stCount> friend struct reflect_base_t;

private:
    vector<field_s> m_vFields;
    vector<field_s*> m_vpAllFields;
    map_t<astring, field_s*> m_mpFields;

public: // @@0 private
    vector<class_c*> m_vpBases;

public:
    class_c(void):
        m_vFields(),
        m_vpAllFields(),
        m_mpFields(8),
        m_vpBases()
    {}

    virtual~class_c(void)
    {
        c_size stCount = m_vFields.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_DEALLOCATE(m_vFields.ref(stIndex).m_pType); }
        m_vFields.resize(0);

        m_vpAllFields.resize(0);
        emp::cnt::foreach(m_vpBases, emp::mem::safe_delete_reset_t());
        m_vpBases.resize(0);
    }

public:
    // >> type_i
    EMP_RETURN size_t field_count(void) const { return m_mpFields.count(); }
    // << type_i

    void reset_all_read(void)
    {
        auto const& oItEnd = m_mpFields.end();
        for (auto oIt = m_mpFields.begin(); oIt != oItEnd; ++oIt)
        {
            field_s* pField = *oIt.value();
            pField->m_bRead = false;
        }
    }

    EMP_RETURN bool all_read(void) const
    {
        auto const& oItEnd = m_mpFields.end();
        for (auto oIt = m_mpFields.begin(); oIt != oItEnd; ++oIt)
        {
            field_s const* const pField = *oIt.value();
            EMP_RET_FALSE_IF_NOT(pField->m_bRead, "#0.#1 #2; not read", id()->m_sName, pField->m_pType->id()->m_sName, pField->m_sName);
        }
        return true;
    }

private:
    EMP_RETURN field_s* field_impl(c_astring& a_sName) const
    {
        auto const oIt = m_mpFields.get(a_sName);
        if (oIt != m_mpFields.end())
        { return *oIt.value(); }
        return nullptr;
    }

public:
    EMP_RETURN field_s const* field(c_astring& a_sName) const { return field_impl(a_sName); }
    EMP_RETURN field_s* field(c_astring& a_sName) { return field_impl(a_sName); }

    EMP_RETURN field_s const* field(c_size a_stIndex) const { return m_vFields.ptr(a_stIndex); }
    EMP_RETURN field_s* field(c_size a_stIndex) { return m_vFields.ptr(a_stIndex); }

    EMP_RETURN field_s const* all_field(c_size a_stIndex) const { return m_vpAllFields.val(a_stIndex); }
    EMP_RETURN field_s* all_field(c_size a_stIndex) { return m_vpAllFields.val(a_stIndex); }

    EMP_RETURN bool read_file(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    { return emp::rfx::read_file(a_rReader, a_stDepth, a_sDirectory, *this); }

    EMP_RETURN bool read_attribute(xml::reader_c& EMP_SILENT(a_rReader)) { EMP_ASSERT_UNREACHABLE(); return false; }

    EMP_RETURN bool read_base_ctor(c_astring& a_sDirectory)
    {
        c_size stCount = m_vpBases.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(emp::rfx::read_ctor(a_sDirectory, *m_vpBases.val(stIndex))); }
        return true;
    }

    EMP_RETURN bool iterate(c_size a_stDepth, reflect_f const& a_rCallback)
    {
        {
            c_size stCount = m_vpBases.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { EMP_RET_FALSE_IF_NOT(m_vpBases.val(stIndex)->iterate(a_stDepth + 1, a_rCallback)); }
        }

        {
            c_size stCount = m_vFields.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                field_s const& rField = m_vFields.ref(stIndex);
                type_i* const pType = rField.m_pType;
                EMP_RET_FALSE_IF_NOT(pType->bind());
                a_rCallback(this, a_stDepth + 1, rField);
            }
        }
        return true;
    }

protected:
    void add_field(field_s const& a_Field)
    { m_vFields.add(a_Field); }

    EMP_RETURN bool reflect_bases(void)
    {
        {
            c_size stCount = m_vFields.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                field_s* const pField = m_vFields.ptr(stIndex);
                astring const& sName = pField->m_sName;
                EMP_RET_FALSE_IF(m_mpFields.get(sName) != m_mpFields.end());
                m_mpFields.set(sName, pField);
                m_vpAllFields.add(pField);
            }
        }

        {
            c_size stCount = m_vpBases.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                auto const& oBaseFields = m_vpBases.val(stIndex)->m_mpFields;
                auto const& oItEnd = oBaseFields.end();
                for (auto oIt = oBaseFields.begin(); oIt != oItEnd; ++oIt)
                {
                    astring const& sName = *oIt.key();
                    EMP_RET_FALSE_IF(m_mpFields.get(sName) != m_mpFields.end());
                    m_mpFields.set(sName, *oIt.value());
                    m_vpAllFields.add(*oIt.value());
                }
            }
        }
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

