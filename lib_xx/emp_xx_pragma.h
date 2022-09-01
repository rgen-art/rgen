//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_PRAGMA_H
#define EMP_XX_PRAGMA_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_COMPILER_MSC
#define EMP_MSC_W_C4548 4548
#define EMP_W_COMPARISON_ALWAYS_TRUE 4296
#define EMP_W_OLD_STYLE_CAST  4001 // @@0 implement
#define EMP_W_FLOAT_EQUAL  4001 // @@0 implement
#define EMP_W_FORMAT_NONLITERAL 4001 // @@0 implement
#define EMP_W_FORMAT_SECURITY 4001 // @@0 implement
#define EMP_W_UNUSED_PRIVATE_FIELD 4001 // @@0 implement
#define EMP_W_EXIT_TIME_DESTRUCTORS 4001 // @@0 implement
#define EMP_W_EFFCPP 4001 // @@0 implement
#define EMP_W_STATIC_DATA_DEFAULT_CTOR 4268
#define EMP_W_PADDING_ADDED 4820
#define EMP_W_UNREFERENCED_INLINE_FUNCTION 4514
#define EMP_W_NOEXPECT 4577
#define EMP_W_SIGN_COMPARE 4001
#define EMP_W_4541 4541
#define EMP_W_4548 4548
#define EMP_W_4265 4265
#define EMP_W_4668 4668
#define EMP_W_4592 4592
#define EMP_W_4324 4324
#define EMP_W_UNSAFE_REINTERPRET_CAST 4191
#define EMP_W_THIS_IN_MIL 4355
#define EMP_W_UNREFERENCED_PARAMETER 4100
#define EMP_W_INLINE 4001 // @@0 implement
#define EMP_W_ERROR_INLINE 4001 // @@0 implement
#define EMP_W_DISABLED_OPTIMIZATION 4001 // @@0 implement
#define EMP_W_SETJMP_CPP 4611
#define EMP_W_SIGNED_UNSIGNED_MISMATCH 4365
#define EMP_W_COPY_CTOR_IMPLICITLY_DEFINED_DELETED 4625
#define EMP_W_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED 4626
#define EMP_W_MOVE_CTOR_IMPLICITLY_DEFINED_DELETED 5026
#define EMP_W_MOVE_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED 5027
#define EMP_W_FORMAT_STRING_NOT_LITERAL 4774
#define EMP_W_NOT_DEFINED_AS_PREPROCESSOR_MACRO 4668
#define EMP_W_FUNCTION_NOT_INLINED 4710
#define EMP_W_CONDITIONAL_EXPRESSION_IS_CONSTANT 4127
#define EMP_W_INHERITS_VIA_DOMINANCE 4250 // @@0 debug
#define EMP_W_LAYOUT_CHANGE_VD2 4435 // @@0 debug
#define EMP_W_CONVERSION_OF_GREATER_SIZE 4312 // @@0 debug

#define EMP_W_STL\
    EMP_W_SIGNED_UNSIGNED_MISMATCH\
    EMP_W_PADDING_ADDED\
    EMP_W_COPY_CTOR_IMPLICITLY_DEFINED_DELETED\
    EMP_W_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED\
    EMP_W_MOVE_CTOR_IMPLICITLY_DEFINED_DELETED\
    EMP_W_MOVE_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED\
    EMP_W_FORMAT_STRING_NOT_LITERAL\
    EMP_W_NOT_DEFINED_AS_PREPROCESSOR_MACRO\
    EMP_W_FUNCTION_NOT_INLINED\
    EMP_W_UNREFERENCED_INLINE_FUNCTION

#define EMP_PRAGMA_PUSH_IGNORE_STL EMP_PRAGMA_PUSH_IGNORE(EMP_W_STL)
#define EMP_PRAGMA_POP_IGNORE_STL EMP_PRAGMA_POP_IGNORE(EMP_W_STL)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#elif defined(EMP_XX_COMPILER_LLVM)

