//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_UTIL_H
#define EMP_MEM_UTIL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct size_s
{
    size_t m_stCount;
    size_s(void) : m_stCount(0) {}
    explicit size_s(c_size a_stCount) : m_stCount(a_stCount) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct profile_s
{
    size_s m_AllocBytes = size_s();
    size_t m_stAllocCount = 0;
    size_s m_DeallocBytes = size_s();
    size_t m_stDeallocCount = 0;

    size_s net_bytes(void) const { return size_s(m_AllocBytes.m_stCount - m_DeallocBytes.m_stCount); }
    size_t net_count(void) const { return m_stAllocCount - m_stDeallocCount; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void set(void* const a_pBuffer, c_size a_stSize, c_char a_cFiller);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE void zero(void* const a_pBuffer, c_size a_stSize);
EMP_INLINE void zero(void* const a_pBuffer, c_size a_stSize)
{ set(a_pBuffer, a_stSize, 0); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN size_t len(T const* const a_pFrom);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN size_t nlen(T const* const a_pFrom, c_size a_stMax);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
