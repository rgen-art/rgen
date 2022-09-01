//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_tt_logical.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(and_<true>::value, "");
static_assert(and_<true, true>::value, "");
static_assert(and_<true, false>::value == false, "");
static_assert(and_<false, true>::value == false, "");
static_assert(and_<true, true, true>::value, "");
static_assert(and_<true, true, false>::value == false, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(or_<true>::value, "");
static_assert(or_<true, true>::value, "");
static_assert(or_<true, false>::value, "");
static_assert(or_<false, true>::value, "");
static_assert(or_<true, true, true>::value, "");
static_assert(or_<true, true, false>::value, "");
static_assert(or_<false>::value == false, "");
static_assert(or_<false, false>::value == false, "");
static_assert(or_<false, false, false>::value == false, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(not_<true>::value == false, "");
static_assert(not_<false>::value == true, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(less_equal<size_t, 0, 1>::value, "");
static_assert(less_equal<size_t, 0, 0>::value, "");
static_assert(less_equal<size_t, 1, 0>::value == false, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(less<size_t, 0, 1>::value, "");
static_assert(less<size_t, 0, 0>::value == false, "");
static_assert(less<size_t, 1, 0>::value == false, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(more<size_t, 0, 1>::value == false, "");
static_assert(more<size_t, 0, 0>::value == false, "");
static_assert(more<size_t, 1, 0>::value, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(more_equal<size_t, 0, 1>::value == false, "");
static_assert(more_equal<size_t, 0, 0>::value, "");
static_assert(more_equal<size_t, 1, 0>::value, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(equal<int, int>::value, "");
static_assert(not_equal<int, char>::value, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }
