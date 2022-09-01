//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_tt_dereference.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(equal<dereference<char>::treturn, char&>::value, "");
static_assert(equal<dereference<char const>::treturn, char const &>::value, "");
static_assert(equal<dereference<char const&>::treturn, char const &>::value, "");
static_assert(equal<dereference<char const* const>::treturn, char const&>::value, "");
static_assert(equal<dereference<char const* const&>::treturn, char const&>::value, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }
