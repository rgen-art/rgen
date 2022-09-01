//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_MEMORY_H
#define EMP_OSE_MEMORY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_copy.h"
#include "emp_cnt_vector.h"

#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, memory_c)
private:
    emp::cnt::vector<unsigned char> m_vcData;

public:
    memory_c(void):
        m_vcData()
    {}

public:
    EMP_RETURN size_t byte_count(void) const
    { return m_vcData.size() * sizeof(unsigned char); }

public:
    void clear(void)
    { m_vcData.resize(0);}

    void copy(memory_c const& a_rFrom)
    { emp::cnt::copy(a_rFrom.m_vcData, m_vcData); }

    EMP_RETURN size_t count(void) const
    { return m_vcData.count(); }

    template <typename T>
    EMP_RETURN bool val(size_t a_stIndex, T& a_rtValue) const
    {
        a_rtValue = 0;
        constexpr c_size stSize = sizeof(a_rtValue);
        c_size stCount = m_vcData.count();
        if ((a_stIndex < stCount) && (a_stIndex + stSize <= stCount))
        {
            memcpy(&a_rtValue, m_vcData.ptr(a_stIndex), stSize);
            return true;
        }
        return false;
    }

    template <typename T>
    void push(T const a_tValue)
    {
        constexpr c_size stSize = sizeof(a_tValue);
        c_size stCount = m_vcData.count();
        m_vcData.resize(stCount + stSize);
        memcpy(m_vcData.ptr(stCount), &a_tValue, sizeof(a_tValue));
    }

    template <typename V, typename T>
    EMP_RETURN bool push_ex(T const a_tValue)
    {
        if (a_tValue <= emp::tt::max<V>::value &&
            a_tValue >= emp::tt::min<V>::value)
        {
            push<V>(a_tValue);
            return true;
        }
        return false;
    }

    template <typename T>
    EMP_RETURN bool set(c_ui32 a_ui32Address, T const a_tValue)
    {
        constexpr c_size stSize = sizeof(a_tValue);
        c_size stCount = m_vcData.count();
        if (a_ui32Address + stSize <= stCount)
        {
            memcpy(m_vcData.ptr(a_ui32Address), &a_tValue, sizeof(a_tValue));
            return true;
        }
        return false;
    }

    template <typename V, typename T>
    EMP_RETURN bool set_ex(c_ui32 a_ui32Address, T const a_tValue)
    {
        if (a_tValue <= emp::tt::max<V>::value &&
            a_tValue >= emp::tt::min<V>::value)
        { return set<V>(a_ui32Address, emp::ast::cast<V>(a_tValue)); }
        return false;
    }

    template <typename T>
    EMP_RETURN bool pop(T& a_rtValue)
    {
        a_rtValue = 0;
        constexpr c_size stSize = sizeof(a_rtValue);
        c_size stCount = m_vcData.count();
        if (stCount < stSize)
        { return false; }
        c_size stPointer = stCount - stSize;
        memcpy(&a_rtValue, m_vcData.ptr(stPointer), stSize);
        m_vcData.resize(stPointer);
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, memory_walker_c)
private:
    memory_c const& m_rMemory;
    size_t m_stPointer;
public:
    explicit memory_walker_c(memory_c const& a_rMemory):
        m_rMemory(a_rMemory),
        m_stPointer(0)
    {}

public:
    EMP_RETURN bool move_to(c_size a_stIndex)
    {
        if (a_stIndex < m_rMemory.count())
        {
            m_stPointer = a_stIndex;
            return true;
        }

        return false;
    }

    void move_to_eof(void)
    { m_stPointer = m_rMemory.count(); }

    template <typename T>
    EMP_RETURN bool next(T& a_rtValue)
    {
        if (m_rMemory.val(m_stPointer, a_rtValue))
        {
            m_stPointer += sizeof(T);
            return true;
        }
        return false;
    }

    EMP_RETURN size_t pointer(void) const
    { return m_stPointer; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

