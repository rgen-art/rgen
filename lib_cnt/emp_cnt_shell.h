//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_SHELL_H
#define EMP_CNT_SHELL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_forward.h"

#include "emp_hh_initializer_list.h"

#include "emp_tt_silent.h"

#include "emp_mem_inplace.h"
#include "emp_mem_util.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(class, shell)
public:
    typedef T type;

private:
    container_t<T, emp::mem::inplace_t<T>> m_Container;

public:
    constexpr shell(void):
        m_Container()
    { }

    constexpr shell(shell const& a_rFrom):
        m_Container(a_rFrom.m_Container)
    { }

    constexpr explicit shell(std::initializer_list<T> const& a_rFrom):
        m_Container(emp::mem::bulk_t<T>(a_rFrom.begin(), a_rFrom.size(), a_rFrom.size()))
    {}

    /*shell(T* const a_pFrom, c_size a_stSize, c_bool a_bReserve):
        tbase(a_pFrom, a_stSize, a_bReserve ? a_stSize : 0)
    { }*/

    constexpr shell(T* const a_pFrom, c_size a_stSize, c_size a_stCount):
        m_Container(emp::mem::bulk_t<T>(a_pFrom, a_stSize, a_stCount))
    { }

    template <size_t t_stSize>
    constexpr explicit shell(T (&a_aFrom)[t_stSize], c_size a_stCount = t_stSize):
        m_Container(emp::mem::bulk_t<T>(a_aFrom, t_stSize, a_stCount))
    { }

    /*template <typename U, size_t t_stSize>
    explicit shell(U (&a_aFrom)[t_stSize], c_size a_stCount = t_stSize):
        tbase(a_aFrom, t_stSize, a_stCount)
    { }*/

    template <typename t_From>
    explicit shell(t_From const& a_rFrom):
        m_Container(a_rFrom.template bulk<T const>())
    { }

    template <typename U>
    constexpr shell(emp::mem::bulk_t<U> const& a_rBulk, c_size a_stBegin, c_size a_stCount, c_bool a_bReserve):
        m_Container(emp::mem::bulk_t<U>(a_rBulk.ptr(a_stBegin), a_stCount, a_bReserve ? a_stCount : 0))
    { const_assert((a_stBegin + a_stCount) <= a_rBulk.count()); }

    /*template <typename t_From>
    explicit shell(t_From& a_rFrom):
        tbase(a_rFrom.bulk().m_ptData, a_rFrom.bulk().m_stSize, a_rFrom.count())
    { }*/

    shell& operator=(shell const& a_rFrom)
    {
        emp::tt::silent(m_Container.operator=(a_rFrom.m_Container));
        return *this;
    }

    constexpr EMP_INLINE EMP_RETURN T const val(c_size a_stIndex) const { return m_Container.val(a_stIndex); }

    EMP_INLINE EMP_RETURN T& ref(c_size a_stIndex) { return m_Container.ref(a_stIndex); }
    constexpr EMP_INLINE EMP_RETURN T const& ref(c_size a_stIndex) const { return m_Container.ref(a_stIndex); }

    EMP_INLINE EMP_RETURN T* ptr(c_size a_stIndex) { return m_Container.ptr(a_stIndex); }
    constexpr EMP_INLINE EMP_RETURN T const* ptr(c_size a_stIndex) const { return m_Container.ptr(a_stIndex); }

    EMP_INLINE EMP_RETURN T* first_ptr(void) { return m_Container.first_ptr(); }
    constexpr EMP_INLINE EMP_RETURN T const* first_ptr(void) const { return m_Container.first_ptr(); }

    EMP_INLINE void trim_left(c_size a_stIndex = 1) { m_Container.trim_left(a_stIndex); }
    EMP_INLINE void trim_right(c_size a_stIndex = 1) { m_Container.trim_right(a_stIndex); }

public:
    constexpr EMP_INLINE EMP_RETURN size_t count(void) const { return m_Container.count(); }

    template <typename U>
    EMP_INLINE EMP_RETURN emp::mem::bulk_t<U> const bulk(void) { return m_Container.template bulk<U>(); }

    template <typename U>
    EMP_INLINE EMP_RETURN emp::mem::bulk_t<U const> const bulk(void) const { return m_Container.template bulk<U const>(); }


public:
    template <typename U>
    constexpr static shell from_container(U const& a_rContainer)
    { return shell(a_rContainer.first_ptr() + a_rContainer.offset(), a_rContainer.count(), true);    }

    template <typename U>
    constexpr static shell from_null_terminated(U const& a_rFrom)
    { return shell(a_rFrom, a_rFrom != nullptr ? emp::mem::len(a_rFrom) : 0, true); }
};

template <typename T> using c_shell = shell<T> const;
template <typename T> using cp_shell = shell<T> const*;
template <typename T> using cpc_shell = shell<T> const* const;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
