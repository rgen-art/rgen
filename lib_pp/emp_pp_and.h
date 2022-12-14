//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_AND_H
#define EMP_PP_AND_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_bool.h"
#include "emp_pp_cat.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_AND_IMP0(x_Left, x_Right) EMP_PP_AND_IMP1(x_Left, x_Right)
#define EMP_PP_AND_IMP1(x_Left, x_Right) EMP_PP_CAT(EMP_PP_AND_, EMP_PP_BOOL(x_Left), EMP_PP_BOOL(x_Right))

#define EMP_PP_AND(...) EMP_PP_FORWARD(EMP_PP_CAT(EMP_PP_AND_, EMP_VAARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_AND_00 0
#define EMP_PP_AND_01 0
#define EMP_PP_AND_10 0
#define EMP_PP_AND_11 1

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_AND_2(x_a, x_b) EMP_PP_AND_IMP0(x_a, x_b)
#define EMP_PP_AND_3(x_a, x_b, x_c) EMP_PP_AND_2(x_a, EMP_PP_AND_2(x_b, x_c))
#define EMP_PP_AND_4(x_a, x_b, x_c, x_d) EMP_PP_AND_2(x_a, EMP_PP_AND_3(x_b, x_c, x_d))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

