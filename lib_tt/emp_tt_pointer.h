//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_POINTER_H
#define EMP_TT_POINTER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_reference.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_pointer, false_)
EMP_TT_DECLARE_VALUE_PARTIAL_CV(is_pointer, T*, true_)
//EMP_TT_DECLARE_VALUE_PARTIAL_CV(is_pointer, T const*, true_)
//EMP_TT_DECLARE_VALUE_PARTIAL_CV(is_pointer, T volatile*, true_)
//EMP_TT_DECLARE_VALUE_PARTIAL_CV(is_pointer, T const volatile*, true_)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_not_pointer, not_<is_pointer<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_TYPE(add_pointer_t, T*)
EMP_TT_DECLARE_TYPE_PARTIAL(add_pointer_t, T const, T const*)
EMP_TT_DECLARE_TYPE_PARTIAL(add_pointer_t, T volatile, T volatile*)
EMP_TT_DECLARE_TYPE_PARTIAL(add_pointer_t, T const volatile, T const volatile*)

EMP_TT_DECLARE_VALUE(can_add_pointer, not_<is_reference<T>::value>)

template <typename T> using add_pointer = typename add_pointer_t<T>::type;
template <typename T> using try_add_pointer = try_<T, can_add_pointer, add_pointer>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> struct remove_pointer_t;
EMP_TT_DECLARE_TYPE_PARTIAL(remove_pointer_t, T*, T)
EMP_TT_DECLARE_TYPE_PARTIAL(remove_pointer_t, T* const, T)
EMP_TT_DECLARE_TYPE_PARTIAL(remove_pointer_t, T* volatile, T)
EMP_TT_DECLARE_TYPE_PARTIAL(remove_pointer_t, T* const volatile, T)

template <typename T> using remove_pointer = typename remove_pointer_t<T>::type;
template <typename T> using try_remove_pointer = try_<T, is_pointer, remove_pointer>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

