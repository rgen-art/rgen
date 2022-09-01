//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_CLASS_H
#define EMP_TT_CLASS_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_logical.h"

#include "emp_hh_type_traits.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

/* @@0 remove #define EMP_TT_HAS_FUNCTION(x_Name)\
template <typename C, typename F, typename = void>\
EMP_NOINSTANCE_BASE(struct, EMP_PP_CAT(has_function_, x_Name), public, emp::tt::false_) };\
template <typename C, typename R, typename... A>\
EMP_NOINSTANCE_BASE(struct, (EMP_PP_CAT(has_function_, x_Name)\
    <\
        C, R(A...),\
        tt::enable_if\
        <\
            tt::equal<R, void>::value ||\
            std::is_convertible<decltype(std::declval<C>().x_Name(std::declval<A>()...)), R>::value\
        >\
    >), public, tt::true_)\
};*/

#define EMP_TT_HAS_FUNCTION(x_Name)\
template <typename C, typename F>\
EMP_NOINSTANCE_BASE(struct, EMP_PP_CAT(has_function_, x_Name), public, emp::tt::false_) };\
template <typename C, typename R, typename... A>\
EMP_NOINSTANCE_BASE(struct, (EMP_PP_CAT(has_function_, x_Name)<C, R(A...)>), public, emp::tt::true_)\
    template <typename CC, typename CR, typename... CA>\
    static EMP_RETURN emp::tt::yes_ test(emp::tt::enable_if<emp::tt::equal<decltype(std::declval<CC>().x_Name(std::declval<CA>()...)), CR>::value, int>);\
    template <typename CC, typename CR, typename... CA>\
    static EMP_RETURN emp::tt::no_  test(...);\
public:\
    static constexpr bool value = emp::tt::equal<decltype(test<C, R, A...>(0)), emp::tt::yes_>::value;\
};
/* @@0 remove static constexpr bool value = emp::tt::equal<decltype(std::declval<C>().x_Name(std::declval<A>()...)), R>::value;\
#define EMP_TT_HAS_FUNCTION(x_Name)\
template <typename C, typename F>\
EMP_NOINSTANCE_BASE(class, EMP_PP_CAT(has_function_, x_Name), public, emp::tt::false_) };\
template <typename C, typename R, typename... A>\
EMP_NOINSTANCE(class, EMP_PP_CAT(has_function_, x_Name)<C, R(A...)>)\
    
    template <typename U, emp::tt::enable_if<decltype(std::declval<C>().x_Name(std::declval<C>())

};
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TT_HAS_FUNCTION(pre_load)
EMP_TT_HAS_FUNCTION(post_load)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename B, typename D>
using is_base = std::is_base_of<B, D>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<typename T, typename R = void>
EMP_NOINSTANCE(struct, enable_if_type) typedef R type; };

#define EMP_TT_HAS_TYPEDEF(x_Typedef)\
template <typename T, class D = void>\
EMP_NOINSTANCE_BASE(struct,  has_typedef_##x_Typedef, public, false_) };\
template <typename T>\
EMP_NOINSTANCE_BASE(struct, (has_typedef_##x_Typedef<T, typename enable_if_type<typename T::x_Typedef>::type>), public, true_) }

EMP_TT_HAS_TYPEDEF( type   );
EMP_TT_HAS_TYPEDEF( tbases );
EMP_TT_HAS_TYPEDEF( tbase1 );
EMP_TT_HAS_TYPEDEF( tbase2 );
EMP_TT_HAS_TYPEDEF( tbase3 );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

