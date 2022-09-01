//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_ARGUMENT_H
#define EMP_PP_ARGUMENT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_and.h"
#include "emp_pp_cat.h"
#include "emp_pp_comma_if.h"
#include "emp_pp_for.h"
#include "emp_pp_inc.h"
#include "emp_pp_not_equal.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_TYPE(x_Index)      EMP_PP_CAT(t_tType, x_Index)
#define EMP_PP_TYPEDEF(x_Index)   EMP_PP_CAT(type, x_Index)
#define EMP_PP_PARAMETER(x_Index) EMP_PP_CAT(a_trParam, x_Index)
#define EMP_PP_ARGUMENT(x_Index)  EMP_PP_TYPE(x_Index) const& EMP_PP_PARAMETER(x_Index)
#define EMP_PP_MEMBER(x_Index)    EMP_PP_CAT(m_tValue, x_Index)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)\
    EMP_PP_COMMA_IF(EMP_PP_NOT_EQUAL(EMP_PP_INC(x_Index), x_Count))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_TEMPLATE_PARAMETER(x_Depth, x_Index, x_Count)\
    typename EMP_PP_TYPE(x_Index)\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_TEMPLATE_PARAMETER_DEFAULT(x_Depth, x_Index, x_Count)\
    typename EMP_PP_TYPE(x_Index) = emp::tt::null\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_TEMPLATE_ARGUMENT(x_Depth, x_Index, x_Count)\
    EMP_PP_TYPE(x_Index)\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_TEMPLATE_ARGUMENT_CONST(x_Depth, x_Index, x_Count)\
    emp::tt::const_param<EMP_PP_TYPE(x_Index)>\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_TEMPLATE_LIST_BEGIN(x_Depth, x_Index, x_Count)\
    emp::tt::list<EMP_PP_TYPE(x_Index),

#define EMP_PP_TEMPLATE_LIST_END(x_Depth, x_Index, x_Count)\
    >

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_FUNCTION_PARAMETER(x_Depth, x_Index, x_Count)\
    EMP_PP_ARGUMENT(x_Index)\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_FUNCTION_ARGUMENT(x_Depth, x_Index, x_Count)\
    EMP_PP_PARAMETER(x_Index)\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_CLASS_TYPEDEF(x_Depth, x_Index, x_Count) typedef EMP_PP_TYPE(x_Index) EMP_PP_TYPEDEF(x_Index);
#define EMP_PP_CLASS_MEMBER(x_Depth, x_Index, x_Count) EMP_PP_TYPE(x_Index) EMP_PP_MEMBER(x_Index);

#define EMP_PP_CLASS_CTOR(x_Depth, x_Index, x_Count)\
    EMP_PP_MEMBER(x_Index)()\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_CLASS_INIT(x_Depth, x_Index, x_Count)\
    EMP_PP_MEMBER(x_Index)(EMP_PP_PARAMETER(x_Index))\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

#define EMP_PP_CLASS_ARGUMENT(x_Depth, x_Index, x_Count)\
    EMP_PP_MEMBER(x_Index)\
    EMP_PP_COMMA_IF_NOT_LAST(x_Depth, x_Index, x_Count)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_FOR_ARGUMENTS(x_Depth, x_Count, x_Macro, x_Left, x_Right)\
    EMP_PP_COMMA_IF(EMP_PP_AND(x_Left, x_Count))\
    EMP_PP_CAT(EMP_PP_FOR_, x_Depth)(x_Depth, 0, x_Count, x_Macro)\
    EMP_PP_COMMA_IF(EMP_PP_AND(x_Right, x_Count))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

