//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_processor.h"

#include "emp_ose_program.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_random.h"
#include "emp_mat_trigonometry.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void stack_c::copy(program_c const* const a_pProgram)
{
    m_Data.clear();
    m_vstFrames.resize(0);

    if (a_pProgram == nullptr)
    {
        EMP_TT_MAX_VAR(m_stInstructionPointer);
        m_vstFrames.add(0);
    }
    else
    {
        m_stInstructionPointer = a_pProgram->entry_point();
        m_Data.copy(a_pProgram->m_Data);
        m_vstFrames.add(a_pProgram->exit_point());
    }

    m_vstStacks.resize(0);
    m_vstStacks.add(0);
    m_stStackIndex = 0;
    m_stStackAddress = 0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void stack_c::halt(c_size a_stIndex)
{ m_stInstructionPointer = a_stIndex; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void stack_c::push_frame(c_size a_stValue)
{
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("PUSH_FRAME #0, #1", a_stValue, m_stStackIndex);)
    m_vstFrames.add(a_stValue);
    m_vstStacks.add(m_stStackIndex);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool stack_c::pop_frame(size_t& a_rstIndex, size_t& a_rstPop)
{
    EMP_TT_MAX_VAR(a_rstIndex);
    a_rstPop = 0;

    if (m_vstFrames.count() > 0)
    {
        c_size stReturn = m_vstFrames.last_ref();
        m_vstFrames.resize(m_vstFrames.count() - 1);
        a_rstIndex = stReturn;
        if (m_vstStacks.count() > 0)
        {
            c_size stCurrent = m_vstStacks.last_ref();
            m_vstStacks.remove_last();
            if (stCurrent <= m_stStackIndex)
            {
                a_rstPop = m_stStackIndex - stCurrent;
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("POP COUNT #0", a_rstPop);)
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("POP_FRAME #0, #1, #2", a_rstIndex, stCurrent, a_rstPop);)
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void stack_c::push_stack(void)
{
    ++m_stStackIndex;
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("PUSH_STACK #0", m_stStackIndex);)
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool stack_c::pop_stack(void)
{
    if (m_stStackIndex > 0)
    {
        --m_stStackIndex;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("POP_STACK #0", m_stStackIndex);)
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
processor_c::processor_c(void):
    m_pFctExtension(),
    m_pFctPush(),
    m_pFctPop()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void processor_c::set_extension(extension_d const& a_rValue)
{ *m_pFctExtension = a_rValue; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void processor_c::set_push(extension_push_d const& a_rValue)
{ *m_pFctPush = a_rValue; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void processor_c::set_pop(extension_pop_d const& a_rValue)
{ *m_pFctPop = a_rValue; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_DISABLED_OPTIMIZATION)
EMP_RETURN si32 processor_c::run(program_c& a_rProgram, stack_c& a_rStack,
    c_ui32 a_uiInstructionsPerFrame, c_ui32 a_uiInstructionsPerImage, ui32& a_ruiInstructionsProcessed)
{
    c_size stCount = a_rProgram.exit_point();
    EMP_RET_NEG1_IF_NOT((stCount > 0));

    c_size stEntry = a_rStack.instruction_pointer();
    EMP_RET_NEG1_IF(stEntry >= stCount);

    size_t stProcessed = 0;

    memory_walker_c oWalker(a_rProgram.m_Instructions);
    EMP_RET_NEG1_IF_NOT(oWalker.move_to(stEntry));

    while (true)
    {
        if (oWalker.pointer() >= stCount)
        { break; }

        if (a_uiInstructionsPerFrame > 0 && (stProcessed >= a_uiInstructionsPerFrame))
        { break; }

        if (a_uiInstructionsPerImage > 0 && (a_ruiInstructionsProcessed >= a_uiInstructionsPerImage))
        { break; }

        ++stProcessed;
        ++a_ruiInstructionsProcessed;
        char cInstruction = 0;
        EMP_RET_NEG1_IF_NOT(oWalker.next(cInstruction));

        c_instruction_e eInstruction = static_cast<instruction_e>(cInstruction);
        switch (eInstruction)
        {
            case instruction_e::Function:
            {
                ui32 ui32Instruction = 0;
                EMP_RET_NEG1_IF_NOT(oWalker.next(ui32Instruction));
                a_rStack.push_frame(oWalker.pointer());

                EMP_OSE_IF_DEBUG(
                    EMP_LOG_INFO("operation_e::Function = name \"#0\", index : #1",
                        a_rProgram.get_name_from_address(ui32Instruction),
                        a_rProgram.get_index_from_address(ui32Instruction));)

                a_rProgram.touch_function_from_address(ui32Instruction);
                EMP_RET_NEG1_IF_NOT(oWalker.move_to(ui32Instruction));
                break;
            }
            case instruction_e::Variant:
            {
                ui32 ui32Instruction = 0;
                EMP_RET_NEG1_IF_NOT(oWalker.next(ui32Instruction));
                a_rStack.push_frame(oWalker.pointer());

                // @@0 analyse
                c_size stFrame = a_rProgram.m_Table.get_variant(*a_rProgram.m_pRandom, ui32Instruction);
                //size_t stFrame = ui32Instruction;
                //stFrame = a_rProgram.m_Table.get_variant(ui32Instruction);
                EMP_OSE_IF_DEBUG(
                    EMP_LOG_INFO("operation_e::Variant ~= name \"#0\", index : #1",
                        a_rProgram.m_Table.get_name_from_index(ui32Instruction),
                        ui32Instruction);)

                a_rProgram.touch_variant_from_index(ui32Instruction);

                EMP_RET_NEG1_IF_NOT(oWalker.move_to(stFrame));
                break;
            }
            case instruction_e::PushScope:
            {
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("operation_e::Push");)
                a_rStack.push_stack();
                ui32 ui32PopAddress = 0;
                EMP_RET_NEG1_IF_NOT(oWalker.next(ui32PopAddress));

                if (m_pFctPush->is_valid())
                {
                    switch ((*m_pFctPush)())
                    {
                        case return_e::Continue:
                        { break; }
                        case return_e::Break:
                        {
                            EMP_RET_NEG1_IF_NOT(oWalker.move_to(ui32PopAddress));
                            break;
                        }
                        case return_e::Halt:
                        case return_e::Error:
                        case return_e::Return:
                        default:
                        {
                            EMP_LOG_ERROR("unhandled error");
                            return -1;
                        }
                    }
                }
                break;
            }
            case instruction_e::PopScope:
            {
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("operation_e::Pop");)
                EMP_RET_NEG1_IF_NOT(a_rStack.pop_stack());
                if (m_pFctPop->is_valid())
                {
                    switch ((*m_pFctPop)())
                    {
                        case return_e::Continue:
                        { break; }
                        case return_e::Break:
                        case return_e::Halt:
                        case return_e::Error:
                        case return_e::Return:
                        default:
                        {
                            EMP_LOG_ERROR("unhandled error");
                            return -1;
                        }
                    }
                }
                break;
            }
            case instruction_e::Enter:
            {
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("Enter");)
                ui32 u32ParameterSize = 0;
                EMP_RET_NEG1_IF_NOT(oWalker.next(u32ParameterSize));
                EMP_RET_NEG1_IF_NOT(a_rStack.push_base_address(u32ParameterSize));
                break;
            }
            case instruction_e::Return:
            {
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("Return");)
                size_t stIndex = 0;
                size_t stPopCount = 0;
                EMP_RET_NEG1_IF_NOT(a_rStack.pop_frame(stIndex, stPopCount));
                if (stIndex == stCount)
                {
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("Exit");)
                    oWalker.move_to_eof();
                }
                else
                {
                    EMP_RET_NEG1_IF_NOT(a_rStack.pop_base_address());
                    EMP_RET_NEG1_IF_NOT(oWalker.move_to(stIndex));
                }
                break;
            }
            case instruction_e::Jump:
            {
                ui32 ui32Jump = 0;
                EMP_RET_NEG1_IF_NOT(oWalker.next(ui32Jump));
                EMP_OSE_IF_DEBUG(
                    EMP_LOG_INFO("operation_e::Jump #0", ui32Jump);)
                EMP_RET_NEG1_IF_NOT(oWalker.move_to(ui32Jump));
                break;
            }
            #define XTMP_LOOP(x_Instruction, x_Type)\
            case x_Instruction:\
            {\
                x_Type tLoop = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop(tLoop));\
                x_Type tIndex = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop(tIndex));\
                ui32 ui32End = 0;\
                EMP_RET_NEG1_IF_NOT(oWalker.next(ui32End));\
                if (tIndex < tLoop)\
                {\
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("loop #0/#1", tIndex, tLoop);)\
                    a_rStack.push(tIndex + 1);\
                }\
                else\
                {\
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("end loop #0", ui32End);)\
                    EMP_RET_NEG1_IF_NOT(oWalker.move_to(ui32End));\
                }\
                break;\
            }
            XTMP_LOOP(instruction_e::JumpCmp_SI32, si32)
            XTMP_LOOP(instruction_e::JumpCmp_UI32, ui32)
            #undef XTMP_LOOP
            case instruction_e::JumpCmp_Bool:
            {
                bool bCondition = false;
                EMP_RET_NEG1_IF_NOT(a_rStack.pop(bCondition));

                ui32 ui32End = 0;
                EMP_RET_NEG1_IF_NOT(oWalker.next(ui32End));

                if (bCondition == true)
                {
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("if true"));
                }
                else
                {
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("end if #0", ui32End);)
                    EMP_RET_NEG1_IF_NOT(oWalker.move_to(ui32End));
                }

                break;
            }

