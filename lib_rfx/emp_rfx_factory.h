//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_REFLECTION_H
#define EMP_RFX_REFLECTION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_type_i.h"
#include "emp_rfx_class.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_map.h"
#include "emp_cnt_string.h"
#include "emp_cnt_vector.h"

#include "emp_fct_function.h"

#include "emp_loc_string.h"

#include "emp_tt_finally.h"
#include "emp_tt_class.h"

#include "emp_xml_reader.h"
#include "emp_xml_writer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RFX_TOKEN_V_STREAM_V1 ALITERAL("__object")
#define EMP_RFX_TOKEN_V_COUNT_V1 ALITERAL("__count")
#define EMP_RFX_TOKEN_V_NAME_V1 ALITERAL("__name")
#define EMP_RFX_TOKEN_V_TYPE_V1 ALITERAL("__type")
#define EMP_RFX_TOKEN_V_VALUE_V1 ALITERAL("__value")
#define EMP_RFX_TOKEN_V_POINTER_V1 ALITERAL("__pointer")

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::fct;
using namespace emp::loc;
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> using getter_t = emp::fct::function_t<bool(*)(T& a_rtValue)>;
template <typename T> using setter_t = emp::fct::function_t<bool(*)(T const& a_tValue)>;
template <typename T> using binder_t = emp::fct::function_t<bool(*)(T*& a_prtValue)>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool to_string(T const& a_tValue, formater_i* const a_pFormater, u8string& a_rsOut);

template <typename T>
EMP_INLINE EMP_RETURN bool to_string_formater_imp(T const& a_rValue, formater_i* const a_pFormater, u8string& a_rsOut)
{
    // @@used ? if (type_t<T>::value == a_pFormater->type())
    {
        formater_a<T>* const pFormater = static_cast<formater_a<T>*>(a_pFormater);
        pFormater->set_value(a_rValue);
        emp::tt::silent(STR_FORMATU8(a_rsOut, "#0", *a_pFormater));
        return true;
    }
    /*else @@implement
    {
        EMP_LOG_ERROR("Invalid format type #0 : #1", emp::ast::cast<ui32>(a_pFormater->type()), emp::rfx::type_t<T>::name());
        a_rsOut = ALITERAL("__format_error__");
        return false;
    }*/
}

template <typename T>
EMP_INLINE EMP_RETURN bool to_string_impl(T const& a_rValue, formater_i* const a_pFormater, u8string& a_rsOut)
{
    if (a_pFormater)
    { return to_string_formater_imp(a_rValue, a_pFormater, a_rsOut); }
    emp::str::from(a_rsOut, a_rValue);
    return true;
}

EMP_INLINE EMP_RETURN bool to_string_impl(c_float a_fValue, formater_i* const a_pFormater, u8string& a_rsOut, c_size a_stDigits = 7);
EMP_INLINE EMP_RETURN bool to_string_impl(c_float a_fValue, formater_i* const a_pFormater, u8string& a_rsOut, c_size a_stDigits)
{
    if (a_pFormater)
    { return to_string_formater_imp(a_fValue, a_pFormater, a_rsOut); }
    formater_t<real_format_s, float> oFormater(real_format_s().round(true).precision(a_stDigits), a_fValue);
    emp::tt::silent(STR_FORMATU8(a_rsOut, "#0", oFormater));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
template <typename T>
EMP_(struct, link_t)
private:
    T* m_pValue;
    getter_t<T> m_Getter;
    setter_t<T> m_Setter;
    binder_t<T> m_Binder;

public:
    link_t(void):
        m_pValue(nullptr), m_Getter(), m_Setter(), m_Binder() {}

    link_t(binder_t<T> const& a_rBinder):
        m_pValue(nullptr), m_Getter(), m_Setter(), m_Binder(a_rBinder) {}

    link_t(getter_t<T> const& a_rGetter):
        m_pValue(nullptr), m_Getter(a_rGetter), m_Setter(), m_Binder() {}

    link_t(getter_t<T> const& a_rGetter, setter_t<T> const& a_rSetter):
        m_pValue(nullptr), m_Getter(a_rGetter), m_Setter(a_rSetter), m_Binder() {}

public:
    EMP_RETURN getter_t<T> const& getter(void) const { return m_Getter; }
    EMP_RETURN setter_t<T> const& setter(void) const { return m_Setter; }
    EMP_RETURN binder_t<T> const& binder(void) const { return m_Binder; }

    EMP_RETURN bool has_value(void) const { return m_pValue != nullptr; }
    EMP_RETURN T const& value(void) const { return *m_pValue; }
    EMP_RETURN T      & value(void)       { return *m_pValue; }

    void set_value(T* const a_pValue) { m_pValue = a_pValue; }

    EMP_RETURN bool from_string(c_astring& a_sValue)
    { return from_string_impl<T>(a_sValue); }
    EMP_RETURN bool from_string(c_u8string& a_sValue)
    { return from_string_impl<T>(a_sValue); }

public:
    EMP_RETURN bool bind(void)
    {
        if (m_Binder.is_valid() == false)
        { return true; }
        else
        {
            T* ptValue = nullptr;
            EMP_RET_FALSE_IF_NOT((m_Binder)(ptValue));
            EMP_RET_FALSE_IF(ptValue == nullptr);

            m_pValue = ptValue;
            return true;
        }
    }

private:
    template <typename U, typename S, ENABLE_IF(std::is_abstract<U>::value == false)>
    EMP_RETURN bool from_string_impl(S const& a_sValue)
    {
        if (m_pValue != nullptr)
        { return to(a_sValue, *m_pValue); }
        else if (m_Setter.is_valid())
        {
            T oValue;
            if (emp::str::to(a_sValue, oValue) == false)
            { return false; }
            return m_Setter(oValue);
        }
        // @@0 factory EMP_LOG_ERROR("type_t<#0> is readonly.", EMP_FACTORY.get_type<T>()->id().m_sName, a_sValue);
        EMP_ASSERT_UNREACHABLE();
        return false;
    }

    template <typename U, typename S, ENABLE_IF(std::is_abstract<U>::value)>
    EMP_RETURN bool from_string_impl(S const& a_sValue)
    {
        if (m_pValue != nullptr)
        { return to(a_sValue, *m_pValue); }
        // @@0 factory EMP_LOG_ERROR("type_t<#0> is readonly.", EMP_FACTORY.get_type<T>()->id().m_sName, a_sValue);
        EMP_ASSERT_UNREACHABLE();
        return false;
    }

    // >> link_i
    // @@0 remove EMP_RETURN void* object(void) { return m_pValue; }
    // << link_i*/
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
class type_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_HAS_FUNCTION(destroy)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, store_t, public, store_i)
private:
    friend class factory_c;

private:
    map_t<astring, size_t> m_mObjectFromName;
    map_t<void const*, size_t> m_mObjectFromAddress;
    emp::cnt::vector<object_s*> m_vpObjects;
    emp::cnt::vector<object_t<T>> m_vObjects;
    map_t<T*, bool> m_mpPostLoad;

public:
    store_t(void) :
        m_mObjectFromName(64),
        m_mObjectFromAddress(64),
        m_vpObjects(),
        m_vObjects(),
        m_mpPostLoad(8)
    {}

    ~store_t(void)
    { destroy(); }

