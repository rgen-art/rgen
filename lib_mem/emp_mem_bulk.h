//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_BULK_H
#define EMP_MEM_BULK_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_algebra.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

#define EMP_MEM_BULK_IMPL(x_Bulk)\
    EMP_INLINE EMP_RETURN T const*       ptr(c_size a_stIndex) const { return x_Bulk.ptr(a_stIndex); }\
    EMP_INLINE EMP_RETURN T      *       ptr(c_size a_stIndex)       { return x_Bulk.ptr(a_stIndex); }\
    EMP_INLINE EMP_RETURN T const* first_const_ptr(void)       const { return x_Bulk.first_ptr(); }\
    EMP_INLINE EMP_RETURN T      * first_ptr(void)                   { return x_Bulk.first_ptr(); }\
    EMP_INLINE EMP_RETURN T const*  last_ptr(void)             const { return x_Bulk.last_ptr(); }\
    EMP_INLINE EMP_RETURN T      *  last_ptr(void)                   { return x_Bulk.last_ptr(); }\
    /*constexpr EMP_INLINE EMP_RETURN bulk_t<T> const& bulk(void) const { return x_Bulk; }*/\
    constexpr EMP_INLINE EMP_RETURN bulk_t<T>      & bulk(void) const { return x_Bulk; }\
    EMP_INLINE EMP_RETURN bool is_valid_index(c_size a_stIndex) const { return x_Bulk.is_valid_index(a_stIndex); }\
    EMP_INLINE EMP_RETURN size_t count(void) const { return x_Bulk.count(); }\
    EMP_INLINE EMP_RETURN size_t  size(void) const { return x_Bulk.size(); }\
    EMP_INLINE void reserve(c_size a_stUsed) { x_Bulk.reserve(a_stUsed); }\
    EMP_INLINE void clear(void) { x_Bulk.clear(); }\
    EMP_INLINE void swap(c_size a_stFrom, c_size a_stTo) { x_Bulk.swap(a_stFrom, a_stTo); }\
    EMP_INLINE void swap_last(c_size a_stIndex) { x_Bulk.swap_last(a_stIndex); }\
    EMP_INLINE void trim_left(c_size a_stCount) { x_Bulk.trim_left(a_stCount); }\
    EMP_INLINE void trim_right(c_size a_stCount) { x_Bulk.trim_right(a_stCount); }\
    template <typename U> constexpr EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset) const\
    { return x_Bulk.template bulk<U>(a_stOffset); }\
    template <typename U> constexpr EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset, c_size a_stCount) const\
    { return x_Bulk.template bulk<U>(a_stOffset, a_stCount); }\
    EMP_INLINE EMP_RETURN bool is_empty(void) const { return x_Bulk.is_empty(); }\
    EMP_INLINE EMP_RETURN bool is_not_empty(void) const { return x_Bulk.is_not_empty(); }

template <typename T, bool t_bPod>
EMP_NOINSTANCE(struct, pod_allocator_t)
public:
    static EMP_INLINE EMP_RETURN T* allocate(c_size a_stCount)
    { return EMP_ALLOCATE_POD_ARRAY(T, a_stCount); }
    static EMP_INLINE void deallocate(T* const a_pAddress)
    { EMP_DEALLOCATE_POD_ARRAY(a_pAddress); }
};

