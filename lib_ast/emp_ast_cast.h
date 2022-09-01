//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_AST_CAST_H
#define EMP_AST_CAST_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ast {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_From, t_From t_tFrom, typename t_To>
EMP_NOINSTANCE(struct, compile_cast)
    // explicit min/max
    static_assert(t_tFrom >= emp::tt::min<t_To>::value, "");
    static_assert(t_tFrom <= emp::tt::max<t_To>::value, "");
    static const t_To value = static_cast<t_To>(t_tFrom);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename t_From, typename t_To>
EMP_RETURN bool try_cast_impl(t_From a_tFrom, t_To& a_rtTo);*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 duplicate
#define XTMP_NEG(x_Type)\
    ENABLE_IF((emp::tt::is_integral<x_Type>::value == false) || emp::tt::is_signed<x_Type>::value)
#define XTMP_NOT_NEG(x_Type)\
    ENABLE_IF((((emp::tt::is_integral<x_Type>::value == false) || emp::tt::is_signed<x_Type>::value) == false))

/*template <typename t_From, typename t_To, XTMP_NEG(t_From), XTMP_NEG(t_To)>
EMP_INLINE EMP_RETURN bool try_cast_impl(t_From a_tFrom, t_To& a_rtTo)
{
    static_assert(emp::tt::is_arithmetic<t_From>::value, "");
    static_assert(emp::tt::is_arithmetic<t_To>::value, "");
    static_assert(emp::tt::not_equal<t_From, t_To>::value, "");

    if (a_tFrom > emp::tt::max<t_To>::value)
    { EMP_ASSERT(false); return false; }
    else if (a_tFrom < emp::tt::min<t_To>::value)
    { EMP_ASSERT(false); return false; }

    a_rtTo = static_cast<t_To>(a_tFrom);
    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 duplicate
/*template <typename t_From, typename t_To, XTMP_NOT_NEG(t_From), XTMP_NOT_NEG(t_To)>
EMP_INLINE EMP_RETURN bool try_cast_impl(t_From a_tFrom, t_To& a_rtTo)
{
    static_assert(emp::tt::is_arithmetic<t_From>::value, "");
    static_assert(emp::tt::is_arithmetic<t_To>::value, "");
    static_assert(emp::tt::not_equal<t_From, t_To>::value, "");

    if (a_tFrom > emp::tt::max<t_To>::value)
    { EMP_ASSERT(false); return false; }
    else if (a_tFrom < emp::tt::min<t_To>::value)
    { EMP_ASSERT(false); return false; }

    a_rtTo = static_cast<t_To>(a_tFrom);
    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 duplicate
/*typename emp::tt::enable_if<
    emp::tt::more_equal<size_t, sizeof(t_From), sizeof(t_To)>::value,
bool>::type*/
/*template <typename t_From, typename t_To, XTMP_NOT_NEG(t_From), XTMP_NEG(t_To)>
EMP_INLINE EMP_RETURN bool try_cast_impl(t_From a_tFrom, t_To& a_rtTo)
{
    static_assert(emp::tt::is_arithmetic<t_From>::value, "");
    static_assert(emp::tt::is_arithmetic<t_To>::value, "");
    static_assert(emp::tt::not_equal<t_From, t_To>::value, "");

    // @@0 wtf
    if (a_tFrom > emp::tt::max<t_To>::value)
    { EMP_ASSERT(false); return false; }

    a_rtTo = static_cast<t_To>(a_tFrom);
    return true;
}

EMP_INLINE EMP_RETURN bool try_cast_impl(unsigned long a_tFrom, float& a_rTo);
EMP_INLINE EMP_RETURN bool try_cast_impl(unsigned long a_tFrom, float& a_rTo)
{
    if (a_tFrom >= (1 << 24))
    { return false; }

    a_rTo = static_cast<float>(a_tFrom);
    return true;
}*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 duplicate
/*typename emp::tt::enable_if
    <sizeof(t_From) == sizeof(t_To), bool>::type*/
/*template <typename t_From, typename t_To, XTMP_NEG(t_From), XTMP_NOT_NEG(t_To)>
EMP_INLINE EMP_RETURN  bool try_cast_impl(t_From a_tFrom, t_To& a_rtTo)
{
    static_assert(emp::tt::is_arithmetic<t_From>::value, "");
    static_assert(emp::tt::is_arithmetic<t_To>::value, "");
    static_assert(emp::tt::not_equal<t_From, t_To>::value, "");

    if (a_tFrom < 0)
    { return false; }

    EMP_PRAGMA_PUSH_IGNORE(EMP_W_SIGN_COMPARE) // @@0 early out when < 0
    if (a_tFrom > emp::tt::max<t_To>::value)
    { return false; }
    EMP_PRAGMA_POP_IGNORE(EMP_W_SIGN_COMPARE)

    a_rtTo = static_cast<t_To>(a_tFrom);
    return true;
}*/

template <typename F, typename T>
EMP_INLINE EMP_RETURN bool try_cast_impl(F a_tFrom, T& a_rtTo)
{
    // @@0 cast impl
    a_rtTo = static_cast<T>(a_tFrom);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_From, typename t_To>
EMP_NOINSTANCE(struct, try_cast_detail)
    static EMP_RETURN bool apply(t_From a_tFrom, t_To& a_rtTo)
    { return try_cast_impl<t_From, t_To>(a_tFrom, a_rtTo); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_From>
EMP_NOINSTANCE(struct, (try_cast_detail<t_From, bool>))
    static EMP_RETURN bool apply(t_From a_tFrom, bool& a_rtTo)
    {
        if (a_tFrom != 0 && a_tFrom != 1)
        { EMP_ASSERT(false); return false; }

        a_rtTo = static_cast<bool>(a_tFrom);
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, (try_cast_detail<T, T>))
    static EMP_RETURN bool apply(T a_tFrom, T& a_rtTo)
    {
        static_assert(emp::tt::is_arithmetic<T>::value, "");

        a_rtTo = a_tFrom;
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_From, typename t_To> EMP_INLINE EMP_RETURN bool try_cast(t_From a_tFrom, t_To& a_rtTo);
template <typename t_From, typename t_To> EMP_INLINE EMP_RETURN bool try_cast(t_From a_tFrom, t_To& a_rtTo)
{ return try_cast_detail<t_From, t_To>::apply(a_tFrom, a_rtTo); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_To, typename t_From> EMP_INLINE EMP_RETURN t_To cast(t_From const a_tFrom);
template <typename t_To, typename t_From> EMP_INLINE EMP_RETURN t_To cast(t_From const a_tFrom)
{
    t_To tResult = 0;
    EMP_ASSERT_BOOL_CALL((try_cast<t_From, t_To>(a_tFrom, tResult)));
    return tResult;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