// ----------
// UNARY cases
// ----------
#define XTMP_OSE_UNARY(x_Instruction, x_Type, x_Operator)\
            case instruction_e::x_Instruction:\
            {\
                x_Type tT = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(tT));\
                x_Type const t = x_Operator tT;\
                EMP_OSE_IF_DEBUG(\
                    EMP_LOG_INFO("instruction_e::#0, #1 => #2",\
                        ALITERAL(EMP_PP_STRING(x_Instruction)), tT, t););\
                a_rStack.push<x_Type>(t);\
                break;\
            }
            XTMP_OSE_UNARY(Neg_F32, float, -)
            XTMP_OSE_UNARY(Neg_I32, si32 , -)
            XTMP_OSE_UNARY(Pos_F32, float, +)
            XTMP_OSE_UNARY(Pos_I32, si32 , +)
#undef XTMP_OSE_UNARY

// ----------
// ABS cases
// ----------
#define XTMP_OSE_ABS(x_Instruction, x_Type)\
        case instruction_e::x_Instruction:\
        {\
            x_Type tT = 0;\
            EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(tT));\
            x_Type const t = tT < 0 ? -tT : tT;\
            EMP_OSE_IF_DEBUG(\
                EMP_LOG_INFO("instruction_e::#0, #1 => #2",\
                    ALITERAL(EMP_PP_STRING(x_Instruction)), tT, t););\
            a_rStack.push<x_Type>(t);\
            break;\
        }
        XTMP_OSE_ABS(Abs_F32, real)
        XTMP_OSE_ABS(Abs_I32, sint)
