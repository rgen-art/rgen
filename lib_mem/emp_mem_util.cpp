//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_mem_util.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN size_t len(T const* const a_pFrom)
{
    EMP_ASSERT(a_pFrom != nullptr);

    T const* pFrom = a_pFrom;
    size_t stLen = 0;
    while (*pFrom != 0)
    {
        ++stLen;
        ++pFrom;
    }
    return stLen;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN size_t nlen(T const* const a_pFrom, c_size a_stMax)
{
    EMP_ASSERT(a_pFrom != nullptr);

    T const* pFrom = a_pFrom;
    size_t stLen = 0;
    size_t stMax = a_stMax;
    while (*pFrom != 0 && stMax > 0)
    {
        ++stLen;
        ++pFrom;
        --stMax;
    }
    return stLen;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template size_t len(cpc_char);
template size_t len(c_ui8* const);
template size_t len(c_ui16* const);
template size_t len(c_ui32* const);

template size_t nlen(cpc_char, c_size);
template size_t nlen(c_ui32* const, c_size);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void set(void* const a_pBuffer, c_size a_stSize, char const a_cFiller)
{ memset(a_pBuffer, a_cFiller, a_stSize); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
