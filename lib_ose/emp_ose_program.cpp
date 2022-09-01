//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_program.h"
#include "emp_ose_compiler.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"
#include "emp_mat_random.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
table_c::table_c(void):
    m_vmOverloads(64),
    m_vmLexemes(128),
    m_vmTypes(128),
    m_vpDeleteLexemes(),
    m_vpDeleteOverloads(),
    m_vpDeleteTypes(),
    m_sEmpty()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
table_c::~table_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool table_c::create(void)
{
    destroy();

    {
        auto pType = new_type<rti::void_c>(ALITERAL(""));
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF(pType->create() == false);
        EMP_RET_FALSE_IF_NOT(reg_type(pType));
        m_pVoidType = pType;
    }

    {
        auto pType = new_type<rti::data_c>(ALITERAL("float"));
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF(pType->create(rti::data_e::Real) == false);
        EMP_RET_FALSE_IF_NOT(reg_type(pType));
        m_pRealType = pType;
    }

    {
        auto pType = new_type<rti::data_c>(ALITERAL("uint"));
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF(pType->create(rti::data_e::UInt) == false);
        EMP_RET_FALSE_IF_NOT(reg_type(pType));
        m_pUIntType = pType;
    }

    {
        auto pType = new_type<rti::data_c>(ALITERAL("int"));
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF(pType->create(rti::data_e::SInt) == false);
        EMP_RET_FALSE_IF_NOT(reg_type(pType));
        m_pSIntType = pType;
    }

    {
        auto pType = new_type<rti::data_c>(ALITERAL("bool"));
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF(pType->create(rti::data_e::eBool) == false);
        EMP_RET_FALSE_IF_NOT(reg_type(pType));
        m_pBoolType = pType;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void table_c::mother(c_bool a_bValue)
{
    for (size_t stIndex = 0; stIndex < m_vmLexemes.vector().count(); ++stIndex)
    { m_vmLexemes.vector().val(stIndex)->m_bMother = a_bValue; }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//void table_c::cold(c_bool a_bValue)
//{
//    for (size_t stIndex = 0; stIndex < m_vmLexemes.vector().count(); ++stIndex)
//    { m_vmLexemes.vector().val(stIndex)->m_bCold = a_bValue; }
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool table_c::merge(table_c& a_rFrom)
{
    for (auto it = a_rFrom.m_vmLexemes.map().begin(); it != a_rFrom.m_vmLexemes.map().end(); ++it)
    {
        lexeme_c* const pLexeme = *it.value();

        c_lexeme_e eLexeme = pLexeme->m_eType;
        if (eLexeme == lexeme_e::Function ||
            eLexeme == lexeme_e::Variant ||
            eLexeme == lexeme_e::Data)
        {
            size_t stIndex = 0;
            while (true)
            {
                //if (m_vmLexemes.map().get(pLexeme->qualified()) != m_vmLexemes.map().end())
                if (m_vmOverloads.map().get(pLexeme->m_sName) != m_vmOverloads.map().end())
                {
                    astring sNewName;
                    emp::tt::silent(STR_FORMATA(sNewName, "#0#1", pLexeme->m_sName, stIndex));
                    pLexeme->reassign(sNewName, index());
                }
                else
                { 
                    pLexeme->reassign(pLexeme->m_sName, index());
                    break;
                }
            }

            EMP_RET_FALSE_IF(reg_lexeme(pLexeme) == nullptr);
        }
        else if (m_vmLexemes.map().get(pLexeme->m_sQualified) == m_vmLexemes.map().end())
        {
            pLexeme->reassign(pLexeme->m_sName, index());
            EMP_RET_FALSE_IF(reg_lexeme(pLexeme) == nullptr);
        }
        else
        {
        }
    
        for (size_t stIndex = 0; stIndex < a_rFrom.m_vpDeleteLexemes.count(); ++stIndex)
        {
            if (a_rFrom.m_vpDeleteLexemes.val(stIndex) == pLexeme)
            {
                a_rFrom.m_vpDeleteLexemes.remove_unordered(stIndex);
                m_vpDeleteLexemes.add(pLexeme);
                break;
            }
        }
    }

    for (auto it = a_rFrom.m_vmTypes.map().begin(); it != a_rFrom.m_vmTypes.map().end(); ++it)
    {
        astring const& sName = *it.key();
        if (m_vmTypes.map().get(sName) == m_vmTypes.map().end())
        {
            rti::type_c* const pType = *it.value();
            EMP_RET_FALSE_IF_NOT(m_vmTypes.add(sName, pType));
            for (size_t stIndex = 0; stIndex < a_rFrom.m_vpDeleteTypes.count(); ++stIndex)
            {
                if (a_rFrom.m_vpDeleteTypes.val(stIndex) == pType)
                {
                    a_rFrom.m_vpDeleteTypes.remove_unordered(stIndex);
                    m_vpDeleteTypes.add(pType);
                    break;
                }
            }
        }
    }

    return true;
}

EMP_RETURN bool table_c::clone(table_c const& a_rFrom)
{
    destroy();

    m_vmLexemes.copy(a_rFrom.m_vmLexemes);
    m_vmOverloads.copy(a_rFrom.m_vmOverloads);
    m_vmTypes.copy(a_rFrom.m_vmTypes);

    m_pVoidType = a_rFrom.m_pVoidType;
    EMP_RET_FALSE_IF(m_pVoidType == nullptr);

    m_pRealType = a_rFrom.m_pRealType;
    EMP_RET_FALSE_IF(m_pRealType == nullptr);

    m_pUIntType = a_rFrom.m_pUIntType;
    EMP_RET_FALSE_IF(m_pUIntType == nullptr);

    m_pSIntType = a_rFrom.m_pSIntType;
    EMP_RET_FALSE_IF(m_pSIntType == nullptr);

    m_pBoolType = a_rFrom.m_pBoolType;
    EMP_RET_FALSE_IF(m_pBoolType == nullptr);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void table_c::destroy(void)
{
    m_vmLexemes.clear();
    m_vmOverloads.clear();
    m_vmTypes.clear();

    emp::cnt::delete_null_resize(m_vpDeleteLexemes);
    emp::cnt::delete_null_resize(m_vpDeleteOverloads);
    emp::cnt::delete_null_resize(m_vpDeleteTypes);

    m_pVoidType = nullptr;
    m_pRealType = nullptr;
    m_pUIntType = nullptr;
    m_pSIntType = nullptr;
    m_pBoolType = nullptr;

    m_stDataAddress = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool table_c::qualify_type(astring& a_rsName, rti::const_type_v const& a_vpTypes) const
{
    a_rsName.append_char('(');
    size_t stVoid = 0;
    c_size stCount = a_vpTypes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        rti::type_c const* const pType = a_vpTypes.val(stIndex);
        EMP_RET_FALSE_IF(get_type(pType->m_sName) == nullptr);
        stVoid += pType == m_pVoidType ? 1 : 0;
        EMP_RET_FALSE_IF(stVoid > 1);
        a_rsName.append_string(pType->m_sName);
        a_rsName.append_char(',');
    }
    a_rsName.append_char(')');
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool table_c::qualify_type(rti::function_c* const a_pType) const
{
    EMP_RET_FALSE_IF(a_pType == nullptr);

    astring sName;
    EMP_RET_FALSE_IF_NOT(qualify_type(sName, a_pType->m_vpReturns));
    EMP_RET_FALSE_IF_NOT(qualify_type(sName, a_pType->m_vpParameters));
    EMP_RET_FALSE_IF_NOT(rti::type_c::create(a_pType, sName));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t table_c::get_variant(emp::mat::random_float_s& a_rRandom, c_ui32 a_ui32Call) const
{
    auto const& vpLexemes = m_vmLexemes.vector();
    c_size stCount = vpLexemes.count();
    if (a_ui32Call < stCount)
    {
        variant_c const* const pVariant = to_function<variant_c const>(vpLexemes.val(a_ui32Call));
        if (pVariant != nullptr)
        { return pVariant->random(a_rRandom); }
    }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_astring& table_c::get_name_from_index(c_ui32 a_ui32Call) const
{
    lexeme_c const* const pLexeme = get_lexeme(a_ui32Call);
    return pLexeme != nullptr ? pLexeme->m_sName : m_sEmpty;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_astring& table_c::get_name_from_instruction(c_instruction_e a_eInstruction) const
{
    auto const& rvpLexemes = m_vmLexemes.vector();
    c_size stCount = rvpLexemes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        lexeme_c const* const pLexeme = rvpLexemes.val(stIndex);
        if (get_instruction(pLexeme) == a_eInstruction)
        { return pLexeme->m_sName; }
    }
    return m_sEmpty;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_astring& program_c::get_name_from_address(c_ui32 a_ui32Address) const
{
    auto const oIt = m_mui32IndexFromAddress.get(a_ui32Address);
    if (oIt != m_mui32IndexFromAddress.end())
    { return m_Table.get_name_from_index(*oIt.value()); }
    return m_sEmpty;
}

void program_c::touch_variant_from_index(c_ui32 a_ui32Call)
{
    lexeme_c* const pLexeme = m_Table.get_lexeme(a_ui32Call);
    pLexeme->m_bCold = false;
}

void program_c::touch_function_from_address(c_ui32 a_ui32Address)
{
    lexeme_c* const pLexeme = m_Table.get_lexeme(get_index_from_address(a_ui32Address));
    pLexeme->m_bCold = false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui32 program_c::get_index_from_address(c_ui32 a_ui32Address) const
{
    auto const oIt = m_mui32IndexFromAddress.get(a_ui32Address);
    if (oIt != m_mui32IndexFromAddress.end())
    { return *oIt.value(); }
    return emp::tt::max<ui32>::value;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN rti::type_c const* table_c::get_type(c_astring& a_sName) const
{
    if (a_sName.len() == 0)
    { return m_pVoidType; }
    auto const oIt = m_vmTypes.map().get(a_sName);
    if (oIt != m_vmTypes.map().end())
    { return *oIt.value(); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN lexeme_c* table_c::get_lexeme(c_ui32 a_ui32Call) const
{ return (a_ui32Call < m_vmLexemes.count()) ? m_vmLexemes.vector().val(a_ui32Call) : nullptr; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN lexeme_c* table_c::get_lexeme(c_astring& a_sCall) const
{
    EMP_RET_NULL_IF_NOT((a_sCall.len() > 0));
    auto const oIt = m_vmLexemes.map().get(a_sCall);
    return oIt != m_vmLexemes.map().end() ? *oIt.value() : nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T0, typename T1>
EMP_RETURN bool match(T0 const& a_rFrom, T1 const& a_rTo);
template <typename T0, typename T1>
EMP_RETURN bool match(T0 const& a_rFrom, T1 const& a_rTo)
{
    c_size stCount = a_rTo.count();
    EMP_RET_FALSE_IF(stCount != a_rFrom.count());
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF(a_rTo.ref(stIndex).type() != a_rFrom.ref(stIndex).type()); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN lexeme_c* table_c::get_lexeme(c_astring& a_sCall, rti::type_c const* const a_pType) const
{
    EMP_RET_NULL_IF(a_pType == nullptr);

    auto const oIt = m_vmOverloads.map().get(a_sCall);
    if (oIt == m_vmOverloads.map().end())
    { return nullptr; }

    auto const& rVector = (*oIt.value())->vector();
    c_size stCount = rVector.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        lexeme_c* const pLexeme = rVector.val(stIndex);
        if (pLexeme == nullptr)
        { continue; }

        EMP_OSE_IF_DEBUG_FULL(EMP_LOG_INFO("get_lexeme #0, #1", a_sCall, pLexeme->qualified_name()); )

        if (a_pType == pLexeme->m_pType)
        { return pLexeme; }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN vector_map_t<astring, lexeme_c*>* table_c::get_overload(c_astring& a_sCall) const
{
    auto const oIt = m_vmOverloads.map().get(a_sCall);
    return oIt != m_vmOverloads.map().end() ? *oIt.value() : nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN lexeme_c* table_c::get_match(c_astring& a_sCall) const
{
    auto* const pOverload = get_overload(a_sCall);
    if (pOverload != nullptr &&
        pOverload->count() > 0)
    { return pOverload->vector().val(0); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_INSTANCIATION(x_Type, x_Rti, x_Format)\
template <>\
EMP_RETURN literal_t<x_Type>* table_c::reg_literal<x_Type>(c_astring& a_sName)\
{\
    auto const pLiteral = to_literal<x_Type>(get_lexeme(a_sName, x_Rti));\
    if (pLiteral != nullptr)\
    { return pLiteral; }\
    auto const pLexeme = new_lexeme_sp<literal_t<x_Type>>(a_sName, x_Rti);\
    EMP_RET_NULL_IF(pLexeme == nullptr);\
    if (pLexeme->create() == false) { return nullptr; }\
    return reg_lexeme(pLexeme);\
}\
template <>\
EMP_RETURN literal_t<x_Type>* table_c::reg_literal<x_Type>(x_Type const& a_tValue)\
{\
    astring sName;\
    emp::tt::silent(STR_FORMATA(sName, "#0", x_Format));\
    auto const pLiteral = to_literal<x_Type>(get_lexeme(sName, x_Rti));\
    if (pLiteral != nullptr)\
    { return pLiteral; }\
    auto const pLexeme = new_lexeme_sp<literal_t<x_Type>>(sName, x_Rti);\
    EMP_RET_NULL_IF(pLexeme == nullptr);\
    if (pLexeme->create(a_tValue) == false) { return nullptr; }\
    return reg_lexeme(pLexeme);\
}\
template <>\
EMP_RETURN literal_t<x_Type>* table_c::reg_literal<x_Type>(c_astring& a_sName, x_Type const& a_tValue)\
{\
    astring sValue;\
    emp::tt::silent(STR_FORMATA(sValue, "#0", x_Format)); \
    auto const pLiteral = to_literal<x_Type>(get_lexeme(a_sName, x_Rti));\
    if (pLiteral != nullptr)\
    { return pLiteral; }\
    auto const pLexeme = new_lexeme_sp<literal_t<x_Type>>(a_sName, x_Rti);\
    EMP_RET_NULL_IF(pLexeme == nullptr);\
    if (pLexeme->create(a_tValue) == false) { return nullptr; }\
    return reg_lexeme(pLexeme);\
}\
template literal_t<x_Type> const* to_literal_impl<literal_t<x_Type> const, lexeme_c const>(lexeme_c const*);
// @@0 literal
XTMP_INSTANCIATION(real, m_pRealType, REAL_FMT_R6(a_tValue)) // @@0 wtf why R0 ???
XTMP_INSTANCIATION(sint, m_pSIntType, static_cast<sint>(a_tValue))
XTMP_INSTANCIATION(uint, m_pUIntType, static_cast<uint>(a_tValue))
XTMP_INSTANCIATION(bool, m_pBoolType, static_cast<bool>(a_tValue))
#undef XTMP_INSTANCIATION

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool table_c::link(void) const
{
    auto const& rVector = m_vmLexemes.vector();
    c_size stCount = rVector.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        lexeme_c const* const pLexeme = rVector.val(stIndex);
        if (pLexeme == nullptr ||
            pLexeme->m_bCold)
        { continue; }

        c_lexeme_e eLexeme = pLexeme->m_eType;
        if (eLexeme == lexeme_e::Function)
        {
            EMP_OSE_IF_DEBUG(
                EMP_LOG_INFO("function #0", pLexeme->m_sName);
            )
            function_c const* const pFunction = to_function<function_c const>(pLexeme);
            EMP_RET_FALSE_IF(pFunction == nullptr || pFunction->link() == false);
        }
        else if (eLexeme == lexeme_e::Variant)
        {
            EMP_OSE_IF_DEBUG(
                EMP_LOG_INFO("variant #0", pLexeme->m_sName);
            )
            variant_c const* const pVariant = to_function<variant_c>(pLexeme);
            EMP_RET_FALSE_IF(pVariant == nullptr || pVariant->link() == false);
        }

    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
program_c::program_c(void):
    m_Table(),
    m_mSlotFromIndex(128),
    m_mui32IndexFromAddress(128),
    m_mui32Calls(128),
    m_pRandom{}
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
program_c::~program_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t program_c::find_node(compiler_c const* const a_pCompiler, lexeme_c const* const a_pLexeme)
{
    EMP_RET_MAX_IF(size_t, a_pLexeme == nullptr);

    c_size stCount = m_pvstNodes->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stNode = m_pvstNodes->val(stIndex);
        if (emp::tt::is_not_max(stNode) && a_pCompiler->m_vNodes.ref(stNode).m_pLexeme == a_pLexeme)
        { return stNode; }
    }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool program_c::instruction(c_size a_stIndex, char& a_rcInstruction) const
{ return m_Instructions.val(a_stIndex, a_rcInstruction); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool program_c::set_entry_point(c_size a_stEntry)
{
    if (emp::tt::is_max(m_stEntryPoint))
    { m_stEntryPoint = a_stEntry; }
    else
    {
        EMP_RET_FALSE_IF(a_stEntry >= emp::tt::max<ui32>::value);
        c_ui32 u32Entry = emp::ast::cast<ui32>(a_stEntry);

        EMP_RET_FALSE_IF(m_stEndOfBoot >= emp::tt::max<ui32>::value);
        c_ui32 u32Boot = emp::ast::cast<ui32>(m_stEndOfBoot);

        EMP_RET_FALSE_IF(set_data(u32Boot, u32Entry) == false);
    }

    EMP_RET_FALSE_IF_NOT(add_instruction(instruction_e::Jump));
    m_stEndOfBoot = exit_point();
    add_data(emp::tt::max<ui32>::value);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool program_c::add_instruction(c_instruction_e a_eValue)
{ return m_Instructions.push_ex<ui8>(enum_to_type(a_eValue, ui8)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void program_c::clear(void)
{
    EMP_TT_MAX_VAR(m_stGen);
    m_stCurrentGen = 0;

    m_sEmpty = astring();
    m_sFilename = astring();
    m_sExpression = astring();
    EMP_TT_MAX_VAR(m_stEntryPoint);
    EMP_TT_MAX_VAR(m_stEndOfBoot);
    EMP_TT_MAX_VAR(m_stAst);

    m_Table.destroy();
    m_Instructions.clear();
    m_Data.clear();

    m_mSlotFromIndex.clear();
    m_mui32IndexFromAddress.clear();
    m_mui32Calls.clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool program_c::add_definition(c_ui32 a_ui32Index)
{
    c_ui32 ui32Address = emp::ast::cast<ui32>(m_Instructions.count());

    lexeme_c* const pLexeme = m_Table.get_lexeme(a_ui32Index);
    EMP_RET_FALSE_IF(pLexeme == nullptr);

    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("ADD_DEFINITION, name '#0', index #1", pLexeme->m_sName, a_ui32Index);)

    c_lexeme_e eLexeme = pLexeme->m_eType;
    if (eLexeme == lexeme_e::Variant)
    {
        variant_c* const pVariant = to_function<variant_c>(pLexeme);
        EMP_RET_FALSE_IF(pVariant == nullptr);
        EMP_RET_FALSE_IF_NOT(pVariant->add_address(ui32Address));
        slot_s oSlot;
        oSlot.variant = true;
        oSlot.address = 0xFF00FF00;
        m_mSlotFromIndex.set(a_ui32Index, oSlot);
        return true;
    }
    else if (eLexeme == lexeme_e::Function)
    {
        function_c* const pFunction = to_function<function_c>(pLexeme);
        EMP_RET_FALSE_IF(pFunction == nullptr);
        pFunction->m_ui32Address = ui32Address;

        EMP_RET_FALSE_IF(m_mSlotFromIndex.get(a_ui32Index) != m_mSlotFromIndex.end());
        slot_s oSlot;
        oSlot.variant = false;
        oSlot.address = ui32Address;
        m_mSlotFromIndex.set(a_ui32Index, oSlot);

        EMP_RET_FALSE_IF(m_mui32IndexFromAddress.get(ui32Address) != m_mui32IndexFromAddress.end());
        m_mui32IndexFromAddress.set(ui32Address, a_ui32Index);
        return true;
    }

    EMP_LOG_ERROR("not a definition");
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <instruction_e t_eCall>
EMP_RETURN bool program_c::add_call_impl(c_ui32 a_ui32Index)
{
    EMP_RET_FALSE_IF_NOT(add_instruction(t_eCall));

    lexeme_c const* const pLexeme = m_Table.get_lexeme(a_ui32Index);
    EMP_RET_FALSE_IF(pLexeme == nullptr);

    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("name '#0', index #1", pLexeme->m_sName, a_ui32Index);)

    c_ui32 ui32Address = emp::ast::cast<ui32>(m_Instructions.count());
    address_m::iterator_c const oIt = m_mui32Calls.get(ui32Address);
    EMP_RET_FALSE_IF(oIt != m_mui32Calls.end(), "duplicate address #0", ui32Address);

    m_mui32Calls.set(ui32Address, a_ui32Index);
    add_data(0xADADADAD);
    return true;
}

template bool program_c::add_call_impl<instruction_e::Function>(c_ui32);
template bool program_c::add_call_impl<instruction_e::Variant>(c_ui32);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool program_c::add_data(lexeme_c const* const a_pLexeme)
{
    EMP_RET_FALSE_IF(a_pLexeme == nullptr);
    EMP_RET_FALSE_IF(a_pLexeme->m_eType != lexeme_e::Literal);

    rti::data_c const* const pType = rti::to_type<rti::data_c>(a_pLexeme->m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);

    rti::c_data_e eData = pType->m_eData;
    switch (eData)
    {
#define XTMP_ADD_LIT(x_Data, x_Type, x_Instruction)\
        case rti::data_e::x_Data:\
        {\
            literal_t<x_Type> const* const pLiteral = to_literal<x_Type>(a_pLexeme);\
            EMP_RET_FALSE_IF(pLiteral == nullptr);\
            EMP_RET_FALSE_IF(add_instruction(instruction_e::x_Instruction) == false);\
            add_data(pLiteral->m_tLiteral);\
            return true;\
        }
        // @@0 literal
        XTMP_ADD_LIT(Real  , real, PushReal)
        XTMP_ADD_LIT(UInt  , uint, PushUInt)
        XTMP_ADD_LIT(SInt  , sint, PushSInt)
        XTMP_ADD_LIT(eBool , bool, PushBool)
#undef XTMP_ADD_LIT
        case rti::data_e::none:
        default:
        {
            EMP_LOG_ERROR("invalid literal_e::#0, #1", enum_to_astring(eData), enum_to(eData));
            return false;
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool program_c::link(c_bool a_bBlob)
{
    c_ui32 ui32Count = emp::ast::cast<ui32>(m_Instructions.count());
    for (address_m::iterator_c oIt = m_mui32Calls.begin(); oIt != m_mui32Calls.end(); ++oIt)
    {
        c_ui32 ui32Index = *oIt.value();
        auto const oItAdd = m_mSlotFromIndex.get(ui32Index);
        EMP_RET_FALSE_IF(oItAdd == m_mSlotFromIndex.end());

        slot_s const& oSlot = *oItAdd.value();
        c_bool bVariant = oSlot.variant;
        c_ui32 ui32Address = oSlot.address;

        EMP_RET_FALSE_IF(bVariant && (ui32Address != 0xFF00FF00));
        EMP_RET_FALSE_IF(bVariant == false && (ui32Address >= ui32Count));

        c_ui32 ui32Offset = *oIt.key();

        ui32 ui32Instruction = 0;
        EMP_RET_FALSE_IF_NOT(m_Instructions.val(ui32Offset, ui32Instruction));
        EMP_RET_FALSE_IF(ui32Instruction != 0xADADADAD);

        ui32Instruction = bVariant ? ui32Index : ui32Address;
        EMP_RET_FALSE_IF_NOT(m_Instructions.set(ui32Offset, ui32Instruction));
    }

    if (a_bBlob == false)
    {
        function_c const* const pMain = m_Table.get_function(ALITERAL("main()()"));
        EMP_RET_FALSE_IF(pMain == nullptr);

        auto const oIt = m_mSlotFromIndex.get(pMain->m_ui32Index);
        EMP_RET_FALSE_IF(oIt == m_mSlotFromIndex.end());

        EMP_RET_FALSE_IF_NOT(set_entry_point((*oIt.value()).address));
    }

    EMP_RET_FALSE_IF(m_Table.link() == false);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

