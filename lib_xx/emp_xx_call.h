//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_CALL_H
#define EMP_XX_CALL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_if_not.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_COMPILER_GCC
#define EMP_XX_DEBUG_BREAK __builtin_trap()
#elif defined EMP_XX_COMPILER_LLVM
#define EMP_XX_DEBUG_BREAK __asm__ volatile("int $0x03")
#else
#endif

#if defined EMP_XX_DEBUG
#define EMP_XX_ERROR_HOOK
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_ERROR_HOOK
#define EMP_XX_HOOK() emp::xx::error_hook();
namespace emp
{
    namespace xx
    {
        void error_hook(void);
    }
}
#else
#define EMP_XX_HOOK()
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RET_X_IF_X_1(x_Test, x_Void, x_Return, x_Call)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
if ((x_Call) x_Test)\
{\
    EMP_XX_IF_LOG(EMP_LOG_ERROR(EMP_PP_STRING(x_Call))); \
    EMP_XX_HOOK()\
    return EMP_PP_IF_NOT(x_Void, x_Return);\
}\
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RET_X_IF_X_0(x_Test, x_Void, x_Return, x_Call, ...)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
if ((x_Call) x_Test)\
{\
    EMP_XX_IF_LOG(EMP_LOG_ERROR(EMP_PP_STRING(x_Call)));\
    EMP_XX_IF_LOG(EMP_PP_FORWARD(EMP_LOG_ERROR(__VA_ARGS__)));\
    EMP_XX_HOOK()\
    return EMP_PP_IF_NOT(x_Void, x_Return); \
}\
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RET_X_1(x_Void, x_Return)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
    EMP_XX_IF_LOG(EMP_LOG_ERROR(EMP_PP_STRING(x_Return)));\
    EMP_XX_HOOK()\
    return EMP_PP_IF_NOT(x_Void, x_Return); \
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RET_X_0(x_Void, x_Return, ...)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
    EMP_XX_IF_LOG(EMP_LOG_ERROR(EMP_PP_STRING(x_Return))); \
    EMP_XX_IF_LOG(EMP_PP_FORWARD(EMP_LOG_ERROR(__VA_ARGS__))); \
    EMP_XX_HOOK()\
    return EMP_PP_IF_NOT(x_Void, x_Return); \
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RET_IF_X(x_Test, ...)\
    EMP_PP_FORWARD(EMP_PP_CAT(EMP_RET_X_IF_X_, EMP_PP_BOOL(EMP_PP_EQUAL(1, EMP_VAARGS_COUNT(__VA_ARGS__))))(x_Test, true, void, __VA_ARGS__))
    
#define EMP_RET_X_IF_X(x_Test, x_Return, ...)\
    EMP_PP_FORWARD(EMP_PP_CAT(EMP_RET_X_IF_X_, EMP_PP_BOOL(EMP_PP_EQUAL(1, EMP_VAARGS_COUNT(__VA_ARGS__))))(x_Test, false, x_Return, __VA_ARGS__))

#define EMP_RET_X(...)\
    EMP_PP_FORWARD(EMP_PP_CAT(EMP_RET_X_, EMP_PP_BOOL(EMP_PP_EQUAL(1, EMP_VAARGS_COUNT(__VA_ARGS__))))(false, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RET_IF(           ...)  EMP_PP_FORWARD(EMP_RET_IF_X(== true  , __VA_ARGS__ ))
#define EMP_RET_IF_NOT(       ...)  EMP_PP_FORWARD(EMP_RET_IF_X(== false , __VA_ARGS__ ))

#define EMP_RET_X_IF(         ...)  EMP_PP_FORWARD(EMP_RET_X_IF_X(== true  , __VA_ARGS__ ))
#define EMP_RET_X_IF_NOT(     ...)  EMP_PP_FORWARD(EMP_RET_X_IF_X(== false , __VA_ARGS__ ))

#define EMP_RET_FALSE                              EMP_RET_X(false)
#define EMP_RET_FALSE_(       ...)  EMP_PP_FORWARD(EMP_RET_X(false, __VA_ARGS__))
#define EMP_RET_FALSE_IF(     ...)  EMP_PP_FORWARD(EMP_RET_X_IF(false , __VA_ARGS__ ))
#define EMP_RET_FALSE_IF_NOT( ...)  EMP_PP_FORWARD(EMP_RET_X_IF_NOT(false , __VA_ARGS__ ))

#define EMP_RET_NULL                               EMP_RET_X(nullptr)
#define EMP_RET_NULL_(        ...)  EMP_PP_FORWARD(EMP_RET_X(nullptr, __VA_ARGS__))
#define EMP_RET_NULL_IF(      ...)  EMP_PP_FORWARD(EMP_RET_X_IF(nullptr , __VA_ARGS__ ))
#define EMP_RET_NULL_IF_NOT(  ...)  EMP_PP_FORWARD(EMP_RET_X_IF_NOT(nullptr , __VA_ARGS__ ))

#define EMP_RET_NEG1                               EMP_RET_X(-1)
#define EMP_RET_NEG1_(        ...)  EMP_PP_FORWARD(EMP_RET_X(-1, __VA_ARGS__))
#define EMP_RET_NEG1_IF(      ...)  EMP_PP_FORWARD(EMP_RET_X_IF(-1 , __VA_ARGS__ ))
#define EMP_RET_NEG1_IF_NOT(  ...)  EMP_PP_FORWARD(EMP_RET_X_IF_NOT(-1 , __VA_ARGS__ ))

#define EMP_RET_MAX(       x_Type     ) EMP_RET_X(emp::tt::max<x_Type>::value)
#define EMP_RET_MAX_(      x_Type, ...) EMP_PP_FORWARD(EMP_RET_X(emp::tt::max<x_Type>::value, __VA_ARGS__))
#define EMP_RET_MAX_IF(    x_Type, ...) EMP_PP_FORWARD(EMP_RET_X_IF(emp::tt::max<x_Type>::value, __VA_ARGS__))
#define EMP_RET_MAX_IF_NOT(x_Type, ...) EMP_PP_FORWARD(EMP_RET_X_IF_NOT(emp::tt::max<x_Type>::value , __VA_ARGS__ ))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

