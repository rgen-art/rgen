//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_AST_ASSERTER_H
#define EMP_AST_ASSERTER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_comma.h"
#include "emp_pp_string.h"

#include "emp_tt_silent.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TRACE()\
    {\
        EMP_XX_FILE,\
        EMP_XX_LINE,\
        EMP_PP_STRING(EMP_XX_LINE),\
        EMP_XX_FUNCTION,\
        EMP_XX_SIGNATURE }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TRACE_COMMA EMP_PP_COMMA()
#define EMP_TRACE_ARG a_rTrace
#define EMP_TRACE_PARAM emp::ast::trace_s const& a_rTrace
#define EMP_TRACE_HERE EMP_TRACE()
namespace emp { namespace ast { struct trace_s; } }

#define EMP_TRACE_ARG_COMMA EMP_TRACE_ARG EMP_TRACE_COMMA
#define EMP_TRACE_PARAM_COMMA EMP_TRACE_PARAM EMP_TRACE_COMMA
#define EMP_TRACE_PARAM_UNUSED emp::ast::trace_c const& EMP_SILENT(a_rTrace)
#define EMP_TRACE_PARAM_UNUSED_COMMA EMP_TRACE_PARAM_UNUSED EMP_TRACE_COMMA
#define EMP_TRACE_HERE_COMMA EMP_TRACE_HERE EMP_TRACE_COMMA

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ast {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(trace_s)
public:
    c_char* m_szFile = nullptr;
    uint m_uiLine = 0;
    c_char* m_szLine = nullptr;
    c_char* m_szFunction = nullptr;
    c_char* m_szSignature = nullptr;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct assert_c
{
public:
    bool m_bCondition = false;
    cpc_char m_szCondition = nullptr;
    trace_s  m_Trace;

    assert_c(void) = delete;
    assert_c(assert_c const&) = delete;
    assert_c& operator=(assert_c const&) = delete;
    assert_c& operator=(assert_c&&) = delete;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void assert_(assert_c const& a_rAssert);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output_pchar(cpc_char a_pValue);
void output_line(cpc_char a_szName, cpc_char a_szValue, c_bool a_bVerbose);
void output_header(cpc_char a_szName, cpc_char a_szAlias, c_bool a_bVerbose);
void output_trace(trace_s const& a_rTrace, c_bool a_bVerbose);
void output_assert(assert_c const& a_rAssert, c_bool a_bVerbose);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_ASSERT

#define EMP_ASSERT(x_Condition) do { emp::ast::assert_({ x_Condition, EMP_PP_STRING(x_Condition), EMP_TRACE_HERE}); } while(0)
#define EMP_ASSERT_UNREACHABLE() EMP_ASSERT(false);
#define EMP_ASSERT_NOT_IMPLEMENTED() EMP_ASSERT(false);
#define EMP_ASSERT_BOOL_CALL(x_Call) EMP_ASSERT(x_Call)

#else

#define EMP_ASSERT(x_Condition) EMP_XX_NOOP()
#define EMP_ASSERT_UNREACHABLE() EMP_XX_NOOP()
#define EMP_ASSERT_NOT_IMPLEMENTED() EMP_ASSERT(false);// static_assert(false, "not implemented")
#define EMP_ASSERT_BOOL_CALL(x_Call) emp::tt::silent(x_Call)

#endif

#define EMP_ASSERT_NOT(x_Condition) EMP_ASSERT((x_Condition))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

