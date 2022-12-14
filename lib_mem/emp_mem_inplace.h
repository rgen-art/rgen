//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_INPLACE_H
#define EMP_MEM_INPLACE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_bulk.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(struct, inplace_t)
private:
    bulk_t<T> m_Bulk;

public:
    constexpr EMP_INLINE inplace_t(void):
        m_Bulk()
    {}

    template <typename U>
    constexpr EMP_INLINE inplace_t(bulk_t<U> const& a_rBulk):
        m_Bulk(a_rBulk)
    {}

public:
    // >> bulk
    EMP_MEM_BULK_IMPL(m_Bulk)
    // << bulk
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
