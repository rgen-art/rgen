//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_TRAIT_H
#define EMP_TT_TRAIT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 : check inclusion CHAR_MIN WCHAR_MIN etc
#include "emp_hh_float.h"
#include "emp_hh_limits.h"
#include "emp_hh_wchar.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_INLINE bool not_const(c_bool EMP_SILENT(a_bCondition));
//EMP_INLINE bool not_const(c_bool EMP_SILENT(a_bCondition))
//{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_INLINE constexpr bool const_assert(c_bool a_bCondition)
//{ return a_bCondition ? true : not_const(a_bCondition); }

#if defined EMP_XX_ASSERT
#define const_assert(...) assert(__VA_ARGS__)
#else
#define const_assert(...)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if (defined(CHAR_MIN) && (CHAR_MIN != 0))
#define EMP_TT_CHAR_EMP_TT_MIN CHAR_MIN
#else
#define EMP_TT_CHAR_EMP_TT_MIN 0
#endif
#define EMP_TT_CHAR_EMP_TT_MAX CHAR_MAX

#define EMP_TT_CHAR_EMP_TT_S_MIN SCHAR_MIN
#define EMP_TT_CHAR_EMP_TT_S_MAX SCHAR_MIN

#define EMP_TT_CHAR_EMP_TT_U_MIN 0
#define EMP_TT_CHAR_EMP_TT_U_MAX UCHAR_MAX

#define EMP_TT_CHAR_MIN EMP_TT_CHAR_EMP_TT_MIN
#define EMP_TT_CHAR_MAX EMP_TT_CHAR_EMP_TT_MAX

#define EMP_TT_CHAR_S_MIN EMP_TT_CHAR_EMP_TT_S_MIN
#define EMP_TT_CHAR_S_MAX EMP_TT_CHAR_EMP_TT_S_MAX

#define EMP_TT_CHAR_U_MIN EMP_TT_CHAR_EMP_TT_U_MIN
#define EMP_TT_CHAR_U_MAX EMP_TT_CHAR_EMP_TT_U_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TODO : #if wchar
#if (defined(WCHAR_MIN) && (WCHAR_MIN != 0))
#define EMP_TT_WCHAR_EMP_TT_MIN WCHAR_MIN
#else
#define EMP_TT_WCHAR_EMP_TT_MIN 0
#endif
#define EMP_TT_WCHAR_EMP_TT_MAX WCHAR_MAX

#define EMP_TT_WCHAR_MIN EMP_TT_WCHAR_EMP_TT_MIN
#define EMP_TT_WCHAR_MAX EMP_TT_WCHAR_EMP_TT_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_BOOL_EMP_TT_MIN 0
#define EMP_TT_BOOL_EMP_TT_MAX 1

#define EMP_TT_BOOL_MIN EMP_TT_BOOL_EMP_TT_MIN
#define EMP_TT_BOOL_MAX EMP_TT_BOOL_EMP_TT_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_SHORT_EMP_TT_MIN SHRT_MIN
#define EMP_TT_SHORT_EMP_TT_MAX SHRT_MAX

#define EMP_TT_SHORT_EMP_TT_U_MIN 0
#define EMP_TT_SHORT_EMP_TT_U_MAX USHRT_MAX

#define EMP_TT_SHORT_MIN EMP_TT_SHORT_EMP_TT_MIN
#define EMP_TT_SHORT_MAX EMP_TT_SHORT_EMP_TT_MAX

#define EMP_TT_SHORT_U_MIN EMP_TT_SHORT_EMP_TT_U_MIN
#define EMP_TT_SHORT_U_MAX EMP_TT_SHORT_EMP_TT_U_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_INT_EMP_TT_MIN INT_MIN
#define EMP_TT_INT_EMP_TT_MAX INT_MAX

#define EMP_TT_INT_EMP_TT_U_MIN 0
#define EMP_TT_INT_EMP_TT_U_MAX UINT_MAX

#define EMP_TT_INT_MIN EMP_TT_INT_EMP_TT_MIN
#define EMP_TT_INT_MAX EMP_TT_INT_EMP_TT_MAX

