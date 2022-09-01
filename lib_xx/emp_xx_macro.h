//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_MACRO_H
#define EMP_XX_MACRO_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_LOUSY
#include "emp_xx_macro_lousy.h"
#elif defined EMP_XX_DEBUG
#include "emp_xx_macro_debug.h"
#elif defined EMP_XX_RELEASE
#include "emp_xx_macro_release.h"
#elif defined EMP_XX_FINAL
#include "emp_xx_macro_final.h"
#else
#error unknown build
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_OS_IOS
#define EMP_XX_IF_IOS(x_Then) x_Then
#define EMP_XX_IF_NOT_IOS(x_Then)
#else
#define EMP_XX_IF_IOS(x_Then)
#define EMP_XX_IF_NOT_IOS(x_Then) x_Then
#endif

#if defined EMP_XX_OS_OSX || defined EMP_XX_OS_WIN || defined EMP_XX_OS_LIN
#define EMP_XX_DEV_DESKTOP
#elif defined EMP_XX_OS_IOS
#define EMP_XX_DEV_MOBILE
#else
#error unknown device type
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_TT_ARRAY_COUNT
#error EMP_XX_TT_ARRAY_COUNT defined
#endif

#ifdef EMP_XX_CNT_TUPLE_COUNT
#error EMP_XX_CNT_TUPLE_COUNT defined
#endif

#ifdef EMP_XX_MEM_ALLOCATE_COUNT
#error EMP_XX_MEM_ALLOCATE_COUNT defined
#endif

#ifdef EMP_XX_STR_FORMAT_COUNT
#error EMP_XX_STR_FORMAT_COUNT defined
#endif

#ifdef EMP_XX_FCT_FUNCTION_COUNT
#error EMP_XX_FCT_FUNCTION_COUNT defined
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(EMP_XX_ASSERT)
#define EMP_XX_IF_ASSERT(x_Then) x_Then
#define EMP_XX_IF_NOT_ASSERT(x_Then)
#define EMP_XX_IF_ELSE_ASSERT(x_Then, x_Else) x_Then
#else
#define EMP_XX_IF_ASSERT(x_Then)
#define EMP_XX_IF_NOT_ASSERT(x_Then) x_Then
#define EMP_XX_IF_ELSE_ASSERT(x_Then, x_Else) x_Else
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(EMP_XX_LOCALIZE)
#define EMP_XX_IF_LOCALIZE(x_Then) x_Then
#define EMP_XX_IF_NOT_LOCALIZE(x_Then)
#define EMP_XX_IF_ELSE_LOCALIZE(x_Then, x_Else) x_Then
#else
#define EMP_XX_IF_LOCALIZE(x_Then)
#define EMP_XX_IF_NOT_LOCALIZE(x_Then) x_Then
#define EMP_XX_IF_ELSE_LOCALIZE(x_Then, x_Else) x_Else
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_TRACE_LEAKS
#define EMP_XX_TRACE
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_DEBUG
#define EMP_XX_IF_DEBUG(x_Then) x_Then
#else
#define EMP_XX_IF_DEBUG(x_Then)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_PROFILE
#define EMP_XX_IF_PROFILE(x_Then) x_Then
#define EMP_XX_IF_ELSE_PROFILE(x_Then, x_Else) x_Then
#else
#define EMP_XX_IF_PROFILE(x_Then)
#define EMP_XX_IF_ELSE_PROFILE(x_Then, x_Else) x_Else
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_vaargs.h"

#include "emp_pp_and.h"
#include "emp_pp_bool.h"
#include "emp_pp_cat.h"
#include "emp_pp_comma_if.h"
#include "emp_pp_empty.h"
#include "emp_pp_equal.h"
#include "emp_pp_forward.h"
#include "emp_pp_if.h"
#include "emp_pp_if_else.h"
#include "emp_pp_not.h"
#include "emp_pp_solve.h"
#include "emp_pp_string.h"

#include "emp_xx_attribute.h"
#include "emp_xx_build.h"
#include "emp_xx_call.h"
#include "emp_xx_class.h"
#include "emp_xx_pragma.h"
#include "emp_xx_noop.h"

#include "emp_hh_typedef.h"

#include "emp_tt_logical.h"
#include "emp_tt_fundamental.h"
#include "emp_tt_enum.h"
#include "emp_tt_limit.h"
#include "emp_tt_class.h"

#include "emp_ast_assert.h"
#include "emp_ast_cast.h"

#include "emp_mem_allocator.h"
#include "emp_mem_pointer.h"

#include "emp_fct_function.h"

#include "emp_cnt_shell.h"
#include "emp_cnt_map.h"
#include "emp_cnt_vector.h"
#include "emp_cnt_vector_map.h"
#include "emp_cnt_array.h"
#include "emp_cnt_foreach.h"
#include "emp_cnt_string.h"

#include "emp_log_logger.h"

#include "emp_loc_string.h"
#include "emp_loc_localizer.h"

#include "emp_tim_time.h"

#include "emp_mat_vector.h"
#include "emp_mat_matrix.h"
#include "emp_mat_algebra.h"
#include "emp_mat_trigonometry.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::str;

EMP_PRAGMA_SET_IGNORE(EMP_W_INLINE) // @@0 remove
EMP_PRAGMA_SET_IGNORE(EMP_W_FUNCTION_NOT_INLINED) // @@0 remove

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