public:
    // >> store_i
    EMP_RETURN type_s const* id(void) const { return emp::rfx::id<T>(); }
    EMP_RETURN bool is_child_of(type_s const* const a_pBase) const { return is_child_of_impl<T>(a_pBase); }
    EMP_RETURN void* read_void(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, c_astring& a_sVariable)
    { return read_object(a_rReader, a_stDepth, a_sDirectory, a_sVariable); }
    // << store_i

    EMP_RETURN bool write(c_astring& a_sPath, c_bool a_bAll)
    {
        if (id()->m_bInterface)
        { return true; }

        if (a_sPath.len() == 0)
        { return true; }

        bool bChanged = a_bAll;
        if (bChanged == false)
        {
            for (auto it = m_mObjectFromName.begin(); it.not_end(); ++it)
            {
                c_size stIndex = *it.value();
                object_t<T>& rObject = m_vObjects.ref(stIndex);
                if (rObject.m_pInterface->changed && rObject.m_pInterface->file)
                { bChanged = true; break; }
            }
        }

        if (bChanged)
        {
            EMP_LOG_INFO("Writing store : #0", id()->m_sSerializableName);

            for (auto it = m_mObjectFromName.begin(); it.not_end(); ++it)
            {
                c_size stIndex = *it.value();

                object_t<T>& rObject = m_vObjects.ref(stIndex);
                if (rObject.m_pInterface->written == false && (rObject.m_pInterface->changed || a_bAll) && rObject.m_pInterface->file)
                {
                    rObject.m_pInterface->changed = false;
                    rObject.m_pInterface->written = true;
                    c_astring& sName = *it.key();
                    EMP_LOG_INFO("Writing object #0, #1", id()->m_sSerializableName, sName);
                    EMP_RET_FALSE_IF_NOT(write(a_sPath, sName, *rObject.pointer), "Failed to write #0 : #1", id()->m_sSerializableName, sName);
                    rObject.m_pInterface->changed = false;
                }
            }
        }

        return true;
    }

    EMP_RETURN bool write_declaration(xml::writer_c& a_rWriter, object_s* const a_pObject)
    {
        EMP_RET_FALSE_IF(a_pObject == nullptr);
        if (a_pObject->declared == false && a_pObject->variable)
        { EMP_RET_FALSE_IF_NOT(emp::rfx::write_name(a_rWriter, a_pObject->name)); }
        a_pObject->declared = true;
        return true;
    }

    EMP_RETURN bool write_void(xml::writer_c& a_rWriter, void* a_pObject)
    {
        object_t<T>* pObject = get_object_info_t(a_pObject);
        EMP_RET_FALSE_IF(pObject == nullptr);
        EMP_RET_FALSE_IF_NOT(emp::rfx::write_type(a_rWriter, emp::rfx::id<T*>()->m_sSerializableName));
        EMP_RET_FALSE_IF_NOT(write_object(a_rWriter, *pObject->pointer));
        return true;
    }

    EMP_RETURN bool write_object(xml::writer_c& a_rWriter, T& a_rObject)
    {
        type_t<T> oType;
        EMP_RET_FALSE_IF_NOT(emp::rfx::reflect(oType), "Failed to reflect type_t<#0 >", id()->m_sName);
        EMP_RET_FALSE_IF_NOT(write_object(a_rWriter, a_rObject, oType));
        return true;
    }

    EMP_RETURN bool write_object(xml::writer_c& a_rWriter, T& a_rObject, type_t<T>& a_rType)
    {
        a_rType.set_value(&a_rObject);
        EMP_RET_FALSE_IF_NOT(write_declaration(a_rWriter, get_object_info_s(&a_rObject)));
        EMP_RET_FALSE_IF_NOT(a_rType.write(a_rWriter));
        return true;
    }

    EMP_RETURN bool write(c_astring& a_sPath, c_astring& a_sFile, T& a_rObject)
    {
        astring sFilename = STR_AFORMAT("#0#1", a_sPath, a_sFile);    // @@2 alloc
        EMP_LOG_INFO("Writing : #0", sFilename);

        emp::xml::writer_c oWriter;
        EMP_RET_FALSE_IF_NOT(oWriter.create(sFilename));
        EMP_RET_FALSE_IF_NOT(oWriter.start_document());
        EMP_RET_FALSE_IF_NOT(oWriter.start_element(EMP_RFX_TOKEN_V_STREAM_V1));
        EMP_RET_FALSE_IF_NOT(emp::rfx::write_type(oWriter, id()->m_sSerializableName));
        EMP_RET_FALSE_IF_NOT(write_object(oWriter, a_rObject));
        EMP_RET_FALSE_IF_NOT(oWriter.end_element());
        EMP_RET_FALSE_IF_NOT(oWriter.end_document());
        return true;
    }

private:
    ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    static EMP_RETURN bool is_child_of_impl(type_s const* const a_pBase)
    { return is_child_of_t<U, std::tuple_size<typename U::tbases>::value>::apply(a_pBase); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    static EMP_RETURN bool is_child_of_impl(type_s const* const)
    { return false; }

private:
    template <typename U, size_t t_stCount> friend struct is_child_of_t;

    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, is_child_of_t)
        static EMP_RETURN bool apply(type_s const* const a_pBase)
        {
            using tbase = typename std::tuple_element<t_stCount - 1, typename U::tbases>::type;
            if (emp::rfx::id<tbase>() == a_pBase)
            { return true; }
            store_t<tbase>* const pStore = emp::rfx::store<tbase>();
            if (pStore->is_child_of(a_pBase))
            { return true; }
            return is_child_of_t<U, t_stCount - 1>::apply(a_pBase);
        }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (is_child_of_t<U, 0>))
        static EMP_RETURN bool apply(type_s const* const)
        { return false; }
    };

    EMP_RETURN bool instanciate(vector<astring>& a_rvsFiles, vector<astring>& a_rvsNames)
    {
        EMP_RET_FALSE_IF(m_mObjectFromAddress.count() > 0);
        EMP_RET_FALSE_IF(m_mObjectFromName.count() > 0);

        {
            type_t<T> oType;
            EMP_LOG_INFO("Instancing : store_t<#0>", id()->m_sName);
            EMP_RET_FALSE_IF_NOT(emp::rfx::reflect(oType), "Failed to reflect type_t<#0 >", id()->m_sName);
        }

        for (size_t stIndex = 0; stIndex < a_rvsFiles.count();)
        {
            c_astring sFilename = a_rvsFiles.ref(stIndex);

            xml::reader_c oReader;

            EMP_RET_FALSE_IF_NOT(oReader.create(sFilename));
            EMP_RET_FALSE_IF_NOT(oReader.read_element(EMP_RFX_TOKEN_V_STREAM_V1), "Failed to instanciate '#0'", sFilename);

            if (read_type(oReader, id()->m_sSerializableName, false) == false)
            {
                ++stIndex;
                continue;
            }

            c_astring sName = a_rvsNames.ref(stIndex);
            EMP_RET_FALSE_IF(sName.is_empty(), "Invalid name while instancing '#0'", sFilename);

            a_rvsFiles.remove_unordered(stIndex);
            a_rvsNames.remove_unordered(stIndex);

            if (sName.get_first_char() == '_')
            { continue; }

            EMP_LOG_INFO("Instancing : #0 : #1", id()->m_sName, sName);
            T* const pObject = new_object(sName);
            EMP_RET_FALSE_IF(pObject == nullptr, "Failed to instanciate object : #0 : #1", id()->m_sName, sFilename);

            //astring sFile;
            //extract_F(sFilename, sFile);
        }

        return a_rvsFiles.count() == a_rvsNames.count();
    }


    EMP_RETURN bool read(c_astring& a_sPath)
    {
        EMP_LOG_DEBUG("Reading store : #0", id()->m_sSerializableName);
        auto const& oItEnd = m_mObjectFromName.end();
        for (auto oIt = m_mObjectFromName.begin(); oIt != oItEnd; ++oIt)
        {
            c_astring& sName = *oIt.key();
            object_t<T> const& oObject = m_vObjects.ref(*oIt.value());
            if (oObject.m_pInterface->file &&
                oObject.m_pInterface->concrete == this)
            {
                EMP_RET_FALSE_IF_NOT(read(a_sPath, sName, *oObject.pointer),
                    "Failed to read #0 : #1", id()->m_sSerializableName, sName);
            }
        }

        return true;
    }

public:
    EMP_RETURN object_s* get_object_info_s(T const* a_pObject)
    {
        if (a_pObject == nullptr)
        { return nullptr; }
        auto const it = m_mObjectFromAddress.get(a_pObject);
        if (it.end())
        { return nullptr; }
        return m_vObjects.ref(*it.value()).m_pInterface;
    }

    EMP_RETURN object_t<T>* get_object_info_t(void const* a_pObject)
    {
        if (a_pObject == nullptr)
        { return nullptr; }
        auto const it = m_mObjectFromAddress.get(a_pObject);
        if (it.end())
        { return nullptr;}
        return &m_vObjects.ref(*it.value());
    }

public:
    EMP_RETURN object_t<T>* get_object_info_t(T const* a_pObject)
    {
        if (a_pObject == nullptr)
        { return nullptr; }
        auto const it = m_mObjectFromAddress.get(a_pObject);
        if (it.end())
        { return nullptr;}
        return &m_vObjects.ref(*it.value());
    }

private:
    EMP_RETURN bool read(c_astring& a_sPath, c_astring& a_sFile, T& a_rObject)
    { return read_impl<T>(a_sPath, a_sFile, a_rObject); }

    ENABLE_IF_(U, std::is_abstract<U>::value)
    EMP_RETURN bool read_impl(c_astring&, c_astring&, U&)
    { return true; }

    ENABLE_IF_(U, std::is_abstract<U>::value == false)
    EMP_RETURN bool read_impl(c_astring& a_sPath, c_astring& a_sFile, U& a_rObject)
    {
        type_t<T> oType;
        EMP_RET_FALSE_IF_NOT(emp::rfx::reflect(oType), "Failed to reflect type_t<#0 >", id()->m_sName);

        astring sFilename = STR_AFORMAT("#0#1", a_sPath, a_sFile);
        EMP_LOG_INFO("Reading : #0", sFilename);
        xml::reader_c oReader;
        EMP_RET_FALSE_IF_NOT(oReader.create(sFilename));
        EMP_RET_FALSE_IF_NOT(oReader.read_element(EMP_RFX_TOKEN_V_STREAM_V1));

        oType.set_value(&a_rObject);
        EMP_RET_FALSE_IF_NOT(oType.read_file(oReader, 1, a_sPath));
        // @@0 remove EMP_RET_FALSE_IF_NOT(oType.post_load());
        return true;
    }



    //EMP_RETURN size_t field_count(void) const;
    //EMP_RETURN bool from_string(c_astring& a_sValue);
    //EMP_RETURN astring to_string(str::formater_i* const a_pFormater) const;
    //EMP_RETURN bool settable(void) const;
    //EMP_RETURN bool bindable(void) const;
    //EMP_RETURN bool iterate(c_size a_stDepth, reflect_f const& a_rCallback);

