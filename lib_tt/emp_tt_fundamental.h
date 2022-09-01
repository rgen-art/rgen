//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_FUNDAMENTAL_H
#define EMP_TT_FUNDAMENTAL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_logical.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_UNREFERENCED_INLINE_FUNCTION)
EMP_TT_DECLARE_VALUE(is_void, false_)
EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_void, void, true_)
EMP_TT_DECLARE_VALUE(is_not_void, not_<is_void<T>::value>)
EMP_PRAGMA_POP_IGNORE(EMP_W_UNREFERENCED_INLINE_FUNCTION)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_integral, false_)

#define XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signedness, x_Distinct, x_Signed)\
EMP_PP_IF(x_Integral,\
    EMP_PP_IF_ELSE(x_Signedness,\
        EMP_PP_IF_ELSE(x_Distinct,\
            EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_integral, x_Type, true_)\
            EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_integral, signed x_Type, true_)\
            EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_integral, unsigned x_Type, true_)\
            ,\
            EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_integral, x_Type, true_)\
            EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_integral, unsigned x_Type, true_))\
        ,\
        EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_integral, x_Type, true_)))
EMP_TT_TYPES()
#undef XTMP_TT_TYPE

EMP_TT_DECLARE_VALUE(is_not_integral, not_<is_integral<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_real, false_)

#define XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signedness, x_Distinct, x_Signed)\
EMP_PP_IF(EMP_PP_NOT(x_Integral), EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_real, x_Type, true_))
EMP_TT_TYPES()
#undef XTMP_TT_TYPE

EMP_TT_DECLARE_VALUE(is_not_real, not_<is_real<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_arithmetic, (or_<is_integral<T>::value, is_real<T>::value>))
EMP_TT_DECLARE_VALUE(is_not_arthmetic, not_<is_arithmetic<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_fundamental, (or_<is_arithmetic<T>::value, is_void<T>::value>))
EMP_TT_DECLARE_VALUE(is_not_fundamental, not_<is_fundamental<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_NOINSTANCE(struct, signed_t) };
#define XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signdness, x_Distinct, x_Signed)\
EMP_PP_IF(x_Signdness,\
\
    EMP_PP_IF(x_Distinct,\
    \
template <> EMP_NOINSTANCE(struct, signed_t< signed x_Type const volatile >) typedef signed x_Type type; };\
template <> EMP_NOINSTANCE(struct, signed_t< signed x_Type       volatile >) typedef signed x_Type type; };\
template <> EMP_NOINSTANCE(struct, signed_t< signed x_Type const          >) typedef signed x_Type type; };\
template <> EMP_NOINSTANCE(struct, signed_t< signed x_Type                >) typedef signed x_Type type; };\
    )\
template <> EMP_NOINSTANCE(struct, signed_t<        x_Type const volatile >) typedef signed x_Type type; };\
template <> EMP_NOINSTANCE(struct, signed_t<        x_Type volatile       >) typedef signed x_Type type; };\
template <> EMP_NOINSTANCE(struct, signed_t<        x_Type const          >) typedef signed x_Type type; };\
template <> EMP_NOINSTANCE(struct, signed_t<        x_Type                >) typedef signed x_Type type; };\
)
EMP_TT_TYPES()
#undef XTMP_TT_TYPE

template <typename T> using signed_ = typename signed_t<T>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_NOINSTANCE(struct, unsigned_t) };
template <> EMP_NOINSTANCE(struct, unsigned_t<bool>) typedef bool type; };

#define XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signdness, x_Distinct, x_Signed)\
EMP_PP_IF(x_Signdness,\
\
template <> EMP_NOINSTANCE(struct, unsigned_t< unsigned x_Type const volatile > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t< unsigned x_Type       volatile > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t< unsigned x_Type const          > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t< unsigned x_Type                > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t<          x_Type const volatile > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t<          x_Type       volatile > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t<          x_Type const          > ) typedef unsigned x_Type type; };\
template <> EMP_NOINSTANCE(struct, unsigned_t<          x_Type                > ) typedef unsigned x_Type type; };\
)
EMP_TT_TYPES()
#undef XTMP_TT_TYPE

template <typename T> using unsigned_ = typename unsigned_t<T>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_signed, false_)

#define XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signedness, x_Distinct, x_Signed)\
EMP_PP_IF(EMP_PP_AND(x_Integral, x_Signedness, x_Distinct, x_Signed),\
    EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_signed, x_Type, true_)\
    EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_signed, signed x_Type, true_))\
EMP_PP_IF(EMP_PP_AND(x_Integral, x_Signedness, x_Distinct, EMP_PP_NOT(x_Signed)),\
    EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_signed, signed x_Type, true_))\
EMP_PP_IF(EMP_PP_AND(x_Integral, x_Signedness, EMP_PP_NOT(x_Distinct), x_Signed),\
    EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_signed, x_Type, true_))\
EMP_PP_IF(EMP_PP_AND(x_Integral, EMP_PP_NOT(x_Signedness), EMP_PP_NOT(x_Distinct), x_Signed),\
    EMP_TT_DECLARE_VALUE_SPECIAL_CV(is_signed, x_Type, true_))
EMP_TT_TYPES()
#undef XTMP_TT_TYPE

EMP_TT_DECLARE_VALUE(is_not_signed, not_<is_signed<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

