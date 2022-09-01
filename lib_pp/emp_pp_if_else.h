//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_IF_ELSE_H
#define EMP_PP_IF_ELSE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_bool.h"
#include "emp_pp_cat.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_IF_ELSE(x_Condition, x_Then, x_Else) EMP_PP_IF_ELSE_IMP(x_Condition, x_Then, x_Else)
#define EMP_PP_IF_ELSE_IMP(x_Condition, x_Then, x_Else) EMP_PP_CAT(EMP_PP_IF_ELSE_IMP_, EMP_PP_BOOL(x_Condition))(x_Then, x_Else)

#define EMP_PP_IF_ELSE_IMP_0(x_Then, x_Else) x_Else
#define EMP_PP_IF_ELSE_IMP_1(x_Then, x_Else) x_Then

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

