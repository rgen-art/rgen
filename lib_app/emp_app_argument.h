//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_APP_ARGUMENT_H
#define EMP_APP_ARGUMENT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_evaluation.h"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name result_e
#define enum_values\
    enum_value( none    )\
    enum_value( Author  )\
    enum_value( User    )\
    enum_value( Unknown )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class argument_i;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,functor, private, emp::mat::functor)
public:
    functor(shell<argument_i const* const> const& a_rvpArguments): m_rvpArguments(a_rvpArguments) {}

public:
    EMP_RETURN int operator()(emp::mat::expression const& a_rBinary, c_astring& a_sLeft, c_int a_siLeft,
        c_astring& a_sRight, c_int a_siRight, emp::mat::c_operator_e a_eOperator) const;

    EMP_RETURN int operator()(emp::mat::expression const& a_rBinary, c_astring& a_sPointer) const;

private:
    emp::cnt::shell<argument_i const* const> const& m_rvpArguments;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, argument_i)
public:
    argument_i(void) {}
    virtual ~argument_i(void) {}

    virtual void destroy(void) = 0;

    virtual EMP_RETURN char alias(void) const = 0;
    virtual EMP_RETURN c_astring& name(void) const = 0;
    virtual EMP_RETURN c_astring& description(void) const = 0;

    virtual EMP_RETURN bool is_active(void) const = 0;

private:
    virtual void set_active(c_bool a_bValue) = 0;
    virtual EMP_RETURN bool parse(c_astring& a_sValue) = 0;

public:
    static EMP_RETURN bool parse(c_int a_siArgc, cpc_char* const a_aszArgv, emp::cnt::shell<argument_i*const>const& a_vpArguments,
        c_astring& a_sUsage);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,argument_a, public, argument_i)
private:
    bool m_bIsActive;
    char m_cAlias;
    astring m_sName;
    astring m_sDescription;

public:
    argument_a(void) : m_bIsActive(false), m_cAlias(), m_sName(), m_sDescription() {}
    EMP_INLINE ~argument_a(void) {}

    virtual void destroy(void)
    {
        m_sName = astring();
        m_sDescription = astring();
    }

    void init(c_char a_cAlias, c_astring& a_sName, c_astring a_sDestription)
    {
        m_cAlias = a_cAlias;
        m_sName = a_sName;
        m_sDescription = a_sDestription;
    }


    EMP_RETURN char alias(void)            const { return m_cAlias; }
    EMP_RETURN c_astring& name(void)       const { return m_sName; }
    EMP_RETURN c_astring& description(void) const { return m_sDescription; }
    EMP_RETURN bool is_active(void)        const { return m_bIsActive; }

protected:
    virtual void set_active(c_bool a_bValue) { m_bIsActive = a_bValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, argument_t, public, argument_a)
private:
    T m_tDefault;
    T m_tValue;

public:
    argument_t(void):
        m_tDefault(),
        m_tValue()
    {}

    EMP_INLINE ~argument_t(void) {}

    void destroy(void)
    {
        m_tDefault = T();
        m_tValue = T();
        tbase1::destroy();
    }

    void init(c_char a_cAlias, c_astring& a_sName, c_astring& a_sDescription, T const& a_tUndefined, T const& a_tDefault)
    {
        tbase1::init(a_cAlias, a_sName, a_sDescription);
        m_tValue = a_tUndefined;
        m_tDefault = a_tDefault;
    }

    void init(c_char a_cAlias, c_astring& a_sName, c_astring& a_sDescription, T const& a_tDefault)
    { init(a_cAlias, a_sName, a_sDescription, a_tDefault, a_tDefault); }

    void init(c_char a_cAlias, c_astring& a_sName, c_astring& a_sDescription)
    { init(a_cAlias, a_sName, a_sDescription, T(), T()); }

    EMP_RETURN emp::tt::const_return<T> value(void) const { return m_tValue; }


    void set_active(c_bool a_bValue)
    {
        m_tValue = m_tDefault;
        argument_a::set_active(a_bValue);
    }

private:
    EMP_RETURN bool parse(c_astring& a_sValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

