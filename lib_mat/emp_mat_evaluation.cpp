//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_mat_evaluation.h"

#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using tool_d = emp::mat::simd_v4<float>;
using vector_d = typename tool_d::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void testdiv(eval_stack_t<float>& a_rStack);
void testdiv(eval_stack_t<float>& a_rStack)
{
    //c_ui32 ui32Arity = a_rStack.node_arity();
    vector_d* const pOutput = a_rStack.node_output();
    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::div(\
    pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_DIV_ARITY)],\
    pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_DIV_ARITY)]);
                        EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
    a_rStack.flush_node_output(div_d::result);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool and_impl(c_bool a_bLeft, c_bool a_bRight);
EMP_RETURN bool and_impl(c_bool a_bLeft, c_bool a_bRight)
{ return a_bLeft & a_bRight; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool rd_impl(c_bool a_bLeft, c_bool EMP_SILENT(a_bRight));
EMP_RETURN bool rd_impl(c_bool a_bLeft, c_bool EMP_SILENT(a_bRight))
{ return a_bLeft; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool xor_impl(c_bool a_bLeft, c_bool a_bRight);
EMP_RETURN bool xor_impl(c_bool a_bLeft, c_bool a_bRight)
{ return a_bLeft ^ a_bRight; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool or_impl(c_bool a_bLeft, c_bool a_bRight);
EMP_RETURN bool or_impl(c_bool a_bLeft, c_bool a_bRight)
{ return a_bLeft | a_bRight; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int functor::operator()(expression const& a_rBinary, c_astring&, c_int a_siLeft, c_astring&, c_int a_siRight,
    c_operator_e a_eOperator) const
{ return a_rBinary.evaluate(a_eOperator, a_siLeft, a_siRight); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int functor::operator()(expression const& EMP_SILENT(a_rBinary), c_astring& a_sPointer) const
{
    EMP_ASSERT(a_sPointer.len() == 1);

    c_char cValue = a_sPointer.get_first_char();
    if (cValue == emp::str::zero<char>::value)
    { return 0; }
    else if (cValue == emp::str::one<char>::value)
    { return 1; }

    // @@0 :char_fmt / str_fmt
   //EMP_LOG_ERROR("Unknown result : '#0'", astring(cValue));
    return -1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
expression::expression(void):
    m_acOperators(),
    m_apfnOperators()
{
// @@2 to_array
#define enum_name EMP_EVL_OPERATOR_E_NAME
#define enum_values EMP_EVL_OPERATOR_E_VALUES
#define enum_value_ex_impl(x_Name, x_Value, x_Function) m_acOperators.set<enum_to__(operator_e::x_Name)>(x_Value);
#include "emp_tt_enum_ex.def.h"

// @@2 to_array
#define enum_name EMP_EVL_OPERATOR_E_NAME
#define enum_values EMP_EVL_OPERATOR_E_VALUES
#define enum_value_ex_impl(x_Name, x_Value, x_Function) m_apfnOperators.set<enum_to__(operator_e::x_Name)>(x_Function);
#include "emp_tt_enum_ex.def.h"
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool expression::is_reserved(c_char a_cChar) const
{
    return  emp::str::is_left_parenthesis(a_cChar) ||
            emp::str::is_right_parenthesis(a_cChar) ||
            m_acOperators.contains(a_cChar);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool expression::is_not_reserved(c_char a_cChar) const
{ return is_reserved(a_cChar) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int expression::evaluate(c_operator_e a_eOperator, c_int a_siLeft, c_int a_siRight) const
{
    //  @@0 remove   return  a_siLeft < 0 || a_siRight < 0 ? -1 : m_apfnOperators.val(enum_to(a_eOperator))(a_siLeft, a_siRight);
    if (a_siLeft < 0)
    { return -1; } // @@0 bug

    if (a_siRight < 0)
    { return -1; }

    return m_apfnOperators.val(enum_to(a_eOperator))(a_siLeft == 0, a_siRight == 0) ? 1 : 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool validate_string(c_astring& a_sValue, astring* const a_psOut);
EMP_INTERN EMP_RETURN bool validate_string(c_astring& a_sValue, astring* const a_psOut)
{
    if (a_sValue.len() > 20000)
    {
        EMP_LOG_ERROR("expression size limit reached");
        return false;
    }

    if (a_sValue.len() == 0)
    {
        EMP_LOG_ERROR("invalid empty string");
        return false;
    }

    astring sExpr(a_sValue);
    astring sCopy; //@@perf sCopy.resize(a_sValue.len());

    while (sExpr.len())
    {
        c_char cChar = sExpr.get_first_char();
        sExpr.trim_left();
        if (emp::str::is_whitespace(cChar) ||
            emp::str::is_newline(cChar))
        { continue; }

        sCopy.append_char(cChar);
        if (emp::str::is_lower_alpha(cChar) == false &&
            emp::str::is_numeric(cChar) == false &&
            emp::str::is_operator(cChar) == false)
        {
            EMP_LOG_ERROR("invalid char #0", sCopy);
            return false;
        }
    }

    if (a_psOut)
    { *a_psOut = sCopy; }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_reserved(c_char a_cChar);
EMP_INTERN EMP_RETURN bool is_reserved(c_char a_cChar)
{
    if (emp::str::is_dot(a_cChar) ||
        emp::str::is_left_parenthesis(a_cChar) ||
        emp::str::is_right_parenthesis(a_cChar) ||
        emp::str::is_space(a_cChar))
    { return true; }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_user_operator(c_char a_cChar);
EMP_INTERN EMP_RETURN bool is_user_operator(c_char a_cChar)
{
    if (emp::str::is_operator(a_cChar) &&
        is_reserved(a_cChar) == false)
    { return true; }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_user_operator(c_astring& a_sValue);
EMP_INTERN EMP_RETURN bool is_user_operator(c_astring& a_sValue)
{
    c_size stLen = a_sValue.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        c_char cChar = a_sValue.get_char(stIndex);
        if (emp::str::is_operator(cChar) == false)
        { return false; }
        if (is_reserved(cChar))
        {
            EMP_LOG_ERROR("'#0' is a reserved character", cChar);
            return false;
        }
    }
    return stLen > 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_lower_alpha(c_astring& a_sValue);
EMP_INTERN EMP_RETURN bool is_lower_alpha(c_astring& a_sValue)
{
    c_size stLen = a_sValue.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        if (emp::str::is_lower_alpha(a_sValue.get_char(stIndex)) == false)
        { return false; }
    }
    return stLen > 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_constant(c_astring& a_sValue);
EMP_INTERN EMP_RETURN bool is_constant(c_astring& a_sValue)
{
    c_size stLen = a_sValue.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        c_char cChar = a_sValue.get_char(stIndex);
        if (emp::str::is_lower_alpha(cChar) == false &&
            emp::str::is_numeric(cChar) == false)
        { return false; }
    }
    return stLen > 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_float_value(c_astring& a_sValue);
EMP_INTERN EMP_RETURN bool is_float_value(c_astring& a_sValue)
{
    bool bDot = false;
    c_size stLen = a_sValue.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        c_char cChar = a_sValue.get_char(stIndex);
        if (emp::str::is_dot(cChar))
        {
            if (bDot)
            { return false; }
            bDot = true;
            continue;
        }
        if (emp::str::is_numeric(cChar) == false)
        { return false; }
    }
    return stLen > 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_EVL_OP_EQUAL(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const f1 = a_rStack.get_input(1);\
    a_rStack.push_node_output(emp::tt::equal_<T>(f0, f1) ? 1 : 0);

#define EMP_EVL_OP_NOT(a_rStack)\
    T const f = a_rStack.get_input(0);\
    a_rStack.push_node_output(emp::tt::equal_<T>(f, 0) ? 1 : 0);

#define EMP_EVL_OP_NTQ(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const f1 = a_rStack.get_input(1);\
    a_rStack.push_node_output(emp::tt::equal_<T>(f0, f1) ? 0 : 1);

#define EMP_EVL_OP_MOD(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const f1 = a_rStack.get_input(1);\
    a_rStack.push_node_output(mod(f0, f1));

#define EMP_EVL_OP_AND(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const f1 = a_rStack.get_input(1);\
    a_rStack.push_node_output(emp::tt::equal_<T>(\
        f0, 0) && emp::tt::equal_<T>(f1, 0) ? 1 : 0);

#define EMP_EVL_OP_OR(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const f1 = a_rStack.get_input(1);\
    a_rStack.push_node_output(emp::tt::equal_<T>(\
        f0, 0) || emp::tt::equal_<T>(f1, 0) ? 1 : 0);

#define EMP_EVL_OP_XOR(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const f1 = a_rStack.get_input(1);\
    a_rStack.push_node_output(emp::tt::equal_<T>(\
        f0, 0) != emp::tt::equal_<T>(f1, 0) ? 1 : 0);

#define EMP_EVL_OP_SAMPLE(a_rStack)\
    T const f0 = a_rStack.get_input(0);\
    T const a_fDepth = a_rStack.get_input(1);\
    T const fSign = f0 < 0 ? -1 : 1;\
    T const fValue = min<T>(1, abs(f0));\
    c_size stDepth = static_cast<size_t>(max<T>(0, a_fDepth));\
    T const fDepth = static_cast<T>(stDepth);\
    T const fRange = pow(2, fDepth) * 0.5f;\
    c_size stReturn = static_cast<size_t>(fValue * fRange);\
    T const fReturn = fSign * static_cast<T>(stReturn) / fRange;\
    a_rStack.push_node_output(fReturn);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void eval_stack_t<T>::destroy(void)
{
    if (m_pv4Values != nullptr)
    { EMP_DEALLOCATE_ALIGNED_POD_ARRAY(m_pv4Values); }

    m_pv4Values = nullptr;
    m_ptFramePointer = nullptr;

    if (m_pStack != nullptr)
    { EMP_DEALLOCATE(m_pStack); }
    m_pStack = nullptr;
    m_pStackPointer = nullptr;

    m_stFrameCount = 0;
    m_stFrameSize = 0;
    m_stStackSize = 0;
    m_stFrameIndex = 0;
    m_stFrameOffset = 0;
    m_bFirstPassed = false;
    m_bSecondPassed = false;
    m_bCompute = true;
    emp::cnt::foreach(m_vpCash, emp::mem::safe_delete_reset_t());
    m_vpCash.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void function_t<T>::destroy(void)
{
    m_vui32Program.resize(0);
    m_mstInputs.clear();
    m_vInputs.resize(0);

    if (m_pv4Constants)
    {
        EMP_DEALLOCATE_ALIGNED_POD_ARRAY(m_pv4Constants);
        m_pv4Constants = nullptr;
    }
    m_stResult = 0;
    m_stArity = 0;
    m_stSize = 0;
    m_stTmpSize = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool function_t<T>::evaluate(eval_stack_t<T>& a_rStack)
{
#define XTMP_PARTIAL_COUNT 12
    c_ui32* pProgramCounter = m_vui32Program.ptr(0);
    cpc_ui32 pEnd = m_vui32Program.last_ptr();

    //EMP_LOG_DEBUG("BEGIN FUNCTION_C::EVALUATE #0", m_sExpression);
    //size_t stInstruction = 0;
    while (true)
    {
        //EMP_LOG_DEBUG("INDEX #0/#1",
          //  static_cast<si32>(m_vui32Program.count())
            //    - 1 - static_cast<si32>(pEnd - pProgramCounter),
              //  m_vui32Program.count());

        if (pProgramCounter <= pEnd)
        {
            c_ui32 ui32Instruction = *pProgramCounter;

            ++pProgramCounter;

            c_instruction_e eInstruction = enum_from(instruction_e, ui32Instruction);
            //EMP_LOG_DEBUG("INSTRUCTION #0", ui32Instruction);
            //++stInstruction;

            switch (eInstruction)
            {
                case instruction_e::Constant:
                {
                    c_ui32 ui32Index = *pProgramCounter;
                    ++pProgramCounter;

                    EMP_ASSERT(ui32Index < m_stConstants);
                    vector_d const fValue = m_pv4Constants[ui32Index];
                    EMP_EVL_ASSERT(is_valid(fValue));

                    //EMP_LOG_DEBUG("CONSTANT INDEX #0, VALUE #1",
                     //   ui32Index, fValue);
#ifdef EMP_EVL_AOS
                    c_size ui32Arity = a_rStack.leaf_arity();
#endif
                    vector_d* const pOutput = a_rStack.leaf_output();
                    for (size_t stStride = 0; stStride < precision_c::array_stride; ++stStride)
                    { pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)]=fValue; }
                    a_rStack.flush_leaf_output(1);
                    break;
                }
                case instruction_e::Input:
                {
                    c_ui32 ui32Index = *pProgramCounter;
                    ++pProgramCounter;

                    //EMP_LOG_DEBUG("INPUT BIND #0", ui32Index);

                    input_t<T>&rInput = m_vInputs.ref(ui32Index);
                    if (emp::tt::is_max(rInput.bind()))
                    {
                        //EMP_LOG_ERROR("unbound input #0/#1", ui32Index, input_count());
                        return false;
                    }
                    else if (rInput.evaluate(a_rStack) == false)
                    {
                        //EMP_LOG_ERROR("Failed to evaluate input #0",ui32Index);
                        return false;
                    }

                    break;
                }
                case instruction_e::Link:
                {
                    // @@0  lib_evl link imp
                    //EMP_LOG_INFO("LINK NOT IMPLEMENTED");
                    //c_ui32 ui32Arity = *pProgramCounter;
                    //if (ui32Arity) {}
                    ++pProgramCounter;

                    //c_ui32 ui32Return = *pProgramCounter;
                    //if (ui32Return) {}
                    ++pProgramCounter;

                    break;
                }
                case instruction_e::Operation:
                {
                    c_ui32 ui32Operation = *pProgramCounter;
                    ++pProgramCounter;

                    c_operation_e eOperation = static_cast<operation_e>(ui32Operation);

                    c_ui32 ui32Arity = *pProgramCounter;
                    ++pProgramCounter;

                    //c_ui32 ui32Return = *pProgramCounter;
                    //if (ui32Return) {}
                    ++pProgramCounter;

                    //EMP_LOG_DEBUG("OPERATION #0, ARITY #1, RETURN #2",
                    //  ui32Operation, ui32Arity, ui32Return);

                    a_rStack.push(eOperation, ui32Arity);
                    break;
                }
                case instruction_e::none:
                default:
                {
                    //EMP_LOG_ERROR("Invalid instruction #0.", ui32Instruction);
                    return false;
                }
            };
        }
        while (a_rStack.remaining() == 0)
        {
            c_size ui32Operation = a_rStack.operation();
#ifdef EMP_EVL_AOS
            c_size ui32Arity = a_rStack.node_arity();
#endif
            c_operation_e eOperation = enum_from(operation_e, ui32Operation);

            switch (eOperation)
            {
                case operation_e::Assign:
                { return true; }
                case operation_e::Comma:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();

                    //@@perf simd aos soa memcpy
                    c_size stCount = a_rStack.leaf_arity();
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, stIndex, stCount)]));\
    pOutput[EMP_EVL_STRIDE(x_Index, stIndex, ui32Arity)] = pInput[EMP_EVL_STRIDE(x_Index, stIndex, stCount)];\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, stIndex, ui32Arity)]));
                        EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    }
                    a_rStack.flush_node_output(stCount);
                    break;
                }
                case operation_e::Add:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 0, EMP_EVL_ADD_ARITY)]));\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 1, EMP_EVL_ADD_ARITY)]));\
    *(pOutput + EMP_EVL_STRIDE(x_Index, 0, ui32Arity)) = tool_d::add(\
    *(pInput + EMP_EVL_STRIDE(x_Index,0,EMP_EVL_ADD_ARITY)),\
    *(pInput + EMP_EVL_STRIDE(x_Index,1,EMP_EVL_ADD_ARITY)));\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(add_d::result);
                    break;
                }
                case operation_e::Sub:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 0, EMP_EVL_SUB_ARITY)]));\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 1, EMP_EVL_SUB_ARITY)]));\
    *(pOutput + EMP_EVL_STRIDE(x_Index, 0, ui32Arity)) = tool_d::sub(\
    *(pInput + EMP_EVL_STRIDE(x_Index,0,EMP_EVL_SUB_ARITY)),\
    *(pInput + EMP_EVL_STRIDE(x_Index,1,EMP_EVL_SUB_ARITY)));\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(sub_d::result);
                    break;
                }
                case operation_e::Mul:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 0, EMP_EVL_MUL_ARITY)]));\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 1, EMP_EVL_MUL_ARITY)]));\
    *(pOutput + EMP_EVL_STRIDE(x_Index, 0, ui32Arity)) = tool_d::mul(\
    *(pInput + EMP_EVL_STRIDE(x_Index,0,EMP_EVL_MUL_ARITY)),\
    *(pInput + EMP_EVL_STRIDE(x_Index,1,EMP_EVL_MUL_ARITY)));\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(mul_d::result);
                    break;
                }
                case operation_e::Div:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 0, EMP_EVL_DIV_ARITY)]));\
    EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, 1, EMP_EVL_DIV_ARITY)]));\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::div(\
    pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_DIV_ARITY)],\
    pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_DIV_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(div_d::result);
                    break;
                }
                case operation_e::Rand:
                {
                    vector_d* const pOutput = a_rStack.node_output();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = {\
        a_rStack.m_pRandom->template rand<float>(-1, 1),\
        a_rStack.m_pRandom->template rand<float>(-1, 1),\
        a_rStack.m_pRandom->template rand<float>(-1, 1),\
        a_rStack.m_pRandom->template rand<float>(-1, 1)};\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(rand_d::result);
                    break;
                }
                case operation_e::Sqrt:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::sqr(pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_SQRT_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(sqrt_d::result);
                    break;
                }
                case operation_e::Pow:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::pow(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_POW_ARITY)],\
        pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_POW_ARITY)]);\
        EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(pow_d::result);
                    break;
                }
                case operation_e::Sin:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::sin(pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_SIN_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(sin_d::result);
                    break;
                }
                case operation_e::Cos:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::cos(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_COS_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(cos_d::result);
                    break;
                }
                case operation_e::Abs:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::abs(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_ABS_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(abs_d::result);
                    break;
                }
                case operation_e::Less:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::less(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_LESS_ARITY)],\
        pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_LESS_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(less_d::result);
                    break;
                }
                case operation_e::More:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::more(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_MORE_ARITY)],\
        pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_MORE_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(more_d::result);
                    break;
                }
                case operation_e::Lsq:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::less_equal(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_LSQ_ARITY)],\
        pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_LSQ_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(lsq_d::result);
                    break;
                }
                case operation_e::Mrq:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = tool_d::more_equal(\
        pInput[EMP_EVL_STRIDE(x_Index,0,EMP_EVL_MRQ_ARITY)],\
        pInput[EMP_EVL_STRIDE(x_Index,1,EMP_EVL_MRQ_ARITY)]);\
    EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)]));
                    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
                    a_rStack.flush_node_output(mrq_d::result);
                    break;
                }
                case operation_e::Sawtooth:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
                    for (size_t stStride = 0; stStride < precision_c::array_stride; ++stStride)
                    {
                        vector_d const v0 = pInput[EMP_EVL_STRIDE(stStride, 0, sawtooth_d::arity)];
                        vector_d const vcount = pInput[EMP_EVL_STRIDE(stStride, 1, sawtooth_d::arity)];
                        vector_d vPartials = tool_d::zero();
                        vector_d const v2PIf0 = tool_d::mul(tool_d::mul(tool_d::two(), tool_d::pi()),v0);

                        c_size stCount = emp::tt::maxof<size_t>(emp::tt::maxof<size_t>(0u,static_cast<size_t>(tool_d::max(
                            tool_d::max(vcount, tool_d::zero())))), XTMP_PARTIAL_COUNT);

                        switch (stCount)
                        {
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
    #define XTMP_PARTIAL(x_Depth, x_Index, x_Count)\
    case x_Count - x_Index:\
    {\
        vector_d const vK = tool_d::set(x_Count - x_Index);\
        vPartials = tool_d::add(vPartials, tool_d::div(\
            tool_d::sin(tool_d::mul(vK, v2PIf0)), vK));\
    }
                            EMP_PP_FOR(0, XTMP_PARTIAL_COUNT, XTMP_PARTIAL)
    #undef XTMP_PARTIAL
                            default:
                            {
                                vector_d const vResult=tool_d::div(tool_d::mul(tool_d::two(), vPartials), tool_d::pi());
                                pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vResult;
                                EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)]));
                                break;
                            }
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)

                        }
                    }
                    a_rStack.flush_node_output(sawtooth_d::result);
                    break;
                }
                case operation_e::Triangle:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
                    for (size_t stStride = 0; stStride < precision_c::array_stride; ++stStride)
                    {
                        vector_d const v0 = pInput[EMP_EVL_STRIDE(stStride, 0, triangle_d::arity)];
                        vector_d const vcount = pInput[EMP_EVL_STRIDE(stStride, 1, triangle_d::arity)];
                        vector_d vPartials = tool_d::zero();
                        vector_d const v2PIf0 = tool_d::mul(tool_d::mul(tool_d::two(), tool_d::pi()), v0);

                        c_size stCount = emp::tt::minof<size_t>(emp::tt::maxof<size_t>(0u, static_cast<size_t>(tool_d::max(
                            tool_d::max(vcount, tool_d::zero())))), XTMP_PARTIAL_COUNT);

                        switch (stCount)
                        {
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
    #define XTMP_PARTIAL(x_Depth, x_Index, x_Count)\
    case x_Count - x_Index:\
    {\
        vector_d const vK= tool_d::set(2 * (x_Count - x_Index - 1) + 1);\
        vPartials = tool_d::add(vPartials, tool_d::div(\
            tool_d::cos(tool_d::mul(vK, v2PIf0)), tool_d::mul(vK, vK)));\
    }
                            EMP_PP_FOR(0, XTMP_PARTIAL_COUNT, XTMP_PARTIAL)
    #undef XTMP_PARTIAL
                            default:
                            {
                                vector_d const vResult=tool_d::div(tool_d::mul(tool_d::eight(),vPartials),tool_d::pipi());
                                pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vResult;
                                EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)]));

                                break;
                            }
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
                        }
                    }
                    a_rStack.flush_node_output(triangle_d::result);
                    break;
                }
                case operation_e::Square:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
                    for (size_t stStride = 0; stStride < precision_c::array_stride; ++stStride)
                    {
                        vector_d const v0 = pInput[EMP_EVL_STRIDE(stStride, 0, square_d::arity)];
                        vector_d const vcount = pInput[EMP_EVL_STRIDE(stStride, 1, square_d::arity)];
                        vector_d vPartials = tool_d::zero();
                        vector_d const v2PIf0 = tool_d::mul(tool_d::mul(tool_d::two(), tool_d::pi()), v0);

                        c_size stCount = emp::tt::minof<size_t>(emp::tt::maxof<size_t>(0u, static_cast<size_t>(tool_d::max(
                            tool_d::max(vcount, tool_d::zero())))), XTMP_PARTIAL_COUNT);

                        switch (stCount)
                        {
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
    #define XTMP_PARTIAL(x_Depth, x_Index, x_Count)\
    case x_Count - x_Index:\
    {\
        vector_d const vK =tool_d::set(2 * (x_Count - x_Index - 1) + 1);\
        vPartials = tool_d::add(vPartials, tool_d::div(\
            tool_d::sin(tool_d::mul(vK, v2PIf0)), vK));\
    }
                            EMP_PP_FOR(0, XTMP_PARTIAL_COUNT, XTMP_PARTIAL)
    #undef XTMP_PARTIAL
                            default:
                            {
                                vector_d const vResult=tool_d::div(tool_d::mul(tool_d::four(), vPartials), tool_d::pi());
                                pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vResult;
                                EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)]));
                                break;
                            }
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
                        }
                    }
                    a_rStack.flush_node_output(square_d::result);
                    break;
                }
                case operation_e::Scale:
                {
                    vector_d* const pOutput = a_rStack.node_output();
                    vector_d const* const pInput = a_rStack.input();
                    for (size_t stStride = 0; stStride < precision_c::array_stride; ++stStride)
                    {
                        c_size stOffset0 = EMP_EVL_STRIDE(stStride,0,scale_d::arity);
                        c_size stOffset1 = EMP_EVL_STRIDE(stStride,1,scale_d::arity);
                        c_size stOffset2 = EMP_EVL_STRIDE(stStride,2,scale_d::arity);
                        c_size stOffset3 = EMP_EVL_STRIDE(stStride,3,scale_d::arity);

                        vector_d const vorigin = pInput[stOffset0];
                        vector_d const vtones = pInput[stOffset1];
                        vector_d const vratio = pInput[stOffset2];
                        vector_d const voffset = pInput[stOffset3];

                        if (a_rStack.first_passed() &&
                            emp::mat::bool_equal(vorigin, a_rStack.get_last_input(stOffset0)) &&
                            emp::mat::bool_equal(vtones, a_rStack.get_last_input(stOffset1)) &&
                            emp::mat::bool_equal(vratio, a_rStack.get_last_input(stOffset2)) &&
                            emp::mat::bool_equal(voffset, a_rStack.get_last_input(stOffset3)))
                       {
                            a_rStack.push_node_output(EMP_EVL_STRIDE(stStride, 0, ui32Arity),
                                a_rStack.get_last_output(EMP_EVL_STRIDE(stStride, 0, ui32Arity)));
                        }
                        else
                        {
                            pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)] =
                                tool_d::mul(vorigin, tool_d::pow(vratio, tool_d::div(voffset, vtones)));
                            EMP_EVL_ASSERT(is_valid(pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)]));
                        }
                    }
                    a_rStack.flush_node_output(scale_d::result);
                    break;
                }
                case operation_e::none:
                case operation_e::Pi:
                case operation_e::True:
                case operation_e::False:
                case operation_e::User:
                case operation_e::UserMax:
                default:
                {
                    //EMP_LOG_ERROR("Invalid operation #0", ui32Operation);
                    return false;
                }
            }
            a_rStack.pop();
        }
        if (pProgramCounter > pEnd)
        { break; }
    }

