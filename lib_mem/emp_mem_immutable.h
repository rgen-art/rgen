//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_IMMUTABLE_H
#define EMP_MEM_IMMUTABLE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(struct, immutable_t)
private:
    mutable size_t* m_pstReferenceCount;
    bulk_t<T> m_Bulk;
    bulk_t<T const> m_Const;

public:
    explicit EMP_INLINE immutable_t(void):
        m_pstReferenceCount(nullptr),
        m_Bulk(),
        m_Const()
    {}

    explicit EMP_INLINE immutable_t(immutable_t const& a_rFrom):
        m_pstReferenceCount(nullptr),
        m_Bulk(),
        m_Const()
    { operator=(a_rFrom); }

    explicit EMP_INLINE immutable_t(immutable_t&& a_rrFrom):
        m_pstReferenceCount(a_rrFrom.m_pstReferenceCount),
        m_Bulk(a_rrFrom.m_Bulk),
        m_Const(a_rrFrom.m_Const)
    {
        a_rrFrom.m_pstReferenceCount = nullptr;
        a_rrFrom.m_Bulk = bulk_t<T>();
        a_rrFrom.m_Const = bulk_t<T const>();
    }

    explicit EMP_INLINE immutable_t(bulk_t<emp::tt::try_add_const<T>> const& a_rFrom):
        m_pstReferenceCount(nullptr),
        m_Bulk(),
        m_Const(a_rFrom)
    {}

    explicit EMP_INLINE immutable_t(bulk_t<emp::tt::try_remove_const<T>> const& a_rFrom):
        m_pstReferenceCount(nullptr),
        m_Bulk(a_rFrom),
        m_Const()
    {}

    EMP_INLINE ~immutable_t(void)
    { deallocate(); }

    EMP_INLINE immutable_t& operator=(immutable_t&& a_rrFrom)
    {
        m_pstReferenceCount = a_rrFrom.m_pstReferenceCount,
        m_Bulk = a_rrFrom.m_Bulk;
        m_Const = a_rrFrom.m_Const;

        a_rrFrom.m_pstReferenceCount = nullptr;
        a_rrFrom.m_Bulk = bulk_t<T>();
        a_rrFrom.m_Const = bulk_t<T const>();

        return *this;
    }

    EMP_INLINE immutable_t& operator=(immutable_t const& a_rFrom)
    {
        // @@0 remove EMP_ASSERT(this != &a_rFrom); //@@0 false
        if (this == &a_rFrom) //@@0 self
        { return *this; }

        deallocate();

        if (a_rFrom.is_const())
        { m_Const = a_rFrom.m_Const; }
        else if (a_rFrom.m_Bulk.is_not_empty())
        {
            a_rFrom.retain();

            m_Bulk = a_rFrom.m_Bulk;
            m_Const = bulk_t<T const>();
            m_pstReferenceCount = a_rFrom.m_pstReferenceCount;
        }
        return *this;
    }

public:
    EMP_INLINE void retain(void) const
    {
        if (m_Bulk.is_not_empty())
        {
            EMP_ASSERT(is_const() == false);
            if (m_pstReferenceCount == nullptr)
            {
                m_pstReferenceCount = EMP_ALLOCATE(size_t);
                *m_pstReferenceCount = 1;
            }
            (*m_pstReferenceCount)++;
        }
    }

public:
    EMP_INLINE EMP_RETURN bool is_const(void) const
    { return m_Const.is_empty() == false; }

    EMP_INLINE EMP_RETURN bool is_owner(void) const
    {
        //EMP_ASSERT(m_pstReferenceCount == nullptr || (*m_pstReferenceCount > 0));
        return  m_Const.is_empty() && (m_pstReferenceCount == nullptr || *m_pstReferenceCount == 1);
    }

