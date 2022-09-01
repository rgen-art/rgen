//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_DEREFERENCE_H
#define EMP_TT_DEREFERENCE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_const.h"
#include "emp_tt_logical.h"
#include "emp_tt_pointer.h"
#include "emp_tt_reference.h"
#include "emp_tt_volatile.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, dereference)
    typedef if_else
    <
        is_reference<T>::value,
        if_else
        <
            is_pointer<try_remove_reference<T>>::value,
            try_remove_const<try_remove_volatile<try_remove_reference<T>>>,
            T
        >,
        if_else
        <
            is_pointer<T>::value,
            try_remove_const<try_remove_volatile<T>>,
            add_reference<T>
        >
    > timpl;

    typedef try_add_reference<try_remove_pointer<timpl>> treturn;
    typedef if_else<is_reference<T>::value, T, add_reference<T>> tparam;

    static treturn apply(tparam a_rtValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
struct dereference_impl;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename  T>
EMP_NOINSTANCE(struct, dereference_impl<T&>)
    static EMP_RETURN T& apply(T& a_rValue) { return a_rValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, dereference_impl<T*>)
    static EMP_RETURN T& apply(T* const a_pValue)
    { return emp::tt::dereference<T>::apply(*a_pValue); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN typename dereference<T>::treturn dereference<T>::apply(typename dereference<T>::tparam a_rtValue)
{ return dereference_impl<timpl>::apply(a_rtValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

