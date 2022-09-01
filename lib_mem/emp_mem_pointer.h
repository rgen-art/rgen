//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_POINTER_H
#define EMP_MEM_POINTER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(struct, true_c)
public:
    EMP_INLINE true_c(void) {}
    EMP_INLINE virtual ~true_c(void) {}

    template <typename T, typename ...P> EMP_INLINE EMP_RETURN T* initialize(P const&...)
    { return nullptr; }
    template <typename T> EMP_INLINE EMP_RETURN bool operator()(T&) const { return true; }
    template <typename T> EMP_INLINE EMP_RETURN bool operator()(T*&) const { return true; }
    template <typename T> static EMP_INLINE EMP_RETURN bool deinitialize(T&) { return true; }
    template <typename T> static EMP_INLINE EMP_RETURN bool deinitialize(T*&) { return true; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Base = true_c>
EMP_NOCOPY_BASE(struct, safe_t, private, t_Base)
public:
    using tbase1::initialize;

public:
    EMP_INLINE safe_t(void) {}

    template <typename T>
    EMP_INLINE EMP_RETURN bool operator()(T*& a_rpObject) const
    {
        if (a_rpObject)
        { return tbase1::operator()(a_rpObject); }
        return true;
    }

    template <typename T>
    static EMP_INLINE EMP_RETURN bool deinitialize(T*& a_rpObject)
    {
        if (a_rpObject)
        { return tbase1::deinitialize(a_rpObject); }
        return true;
    }
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Base = true_c>
EMP_NOCOPY_BASE(struct, destroy_t, private, t_Base)
public:
    using tbase1::initialize;

public:
    EMP_INLINE destroy_t(void)
    { }

    template <typename T>
    EMP_INLINE EMP_RETURN bool operator()(T& a_rObject) const
    {
        a_rObject.destroy();
        return tbase1::operator()(a_rObject);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN bool operator()(T*& a_rpObject) const
    {
        EMP_ASSERT(a_rpObject != nullptr);
        a_rpObject->destroy();
        return tbase1::operator()(a_rpObject);
    }

    template <typename T>
    static EMP_INLINE EMP_RETURN bool deinitialize(T& a_rObject)
    {
        a_rObject.destroy();
        return tbase1::deinitialize(a_rObject);
    }

    template <typename T>
    static EMP_INLINE EMP_RETURN bool deinitialize(T*& a_rpObject)
    {
        EMP_ASSERT(a_rpObject != nullptr);
        a_rpObject->destroy();
        return tbase1::deinitialize(a_rpObject);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Base = true_c>
EMP_NOCOPY_BASE(struct, delete_t, private, t_Base)
public:
    EMP_INLINE delete_t(void)
    { }

    template <typename T, typename... P>
    EMP_INLINE EMP_RETURN T* initialize(P const&... a_rArgs)
    {
        T* pObject = EMP_ALLOCATE(T, a_rArgs...);
        EMP_ASSERT(pObject != nullptr);
        return pObject;
    }

    template <typename T>
    EMP_INLINE EMP_RETURN bool operator()(T*& a_rpObject) const
    {
        EMP_ASSERT(a_rpObject != nullptr);
        EMP_DEALLOCATE(a_rpObject);
        return tbase1::operator()(a_rpObject);
    }

    template <typename T>
    static EMP_INLINE EMP_RETURN bool deinitialize(T*& a_rpObject)
    {
        EMP_ASSERT(a_rpObject != nullptr);
        EMP_DEALLOCATE(a_rpObject);
        return tbase1::deinitialize(a_rpObject);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Base = true_c>
EMP_NOCOPY_BASE(struct, reset_t, private, t_Base)
public:
    using tbase1::initialize;

public:
    EMP_INLINE reset_t(void) {}

    template <typename T>
    EMP_INLINE EMP_RETURN bool operator()(T*& a_rpObject) const
    {
        a_rpObject = nullptr;
        return tbase1::operator()(a_rpObject);
    }

    template <typename T>
    static EMP_INLINE EMP_RETURN bool deinitialize(T*& a_rpObject)
    {
        a_rpObject = nullptr;
        return tbase1::deinitialize(a_rpObject);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef delete_t<reset_t<> > delete_reset_t;
typedef safe_t<delete_reset_t> safe_delete_reset_t;
typedef destroy_t<delete_reset_t> destroy_delete_reset_t;
typedef safe_t<destroy_delete_reset_t> safe_destroy_delete_reset_t;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <class T>
EMP_NOINSTANCE(class, tnull)
public:
    static void apply(T& a_tValue)
    { }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <class T, class t_tBase>
EMP_NOINSTANCE_CLASS_BASE(tdestroy, public, t_tBase)
public:
    static void apply(T& a_tValue)
    {
        t_tBase::apply(a_tValue);
        a_tValue.destroy();
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <class T, class t_tBase>
EMP_NOINSTANCE_CLASS_BASE((tdestroy<T*, t_tBase>), public, t_tBase)
public:
    static void apply(T* a_tValue)
    { a_tValue->destroy(); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <class T, class t_tBase>
EMP_NOINSTANCE(class, tdelete)
public:
    static void apply(T* a_tValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <class T, class t_tBase>
EMP_NOINSTANCE(class, (tdelete<T*, t_tBase>))
public:
    static void apply(T* a_tValue)
    { EMP_DESALLOCATE(a_tValue); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <class T, class t_tBase>
EMP_NOINSTANCE_CLASS_BASE(tdestroy_delete, public, tdestroy<T, tdelete<T, t_tBase>>)
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename t_Finalize = emp::mem::safe_t<>>
EMP_(struct, holder_t)
public:
    EMP_INLINE holder_t(void):
        m_pPointer(t_Finalize().template initialize<T>())
    { }

    explicit EMP_INLINE holder_t(T* a_pFrom):
        m_pPointer(a_pFrom)
    { }

    EMP_INLINE holder_t(holder_t const& a_rFrom):
        m_pPointer(a_rFrom.m_pPointer)
    { }

    virtual ~holder_t(void)
    { EMP_ASSERT_BOOL_CALL(t_Finalize::template deinitialize(m_pPointer)); }

    EMP_INLINE holder_t& operator=(T* const a_pFrom)
    {
        m_pPointer = a_pFrom;
        return *this;
    }

    EMP_INLINE holder_t& operator=(holder_t const& a_rFrom)
    {
        m_pPointer = a_rFrom.m_pPointer;
        return *this;
    }

    EMP_INLINE EMP_RETURN bool operator==(T const* const a_pTo)  const { return m_pPointer == a_pTo; }
    EMP_INLINE EMP_RETURN bool operator!=(T* const a_pTo)              { return operator==(a_pTo) == false; }
    EMP_INLINE EMP_RETURN bool operator!=(T const* const a_pTo)  const { return operator==(a_pTo) == false; }

    EMP_INLINE EMP_RETURN bool operator==(holder_t const& a_pTo) const { return operator==(a_pTo.m_pPointer); }
    EMP_INLINE EMP_RETURN bool operator!=(holder_t const& a_pTo) const { return operator==(a_pTo) == false; }

public:
    EMP_INLINE operator T* (void) const { return operator->(); }
    EMP_INLINE operator T* (void)       { return operator->(); }

public:

    EMP_INLINE T* operator->(void) const { EMP_ASSERT(m_pPointer != nullptr); return m_pPointer; }
    EMP_INLINE T* operator->(void)       { EMP_ASSERT(m_pPointer != nullptr); return m_pPointer; }

public:
    EMP_INLINE T& operator*(void) const { return *(operator->()); }
    EMP_INLINE T& operator*(void)       { return *(operator->()); }

protected:
    T* m_pPointer;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_LOUSY
#define EMP_HOLDER(x_Type) emp::mem::holder_t<x_Type>
#elif defined EMP_XX_DEBUG
#define EMP_HOLDER(x_Type) emp::mem::holder_t<x_Type>
#elif defined EMP_XX_RELEASE
#define EMP_HOLDER(x_Type) emp::mem::holder_t<x_Type>
#elif defined EMP_XX_RETAIL
#define EMP_HOLDER(x_Type) x_Type*
#elif defined EMP_XX_FINAL
#define EMP_HOLDER(x_Type) x_Type*
#else
#error unknown build
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename t_Finalize = emp::mem::safe_destroy_delete_reset_t>
EMP_NOCOPY(struct, pointer_t)
public:
    /*using tbase1::operator->;
    using tbase1::operator*;
    using tbase1::operator==;
    using tbase1::operator T*;

public:*/
    //EMP_INLINE pointer_t(void) : m_pPointer(t_Finalize().template initializ) { }
    template <typename ...P> EMP_INLINE pointer_t(P const&... a_pArgs) : m_pPointer(t_Finalize().template initialize<T>(a_pArgs...)) { }

public:
    /*EMP_INLINE holder_t(void) :
        m_pPointer(t_Finalize().template initialize<T>())
    { }

    explicit EMP_INLINE holder_t(T* a_pFrom) :
        m_pPointer(a_pFrom)
    { }

    EMP_INLINE holder_t(holder_t const& a_rFrom) :
        m_pPointer(a_rFrom.m_pPointer)
    { }*/

    ~pointer_t(void)
    {
        EMP_ASSERT_BOOL_CALL(t_Finalize::template deinitialize(m_pPointer));
    }

    
    /*EMP_INLINE holder_t& operator=(T* const a_pFrom)
    {
        m_pPointer = a_pFrom;
        return *this;
    }

    EMP_INLINE holder_t& operator=(holder_t const& a_rFrom)
    {
        m_pPointer = a_rFrom.m_pPointer;
        return *this;
    }*/

    EMP_INLINE EMP_RETURN bool operator==(T const* const a_pTo)  const { return m_pPointer == a_pTo; }
    EMP_INLINE EMP_RETURN bool operator!=(T* const a_pTo) { return operator==(a_pTo) == false; }
    EMP_INLINE EMP_RETURN bool operator!=(T const* const a_pTo)  const { return operator==(a_pTo) == false; }

    EMP_INLINE EMP_RETURN bool operator==(pointer_t const& a_pTo) const { return operator==(a_pTo.m_pPointer); }
    EMP_INLINE EMP_RETURN bool operator!=(pointer_t const& a_pTo) const { return operator==(a_pTo) == false; }

public:
    EMP_INLINE operator T* (void) const { return operator->(); }
    EMP_INLINE operator T* (void) { return operator->(); }

public:

    EMP_INLINE T* operator->(void) const { EMP_ASSERT(m_pPointer != nullptr); return m_pPointer; }
    EMP_INLINE T* operator->(void) { EMP_ASSERT(m_pPointer != nullptr); return m_pPointer; }

public:
    EMP_INLINE T& operator*(void) const { return *(operator->()); }
    EMP_INLINE T& operator*(void) { return *(operator->()); }

protected:
    T* m_pPointer;

};

template <typename T> using obj_pointer = pointer_t<T, emp::mem::safe_destroy_delete_reset_t>;
template <typename T> using raw_pointer = pointer_t<T, emp::mem::safe_delete_reset_t>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_POINTER(...) emp::mem::raw_pointer<EMP_PP_TRY_SOLVE(__VA_ARGS__)>
#define EMP_OBJECT(x_Type) emp::mem::pointer_t<x_Type >

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