template <typename T>
EMP_NOINSTANCE(struct, (pod_allocator_t<T, false>))
public:
    static EMP_INLINE EMP_RETURN T* allocate(c_size a_stCount)
    { return EMP_ALLOCATE_ARRAY(T, a_stCount); }
    static EMP_INLINE void deallocate(T* const a_pAddress)
    { EMP_DEALLOCATE_ARRAY(a_pAddress); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN bool do_not_overlap(T const* const a_ptDestination, c_size a_stDestinationSize,
            T const* const a_ptSource, c_size a_stSourceSize);
template <typename T> EMP_INLINE EMP_RETURN bool do_not_overlap(T const* const a_ptDestination, c_size a_stDestinationSize,
            T const* const a_ptSource, c_size a_stSourceSize)
{
    uintptr_t const ptDestination = reinterpret_cast<uintptr_t const>(a_ptDestination);
    uintptr_t const ptDestinationRight = ptDestination + a_stDestinationSize;

    uintptr_t const ptSource = reinterpret_cast<uintptr_t const>(a_ptSource);
    uintptr_t const ptSourceRight = ptSource + a_stSourceSize;

    return  ptDestinationRight < ptSource ||
            ptDestination > ptSourceRight;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> struct bulk_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE void bulk_copy_from_end(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest);
template <typename T> EMP_INLINE void bulk_copy_from_end(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest, c_size a_stCount);

template <typename T> EMP_INLINE void bulk_copy_from_start(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest);
template <typename T> EMP_INLINE void bulk_copy_from_start(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest, c_size a_stCount);

template <typename T> EMP_INLINE void bulk_allocate(bulk_t<T>& a_rBulk, c_size a_stSize, c_size a_stUsed);
template <typename T> EMP_INLINE void bulk_deallocate(bulk_t<T>& a_rBulk);

template <typename T> EMP_INLINE EMP_RETURN size_t reallocable_size(bulk_t<T> const& a_rBulk, c_size a_stSize);
template <typename T> EMP_INLINE void reallocable_grow(bulk_t<T>& a_rBulk, c_size a_stSize, c_size a_stUsed);
template <typename T> EMP_INLINE void reallocable_remove(bulk_t<T>& a_rBulk, c_size a_stIndex, c_size a_stCount);
template <typename T> EMP_INLINE void reallocable_insert(bulk_t<T>& a_rBulk, c_size a_stIndex, c_size a_stCount);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
struct bulk_t
{
    template <typename U> friend struct bulk_t;
    //friend bulk_t<emp::tt::try_remove_const<T>>;
    //friend bulk_t<emp::tt::try_add_const<T>>;

    template <typename U> friend void emp::mem::bulk_copy_from_end(bulk_t<U const> const&, bulk_t<U> const&);
    template <typename U> friend void emp::mem::bulk_copy_from_end(bulk_t<U const> const&, bulk_t<U> const&, c_size);

    template <typename U> friend void emp::mem::bulk_copy_from_start(bulk_t<U const> const&, bulk_t<U> const&);
    template <typename U> friend void emp::mem::bulk_copy_from_start(bulk_t<U const> const&, bulk_t<U> const&, c_size);

    template <typename U> friend void emp::mem::bulk_allocate(bulk_t<U>&, c_size, c_size);
    template <typename U> friend void emp::mem::bulk_deallocate(bulk_t<U>&);

    template <typename U> EMP_RETURN size_t reallocable_size(bulk_t<U> const& a_rBulk, c_size a_stSize);
    template <typename U> void reallocable_grow(bulk_t<U>& a_rBulk, c_size a_stSize, c_size a_stUsed);
    template <typename U> void reallocable_remove(bulk_t<U>& a_rBulk, c_size a_stIndex, c_size a_stCount);
    template <typename U> void reallocable_insert(bulk_t<U>& a_rBulk, c_size a_stIndex, c_size a_stCount);

private:
    T* m_ptData = nullptr;
    size_t m_stSize = 0;
    size_t m_stOffset = 0;
    size_t m_stUsed = 0;

public:
    constexpr EMP_INLINE bulk_t(void):
        m_ptData(nullptr), m_stSize(0), m_stOffset(0), m_stUsed(0)
    {}

    template <size_t t_stSize>
    constexpr EMP_INLINE bulk_t(T (&a_ptData)[t_stSize]):
        m_ptData(a_ptData), m_stSize(t_stSize), m_stOffset(0), m_stUsed(t_stSize)
    {}

    constexpr EMP_INLINE bulk_t(T* const a_ptData, c_size a_stSize, c_size a_stUsed = 0):
        m_ptData(a_ptData), m_stSize(a_stSize), m_stOffset(0), m_stUsed(a_stUsed)
    {}

    constexpr EMP_INLINE bulk_t(T* const a_ptData, c_size a_stSize, bool a_bUsed):
        m_ptData(a_ptData), m_stSize(a_stSize), m_stOffset(0), m_stUsed(a_bUsed ? a_stSize : 0)
    {}

    template <typename U>
    EMP_INLINE bulk_t(bulk_t<U> const& a_rFrom):
        m_ptData(a_rFrom.m_ptData),
        m_stSize(a_rFrom.m_stSize),
        m_stOffset(a_rFrom.m_stOffset),
        m_stUsed(a_rFrom.m_stUsed)
    {}

public:
    EMP_INLINE bool operator==(bulk_t const& a_rFrom)
    {
        return  m_ptData   == a_rFrom.m_ptData   &&
                m_stSize   == a_rFrom.m_stSize   &&
                m_stOffset == a_rFrom.m_stOffset &&
                m_stUsed   == a_rFrom.m_stUsed;
    }

public:
    EMP_INLINE void clear(void)
    {
        m_stOffset = 0;
        m_stUsed = 0;
    }

    constexpr EMP_INLINE EMP_RETURN size_t offset (void) const
    { return m_stOffset; }

    constexpr EMP_INLINE EMP_RETURN size_t count(void) const
    {
        const_assert(offset() <= m_stUsed);
        return m_stUsed - offset();
    }

    constexpr EMP_INLINE EMP_RETURN size_t size(void) const
    {
        const_assert(offset() <= m_stSize);
        return m_stSize - offset();
    }

    constexpr EMP_INLINE EMP_RETURN size_t absolute(c_size a_stValue) const
    { return offset() + a_stValue; }

    constexpr EMP_INLINE EMP_RETURN bool is_valid_index(c_size a_stIndex) const
    { return a_stIndex < count(); }

    template <typename U>
    EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset) const
    {
        const_assert((m_ptData == nullptr && a_stOffset == 0) || a_stOffset < size());
        const_assert((m_ptData == nullptr && a_stOffset == 0) || a_stOffset <= count());
        c_size stOffset = absolute(a_stOffset);
        const_assert((m_ptData == nullptr && stOffset == 0) || stOffset < m_stSize);
        const_assert((m_ptData == nullptr && stOffset == 0) || stOffset <= m_stUsed);
        return bulk_t<U>((m_ptData == nullptr || stOffset >= m_stUsed) ? nullptr : ptr(a_stOffset),
            m_stSize - emp::tt::minof(m_stSize, stOffset),
            m_stUsed - emp::tt::minof(m_stUsed, stOffset));
    }

    template <typename U>
    EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset, c_size a_stCount) const
    {
        EMP_ASSERT((m_ptData == nullptr && a_stOffset == 0) || a_stOffset < size());
        EMP_ASSERT((m_ptData == nullptr && a_stOffset == 0) || a_stOffset <= count());
        c_size stOffset = absolute(a_stOffset);
        EMP_ASSERT((m_ptData == nullptr && stOffset == 0) || stOffset < m_stSize);
        EMP_ASSERT((m_ptData == nullptr && stOffset == 0) || stOffset <= m_stUsed);
        EMP_XX_IF_ASSERT(c_size stCount = absolute(a_stCount);)
        EMP_ASSERT((m_ptData == nullptr && stOffset == 0 && stCount == 0) || stCount < m_stSize);
        EMP_ASSERT((m_ptData == nullptr && stOffset == 0 && stCount == 0) || stCount <= m_stUsed);
        return bulk_t<U>((m_ptData == nullptr || stOffset >= m_stUsed) ? nullptr : ptr(a_stOffset),
            emp::tt::minof(m_stSize, a_stCount),
            emp::tt::minof(m_stUsed, a_stCount));
    }

    /*constexpr*/ EMP_INLINE EMP_RETURN T* ptr(c_size a_stIndex) const
    {
        EMP_ASSERT(is_valid_index(a_stIndex));
        // const_assert(is_valid_index(a_stIndex));
        return &m_ptData[absolute(a_stIndex)];
    }

    //constexpr EMP_INLINE EMP_RETURN T* ptr(c_size a_stIndex)
    //{
    //    const_assert(is_valid_index(a_stIndex));
    //    return &m_ptData[absolute(a_stIndex)];
    //}

    EMP_INLINE EMP_RETURN T const* first_ptr(void) const { return ptr(0); }
    EMP_INLINE EMP_RETURN T* first_ptr(void) { return ptr(0); }

    EMP_INLINE EMP_RETURN T const* last_ptr(void) const
    {
        EMP_ASSERT(count() > 0);
        return ptr(count() - 1);
    }

    EMP_INLINE EMP_RETURN T* last_ptr(void)
    {
        EMP_ASSERT(count() > 0);
        return ptr(count() - 1);
    }

    EMP_INLINE void reserve(c_size a_stCount)
    {
        m_stUsed = absolute(a_stCount);
        const_assert(count() <= size());
    }

    constexpr EMP_INLINE void set(c_size a_stIndex, emp::tt::param<T> a_tValue) const
    { *ptr(a_stIndex) = a_tValue; }

    EMP_INLINE void set_first (emp::tt::param<T> a_tValue)
    { *first_ptr() = a_tValue; }

    EMP_INLINE void set_last(emp::tt::param<T> a_tValue)
    { *last_ptr() = a_tValue; }

    EMP_INLINE void swap(c_size a_stFrom, c_size a_stTo)
    {
        EMP_ASSERT(a_stFrom != a_stTo);
        T oTmp = *ptr(a_stTo);
        set(a_stTo, *ptr(a_stFrom));
        set(a_stFrom, oTmp);
    }

    EMP_INLINE void swap_last(c_size a_stIndex)
    {
        EMP_ASSERT(count() > 0);
        swap(a_stIndex, count() - 1);
    }

    EMP_INLINE void trim_left(c_size a_stCount)
    {
        EMP_ASSERT(a_stCount <= count());
        EMP_ASSERT(a_stCount <= size());
        m_stOffset += a_stCount;
    }

    EMP_INLINE void trim_right(c_size a_stCount)
    {
        EMP_ASSERT(a_stCount <= count());
        EMP_ASSERT(a_stCount <= size());
        reserve(count() - a_stCount);
    }

    EMP_INLINE EMP_RETURN bool is_empty(void) const
    {
        EMP_ASSERT((m_ptData == nullptr && m_stSize == 0 && m_stOffset == 0 && m_stUsed == 0) ||
            (m_ptData != nullptr && m_stSize > 0));
        return m_ptData == nullptr;
    }

    EMP_INLINE EMP_RETURN bool is_not_empty(void) const
    { return is_empty() == false; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void bulk_copy_from_start(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest, c_size a_stCount)
{
   // @@0 READD EMP_ASSERT(a_Dest.count() >= a_stCount && a_Source.count() >= a_stCount);

//    EMP_ASSERT(do_not_overlap(a_Dest.first_ptr(), a_Dest.count(), a_Source.first_ptr(), a_Source.count())); // @@0 uncomment
    c_size stCount = emp::tt::minof(a_Source.count(), emp::tt::minof(a_Dest.count(), a_stCount));
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_Dest.set(stIndex, *a_Source.ptr(stIndex)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void bulk_copy_from_start(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest)
{ emp::mem::bulk_copy_from_start(a_Source, a_Dest, a_Source.count()); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void bulk_copy_from_end(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest, c_size a_stCount)
{
    // @@0 READD EMP_ASSERT(a_Dest.count() >= a_stCount && a_Source.count() >= a_stCount);
//    EMP_ASSERT(do_not_overlap(a_Dest.first_ptr(), a_Dest.count(), a_Source.first_ptr(), a_Source.count())); // @@0 uncomment
    // @@0 optim pod memcpy
    c_size stCount = emp::tt::minof(a_Source.count(), emp::tt::minof(a_Dest.count(), a_stCount));
    if (stCount > 0)
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stOffset = stCount - stIndex - 1;
            a_Dest.set(stOffset, *a_Source.ptr(stOffset));
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> // @@0 optim memmove
EMP_INLINE void bulk_copy_from_end(bulk_t<T const> const& a_Source, bulk_t<T> const& a_Dest)
{ emp::mem::bulk_copy_from_end(a_Source, a_Dest, a_Source.count()); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void bulk_reverse(bulk_t<T> const& a_rBulk)
{
    c_size stCount = a_rBulk.count();
    c_size stHalfCount = stCount / 2;
    for (size_t stIndex = 0; stIndex < stHalfCount; ++stIndex)
    {
        c_size stLast = stCount - stIndex - 1;
        auto const tLast = *a_rBulk.ptr(stLast);
        a_rBulk.set(stLast, *a_rBulk.ptr(stIndex));
        a_rBulk.set(stIndex, tLast);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void bulk_allocate(bulk_t<T>& a_rBulk, c_size a_stSize, c_size a_stUsed)
{
    EMP_ASSERT(a_stSize > 0);
    EMP_ASSERT(a_rBulk.is_empty());
    a_rBulk.m_stSize = a_stSize;
    a_rBulk.m_stUsed = a_stUsed;
    a_rBulk.m_stOffset = 0;
    a_rBulk.m_ptData = emp::mem::pod_allocator_t<T, std::is_pod<T>::value>::allocate(a_stSize);
    //memset(m_ptData, 0, sizeof(T) * a_ui32Size);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void bulk_deallocate(bulk_t<T>& a_rBulk)
{
    EMP_ASSERT(a_rBulk.is_not_empty());
    if (a_rBulk.m_ptData != nullptr)
    {
        emp::mem::pod_allocator_t<T, std::is_pod<T>::value>::deallocate(a_rBulk.m_ptData);
        a_rBulk.m_stSize = 0;
        a_rBulk.m_stUsed = 0;
        a_rBulk.m_stOffset = 0;
        a_rBulk.m_ptData = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN size_t reallocable_size(bulk_t<T> const& a_rBulk, c_size a_stSize)
{
    c_size stSize = a_rBulk.size();
    if (a_stSize > stSize)
    { return emp::tt::maxof(a_rBulk.size() * 2, a_stSize); }
    return stSize;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void reallocable_grow(bulk_t<T>& a_rBulk, c_size a_stSize, c_size a_stUsed)
{
    c_size stBulkSize = a_rBulk.size();
    if (a_stSize <= stBulkSize)
    {
        a_rBulk.reserve(a_stUsed);
        return;
    }

    /*c_size stBulkOffset = a_rBulk.offset(); @@0 when trim remove
    if (stBulkOffset > 0)
    {
        if (a_stSize <= (stBulkSize + stBulkOffset))
        {

            return;
        }
    }*/

    if (a_rBulk.is_not_empty())
    {
        c_size stNewSize = reallocable_size(a_rBulk, a_stSize);
        c_size stCount = a_rBulk.count();
        if (stCount > 0)
        {
            bulk_t<T> oDelete(a_rBulk);
            bulk_t<T const> oSource(a_rBulk);
            a_rBulk = bulk_t<T>();
            emp::mem::bulk_allocate(a_rBulk, stNewSize, a_stUsed);
            emp::mem::bulk_copy_from_end(oSource, a_rBulk);
            emp::mem::bulk_deallocate(oDelete);
        }
        else
        {
            emp::mem::bulk_deallocate(a_rBulk);
            emp::mem::bulk_allocate(a_rBulk, stNewSize, a_stUsed);
        }
    }
    else
    { emp::mem::bulk_allocate(a_rBulk, a_stSize, a_stUsed); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void reallocable_remove(bulk_t<T>& a_rBulk, c_size a_stIndex, c_size a_stCount)
{
    EMP_ASSERT(a_stCount > 0);
    EMP_ASSERT(a_rBulk.is_valid_index(a_stIndex));
    EMP_ASSERT(a_rBulk.is_valid_index(a_stIndex + a_stCount - 1));

    /*if (a_stIndex == 0) @@0 trim remove
    {
        a_rBulk.trim_left(a_stCount);
        return;
    }*/

    c_size stEnd = a_stIndex + a_stCount;
    c_size stUsed = a_rBulk.count();
    EMP_ASSERT(stEnd <= stUsed);
    /*if (stEnd == stUsed)
    {
        a_rBulk.trim_right(a_stCount);
        return;
    }*/

    EMP_ASSERT(a_stCount <= stUsed);
    c_size stNewCount = stUsed - a_stCount;
    if (stEnd < stUsed)
    {
        bulk_t<T const> oSource(a_rBulk.template bulk<T const>(stEnd));
        a_rBulk.reserve(stNewCount);
        emp::mem::bulk_copy_from_start(oSource, a_rBulk.template bulk<T>(a_stIndex), stUsed - stEnd);
    }
    else // TODO:RG unreachable ?
    { a_rBulk.reserve(stNewCount); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void reallocable_insert(bulk_t<T>& a_rBulk, c_size a_stIndex, c_size a_stCount)
{
    EMP_ASSERT(a_stCount > 0);

    if (a_rBulk.is_not_empty())
    {
        c_size stUsed = a_rBulk.count();
        if (stUsed > 0)
        {
            EMP_ASSERT(a_rBulk.is_valid_index(a_stIndex) || a_stIndex == stUsed);

            c_size stNewUsed = stUsed + a_stCount;

            bulk_t<T> oDelete(a_rBulk);
            bulk_t<T const> oSource(a_rBulk);

            c_bool bRealloc = stNewUsed > a_rBulk.size();
            if (bRealloc)
            {
                c_size stNewSize = reallocable_size(a_rBulk, stNewUsed);
                a_rBulk = bulk_t<T>();
                emp::mem::bulk_allocate(a_rBulk, stNewSize, stNewUsed);

                if (a_stIndex > 0)
                { emp::mem::bulk_copy_from_end(oSource, a_rBulk, a_stIndex); }

                if (a_stIndex < stUsed)
                { emp::mem::bulk_copy_from_end(oSource.template bulk<T const>(a_stIndex), a_rBulk.template bulk<T>(a_stIndex + a_stCount)); }

                emp::mem::bulk_deallocate(oDelete);
            }
            else
            {
                a_rBulk.reserve(stNewUsed);
                //if (a_stIndex > 0)
                //{ emp::mem::bulk_move(oSource, a_rBulk, a_stIndex); }

                if (a_stIndex < stUsed)
                { emp::mem::bulk_copy_from_end(oSource.template bulk<T const>(a_stIndex), a_rBulk.template bulk<T>(a_stIndex + a_stCount)); }
            }
        }
        else
        {
            EMP_ASSERT(a_stIndex == 0);
            emp::mem::bulk_deallocate(a_rBulk);
            emp::mem::bulk_allocate(a_rBulk, a_stCount, a_stCount);
        }
    }
    else
    {
        EMP_ASSERT(a_stIndex == 0);
        emp::mem::bulk_allocate(a_rBulk, a_stCount, a_stCount);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