#undef XTMP_PARTIAL_COUNT
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::create(c_size a_stCount)
{
    using namespace emp::str;

    destroy();

    m_pOperations = EMP_ALLOCATE_ALIGNED_ARRAY(operation_t<T>, a_stCount, 16u); //-V747
    m_stCount = a_stCount;

#define REG(x_Operation, x_Name, x_Value, x_Alloc)\
regop(operation_e::x_Operation, operation_t<T>(ALITERAL(x_Name), operation_e::x_Operation,\
        operation_d<operation_e::x_Operation>::precedence,\
        operation_d<operation_e::x_Operation>::result,\
        operation_d<operation_e::x_Operation>::arity,\
        x_Value, x_Alloc))

    vector_d const noval = tool_d::zero();
    typename operation_t<T>::pfn_allocate_d const noalloc = nullptr;
    vector_d const pival = tool_d::pi();
    vector_d const val_0 = tool_d::zero();
    vector_d const val_1 = tool_d::one();
    //c_ui32 narity = emp::tt::max<ui32>::value;

    if (REG(Comma     , ","        , noval , noalloc ) &&
        REG(Add       , "+"        , noval , noalloc ) &&
        REG(Sub       , "-"        , noval , noalloc ) &&
        REG(Mul       , "*"        , noval , noalloc ) &&
        REG(Div       , "/"        , noval , noalloc ) &&
        REG(Rand      , "random"   , noval , noalloc ) &&
        REG(Sqrt      , "sqrt"     , noval , noalloc ) &&
        REG(Pow       , "pow"      , noval , noalloc ) &&
        REG(Sin       , "sin"      , noval , noalloc ) &&
        REG(Cos       , "cos"      , noval , noalloc ) &&
        REG(Abs       , "abs"      , noval , noalloc ) &&
        REG(Less      , "less"     , noval , noalloc ) &&
        REG(More      , "more"     , noval , noalloc ) &&
        REG(Lsq       , "lsq"      , noval , noalloc ) &&
        REG(Mrq       , "mrq"      , noval , noalloc ) &&
        REG(Sawtooth  , "sawtooth" , noval , noalloc ) &&
        REG(Triangle  , "triangle" , noval , noalloc ) &&
        REG(Square    , "square"   , noval , noalloc ) &&
        REG(Scale     , "scale"    , noval , noalloc ) &&
        REG(Pi        , "pi"       , pival , noalloc ) &&
        REG(True      , "true"     , val_1 , noalloc ) &&
        REG(False     , "false"    , val_0 , noalloc ))
    { return true; }

#undef REG

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::build(emp::cnt::vector<function_i<T>*>& a_rvpFunctions, c_astring& a_sExpression) const
{
    astring sCopy;
    if (validate_string(a_sExpression, &sCopy) == false)
    {
        EMP_LOG_ERROR("failed to validate expression #0", a_sExpression);
        return false;
    }

    size_t EMP_TT_MAX_VAR(stArity);
    size_t EMP_TT_MAX_VAR(stResult);
    if (build__(a_rvpFunctions, nullptr, sCopy, stArity, stResult) == false)
    {
        EMP_LOG_ERROR("failed do build function #0", a_sExpression);
        emp::cnt::foreach(a_rvpFunctions, emp::mem::delete_reset_t());
        a_rvpFunctions.resize(0);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::build_call(vector<function_i<T>*>& a_rvpFunctions, function_t<T>* const a_pRoot,
    c_astring& a_sExpression, size_t& a_rstArity, size_t& a_rstResult) const
{
    astring sCursor(a_sExpression);

    astring sCopy(sCursor);
    size_t stNary = 0;
    while (sCursor.len())
    {
        c_char cNary = sCursor.get_first_char();
        if (emp::str::is_lower_alpha(cNary) == false)
        { break; }

        sCursor.trim_left();
        //EMP_LOG_DEBUG("CALLCURSOR #0", sCursor);
        ++stNary;
    }

    if (stNary > 0)
    {
        astring sNaryName(sCopy);
        sNaryName.trim_right(sCopy.len() - stNary);
        //EMP_LOG_DEBUG("CALL : #0", sNaryName);

        operation_t<T> const* const pCall = getop(sNaryName);
        if (pCall == nullptr)
        {
            EMP_LOG_ERROR("undefined function call #0", sNaryName);
            return false;
        }

        size_t EMP_TT_MAX_VAR(stArityIndex);
        size_t EMP_TT_MAX_VAR(stResultIndex);

        size_t EMP_TT_MAX_VAR(stArity);
        size_t EMP_TT_MAX_VAR(stResult);

        function_t<T>* pRoot = nullptr;
        function_i<T>* pFunctionCall = nullptr;
        if (pCall->function() != nullptr)
        {
            pFunctionCall = (*pCall->function())();
            a_rvpFunctions.add(pFunctionCall);
            pRoot = pFunctionCall->implementation();

            if (a_pRoot)
            { a_pRoot->add_link(pRoot, stArityIndex, stResultIndex); }
        }
        else
        {
            pRoot = a_pRoot;
            if (pRoot == nullptr)
            {
                pRoot = EMP_ALLOCATE(function_t<T>);
                a_rvpFunctions.add(pRoot);
            }

            pRoot->add_operation(pCall->operation(), stArityIndex, stResultIndex);
        }

        c_bool bNullary =
            sCursor.len() == 2 &&
            emp::str::is_left_parenthesis(sCursor.get_first_char()) &&
            emp::str::is_right_parenthesis(sCursor.get_last_char());

        if (pCall->arity() > 0)
        {
            if (pCall->arity() == emp::tt::max<ui32>::value &&
                bNullary)
            {
                stArity = 0;
                stResult = 0;
            }
            else if (build__(a_rvpFunctions, pRoot, sCursor, stArity, stResult) ==false)
            {
                EMP_LOG_INFO("failed to build subexpression #0", sCursor);
                return false;
            }
        }
        else
        {
            stArity = bNullary ? 0 : emp::tt::max<ui32>::value;
            stResult = bNullary ? 0 : emp::tt::max<ui32>::value;
        }

        if (pCall->arity() != stResult &&
            pCall->arity() != emp::tt::max<ui32>::value)
        {
            EMP_LOG_ERROR("invalid function call #0, arity mismatch, expected #1, found #2", sNaryName, pCall->arity(), stResult);
            return false;
        }

        a_rstArity = stResult;
        a_rstResult = pCall->result();

        //if (a_pRoot)
        if (pCall->function() == nullptr)
        {
            pRoot->set_instruction_arity(stArityIndex, static_cast<ui32>(a_rstArity));
            pRoot->set_instruction_result(stResultIndex, static_cast<ui32>(a_rstResult));
        }

        if (pRoot != a_pRoot)
        {
            if (pFunctionCall)
            {
                pRoot->set_arity(stArity);
                pRoot->set_result(stResult);

                pFunctionCall->set_result(a_rstResult);
            }
            else
            {
                pRoot->set_arity(a_rstArity);
                pRoot->set_result(a_rstResult);
            }
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::build_leaf(vector<function_i<T>*>& a_rvpFunctions, function_t<T>* const a_pRoot,
    c_astring& a_sExpression, size_t& a_rstArity, size_t& a_rstResult) const
{
    if (is_constant(a_sExpression))
    {
        T fFactor = 1;
        astring sEnd;
        if (emp::str::to(a_sExpression, fFactor, &sEnd) == false)
        { fFactor = 1; }
        operation_t<T> const* const pConstant = getop(sEnd);
        if (pConstant != nullptr &&
            pConstant->arity() == 0 &&
            pConstant->result() == 1)
        {
            //EMP_LOG_DEBUG("CONSTANT : #0", a_sExpression);
            function_t<T>* pRoot = a_pRoot;
            if (pRoot == nullptr)
            {
                pRoot = EMP_ALLOCATE(function_t<T>);
                a_rvpFunctions.add(pRoot);
            }
            pRoot->add_constant(
                tool_d::mul(tool_d::set(fFactor),pConstant->value()),
                    a_rstArity, a_rstResult);
            if (pRoot != a_pRoot)
            {
                pRoot->set_arity(a_rstArity);
                pRoot->set_result(a_rstResult);
            }
            return true;
        }
    }
    if (is_lower_alpha(a_sExpression))
    {
        //EMP_LOG_DEBUG("VARIABLE : #0", a_sExpression);
        function_t<T>* pRoot = a_pRoot;
        if (pRoot == nullptr)
        {
            pRoot = EMP_ALLOCATE(function_t<T>);
            a_rvpFunctions.add(pRoot);
        }
        pRoot->add_input(a_sExpression, a_rstArity, a_rstResult);
        if (pRoot != a_pRoot)
        {
            pRoot->set_arity(a_rstArity);
            pRoot->set_result(a_rstResult);
        }
        return true;
    }
    else if (is_float_value(a_sExpression))
    {
        T fValue = 0;
        if (emp::str::to(a_sExpression, fValue))
        {
            function_t<T>* pRoot = a_pRoot;
            if (pRoot == nullptr)
            {
                pRoot = EMP_ALLOCATE(function_t<T>);
                a_rvpFunctions.add(pRoot);
            }
            pRoot->add_constant(
                tool_d::set(fValue), a_rstArity, a_rstResult);
            if (pRoot != a_pRoot)
            {
                pRoot->set_arity(a_rstArity);
                pRoot->set_result(a_rstResult);
            }
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::build__(vector<function_i<T>*>& a_rvpFunctions, function_t<T>* const a_pRoot,
    c_astring& a_sExpression, size_t& a_rstArity, size_t& a_rstResult) const
{
    if ((a_sExpression.len() > 0) == false)
    { return false; }
    //EMP_LOG_DEBUG("_____________________________________________");
    //EMP_LOG_DEBUG("EXPRESSION : #0", a_sExpression);

    size_t EMP_TT_MAX_VAR(stBinaryOffset);
    size_t EMP_TT_MAX_VAR(stBinaryLen);

    operation_t<T> const* pBinary = nullptr;

    {int siParIndex = 0;
    astring sCursor(a_sExpression);
    while (sCursor.len())
    {
        c_char cCursor = sCursor.get_first_char();
        if (emp::str::is_left_parenthesis(cCursor))
        { ++siParIndex; }
        else if (emp::str::is_right_parenthesis(cCursor))
        { --siParIndex; }

        //EMP_LOG_DEBUG("CURSOR : #0", sCursor);
        if (siParIndex == 0 &&
            is_user_operator(cCursor))
        {
            astring sCopy(sCursor);
            size_t stBinary = 0;
            while (sCursor.len())
            {
                c_char cBinary = sCursor.get_first_char();
                //EMP_LOG_DEBUG("CURSOR : #0", sCursor);
                if (is_user_operator(cBinary) == false)
                { break; }

                sCursor.trim_left();
                ++stBinary;
            }

            if (stBinary > 0)
            {
                astring sBinaryName(sCopy);
                sBinaryName.trim_right(sCursor.len() - 1 + stBinary);
                //EMP_LOG_DEBUG("INFIX : #0", sBinaryName);
                operation_t<T> const* const pCurrent = getop(sBinaryName);
                if (pCurrent && pCurrent->arity() == 2 && pCurrent->infix() &&
                    (pBinary == nullptr ||
                        pCurrent->precedence() < pBinary->precedence()))
                {
                    pBinary = pCurrent;
                    stBinaryOffset = a_sExpression.len() - sCursor.len() - stBinary;
                    stBinaryLen = sBinaryName.len();
                }
            }
        }
        else
        { sCursor.trim_left(); }
    }

    if (siParIndex != 0)
    {
        EMP_LOG_ERROR("missing #0 #1 parenthesis in expression #2",
            abs(siParIndex), siParIndex > 0 ? ALITERAL(")") : ALITERAL("("), a_sExpression);
        return false;
    } }

    if (emp::tt::is_not_max(stBinaryOffset))
    {
        c_operation_e eOperation = pBinary->operation();
        function_t<T>* pRoot = a_pRoot;
        if (a_pRoot == nullptr)
        {
            pRoot = EMP_ALLOCATE(function_t<T>);
            a_rvpFunctions.add(pRoot);
        }

        size_t EMP_TT_MAX_VAR(stArityIndex);
        size_t EMP_TT_MAX_VAR(stResultIndex);
        pRoot->add_operation(eOperation, stArityIndex, stResultIndex);

        astring sLeft(a_sExpression);
        sLeft.trim_right(sLeft.len() - stBinaryOffset);
        //EMP_LOG_DEBUG("LEFT : #0", sLeft);
        size_t EMP_TT_MAX_VAR(stAriLeft);
        size_t EMP_TT_MAX_VAR(stResLeft);

        if (build__(a_rvpFunctions, pRoot, sLeft, stAriLeft, stResLeft) == false)
        {
            if (eOperation == operation_e::Add ||
                eOperation == operation_e::Sub)
            { pRoot->add_constant(tool_d::zero(), stAriLeft, stResLeft); }
            else
            { return false; }
        }

        astring sRight(a_sExpression);
        sRight.trim_left(stBinaryOffset + stBinaryLen);
        //EMP_LOG_DEBUG("RIGHT : #0", sRight);
        size_t EMP_TT_MAX_VAR(stAriRight);
        size_t EMP_TT_MAX_VAR(stResRight);

        if (build__(a_rvpFunctions, pRoot, sRight, stAriRight, stResRight) == false)
        { return false; }

        c_bool bOpComma = eOperation == operation_e::Comma;

        c_size stArity = bOpComma ? stResLeft + stResRight : emp::ast::cast<size_t>(pBinary->arity());
        c_size stReturn = bOpComma ? stResLeft + stResRight : emp::ast::cast<size_t>(pBinary->result());

        pRoot->set_instruction_arity(stArityIndex, static_cast<ui32>(stArity));
        pRoot->set_instruction_result(stResultIndex, static_cast<ui32>(stReturn));

        a_rstArity = stArity;
        a_rstResult = stReturn;

        if (pRoot != a_pRoot)
        {
            pRoot->set_arity(a_rstArity);
            pRoot->set_result(a_rstResult);
        }

        return true;
    }

    c_bool bLeftPar = emp::str::is_left_parenthesis(a_sExpression.get_first_char());
    c_bool bRightPar = emp::str::is_right_parenthesis(a_sExpression.get_last_char());

    if (bLeftPar && bRightPar)
    {
        //EMP_LOG_DEBUG("PAR : #0", a_sExpression);
        astring sNewExpression(a_sExpression);
        sNewExpression.trim_left();
        sNewExpression.trim_right();
        return build__(a_rvpFunctions, a_pRoot, sNewExpression, a_rstArity, a_rstResult);
    }

    if (bLeftPar == false && bRightPar)
    {
        if (build_call(a_rvpFunctions, a_pRoot, a_sExpression, a_rstArity, a_rstResult))
        { return true; }
    }

    if (bLeftPar == false && bRightPar == false)
    {
        if (build_leaf(a_rvpFunctions, a_pRoot, a_sExpression, a_rstArity, a_rstResult))
        { return true; }
    }

    EMP_LOG_ERROR("unhandled expression #0", a_sExpression);

    if (bLeftPar && bRightPar == false)
    { EMP_ASSERT_UNREACHABLE(); }

    EMP_ASSERT_UNREACHABLE();
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::validate(c_astring& a_sName) const
{
    if (validate_string(a_sName, nullptr) == false)
    {
        EMP_LOG_ERROR("invalid function name #0", a_sName);
        return false;
    }

    c_size stCount = m_stCount;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        operation_t<T> const& rOperation = m_pOperations[stIndex];
        if (emp::str::cmp(rOperation.name(), a_sName))
        { return false; }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool compiler_t<T>::regop(c_operation_e a_eOperation, operation_t<T> const& a_rOperation)
{
    c_size stOperation = enum_to(a_eOperation);
    c_size stCount = m_stCount;
    if (stOperation >= stCount)
    { return false; }

    c_astring& sName = a_rOperation.name();
    if (validate(sName) == false)
    { return false; }

    if (is_user_operator(sName) == false &&
        is_lower_alpha(sName) == false &&
        is_constant(sName) == false)
    {
        EMP_LOG_ERROR("#0 is not a valid operator name", sName);
        return false;
    }

    m_pOperations[stOperation] = a_rOperation;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN operation_t<T> const* compiler_t<T>::getop(c_astring& a_rsName) const
{
    c_size stCount = m_stCount;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        operation_t<T> const& rOperation = m_pOperations[stIndex];
        if (emp::str::cmp(rOperation.name(), a_rsName))
        { return &rOperation; }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template class eval_stack_t<precision_c::type>;
template class compiler_t<precision_c::type>;
template class function_t<precision_c::type>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
