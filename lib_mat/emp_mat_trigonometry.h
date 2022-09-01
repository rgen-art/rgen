//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_TRIGONOMETRY_H
#define EMP_MAT_TRIGONOMETRY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_math.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PI_DOUBLE (3.14159265358979323846)
#define EMP_PI_FLOAT emp::ast::cast<float>(EMP_PI_DOUBLE)

#define EMP_PIPI_FLOAT ((EMP_PI_FLOAT) * (EMP_PI_FLOAT))
#define EMP_PIPI_DOUBLE ((EMP_PI_DOUBLE) * (EMP_PI_DOUBLE))

#define EMP_PI_OVER_TWO_FLOAT (EMP_PI_FLOAT / 2)
#define EMP_PI_OVER_TWO_DOUBLE (EMP_PI_DOUBLE / 2)

#define EMP_TWO_OVER_PI_FLOAT (2 / EMP_PI_FLOAT)
#define EMP_TWO_OVER_PI_DOUBLE (2 / EMP_PI_DOUBLE)

#define EMP_TWO_PI_FLOAT (2 * EMP_PI_FLOAT)
#define EMP_TWO_PI_DOUBLE (2 * EMP_PI_DOUBLE)

#define EMP_HALF_FLOAT 0.5f
#define EMP_HALF_DOUBLE 0.5

#define EMP_SINCOS_CC0_FLOAT -0.0013602249f; // (-1)**6/6!
#define EMP_SINCOS_CC1_FLOAT  0.0416566950f; // (-1)**4/4!
#define EMP_SINCOS_CC2_FLOAT -0.4999990225f; // (-1)**2/2!

#define EMP_SINCOS_SC0_FLOAT -0.0001950727f; // (-1)**7/7!
#define EMP_SINCOS_SC1_FLOAT  0.0083320758f; // (-1)**5/5!
#define EMP_SINCOS_SC2_FLOAT -0.1666665247f; // (-1)**3/3!

#define EMP_SINCOS_KC1_FLOAT  1.57079625129f;    // PI/2
#define EMP_SINCOS_KC2_FLOAT  7.54978995489e-8f; // PI/2-1.57079625129f

#define EMP_ZERO 0
#define EMP_ONE 1
#define EMP_TWO 2
#define EMP_FOUR 4
#define EMP_EIGHT 8

