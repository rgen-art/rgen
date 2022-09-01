//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_BOOL_H
#define EMP_PP_BOOL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_cat.h"
#include "emp_pp_not.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_CHECK_N(x, n, ...) n
#define EMP_PP_CHECK(...) EMP_PP_FORWARD(EMP_PP_CHECK_N(__VA_ARGS__, 0,))

#define EMP_PP_PROBE(x) x, 1,

#define EMP_PP_IS0(x_Value) EMP_PP_FORWARD(EMP_PP_CHECK(EMP_PP_CAT(EMP_PP_IS0_, x_Value)))
#define EMP_PP_IS0_0 EMP_PP_PROBE(~)
#define EMP_PP_IS0_false EMP_PP_PROBE(~)

#define EMP_PP_BOOL(x_Value) EMP_PP_NOT(EMP_PP_IS0(x_Value))

#define EMP_PP_IS_VOID(x_Value) EMP_PP_CAT(EMP_PP_IS_VOID_, x_Value)
#define EMP_PP_IS_VOID_void 1

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

