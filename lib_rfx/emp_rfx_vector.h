//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_VECTOR_H
#define EMP_RFX_VECTOR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#include "emp_rfx_factory.h"
//
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//namespace emp { namespace rfx {
//
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//template <typename T>
//EMP_NOCOPY_BASE(class, type_t<vector<T>>, public, type_i)
//private:
//    static constexpr c_bool pod = false;
//
//private:
//    link_t<vector<T>> m_Link;
//    vector<type_t<T>*> m_vpTypes;
//    vector<astring> m_vsNames;
//
//public:
//    type_t(void) :
//        m_Link(),
//        m_vpTypes(),
//        m_vsNames()
//    {}
//
//    type_t(binder_t<vector<T>> const& a_Binder) :
//        m_Link(a_Binder),
//        m_vpTypes(),
//        m_vsNames()
//    {}
//
//    type_t(getter_t<vector<T>> const& a_Getter) :
//        m_Link(a_Getter),
//        m_vpTypes(),
//        m_vsNames()
//    {}
//
//    type_t(getter_t<vector<T>> const& a_Getter, setter_t<vector<T>> const& a_Setter) :
//        m_Link(a_Getter, a_Setter),
//        m_vpTypes(),
//        m_vsNames()
//    {}
//
//    ~type_t(void)
//    {
//        foreach(m_vpTypes, emp::mem::safe_delete_reset_t());
//    }
//
//public:
//    // >> type_i
//    EMP_RETURN type_s const* id(void) const { return EMP_FACTORY.id<vector<T>>(); }
//    EMP_RETURN link_t<vector<T>> const& link(void) const { return m_Link; }
//    EMP_RETURN link_t<vector<T>>& link(void) { return m_Link; }
//    EMP_RETURN bool bind(void) { return m_Link.bind(); }
//    EMP_RETURN bool post_load(void) { return true; }
//
//    EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory)
//    {
//        return read_impl_t<T>(*this, a_rReader, a_stDepth, a_sDirectory);
//    }
//    EMP_RETURN bool eat_object(astring& EMP_SILENT(a_rReader)) { EMP_ASSERT(false); return false; }
//    EMP_RETURN bool from_string(c_astring& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
//    EMP_RETURN bool from_string(c_u8string& EMP_SILENT(a_sValue)) { EMP_ASSERT_NOT_IMPLEMENTED(); return false; }
//    EMP_RETURN bool to_string(str::formater_i* const EMP_SILENT(a_pFormater), u8string& EMP_SILENT(a_rsOut)) const
//    {
//        emp::tt::silent(STR_FORMATU8(a_rsOut, "count {#0}", m_Link.value().count()));
//        return true;
//    }
//
//    EMP_RETURN size_t field_count(void) const { return m_Link.value().count(); }
//    EMP_RETURN bool settable(void) const { return m_Link.setter().is_valid(); }
//    EMP_RETURN bool bindable(void) const { return m_Link.binder().is_valid(); }
//    EMP_RETURN bool iterate(c_size EMP_SILENT(a_stDepth), reflect_f const& EMP_SILENT(a_rCallback))
//    {
//        c_size stCount = m_Link.value().count();
//        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
//        {
//            T& rObject = m_Link.value().ref(stIndex);
//
//            type_t<T>* const pReflection = get_reflection(stIndex);
//            EMP_RET_FALSE_IF(pReflection == nullptr);
//            pReflection->link().set_value(&rObject);
//
//            /*
//            // @@0 device
//            c_astring& sField = m_vsFields.ref(stIndex);
//            m_pLocName->set_value(sField);
//            a_rCallback(this, a_stDepth + 1, pReflection, sField, *m_pLocName , *m_pLocDesc);
//            */
//        }
//        return true;
//    }
//
//    EMP_RETURN bool reflect(factory_c& EMP_SILENT(a_rFactory))
//    {
//        return true;
//    }
//    // << type_i
//
//    static EMP_INLINE EMP_RETURN type_s* new_id(factory_c& a_rFactory)
//    {
//        return a_rFactory.new_id<vector<T>>(STR_AFORMAT("emp::cnt::vector<#0>", a_rFactory.id<T>()->m_sName), type_e::Vector);
//    }
//
//    //>> class_i
//    void set_value(vector<T>* const a_pValue) { m_Link.set_value(a_pValue); }
//    //<< class_i
//
//private:
//    //template <typename U>
//    //using is_data = emp::tt::and_<std::is_standard_layout<U>::value, std::is_pointer<U>::value == false>;
//    //emp::tt::or_<emp::tt::equal<float, U>::value, emp::tt::equal<uint, U>::value>; // @@0 hardcoded
//
//    //template <typename U>
//    //using is_data = emp::tt::integral<bool, type_t<U>::pod>;
//
//    //template <typename F, emp::tt::enable_if<type_t<F>::pod>* = nullptr>
//    ENABLE_IF_(U, type_t<U>::pod)
//    static EMP_RETURN bool read_impl_t(type_t& a_rThis, xml::reader_c& a_rReader, c_size a_stDepth, c_astring& EMP_SILENT(a_sDirectory))
//    {
//        size_t stCount = 0;
//        EMP_RET_FALSE_IF_NOT(emp::rfx::read_count(a_rReader, stCount, true));
//        a_rThis.m_Link.value().resize(stCount);
//
//        size_t EMP_TT_MAX_VAR(stCurrentDepth);
//        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
//        if ((stCurrentDepth + 1) == a_stDepth ||
//            a_rReader.read_type() != emp::xml::node_e::Text)
//        {
//            xml::reader_c::c_read_result_e eReturn = a_rReader.next_element();
//            EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
//            if (eReturn == xml::reader_c::read_result_e::Break)
//            {
//                return stCount == 0;
//            }
//        }
//
//        type_t<T>* const pType = a_rThis.get_reflection(0);
//        EMP_RET_FALSE_IF(pType == nullptr);
//
//        EMP_TT_MAX_VAR(stCurrentDepth);
//        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
//        size_t stRead = 0;
//        if (stCurrentDepth == a_stDepth)
//        {
//            EMP_RET_FALSE_IF(a_rReader.read_type() != emp::xml::node_e::Text);
//
//            astring sValue;
//            EMP_RET_FALSE_IF_NOT(a_rReader.read_const_value(sValue));
//
//            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
//            {
//                T& tValue = a_rThis.m_Link.value().ref(stIndex);
//                pType->set_value(&tValue);
//
//                EMP_RET_FALSE_IF_NOT(pType->eat_object(sValue), "Failed to eat pod value #0/#1", stIndex, stCount);
//                ++stRead;
//            }
//
//            /*EMP_TT_MAX_VAR(stCurrentDepth);
//            EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
//            if (stCurrentDepth > a_stDepth ||
//            a_rReader.read_type() != xml::node_e::Element)
//            {
//            xml::reader_c::read_result_e eReturn = a_rReader.next_valid_element();
//            EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
//            }*/
//            /* @@0 remove astring sEnd;
//            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
//            {
//            T& tValue = m_Link.value().ref(stIndex);
//            EMP_RET_FALSE_IF_NOT(to(sValue, tValue, &sEnd), "Failed to read vector index #0/#1", stIndex, stCount);
//            sValue = sEnd;
//            ++stRead;
//            }*/
//        }
//        return true; //@@0 bug ?
//    }
//
//    //template <typename F, emp::tt::enable_if<type_t<F>::pod == false>* = nullptr>
//    ENABLE_IF_(U, type_t<U>::pod == false)
//    static EMP_RETURN bool read_impl_t(type_t& a_rThis, xml::reader_c& a_rReader, c_size a_stDepth, c_astring& EMP_SILENT(a_sDirectory))
//    {
//        // @@0 remove ? EMP_RET_FALSE_IF_NOT(emp::rfx::read_type(a_rReader, this->xml_type(), true));
//
//        size_t stIndex = 0;
//        size_t stCount = 0;
//        EMP_RET_FALSE_IF_NOT(emp::rfx::read_count(a_rReader, stCount, true));
//        a_rThis.m_Link.value().resize(stCount);
//
//        size_t EMP_TT_MAX_VAR(stCurrentDepth);
//
//        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
//
//        if ((stCurrentDepth + 1) == a_stDepth ||
//            a_rReader.read_type() != xml::node_e::Element)
//        {
//            xml::reader_c::read_result_e eReturn = a_rReader.next_valid_element();
//            EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
//
//            if (eReturn == xml::reader_c::read_result_e::Break)
//            {
//                return stCount == 0;
//            }
//        }
//
//        type_t<T>* const pType = a_rThis.get_reflection(0);
//        EMP_RET_FALSE_IF(pType == nullptr);
//
//        while (true)
//        {
//            EMP_TT_MAX_VAR(stCurrentDepth);
//            EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
//
//            if (stCurrentDepth == a_stDepth)
//            {
//                astring sName;
//                EMP_RET_FALSE_IF_NOT(a_rReader.read_const_name(sName));
//
//                c_astring sReflectionName(ALITERAL("i"));
//
//                EMP_RET_FALSE_IF_NOT(cmp(sName, sReflectionName),
//                    "Failed to read #0, #1 found #2", a_rThis.id()->m_sName, sReflectionName, sName);
//
//                EMP_RET_FALSE_IF(stCount > 0 && stIndex >= stCount, "Invalid vector index : #0/#1", stIndex, stCount);
//
//                if (stCount == 0)
//                {
//                    a_rThis.m_Link.value().resize(stIndex + 1);
//                }
//
//                T& tValue = a_rThis.m_Link.value().ref(stIndex);
//                pType->link().set_value(&tValue);
//
//                EMP_RET_FALSE_IF_NOT(pType->read_object(a_rReader, a_stDepth + 1, a_sDirectory));
//
//                ++stIndex;
//
//                EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));
//                if (stCurrentDepth > a_stDepth ||
//                    a_rReader.read_type() != xml::node_e::Element)
//                {
//                    xml::reader_c::read_result_e eReturn = a_rReader.next_valid_element();
//                    EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error);
//
//                    if (eReturn == xml::reader_c::read_result_e::Break)
//                    {
//                        break;
//                    }
//                }
//            }
//            else
//            {
//                break;
//            }
//        }
//
//        if ((stCount == 0 && stIndex == 0) || stIndex == stCount)
//        {
//            return true;
//        }
//
//        EMP_RET_FALSE_IF_NOT(false, "Invalid vector element count : #0/#1", stIndex, stCount);
//        return true;
//    }
//
//    EMP_RETURN type_t<T>* get_reflection(c_size a_stIndex)
//    {
//        if (a_stIndex < m_vpTypes.count())
//        {
//            return m_vpTypes.val(a_stIndex);
//        }
//
//        type_t<T>* pType = EMP_ALLOCATE(type_t<T>);
//        m_vpTypes.add(pType);
//        EMP_RET_NULL_IF_NOT(EMP_FACTORY.reflect(*pType));
//
//        m_vsNames.add(STR_AFORMAT("[#0]", a_stIndex));
//        return pType;
//    }
//
//    EMP_RETURN bool read_attribute(xml::reader_c& EMP_SILENT(a_rReader)) { EMP_ASSERT_UNREACHABLE(); return false; }
//
//    /*astring m_sXmlType;
//    EMP_POINTER(string_s) m_pLocName;
//    EMP_POINTER(string_s) m_pLocDesc;
//
//    public:
//    type_t(getter_t<vector<T>>* const a_pGetter, setter_t<vector<T>>* const a_pSetter, binder_t<vector<T>>* const a_pBinder):
//    m_sXmlType(),
//    m_pLocName("emp_rfx_vector_index_name"),
//    m_pLocDesc("emp_rfx_vector_index_desc")
//    {}
//
//    public:
//    //>> type_i
//    EMP_RETURN bool write(writer_c& a_rWriter)
//    {
//    EMP_RET_FALSE_IF_NOT(write(this, a_rWriter));
//    return true;
//    }
//
//    EMP_RETURN enable_if<is_data, bool>::type write_object(writer_c& a_rWriter)
//    {
//    size_t stCount = m_Bind.value().count();
//    EMP_RET_FALSE_IF_NOT(emp::rfx::write_count(a_rWriter, stCount));
//    type_t<T>* const pReflection = get_reflection(m_vpReflections, 0, m_vsFields);
//    if (pReflection == nullptr)
//    { return false; }
//    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
//    {
//    T& tValue = m_Bind.value().ref(stIndex);
//    pReflection->set_value(&tValue);
//    EMP_RET_FALSE_IF_NOT(pReflection->write_element(a_rWriter));
//    EMP_RET_FALSE_IF_NOT(a_rWriter.write_raw(" "));
//    }
//    return true;
//    }
//
//    EMP_RETURN enable_if_not<is_data, bool>::type write_object(writer_c& a_rWriter)
//    {
//    size_t stCount = m_Bind.value().count();
//    EMP_RET_FALSE_IF_NOT(write_count(a_rWriter, stCount));
//
//    type_t<T>* const pReflection = get_reflection(m_vpReflections, 0, m_vsFields);
//    if (pReflection == nullptr)
//    { return false; }
//
//    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
//    {
//    T& tValue = m_Bind.value().ref(stIndex);
//    pReflection->set_value(&tValue);
//
//    EMP_RET_FALSE_IF_NOT(a_rWriter.start_element("i"));
//    if (pReflection->e_type() == type_e::Class ||
//    pReflection->e_type() == type_e::Vector ||
//    pReflection->e_type() == type_e::StoreObject ||
//    pReflection->e_type() == type_e::HeapObject ||
//    pReflection->e_type() == type_e::Function)
//    { EMP_RET_FALSE_IF_NOT(pReflection->write(a_rWriter)); }
//    else
//    { EMP_RET_FALSE_IF_NOT(pReflection->write_element(a_rWriter)); }
//    EMP_RET_FALSE_IF_NOT(a_rWriter.end_element());
//    }
//
//    return true;
//    }
//
//    EMP_RETURN bool write_object_impl(writer_c& a_rWriter);
//
//    EMP_RETURN astring to_string(formater_i* const a_pFormater) const
//    { return STR_AFORMAT("count : {#0}", m_Bind.value().count()); }   // @@2 alloc
//
//    EMP_RETURN bool from_string(c_astring&) { return false; }
//
//    EMP_RETURN size_t field_count(void) const { return m_Bind.value().count(); }
//    //<< type_i
//    */
//};
//
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//} }
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

