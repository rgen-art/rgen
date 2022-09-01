//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_LIMIT_H
#define EMP_TT_LIMIT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#include "emp_tt_literal.h"
#include "emp_tt_dereference.h"
#include "emp_tt_trait.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_math.h"
#include "emp_hh_limits.h"
#include "emp_hh_assert.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_TT_MIN(x_Name) EMP_PP_CAT(x_Name, _EMP_TT_MIN)
#define XTMP_TT_MAX(x_Name) EMP_PP_CAT(x_Name, _EMP_TT_MAX)

#define XTMP_TT_UMIN(x_Name) EMP_PP_CAT(x_Name, _EMP_TT_U_MIN)
#define XTMP_TT_UMAX(x_Name) EMP_PP_CAT(x_Name, _EMP_TT_U_MAX)

#define XTMP_TT_SMIN(x_Name) EMP_PP_CAT(x_Name, _EMP_TT_S_MIN)
#define XTMP_TT_SMAX(x_Name) EMP_PP_CAT(x_Name, _EMP_TT_S_MAX)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_NOINSTANCE(struct, min) };
template <typename T> EMP_NOINSTANCE(struct, max) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_TT_TYPE_IMPL(x_Type, x_Name, x_Value)\
template <> EMP_NOINSTANCE(struct, x_Name<x_Type>) static constexpr x_Type const value = x_Value; };
//constexpr x_Type const x_Name<x_Type>::value;


#define XTMP_TT_TYPE(x_Type, x_Name, x_Integral, x_Signedness, x_Distinct, x_Signed)\
XTMP_TT_TYPE_IMPL(x_Type, min, XTMP_TT_MIN(x_Name))\
XTMP_TT_TYPE_IMPL(x_Type, max, XTMP_TT_MAX(x_Name))\
EMP_PP_IF(x_Signedness,\
    XTMP_TT_TYPE_IMPL(unsigned x_Type, min, XTMP_TT_UMIN(x_Name))\
    XTMP_TT_TYPE_IMPL(unsigned x_Type, max, XTMP_TT_UMAX(x_Name))\
    EMP_PP_IF(x_Distinct,\
        XTMP_TT_TYPE_IMPL(signed x_Type, min, XTMP_TT_SMIN(x_Name))\
        XTMP_TT_TYPE_IMPL(signed x_Type, max, XTMP_TT_SMAX(x_Name))))


