//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_ALGEBRA_H
#define EMP_MAT_ALGEBRA_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_logical.h"

#include "emp_ast_cast.h"

#include "emp_hh_math.h"
#include "emp_hh_stdlib.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <random>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T> EMP_INLINE EMP_RETURN T abs(T const a_tFrom);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*ENABLE_IF_T(tt::is_signed<T>::value || tt::is_real<T>::value)
EMP_INLINE EMP_RETURN tt::signed_<T> abs(T const a_tFrom) //@@0 typename emp::tt::signed_<T>::type
{ return a_tFrom < 0 ? -a_tFrom : a_tFrom; }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//@@0 int_promotion
/*EMP_INLINE EMP_RETURN tt::signed_<char> abs(c_char a_tFrom);
EMP_INLINE EMP_RETURN tt::signed_<char> abs(c_char a_tFrom)
{
    static_assert(emp::tt::is_signed<char>::value, "");
    if (a_tFrom < 0)
    { return emp::ast::cast<tt::signed_<char>>(-a_tFrom); }  // @@0 runtime_cast
    return a_tFrom;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//@@0 int_promotion
/*EMP_INLINE EMP_RETURN short abs(short const a_tFrom);
EMP_INLINE EMP_RETURN short abs(short const a_tFrom)
{
    static_assert(emp::tt::is_signed<short>::value, "");
    if (a_tFrom < 0)
    { return emp::ast::cast<tt::signed_<short>>(-a_tFrom); } // @@0 runtime_cast
    return a_tFrom;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_T(std::is_unsigned<T>::value == false)
EMP_INLINE EMP_RETURN T abs(T const a_tFrom)
{
    EMP_ASSERT_NUMBER(a_tFrom);
    return a_tFrom < 0 ? emp::ast::cast<T>(-a_tFrom) : a_tFrom;
}

ENABLE_IF_T(std::is_unsigned<T>::value)
EMP_INLINE EMP_RETURN T abs(T const a_tFrom)
{
    EMP_ASSERT_NUMBER(a_tFrom);
    return a_tFrom;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_SIGNED_T
EMP_INLINE EMP_RETURN tt::unsigned_<T> uabs(T const a_tFrom)
{
    EMP_ASSERT_NUMBER(a_tFrom);
    T const tResult = abs<T>(a_tFrom);
    EMP_ASSERT(tResult >= 0);
    return ast::cast<tt::unsigned_<T>>(tResult); // @@0 runtime_cast
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_NOT_SIGNED_T
EMP_INLINE EMP_RETURN emp::tt::unsigned_<T> uabs(T const a_tFrom)
{
    EMP_ASSERT_NUMBER(a_tFrom);
    return static_cast<emp::tt::unsigned_<T>>(a_tFrom);// TODO:RG
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_SIGNED_T EMP_RETURN bool is_negative(T const a_tValue)
{ return a_tValue < 0; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, ENABLE_IF_NOT_SIGNED(T), ENABLE_IF_NOT_REAL(T)>
EMP_RETURN bool is_negative(T const EMP_SILENT(a_tValue))
{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, ENABLE_IF_NOT_SIGNED(T), ENABLE_IF_REAL(T)>
EMP_RETURN bool is_negative(T const a_tValue)
{
    EMP_ASSERT_NUMBER(a_tValue);
    return a_tValue < 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool add(T const a_t0, T const a_t1, T& a_rtResult)
{
    EMP_ASSERT_NUMBER(a_t0);
    EMP_ASSERT_NUMBER(a_t1);

    if ((emp::tt::max<T>::value - a_t0) < a_t1)
    { return false; }
    if (ast::try_cast(a_t0 + a_t1, a_rtResult) == false)
    { return false; }

    EMP_ASSERT_NUMBER(a_rtResult);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool sub(T const a_t0, T const a_t1, T& a_rtResult)
{
    EMP_ASSERT_NUMBER(a_t0);
    EMP_ASSERT_NUMBER(a_t1);

    a_rtResult = ast::cast<T>(a_t0 - a_t1);
    EMP_ASSERT_NUMBER(a_rtResult);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool mul(T const a_t0, T const a_t1, T& a_rtResult)
{
    EMP_ASSERT_NUMBER(a_t0);
    EMP_ASSERT_NUMBER(a_t1);

    a_rtResult = ast::cast<T>(a_t0 * a_t1);
    EMP_ASSERT_NUMBER(a_rtResult);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool div(T const a_t0, T const a_t1, T& a_rtResult)
{
    EMP_ASSERT_NUMBER(a_t0);
    EMP_ASSERT_NUMBER(a_t1);

    a_rtResult = ast::cast<T>(a_t0 / a_t1);
    EMP_ASSERT_NUMBER(a_rtResult);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float log10(c_float a_fValue);
EMP_INLINE EMP_RETURN float log10(c_float a_fValue)
{
    c_float fResult = log10f(a_fValue);
    EMP_ASSERT_NUMBER(fResult);
    return fResult;
}

EMP_INLINE EMP_RETURN double log10(c_double a_fValue);
EMP_INLINE EMP_RETURN double log10(c_double a_fValue)
{
    c_double dResult = ::log10(a_fValue);
    EMP_ASSERT_NUMBER(dResult);
    return dResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN T mod(T const a_tValue, T const a_tMod)
{
    EMP_ASSERT_NUMBER(a_tValue);
    EMP_ASSERT_NUMBER(a_tMod);

    size_t stIndex = 0;
    while ((a_tValue - (emp::ast::cast<float>(stIndex) * a_tMod)) > a_tMod)
    { ++stIndex; }

    T const tResult = a_tValue - emp::ast::cast<float>(stIndex) * a_tMod;
    EMP_ASSERT_NUMBER(tResult);
    return tResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float pow(c_float a_fValue, c_float a_fPow);
EMP_INLINE EMP_RETURN float pow(c_float a_fValue, c_float a_fPow)
{
    EMP_ASSERT_NUMBER(a_fValue);
    EMP_ASSERT_NUMBER(a_fPow);

    c_float fResult = powf(a_fValue, a_fPow);
    EMP_ASSERT_NUMBER(fResult);
    return fResult;
}

EMP_INLINE EMP_RETURN double pow(c_double a_fValue, c_double a_fPow);
EMP_INLINE EMP_RETURN double pow(c_double a_fValue, c_double a_fPow)
{
    EMP_ASSERT_NUMBER(a_fValue);
    EMP_ASSERT_NUMBER(a_fPow);

    c_double fResult = ::pow(a_fValue, a_fPow);
    EMP_ASSERT_NUMBER(fResult);
    return fResult;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, random_c)
private:
    std::random_device m_Device;
    ui32 m_ui32Seed = 0;
    std::mt19937 m_Engine;
    std::uniform_real_distribution<float> m_fDistribution;
    std::uniform_int_distribution<ui32> m_ui32Distribution;
    std::uniform_int_distribution<size_t> m_stDistribution;

public:
    random_c(void);

    EMP_RETURN ui32 create(void);
    EMP_RETURN ui32 create(c_ui32 a_ui32Seed);

    EMP_RETURN ui32 seed(void) const { return m_ui32Seed; }

    template <typename T>
    EMP_RETURN T rand(T const a_tMin, T const a_tMax);

};

template <>
EMP_RETURN EMP_INLINE float random_c::rand(c_float a_fMin, c_float a_fMax)
{ return std::uniform_real_distribution<float>{a_fMin, a_fMax}(m_Engine); }

template <>
EMP_RETURN EMP_INLINE ui32 random_c::rand(c_ui32 a_ui32Min, c_ui32 a_ui32Max)
{ return std::uniform_int_distribution<ui32>{a_ui32Min, a_ui32Max}(m_Engine); }

template <>
EMP_RETURN EMP_INLINE emp::tt::enable_if<emp::tt::not_equal<size_t, ui32>::value, size_t>
random_c::rand(c_size a_stMin, c_size a_stMax)
{ return std::uniform_int_distribution<size_t>{a_stMin, a_stMax}(m_Engine); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove
EMP_INLINE void random_seed(c_uint a_uiValue);
EMP_INLINE void random_seed(c_uint a_uiValue) { srand(a_uiValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     random_max(void);
template <          > EMP_INLINE EMP_RETURN int   random_max(void);
template <          > EMP_INLINE EMP_RETURN int   random_max(void)
{ return RAND_MAX; }

template <          > EMP_INLINE EMP_RETURN float random_max(void);
template <          > EMP_INLINE EMP_RETURN float random_max(void)
{ return emp::ast::cast<float>(RAND_MAX); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN int random(void);
EMP_INLINE EMP_RETURN int random(void) { return rand(); }

EMP_INLINE EMP_RETURN uint urandom(void)
{ return emp::ast::cast<uint>(random()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float random_float(c_float a_fScale);
EMP_INLINE EMP_RETURN float random_float(c_float a_fScale)
{
    c_double dRandom = emp::ast::cast<double>(random());
    c_double dMaxRandom = random_max<float>();
    c_float fResult = emp::ast::cast<float>((emp::ast::cast<double>(a_fScale) * dRandom) / dMaxRandom);
    EMP_ASSERT_NUMBER(fResult);
    return fResult;
}
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void swap(T& a_rLeft, T& a_rRight)
{
    T oTmp = a_rRight;
    a_rRight = a_rLeft;
    a_rLeft = oTmp;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float sqr(c_float a_fValue);
EMP_INLINE EMP_RETURN float sqr(c_float a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    c_float fResult = sqrtf(a_fValue);
    EMP_ASSERT_NUMBER(fResult);
    return fResult;
}

EMP_INLINE EMP_RETURN double sqr(c_double a_dValue);
EMP_INLINE EMP_RETURN double sqr(c_double a_dValue)
{
    EMP_ASSERT_NUMBER(a_dValue);
    c_double dResult = sqrt(a_dValue);
    EMP_ASSERT_NUMBER(dResult);
    return dResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
