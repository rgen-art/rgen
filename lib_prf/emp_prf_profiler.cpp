//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_prf_profiler.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_PROFILE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace prf {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
time_profiler_c::time_profiler_c(c_astring& a_sName, c_bool a_bAuto):
m_Watch(),
m_sName(a_sName),
m_sCurrent(),
m_sMin(),
m_sMax(),
m_sAverage(),
m_sDelta(),
m_bStarted(false),
m_bAuto(a_bAuto)
{
    if (m_bAuto)
    {
        begin_sample();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void time_profiler_c::output(void)
{
    m_sCurrent.clear();
    m_sMin.clear();
    m_sMax.clear();
    m_sAverage.clear();
    m_sDelta.clear();

    EMP_LOG_INFO("time_profiler => #0 : cur : #1, min : #2, max : #3, avg : #4, dt : #5",
            m_sName,
            emp::str::from(m_sCurrent, m_Watch.current()), // @@0 tmp needed ?
            emp::str::from(m_sMin, m_Watch.min()), // @@0 tmp needed ?
            emp::str::from(m_sMax, m_Watch.max()), // @@0 tmp needed ?
            emp::str::from(m_sAverage, m_Watch.average()), // @@0 tmp needed ?
            emp::str::from(m_sDelta, m_Watch.delta())); // @@0 tmp needed ?
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
memory_profiler_c::memory_profiler_c(c_astring& a_sName, c_bool a_bAuto):
    m_sName(a_sName),
    m_bAuto(a_bAuto),
    m_bStarted(false)
{
    if (m_bAuto)
    {
        begin_sample();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void memory_profiler_c::begin_sample(void)
{
    m_bStarted = true;

    auto const& rAllocator = EMP_ALLOCATOR;

    m_AllocationTimeSum = rAllocator.alloc().sum();
    m_DeallocationTimeSum = rAllocator.dealloc().sum();

    m_ReqAllocatedSum.bytes = rAllocator.req().m_AllocBytes;
    m_ReqAllocatedSum.count = rAllocator.req().m_stAllocCount;

    m_ReqDeallocatedSum.bytes = rAllocator.req().m_DeallocBytes;
    m_ReqDeallocatedSum.count = rAllocator.req().m_stDeallocCount;

    m_ReqNetSum.bytes = rAllocator.req().net_bytes();
    m_ReqNetSum.count = rAllocator.req().net_count();

    m_RawAllocatedSum.bytes = rAllocator.raw().m_AllocBytes;
    m_RawAllocatedSum.count = rAllocator.raw().m_stAllocCount;

    m_RawDeallocatedSum.bytes = rAllocator.raw().m_DeallocBytes;
    m_RawDeallocatedSum.count = rAllocator.raw().m_stDeallocCount;

    m_RawNetSum.bytes = rAllocator.raw().net_bytes();
    m_RawNetSum.count = rAllocator.raw().net_count();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void update_fragmentation(frame_s& a_rFrame);
void update_fragmentation(frame_s& a_rFrame)
{
    //a_rFrame.fragmentation = (float)(100.0 * ((double)a_rFrame.count / (double)a_rFrame.bytes.m_ui64Count));
    if (a_rFrame.count == 0)
    { a_rFrame.fragmentation = 0; }
    else
    { a_rFrame.fragmentation = (float)((double)a_rFrame.bytes.m_stCount / (double)a_rFrame.count); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void update_frame(T& a_rtFrame, bool& a_rbSigned, T a_stCount, c_bool a_bSub)
{
    if (a_bSub)
    {
        if (a_rbSigned)
        {
            a_rtFrame += a_stCount;
            a_rbSigned = true;
        }
        else
        {
            if (a_stCount > a_rtFrame)
            {
                a_rtFrame = a_stCount - a_rtFrame;
                a_rbSigned = true;
            }
            else
            {
                a_rtFrame -= a_stCount;
                a_rbSigned = false;
            }
        }
    }
    else
    {
        if (a_rbSigned)
        {
            if (a_stCount > a_rtFrame)
            {
                a_rtFrame = a_stCount - a_rtFrame;
                a_rbSigned = false;
            }
            else
            {
                a_rtFrame -= a_stCount;
                a_rbSigned = true;
            }
        }
        else
        {
            a_rtFrame += a_stCount;
            a_rbSigned = false;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void update_frame(frame_s const& a_rSum, frame_s& a_rFrame, emp::mem::size_s a_Bytes, c_size a_stCount);
void update_frame(frame_s const& a_rSum, frame_s& a_rFrame, emp::mem::size_s a_Bytes, c_size a_stCount)
{
    c_size stCount = a_rSum.count;
    if (stCount > a_stCount)
    { update_frame(a_rFrame.count, a_rFrame.count_signed, stCount - a_stCount, true); }
    else if (stCount <= a_stCount)
    { update_frame(a_rFrame.count, a_rFrame.count_signed, a_stCount - stCount, false); }

    c_size uiBytes = a_rSum.bytes.m_stCount;
    if (uiBytes > a_Bytes.m_stCount)
    { update_frame(a_rFrame.bytes.m_stCount, a_rFrame.byte_signed, uiBytes - a_Bytes.m_stCount, true); }
    else if (uiBytes <= a_Bytes.m_stCount)
    { update_frame(a_rFrame.bytes.m_stCount, a_rFrame.byte_signed, a_Bytes.m_stCount - uiBytes, false); }

    update_fragmentation(a_rFrame);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void update_abs_frame(frame_s const& a_rSum, frame_s& a_rFrame, emp::mem::size_s a_Bytes, c_size a_stCount);
void update_abs_frame(frame_s const& a_rSum, frame_s& a_rFrame, emp::mem::size_s a_Bytes, c_size a_stCount)
{
    a_rFrame.bytes.m_stCount += a_Bytes.m_stCount - a_rSum.bytes.m_stCount;
    a_rFrame.count += a_stCount - a_rSum.count;
    update_fragmentation(a_rFrame);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void memory_profiler_c::end_sample(void)
{
    if (m_bStarted == false)
    { return; }

    auto const& rAllocator = EMP_ALLOCATOR;

    update_abs_frame(m_ReqAllocatedSum, m_ReqAllocated, rAllocator.req().m_AllocBytes, rAllocator.req().m_stAllocCount);
    update_abs_frame(m_ReqDeallocatedSum, m_ReqDeallocated, rAllocator.req().m_DeallocBytes, rAllocator.req().m_stDeallocCount);
    update_abs_frame(m_RawAllocatedSum, m_RawAllocated, rAllocator.raw().m_AllocBytes, rAllocator.raw().m_stAllocCount);
    update_abs_frame(m_RawDeallocatedSum, m_RawDeallocated, rAllocator.raw().m_DeallocBytes, rAllocator.raw().m_stDeallocCount);

    update_frame(m_ReqNetSum, m_ReqNet, rAllocator.req().net_bytes(), rAllocator.req().net_count());
    update_frame(m_RawNetSum, m_RawNet, rAllocator.raw().net_bytes(), rAllocator.raw().net_count());

    emp::tim::time_c::sub(rAllocator.alloc().sum(), m_AllocationTimeSum, m_AllocationTime);
    emp::tim::time_c::sub(rAllocator.dealloc().sum(), m_DeallocationTimeSum, m_DeallocationTime);

    m_OverheadNet.m_stCount = m_RawNet.bytes.m_stCount - m_ReqNet.bytes.m_stCount;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void memory_profiler_c::output(void)
{
    EMP_LOG_INFO("memory_profiler 1/4 => #0 : time     : alloc (#1), dealloc (#2)", m_sName,
        m_AllocationTime,
        m_DeallocationTime);

    EMP_LOG_INFO("memory_profiler 2/4 => #0 : alloc    : req (#1)/#2 - #3 || raw (#4)/#5 - #6", m_sName,
        m_ReqAllocated.bytes,
        m_ReqAllocated.count,
        m_ReqAllocated.fragmentation,
        m_RawAllocated.bytes,
        m_RawAllocated.count,
        m_RawAllocated.fragmentation);

    EMP_LOG_INFO("memory_profiler 3/4 => #0 : dealloc  : req (#1)/#2 - #3 || raw (#4)/#5 - #6", m_sName,
        m_ReqDeallocated.bytes,
        m_ReqDeallocated.count,
        m_ReqDeallocated.fragmentation,
        m_RawDeallocated.bytes,
        m_RawDeallocated.count,
        m_RawDeallocated.fragmentation);

    EMP_LOG_INFO("memory_profiler 4/4 => #0 : delta    : req (#1#2)/#3#4 - #5 || raw (#6#7)/#8#9 - #10", m_sName,
        m_ReqNet.byte_signed ? ALITERAL("-") : ALITERAL(""),
        m_ReqNet.bytes,
        m_ReqNet.count_signed ? ALITERAL("-") : ALITERAL(""),
        m_ReqNet.count,
        m_ReqNet.fragmentation,
        m_RawNet.byte_signed ? ALITERAL("-") : ALITERAL(""),
        m_RawNet.bytes,
        m_RawNet.count_signed ? ALITERAL("-") : ALITERAL(""),
        m_RawNet.count,
        m_RawNet.fragmentation);

    EMP_LOG_INFO("memory_profiler 5/5 => #0 : overhead : #1", m_sName, m_OverheadNet);

    m_ReqAllocated = frame_s();
    m_ReqDeallocated = frame_s();
    m_ReqNet = frame_s();

    m_RawAllocated = frame_s();
    m_RawDeallocated = frame_s();
    m_RawNet = frame_s();

    m_ReqAllocatedSum = frame_s();
    m_ReqDeallocatedSum = frame_s();
    m_ReqNetSum = frame_s();

    m_RawAllocatedSum = frame_s();
    m_RawDeallocatedSum = frame_s();
    m_RawNetSum = frame_s();

    m_OverheadNet = emp::mem::size_s();

    m_AllocationTime = emp::tim::time_c();
    m_DeallocationTime = emp::tim::time_c();

    m_AllocationTimeSum = emp::tim::time_c();
    m_DeallocationTimeSum = emp::tim::time_c();

    m_bStarted = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
