//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PRF_PROFILER_H
#define EMP_PRF_PROFILER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace prf {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mem;
using namespace emp::str;
using namespace emp::tim;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, time_profiler_c)
private:
    watch_c m_Watch;
    astring m_sName;
    astring m_sCurrent;
    astring m_sMin;
    astring m_sMax;
    astring m_sAverage;
    astring m_sDelta;
    bool m_bStarted;
    bool m_bAuto;

public:
    time_profiler_c(c_astring& a_sName, c_bool a_bAuto = false);
    ~time_profiler_c(void)
    {
        if (m_bAuto)
        {
            end_sample();
            output();
        }
    }

public:
    void begin_sample(void) { m_Watch.start(); }
    void end_sample(void) { m_Watch.stop(); }

public:
    void output(void);

public:
    EMP_RETURN watch_c const& watch(void) const { return m_Watch; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(struct, frame_s)
    emp::mem::size_s bytes;
    size_t count;
    double fragmentation;
    bool byte_signed;
    bool count_signed;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, memory_profiler_c)
private:
    astring m_sName;
    bool m_bStarted;
    bool m_bAuto;

    frame_s m_ReqAllocated;
    frame_s m_ReqDeallocated;
    frame_s m_ReqNet;

    frame_s m_RawAllocated;
    frame_s m_RawDeallocated;
    frame_s m_RawNet;

    frame_s m_ReqAllocatedSum;
    frame_s m_ReqDeallocatedSum;
    frame_s m_ReqNetSum;

    frame_s m_RawAllocatedSum;
    frame_s m_RawDeallocatedSum;
    frame_s m_RawNetSum;

    emp::mem::size_s m_OverheadNet;

    emp::tim::time_c m_AllocationTime;
    emp::tim::time_c m_DeallocationTime;

    emp::tim::time_c m_AllocationTimeSum;
    emp::tim::time_c m_DeallocationTimeSum;

public:
    memory_profiler_c(c_astring& a_sName, c_bool a_bAuto = false);
    ~memory_profiler_c(void)
    {
        if (m_bAuto)
        {
            end_sample();
            output();
        }
    }

public:
    void begin_sample(void);
    void end_sample(void);

public:
    void output(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, profiler_c)
private:
    memory_profiler_c m_Memory;
    time_profiler_c m_Time;

public:
    EMP_INLINE profiler_c(c_astring& a_sName, c_bool a_bAuto = false):
        m_Memory(a_sName, a_bAuto),
        m_Time(a_sName, a_bAuto)
    {}

public:
    EMP_INLINE void begin_sample(void)
    {
        m_Time.begin_sample();
        m_Memory.begin_sample();
    }

    EMP_INLINE void end_sample(void)
    {
        m_Time.end_sample();
        m_Memory.end_sample();
    }

public:
    EMP_INLINE void output(void)
    {
        m_Time.output();
        m_Memory.output();
    }

public:
    EMP_RETURN emp::tim::watch_c const& watch(void) const { return m_Time.watch(); }
    EMP_RETURN time_profiler_c const& time(void) const { return m_Time; }
    EMP_RETURN memory_profiler_c const& memory(void) const { return m_Memory; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