#define EMP_MSC_W_C4548 "-Wformat-security"
#define EMP_W_COMPARISON_ALWAYS_TRUE "-Wformat-security" // @@0 implement
#define EMP_W_OLD_STYLE_CAST "-Wold-style-cast"
#define EMP_W_FLOAT_EQUAL "-Wfloat-equal"
#define EMP_W_FORMAT_NONLITERAL "-Wformat-nonliteral"
#define EMP_W_FORMAT_SECURITY "-Wformat-security"
#define EMP_W_UNUSED_PRIVATE_FIELD "-Wunused-private-field"
#define EMP_W_EXIT_TIME_DESTRUCTORS "-Wexit-time-destructors" // @@0 llvm osx not on llvm win ?
#define EMP_W_EFFCPP "-Weffc++"
#define EMP_W_STATIC_DATA_DEFAULT_CTOR "-Wformat-security" // @@0 implement
#define EMP_W_PADDING_ADDED "-Wformat-security" // @@0 implement
#define EMP_W_UNREFERENCED_INLINE_FUNCTION "-Wformat-security" // @@0 implement
#define EMP_W_NOEXPECT "-Wformat-security"
#define EMP_W_SIGN_COMPARE "-Wsign-compare"
#define EMP_W_4541 "-Wformat-security" // @@0 implement
#define EMP_W_4548 "-Wformat-security" // @@0 implement
#define EMP_W_4265 "-Wformat-security" // @@0 implement
#define EMP_W_4668  "-Wformat-security" // @@0 implement
#define EMP_W_4592  "-Wformat-security" // @@0 implement
#define EMP_W_4324  "-Wformat-security" // @@0 implement
#define EMP_W_UNSAFE_REINTERPRET_CAST "-Wformat-security" // @@0 implement
#define EMP_W_UNREFERENCED_PARAMETER "-Wformat-security" // @@0 implement
#define EMP_W_SETJMP_CPP "-Wformat-security" // @@0 implement
#define EMP_W_INLINE "-Winline"
#define EMP_W_ERROR_INLINE "-Wformat-security" // @@0 implement
#define EMP_W_THIS_IN_MIL "-Wformat-security" // @@0 implement
#define EMP_W_DISABLED_OPTIMIZATION "-Wdisabled-optimization"
#define EMP_W_SIGNED_UNSIGNED_MISMATCH "-Wformat-security" // @@0 implement
#define EMP_W_COPY_CTOR_IMPLICITLY_DEFINED_DELETED "-Wformat-security" // @@0 implement
#define EMP_W_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED "-Wformat-security" // @@0 implement
#define EMP_W_MOVE_CTOR_IMPLICITLY_DEFINED_DELETED "-Wformat-security" // @@0 implement
#define EMP_W_MOVE_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED "-Wformat-security" // @@0 implement
#define EMP_W_FORMAT_STRING_NOT_LITERAL "-Wformat-security" // @@0 implement
#define EMP_W_NOT_DEFINED_AS_PREPROCESSOR_MACRO "-Wformat-security" // @@0 implement
#define EMP_W_FUNCTION_NOT_INLINED "-Wformat-security" // @@0 implement
#define EMP_W_LAYOUT_CHANGE_VD2 "-Wformat-security" // @@0 implement
#define EMP_W_INHERITS_VIA_DOMINANCE "-Wformat-security" // @@0 implement
#define EMP_W_IMPLICIT_FALLTHROUGH "-Wimplicit-fallthrough"



#define EMP_PRAGMA_PUSH_IGNORE_STL
#define EMP_PRAGMA_POP_IGNORE_STL

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#elif defined(EMP_XX_COMPILER_GCC)