#undef XTMP_OSE_ABS

// ----------
// SQRT cases
// ----------
#define XTMP_OSE_SQRT(x_Instruction, x_Type)\
        case instruction_e::x_Instruction:\
        {\
            x_Type tT = 0;\
            EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(tT));\
            x_Type const t = emp::mat::sqr(tT);\
            EMP_OSE_IF_DEBUG(\
                EMP_LOG_INFO("instruction_e::#0, #1 => #2",\
                    ALITERAL(EMP_PP_STRING(x_Instruction)), tT, t););\
            a_rStack.push<x_Type>(t);\
            break;\
        }
        XTMP_OSE_SQRT(Sqrt_F32, real)
#undef XTMP_OSE_SQRT

// ----------
// TRIG cases
// ----------
#define XTMP_OSE_TRIG(x_Instruction, x_Type, x_Trig, x_Deg)\
            case instruction_e::x_Instruction:\
            {\
                x_Type tT = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(tT));\
                auto const tValue = tT EMP_PP_IF(x_Deg, *emp::mat::to_rad<x_Type>());\
                EMP_RET_NEG1_IF_NOT(emp::tt::is_valid(tValue));\
                x_Type const result = emp::mat::x_Trig(tValue);\
                EMP_OSE_IF_DEBUG(\
                    EMP_LOG_INFO("instruction_e::#0, #1 => #2",\
                        ALITERAL(EMP_PP_STRING(x_Instruction)), tT, result););\
                a_rStack.push<x_Type>(result);\
                break;\
            }
            XTMP_OSE_TRIG( CosD_F32 , real, cos, true  )
            XTMP_OSE_TRIG( SinD_F32 , real, sin, true  )
            XTMP_OSE_TRIG( Cos_F32  , real, cos, false )
            XTMP_OSE_TRIG( Sin_F32  , real, sin, false )