#define EMP_NEGATIVE_FLOAT -0.0f
#define EMP_NEGATIVE_DOUBLE -0.0


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T to_rad(void);
template <> EMP_INLINE EMP_RETURN float to_rad(void);
template <> EMP_INLINE EMP_RETURN float to_rad(void)
{ return EMP_PI_FLOAT / 180.0f; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      pi(void);
template <          > EMP_INLINE EMP_RETURN float  pi(void);
template <          > EMP_INLINE EMP_RETURN float  pi(void)
{ return EMP_PI_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double pi(void);
template <          > EMP_INLINE EMP_RETURN double pi(void)
{ return EMP_PI_DOUBLE; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      pipi(void);
template <          > EMP_INLINE EMP_RETURN float  pipi(void);
template <          > EMP_INLINE EMP_RETURN float  pipi(void)
{ return EMP_PIPI_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double pipi(void);
template <          > EMP_INLINE EMP_RETURN double pipi(void)
{ return EMP_PIPI_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      pi_over_two(void);
template <          > EMP_INLINE EMP_RETURN float  pi_over_two(void);
template <          > EMP_INLINE EMP_RETURN float  pi_over_two(void)
{ return EMP_PI_OVER_TWO_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double pi_over_two(void);
template <          > EMP_INLINE EMP_RETURN double pi_over_two(void)
{ return EMP_PI_OVER_TWO_DOUBLE; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      two_over_pi(void);
template <          > EMP_INLINE EMP_RETURN float  two_over_pi(void);
template <          > EMP_INLINE EMP_RETURN float  two_over_pi(void)
{ return EMP_TWO_OVER_PI_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double two_over_pi(void);
template <          > EMP_INLINE EMP_RETURN double two_over_pi(void)
{ return EMP_TWO_OVER_PI_DOUBLE; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      two_pi(void);
template <          > EMP_INLINE EMP_RETURN float  two_pi(void);
template <          > EMP_INLINE EMP_RETURN float  two_pi(void)
{ return EMP_TWO_PI_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double two_pi(void);
template <          > EMP_INLINE EMP_RETURN double two_pi(void)
{ return EMP_TWO_PI_DOUBLE; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      half(void);
template <          > EMP_INLINE EMP_RETURN float  half(void);
template <          > EMP_INLINE EMP_RETURN float  half(void)
{ return EMP_HALF_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double half(void);
template <          > EMP_INLINE EMP_RETURN double half(void)
{ return EMP_HALF_DOUBLE; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T one(void);
template <typename T> EMP_INLINE EMP_RETURN T one(void)
{ return 1; }

template <typename T> EMP_INLINE EMP_RETURN T two(void);
template <typename T> EMP_INLINE EMP_RETURN T two(void)
{ return 2; }

template <typename T> EMP_INLINE EMP_RETURN T three(void);
template <typename T> EMP_INLINE EMP_RETURN T three(void)
{ return 3; }

template <typename T> EMP_INLINE EMP_RETURN T four(void);
template <typename T> EMP_INLINE EMP_RETURN T four(void)
{ return 4; }

template <typename T> EMP_INLINE EMP_RETURN T eight(void);
template <typename T> EMP_INLINE EMP_RETURN T eight(void)
{ return 8; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T      negative(void);
template <          > EMP_INLINE EMP_RETURN float  negative(void);
template <          > EMP_INLINE EMP_RETURN float  negative(void)
{ return EMP_NEGATIVE_FLOAT; }

template <          > EMP_INLINE EMP_RETURN double negative(void);
template <          > EMP_INLINE EMP_RETURN double negative(void)
{ return EMP_NEGATIVE_DOUBLE; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_cc0(void);
template <          > EMP_INLINE EMP_RETURN float sincos_cc0(void);
template <          > EMP_INLINE EMP_RETURN float sincos_cc0(void)
{ return EMP_SINCOS_CC0_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_cc1(void);
template <          > EMP_INLINE EMP_RETURN float sincos_cc1(void);
template <          > EMP_INLINE EMP_RETURN float sincos_cc1(void)
{ return EMP_SINCOS_CC1_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_cc2(void);
template <          > EMP_INLINE EMP_RETURN float sincos_cc2(void);
template <          > EMP_INLINE EMP_RETURN float sincos_cc2(void)
{ return EMP_SINCOS_CC2_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_sc0(void);
template <          > EMP_INLINE EMP_RETURN float sincos_sc0(void);
template <          > EMP_INLINE EMP_RETURN float sincos_sc0(void)
{ return EMP_SINCOS_SC0_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_sc1(void);
template <          > EMP_INLINE EMP_RETURN float sincos_sc1(void);
template <          > EMP_INLINE EMP_RETURN float sincos_sc1(void)
{ return EMP_SINCOS_SC1_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_sc2(void);
template <          > EMP_INLINE EMP_RETURN float sincos_sc2(void);
template <          > EMP_INLINE EMP_RETURN float sincos_sc2(void)
{ return EMP_SINCOS_SC2_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_kc1(void);
template <          > EMP_INLINE EMP_RETURN float sincos_kc1(void);
template <          > EMP_INLINE EMP_RETURN float sincos_kc1(void)
{ return EMP_SINCOS_KC1_FLOAT; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN T     sincos_kc2(void);
template <          > EMP_INLINE EMP_RETURN float sincos_kc2(void);
template <          > EMP_INLINE EMP_RETURN float sincos_kc2(void)
{ return EMP_SINCOS_KC2_FLOAT; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float cos(c_float a_fValue);
EMP_INLINE EMP_RETURN float cos(c_float a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    return cosf(a_fValue);
}

EMP_INLINE EMP_RETURN double cos(c_double a_fValue);
EMP_INLINE EMP_RETURN double cos(c_double a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    return ::cos(a_fValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float sin(c_float a_fValue); // @@0 remove declaration?
EMP_INLINE EMP_RETURN float sin(c_float a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    return sinf(a_fValue);
}

EMP_INLINE EMP_RETURN double sin(c_double a_fValue);
EMP_INLINE EMP_RETURN double sin(c_double a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    return ::sin(a_fValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float tan(c_float a_fValue);
EMP_INLINE EMP_RETURN float tan(c_float a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    return tanf(a_fValue);
}

EMP_INLINE EMP_RETURN double tan(c_double a_fValue);
EMP_INLINE EMP_RETURN double tan(c_double a_fValue)
{
    EMP_ASSERT_NUMBER(a_fValue);
    return ::tan(a_fValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
