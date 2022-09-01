//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_CONST_H
#define EMP_TT_CONST_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_logical.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_const, false_)
EMP_TT_DECLARE_VALUE_PARTIAL(is_const, T const, true_)
EMP_TT_DECLARE_VALUE_PARTIAL(is_const, T const volatile, true_)

EMP_TT_DECLARE_VALUE(is_not_const, not_<is_const<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, add_const_t)
    static_assert(is_not_const<T>::value, "");
    typedef T const type;
};

template <typename T> using add_const = typename add_const_t<T>::type;
template <typename T> using try_add_const = try_<T, is_not_const, add_const>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
struct remove_const_t;

EMP_TT_DECLARE_TYPE_PARTIAL(remove_const_t, T const, T)
EMP_TT_DECLARE_TYPE_PARTIAL(remove_const_t, T volatile const, T)

template <typename T> using remove_const = typename remove_const_t<T>::type;
template <typename T> using try_remove_const = try_<T, is_const, remove_const>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

