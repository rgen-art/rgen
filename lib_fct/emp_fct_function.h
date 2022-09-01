//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_FCT_FUNCTION_H
#define EMP_FCT_FUNCTION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fct {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class function_i;

template <typename R, typename...P>
EMP_(class, function_i<R(*)(P...)>)
public:
    function_i(void) {}
    virtual ~function_i(void) {}
    virtual R operator()(P const&... a_rtParams) const = 0;
    virtual R operator()(P const&... a_rtParams) = 0;
    virtual EMP_RETURN function_i* clone(void) = 0;
    virtual EMP_RETURN bool is_valid(void) const = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@2 remove
template <typename T>
EMP_RETURN T* function_clone(T* a_pFrom)
{
    if (a_pFrom)
    { return a_pFrom->clone(); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class static_handler;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename R, typename... P>
EMP_BASE(class, static_handler<R(*)(P...)>, public, function_i<R(*)(P...)>)
public:
    typedef R(*F)(P...);

private:
    F m_Function;

public:
    static_handler(static_handler const& a_rFrom) :
    m_Function(a_rFrom.m_Function) {}

    static_handler(F const& a_rFunction) :
    m_Function(a_rFunction) {}

    static_handler& operator=(static_handler const& a_rFrom)
    {
        m_Function = a_rFrom.m_Function;
        return *this;
    }

    EMP_RETURN static_handler* clone(void)
    { return EMP_ALLOCATE(static_handler, *this); }

    EMP_RETURN bool is_valid(void) const
    { return m_Function != nullptr; }

    R operator()(P const&... a_rtParams) const { return m_Function(a_rtParams...); }
    R operator()(P const&... a_rtParams)       { return m_Function(a_rtParams...); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename R, typename... P>
EMP_BASE(class, static_handler<function_t<R(*)(P...)>>, public, function_i<R(*)(P...)>)
public:
    typedef function_t<R(*)(P...)> F;

private:
    F m_Function;

public:
    static_handler(static_handler const& a_rFrom) :
    m_Function(a_rFrom.m_Function) {}

    static_handler(F const& a_rFunction) :
    m_Function(a_rFunction) {}

    static_handler& operator=(static_handler const& a_rFrom)
    {
        m_Function = a_rFrom.m_Function;
        return *this;
    }

    EMP_RETURN static_handler* clone(void)
    { return EMP_ALLOCATE(i, *this); }

    EMP_RETURN bool is_valid(void) const
    { return m_Function.is_valid(); }

    R operator()(P const&... a_rtParams) const { return m_Function(a_rtParams...); }
    R operator()(P const&... a_rtParams)       { return m_Function(a_rtParams...); }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class member_handler;

template <typename R, typename O, typename...P>
EMP_BASE(class, member_handler<R(O::*)(P...)>, public, function_i<R(*)(P...)>)
private:
    typedef R(O::*F)(P...);
    typedef R(O::*MF)(P...);

    O* m_pObject;
    MF m_pFunction;

public:
    member_handler(O* const& a_pObject, MF const& a_pFunction) :
        m_pObject(a_pObject), m_pFunction(a_pFunction)
    {}

    member_handler(member_handler const& a_rFrom):
        m_pObject(a_rFrom.m_pObject), m_pFunction(a_rFrom.m_pFunction)
    {}

    member_handler& operator=(member_handler const& a_rFrom)
    {
        m_pObject = a_rFrom.m_pObject;
        m_pFunction = a_rFrom.m_pFunction;
        return *this;
    }

    EMP_RETURN member_handler* clone(void) { return EMP_ALLOCATE(member_handler, *this); }

    EMP_RETURN bool is_valid(void) const { return m_pObject != nullptr && m_pFunction != nullptr; }

    R operator()(P const&... a_rtParams) const { return ((*m_pObject).*m_pFunction)(a_rtParams...); }
    R operator()(P const&... a_rtParams)       { return ((*m_pObject).*m_pFunction)(a_rtParams...); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
class function_t;

template <typename R, typename... P>
EMP_(class, function_t<R(*)(P...)>)
private:
    typedef R(*F)(P...);
    function_i<F>* m_pImpl;

public:
    function_t(void) :
        m_pImpl(nullptr)
    {}

    function_t(function_t const& a_rFrom) :
        m_pImpl(function_clone(a_rFrom.m_pImpl))
    {}

    function_t(function_i<R(*)(P...)>* a_pImpl) :
        m_pImpl(a_pImpl)
    {}

    function_t(F const& a_rFrom):
        m_pImpl(EMP_ALLOCATE(static_handler<F>, a_rFrom)) //-V747
    {}

    template <typename O>
    function_t(O* const a_pObject, R(O::* const& a_pFunction)(P...)) :
        m_pImpl(EMP_ALLOCATE(member_handler<R(O::*)(P...)>, a_pObject, a_pFunction))
    {}

    EMP_INLINE ~function_t(void)
    { if (m_pImpl) { EMP_DEALLOCATE(m_pImpl); } }

    function_t& operator=(function_t const& a_rFrom)
    {
        if (m_pImpl) { EMP_DEALLOCATE(m_pImpl); }
        m_pImpl = function_clone(a_rFrom.m_pImpl);
        return *this;
    }

public:
    EMP_RETURN bool is_valid(void) const
    { return m_pImpl != nullptr && m_pImpl->is_valid(); }

public:
    R operator()(P const&... a_rtParams) const { return (*m_pImpl)(a_rtParams...); }
    R operator()(P const&... a_rtParams)       { return (*m_pImpl)(a_rtParams...); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S>
struct remove_first_t;

template <typename R, typename T0, typename... P>
EMP_NOINSTANCE(struct, remove_first_t<R(*)(T0, P...)>)
    typedef R(*type)(P...);
};

template <typename R, typename T0, typename... P>
EMP_NOINSTANCE(struct, remove_first_t<function_t<R(*)(T0, P...)>>)
    typedef R(*type)(P...);
};

template <typename...P>
using remove_first = typename remove_first_t<P...>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class bind_handler;

template <typename R, typename P0, typename... P>
EMP_BASE(class, bind_handler<function_t<R(*)(P0, P...)>>, public, function_i<R(*)(P...)>)
private:
    typedef R(*F)(P0, P...);
    P0 m_p0;
    function_t<F> m_Function;

public:
    bind_handler(bind_handler const& a_rFrom):
        m_p0(a_rFrom.m_p0), m_Function(a_rFrom.m_Function)
    {}

    bind_handler& operator=(bind_handler const& a_rFrom)
    {
        m_Function = a_rFrom.m_Function;
        m_p0 = a_rFrom.m_p0;
        return *this;
    }

    virtual ~bind_handler(void) {}

    bind_handler(function_t<F> const& a_rFunction, P0 p0) :
        m_p0(p0), m_Function(a_rFunction)
    {}

    R operator()(P const... a_rtParams) const { return m_Function(m_p0, a_rtParams...); }
    R operator()(P const... a_rtParams)       { return m_Function(m_p0, a_rtParams...); }

    EMP_RETURN bind_handler* clone(void) { return EMP_ALLOCATE(bind_handler, *this); }
    EMP_RETURN bool is_valid(void) const { return m_Function.is_valid(); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> struct member_signature_t;

template <typename R, typename C, typename ...P>
EMP_NOINSTANCE(struct, member_signature_t<R(C::*)(P...)>)
    typedef R(*type)(P...);
};

/*template <typename R, typename C, typename ...P>
EMP_NOINSTANCE(struct, member_signature_t<R(C::*)(P...) const>)
    typedef R(*type)(P...);
};*/

template <typename...P>
using member_signature = typename member_signature_t<P...>::type;

// @@3 auto bind/member/cfunction

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename F, typename B0>
EMP_RETURN auto bind(F const& a_rFunction, B0 const& a_Bound)
{ return function_t<remove_first<F>>(EMP_ALLOCATE(bind_handler<F>, a_rFunction, a_Bound)); } //-V747

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename F, typename B0, typename...P>
EMP_RETURN auto const& bind(F const& a_rFunction, B0 const& a_Bound, P... a_ptParams)
{ return bind(bind(a_rFunction, a_Bound), a_ptParams...); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename  F>
EMP_RETURN function_t<F> function(F const& a_rFunction)
{ return function_t<F>(a_rFunction); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename O, typename F>
EMP_RETURN auto function(O* const& a_pObject, F const& a_rFunction)
{ return function_t<member_signature<F>>(a_pObject, a_rFunction); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