#undef XTMP_OSE_TRIG

// ----------
// PRINT cases
// ----------
#define XTMP_OSE_CASE(x_Instruction, x_Type)\
            case instruction_e::x_Instruction:\
            {\
                x_Type tT = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(tT));\
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("#0", tT);)\
                break;\
            }
            XTMP_OSE_CASE(Print_I32, sint)
#undef XTMP_OSE_CASE

// ----------
// RAND cases
// ----------
            case instruction_e::Randf_F32:
            {
                real const fRandom = a_rProgram.m_pRandom->value();
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("instruction_e::#0, #1", enum_to_cstr(eInstruction), fRandom);)
                a_rStack.push<real>(fRandom);
                break;
            }
// ----------
// BIN cases
// ----------
#define XTMP_OSE_BIN(x_Instruction, x_Type, x_Ret, x_Operator, x_Test)\
            case instruction_e::x_Instruction:\
            {\
                x_Type t1 = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(t1));\
                EMP_PP_IF(x_Test, if(emp::tt::equal_<x_Type>(t1, 0))\
                    {\
                        EMP_OSE_IF_DEBUG(EMP_LOG_ERROR("Divide by 0");)\
                        return -1; })\
                x_Type t0 = 0;\
                EMP_RET_NEG1_IF_NOT(a_rStack.pop<x_Type>(t0));\
                x_Ret const t = t0 x_Operator t1;\
                EMP_OSE_IF_DEBUG(\
                    EMP_LOG_INFO("instruction_e::#0, #1 #2 #3 = #4",\
                        ALITERAL(EMP_PP_STRING(x_Instruction)), t0, ALITERAL(EMP_PP_STRING(x_Operator)), t1, t););\
                a_rStack.push<x_Ret>(t);\
                break;\
            }
            XTMP_OSE_BIN(Equal_I32  , sint , bool , == , false)
            XTMP_OSE_BIN(More_F32   , real , bool , >  , false)
            XTMP_OSE_BIN(More_I32   , sint , bool , >  , false)
            XTMP_OSE_BIN(MoreEq_F32 , real , bool , >= , false)
            XTMP_OSE_BIN(MoreEq_I32 , sint , bool , >= , false)
            XTMP_OSE_BIN(Less_F32   , real , bool , <  , false)
            XTMP_OSE_BIN(Less_I32   , sint , bool , <  , false)
            XTMP_OSE_BIN(LessEq_F32 , real , bool , <= , false)
            XTMP_OSE_BIN(LessEq_I32 , sint , bool , <= , false)
            XTMP_OSE_BIN(Add_F32    , real , real , +  , false)
            XTMP_OSE_BIN(Add_U32    , uint , uint , +  , false)
            XTMP_OSE_BIN(Add_I32    , sint , sint , +  , false)
            XTMP_OSE_BIN(Sub_F32    , real , real , -  , false)
            XTMP_OSE_BIN(Sub_I32    , sint , sint , -  , false)
            XTMP_OSE_BIN(Mul_F32    , real , real , *  , false)
            XTMP_OSE_BIN(Mul_I32    , sint , sint , *  , false)
            XTMP_OSE_BIN(Div_F32    , real , real , /  , true )
            XTMP_OSE_BIN(Div_I32    , sint , sint , /  , true )
            XTMP_OSE_BIN(Mod_I32    , sint , sint , %  , true )
#undef XTMP_OSE_BIN

// ----------
// PUSH cases
// ----------
#define XTMP_OSE_PUSH_TYPE(x_Instruction, x_Type)\
            case instruction_e::x_Instruction:\
            {\
                x_Type tValue = 0;\
                EMP_RET_NEG1_IF_NOT(oWalker.next(tValue));\
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO(\
                    "instruction_e::#0 '#1'", ALITERAL(EMP_PP_STRING(x_Instruction)), tValue););\
                a_rStack.push<x_Type>(tValue);\
                break;\
            }
            // @@0 literal
            XTMP_OSE_PUSH_TYPE(PushReal, real)
            XTMP_OSE_PUSH_TYPE(PushSInt, sint)
            XTMP_OSE_PUSH_TYPE(PushUInt, uint)
            XTMP_OSE_PUSH_TYPE(PushBool, bool)