public:
    // >> bulk
    EMP_INLINE void set(c_size a_stIndex, T const& a_tValue)
    {
        c_bool bNotConst = m_Const.is_empty();
        EMP_ASSERT(bNotConst);
        if (bNotConst)
        { *m_Bulk.ptr(a_stIndex) = a_tValue; }
    }
    
    EMP_INLINE EMP_RETURN T const* ptr(c_size a_stIndex) const
    { return m_Const.is_not_empty() ? m_Const.ptr(a_stIndex) : m_Bulk.ptr(a_stIndex); }

    EMP_INLINE EMP_RETURN T* ptr(c_size a_stIndex)
    {
        EMP_ASSERT(m_Const.is_empty());
        return m_Const.is_not_empty() ? nullptr : m_Bulk.ptr(a_stIndex);
    }

    EMP_INLINE EMP_RETURN T const* first_const_ptr(void) const
    { return m_Const.is_not_empty() ? m_Const.first_ptr() : m_Bulk.first_ptr(); }

    EMP_INLINE EMP_RETURN T* first_ptr(void)
    {
        EMP_ASSERT(m_Const.is_empty());
        return m_Const.is_not_empty() ? nullptr : m_Bulk.first_ptr();
    }

    EMP_INLINE EMP_RETURN T const* last_ptr(void) const
    { return m_Const.is_not_empty() ? m_Const.last_ptr() : m_Bulk.last_ptr(); }

    EMP_INLINE EMP_RETURN T* last_ptr(void)
    {
        EMP_ASSERT(m_Const.is_empty());
        return m_Const.is_not_empty() ? nullptr : m_Bulk.last_ptr();
    }

    EMP_INLINE EMP_RETURN size_t count(void) const { return m_Const.is_not_empty() ? m_Const.count() : m_Bulk.count(); }
    EMP_INLINE EMP_RETURN size_t  size(void) const { return m_Const.is_not_empty() ? m_Const .size() : m_Bulk.count(); }
    EMP_INLINE void reserve(c_size a_stUsed) { m_Const.is ? m_Const.reserve(a_stUsed) : m_Bulk.reserve(a_stUsed); }

    EMP_INLINE void clear(void)
    {
        m_Bulk.clear();
        m_Const.clear();
    }

    EMP_INLINE void trim_left(c_size a_stCount)
    {
        if (m_Const.is_not_empty())
        { m_Const.trim_left(a_stCount); }
        else
        { m_Bulk.trim_left(a_stCount); }
    }

    EMP_INLINE void trim_right(c_size a_stCount)
    {
        if (m_Const.is_not_empty())
        { m_Const.trim_right(a_stCount); }
        else
        { m_Bulk.trim_right(a_stCount); }
    }

    template <typename U, ENABLE_IF_CONST(U)>
    EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset = 0) const
    { return m_Const.is_not_empty() ? m_Const.template bulk<U>(a_stOffset) : m_Bulk.template bulk<U>(a_stOffset); }

    template <typename U, ENABLE_IF_NOT_CONST(U)>
    EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset = 0) const
    { return m_Bulk.template bulk<U>(a_stOffset); }

    template <typename U, ENABLE_IF_NOT_CONST(U)>
    EMP_INLINE EMP_RETURN bulk_t<U> bulk(c_size a_stOffset, c_size a_stCount) const
    { return m_Bulk.template bulk<U>(a_stOffset, a_stCount); }

    template <typename U, ENABLE_IF_NOT_CONST(U)>
    EMP_INLINE EMP_RETURN bulk_t<U> retain_bulk(size_t*& a_rpstReferenceCount, c_size a_stOffset = 0) const
    {
        EMP_ASSERT(is_const() == false);
        retain();
        a_rpstReferenceCount = m_pstReferenceCount;
        return bulk<U>(a_stOffset);
    }

    EMP_INLINE EMP_RETURN bool share(T* const a_pFrom, c_size a_stSize, size_t* a_pstReferenceCount)
    {
        if (a_pstReferenceCount != m_pstReferenceCount || a_pFrom != m_Bulk.first_ptr())
        {
        deallocate();
        }
        m_Const = bulk_t<T const>();
        m_Bulk = bulk_t<T>(a_pFrom, a_stSize, true);
        m_pstReferenceCount = a_pstReferenceCount;
        return true;
    }

    EMP_INLINE EMP_RETURN bool is_valid_index(c_size a_stIndex) const
    { return m_Const.is_not_empty() ? m_Const.is_valid_index(a_stIndex) : m_Bulk.is_valid_index(a_stIndex); }
    // << bulk

public:
    EMP_INLINE void deallocate(void)
    {
        if (m_pstReferenceCount == nullptr)
        {
            if (m_Bulk.is_not_empty())
            { emp::mem::bulk_deallocate(m_Bulk); }
        }
        else
        {
            EMP_ASSERT((*m_pstReferenceCount) > 0);
            (*m_pstReferenceCount)--;

            if (*m_pstReferenceCount == 0)
            {
                EMP_DEALLOCATE(m_pstReferenceCount);
                if (m_Bulk.is_not_empty())
                { emp::mem::bulk_deallocate(m_Bulk); }
            }
            else
            { m_Bulk = bulk_t<T>(); }
            m_pstReferenceCount = nullptr;
        }

        EMP_ASSERT(m_pstReferenceCount == nullptr || *m_pstReferenceCount > 0);
    }

