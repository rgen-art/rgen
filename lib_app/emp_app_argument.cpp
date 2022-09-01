//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_app_argument.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_name_delimiter(c_astring& a_sValue);
EMP_INTERN EMP_RETURN bool is_name_delimiter(c_astring& a_sValue)
{
    return  a_sValue.len() > 2 &&
        emp::str::is_sub(a_sValue.get_char(0)) &&
        emp::str::is_sub(a_sValue.get_char(1));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_alias_delimiter(c_astring& a_sValue);
EMP_INTERN EMP_RETURN bool is_alias_delimiter(c_astring& a_sValue)
{
    return  a_sValue.len() > 1 &&
        emp::str::is_sub(a_sValue.get_char(0)) &&
        emp::str::is_not_sub(a_sValue.get_char(1));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Container>
EMP_INTERN EMP_RETURN typename t_Container::type find_by_name(t_Container const& a_vpArguments, c_astring& a_sValue);
template <typename t_Container>
EMP_INTERN EMP_RETURN typename t_Container::type find_by_name(t_Container const& a_vpArguments, c_astring& a_sValue)
{
    c_size stCount = a_vpArguments.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        typename t_Container::type const pArgument = a_vpArguments.val(stIndex);
        EMP_ASSERT(pArgument != nullptr);
        if (emp::str::cmp(pArgument->name(), a_sValue))
        { return pArgument; }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Container>
EMP_INTERN EMP_RETURN typename t_Container::type find_by_alias(t_Container const& a_vpArguments, c_astring& a_sValue);
template <typename t_Container>
EMP_INTERN EMP_RETURN typename t_Container::type find_by_alias(t_Container const& a_vpArguments, c_astring& a_sValue)
{
    if (a_sValue.len() != 1)
    { return nullptr; }

    c_size stCount = a_vpArguments.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        typename t_Container::type const pArgument = a_vpArguments.val(stIndex);
        EMP_ASSERT(pArgument != nullptr);
        if (pArgument->alias() == a_sValue.get_first_char())
        { return pArgument; }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool compare_arguments(emp::cnt::shell<argument_i const* const> const& a_vpArgumentsLeft,
                                           emp::cnt::shell<argument_i const* const> const& a_vpArgumentsRight);
EMP_INTERN EMP_RETURN bool compare_arguments(emp::cnt::shell<argument_i const* const> const& a_vpArgumentsLeft,
                                           emp::cnt::shell<argument_i const* const> const& a_vpArgumentsRight)
{
    c_size stLeftCount = a_vpArgumentsLeft.count();
    c_size stRightCount = a_vpArgumentsRight.count();

    c_bool bSameAddress = &a_vpArgumentsLeft == &a_vpArgumentsRight;

    for (size_t stLeftIndex = 0; stLeftIndex < stLeftCount; ++stLeftIndex)
    {
        EMP_LOG_DEBUG("parsing argument : #0", stLeftIndex);

        emp::app::argument_i const* const pLeft = a_vpArgumentsLeft.val(stLeftIndex);

        size_t stRightIndex = bSameAddress ? (stLeftIndex + 1) : 0;

        for (;stRightIndex < stRightCount; ++stRightIndex)
        {
            EMP_LOG_DEBUG("comparing argument : #0", stRightIndex);

            emp::app::argument_i const* const pRight = a_vpArgumentsRight.val(stRightIndex);

            if (pLeft == pRight)
            {
                EMP_LOG_ERROR("duplicate address");
                return false;
            }

            EMP_LOG_DEBUG("comparing argument names : '#0' : '#1'", pLeft->name(), pRight->name());
            if (emp::str::cmp(pLeft->name(), pRight->name()))
            {
                EMP_LOG_ERROR("duplicate name #0", pLeft->name());
                return false;
            }

            EMP_LOG_DEBUG("comparing argument aliases : '#0' : '#1'", astring(pLeft->alias()), astring(pRight->alias()));
            if (pLeft->alias() == pRight->alias())
            {
                EMP_LOG_ERROR("duplicate alias : #0", astring(pLeft->alias()));
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool error_output_usage(shell<argument_i const* const> const& EMP_SILENT(a_rpArguments), c_astring& EMP_XX_IF_LOG(a_sUsages));
EMP_INTERN EMP_RETURN bool error_output_usage(shell<argument_i const* const> const& EMP_SILENT(a_rpArguments), c_astring& EMP_XX_IF_LOG(a_sUsages))
{
    EMP_LOG_ERROR("invalid usage");
    EMP_LOG_INFO("#0", a_sUsages);
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0
EMP_INTERN EMP_RETURN bool output_name(argument_i const* const a_pArgument);
EMP_INTERN EMP_RETURN bool output_name(argument_i const* const a_pArgument)
{
    EMP_ASSERT(a_pArgument);

    c_astring& sName = a_pArgument->name();
    if (emp::str::is_not_null(sName.get_first_char()))
    { EMP_LOG_INFO("        #0", sName); }
    return true;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0
EMP_INTERN EMP_RETURN bool output_help(shell<argument_i const* const> const& a_vpArguments,
    emp::app::argument_t<astring> const& a_rHelpArgument);
EMP_INTERN EMP_RETURN bool output_help(shell<argument_i const* const> const& a_vpArguments,
    emp::app::argument_t<astring> const& a_rHelpArgument)
{
    EMP_ASSERT(a_rHelpArgument.is_active());

    c_astring& sArgument = a_rHelpArgument.value();

    if (sArgument.len() == 0)
    {
        EMP_LOG_INFO(a_rHelpArgument.description());
        EMP_ASSERT_BOOL_CALL(emp::cnt::foreach(a_vpArguments, output_name));
        return true;
    }
    else
    {
        argument_i const* const pArgument = find_by_name(a_vpArguments, sArgument);
        if (pArgument)
        {
            c_ui32 cAlias = pArgument->alias();
            EMP_ASSERT(emp::str::is_not_null(cAlias));

            EMP_LOG_INFO("-#0, --#1 : #2", emp::str::fmt(cAlias), pArgument->name(), pArgument->description());
            return true;
        }
    }
    return false;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int functor::operator()(emp::mat::expression const& a_rBinary, c_astring& a_sLeft, c_int a_siLeft,
    c_astring& a_sRight, c_int a_siRight, emp::mat::c_operator_e a_eOperator) const
{
    EMP_LOG_DEBUG("left : #0", a_sLeft);
    EMP_LOG_DEBUG("right : #0", a_sRight);

    EMP_RET_X_IF(-2, a_eOperator == emp::mat::operator_e::RD && a_siRight == 1 && a_siLeft != 1,
        "invalid dependency : '#0' : '#1'", a_sLeft, a_sRight);

    EMP_RET_X_IF(-2, a_eOperator == emp::mat::operator_e::XOr && a_siRight == 1 && a_siLeft == 1,
        "invalid exclusive : '#0' : '#1'", a_sLeft, a_sRight);

    return emp::mat::functor::operator()(a_rBinary, a_sLeft, a_siLeft, a_sRight, a_siRight, a_eOperator);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int functor::operator()(emp::mat::expression const& EMP_SILENT(a_rBinary), c_astring& a_sPointer) const
{
    EMP_LOG_DEBUG("looking for argument : '#0'", a_sPointer);

    emp::app::argument_i const* const pArgument = find_by_name(m_rvpArguments, a_sPointer);

    EMP_RET_X_IF(-1, pArgument == nullptr, "invalid argument '#0'", a_sPointer);

    EMP_LOG_DEBUG("argument found : '#0'", a_sPointer);
    return pArgument->is_active();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool argument_i::parse(c_int a_siArgc, cpc_char* const a_aszArgv, emp::cnt::shell<argument_i* const> const& a_vpArguments,
    c_astring& a_sUsage)
{
    EMP_RET_FALSE_IF(a_sUsage.len() == 0  , "empty a_szUsage");
    EMP_RET_FALSE_IF(a_aszArgv == nullptr , "invalid pointer a_aszArgv");
    EMP_RET_FALSE_IF(a_siArgc <= 0        , "invalid argument count #0", a_siArgc);

    emp::cnt::shell<argument_i const* const> vpArguments(a_vpArguments);
    EMP_RET_FALSE_IF_NOT(vpArguments.count() > 0, "no argument specified");
    EMP_RET_FALSE_IF_NOT(a_vpArguments.count() > 0, "unknown error");

    emp::cnt::vector<astring> vsArgs;
    {
        size_t stCount = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_siArgc, stCount), "invalid argument count #0", a_siArgc);
        for (size stIndex = 1; stIndex < stCount; ++stIndex)
        { vsArgs.add(astring::ref(a_aszArgv[stIndex])); }
    }

    EMP_RET_FALSE_IF_NOT(compare_arguments(vpArguments, vpArguments));

    c_size stCount = vsArgs.count();
    static_assert(std::is_unsigned<size_t>::value, "");

    //EMP_RET_FALSE_IF((stCount >= 0) == false, "invalid argument count #0", stCount);

    emp::app::argument_i* pLastArgument = nullptr;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_astring& sValue = vsArgs.ref(stIndex);
        if (emp::app::is_name_delimiter(sValue))
        {
            astring sToFind(sValue);
            sToFind.trim_left(2);

            emp::app::argument_i* const pCurrent = find_by_name(a_vpArguments, sToFind);
            EMP_RET_FALSE_IF(pCurrent == nullptr, "unknown argument '#0'", sToFind);

            pLastArgument = pCurrent;
            pLastArgument->set_active(true);
        }
        else if (emp::app::is_alias_delimiter(sValue))
        {
            astring sToFind(sValue);
            sToFind.trim_left();

            emp::app::argument_i* const pCurrent = find_by_alias(a_vpArguments, sToFind);
            EMP_RET_FALSE_IF(pCurrent == nullptr, "unknown argument '#0'", sToFind);

            pLastArgument = pCurrent;
            pLastArgument->set_active(true);
        }
        else if (pLastArgument != nullptr)
        {
            EMP_RET_FALSE_IF_NOT(pLastArgument->parse(sValue), "parsing failed #0", pLastArgument->name());
            pLastArgument = nullptr;
        }
        else
        { return error_output_usage(vpArguments, a_sUsage); }
    }

    emp::mat::expression const oBinaryExpression;
    c_int siResult = oBinaryExpression.evaluate(a_sUsage,emp::app::functor(emp::cnt::shell<argument_i const* const>(a_vpArguments)));

    if (siResult == 0 || siResult == 1)
    { return true; }

    EMP_RET_FALSE_IF(siResult == -1, "invalid argument specification, contact the author");
    if (siResult == -2)
    { return error_output_usage(vpArguments, a_sUsage); }

    EMP_LOG_ERROR("unknown error");
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool argument_t<T>::parse(c_astring& a_sValue)
{
    // @@0 ascii
    /*if (a_sValue.is_ascii() == false)
    {
        EMP_LOG_ERROR("Libopt does not handle non ascii arguments : #0", a_sValue);
        EMP_ASSERT(false);
        return false;
    }*/

    EMP_LOG_INFO("parsing #0", a_sValue);
    return emp::str::to(a_sValue, m_tValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0
EMP_RETURN bool argument_t<void>::parse(c_astring& a_sValue)
{
    EMP_LOG_ERROR("argument_t<void>::parse : NotImplemented : #0", a_sValue);
    return false;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0
EMP_RETURN bool argument_t<bool>::parse(c_astring& a_sValue)
{
    EMP_LOG_ERROR("argument_t<bool>::parse : NotImplemented : #0", a_sValue);
    return false;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template class argument_t<bool>;
//template class argument_t<void>;
template class argument_t<int>;
template class argument_t<uint>;
template class argument_t<unsigned long int>;
template class argument_t<float>;
template class argument_t<astring>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