public:

    EMP_RETURN bool post_load(void)
    {
        type_t<T> oType;
        EMP_RET_FALSE_IF_NOT(emp::rfx::reflect(oType), "Failed to reflect type_t<#0 >", id()->m_sName);
        EMP_LOG_INFO("Post loading store_t<#0>", id()->m_sName);

        {
            c_size stCount = m_vObjects.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                auto const& rObject = m_vObjects.val(stIndex);
                oType.set_value(rObject.pointer);
                EMP_RET_FALSE_IF_NOT(oType.post_load(), "Failed to postload #0 : #1", id()->m_sName, rObject.m_pInterface->name);
            }
        }
        {
            auto const& itEnd = m_mpPostLoad.end();
            for (auto it = m_mpPostLoad.begin(); it != itEnd; ++it)
            {
                T* const pObject = *it.key();
                oType.set_value(pObject);
                EMP_RET_FALSE_IF_NOT(oType.post_load(), "Failed to postload #0", id()->m_sName);
            }
            m_mpPostLoad.clear(); // @@0 deallocate
        }
       return true;
    }

    EMP_RETURN bool add_post_load(T* const a_pValue)
    {
        if (m_mObjectFromAddress.get(a_pValue) == m_mObjectFromAddress.end() &&
            m_mpPostLoad.get(a_pValue) == m_mpPostLoad.end()) // @@0 should return false but, registered twice in read_object and read_ctor
        {
            m_mpPostLoad.set(a_pValue, true);
        }
        return true;
    }

    void touch(void)
    {
        for (auto it = m_mObjectFromAddress.begin(); it.not_end(); ++it)
        { touch(m_vpObjects.ref(*it.value())); }
    }

    void touch(T const* const a_pObject)
    { touch(m_vpObjects.ref(*m_mObjectFromAddress.get(a_pObject).value())); }

    void touch(object_s* const a_pInterface)
    {
        a_pInterface->changed = true;
        a_pInterface->written = false;
    }

    EMP_RETURN size_t object_count(void) const { return m_mObjectFromAddress.count(); }

    EMP_RETURN bool write_attribute(c_astring& EMP_SILENT(a_sName), xml::writer_c& EMP_SILENT(a_rWriter))
    { return false; }
    /*EMP_RETURN size_t recursive_object_count(void) const
    {
        size_t stObjectCount = object_count();
        c_size stCount = m_vpChildren.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { stObjectCount += m_vpChildren.val(stIndex)->recursive_object_count(); }
        return stObjectCount;
    }*/


    /*EMP_RETURN T* recursive_get_object(c_size a_stIndex)
    {
        size_t stObjectCount = m_vpObjects.count();
        size_t stObject = a_stIndex;
        if (a_stIndex < stObjectCount)
        { return m_vpObjects.val(a_stIndex); }
        stObject -= stObjectCount;
        c_size stCount = m_vpChildren.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            store_i* const pStore = m_vpChildren.val(stIndex);
            c_size stStoreCount = pStore->recursive_object_count();
            stObjectCount += stStoreCount;
            if (a_stIndex < stObjectCount)
            { return pStore->recursive_get_object(stObject); }
            stObject -= stStoreCount;
        }
        return nullptr;
    }*/

public:
    void destroy(void)
    {
        /*{
            c_size stCount = m_vObjects.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                object_t<T>& rObject = m_vObjects.ref(stIndex);
                if (rObject.m_pInterface->concrete != nullptr)
                {
                    rObject.m_pInterface->concrete->destroy_concrete(rObject.m_pPointer);
                    rObject = object_t<T>();
                }
            }
        }*/

        {
            c_size stCount = m_vpObjects.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                object_s* const pObject = m_vpObjects.val(stIndex);
                object_t<T>* const pConcrete = get_object_info_t(pObject->pointer);
                destroy_pointer(pConcrete->pointer);
                EMP_DEALLOCATE(pObject);
            }
        }

        m_mObjectFromName.clear();
        m_mObjectFromAddress.clear();
        m_vObjects.resize(0);
        m_vpObjects.resize(0);
        m_mpPostLoad.clear();
    }


    /*void destroy_concrete(void* a_pObject)
    {
        object_t<T>* pObject = get_object_info_t(a_pObject);
        if (pObject != nullptr)
        {
            destroy_object_base<T>(*pObject);
            *pObject = object_t<T>();
        }
    }*/

    ENABLE_IF_(U, has_function_destroy<U, void(void)>::value)
    static void destroy_pointer_impl(T* const a_pObject)
    { a_pObject->destroy(); }

    ENABLE_IF_(U, false == has_function_destroy<U, void(void)>::value)
    static void destroy_pointer_impl(T* const EMP_SILENT(a_rObject))
    { }

    static void destroy_pointer(T*& a_rpObject)
    {
        if (a_rpObject != nullptr)
        {
            destroy_pointer_impl<T>(a_rpObject);
            EMP_DEALLOCATE(a_rpObject);
            a_rpObject = nullptr;
        }
    }


    /*ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    void destroy_object_base(object_t<U>& a_rObject)
    { destroy_object_base_t<U, std::tuple_size<typename U::tbases>::value>::apply(a_rObject); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    void destroy_object_base(object_t<U>& a_rObject)
    { destroy_pointer(a_rObject.m_pPointer); }*/

private:
    /*template <typename U, size_t t_stCount> friend struct destroy_object_base_t;

    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, destroy_object_base_t)
        static void apply(object_t<U>& a_rObject)
        {
            using tbase = typename std::tuple_element<t_stCount - 1, typename U::tbases>::type;
            store_t<tbase>* const pStore = emp::rfx::store<tbase>();
            object_t<tbase>* pObject = pStore->get_object_info_t(a_rObject.m_pPointer);
            if (pObject != nullptr)
            {
                if (t_stCount == 1 &&
                    a_rObject.m_pPointer != nullptr)
                { destroy_pointer(a_rObject.m_pPointer); }
                *pObject = object_t<tbase>();
            }
            destroy_object_base_t<U, t_stCount - 1>::apply(a_rObject);
        }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (destroy_object_base_t<U, 0>))
        static void apply(object_t<T>&)
        {}
    };*/

public:
    EMP_RETURN T const* get_object(c_astring& a_sName) const { return get_object_impl(a_sName); }
    EMP_RETURN T      * get_object(c_astring& a_sName)       { return get_object_impl(a_sName); }

    EMP_RETURN T const* get_object(c_size a_stIndex) const { return get_object_impl(a_stIndex); }
    EMP_RETURN T      * get_object(c_size a_stIndex)       { return get_object_impl(a_stIndex); }

    EMP_RETURN

    EMP_RETURN T* get_or_new_object(c_astring& a_sFile)
    {
        T* pObject = get_object(a_sFile);
        if (pObject != nullptr)
        { return pObject; }
        return new_object(a_sFile);
    }

private:
    //EMP_RETURN T* new_object(void)
    //{ return new_object_impl<T>(); }

public:
    EMP_RETURN T* new_object(c_astring& a_sFile)
    { return new_object_impl<T>(a_sFile); }

    EMP_RETURN T* new_object_pattern(c_astring& a_sPattern)
    { return new_object_pattern_impl<T>(a_sPattern); }

    //void release_object(T*& a_rpObject)
    //{ release_object_impl<T>(a_rpObject); }

    //void destroy_object(T*& a_rpObject)
    //{ destroy_object_impl<T>(a_rpObject); }

private:
    /*ENABLE_IF_(U, std::is_abstract<U>::value)
    EMP_RETURN U* new_object_impl(void)
    { return nullptr; }

    ENABLE_IF_(U, std::is_abstract<U>::value == false)
    EMP_RETURN U* new_object_impl(void)
    { return EMP_ALLOCATE(T); }*/

    ENABLE_IF_(U, std::is_abstract<U>::value)
    EMP_RETURN U* new_object_impl(c_astring&)
    { return nullptr; }

    ENABLE_IF_(U, std::is_abstract<U>::value == false)
    EMP_RETURN U* new_object_impl(c_astring& a_sFile)
    {
        c_bool bEmpty = (a_sFile.len() > 0) == false;
        astring sFile(bEmpty ? emp::str::afrom(emp::rfx::next_object_id()) : a_sFile);
        EMP_RET_NULL_IF(sFile.is_empty());

        astring sExtension;
        emp::str::extract_E(sFile, sExtension);
        c_bool bFile = emp::str::cmp(astring(ALITERAL("xml")), sExtension);
        c_bool bVariable = (bFile == false) && (bEmpty == false);

        U* const pPointer = EMP_ALLOCATE(T);
        object_s* const pObject = EMP_ALLOCATE(object_s, sFile, this, pPointer, bFile, bVariable);

        auto oFinally(emp::tt::finally([pPointer, pObject]{ EMP_DEALLOCATE(pPointer); EMP_DEALLOCATE(pObject); }));

        EMP_RET_NULL_IF(object_exists(sFile), "Duplicate object name '#0'", a_sFile);
        EMP_RET_NULL_IF(object_exists_base<U>(sFile), "Duplicate object name '#0'", a_sFile);

        EMP_RET_NULL_IF_NOT(add_object(sFile, pPointer, pObject));
        EMP_RET_NULL_IF_NOT(add_object_base<U>(sFile, pPointer, pObject));

        oFinally.release();

        m_vpObjects.add(pObject);
        return pPointer;
    }

