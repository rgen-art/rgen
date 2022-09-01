//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_PARAM_H
#define EMP_TT_PARAM_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_const.h"
#include "emp_tt_fundamental.h"
#include "emp_tt_reference.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_param, (or_<is_fundamental<T>::value, is_pointer<T>::value>))
EMP_TT_DECLARE_VALUE(is_not_param, not_<is_param<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
using param = if_else<is_param<T>::value, try_add_const<T>, try_add_reference<try_add_const<T>>>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, const_param_t)
public:
    typedef if_else
    <
        emp::tt::is_pointer<T>::value,
        try_add_const<try_add_pointer<try_impl<try_remove_pointer<T>, emp::tt::is_pointer<T>::value, emp::tt::const_param_t>>>,
        if_else
        <
            emp::tt::is_reference<T>::value,
            try_add_reference<try_impl<try_remove_reference<T>, emp::tt::is_reference<T>::value, emp::tt::const_param_t>>,
            try_add_const<T>
        >
    > type;
};

template <typename T> using const_param = param<typename emp::tt::const_param_t<T>::type>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

