//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_LOG_LOGGER_H
#define EMP_LOG_LOGGER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace log {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name log_e
#define enum_type
#define enum_values\
    enum_value( none        )\
    enum_value( Lousy       )\
    enum_value( Debug       )\
    enum_value( Information )\
    enum_value( Warning     )\
    enum_value( Error       )\
    enum_value( Output      )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name verbose_e
#define enum_type
#define enum_values\
    enum_value( none )\
    enum_value( Type )\
    enum_value( File )\
    enum_value( All  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void output(c_log_e a_eLog, emp::ast::trace_s const& a_rTrace, c_astring& a_sMessage);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, logger_c)
private:
    static logger_c* s_pInstance;
public:
    #define EMP_LOGGER emp::log::logger_c::instance()
    static void set_instance(logger_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN logger_c& instance(void) { return *s_pInstance; }

public:
    log_e m_eLogLevel = log_e::none;
    log_e m_eDisableLog = log_e::none;
    verbose_e m_eVerboseLevel = verbose_e::Type;

private:
    astring m_sOutput{};

public:
    astring m_sMessage{};

public:
    logger_c(void);
    ~logger_c(void);

public:
    void destroy(void);

public:
/*    void disable(c_log_e a_eValue) { m_eDisableLog = a_eValue; }
    EMP_RETURN log_e log_level(void) const { return m_eLogLevel; }
    void set_log_level(c_log_e a_eValue) { m_eLogLevel = a_eValue; }

    EMP_RETURN verbose_e verbose_level(void) const { return m_eVerboseLevel; }
    void set_verbose_level(c_verbose_e a_eValue) { m_eVerboseLevel = a_eValue; }
*/
    void output(c_log_e a_eLog, emp::ast::trace_s const& a_rTrace, c_astring& a_sMessage);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

#define EMP_LOG_MESSAGE_0(x_Type, x_Trace, x_Format, ...)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
    EMP_LOGGER.m_sMessage.clear();\
    EMP_PP_FORWARD(emp::tt::silent(STR_FORMATA(EMP_LOGGER.m_sMessage, x_Format, __VA_ARGS__)));\
    EMP_LOGGER.output(emp::log::log_e::x_Type, x_Trace, EMP_LOGGER.m_sMessage);\
EMP_XX_MULTI_STATEMENT_END()

#define EMP_LOG_MESSAGE_1(x_Type, x_Trace, x_Message)\
EMP_XX_MULTI_STATEMENT_BEGIN()\
    EMP_LOGGER.output(emp::log::log_e::x_Type, x_Trace, AFORMAT(x_Message, 0));\
EMP_XX_MULTI_STATEMENT_END()

#define EMP_LOG_MESSAGE(x_Type, ...)\
    EMP_PP_FORWARD(EMP_PP_CAT(EMP_LOG_MESSAGE_, EMP_PP_EQUAL(1, EMP_VAARGS_COUNT(__VA_ARGS__)))(x_Type, EMP_TRACE_HERE, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(EMP_XX_LOG)
#define EMP_XX_IF_LOG(x_Then) x_Then
#if defined EMP_XX_LOUSY
#define EMP_LOG_LOUSY(       ...) EMP_LOG_MESSAGE(Lousy       ,  __VA_ARGS__)
#define EMP_LOG_DEBUG(       ...) EMP_LOG_MESSAGE(Debug       ,  __VA_ARGS__)
#define EMP_LOG_WARNING(     ...) EMP_LOG_MESSAGE(Warning     ,  __VA_ARGS__)
#elif defined EMP_XX_DEBUG
#define EMP_LOG_LOUSY(       ...)
#define EMP_LOG_DEBUG(       ...) EMP_LOG_MESSAGE(Debug       ,  __VA_ARGS__)
#define EMP_LOG_WARNING(     ...) EMP_LOG_MESSAGE(Warning     ,  __VA_ARGS__)
#else
#define EMP_LOG_LOUSY(       ...)
#define EMP_LOG_DEBUG(       ...)
#define EMP_LOG_WARNING(     ...)
#endif
#define EMP_LOG_INFO(        ...) EMP_LOG_MESSAGE(Information ,  __VA_ARGS__)
#define EMP_LOG_ERROR(       ...) EMP_LOG_MESSAGE(Error       ,  __VA_ARGS__)
#else
#define EMP_XX_IF_LOG(x_Then) 
#define EMP_LOG_DEBUG(       ...) //EMP_LOG_MESSAGE(Debug       ,  __VA_ARGS__)
#define EMP_LOG_INFO(        ...) //EMP_LOG_MESSAGE(Information ,  __VA_ARGS__)
#define EMP_LOG_WARNING(     ...) //EMP_LOG_MESSAGE(Warning     ,  __VA_ARGS__)
#define EMP_LOG_ERROR(       ...) //EMP_LOG_MESSAGE(Error       ,  __VA_ARGS__)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