public: //@@0 private
    EMP_RETURN bool object_exists(c_astring& a_sFile)
    { return m_mObjectFromName.get(a_sFile).not_end(); }

    EMP_RETURN bool add_object(c_astring& a_sFile, T* const a_pPointer, object_s* const a_pObject)
    {
        EMP_RET_FALSE_IF(a_pPointer == nullptr || a_pObject == nullptr);

        object_t<T> const oObject(this, a_pPointer, a_pObject);
        c_size stIndex = m_vObjects.count();
        m_vObjects.add(oObject);
        m_mObjectFromName.set(a_sFile, stIndex);
        m_mObjectFromAddress.set(a_pPointer, stIndex);

        return true;
    }

    // object_exists_base
    ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool object_exists_base(c_astring& a_sFile)
    { return object_exists_base_t<U, std::tuple_size<typename U::tbases>::value>::apply(a_sFile); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool object_exists_base(c_astring&)
    { return false;}

    template <typename U, size_t t_stCount> friend struct object_exists_base_t;

    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, object_exists_base_t)
        static EMP_RETURN bool apply(c_astring& a_sFile)
        {
            using tbase = typename std::tuple_element<t_stCount - 1, typename U::tbases>::type;
            store_t<tbase>* const pStore = emp::rfx::store<tbase>();
            if (pStore->object_exists(a_sFile))
            { return true; }
            if (pStore->template object_exists_base<tbase>(a_sFile))
            { return true; }
            return object_exists_base_t<U, t_stCount - 1>::apply(a_sFile);
        }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (object_exists_base_t<U, 0>))
        static EMP_RETURN bool apply(c_astring&)
        { return false; }
    };

    // add_object_base
    ENABLE_IF_(U, tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool add_object_base(c_astring& a_sFile, U* const a_pPointer, object_s* const a_pObject)
    { return add_object_base_t<U, std::tuple_size<typename U::tbases>::value>::apply(a_sFile, a_pPointer, a_pObject); }

    ENABLE_IF_(U, false == tt::has_typedef_tbases<U>::value)
    EMP_RETURN bool add_object_base(c_astring&, U* const, object_s* const)
    { return true;}

    template <typename U, size_t t_stCount> friend struct add_object_base_t;

    template <typename U, size_t t_stCount>
    EMP_NOINSTANCE(struct, add_object_base_t)
        static EMP_RETURN bool apply(c_astring& a_sFile, U* const a_pPointer, object_s* const a_pObject)
        {
            using tbase = typename std::tuple_element<t_stCount - 1, typename U::tbases>::type;
            store_t<tbase>* const pStore = emp::rfx::store<tbase>();
            EMP_RET_FALSE_IF_NOT(pStore->add_object(a_sFile, a_pPointer, a_pObject));
            EMP_RET_FALSE_IF_NOT(pStore->template add_object_base<tbase>(a_sFile, a_pPointer, a_pObject));
            return add_object_base_t<U, t_stCount - 1>::apply(a_sFile, a_pPointer, a_pObject);
        }
    };

    template <typename U>
    EMP_NOINSTANCE(struct, (add_object_base_t<U, 0>))
        static EMP_RETURN bool apply(c_astring&, U* const, object_s* const)
        { return true; }
    };

public:
    ENABLE_IF_(U, std::is_abstract<U>::value)
    EMP_RETURN U* new_object_pattern_impl(c_astring&)
    { return nullptr; }

    ENABLE_IF_(U, std::is_abstract<U>::value == false)
    EMP_RETURN U* new_object_pattern_impl(c_astring& a_sPattern)
    {
        size_t stName = 0;
        astring sName;
        while (true)
        {
            sName.clear();
            emp::tt::silent(STR_FORMATA(sName, "#0_#1.xml", a_sPattern, stName));
            if (get_object(sName) == nullptr)
            { break; }
            ++stName;
        }
        return new_object(sName);
    }

    /*void release(T* const a_pObject)
    {
        object_s const oObject = recursive_get_object(a_pObject);
        if (oObject.store == nullptr)
        { EMP_DEALLOCATE(a_pObject); }
    }

    ENABLE_IF_(U, has_function_destroy<U, void(void)>::value)
    void release_object_impl(T*& a_rpObject)
    {
        if (a_rpObject != nullptr)
        {
            a_rpObject->destroy();
            release(a_rpObject);
            a_rpObject = nullptr;
        }
    }

    ENABLE_IF_(U, false == has_function_destroy<U, void(void)>::value)
    void release_object_impl(T*& a_rpObject)
    {
        if (a_rpObject != nullptr)
        {
            release(a_rpObject);
            a_rpObject = nullptr;
        }
    }*/

public:
    void set_value(T* const a_pValue);// { m_Link.set_value(a_pValue); }

private:
    EMP_RETURN T* get_object_impl(c_astring& a_sName)
    {
        auto const oIt = m_mObjectFromName.get(a_sName);
        if (oIt != m_mObjectFromName.end())
        { return get_object_impl(*oIt.value()); }
        return nullptr;
    }

    EMP_RETURN T* get_object_impl(c_size a_stIndex)
    { return m_vObjects.ref(a_stIndex).pointer; }

public:
    /* //@@0 remove
    EMP_RETURN T* get_object_in_hierarchy(c_astring& a_sName)
    {
        T* const pObject = get_object(a_sName);
        EMP_RET_NULL_IF(pObject == nullptr, "Pointer not found #0* #1;", id()->m_sName, a_sName);
        return pObject;
    }*/

    /* //@@0 remove
    EMP_RETURN bool save_variable(c_astring& a_sName, T* const a_pAddress)
    {
        m_mpObjectNames.set(a_pAddress, a_sName);
        m_mpObjectAddresses.set(a_sName, a_pAddress);
        return true;
    }*/

public:
    EMP_RETURN T* read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, c_astring& a_sType,
        c_astring& a_sVariable)
    {
        if (cmp(id()->m_sSerializableName, a_sType))
        { return read_object(a_rReader, a_stDepth, a_sDirectory, a_sVariable); }

        store_i* const pStore = emp::rfx::store(a_sType);
        EMP_RET_NULL_IF(pStore == nullptr);
        EMP_RET_NULL_IF_NOT(pStore->is_child_of(emp::rfx::id<T>()));

        void* const pVoid = pStore->read_void(a_rReader, a_stDepth, a_sDirectory, a_sVariable);
        EMP_RET_NULL_IF(pVoid == nullptr);

        return static_cast<T*>(pVoid);
    }

    EMP_RETURN T* read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, c_astring& a_sVariable)
    {
        type_t<T> oType;
        EMP_RET_NULL_IF_NOT(emp::rfx::reflect(oType), "Failed to reflect type_t<#0 >", oType.id()->m_sName);

        T* pObject = new_object(a_sVariable);
        //EMP_RET_NULL_IF(pObject == nullptr, "Failed to allocate object #0", oType.id()->m_sName);

        // @@0 remove
        //if (a_sVariable.len() > 0)
        //{ EMP_RET_NULL_IF_NOT(save_variable(a_sVariable, pObject) == false, "Failed to save variable : #0", a_sVariable); }

        if (pObject != nullptr)
        {
            oType.set_value(pObject);

            EMP_RET_NULL_IF_NOT(oType.read_object(a_rReader, a_stDepth, a_sDirectory),
                "Failed to read object #0", oType.id()->m_sName);
            // @@0 remove EMP_RET_NULL_IF_NOT(oType.post_load(), "Failed to post load #0", oType.id()->m_sName);
        }
        return pObject;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
EMP_NOCOPY_BASE(class,store_t, public, store_i)
public:
    template <typename U>
    EMP_RETURN bool register_child(void);

    void destroy(void)
    {
        EMP_LOG_INFO("Destroying store_t<#0 >", this->s_type());

        if (m_bInterface == false)
        {
            {
                c_size stCount = m_vpObjects.count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                { private_destroy_object<T>(m_vpObjects.ref(stIndex)); }
            }
            {
                c_size stCount = m_vpPointers.count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                { private_release_object<T>(m_vpPointers.ref(stIndex)); }
            }
        }

        m_vsNames.resize(0);
        m_vpObjects.resize(0);
        m_vpPointers.resize(0);
        m_vpChildren.resize(0);
        m_vbChanged.resize(0);
        m_mpObjectAddresses.clear();
        m_mpObjectNames.clear();
        m_bLoaded = false;
    }

public:
    // @@0 in factory EMP_RETURN bool pre_load(void)
    //{
    //    if (m_bLoaded)
    //    { return true; }
    //    if (m_bInterface)
    //    { return true; }
    //    type_t<T> oClass(nullptr, nullptr, nullptr);
    //    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.reflect_object(&oClass), "Failed to reflect type_t<#0 >", this->s_type())
    //
    //    EMP_LOG_INFO("Pre loading store_t<#0>", this->s_type());
    //    c_size stCount = m_vpObjects.count();
    //    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    //    {
    //          T* const pObject =  m_vpObjects.val(stIndex);
    //          oClass.set_value(pObject);
    //          EMP_RET_FALSE_IF_NOT(oClass.pre_load(), "Failed to preload #0 : #1", this->s_type(), m_vsNames.ref(stIndex))
    //    }
    //   return true;
    //}

public:
    EMP_RETURN void* get_all_object(c_size a_stIndex)
    {
        size_t stObjectCount = m_vpObjects.count();
        size_t stObject = a_stIndex;
        if (a_stIndex < stObjectCount)
        { return m_vpObjects.val(a_stIndex); }
        stObject -= stObjectCount;
        c_size stCount = m_vpChildren.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            store_i* const pStore = m_vpChildren.val(stIndex);
            c_size stStoreCount = pStore->get_all_object_count();
            stObjectCount += stStoreCount;
            if (a_stIndex < stObjectCount)
            { return pStore->get_all_object(stObject); }
            stObject -= stStoreCount;
        }
        return nullptr;
    }

    void set_changed(void const* const a_pValue)
    {
        EMP_ASSERT(a_pValue);
        c_size stCount = m_vpObjects.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            T const* const pObject = m_vpObjects.val(stIndex);
            EMP_ASSERT(pObject);
            if (pObject == a_pValue)
            { m_vbChanged.set(stIndex, true); }
        }
    }

    EMP_RETURN T* get_variable(c_astring& a_sName) const
    {
        typename map_t<astring, T*>::iterator_c
        const oIt = m_mpObjectAddresses.get(a_sName);
        if (oIt != m_mpObjectAddresses.end())
        { return *oIt.value(); }
        return nullptr;
    }