#define EMP_TT_INT_U_MIN EMP_TT_INT_EMP_TT_U_MIN
#define EMP_TT_INT_U_MAX EMP_TT_INT_EMP_TT_U_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_LONG_EMP_TT_MIN LONG_MIN
#define EMP_TT_LONG_EMP_TT_MAX LONG_MAX

#define EMP_TT_LONG_EMP_TT_U_MIN 0
#define EMP_TT_LONG_EMP_TT_U_MAX ULONG_MAX

#define EMP_TT_LONG_MIN EMP_TT_LONG_EMP_TT_MIN
#define EMP_TT_LONG_MAX EMP_TT_LONG_EMP_TT_MAX

#define EMP_TT_LONG_U_MIN EMP_TT_LONG_EMP_TT_U_MIN
#define EMP_TT_LONG_U_MAX EMP_TT_LONG_EMP_TT_U_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_LONG_LONG_ENABLE

#if defined(EMP_TT_LONG_LONG_ENABLE)
#define EMP_TT_LONG_LONG_EMP_TT_MIN LLONG_MIN
#define EMP_TT_LONG_LONG_EMP_TT_MAX LLONG_MAX

#define EMP_TT_LONG_LONG_EMP_TT_U_MIN 0
#define EMP_TT_LONG_LONG_EMP_TT_U_MAX ULLONG_MAX

#define EMP_TT_LONG_LONG_MIN EMP_TT_LONG_LONG_EMP_TT_MIN
#define EMP_TT_LONG_LONG_MAX EMP_TT_LONG_LONG_EMP_TT_MAX

#define EMP_TT_LONG_LONG_U_MIN EMP_TT_LONG_LONG_EMP_TT_U_MIN
#define EMP_TT_LONG_LONG_U_MAX EMP_TT_LONG_LONG_EMP_TT_U_MAX
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_FLOAT_EMP_TT_MIN -FLT_MAX
#define EMP_TT_FLOAT_EMP_TT_MAX FLT_MAX

#define EMP_TT_FLOAT_MIN EMP_TT_FLOAT_EMP_TT_MIN
#define EMP_TT_FLOAT_MAX EMP_TT_FLOAT_EMP_TT_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DOUBLE_EMP_TT_MIN -DBL_MAX
#define EMP_TT_DOUBLE_EMP_TT_MAX DBL_MAX

#define EMP_TT_DOUBLE_MIN EMP_TT_DOUBLE_EMP_TT_MIN
#define EMP_TT_DOUBLE_MAX EMP_TT_DOUBLE_EMP_TT_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_LONG_DOUBLE_EMP_TT_MIN LDBL_MIN
#define EMP_TT_LONG_DOUBLE_EMP_TT_MAX LDBL_MAX

#define EMP_TT_LONG_DOUBLE_MIN EMP_TT_LONG_DOUBLE_EMP_TT_MIN
#define EMP_TT_LONG_DOUBLE_MAX EMP_TT_LONG_DOUBLE_EMP_TT_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if (EMP_TT_CHAR_MIN != 0)
#define EMP_TT_CHAR_SIGNED true
#else
#define EMP_TT_CHAR_SIGNED false
#endif

#if (EMP_TT_WCHAR_MIN != 0)
#define EMP_TT_WCHAR_SIGNED true
#else
#define EMP_TT_WCHAR_SIGNED false
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined (EMP_TT_LONG_LONG_ENABLE)
#define EMP_TT_IF_LONG_LONG(x_Then) x_Then
#else
#define EMP_TT_IF_LONG_LONG(x_Then)
#endif


