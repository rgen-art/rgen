//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

#include "emp_rfx_class.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_fs_file.h"

#include "emp_xml_reader.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
factory_c* factory_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_token(c_astring& a_sToken, xml::reader_c& a_rReader, c_astring& a_sValue, c_bool a_bLog)
{
    EMP_RET_FALSE_IF_NOT(a_sToken.len() > 0);

    astring sValue;
    if (a_rReader.read_const_attribute(a_sToken, sValue, a_bLog) == false)
    {
        if (a_bLog)
        { EMP_LOG_ERROR("Failed to read attribute #0", a_sToken); }
        return false;
    }

    if (cmp(a_sValue, sValue) == false)
    {
        if (a_bLog)
        { EMP_LOG_ERROR("Failed to read token : '#0', found '#1'", a_sValue, sValue); }
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_type(xml::reader_c& a_rReader, c_astring& a_sType, c_bool a_bLog)
{ return read_token(EMP_RFX_TOKEN_V_TYPE_V1, a_rReader, a_sType, a_bLog); }

EMP_RETURN bool write_type(xml::writer_c& a_rWriter, c_astring& a_sType)
{ return a_rWriter.write_attribute(EMP_RFX_TOKEN_V_TYPE_V1, a_sType); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_name(xml::reader_c& a_rReader, c_astring& a_sName, c_bool a_bLog)
{ return read_token(EMP_RFX_TOKEN_V_NAME_V1, a_rReader, a_sName, a_bLog); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_count(xml::reader_c& a_rReader, size_t& a_rstCount, c_bool a_bLog)
{ return a_rReader.read_const_attribute(EMP_RFX_TOKEN_V_COUNT_V1, a_rstCount, a_bLog); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_file(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, class_c& a_rClass)
{
    EMP_RET_FALSE_IF_NOT(emp::rfx::read_type(a_rReader, a_rClass.id()->m_sSerializableName, true));
    EMP_RET_FALSE_IF_NOT(read_object(a_rReader, a_stDepth, a_sDirectory, a_rClass));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, class_c& a_rClass)
{
    a_rClass.reset_all_read();
    EMP_RET_FALSE_IF_NOT(read_ctor(a_sDirectory, a_rClass));
    EMP_RET_FALSE_IF_NOT(read_value(a_rReader, a_stDepth, a_sDirectory, a_rClass));
    return a_rClass.all_read();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool eat_object(astring& a_rsReader, class_c& a_rClass)
{
    a_rClass.reset_all_read();
    EMP_RET_FALSE_IF_NOT(eat_value(a_rsReader, a_rClass));
    return a_rClass.all_read();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_value(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, class_c& a_rClass)
{
    size_t stRead = 0;

    if (emp::str::cmp(astring(ALITERAL("ase::cor::user_c")), a_rClass.id()->m_sName)) // @@0 remove
    {
        int kk = 0;
        kk++;
    }
    //if (a_rReader.read_type() == node_e::Attribute)
    {
        while (a_rReader.next_valid_attribute())
        {
            astring sName;
            EMP_RET_FALSE_IF_NOT(a_rReader.read_const_name(sName));

            field_s* const pField = a_rClass.field(sName);
            EMP_RET_FALSE_IF(pField == nullptr, "field not found : #0.#1", a_rClass.id()->m_sSerializableName, sName);

            type_i* const pType = pField->m_pType;
            EMP_RET_FALSE_IF(pField == nullptr, "type not found : #0.#1", a_rClass.id()->m_sSerializableName, sName);

            EMP_RET_FALSE_IF_NOT(pType->bind(), "failed to bind field #0 #1.#2",
                pType->id()->m_sSerializableName, a_rClass.id()->m_sSerializableName, pField->m_sName);

            EMP_RET_FALSE_IF_NOT(pType->read_attribute(a_rReader),
                "failed to read field : #0 #1.#2", pType->id()->m_sSerializableName, a_rClass.id()->m_sSerializableName, pField->m_sName);

            pField->m_bRead = true;
            ++stRead;
        }
        // @@0 remove : while (a_rReader.next_valid_attribute());
    }

    size_t EMP_TT_MAX_VAR(stCurrentDepth);
    EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));

    if ((stCurrentDepth + 1) ==  a_stDepth ||
        a_rReader.read_type() != emp::xml::node_e::Element)
    {
        xml::reader_c::c_read_result_e eReturn = a_rReader.next_valid_element();
        EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error,
            "Failed to read next element : #0", a_rClass.id()->m_sName);
        if (eReturn == xml::reader_c::read_result_e::Break)
        { return true; }
    }

    while (true)
    {
        EMP_TT_MAX_VAR(stCurrentDepth);
        EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));

        if (stCurrentDepth == a_stDepth)
        {
            astring sName;
            EMP_RET_FALSE_IF_NOT(a_rReader.read_const_name(sName));

            field_s* pField = a_rClass.field(sName);
            EMP_RET_FALSE_IF(pField == nullptr, "Field not found '#0'", sName);

            type_i* const pType = pField->m_pType;
            EMP_RET_FALSE_IF(pType == nullptr);
            EMP_RET_FALSE_IF_NOT(pType->bind());

            EMP_RET_FALSE_IF_NOT(pType->read_object(a_rReader, a_stDepth + 1, a_sDirectory),
                "Failed to read field : #0 #1;", pType->id()->m_sSerializableName, sName);

           // EMP_RET_FALSE_IF_NOT(pType->post_load(), "Failed to post load : #0", pType->id()->m_sName);

            pField->m_bRead = true;
            ++stRead;

            EMP_RET_FALSE_IF_NOT(a_rReader.read_depth(stCurrentDepth));

            if (stCurrentDepth > a_stDepth ||
                a_rReader.read_type() != emp::xml::node_e::Element)
            {
                xml::reader_c::c_read_result_e eReturn = a_rReader.next_valid_element();
                EMP_RET_FALSE_IF(eReturn == xml::reader_c::read_result_e::Error,
                    "Failed to read next element : #0", a_rClass.id()->m_sName);
                if (eReturn == xml::reader_c::read_result_e::Break)
                { break; }
            }
        }
        else
        { break; }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool eat_value(astring& a_rReader, class_c& a_rClass)
{
    c_size stCount = a_rClass.field_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        EMP_RET_FALSE_IF_NOT(a_rReader.len() > 0);

        field_s* const pField = a_rClass.all_field(stIndex);
        EMP_RET_FALSE_IF(pField == nullptr, "field not found : #0.#1", a_rClass.id()->m_sSerializableName, stIndex);

        type_i* const pType = pField->m_pType;
        EMP_RET_FALSE_IF(pField == nullptr, "type not found : #0.#1", a_rClass.id()->m_sSerializableName, stIndex);

        EMP_RET_FALSE_IF_NOT(pType->bind(), "failed to bind field #0 #1.#2",
            pType->id()->m_sSerializableName, a_rClass.id()->m_sSerializableName, pField->m_sName);

        EMP_RET_FALSE_IF_NOT(pType->eat_object(a_rReader));
        pField->m_bRead = true;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_ctor(c_astring& a_sDirectory EMP_UNUSED, class_c& a_rClass EMP_UNUSED)
{
    xml::reader_c oReader;

    astring sFilename = STR_AFORMAT("#0#1", a_sDirectory, a_rClass.id()->m_sCtorFilename);

    EMP_RET_FALSE_IF_NOT(oReader.create(sFilename)                                    , "Failed to read ctor '#0'", sFilename);
    EMP_RET_FALSE_IF_NOT(oReader.read_element(EMP_RFX_TOKEN_V_STREAM_V1)              , "Failed to read ctor '#0'", sFilename);
    EMP_RET_FALSE_IF_NOT(read_type(oReader, a_rClass.id()->m_sSerializableName, true) , "Failed to read ctor '#0'", sFilename);
    EMP_RET_FALSE_IF_NOT(read_value(oReader, 1, a_sDirectory, a_rClass)               , "Failed to read ctor '#0'", sFilename);
    EMP_RET_FALSE_IF_NOT(a_rClass.read_base_ctor(a_sDirectory)                        , "Failed to read ctor '#0'", sFilename);
    // @@0 ctor object cache
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool write(xml::writer_c& a_rWriter, class_c& a_rClass)
{
    // @@0 remove EMP_RET_FALSE_IF_NOT(write_type(a_rWriter, a_rClass.id()->m_sSerializableName));
    EMP_RET_FALSE_IF_NOT(write_object(a_rWriter, a_rClass));
    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool write_element(xml::writer_c& a_rWriter, class_c& a_rClass)
{
    // @@0 remove EMP_RET_FALSE_IF_NOT(a_rClass.write_object(a_rWriter));
    c_size stCount = a_rClass.field_count();
    if (stCount > 0)
    {
        auto const l_write_field = [& a_rWriter, &a_rClass](c_size a_stIndex) EMP_RETURN -> bool
        {
            field_s* const pField = a_rClass.all_field(a_stIndex);
            type_i* const pType = pField->m_pType;

            EMP_RET_FALSE_IF_NOT(pType->bind());

            c_type_e eType = pType->id()->m_eType;
            EMP_RET_FALSE_IF(eType == type_e::Vector);

            EMP_RET_FALSE_IF_NOT(pType->write_element(a_rWriter));
            return true;
        };

        EMP_RET_FALSE_IF_NOT(l_write_field(0));
        for (size_t stIndex = 1; stIndex < stCount; ++stIndex)
        {
            EMP_RET_FALSE_IF_NOT(a_rWriter.write_raw(U8LITERAL(" ")));
            EMP_RET_FALSE_IF_NOT(l_write_field(stIndex));
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, class_c& a_rClass, c_astring& a_sName)
{
    EMP_RET_FALSE_IF_NOT(a_rClass.is_attribute());
    EMP_RET_FALSE_IF_NOT(a_rClass.field_count() == 1);
    field_s* const pField = a_rClass.all_field(0);
    type_i* const pType = pField->m_pType;
    EMP_RET_FALSE_IF_NOT(pType->bind());
    EMP_RET_FALSE_IF_NOT(pType->write_attribute(a_rWriter, a_sName), "Failed to write : #0 #1;", pType->id()->m_sSerializableName, pField->m_sName);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool write_object(xml::writer_c& a_rWriter, class_c& a_rClass)
{
    // @@0 remove EMP_RET_FALSE_IF_NOT(a_rClass.write_object(a_rWriter));
    c_size stCount = a_rClass.field_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        field_s* const pField = a_rClass.all_field(stIndex);
        type_i* const pType = pField->m_pType;

        EMP_RET_FALSE_IF_NOT(pType->bind());

        if (pType->is_attribute())
        { EMP_RET_FALSE_IF_NOT(pType->write_attribute(a_rWriter, pField->m_sName), "Failed to write : #0 #1;", pType->id()->m_sSerializableName, pField->m_sName); }
    }

    //c_bool bIndent = a_rWriter.indent();

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        field_s* const pField = a_rClass.all_field(stIndex);
        type_i* const pType = pField->m_pType;

        EMP_RET_FALSE_IF_NOT(pType->bind());

        if (pType->is_attribute() == false)
        {
            c_bool bNoIndent =
                emp::str::cmp(pField->m_sName, astring(ALITERAL("data"))) && (stCount == 1) && (pType->id()->m_eType == emp::rfx::type_e::Vector); // @@0 tmp debugging.
            if (bNoIndent) // @@0 remove 
            {
                int k = 0;
                k++;
            }
            EMP_RET_FALSE_IF_NOT(a_rWriter.indent(bNoIndent == false));
            EMP_RET_FALSE_IF_NOT(a_rWriter.start_element(pField->m_sName));

            EMP_RET_FALSE_IF_NOT(pType->write(a_rWriter),
                "Failed to write : #0 #1;", pType->id()->m_sSerializableName, pField->m_sName);

            EMP_RET_FALSE_IF_NOT(a_rWriter.end_element());
            EMP_RET_FALSE_IF_NOT(a_rWriter.indent(true));
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool write_count(xml::writer_c& a_rWriter, c_size a_stCount)
{ return a_rWriter.write_attribute(EMP_RFX_TOKEN_V_COUNT_V1, a_stCount); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool write_name(xml::writer_c& a_rWriter, c_astring& a_sName)
{ return a_rWriter.write_attribute(EMP_RFX_TOKEN_V_NAME_V1, a_sName); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_xml_type(c_astring& a_sType, astring& a_rOut)
{
    a_rOut = a_sType;
    c_size stLen = a_rOut.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        if (a_rOut.get_char(stIndex) == '<')
        { a_rOut.set_char(stIndex, '['); }
        else if (a_rOut.get_char(stIndex) == '>')
        { a_rOut.set_char(stIndex, ']'); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_ctor_type(c_astring& a_sType, astring& a_rOut)
{
    a_rOut.append_literal(ALITERAL("__"));
    char cLast = 0;
    c_size stLen = a_sType.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        c_char cCurrent = a_sType.get_char(stIndex);
        if (cCurrent == ':')
        {
            if (cLast == cCurrent)
            {
                a_rOut.append_char('_');
                cLast = 0;
            }
            else
            { cLast = cCurrent; }
        }
        else if (cCurrent == '<')
        { a_rOut.append_char('['); }
        else if (cCurrent == '>')
        { a_rOut.append_char(']'); }
        else
        {
            a_rOut.append_char(cCurrent);
            cLast = cCurrent;
        }
    }
    a_rOut.append_literal(ALITERAL(".xml"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
factory_c::~factory_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void factory_c::create(c_astring& a_sPath)
{
    destroy();
    m_sPath = a_sPath;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void factory_c::destroy(void)
{
    {
        c_size stCount = m_vpStores.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stOffset = stCount - stIndex - 1;
            store_i* const pStore = m_vpStores.val(stOffset);
            pStore->destroy();
        }
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stOffset = stCount - stIndex - 1;
            store_i* const pStore = m_vpStores.val(stOffset);
            EMP_DEALLOCATE(pStore);
            m_vpStores.set(stOffset, nullptr);
        }
    }

    emp::cnt::foreach(m_vpTypes, emp::mem::safe_delete_reset_t());
    m_vpTypes.resize(0);

    m_vpStores.resize(0);
    m_vsStack.resize(0);

    m_mpStores.clear();

    m_sPath = astring();

    {
        c_size stCount = m_vDealloc.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vDealloc.ref(stIndex)(); }
        m_vDealloc.resize(0);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool factory_c::read(void)
{
    c_size stCount = m_vpStores.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stIndex;//stCount - stIndex - 1;
        EMP_RET_FALSE_IF_NOT(m_vpStores.val(stOffset)->read(m_sPath));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool factory_c::write(c_bool a_bAll)
{
    if (readonly)
    { return true; }

    c_size stCount = m_vpStores.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(m_vpStores.val(stIndex)->write(m_sPath, a_bAll)); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool factory_c::instanciate(c_bool a_bCheck)
{
    vector<astring> vsFiles;
    vector<astring> vsNames;
    {
        EMP_RET_FALSE_IF_NOT(emp::fs::list(m_sPath, ALITERAL("*.xml"), vsFiles, false),
            "failed to list directory : #0", m_sPath);

        c_size stCount = vsFiles.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            // @@0 alloc
            c_astring sFilename = vsFiles.ref(stIndex);
            vsFiles.set(stIndex, STR_AFORMAT("#0#1", m_sPath, sFilename));
            vsNames.add(sFilename);
        }
    }

    {
        c_size stCount = m_vpStores.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(m_vpStores.val(stIndex)->instanciate(vsFiles, vsNames)); }
    }

    if (a_bCheck)
    {
        for (size_t stIndex = 0; stIndex < vsFiles.count();)
        {
            c_astring& sFilename = vsFiles.ref(stIndex);
            emp::xml::reader_c oReader;
            EMP_RET_FALSE_IF_NOT(oReader.create(sFilename));
            EMP_RET_FALSE_IF_NOT(oReader.read_element(EMP_RFX_TOKEN_V_STREAM_V1));
            astring sType;
            EMP_RET_FALSE_IF_NOT(oReader.read_const_attribute(EMP_RFX_TOKEN_V_TYPE_V1, sType, true));

            bool bRemoved = false;
            c_size stStoreCount = m_vpStores.count();
            for (size_t stStore = 0; stStore < stStoreCount; ++stStore)
            {
                //EMP_LOG_DEBUG("Comparing : '#0' with '#1'", sType, m_vpStores.ref(stStore)->id()->m_sSerializableName);
                if (emp::str::cmp(sType, m_vpStores.ref(stStore)->id()->m_sSerializableName))
                {
                    vsFiles.remove_unordered(stIndex);
                    vsNames.remove_unordered(stIndex);
                    bRemoved = true;
                    break;
                }
            }
            if (bRemoved)
            { continue; }

            c_astring& sName = vsNames.ref(stIndex);
            if (sName.get_first_char() == '_')
            {
                vsFiles.remove_unordered(stIndex);
                vsNames.remove_unordered(stIndex);
                bRemoved = true;
            }
            else
            { EMP_LOG_ERROR("Unknown store_t<#0> (#1)", sType, sFilename); }
            if (bRemoved)
            { continue; }
            ++stIndex;
        }

        if (vsFiles.count() > 0 || vsNames.count() > 0)
        {
            for (size_t stIndex = 0; stIndex < vsFiles.count(); ++stIndex)
            { EMP_LOG_ERROR("vsFiles #0", vsFiles.ref(stIndex)); }
            for (size_t stIndex = 0; stIndex < vsNames.count(); ++stIndex)
            { EMP_LOG_ERROR("vsNames #0", vsNames.ref(stIndex)); }
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool factory_c::pre_load(void)
{
    /*c_size stCount = m_vpStores.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(m_vpStores.val(stIndex)->pre_load()); }*/
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool factory_c::post_load(void)
{
    c_size stCount = m_vpStores.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(m_vpStores.val(stIndex)->post_load()); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool factory_c::reflect(type_i& a_rType)
{
    bool bFound = false;
    c_astring& sType = a_rType.id()->m_sName;
    c_size stCount = m_vsStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (emp::str::cmp(m_vsStack.ref(stIndex), sType))
        {
            bFound = true;
            break;
        }
    }

    if (bFound == false)
    {
        m_vsStack.add(sType);
        EMP_RET_FALSE_IF_NOT(a_rType.reflect(*this));
        m_vsStack.resize(stCount);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