public:
    EMP_RETURN bool interface(void) const { return m_bInterface; }

public:
    EMP_RETURN T* read(c_astring& a_sPath, c_astring& a_sFile)
    {
        if (m_bInterface)
        { return nullptr; }

        T* pObject = new_object(a_sFile);
        EMP_RET_NULL_IF_NOT(read(a_sPath, a_sFile, *get_object(a_sFile)), "Failed to read object : #0#1", a_sPath, a_sFile)
        return pObject;
    }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, factory_c)
private:
    static factory_c* s_pInstance;
public:
    #define EMP_FACTORY emp::rfx::factory_c::instance()
    #define EMP_STORE(...) emp::rfx::store<__VA_ARGS__>()
    static void set_instance(factory_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN factory_c& instance(void) { return *s_pInstance; }

public:
    template <typename T>
    EMP_RETURN bool write_changes(T const* const a_pValue)
    {
        EMP_STORE(T)->touch(a_pValue);
        return write(false);
    }

public:
    EMP_FIELD(bool, readonly, false)

private:
    vector<emp::fct::function_t<void(*)(void)>> m_vDealloc{};
    vector<store_i*> m_vpStores{};
    map_t<astring, store_i*> m_mpStores{64};
    vector<type_s*> m_vpTypes{};
    vector<astring> m_vsStack{};
    astring m_sPath{0};

    size_t m_stNextObjectId;

public:
    void register_deallocation(emp::fct::function_t<void(*)(void)> const& a_rFunction)
    { m_vDealloc.add(a_rFunction); }

public:
    template <typename T>
    EMP_RETURN type_s* id(void)
    {
        type_s* const pType = typeid_t<T>::s_pInstance;
        if (pType != nullptr)
        { return pType; }
        return type_t<T>::new_id(*this);
    }

    template <typename T>
    EMP_RETURN type_s* new_id(c_astring a_sName, c_type_e a_eType)
    {
        EMP_RET_NULL_IF_NOT(a_sName.len() > 0);
        EMP_RET_NULL_IF(typeid_t<T>::s_pInstance != nullptr);
        type_s* const pType = EMP_ALLOCATE(type_s, a_sName, a_eType, std::is_abstract<T>::value);
        m_vpTypes.add(pType);
        typeid_t<T>::s_pInstance = pType;
        compute_xml_type(pType->m_sName, pType->m_sSerializableName);
        compute_ctor_type(pType->m_sName, pType->m_sCtorFilename);
        return id<T>();
    }

    template <typename T>
    EMP_RETURN type_t<T>* cast(type_i* const a_pFrom)
    {
        EMP_ASSERT(a_pFrom != nullptr && a_pFrom->id() == id<T>());
        return static_cast<type_t<T>*>(a_pFrom);
    }

    template <typename T>
    EMP_RETURN type_t<T> const* cast(type_i const* const a_pFrom)
    {
        EMP_ASSERT(a_pFrom != nullptr && a_pFrom->id() == id<T>());
        return static_cast<type_t<T> const*>(a_pFrom);
    }

    template <typename T>
    EMP_RETURN store_t<T>* cast(store_i* const a_pFrom)
    {
        EMP_ASSERT(a_pFrom != nullptr && a_pFrom->id() == id<T>());
        return static_cast<store_t<T>*>(a_pFrom);
    }

    template <typename T>
    void deallocate(T*& a_rpObject)
    {
        store_t<T>* const pStore = store<T>();
        if (a_rpObject != nullptr &&
            pStore->get_object_info_s(a_rpObject) == nullptr)
        { pStore->destroy_object(a_rpObject); }
    }

    template <typename T>
    EMP_RETURN store_t<T>* store(void)
    {
        c_astring& sSerializableName = id<T>()->m_sSerializableName;
        auto const& oIt = m_mpStores.get(sSerializableName);
        if (oIt != m_mpStores.end())
        { return cast<T>(*oIt.value()); }

        {
            c_size stCount = m_vpStores.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                store_i* const pRegistered = m_vpStores.val(stIndex);
                EMP_RET_NULL_IF(pRegistered == nullptr);
                EMP_RET_NULL_IF(emp::str::cmp(pRegistered->id()->m_sSerializableName, sSerializableName));
            }
        }

        store_t<T>* const pStore = EMP_ALLOCATE(store_t<T>);
        m_mpStores.set(sSerializableName, pStore);
        m_vpStores.add(pStore);
        return pStore;
    }

    EMP_RETURN store_i* store(c_astring& a_sType)
    {
        auto const& oIt = m_mpStores.get(a_sType);
        if (oIt != m_mpStores.end())
        { return *oIt.value(); }
        return nullptr;
    }

public:
    factory_c(void) = default;
    ~factory_c(void);
    
public:
    void create(c_astring& a_sPath);
    void destroy(void);

    EMP_RETURN bool read(void);
    EMP_RETURN bool write(c_bool a_bAll);

    EMP_RETURN bool instanciate(c_bool a_bCheck);

    EMP_RETURN bool pre_load(void);
    EMP_RETURN bool post_load(void);

public:
    EMP_RETURN bool reflect_object(type_i& a_rType)
    {
        m_vsStack.resize(0);
        return reflect(a_rType);
    }

    EMP_RETURN bool reflect(type_i& a_rType);

    EMP_RETURN size_t next_object_id(void)
    { return m_stNextObjectId++; }
/*

    template <typename T, typename S>
    EMP_RETURN T* get_object(S a_sName)
    {
        store_t<T>* const pStore = store<T>();
        if (pStore == nullptr)
        {
            EMP_LOG_WARNING("store_t<#0> not found", type_t<T>::name());
            return nullptr;
        }

        return pStore->get_object(a_sName);
    }

    template <typename T, typename S>
    EMP_RETURN T const* get_object(S a_sName) const
    { return store<T>()->get_object(a_sName); }

    EMP_RETURN c_astring& path(void) const { return m_sPath; }
*/
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,type_t<vector<T>>, public, type_i)
private:
    static constexpr c_bool pod = false;

private:
    link_t<vector<T>> m_Link;
    vector<type_t<T>*> m_vpTypes;
    vector<astring> m_vsNames;

public:
    type_t(void):
        m_Link(),
        m_vpTypes(),
        m_vsNames()
    {}

    type_t(binder_t<vector<T>> const& a_Binder):
        m_Link(a_Binder),
        m_vpTypes(),
        m_vsNames()
    {}

    type_t(getter_t<vector<T>> const& a_Getter):
        m_Link(a_Getter),
        m_vpTypes(),
        m_vsNames()
    {}

    type_t(getter_t<vector<T>> const& a_Getter, setter_t<vector<T>> const& a_Setter):
        m_Link(a_Getter, a_Setter),
        m_vpTypes(),
        m_vsNames()
    {}

    ~type_t(void)
    {
        foreach(m_vpTypes, emp::mem::safe_delete_reset_t());
        m_vpTypes.resize(0);
    }