//-----------------------------------------------------------------------------
// XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signdness, x_Distinct, x_Signed)
//-----------------------------------------------------------------------------
#define EMP_TT_TYPES()\
EMP_TT_IF_LONG_LONG(\
XTMP_TT_TYPE( long long int , EMP_TT_LONG_LONG   , true  , true  , false , true                ))\
XTMP_TT_TYPE( long int      , EMP_TT_LONG        , true  , true  , false , true                )\
XTMP_TT_TYPE( int           , EMP_TT_INT         , true  , true  , false , true                )\
XTMP_TT_TYPE( short         , EMP_TT_SHORT       , true  , true  , false , true                )\
XTMP_TT_TYPE( bool          , EMP_TT_BOOL        , true  , false , false , false               )\
XTMP_TT_TYPE( wchar_t       , EMP_TT_WCHAR       , true  , false , false , EMP_TT_WCHAR_SIGNED )\
XTMP_TT_TYPE( char          , EMP_TT_CHAR        , true  , true  , true  , EMP_TT_CHAR_SIGNED  )\
XTMP_TT_TYPE( long double   , EMP_TT_LONG_DOUBLE , false , false , false , false               )\
XTMP_TT_TYPE( double        , EMP_TT_DOUBLE      , false , false , false , false               )\
XTMP_TT_TYPE( float         , EMP_TT_FLOAT       , false , false , false , false               )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_TYPE(x_Trait, ...)\
    template <typename T> EMP_PP_FORWARD(EMP_NOINSTANCE(struct, x_Trait)) typedef __VA_ARGS__ type; };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_TYPE_PARTIAL(x_Trait, x_Partial, ...)\
    template <typename T> EMP_PP_FORWARD(EMP_NOINSTANCE(struct, x_Trait<x_Partial>)) typedef __VA_ARGS__ type; };
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE(x_Trait, ...)\
    template <typename T> EMP_PP_FORWARD(EMP_NOINSTANCE_BASE(struct, x_Trait, public, __VA_ARGS__)) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE_PARTIAL_SPECIAL(x_bPartial, x_Trait, x_Partial, ...)\

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE_PARTIAL(x_Trait, x_Partial, ...)\
    template <typename T>\
    EMP_PP_FORWARD(EMP_NOINSTANCE_BASE(struct, x_Trait<x_Partial>, public, __VA_ARGS__)) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE_SPECIAL(x_Trait, x_Special, ...)\
    template <>\
    EMP_PP_FORWARD(EMP_NOINSTANCE_BASE(struct, x_Trait<x_Special>, public, __VA_ARGS__)) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE_PARTIAL_SPECIAL_CV(x_bPartial, x_Trait, x_Type, ...)\
    EMP_PP_FORWARD(EMP_PP_IF_ELSE(  x_bPartial,\
                EMP_TT_DECLARE_VALUE_PARTIAL,\
                EMP_TT_DECLARE_VALUE_SPECIAL)(x_Trait, x_Type, __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_PP_IF_ELSE(  x_bPartial,\
                EMP_TT_DECLARE_VALUE_PARTIAL,\
                EMP_TT_DECLARE_VALUE_SPECIAL)(x_Trait, x_Type const, __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_PP_IF_ELSE(  x_bPartial,\
                EMP_TT_DECLARE_VALUE_PARTIAL,\
                EMP_TT_DECLARE_VALUE_SPECIAL)(x_Trait, x_Type volatile, __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_PP_IF_ELSE(  x_bPartial,\
                EMP_TT_DECLARE_VALUE_PARTIAL,\
                EMP_TT_DECLARE_VALUE_SPECIAL)(x_Trait, x_Type const volatile, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE_PARTIAL_CV(x_Trait, x_Type, ...)\
    EMP_PP_FORWARD(EMP_TT_DECLARE_VALUE_PARTIAL_SPECIAL_CV(true, x_Trait, x_Type, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_DECLARE_VALUE_SPECIAL_CV(x_Trait, x_Type, ...)\
    EMP_PP_FORWARD(EMP_TT_DECLARE_VALUE_SPECIAL(x_Trait, x_Type                , __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_TT_DECLARE_VALUE_SPECIAL(x_Trait, x_Type const          , __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_TT_DECLARE_VALUE_SPECIAL(x_Trait, x_Type volatile       , __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_TT_DECLARE_VALUE_SPECIAL(x_Trait, x_Type const volatile , __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