#undef XTMP_OSE_PUSH

// ----------
// LOAD cases
// ----------
#define XTMP_CASE(x_Instruction, x_Type, x_Abs)\
            case instruction_e::x_Instruction:\
            {\
                ui32 u32Address = 0;\
                EMP_RET_NEG1_IF_NOT(oWalker.next(u32Address));\
                x_Type tValue = 0;\
                c_size stBase = EMP_PP_IF_ELSE(x_Abs, 0, a_rStack.base_address());\
                c_size stOffset = u32Address;\
                c_size stAddress = stBase + stOffset;\
                EMP_RET_NEG1_IF_NOT(a_rStack.val(stAddress, tValue));\
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO(\
                    "instruction_e::#0 base #1, offset #2, address #3 => '#4'",\
                        ALITERAL(EMP_PP_STRING(x_Instruction)), stBase, stOffset, stAddress, tValue););\
                a_rStack.push<x_Type>(tValue);\
                break;\
            }
            // @@0 literal
            XTMP_CASE(LoadRel_F32 , real, false )
            XTMP_CASE(LoadRel_U32 , uint, false )
            XTMP_CASE(LoadRel_S32 , sint, false )
            XTMP_CASE(LoadRel_U8  , bool, false )
            XTMP_CASE(LoadAbs_F32 , real, true  )
            XTMP_CASE(LoadAbs_U32 , uint, true  )
            XTMP_CASE(LoadAbs_S32 , sint, true  )
            XTMP_CASE(LoadAbs_U8  , bool, true  )
#undef XTMP_CASE

// ----------
// UNHANDLED cases
// ----------
#define XTMP_OSE_UNHANDLED(x_Case)\
            case instruction_e::x_Case:\
            {\
                EMP_LOG_ERROR("unhandled instruction '#0' : #1", ALITERAL(EMP_PP_STRING(x_Case)), static_cast<ui32>(eInstruction));\
                return -1;\
            }
            XTMP_OSE_UNHANDLED(Reserved)
            XTMP_OSE_UNHANDLED(Invalid)
            XTMP_OSE_UNHANDLED(none)
#undef XTMP_OSE_UNHANDLED
            case instruction_e::Extension:
            default:
            {
                if (eInstruction >= instruction_e::Extension)
                {
                    c_ui32 ui32Extension = enum_to_type(eInstruction, ui32) - enum_to_type(instruction_e::Extension, ui32);
                    EMP_RET_NEG1_IF(ui32Extension > emp::tt::max<char>::value);
                    if (m_pFctExtension->is_valid())
                    {
                        switch ((*m_pFctExtension)(a_rProgram, a_rStack, ui32Extension))
                        {
                            case return_e::Continue:
                            { break; }
                            case return_e::Break:
                            case return_e::Halt:
                            case return_e::Error:
                            case return_e::Return:
                            default:
                            {
                                EMP_LOG_ERROR("unhandled error");
                                return -1;
                            }
                        }
                    }
                    break;
                }

                EMP_LOG_ERROR("invalid instruction #0", static_cast<ui32>(eInstruction));
                EMP_LOG_ERROR("//----------------------------------------");
                return -1;
            }
        };
        //if (stIndex == stCount)
        //{
            /*size_t stPopCount = 0;
            if (a_rStack.pop_frame(stIndex, stPopCount) == false)
            {
                EMP_LOG_ERROR("failed to pop frame");
                return -1;
            }
            for (size_t stPop = 0; stPop < stPopCount; ++stPop)
            {
                switch ((*m_pFctPop)())
                {
                    case return_e::Continue:
                    case return_e::Break:
                    case return_e::Return:
                    { break; }
                    case return_e::Error:
                    default:
                    {
                        EMP_LOG_ERROR("unhandled error");
                        return -1;
                    }
                }
            }*/
            //continue;
        //}
    }

    a_rStack.halt(oWalker.pointer());
    return oWalker.pointer() <= stCount ? 1 : 0;
}
EMP_PRAGMA_POP_IGNORE(EMP_W_DISABLED_OPTIMIZATION)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

