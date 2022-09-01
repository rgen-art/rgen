//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_LIST_H
#define EMP_CNT_LIST_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_allocator.h"

#include "emp_tt_add_const.h"
#include "emp_tt_if_else.h"
#include "emp_tt_is_const.h"
#include "emp_tt_param.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, list)
public:
    typedef T type;
public:
    EMP_NOCOPY(class, element)
    private:
        friend class list;
    public:
        element(list* a_pList):
        m_Value(),
        m_pPrevious(nullptr),
        m_pNext(nullptr),
        m_pList(a_pList)
        { }

    public:
        EMP_RETURN T const& ref(void) const
        { return m_Value; }

        EMP_RETURN T& ref(void)
        { return m_Value; }

        EMP_RETURN T const* ptr(void) const
        { return &ref(); }

        EMP_RETURN T* ptr(void)
        { return &ref(); }

    public:
        EMP_RETURN element const* previous(void) const
        { return m_pPrevious; }

        EMP_RETURN element* previous(void)
        { return m_pPrevious; }

        EMP_RETURN element const* next(void) const
        { return m_pNext; }

        EMP_RETURN element* next(void)
        { return m_pNext; }

        EMP_RETURN list const* parent(void) const
        { return m_pList; }

    private:
        void set_previous(element* a_pElement)
        {
            EMP_ASSERT(
                a_pElement == nullptr || a_pElement->parent() == parent());
            m_pPrevious = a_pElement;
        }

        void set_next(element* a_pElement)
        {
            EMP_ASSERT(
                a_pElement == nullptr || a_pElement->parent() == parent());
            m_pNext = a_pElement;
        }

        EMP_RETURN list* parent(void)
        { return m_pList; }

    private:
        T m_Value;

        element* m_pPrevious;
        element* m_pNext;

        list* m_pList;
    };

private:
    template <typename U>
    EMP_(class, iterator_impl)
    private:
        static bool is_const, emp::tt::is_const<U>::value;
        typedef emp::tt::if_else
        <
            is_const,
            typename emp::tt::add_const<list>::type,
            list
        > tlist;

        typedef emp::tt::if_else
        <
            is_const,
            typename emp::tt::add_const<typename tlist::element>::type,
            element
        > telement;

    private:
        friend class list;
    public:
        iterator_impl(iterator_impl const& a_rFrom):
        m_pIndex(a_rFrom.m_pIndex),
        m_rList(a_rFrom.m_rList)
        { }

    private:
        iterator_impl(telement* a_pIndex, tlist& a_rList):
        m_pIndex(a_pIndex),
        m_rList(a_rList)
        { }

    public:
        iterator_impl& operator=(iterator_impl const& a_rFrom)
        {
            m_pIndex = a_rFrom.m_pIndex;
            EMP_ASSERT(operator==(a_rFrom));
            return *this;
        }

    public:
        EMP_RETURN U& ref(void) const
        {
            EMP_ASSERT(m_pIndex);
            EMP_ASSERT(m_pIndex->parent() == &m_rList);
            return m_pIndex->ref();
        }

        EMP_RETURN U* ptr(void) const
        {
            EMP_ASSERT(m_pIndex);
            EMP_ASSERT(m_pIndex->parent() == &m_rList);
            return m_pIndex->ptr();
        }

    public:
        EMP_RETURN bool operator==(iterator_impl const& a_rFrom) const
        {
            EMP_ASSERT(&m_rList == &a_rFrom.m_rList);
            return m_pIndex == a_rFrom.m_pIndex;
        }

        EMP_RETURN bool operator!=(iterator_impl const& a_rFrom) const
        { return operator==(a_rFrom) == false; }

        iterator_impl& operator++(void)
        {
            EMP_ASSERT(m_pIndex);
            EMP_ASSERT(m_pIndex->parent() == &m_rList);
            m_pIndex = m_pIndex->next();
            EMP_ASSERT(m_pIndex == nullptr || m_pIndex->parent() == &m_rList);
            //@@0 count validation
            //++m_stCount;
            //EMP_ASSERT(m_pIndex || m_stCount == m_rList.count);
            //EMP_ASSERT(m_stCount <= m_rList.count());
            return *this;
        }

    private:
        telement* m_pIndex;
        tlist& m_rList;
        //size_t m_stCount;
    };

public:
    typedef iterator_impl<T const> const_iterator;
    typedef iterator_impl<T> iterator;


public:
    list(void):
    m_stCount(0),
    m_pHead(nullptr),
    m_pTail(nullptr)
    { }

    ~list(void)
    { clear(); }

    void clear(void)
    {
        while(count())
            { remove(tail()); }
        EMP_ASSERT(tail() == nullptr);
        EMP_ASSERT(head() == nullptr);
    }

    EMP_RETURN element* push_head(void)
    {
        element* const pHead = head();
        element* const pElement = push(m_pTail, m_pHead, this, m_stCount);

        if (pHead)
            { pHead->set_previous(pElement); }
        pElement->set_next(pHead);

        return pElement;
    }

    EMP_RETURN element* push_tail(void)
    {
        element* const pTail = tail();
        element* const pElement = push(m_pHead, m_pTail, this, m_stCount);

        if (pTail)
            { pTail->set_next(pElement); }
        pElement->set_previous(pTail);

        return pElement;
    }

    EMP_RETURN element* add_head(typename emp::tt::param<T>::type a_Value)
    {
        element* pElement = push_head();
        pElement->m_Value = a_Value;
        return pElement;
    }

    EMP_RETURN element* add_tail(typename emp::tt::param<T>::type a_Value)
    {
        element* pElement = push_tail();
        pElement->m_Value = a_Value;
        return pElement;
    }

    void remove(element* a_pElement)
    {
        EMP_ASSERT(a_pElement);
        EMP_ASSERT(a_pElement->parent() == this);
        EMP_ASSERT(m_stCount);

        element* pPrevious = a_pElement->previous();
        element* pNext = a_pElement->next();

        if (pPrevious)
            { pPrevious->set_next(pNext); }

        if (pNext)
            { pNext->set_previous(pPrevious); }

        if (m_pTail == a_pElement)
            { m_pTail = pPrevious; }

        if (m_pHead == a_pElement)
            { m_pHead = pNext; }

        EMP_DEALLOCATE(a_pElement);
        --m_stCount;
    }

    EMP_RETURN element const* head(void) const
    { return m_pHead; }

    EMP_RETURN element* head(void)
    { return m_pHead; }

    EMP_RETURN element const* tail(void) const
    { return m_pTail; }

    EMP_RETURN element* tail(void)
    { return m_pTail; }

    EMP_RETURN const_iterator const begin(void) const
    { return const_iterator(m_pHead, *this); }

    EMP_RETURN iterator const begin(void)
    { return iterator(m_pHead, *this); }

    EMP_RETURN const_iterator const end(void) const
    { return const_iterator(nullptr, *this); }

    EMP_RETURN iterator const end(void)
    { return iterator(nullptr, *this); }

    EMP_RETURN size_t count(void) const
    { return m_stCount; }

private:
    static EMP_RETURN element* push(  element*& a_rpLeft, element*& a_rpRight,
                                    list* a_pParent,
                                    size_t& a_rstCount)
    {
        element* const pElement = EMP_ALLOCATE(element, a_pParent);
        EMP_ASSERT(pElement);

        if (a_rpLeft == nullptr)
        { a_rpLeft = pElement; }

        a_rpRight = pElement;

        ++a_rstCount;

        return pElement;
    }

private:
    size_t m_stCount;
    element* m_pHead;
    element* m_pTail;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
