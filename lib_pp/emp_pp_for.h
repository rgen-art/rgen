//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_FOR_H
#define EMP_PP_FOR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_eat.h"
#include "emp_pp_equal.h"
#include "emp_pp_if_else.h"
#include "emp_pp_inc.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_FOR(x_Index, x_Count, x_Macro)\
    EMP_PP_FOR_IMP(x_Index, x_Count, x_Macro)

#define EMP_PP_FOR_IMP(x_Index, x_Count, x_Macro)\
    EMP_PP_FOR_0(0, x_Index, x_Count, x_Macro)

#define EMP_PP_FOR_0(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (1, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_1)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_1(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (2, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_2)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_2(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (3, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_3)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_3(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (4, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_4)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_4(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (5, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_5)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_5(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (6, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_6)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_6(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (7, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_7)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_7(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (8, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_8)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_8(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (9, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_9)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_9(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (10, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_10)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_10(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (11, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_11)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_11(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (12, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_12)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_12(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (13, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_13)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_13(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (14, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_14)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_14(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (15, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_15)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_15(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (16, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_16)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_16(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (17, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_17)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_17(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (18, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_18)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_18(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (19, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_19)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_19(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (20, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_20)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_20(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (21, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_21)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_21(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (22, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_22)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_22(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (23, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_23)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_23(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (24, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_24)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_24(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (25, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_25)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_25(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (26, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_26)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_26(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (27, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_27)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_27(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (28, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_28)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_28(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (29, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_29)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_29(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (30, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_30)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_30(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (31, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_31)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_31(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (32, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_32)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_32(x_Depth, x_Index, x_Count, x_Macro)\
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_3, x_Macro)\
    (33, x_Index, x_Count) \
EMP_PP_IF_ELSE(EMP_PP_EQUAL(x_Index, x_Count), EMP_PP_EAT_4, EMP_PP_FOR_33)\
    (x_Depth, EMP_PP_INC(x_Index), x_Count, x_Macro)

#define EMP_PP_FOR_33(x_Depth, x_Index, x_Count, x_Macro)\
    EMP_PP_ERROR_EMP_PP_FOR_OVERFLOW

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

