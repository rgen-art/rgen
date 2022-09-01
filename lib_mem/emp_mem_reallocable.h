//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_REALLOCABLE_H
#define EMP_MEM_REALLOCABLE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_bulk.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, bool t_bPod = false>
EMP_NOCOPY(struct, reallocable_t)
public:
    bulk_t<T> m_Bulk;

public:
    EMP_INLINE reallocable_t(void):
        m_Bulk()
    {}

    EMP_INLINE ~reallocable_t(void)
    { destroy(); }

public:
    // >> bulk
    EMP_MEM_BULK_IMPL(m_Bulk)
    // << bulk

public:
    EMP_INLINE void destroy(void)
    {
        if (m_Bulk.is_not_empty())
        { emp::mem::bulk_deallocate(m_Bulk); }
    }

public:
    EMP_INLINE void grow(c_size a_stSize, c_size a_stUsed)
    { reallocable_grow(m_Bulk, a_stSize, a_stUsed); }

    EMP_INLINE void remove(c_size a_stIndex, c_size a_stCount)
    { reallocable_remove(m_Bulk, a_stIndex, a_stCount); }

    EMP_INLINE void insert(c_size a_stIndex, c_size a_stCount)
    { reallocable_insert(m_Bulk, a_stIndex, a_stCount); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
