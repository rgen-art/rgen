//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_tim_time.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ast_errno.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_COMPILER_MSC
#include "emp_hh_windows.h"
#undef min
#undef max
#else
#include <sys/time.h>
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tim {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
clock_c* clock_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN time_c const time_c::from_seconds(float const a_fSeconds)
{
    // @@0 :runtime_cast
    EMP_ASSERT(a_fSeconds >= 0);
    return time_c(emp::ast::cast<ui64>(a_fSeconds * 1000000));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float time_c::to_seconds(c_ui64 a_ui64Usec)
{
    // @@0 :runtime_cast
    return emp::ast::cast<float>(a_ui64Usec) / 1000000.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float time_c::to_seconds(time_c const& t)
{ return to_seconds(t.usec()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool time_c::get(time_c& t)
{
#if defined EMP_XX_COMPILER_MSC
    LARGE_INTEGER liTicks;
    EMP_RET_FALSE_IF_NOT(QueryPerformanceCounter(&liTicks)); // @@0 GetLastError
    t.m_ui64usec = emp::ast::cast<ui64>(liTicks.QuadPart);
    return true;
#else
    timeval tv;

    if (gettimeofday(&tv, nullptr) != 0)
    {
        EMP_LOG_ERROR("failed to \"gettimeofday\" : #0", astring::ref(emp::ast::errno_to_cstr()));
        return false;
    }

    EMP_ASSERT(tv.tv_sec > 0 || tv.tv_sec > 0);

    // @@0 :runtime_cast
    t.m_ui64usec =  emp::ast::cast<ui64>(tv.tv_sec) * 1000000 + emp::ast::cast<ui64>(tv.tv_usec);

    return true;
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui64 time_c::miliseconds(time_c const& t)
{
    // @@0 :runtime_cast
//    EMP_ASSERT(t.m_ui64usec < emp::tt::max<uint>::value);
    return t.m_ui64usec / 1000;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
clock_c::clock_c(void):
m_Reference(),
m_Application(),
m_System(),
m_Last(),
m_Delta(),
m_bFirstFrame(true),
m_bPaused(false)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool clock_c::create(void)
{
    if (poll_current() == false)
    { return false; }

    m_Reference = m_System;
    m_Last = m_System;

    m_Application = emp::tim::time_c();
    m_Delta = emp::tim::time_c();

    m_bFirstFrame = true;

    m_bPaused = false;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool clock_c::update(void)
{
    if (poll_current() == false)
    { return false; }

    emp::tim::time_c::sub(m_System, m_Last, m_Delta);
    // @@0 
    /*if (emp::tim::time_c::to_seconds(m_Delta) > (0.016f * 4.0f))
    {
        emp::tim::time_c::from_seconds((0.016f * 4.0f), m_Delta);
        if (m_bPaused)
        {
            m_Delta = emp::tim::time_c();
        }
    }*/
        //emp::tim::time_c::from_seconds(0.016f, m_Delta);

    m_Last = m_System;
    if (m_bFirstFrame)
    {
        m_bFirstFrame = false;
        return update();
    }
    emp::tim::time_c::sub(m_System, m_Reference, m_Application);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool clock_c::system_current(time_c& a_rfTime)
{
    a_rfTime = time_c();
    if (poll_current() == false)
    { return false; }
    a_rfTime = m_System;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool clock_c::poll_current(void)
{
    while (true)
    {
        emp::tim::time_c oCurrent;
        if (emp::tim::time_c::get(oCurrent) == false)
        { return false; }
        c_ui64 uiLast = m_System.m_ui64usec;
        c_ui64 uiCurrent = oCurrent.m_ui64usec;
        if (uiLast == 0)
        {
            m_Last = oCurrent;
            m_System = oCurrent;
            continue;
        }
        else if (uiCurrent > uiLast &&
                (uiCurrent - uiLast) > 35000000)
        {
            EMP_LOG_ERROR("gettimeofday is jumping forward");
            m_System = oCurrent;
            continue;
        }
        else if (uiCurrent < uiLast)
        {
            EMP_LOG_ERROR("gettimeofday is jumping backward");
            m_System = oCurrent;
            continue;
        }
        m_System = oCurrent;
        break;
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
watch_c::watch_c(void):
m_Last(),
m_DeltaCur(0),
m_DeltaMin(time_c::max()),
m_DeltaMax(0),
m_DeltaDelta(),
m_DeltaAverage(),
m_DeltaSum(0),
m_stSampleCount(0)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void watch_c::start(void)
{ EMP_ASSERT_BOOL_CALL(EMP_CLOCK.system_current(m_Last)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void watch_c::stop(void)
{
    if (emp::tim::time_c::equal(m_Last, emp::tim::time_c()))
    { return; }

    emp::tim::time_c oCurrent;
    EMP_ASSERT_BOOL_CALL(EMP_CLOCK.system_current(oCurrent));

    emp::tim::time_c oDelta;
    emp::tim::time_c::sub(oCurrent, m_Last, oDelta);

    if (emp::tim::time_c::less(m_DeltaCur, oDelta))
    { emp::tim::time_c::sub(oDelta, m_DeltaCur, m_DeltaDelta); }
    else
    { emp::tim::time_c::sub(m_DeltaCur, oDelta, m_DeltaDelta); }

    m_DeltaCur = oDelta;

    emp::tim::time_c::min(m_DeltaMin, m_DeltaCur, m_DeltaMin);
    emp::tim::time_c::max(m_DeltaMax, m_DeltaCur, m_DeltaMax);

    emp::tim::time_c::add(m_DeltaSum, m_DeltaCur, m_DeltaSum);
    ++m_stSampleCount;

    emp::tim::time_c::div(m_DeltaSum, time_c(m_stSampleCount), m_DeltaAverage);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
