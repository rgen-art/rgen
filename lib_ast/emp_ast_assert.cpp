//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE_STL
#include "emp_ast_assert.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_stdio.h"
#include "emp_hh_stdlib.h"


#if defined EMP_XX_OS_WIN
#include "emp_hh_windows.h"
#endif

#include "emp_xx_string.h"
EMP_PRAGMA_POP_IGNORE_STL
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ast {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output_pchar(cpc_char a_pValue)
{

    EMP_ASSERT(a_pValue != nullptr);
#ifdef EMP_XX_COMPILER_MSC
    OutputDebugStringA(a_pValue);
#endif
    fputs(a_pValue, stdout);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output_line(cpc_char a_szName, cpc_char a_szValue, c_bool a_bVerbose)
{
    if (a_bVerbose)
    {
        output_pchar(EMP_XSZ_NEWLINE_S1_SPACE);
        output_pchar(a_szName);
        output_pchar(EMP_XSZ_SPACE_COMMA_SPACE);
    }

    output_pchar(a_szValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output_header(cpc_char a_szName, cpc_char a_szAlias, c_bool a_bVerbose)
{
    if (a_bVerbose)
    {
        output_pchar(EMP_XSZ_S0);
        output_pchar(EMP_XSZ_NEWLINE_S1_SPACE);
        output_pchar(a_szName);
        output_pchar(EMP_XSZ_NEWLINE);
        output_pchar(EMP_XSZ_S0);
    }
    else
    {
        output_pchar(a_szAlias);
        output_pchar(EMP_XSZ_SPACE_COMMA_SPACE);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output_trace(trace_s const& a_rTrace, c_bool a_bVerbose)
{
    if (a_bVerbose)
    {
        output_line(EMP_XSZ_FILE, a_rTrace.m_szFile, a_bVerbose);
        output_line(EMP_XSZ_LINE, a_rTrace.m_szLine, a_bVerbose);
        output_line(EMP_XSZ_FUNCTION, a_rTrace.m_szFunction, a_bVerbose);
        output_line(EMP_XSZ_SIGNATURE, a_rTrace.m_szSignature, a_bVerbose);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output_assert(assert_c const& a_rAssert, c_bool a_bVerbose)
{
    output_trace(a_rAssert.m_Trace, a_bVerbose);
    output_line(EMP_XSZ_CONDITION, a_rAssert.m_szCondition, a_bVerbose);
    output_pchar(EMP_XSZ_NEWLINE);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void assert_(emp::ast::assert_c const& a_rAssert)
{
    if (a_rAssert.m_bCondition == false)
    {
        output_header(EMP_XSZ_ASSERT, EMP_XSZ_UPPER_A, true);
        output_assert(a_rAssert, true);
        EMP_XX_DEBUG_BREAK;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

