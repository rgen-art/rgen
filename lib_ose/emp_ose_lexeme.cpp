//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_lexeme.h"

#include "emp_mat_random.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN astring lexeme_c::compute_qualified_name(c_astring& a_sName) const
{
    astring sQualified = a_sName;
    if ( m_pType->m_eType == rti::type_e::Data)
    { sQualified.append_char(' '); }
    sQualified.append_string(m_pType->m_sName);
    return sQualified;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void lexeme_c::reassign(c_astring& a_sName, c_ui32 a_ui32Index)
{
    m_ui32Index = a_ui32Index;
    m_sName = a_sName;
    m_sQualified = compute_qualified_name(m_sName);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void lexeme_c::rename(c_astring& a_sName)
{
    m_bRenamed = true;
    reassign(a_sName, m_ui32Index);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool comment_c::create(c_comment_e a_eComment)
{
    EMP_RET_FALSE_IF(a_eComment == comment_e::none);
    EMP_RET_FALSE_IF_NOT(enum_is_valid(a_eComment));
    m_eCommentType = a_eComment;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool punctuation_c::create(c_punctuation_e a_ePunctuation, c_ui32 a_ui32Precedence)
{
    EMP_RET_FALSE_IF(a_ePunctuation == punctuation_e::none);
    EMP_RET_FALSE_IF_NOT(enum_is_valid(a_ePunctuation));
    m_ePunctuationType = a_ePunctuation;

    m_ui32Precedence = a_ui32Precedence;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool operation_c::create(c_instruction_e a_eInstruction, c_ui32 a_ui32Precedence, c_bool a_bLeftToRight, c_bool a_bReserved)
{
    rti::function_c const* const pType = rti::to_type<rti::function_c const>(m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);
    m_pFunctionType = pType;

    EMP_RET_FALSE_IF(a_eInstruction == instruction_e::none);
    EMP_RET_FALSE_IF_NOT(enum_is_valid(a_eInstruction));
    m_eInstruction = a_eInstruction;

    m_ui32Precedence = a_ui32Precedence;
    m_bLeftToRight = a_bLeftToRight;
    m_bReserved = a_bReserved;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool intrinsic_c::create(c_instruction_e a_eInstruction, c_bool a_bReserved)
{
    rti::function_c const* const pType = rti::to_type<rti::function_c const>(m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);
    m_pFunctionType = pType;

    EMP_RET_FALSE_IF(a_eInstruction == instruction_e::none);
    EMP_RET_FALSE_IF_NOT(enum_is_valid(a_eInstruction));
    m_eInstruction = a_eInstruction;
    m_bReserved = a_bReserved;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool control_c::create(c_control_e a_eControl, c_bool a_bReserved)
{
    rti::function_c const* const pType = rti::to_type<rti::function_c const>(m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);
    m_pFunctionType = pType;

    EMP_RET_FALSE_IF(a_eControl == control_e::none);
    EMP_RET_FALSE_IF_NOT(enum_is_valid(a_eControl));
    m_eControlType = a_eControl;
    m_bReserved = a_bReserved;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool function_c::create(void)
{
    rti::function_c const* const pType = rti::to_type<rti::function_c const>(m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);
    m_pFunctionType = pType;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool variant_c::create(void)
{
    rti::function_c const* const pType = rti::to_type<rti::function_c const>(m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);
    m_pFunctionType = pType;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t variant_c::random(emp::mat::random_float_s& a_rRandom) const
{
    //TODO:RG c_float fRandom = a_rRandom.rand<float>(0, m_fSum);
    c_float fRandom = a_rRandom.value() * m_fSum;
    float fWeight = 0;
    c_size stCount = m_vSlots.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        slot_s const& rSlot = m_vSlots.ref(stIndex);
        fWeight += rSlot.weight;
        if (fRandom <= fWeight)
        { return rSlot.address; }
    }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void variant_c::add(float const a_fWeight)
{
    m_fSum += a_fWeight;
    slot_s& rSlot = m_vSlots.push();
    rSlot.weight = a_fWeight;
    EMP_TT_MAX_VAR(rSlot.address);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool variant_c::add_address(c_ui32 a_ui32Address)
{
    if (m_stLink < m_vSlots.count())
    {
        m_vSlots.ref(m_stLink).address = a_ui32Address;
        ++m_stLink;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void variant_c::reassign(c_astring& a_sName, c_ui32 a_ui32Index)
{
    tbase1::reassign(a_sName, a_ui32Index);

    m_stLink = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool variant_c::link(void) const
{
    c_size stCount = m_vSlots.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF(emp::tt::is_max(m_vSlots.ref(stIndex).address)); }
    EMP_RET_FALSE_IF_NOT(m_stLink == stCount && stCount > 0);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool data_c::create(void)
{
    rti::data_c const* const pType = rti::to_type<rti::data_c const>(m_pType);
    EMP_RET_FALSE_IF(pType == nullptr);
    m_pDataType = pType;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

