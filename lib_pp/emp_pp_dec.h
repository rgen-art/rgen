//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_DEC_H
#define EMP_PP_DEC_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_cat.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_DEC(x_Value) EMP_PP_DEC_IMP(x_Value)
#define EMP_PP_DEC_IMP(x_Value) EMP_PP_CAT(EMP_PP_DEC_, x_Value)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_DEC_0 EMP_PP_ERROR_EMP_PP_DEC_UNDERFLOW
#define EMP_PP_DEC_1 0
#define EMP_PP_DEC_2 1
#define EMP_PP_DEC_3 2
#define EMP_PP_DEC_4 3
#define EMP_PP_DEC_5 4
#define EMP_PP_DEC_6 5
#define EMP_PP_DEC_7 6
#define EMP_PP_DEC_8 7
#define EMP_PP_DEC_9 8
#define EMP_PP_DEC_10 9
#define EMP_PP_DEC_11 10
#define EMP_PP_DEC_12 11
#define EMP_PP_DEC_13 12
#define EMP_PP_DEC_14 13
#define EMP_PP_DEC_15 14
#define EMP_PP_DEC_16 15
#define EMP_PP_DEC_17 16
#define EMP_PP_DEC_18 17
#define EMP_PP_DEC_19 18
#define EMP_PP_DEC_20 19
#define EMP_PP_DEC_21 20
#define EMP_PP_DEC_22 21
#define EMP_PP_DEC_23 22
#define EMP_PP_DEC_24 23
#define EMP_PP_DEC_25 24
#define EMP_PP_DEC_26 25
#define EMP_PP_DEC_27 26
#define EMP_PP_DEC_28 27
#define EMP_PP_DEC_29 28
#define EMP_PP_DEC_30 29
#define EMP_PP_DEC_31 30
#define EMP_PP_DEC_32 EMP_PP_ERROR_EMP_PP_DEC_OVERFLOW

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

