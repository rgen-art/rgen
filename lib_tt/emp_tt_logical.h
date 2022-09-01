//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_LOGICAL_H
#define EMP_TT_LOGICAL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_trait.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef char yes_;
typedef struct { yes_ no[2]; } no_;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_(struct, void_t) typedef void type; };
template <typename T> using void_ = typename void_t<T>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, T t_tValue> EMP_NOINSTANCE(struct, integral) static constexpr T const value = t_tValue; };
// @@0 remove //template <typename T, T t_tValue> T const integral<T, t_tValue>::value;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool t_bValue> EMP_NOINSTANCE_BASE(struct, bool_, public, (integral<bool, t_bValue>)) };
typedef bool_<true> true_;
typedef bool_<false> false_;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool b, bool... bn> EMP_NOINSTANCE_BASE(struct, and_, public, (and_<b, and_<bn...>::value>)) };
template <bool b0, bool b1> EMP_NOINSTANCE_BASE(struct, (and_<b0, b1>), public, bool_<b0 && b1>) };
template <bool b0> EMP_NOINSTANCE_BASE(struct, and_<b0>, public, bool_<b0>) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool b, bool... bn> EMP_NOINSTANCE_BASE(struct, or_, public, (or_<b, or_<bn...>::value>)) };
template <bool b0, bool b1> EMP_NOINSTANCE_BASE(struct, (or_<b0, b1>), public, bool_<b0 || b1>) };
template <bool b0> EMP_NOINSTANCE_BASE(struct, or_<b0>, public, bool_<b0>) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool t_bValue> EMP_NOINSTANCE_BASE(struct, not_, public, bool_<t_bValue == false>) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)
template <typename T, T t_Left, T t_Right> EMP_NOINSTANCE_BASE(struct, less_equal, public, bool_<t_Left <= t_Right>) };
EMP_PRAGMA_POP_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)
template <typename T, T t_Left, T t_Right> EMP_NOINSTANCE_BASE(struct, less, public, bool_<t_Left < t_Right>) };
EMP_PRAGMA_POP_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)
template <typename T, T t_Left, T t_Right> EMP_NOINSTANCE_BASE(struct, more, public, bool_<(t_Left > t_Right)>) };
EMP_PRAGMA_POP_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)
template <typename T, T t_Left, T t_Right> EMP_NOINSTANCE_BASE(struct, more_equal, public, bool_<t_Left >= t_Right>) };
EMP_PRAGMA_POP_IGNORE(EMP_W_COMPARISON_ALWAYS_TRUE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename L, typename R> EMP_NOINSTANCE_BASE(struct, equal, public, false_) };
template <typename T> EMP_NOINSTANCE_BASE(struct, (equal<T, T>), public, true_) };
template <typename L, typename R> using not_equal = not_<equal<L, R>::value>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOINSTANCE_BASE(struct, null, public, (integral<int, 0>)) };
EMP_TT_DECLARE_VALUE(is_null, (equal<T, null>))
EMP_TT_DECLARE_VALUE(is_not_null, not_<is_null<T>::value>)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, bool t_bTry, template <typename> class t_Type>
EMP_NOINSTANCE(struct, try_impl_t)
    typedef T type;
};

template <typename T, template <typename> class t_Type>
EMP_NOINSTANCE(struct, (try_impl_t<T, true, t_Type>))
    typedef typename t_Type<T>::type type;
};

template <typename T, bool t_bTry, template <typename> class t_Type>
using try_impl = typename try_impl_t<T, t_bTry, t_Type>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, bool t_bTry, template <typename> class t_Type>
EMP_NOINSTANCE(struct, try_t)
    typedef T type;
};

template <typename T, template <typename> class t_Type>
EMP_NOINSTANCE(struct, (try_t<T, true, t_Type>))
    typedef t_Type<T> type;
};
template <typename T, template <typename> class t_Try, template <typename> class t_Type>
using try_ = typename try_t<T, t_Try<T>::value, t_Type>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool t_bIf, typename t_Then, typename t_Else>
EMP_NOINSTANCE(struct, if_else_t)
    typedef t_Then type;
};

template <typename t_Then, typename t_Else>
EMP_NOINSTANCE(struct, (if_else_t<false, t_Then, t_Else>))
    typedef t_Else type;
};

template <bool t_bIf, typename t_Then, typename t_Else>
using if_else = typename if_else_t<t_bIf, t_Then, t_Else>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool t_bIf, typename T = void>
EMP_NOINSTANCE(struct, enable_if_t)
};

template <typename T>
EMP_NOINSTANCE(struct, (enable_if_t<true, T>))
    typedef T type;
};

struct dummy {};
template <bool t_bIf, typename T = emp::tt::dummy> using enable_if = typename tt::enable_if_t<t_bIf, T>::type;
template <bool t_bIf, typename T = emp::tt::dummy> using enable_if_not = typename tt::enable_if_t<t_bIf == false, T>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define ENABLE_IF(...) emp::tt::enable_if<__VA_ARGS__>* = nullptr
#define ENABLE_IF_REAL(...) ENABLE_IF(tt::is_real<__VA_ARGS__>::value)
#define ENABLE_IF_NOT_REAL(...) ENABLE_IF(tt::is_not_real<__VA_ARGS__>::value)
#define ENABLE_IF_SIGNED(...) ENABLE_IF(tt::is_signed<__VA_ARGS__>::value)
#define ENABLE_IF_NOT_SIGNED(...) ENABLE_IF(tt::is_not_signed<__VA_ARGS__>::value)
#define ENABLE_IF_POINTER(...) ENABLE_IF(tt::is_pointer<__VA_ARGS__>::value)
#define ENABLE_IF_INTEGRAL(...) ENABLE_IF(tt::is_integral<__VA_ARGS__>::value)
#define ENABLE_IF_NOT_INTEGRAL(...) ENABLE_IF(tt::is_not_integral<__VA_ARGS__>::value)
#define ENABLE_IF_CONST(...) ENABLE_IF(tt::is_const<__VA_ARGS__>::value)
#define ENABLE_IF_NOT_CONST(...) ENABLE_IF(tt::is_not_const<__VA_ARGS__>::value)

#define ENABLE_IF_B(...) template <bool t_bValue, ENABLE_IF(t_bValue == __VA_ARGS__)>
#define ENABLE_IF_T(...) template <typename T, ENABLE_IF(__VA_ARGS__)>
#define ENABLE_IF_(x_Type, ...) template <typename x_Type, ENABLE_IF(__VA_ARGS__)>
#define ENABLE_IF_REAL_T template <typename T, ENABLE_IF_REAL(T)>
#define ENABLE_IF_SIGNED_T template <typename T, ENABLE_IF_SIGNED(T)>
#define ENABLE_IF_NOT_SIGNED_T template <typename T, ENABLE_IF_NOT_SIGNED(T)>
#define ENABLE_IF_POINTER_T template <typename T, ENABLE_IF_POINTER(T)>
#define ENABLE_IF_INTEGRAL_T template <typename T, ENABLE_IF_INTEGRAL(T)>
#define ENABLE_IF_NOT_INTEGRAL_T template <typename T, ENABLE_IF_NOT_INTEGRAL(T)>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