public:
    // >> type_i
    EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<vector<T>>(); }
    EMP_RETURN link_t<vector<T>> const& link(void) const { return m_Link; }
    EMP_RETURN link_t<vector<T>>& link(void) { return m_Link; }
    EMP_RETURN bool bind(void) { return m_Link.bind(); }
    EMP_RETURN bool post_load(void) { return true; }

    EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    { return read_impl__<T>(*this, a_rReader, a_stDepth, a_sDirectory); }
    EMP_RETURN bool eat_object(astring& EMP_SILENT(a_rReader)) { EMP_ASSERT(false); return false; }
    EMP_RETURN bool from_string(c_astring& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool from_string(c_u8string& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), u8string& a_rsOut EMP_UNUSED) const
    { emp::tt::silent(STR_FORMATU8(a_rsOut, "###0", m_Link.value().count())); return true; }

    EMP_RETURN size_t field_count(void) const { return m_Link.value().count(); }
    EMP_RETURN bool settable(void) const { return m_Link.setter().is_valid(); }
    EMP_RETURN bool bindable(void) const { return m_Link.binder().is_valid() ;}
    EMP_RETURN bool iterate(c_size a_stDepth, reflect_f const& a_rCallback)
    {
        c_size stCount = m_Link.value().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            T& rObject = m_Link.value().ref(stIndex);

            type_t<T>* const pReflection = get_reflection(stIndex);
            EMP_RET_FALSE_IF(pReflection == nullptr);
            pReflection->link().set_value(&rObject);

            field_s oField;
            oField.m_pType = pReflection;

            astring const& rsName = m_vsNames.ref(stIndex);
            oField.m_sName = rsName;
            oField.m_sLabel = string_s(rsName, u8string(rsName));
            a_rCallback(this, a_stDepth + 1, oField);
            //EMP_RET_FALSE_IF_NOT(pReflection->iterate(a_stDepth, a_rCallback));
        }
        return true;
    }

    EMP_RETURN bool reflect(factory_c& EMP_SILENT(a_rFactory))
    { return true; }
    // << type_i

    static EMP_INLINE EMP_RETURN type_s* new_id(factory_c& a_rFactory)
    { return a_rFactory.new_id<vector<T>>(STR_AFORMAT("emp::cnt::vector<#0>", a_rFactory.id<T>()->m_sName), type_e::Vector); }

    //>> class_i
    void set_value(vector<T>* const a_pValue) { m_Link.set_value(a_pValue); }
    //<< class_i

private:
    //template <typename U>
    //using is_data = emp::tt::and_<std::is_standard_layout<U>::value, std::is_pointer<U>::value == false>;
    //emp::tt::or_<emp::tt::equal<float, U>::value, emp::tt::equal<uint, U>::value>; // @@0 hardcoded

    ENABLE_IF_(U, is_data<U>::value)
    static EMP_RETURN bool read_impl__(type_t& a_rThis, xml::reader_c& a_rReader, c_size a_stDepth, c_astring& EMP_SILENT(a_sDirectory))
    {
        size_t stCount = 0;
        EMP_RET_FALSE_IF_NOT(emp::rfx::read_count(a_rReader, stCount, true));
        a_rThis.m_Link.value().resize(stCount);

        size_t EMP_TT_MAX_VAR(stCurrentDepth);
        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
        if ((stCurrentDepth + 1) == a_stDepth ||
            a_rReader.read_type() != emp::xml::node_e::Text)
        {
            xml::reader_c::c_read_result_e eReturn = a_rReader.next_element();
            EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
            if (eReturn == xml::reader_c::read_result_e::Break)
            { return stCount == 0; }
        }

        type_t<T>* const pType = a_rThis.get_reflection(0);
        EMP_RET_FALSE_IF(pType == nullptr);

        EMP_TT_MAX_VAR(stCurrentDepth);
        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
        size_t stRead = 0;
        if (stCurrentDepth == a_stDepth)
        {
            EMP_RET_FALSE_IF(a_rReader.read_type() != emp::xml::node_e::Text);

            astring sValue;
            EMP_RET_FALSE_IF_NOT(a_rReader.read_const_value(sValue));

            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                T& tValue = a_rThis.m_Link.value().ref(stIndex);
                pType->set_value(&tValue);

                EMP_RET_FALSE_IF_NOT(pType->eat_object(sValue), "Failed to eat pod value #0/#1", stIndex, stCount);
                ++stRead;
            }

            /*EMP_TT_MAX_VAR(stCurrentDepth);
            EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
            if (stCurrentDepth > a_stDepth ||
                a_rReader.read_type() != xml::node_e::Element)
            {
                xml::reader_c::read_result_e eReturn = a_rReader.next_valid_element();
                EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
            }*/
            /* @@0 remove astring sEnd;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                T& tValue = m_Link.value().ref(stIndex);
                EMP_RET_FALSE_IF_NOT(to(sValue, tValue, &sEnd), "Failed to read vector index #0/#1", stIndex, stCount);
                sValue = sEnd;
                ++stRead;
            }*/
        }
        return true; //@@0 bug ?
    }

    ENABLE_IF_(U, is_data<U>::value == false)
    static EMP_RETURN bool read_impl__(type_t& a_rThis, xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    {
        // @@0 remove ? EMP_RET_FALSE_IF_NOT(emp::rfx::read_type(a_rReader, this->xml_type(), true));

        size_t stIndex = 0;
        size_t stCount = 0;
        EMP_RET_FALSE_IF_NOT(emp::rfx::read_count(a_rReader, stCount, true));
        a_rThis.m_Link.value().resize(stCount);

        size_t EMP_TT_MAX_VAR(stCurrentDepth);

        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));

        if ((stCurrentDepth + 1) == a_stDepth ||
            a_rReader.read_type() != xml::node_e::Element)
        {
            xml::reader_c::read_result_e eReturn = a_rReader.next_valid_element();
            EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
            if (eReturn == xml::reader_c::read_result_e::Break)
            { return stCount == 0; }
        }

        type_t<T>* const pType = a_rThis.get_reflection(0);
        EMP_RET_FALSE_IF(pType == nullptr);

        while (true)
        {
            EMP_TT_MAX_VAR(stCurrentDepth);
            EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));

            if (stCurrentDepth == a_stDepth)
            {
                astring sName;
                EMP_RET_FALSE_IF_NOT(a_rReader.read_const_name(sName));

                c_astring sReflectionName(ALITERAL("i"));

                EMP_RET_FALSE_IF_NOT(cmp(sName, sReflectionName),
                    "Failed to read #0, #1 found #2", a_rThis.id()->m_sName, sReflectionName, sName);

                EMP_RET_FALSE_IF(stCount > 0 && stIndex >= stCount, "Invalid vector index : #0/#1", stIndex, stCount);

                if (stCount == 0)
                { a_rThis.m_Link.value().resize(stIndex + 1); }

                T& tValue = a_rThis.m_Link.value().ref(stIndex);
                pType->link().set_value(&tValue);

                EMP_RET_FALSE_IF_NOT(pType->read_object(a_rReader, a_stDepth + 1, a_sDirectory));

                ++stIndex;

                EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
                if (stCurrentDepth > a_stDepth ||
                    a_rReader.read_type() != xml::node_e::Element)
                {
                    xml::reader_c::read_result_e eReturn = a_rReader.next_valid_element();
                    EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);

                    if (eReturn == xml::reader_c::read_result_e::Break)
                    { break; }
                }
            }
            else
            { break; }
        }

        if ((stCount == 0 && stIndex == 0) || stIndex == stCount)
        { return true; }

        EMP_LOG_ERROR("Invalid vector element count : #0/#1", stIndex, stCount);
        EMP_XX_HOOK()
        return false;
    }

    EMP_RETURN type_t<T>* get_reflection(c_size a_stIndex)
    {
        if (a_stIndex < m_vpTypes.count())
        { return m_vpTypes.val(a_stIndex); }

        type_t<T>* pType = EMP_ALLOCATE(type_t<T>);
        m_vpTypes.add(pType);
        EMP_RET_NULL_IF_NOT(EMP_FACTORY.reflect(*pType));

        m_vsNames.add(STR_AFORMAT("###0", a_stIndex));
        return pType;
    }

    EMP_RETURN bool read_attribute(xml::reader_c& EMP_SILENT(a_rReader)) { EMP_ASSERT_UNREACHABLE(); return false; }

    EMP_RETURN bool write(xml::writer_c& a_rWriter) { return write_object(a_rWriter); }
    EMP_RETURN bool write_attribute(xml::writer_c& EMP_SILENT(a_rWriter), c_astring& EMP_SILENT(a_sName)) { return false; }
    EMP_RETURN bool write_object(xml::writer_c& a_rWriter) { return write_object_impl<T>(a_rWriter); }
    EMP_RETURN bool write_element(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }
    ENABLE_IF_(U, is_data<U>::value)
    EMP_RETURN bool write_object_impl(xml::writer_c& a_rWriter)
    {
        c_size stCount = m_Link.value().count();
        EMP_RET_FALSE_IF_NOT(emp::rfx::write_count(a_rWriter, stCount));

        type_t<T>* const pType = get_reflection(0);
        EMP_RET_FALSE_IF(pType == nullptr);

        if (stCount > 0)
        {
            auto const l_write_value = [this, &a_rWriter, pType](c_size a_stIndex) EMP_RETURN -> bool
            {
                T& tValue = m_Link.value().ref(a_stIndex);
                pType->set_value(&tValue);
                EMP_RET_FALSE_IF_NOT(pType->write_element(a_rWriter));
                return true;
            };

            EMP_RET_FALSE_IF_NOT(l_write_value(0));
            for (size_t stIndex = 1; stIndex < stCount; ++stIndex)
            {
                EMP_RET_FALSE_IF_NOT(a_rWriter.write_raw(U8LITERAL(" ")));
                EMP_RET_FALSE_IF_NOT(l_write_value(stIndex));
            }
        }
        return true;
    }

    ENABLE_IF_(U, is_data<U>::value == false)
    EMP_RETURN bool write_object_impl(xml::writer_c& a_rWriter)
    {
        size_t stWritableCount = 0;
        c_size stCount = m_Link.value().count();

        type_t<T>* const pType = get_reflection(0);
        c_type_e eType = pType->id()->m_eType;
        c_bool bClass = eType == type_e::Vector || eType == type_e::Pointer || eType == type_e::Function;

        if (eType == type_e::Pointer)
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                T& tValue = m_Link.value().ref(stIndex);
                pType->set_value(&tValue);
                if (pType->is_writable())
                { ++stWritableCount; }
            }
        }
        else
        {
            stWritableCount = stCount;
        }

        EMP_RET_FALSE_IF_NOT(write_count(a_rWriter, stWritableCount));

        EMP_RET_FALSE_IF(pType == nullptr);

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            T& tValue = m_Link.value().ref(stIndex);
            pType->set_value(&tValue);
            if (pType->is_writable() == false)
            { continue; }

            EMP_RET_FALSE_IF_NOT(a_rWriter.start_element(ALITERAL("i")));
            if (bClass)
            { EMP_RET_FALSE_IF_NOT(pType->write(a_rWriter)); }
            else
            { EMP_RET_FALSE_IF_NOT(pType->write_element(a_rWriter)); }
            EMP_RET_FALSE_IF_NOT(a_rWriter.end_element());
        }

        return true;
    }

    EMP_RETURN bool is_attribute(void) const { return false; }
    EMP_RETURN bool is_writable(void) const { return true; }
    /*
public:
    //>> type_i


    EMP_RETURN bool write_object_impl(writer_c& a_rWriter);

    EMP_RETURN astring to_string(formater_i* const a_pFormater) const
    { return STR_AFORMAT("count : {#0}", m_Bind.value().count()); }   // @@2 alloc

    EMP_RETURN bool from_string(c_astring&) { return false; }

    EMP_RETURN size_t field_count(void) const { return m_Bind.value().count(); }
    //<< type_i
*/
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, type_t<T*>, public, type_i)
public:
    link_t<T*> m_Link;
    static constexpr c_bool pod = false;
    type_t<T> m_Type;