#define EMP_MSC_W_C4548 "-Wformat-security"
#define EMP_W_COMPARISON_ALWAYS_TRUE "-Wformat-security" // @@0 implement
#define EMP_W_OLD_STYLE_CAST "-Wold-style-cast"
#define EMP_W_FLOAT_EQUAL "-Wfloat-equal"
#define EMP_W_FORMAT_NONLITERAL "-Wformat-nonliteral"
#define EMP_W_FORMAT_SECURITY "-Wformat-security"
#define EMP_W_UNUSED_PRIVATE_FIELD "-Wunused-private-field"
#define EMP_W_EXIT_TIME_DESTRUCTORS "-Wexit-time-destructors"
#define EMP_W_EFFCPP "-Weffc++"
#define EMP_W_STATIC_DATA_DEFAULT_CTOR "-Wformat-security" // @@0 implement
#define EMP_W_PADDING_ADDED "-Wformat-security" // @@0 implement
#define EMP_W_UNREFERENCED_INLINE_FUNCTION "-Wformat-security" // @@0 implement
#define EMP_W_NOEXPECT "-Wformat-security"
#define EMP_W_SIGN_COMPARE "-Wsign-compare"
#define EMP_W_4541 "-Wformat-security" // @@0 implement
#define EMP_W_4548 "-Wformat-security" // @@0 implement
#define EMP_W_4265 "-Wformat-security" // @@0 implement
#define EMP_W_4668  "-Wformat-security" // @@0 implement
#define EMP_W_4592  "-Wformat-security" // @@0 implement
#define EMP_W_4324  "-Wformat-security" // @@0 implement
#define EMP_W_UNSAFE_REINTERPRET_CAST "-Wcast-function-type"
#define EMP_W_UNREFERENCED_PARAMETER "-Wformat-security" // @@0 implement
#define EMP_W_SETJMP_CPP "-Wformat-security" // @@0 implement
#define EMP_W_INLINE "-Winline"
#define EMP_W_ERROR_INLINE "-Wformat-security"
#define EMP_W_THIS_IN_MIL "-Wformat-security" // @@9 implement
#define EMP_W_DISABLED_OPTIMIZATION "-Wdisabled-optimization"
#define EMP_W_FUNCTION_NOT_INLINED "-Wformat-security"
#define EMP_W_IMPLICIT_FALLTHROUGH "-Wimplicit-fallthrough"

#define EMP_PRAGMA_PUSH_IGNORE_STL
#define EMP_PRAGMA_POP_IGNORE_STL

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_COMPILER_MSC
#define EMP_PRAGMA_SET_IGNORE(x_Value)\
    __pragma(warning(disable:x_Value))

#define EMP_PRAGMA_PUSH_IGNORE(x_Value)\
    __pragma(warning(push))\
    EMP_PRAGMA_SET_IGNORE(x_Value)

#define EMP_PRAGMA_POP_IGNORE(x_Value)\
    __pragma(warning(pop))

#define EMP_PRAGMA_PUSH_PACK(x_Value)\
    __pragma(pack(push, x_Value))

#define EMP_PRAGMA_POP_PACK()\
    __pragma(pack(pop))

#else
#define EMP_DIAGNOSTIC(x_Value) GCC diagnostic x_Value

#define EMP_DIAGNOSTIC_IGNORED ignored
#define EMP_DIAGNOSTIC_WARNING warning
#define EMP_DIAGNOSTIC_ERROR error

#define EMP_PRAGMA(x_Value) _Pragma(EMP_PP_STRING(x_Value))
#define EMP_PRAGMA_SET(x_State, x_Value) EMP_PRAGMA(EMP_DIAGNOSTIC(x_State), x_Value)
#define EMP_PRAGMA_SET_IGNORE(x_Value) EMP_PRAGMA(EMP_DIAGNOSTIC(EMP_DIAGNOSTIC_IGNORED) x_Value)
#define EMP_PRAGMA_SET_ERROR(x_Value) EMP_PRAGMA(EMP_DIAGNOSTIC(EMP_DIAGNOSTIC_ERROR) x_Value)

#define EMP_PRAGMA_PUSH_IGNORE(x_Value) EMP_PRAGMA_SET_IGNORE(x_Value)
#define EMP_PRAGMA_POP_IGNORE(x_Value) EMP_PRAGMA_SET_ERROR(x_Value) // @@0 set error ?

// @@0 implement pack
#define EMP_PRAGMA_PUSH_PACK(x_Value)
#define EMP_PRAGMA_POP_PACK(x_Value)
#endif

#define EMP_PRAGMA_IGNORE_W(x_Warning, ...)\
EMP_PRAGMA_PUSH_IGNORE(x_Warning)\
    __VA_ARGS__ \
EMP_PRAGMA_POP_IGNORE(x_Warning)

#define EMP_PRAGMA_IGNORE_W_PADDING_ADDED(...) EMP_PRAGMA_IGNORE_W(EMP_W_PADDING_ADDED, __VA_ARGS__)
#define EMP_PRAGMA_IGNORE_W_UNSAFE_REINTERPRET_CAST(...) EMP_PRAGMA_IGNORE_W(EMP_W_UNSAFE_REINTERPRET_CAST, __VA_ARGS__)
#define EMP_PRAGMA_IGNORE_W_THIS_IN_MIL(...) EMP_PRAGMA_IGNORE_W(EMP_W_THIS_IN_MIL, __VA_ARGS__)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