EMP_TT_TYPES()
#undef XTMP_TT_TYPE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN T epsilon(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_INLINE EMP_RETURN float epsilon(void)
{
    return 0.00001f;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_inf(c_float f); // @@0 remove declaration ?
EMP_INLINE EMP_RETURN bool is_inf(c_float f) { return std::isinf(f); }

EMP_INLINE EMP_RETURN bool is_inf(c_double d);
EMP_INLINE EMP_RETURN bool is_inf(c_double d) { return std::isinf(d); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float inff(void);
EMP_INLINE EMP_RETURN float inff(void) { return std::numeric_limits<float>::infinity(); }

EMP_INLINE EMP_RETURN double infd(void);
EMP_INLINE EMP_RETURN double infd(void) { return std::numeric_limits<double>::infinity(); }

template <typename T> EMP_RETURN T inf(void);
template <> EMP_INLINE  EMP_RETURN float inf<float>(void) { return inff(); }
template <> EMP_INLINE EMP_RETURN double inf<double>(void) { return infd(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_nan(c_float f);
EMP_INLINE EMP_RETURN bool is_nan(c_float f) { return std::isnan(f); }

EMP_INLINE EMP_RETURN bool is_nan(c_double d);
EMP_INLINE EMP_RETURN bool is_nan(c_double d) { return std::isnan(d); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
constexpr EMP_INLINE EMP_RETURN float nanf(void);
constexpr EMP_INLINE EMP_RETURN float nanf(void) { return std::numeric_limits<float>::quiet_NaN(); }

constexpr EMP_INLINE EMP_RETURN double nand(void);
constexpr EMP_INLINE EMP_RETURN double nand(void) { return std::numeric_limits<double>::quiet_NaN(); }

template <typename T> EMP_RETURN T nan(void);
template <> constexpr EMP_INLINE EMP_RETURN float nan<float>(void) { return nanf(); }
template <> constexpr EMP_INLINE EMP_RETURN double nan<double>(void) { return nand(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
constexpr EMP_INLINE EMP_RETURN bool is_valid(T)
{ return true; }

template <typename T>
constexpr EMP_INLINE EMP_RETURN bool is_not_valid(T)
{ return false; }

EMP_INLINE EMP_RETURN bool is_valid(c_float f);
EMP_INLINE EMP_RETURN bool is_valid(c_float f)
{ return is_nan(f) == false && is_inf(f) == false; }

EMP_INLINE EMP_RETURN bool is_valid(c_double d);
EMP_INLINE EMP_RETURN bool is_valid(c_double d)
{ return is_nan(d) == false && is_inf(d) == false; }

#if defined EMP_ASSERT_NOT_NAN
#define EMP_ASSERT_NUMBER(x_Value) EMP_ASSERT(emp::tt::is_valid(x_Value))
#define EMP_CONST_ASSERT_NUMBER(x_Value) assert(emp::tt::is_valid(x_Value))
#else
#define EMP_ASSERT_NUMBER(x_Value)
#define EMP_CONST_ASSERT_NUMBER(x_Value) 
#endif

EMP_PRAGMA_PUSH_IGNORE(EMP_W_FLOAT_EQUAL)
template <typename T> constexpr EMP_INLINE EMP_RETURN bool equal_(T const f0, T const f1);
template <typename T> constexpr EMP_INLINE EMP_RETURN bool equal_(T const f0, T const f1)
{
    EMP_CONST_ASSERT_NUMBER(f0);
    EMP_CONST_ASSERT_NUMBER(f1);
    return f0 == f1;
}
EMP_PRAGMA_POP_IGNORE(EMP_W_FLOAT_EQUAL)

template <typename T> EMP_INLINE EMP_RETURN bool not_equal_(T const t0, T const t1);
template <typename T> EMP_INLINE EMP_RETURN bool not_equal_(T const t0, T const t1)
{ return equal_(t0, t1) == false; }

template <typename T> EMP_INLINE void get_min(T& a_rtValue) { a_rtValue = emp::tt::min<T>::value; }
template <typename T> EMP_INLINE void get_max(T& a_rtValue) { a_rtValue = emp::tt::max<T>::value; }
template <typename T> EMP_INLINE EMP_RETURN T get_max(void) { return emp::tt::max<T>::value; }
template <typename T> EMP_INLINE EMP_RETURN bool is_min(T const a_rtValue) { return equal_(a_rtValue, emp::tt::min<T>::value); }
template <typename T> constexpr EMP_INLINE EMP_RETURN bool is_max(T const a_rtValue) { return equal_(a_rtValue, emp::tt::max<T>::value); }
template <typename T> EMP_INLINE EMP_RETURN bool is_not_min(T const a_rtValue) { return is_min(a_rtValue) == false; }
template <typename T> EMP_INLINE EMP_RETURN bool is_not_max(T const a_rtValue) { return is_max(a_rtValue) == false; }
template <typename T> EMP_INLINE EMP_RETURN bool has_max(T const a_rtValue) { return is_max(a_rtValue); }
template <typename T> EMP_INLINE EMP_RETURN bool has_min(T const a_rtValue) { return is_min(a_rtValue); }

template <typename T, T first, T... params>
EMP_NOINSTANCE(struct, max_t) static constexpr const T value = max_t<T, first, max_t<T, params...>::value>::value; };

template <typename T, T first, T last>
EMP_NOINSTANCE(struct, (max_t<T, first, last>)) static constexpr const T value = first > last ? first : last; };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TT_MIN_CTOR(x_Variable) x_Variable(emp::tt::min<decltype(x_Variable)>::value)
#define EMP_TT_MAX_CTOR(x_Variable) x_Variable(emp::tt::max<decltype(x_Variable)>::value)

#define EMP_TT_MIN(x_Variable) emp::tt::min<emp::tt::try_remove_reference<emp::tt::try_remove_const<decltype(x_Variable)>>>::value
#define EMP_TT_MAX(x_Variable) emp::tt::max<emp::tt::try_remove_reference<emp::tt::try_remove_const<decltype(x_Variable)>>>::value

#define EMP_TT_MIN_VAR(x_Variable) x_Variable = EMP_TT_MIN(x_Variable)
#define EMP_TT_MAX_VAR(x_Variable) x_Variable = EMP_TT_MAX(x_Variable)


/* @@0 wtf template <typename T>
EMP_NOINSTANCE(class, min<T*>)
public:
    static_assert(sizeof(T*) == sizeof(uintptr_t), "");
    static const uintptr_t value = emp::tt::min<uintptr_t>::value;
};

template <typename T>
EMP_NOINSTANCE(class, max<T*>)
public:
    static_assert(sizeof(T*) == sizeof(uintptr_t), "");
    static const uintptr_t value = emp::tt::max<uintptr_t>::value;
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
constexpr EMP_RETURN T maxof(T const& a_rLeft, T const& a_rRight)
{
    EMP_CONST_ASSERT_NUMBER(a_rLeft);
    EMP_CONST_ASSERT_NUMBER(a_rRight);
    return a_rLeft > a_rRight ? a_rLeft : a_rRight;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
constexpr EMP_RETURN T minof(T const& a_rLeft, T const& a_rRight)
{
    EMP_CONST_ASSERT_NUMBER(a_rLeft);
    EMP_CONST_ASSERT_NUMBER(a_rRight);
    return a_rLeft < a_rRight ? a_rLeft : a_rRight;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
constexpr EMP_RETURN T saturate(T const& a_rLeft)
{
    EMP_CONST_ASSERT_NUMBER(a_rLeft);
    return maxof<T>(0, minof<T>(1, a_rLeft));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