public:
    type_t(void):
        m_Link(),
        m_Type()
    {}

    type_t(binder_t<T*> const& a_Binder):
        m_Link(a_Binder),
        m_Type()
    {}

    type_t(getter_t<T*> const& a_Getter):
        m_Link(a_Getter),
        m_Type()
    {}

    type_t(getter_t<T*> const& a_Getter, setter_t<T*> const& a_Setter):
        m_Link(a_Getter, a_Setter),
        m_Type()
    {}

    virtual ~type_t(void)
    {}

public:
    // >> type_i
    EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<T*>(); }
    EMP_RETURN link_t<T*> const& link(void) const { return m_Link; }
    EMP_RETURN link_t<T*>& link(void) { return m_Link; }
    EMP_RETURN bool bind(void)
    {
        if (bindable())
        {
            EMP_RET_FALSE_IF_NOT(m_Link.bind());
            m_Type.link().set_value(m_Link.value());
        }
        else
        {
            T* tValue;
            EMP_RET_FALSE_IF_NOT(m_Link.getter()(tValue));
            // @@0 implement : m_eType = get_type(tValue);
        }

        return true;
    }

    EMP_RETURN bool post_load(void) { return true; }
    
    EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
    {
        auto const eResult = a_rReader.next_attribute();
        EMP_RET_FALSE_IF(eResult == emp::xml::reader_c::read_result_e::Error);

        store_t<T>* const pStore = EMP_STORE(T);
        EMP_RET_FALSE_IF(pStore == nullptr, "Store not found : #0", id()->m_sName);

        if (eResult == emp::xml::reader_c::read_result_e::Break)
        {
            // @@0 remove : m_eType = type_e::HeapObject;
            astring sType(id()->m_sName);
            sType.trim_right();
            m_Link.value() = pStore->read_object(a_rReader, a_stDepth, a_sDirectory, sType, astring());
            EMP_RET_FALSE_IF(m_Link.value() == nullptr, "Failed to read object : #0", id()->m_sName);
            return true;
        }
        else if (eResult == emp::xml::reader_c::read_result_e::Continue)
        {
            astring sName;
            astring sVariable;

            astring sAttribute;
            EMP_RET_FALSE_IF_NOT(a_rReader.read_const_name(sAttribute));
            if (emp::str::cmp(sAttribute, astring(EMP_RFX_TOKEN_V_TYPE_V1))) // @@0 optim map token
            {
                astring sType;
                EMP_RET_FALSE_IF_NOT(a_rReader.read_const_value(sType));
                if (sType.len() > 0)
                {
                    EMP_RET_FALSE_IF(emp::str::cmp(sType, id()->m_sSerializableName),
                        "Useless type information #0", sType);
                    EMP_RET_FALSE_IF(sType.get_last_char() != '*',
                        "Failed to read pointer type #0, found #1", id()->m_sName, sType);
                    sType.trim_right();
                }

                EMP_RET_FALSE_IF(sType.len() == 0,  "empty type found while reading #0", id()->m_sName);

                if (a_rReader.next_valid_attribute())
                {
                    EMP_RET_FALSE_IF_NOT(a_rReader.read_const_name(sName));
                    if (emp::str::cmp(sName, astring(EMP_RFX_TOKEN_V_NAME_V1)))
                    { EMP_RET_FALSE_IF_NOT(a_rReader.read_owned_value(sVariable)); }
                    else
                    { a_rReader.set_unused_attribute(true); }
                }

                // @@0 remove : m_eType = type_e::HeapObject;
                m_Link.value() = pStore->read_object(a_rReader, a_stDepth, a_sDirectory, sType, sVariable);
                EMP_RET_FALSE_IF(m_Link.value() == nullptr, "Failed to read object : #0, #1", id()->m_sName, sType);
                return true;
            }
            else if (emp::str::cmp(sAttribute, astring(EMP_RFX_TOKEN_V_POINTER_V1)))
            {
                // @@0 remove : m_eType = type_e::HeapObject;
                astring sPointer;
                EMP_RET_FALSE_IF_NOT(a_rReader.read_const_value(sPointer));
                EMP_RET_FALSE_IF_NOT(sPointer.len() > 0);
                m_Link.value() = pStore->get_object(sPointer);
                EMP_RET_FALSE_IF(m_Link.value() == nullptr, "Pointer not found : #0 #1", id()->m_sName, sPointer);
                return true;
            }
            else
            {
                if (emp::str::cmp(sAttribute, astring(EMP_RFX_TOKEN_V_NAME_V1)))
                { EMP_RET_FALSE_IF_NOT(a_rReader.read_owned_value(sVariable)); }
                else
                { a_rReader.set_unused_attribute(true); }
                // @@0 remove : m_eType = type_e::HeapObject;
                astring sType(id()->m_sName);
                sType.trim_right();
                m_Link.value() = pStore->read_object(a_rReader, a_stDepth, a_sDirectory, sType, sVariable);
                EMP_RET_FALSE_IF(m_Link.value() == nullptr, "Failed to read object : #0", id()->m_sName);
                return true;
            }
        }

        EMP_XX_HOOK();
        return false;
    }

    EMP_RETURN bool eat_object(astring& EMP_SILENT(a_rReader)) { EMP_ASSERT(false); return false; }


    EMP_RETURN bool from_string(c_astring& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
    EMP_RETURN bool from_string(c_u8string& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }

    EMP_RETURN bool to_string(str::formater_i* const a_pFormater, u8string& a_rsOut) const
    {
        if (m_Link.has_value())
        { return emp::rfx::to_string(*m_Link.value(), a_pFormater, a_rsOut); }
        else
        {
            T* tValue;
            EMP_RET_FALSE_IF_NOT(m_Link.getter()(tValue));
            return emp::rfx::to_string(*tValue, a_pFormater, a_rsOut);
        }
    }

    EMP_RETURN size_t field_count(void) const { return 1; }
    EMP_RETURN bool settable(void) const { return m_Link.setter().is_valid(); }
    EMP_RETURN bool bindable(void) const { return m_Link.binder().is_valid(); }
    EMP_RETURN bool iterate(c_size a_stDepth, reflect_f const& a_rCallback)
    {
        if (m_Link.has_value())
        { m_Type.link().set_value(m_Link.value());
        }
        else
        {
            T* tValue;
            EMP_ASSERT_BOOL_CALL(m_Link.getter()(tValue));
            m_Type.link().set_value(tValue);
        }
        return m_Type.iterate(a_stDepth, a_rCallback);
    }

     EMP_RETURN bool reflect(factory_c& EMP_SILENT(a_rFactory)) { return emp::rfx::reflect(m_Type); }
    // << type_i

    static EMP_RETURN EMP_INLINE type_s* new_id(factory_c& a_rFactory)
    { return a_rFactory.new_id<T*>(STR_AFORMAT("#0*", a_rFactory.id<T>()->m_sName), type_e::Pointer); }

    void set_value(T** const a_pValue)
    {
        m_Link.set_value(a_pValue);
        if (m_Link.value() != nullptr)
        {
            // @@0 implement { m_eType = get_type(m_Bind.value());
        }
    }

    // >> type_i
    //EMP_RETURN bool instanciate(vector<astring>&, vector<astring>&) { return false; }
    //EMP_RETURN type_s const& id(void) const;
    //EMP_RETURN size_t recursive_object_count(void) const;
    // << type_i

    //>> class_i
    //<< class_i

    EMP_RETURN bool read_attribute(xml::reader_c& a_rReader)
    {
        store_t<T>* const pStore = EMP_STORE(T);
        EMP_RET_FALSE_IF(pStore == nullptr, "Store not found : #0", id()->m_sName);

        astring sPointer;
        EMP_RET_FALSE_IF_NOT(a_rReader.read_const_attribute_value(sPointer, true));

        // @@0 implement m_eType = type_e::StoreObject;
        if (sPointer.len() > 0)
        {
            m_Link.value() = pStore->get_object(sPointer);
            EMP_RET_FALSE_IF(m_Link.value() == nullptr, "Pointer not found : #0 #1", id()->m_sName, sPointer);
        }
        return true;
    }

    /*
public:
    //>> type_i
    void refresh(void)
    {
        m_Reflection.refresh();
        if (m_eType == type_e::StoreObject &&
            m_bStored == false &&
            m_Reflection.changed())
        {
            store_t<T>* const pStore = EMP_STORE(T);
            EMP_ASSERT(pStore);
            pStore->set_changed(m_Bind.value());
            m_bStored = true;
        }
    }
*/
public:
    EMP_RETURN bool write(xml::writer_c& a_rWriter)
    {
        store_t<T>* const pStore = EMP_STORE(T);
        EMP_RET_FALSE_IF(pStore == nullptr, "Store not found : #0", id()->m_sName);

        object_s* const pObject = pStore->get_object_info_s(m_Link.value());
        EMP_RET_FALSE_IF(pObject == nullptr);

        if (pObject->file ||
            (pObject->variable && pObject->declared))
        {
            EMP_RET_FALSE_IF_NOT(a_rWriter.write_attribute(EMP_RFX_TOKEN_V_POINTER_V1, pObject->name));
            return true;
        }

        if (pStore != pObject->concrete)
        {
            EMP_RET_FALSE_IF_NOT(pObject->concrete->write_void(a_rWriter, pObject->pointer));
            return true;
        }

        EMP_RET_FALSE_IF_NOT(pStore->write_object(a_rWriter, *m_Link.value(), m_Type));
        return true;
    }

    EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, c_astring& a_sName)
    {
        store_t<T>* const pStore = EMP_STORE(T);
        EMP_RET_FALSE_IF(pStore == nullptr, "Store not found : #0", id()->m_sName);

        if (m_Link.value() != nullptr)
        {
            object_s const* const pObject = pStore->get_object_info_s(m_Link.value());
            EMP_RET_FALSE_IF(pObject == nullptr);

            if (pObject->file ||
                (pObject->variable && pObject->declared))
            {
                EMP_RET_FALSE_IF_NOT(a_rWriter.write_attribute(a_sName, pObject->name));
                return true;
            }
        }
        else
        {
            EMP_RET_FALSE_IF_NOT(a_rWriter.write_attribute(a_sName, u8string(U8LITERAL(""))));
            return true;
        }
        return false;
    }

    EMP_RETURN bool write_object(xml::writer_c& EMP_SILENT(a_rWriter))
    {
        return false;
        // @@0 save m_Reflection.set_value(m_Bind.value());
        // @@0 EMP_RET_FALSE_IF_NOT(m_Reflection.write_object(a_rWriter), "Failed to write : #0;", m_Reflection.xml_type())
        // @@0 return true;
    }

    EMP_RETURN bool write_element(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }

    EMP_RETURN bool is_attribute(void) const
    {
        store_t<T>* const pStore = EMP_STORE(T);
        EMP_RET_FALSE_IF(pStore == nullptr, "Store not found : #0", id()->m_sName);

        if (m_Link.value() == nullptr)
        { return true; }
        object_s const* const pObject = pStore->get_object_info_s(m_Link.value());
        if (pObject != nullptr && (pObject->file || (pObject->variable && pObject->declared)))
        { return true; }
        return false;
    }

    EMP_RETURN bool is_writable(void) const
    { return EMP_STORE(T)->get_object_info_s(m_Link.value()) != nullptr; }

