//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_COMMA_IF_H
#define EMP_PP_COMMA_IF_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_comma.h"
#include "emp_pp_empty.h"
#include "emp_pp_if_else.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_COMMA_IF(x_Condition)\
    EMP_PP_IF_ELSE(x_Condition, EMP_PP_COMMA, EMP_PP_EMPTY)()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
