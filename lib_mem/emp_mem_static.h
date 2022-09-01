//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_STATIC_H
#define EMP_MEM_STATIC_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, size_t t_stCount>
EMP_(struct, static_t)
private:
    T m_atData[t_stCount];
    bulk_t<T> m_Bulk;
    //static_assert(sizeof(m_atData) == sizeof_storage, "");
    //static c_size sizeof_T = sizeof(T);
    //static c_size numberof_T = t_stSize;
    //static c_size sizeof_storage = sizeof_T * numberof_T;

public:
    EMP_INLINE static_t(void):
        m_atData(),
        m_Bulk(m_atData, t_stCount)
    {}

    explicit EMP_INLINE static_t(static_t const& a_rFrom) :
        m_atData(a_rFrom),
        m_Bulk(a_rFrom.m_Bulk)
    {}

    static_t& operator=(static_t const& a_rFrom)
    {
        m_atData = a_rFrom.m_atData;
        m_Bulk = a_rFrom.m_Bulk;
        return *this;
    }
public:
    // >> bulk
    EMP_MEM_BULK_IMPL(m_Bulk)
    // << bulk

public:
    EMP_INLINE void grow(c_size EMP_UNUSED(a_stSize), c_size a_stUsed)
    {
        const_assert(a_stSize <= t_stCount);
        const_assert(a_stUsed <= t_stCount);
        m_Bulk.reserve(a_stUsed);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