/*
    EMP_RETURN bool write_attribute(c_astring& a_sName, writer_c& a_rWriter)
    {
        if (this->e_type() == type_e::StoreObject)
        {
            store_t<T>* const pStore = EMP_STORE(T);
            object_s const oObject = pStore ? pStore->get_object(m_Bind.value()) : object_s();
            if (pStore != nullptr &&
                oObject.store != nullptr &&
                oObject.type.len() > 0 &&
                oObject.name.len() > 0)
            {
                EMP_RET_FALSE_IF_NOT(a_rWriter.write_attribute(a_sName, oObject.name));
                return true;
            }
        }
        return false;
    }

    EMP_RETURN bool settable(void) const { return m_Bind.settable(); }
    EMP_RETURN bool bindable(void) const { return m_Bind.bindable(); }

    EMP_RETURN bool from_string(c_astring&) { return false; }

    EMP_RETURN size_t field_count(void) const { return 1; }*/
    //<< type_i
};

/*
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN EMP_INLINE type_i* get_type(types_m const& a_rmpTypes, c_astring& a_sName);
EMP_RETURN EMP_INLINE type_i* get_type(types_m const& a_rmpTypes, c_astring& a_sName)
{
    types_m::iterator_c const oIt = a_rmpTypes.get(a_sName);
    if (oIt != a_rmpTypes.end())
    { return *oIt.value(); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_RETURN type_e get_type(T* const a_pValue);
template <typename T> EMP_RETURN type_e get_type(T* const a_pValue)
{
    store_t<T>* const pStore = EMP_STORE(T);
    if (pStore == nullptr)
    { return type_e::HeapObject; }

    object_s const oObject = pStore->get_object(a_pValue);
    if (oObject.store != nullptr &&
        oObject.type.len() > 0 &&
        oObject.name.len() > 0)
    { return type_e::StoreObject; }
    else
    { return type_e::HeapObject; }
    return type_e::none;
}

*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* #ifdef EMP_XX_ARCH_32
EMP_RFX_TYPE(long int, type_e::SInt64, false, false) #endif #if defined EMP_XX_OS_OSX || defined EMP_XX_OS_IOS
EMP_RFX_TYPE(unsigned long int, type_e::UInt64, false, false) #endif
// @@4long, long long. */

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
EMP_RETURN type_t<T>* cast(type_i* const a_pFrom)
{
    EMP_ASSERT(a_pFrom);
    EMP_ASSERT(a_pFrom->to_enum() == type_t<T>::type);
    // @@0 assert name equal
    return emp::ast::cast<type_t<T>*>(a_pFrom);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN store_t<T>* store(void)
{ return EMP_FACTORY.store<T>(); }

EMP_INLINE EMP_RETURN bool reflect(type_i& a_rType)
{ return EMP_FACTORY.reflect_object(a_rType); }

EMP_INLINE EMP_RETURN store_i* store(c_astring& a_sType)
{ return EMP_FACTORY.store(a_sType); }

template <typename T>
EMP_RETURN type_s const* id(void)
{ return EMP_FACTORY.id<T>(); }

EMP_INLINE EMP_RETURN size_t next_object_id(void)
{ return EMP_FACTORY.next_object_id(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_values EMP_RFX_TYPE_E_VALUES
#include "emp_rfx_value.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

