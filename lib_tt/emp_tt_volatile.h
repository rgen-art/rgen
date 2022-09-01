//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_VOLATILE_H
#define EMP_TT_VOLATILE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_volatile, false_)
EMP_TT_DECLARE_VALUE_PARTIAL(is_volatile, T volatile, true_)
EMP_TT_DECLARE_VALUE_PARTIAL(is_volatile, T const volatile, true_)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_DECLARE_VALUE(is_not_volatile, not_<is_volatile<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, add_volatile_t)
    static_assert(is_not_volatile<T>::value, "");
    typedef T volatile type;
};

template <typename T> using add_volatile = typename add_volatile_t<T>::type;
template <typename T> using try_add_volatile = try_<T, is_not_volatile, add_volatile>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> struct remove_volatile_t;
EMP_TT_DECLARE_TYPE_PARTIAL(remove_volatile_t, T volatile, T)
EMP_TT_DECLARE_TYPE_PARTIAL(remove_volatile_t, T const volatile, T)

template <typename T> using remove_volatile = typename remove_volatile_t<T>::type;
template <typename T> using try_remove_volatile = try_<T, is_volatile, remove_volatile>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

