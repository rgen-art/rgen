//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_log_logger.h"

//#include "emp_app_application.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace log {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
logger_c* logger_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
logger_c::logger_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
logger_c::~logger_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void logger_c::destroy(void)
{
    m_sOutput = astring();
    m_sMessage = astring();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void logger_c::output(c_log_e a_eLog, emp::ast::trace_s const& a_rTrace, c_astring& a_sMessage)
{
    if (a_eLog < m_eLogLevel ||
        a_eLog == m_eDisableLog)
    { return; }

    m_sOutput.clear();

    switch (m_eVerboseLevel)
    {
        case verbose_e::none:
        { break; }
        case verbose_e::Type:
        {
            // @@0 right align file & line
            astring sPath;
            astring sFilename;
            emp::str::extract_PF(astring::ref(a_rTrace.m_szFile), sPath, sFilename);
            emp::tt::silent(
                STR_FORMATA(m_sOutput, "// #0 | #1:#2 | ", enum_to_astring(a_eLog).get_first_char(),
                    sFilename, astring::ref(a_rTrace.m_szLine)));
            break;
        }
        case verbose_e::File:
        {
            // @@0 right align file & line
            emp::tt::silent(
                STR_FORMATA(m_sOutput, "// #0 | #1:#2 | ", enum_to_astring(a_eLog).get_first_char(),
                    astring::ref(a_rTrace.m_szFile), astring::ref(a_rTrace.m_szLine)));
            break;
        }
        case verbose_e::All:
        {
            emp::tt::silent(STR_FORMATA(m_sOutput,
                EMP_XSZ_S0 EMP_XSZ_NEWLINE
                "// #0" EMP_XSZ_NEWLINE
                EMP_XSZ_S0 EMP_XSZ_NEWLINE
                "// File : #1" EMP_XSZ_NEWLINE
                "// Line : #2" EMP_XSZ_NEWLINE
                "// Func : #3" EMP_XSZ_NEWLINE
                "// Sign : #4" EMP_XSZ_NEWLINE,
                    enum_to_astring(a_eLog),
                    astring::ref(a_rTrace.m_szFile),
                    astring::ref(a_rTrace.m_szLine),
                    astring::ref(a_rTrace.m_szFunction),
                    astring::ref(a_rTrace.m_szSignature)));
            break;
        }
        default:
        { EMP_ASSERT_UNREACHABLE(); }
    };

    m_sOutput.append_string(a_sMessage);
    m_sOutput.append_char(emp::str::newline<char>::value);

    //EMP_APPLICATION.output(m_sOutput.cstr());

    ast::output_pchar(reinterpret_cast<char const*>(m_sOutput.cstr()));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
