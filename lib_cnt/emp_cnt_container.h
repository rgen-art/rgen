//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_CONTAINER_H
#define EMP_CNT_CONTAINER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_dereference.h"
#include "emp_tt_param.h"
#include "emp_tt_limit.h"
#include "emp_tt_return.h"

#include "emp_mem_bulk.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-------------------------------u----------------------------------------------
template <typename T, typename t_Storage>
EMP_(struct, container_t)
public:
    typedef T type;

private:
    t_Storage m_Storage;

private:
    template <bool t_bConst>
    EMP_(struct, iterator_impl)
    private:
        typedef emp::tt::if_else<t_bConst, emp::tt::add_const<container_t>, container_t> tarray;
        typedef emp::tt::if_else<t_bConst, emp::tt::try_add_const<T>, T> ttype;

    private:
        friend struct container_t;

    private:
        size_t m_stIndex;
        tarray& m_rArray;

    public:
        EMP_INLINE iterator_impl(iterator_impl const& a_rFrom):
            m_stIndex(a_rFrom.m_stIndex),
            m_rArray(a_rFrom.m_rArray)
        { }

        EMP_INLINE iterator_impl(c_size a_stIndex, tarray& a_rArray):
            m_stIndex(a_stIndex),
            m_rArray(a_rArray)
        { }

        EMP_INLINE iterator_impl& operator=(iterator_impl const& a_rFrom)
        {
            m_stIndex = a_rFrom.m_stIndex;
            EMP_ASSERT(operator==(a_rFrom));
            return *this;
        }

        EMP_INLINE EMP_RETURN ttype& ref(void) { return m_rArray.ref(m_stIndex); }
        EMP_INLINE EMP_RETURN ttype* ptr(void) { return m_rArray.ptr(m_stIndex); }

        EMP_INLINE EMP_RETURN bool operator==(iterator_impl const& a_rFrom) const
        {
            EMP_ASSERT(&m_rArray == &a_rFrom.m_rArray);
            return m_stIndex == a_rFrom.m_stIndex;
        }

        EMP_INLINE EMP_RETURN bool operator!=(iterator_impl const& a_rFrom) const
        { return operator==(a_rFrom) == false; }

        EMP_INLINE iterator_impl& operator++(void)
        {
            EMP_ASSERT(m_rArray.is_valid_index(m_stIndex));
            ++m_stIndex;
            return *this;
        }
    };

public:
    typedef iterator_impl<true> const_iterator;
    typedef iterator_impl<false> iterator;

public:
    EMP_INLINE container_t(void):
        m_Storage()
    { }

    explicit container_t(container_t const& a_rFrom) :
        m_Storage(a_rFrom.m_Storage)
    {}

    ~container_t(void)
    {}

    template <typename U>
    constexpr EMP_INLINE explicit container_t(mem::bulk_t<U> const& a_rBulk):
        m_Storage(a_rBulk)
    {}

public:
    EMP_INLINE container_t& operator=(container_t const& a_rFrom)
    {
        m_Storage = a_rFrom.m_Storage;
        return *this;
    }
public:
    EMP_INLINE EMP_RETURN bool is_owner(void) const
    { return m_Storage.is_owner(); }

