//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_ARRAY_H
#define EMP_CNT_ARRAY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_static.h"

#include "emp_tt_silent.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename, size_t, bool> struct array;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 unused template <typename T, size_t t_stSize>
void array_copy(array<T, t_stSize>& a_rDestination, array<T, t_stSize> const& a_rSource);*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, size_t t_stSize, bool t_bReserve>
EMP_(struct, array)
private:
    container_t<T, emp::mem::static_t<T, t_stSize>> m_Container;

public:
    explicit array(void):
        m_Container()
    {
        if (t_bReserve)
        { resize(t_stSize); }
    }

    explicit array(c_bool a_bReserve):
        m_Container()
    {
        if (a_bReserve)
        { resize(t_stSize); }
    }

    /*template <typename t_From>
    explicit array(t_From const& a_rFrom)
    {
        static_assert(tstorage::numberof_T == t_From::numberof_T, "");
        resize(tstorage::numberof_T);
        emp::cnt::static_copy<array, t_From, tstorage::numberof_T>::apply(*this, a_rFrom);
    }*/

    explicit array(std::initializer_list<T> const& a_rFrom):
        m_Container()
    {
        resize(a_rFrom.size());
        emp::mem::bulk_copy_from_start(emp::mem::bulk_t<T const>(a_rFrom.begin(), a_rFrom.size(), true), m_Container.template bulk<T>());
    }

    explicit array(T const (&a_aFrom)[t_stSize]):
        m_Container()
    { this->operator=(a_aFrom);}

    constexpr array(array const& a_rFrom):
        m_Container()
    { this->operator=(a_rFrom); }

public:
    array& operator=(T const (&a_aFrom)[t_stSize])
    {
        resize<t_stSize>();
        emp::mem::bulk_copy_from_start(emp::mem::bulk_t<T const>(a_aFrom), m_Container.template bulk<T>());
        return *this;
    }

    array& operator=(array const& a_rFrom)
    {
        EMP_ASSERT(&a_rFrom != this);
        resize(a_rFrom.count());
        emp::mem::bulk_copy_from_start(a_rFrom.template bulk<T const>(), m_Container.template bulk<T>());
        return *this;
    }

public:
    template <size_t t_stIndex>
    void set(emp::tt::param<T> a_tValue)
    {
        static_assert(t_stIndex < t_stSize, "");
        m_Container.set(t_stIndex, a_tValue);
    }

    template <size_t t_stValue>
    void resize(void)
    {
        static_assert(t_stValue <= t_stSize, "");
        m_Container.resize(t_stValue);
    }

    template <size_t t_stIndex>
    EMP_RETURN T val(void)
    {
        static_assert(t_stIndex < t_stSize, "");
        return m_Container.val(t_stIndex);
    }

    template <size_t t_stIndex>
    EMP_RETURN T const val(void) const
    {
        static_assert(t_stIndex < t_stSize, "");
        return m_Container.val(t_stIndex);
    }

    EMP_INLINE EMP_RETURN bool contains(T const& a_tValue) const
    { return m_Container.contains(a_tValue); }

    EMP_INLINE EMP_RETURN T const first_val(void) const { return m_Container.first_val(); }
    EMP_INLINE EMP_RETURN T const val(c_size a_stIndex) const { return m_Container.val(a_stIndex); }
    EMP_INLINE EMP_RETURN T const& ref(c_size a_stIndex) const { return m_Container.ref(a_stIndex); }
    EMP_INLINE EMP_RETURN T const& last_ref(void) const { return m_Container.last_ref(); }
    EMP_INLINE EMP_RETURN T& last_ref(void) { return m_Container.last_ref(); }
    EMP_INLINE EMP_RETURN T& ref(c_size a_stIndex) { return m_Container.ref(a_stIndex); }
    EMP_INLINE EMP_RETURN T const* ptr(c_size a_stIndex) const { return m_Container.ptr(a_stIndex); }
    EMP_INLINE EMP_RETURN T* ptr(c_size a_stIndex) { return m_Container.ptr(a_stIndex); }
    EMP_INLINE EMP_RETURN T* first_ptr(void) { return m_Container.first_ptr(); }
    EMP_INLINE EMP_RETURN T const* first_ptr(void) const { return m_Container.first_ptr(); }

    EMP_INLINE void resize(void) { m_Container.resize(); }
    EMP_INLINE void resize(c_size a_stCount) { m_Container.resize(a_stCount); }
    EMP_INLINE EMP_RETURN size_t count(void) const { return m_Container.count(); }
    EMP_INLINE EMP_RETURN size_t size(void) const { return m_Container.size(); }

    EMP_INLINE void set(c_size a_stIndex, T const& a_Value) { m_Container.set(a_stIndex, a_Value); }

    template <typename U>
    constexpr EMP_INLINE EMP_RETURN emp::mem::bulk_t<U> bulk(c_size a_stOffset, c_size a_stCount) const
    { return m_Container.template bulk<U>(a_stOffset, a_stCount); }

    template <typename U>
    constexpr EMP_INLINE EMP_RETURN emp::mem::bulk_t<U> bulk(c_size a_stOffset = 0) const
    { return m_Container.template bulk<U>(a_stOffset); }

    template <size_t t_stIndex>
    EMP_RETURN T& ref(void)
    {
        static_assert(t_stIndex < t_stSize, "");
        return m_Container.ref(t_stIndex);
    }

    template <size_t t_stIndex>
    EMP_RETURN T const& ref(void) const
    {
        static_assert(t_stIndex < t_stSize, "");
        return m_Container.ref(t_stIndex);
    }

    EMP_INLINE void add(emp::tt::param<T> a_tValue)
    { m_Container.add(a_tValue); }


};

template <typename T, size_t t_stSize, bool t_bReserve> using c_array = array<T, t_stSize, t_bReserve> const;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 unused template <typename t_Array, size_t t_stCount>
EMP_NOINSTANCE(struct, array_copy_t)
    static void apply(t_Array& a_rDestination, t_Array const& a_rSource)
    {
        c_size stIndex = t_stCount - 1;
        a_rDestination.template ref<stIndex>() = a_rSource.template ref<stIndex>();
        array_copy_t<t_Array, stIndex>::apply(a_rDestination, a_rSource);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Array>
EMP_NOINSTANCE(struct, (array_copy_t<t_Array, 0>))
    static void apply(t_Array&, t_Array const&) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, size_t t_stSize>
void array_copy(array<T, t_stSize>& a_rDestination, array<T, t_stSize> const& a_rSource)
{ array_copy_t<array<T, t_stSize>, t_stSize>::apply(a_rDestination, a_rSource); }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