private:
    EMP_INLINE void get_source(bulk_t<T const>& a_rSource)
    {
        if (m_Const.is_not_empty())
        {
            EMP_ASSERT(m_Bulk == bulk_t<T>());
            a_rSource = m_Const;
            m_Const = bulk_t<T const>();
        }
        else
        {
            if (count() > 0)
            { a_rSource = m_Bulk.template bulk<T const>(0); }
            deallocate();
        }
    }

public:
    EMP_INLINE void grow(c_size a_stSize, c_size a_stUsed)
    {
        if (is_owner())
        {
            reallocable_grow(m_Bulk, a_stSize, a_stUsed);
            return;
        }

        c_size stNewSize = reallocable_size(m_Bulk, a_stSize);
        c_size stCount = count();

        bulk_t<T const> oSource;
        get_source(oSource);

        emp::mem::bulk_allocate(m_Bulk, stNewSize, a_stUsed);
        if (stCount > 0)
        { emp::mem::bulk_copy_from_end(oSource, m_Bulk); }
    }

    //constexpr EMP_INLINE void const_grow(c_size a_stSize, c_size a_stUsed)
    //{

    EMP_INLINE void remove(c_size a_stIndex, c_size a_stCount)
    {
        if (is_owner())
        {
            reallocable_remove(m_Bulk, a_stIndex, a_stCount);
            return;
        }

        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(is_valid_index(a_stIndex));
        EMP_ASSERT(is_valid_index(a_stIndex + a_stCount - 1));

        if (a_stIndex == 0)
        {
            trim_left(a_stCount);
            return;
        }

        c_size stEnd = a_stIndex + a_stCount;
        c_size stUsed = count();
        EMP_ASSERT(stEnd <= stUsed);
        if (stEnd == stUsed)
        {
            m_Bulk.trim_right(a_stCount);
            return;
        }

        EMP_ASSERT(a_stCount <= stUsed);
        c_size stNewCount = stUsed - a_stCount;

        if (stNewCount > 0)
        {
            bulk_t<T const> oSource;
            get_source(oSource);

            emp::mem::bulk_allocate(m_Bulk, stNewCount, stNewCount);
            if (a_stIndex > 0)
            { bulk_copy_from_start(oSource, m_Bulk, a_stIndex); }

            if (stEnd < stUsed)
            {
                bulk_copy_from_start(
                    oSource.template bulk<T const>(stEnd),
                    m_Bulk.template bulk<T>(a_stIndex), stUsed - stEnd);
            }
        }
    }

    EMP_INLINE void insert(c_size a_stIndex, c_size a_stCount)
    {
        if (is_owner())
        {
            reallocable_insert(m_Bulk, a_stIndex, a_stCount);
            return;
        }

        EMP_ASSERT(a_stCount > 0);

        bulk_t<T const> oSource;
        get_source(oSource);

        if (oSource.is_not_empty())
        {
            c_size stUsed = oSource.count();
            if (stUsed > 0)
            {
                EMP_ASSERT(oSource.is_valid_index(a_stIndex));

                c_size stNewUsed = stUsed + a_stCount;

                c_size stNewSize = reallocable_size(oSource, stNewUsed);
                emp::mem::bulk_allocate(m_Bulk, stNewSize, stNewUsed);

                if (a_stIndex > 0)
                { emp::mem::bulk_copy_from_end(oSource, m_Bulk, a_stIndex); }

                if (a_stIndex < stUsed)
                { emp::mem::bulk_copy_from_end(oSource.template bulk<T const>(a_stIndex), m_Bulk.template bulk<T>(a_stIndex + a_stCount)); }
            }
            else
            {
                EMP_ASSERT(a_stIndex == 0);
                emp::mem::bulk_allocate(m_Bulk, a_stCount, a_stCount);
            }
        }
        else
        {
            EMP_ASSERT(a_stIndex == 0);
            emp::mem::bulk_allocate(m_Bulk, a_stCount, a_stCount);
        }
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