public:
    EMP_INLINE void resize(void) { resize(size()); }

    EMP_INLINE void resize(c_size a_stSize)
    { m_Storage.grow(a_stSize, a_stSize); }

    //constexpr EMP_INLINE void const_resize(c_size a_stSize) const
    //{ m_Storage.const_grow(a_stSize, a_stSize); }

    EMP_INLINE void destroy(void)
    {
        resize(0);
        m_Storage.destroy();
    }
    
    EMP_INLINE void clear(void)
    { m_Storage.clear(); }

    EMP_INLINE void insert(c_size a_stIndex, T const& a_rValue)
    {
        m_Storage.insert(a_stIndex, 1);
        set(a_stIndex, a_rValue);
    }

    EMP_INLINE void remove(c_size a_stIndex)
    { remove(a_stIndex, a_stIndex); }

    EMP_INLINE void remove_unordered(c_size a_stIndex)
    {
        c_size stCount = m_Storage.count();
        EMP_ASSERT(stCount > 0);
        if (stCount > 0)
        {
            if (a_stIndex < (stCount - 1))
            { swap_last(a_stIndex); }
            remove_last();
        }
    }

    EMP_INLINE void remove_last(void)
    { remove(count() - 1); }

    EMP_INLINE void remove(c_size a_stBegin, c_size a_stEnd)
    {
        c_size stCount = count();

        EMP_ASSERT(stCount > 0);
        EMP_ASSERT(a_stBegin <= a_stEnd);
        EMP_ASSERT(a_stEnd < stCount);

        c_size stRemoveCount = a_stEnd - a_stBegin + 1;
        EMP_ASSERT(stRemoveCount <= stCount);

        if (a_stEnd + 1 <= stCount)
        { m_Storage.remove(a_stBegin, stRemoveCount); }
        EMP_ASSERT(count() <= size());
    }

    EMP_INLINE EMP_RETURN size_t count(void) const { return m_Storage.count(); }
    EMP_INLINE EMP_RETURN size_t  size(void) const { return m_Storage. size(); }

    template <typename U>
    constexpr EMP_INLINE EMP_RETURN emp::mem::bulk_t<U> bulk(c_size a_stOffset = 0) const
    { return m_Storage.template bulk<U>(a_stOffset); }

    template <typename U>
    constexpr EMP_INLINE EMP_RETURN emp::mem::bulk_t<U> bulk(c_size a_stOffset, c_size a_stCount) const
    { return m_Storage.template bulk<U>(a_stOffset, a_stCount); }

    template <typename U>
    EMP_INLINE EMP_RETURN emp::mem::bulk_t<U> retain_bulk(size_t*& a_rpstReferenceCount, c_size a_stOffset = 0) const
    { return m_Storage.template retain_bulk<U>(a_rpstReferenceCount, a_stOffset); }

    EMP_INLINE EMP_RETURN bool share(T* const a_pFrom, c_size a_stSize, size_t* a_pstReferenceCount)
    { return m_Storage.share(a_pFrom, a_stSize, a_pstReferenceCount); }

    EMP_INLINE EMP_RETURN bool is_valid_index(c_size a_stIndex) const { return m_Storage.is_valid_index(a_stIndex); }
    EMP_INLINE EMP_RETURN bool is_const(void) const { return m_Storage.is_const(); }

    EMP_INLINE void swap(c_size a_stFrom, c_size a_stTo)
    { m_Storage.swap(a_stFrom, a_stTo); }

    EMP_INLINE void swap_last(c_size a_stIndex)
    { m_Storage.swap_last(a_stIndex); }

    EMP_INLINE void trim_left(c_size a_stCount = 1)
    { m_Storage.trim_left(a_stCount); }

    EMP_INLINE void trim_right(c_size a_stCount = 1)
    { m_Storage.trim_right(a_stCount); }

    EMP_INLINE void set       (c_size a_stIndex, emp::tt::param<T> a_tValue) { ref(a_stIndex) = a_tValue; }
    EMP_INLINE void set_first (emp::tt::param<T> a_tValue)                   { first_ref() = a_tValue; }
    EMP_INLINE void set_last  (emp::tt::param<T> a_tValue)                   { last_ref() = a_tValue; }

    EMP_INLINE void add(emp::tt::param<T> a_tValue)
    {
        T& rValue = push();
        rValue = a_tValue;
    }

    EMP_INLINE void add_max(void)
    { add(emp::tt::max<T>::value); }

    EMP_INLINE EMP_RETURN T& push(void)
    {
        c_size stCount = count();
        c_size stNewCount = stCount + 1;
        m_Storage.grow(stNewCount, stNewCount);
        return ref(stCount);
    }

    EMP_INLINE EMP_RETURN bool contains(T const& a_tValue) const
    {
        c_size stCount = count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { if (ref(stIndex) == a_tValue) { return true; } }
        return false;
    }

    EMP_INLINE EMP_RETURN T const* ptr(c_size a_stIndex) const { return m_Storage.ptr(a_stIndex); }
    EMP_INLINE EMP_RETURN T*       ptr(c_size a_stIndex)       { return m_Storage.ptr(a_stIndex); }
    EMP_INLINE EMP_RETURN T const& ref(c_size a_stIndex) const { return          *ptr(a_stIndex); }
    EMP_INLINE EMP_RETURN T&       ref(c_size a_stIndex)       { return          *ptr(a_stIndex); }
    EMP_INLINE EMP_RETURN T        val(c_size a_stIndex) const { return           ref(a_stIndex); }
    // @@0 remove : EMP_INLINE EMP_RETURN T        val(c_size a_stIndex)       { return           ref(a_stIndex); }

    EMP_INLINE EMP_RETURN T const* first_const_ptr (void) const { return m_Storage.first_const_ptr(); }
    EMP_INLINE EMP_RETURN T*       first_ptr       (void)       { return m_Storage.first_ptr(); }
    EMP_INLINE EMP_RETURN T const& first_const_ref (void) const { return          *first_const_ptr(); }
    EMP_INLINE EMP_RETURN T&       first_ref (void)       { return          *first_ptr(); }
    EMP_INLINE EMP_RETURN T        first_val (void) const { return           first_const_ref(); }
    // @@0 remove : EMP_INLINE EMP_RETURN T        first_val (void)       { return           first_ref(); }

    EMP_INLINE EMP_RETURN T const* last_ptr  (void) const { return m_Storage.last_ptr(); }
    EMP_INLINE EMP_RETURN T*       last_ptr  (void)       { return m_Storage.last_ptr(); }
    EMP_INLINE EMP_RETURN T const& last_ref  (void) const { return          *last_ptr(); }
    EMP_INLINE EMP_RETURN T&       last_ref  (void)       { return          *last_ptr(); }
    EMP_INLINE EMP_RETURN T        last_val  (void) const { return           last_ref(); }
    // @@0 remove : EMP_INLINE EMP_RETURN T        last_val  (void)       { return           last_ref(); }

    EMP_INLINE EMP_RETURN const_iterator const begin (void) const { return const_iterator(0      , *this); }
    EMP_INLINE EMP_RETURN iterator       const begin (void)       { return iterator      (0      , *this); }
    EMP_INLINE EMP_RETURN const_iterator const end   (void) const { return const_iterator(count(), *this); }
    EMP_INLINE EMP_RETURN iterator       const end   (void)       { return iterator      (count(), *this); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
