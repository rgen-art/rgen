//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TIM_TIME_H
#define EMP_TIM_TIME_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tim {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(time_c)
public:
    friend class clock_c;

    ui64 m_ui64usec = 0;

public:

    explicit time_c(c_ui64 a_ui64usec):
        m_ui64usec(a_ui64usec)
    {}

    EMP_RETURN bool operator==(time_c const& a_rFrom) const
    { return m_ui64usec == a_rFrom.m_ui64usec; }

    //time_c(time_c const& a_rFrom):
    //    m_ui64usec(a_rFrom.m_ui64usec)
    //{ }

    //time_c& operator=(time_c const& a_rFrom)
    //{
    //    m_ui64usec = a_rFrom.m_ui64usec;
    //    return *this;
    //}

    EMP_RETURN ui64 usec(void) const { return m_ui64usec; }

public:
    static void div(time_c const& t0, time_c const& t1, time_c& tout) { tout.m_ui64usec = t0.m_ui64usec / t1.m_ui64usec; }

    static EMP_INLINE time_c const sub(time_c const t0, time_c const t1)
    {
        EMP_ASSERT(t0.m_ui64usec >= t1.m_ui64usec);
        return time_c(t0.m_ui64usec - t1.m_ui64usec);
    }

    static EMP_INLINE void sub(time_c const& t0, time_c const& t1, time_c& tout)
    {
        EMP_ASSERT(t0.m_ui64usec >= t1.m_ui64usec);
        tout.m_ui64usec = t0.m_ui64usec - t1.m_ui64usec;
    }

    static EMP_RETURN bool equal(time_c const& t0, time_c const& t1) { return t0.m_ui64usec == t1.m_ui64usec; }

    static EMP_RETURN float to_seconds(c_ui64 a_ui64Time);
    static EMP_RETURN float to_seconds(time_c const& t);

    static EMP_RETURN time_c const from_seconds(float const a_fSeconds);

    static EMP_RETURN bool get(time_c& t);

    static EMP_RETURN time_c const add(time_c const t0, time_c const t1) { return time_c(t0.m_ui64usec + t1.m_ui64usec); }
    static void add(time_c const& t0, time_c const& t1, time_c& tout) { tout.m_ui64usec = t0.m_ui64usec + t1.m_ui64usec; }

    static EMP_RETURN bool less(time_c const& t0, time_c const& t1) { return t0.m_ui64usec < t1.m_ui64usec; }
    static EMP_RETURN bool less_equal(time_c const& t0, time_c const& t1) { return t0.m_ui64usec <= t1.m_ui64usec; }

    static EMP_RETURN bool more(time_c const& t0, time_c const& t1) { return t0.m_ui64usec > t1.m_ui64usec; }
    static EMP_RETURN bool more_equal(time_c const& t0, time_c const& t1) { return t0.m_ui64usec >= t1.m_ui64usec; }

    static EMP_RETURN ui64 miliseconds(time_c const& t);

    static void max(time_c const& t0, time_c const& t1, time_c& tout)
    {
        if (less(t0, t1))
        { tout = t1; }
        else
        { tout = t0; }
    }

    static EMP_RETURN emp::tim::time_c const max(void)
    {
        time_c oTime;
        emp::tt::get_max(oTime.m_ui64usec);
        return oTime;
    }

    static void min(time_c const& t0, time_c const& t1, time_c& tout)
    {
        if (less(t0, t1))
        { tout = t0; }
        else
        { tout = t1; }
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, clock_c)
private:
    static clock_c* s_pInstance;
public:
    #define EMP_CLOCK emp::tim::clock_c::instance()
    static void set_instance(clock_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN clock_c& instance(void) { return *s_pInstance; }

private:
    time_c m_Reference;
    time_c m_Application;
    time_c m_System;
    time_c m_Last;
    time_c m_Delta;
    bool m_bFirstFrame;
    bool m_bPaused;

public:
    clock_c(void);

    EMP_RETURN bool create(void);

    EMP_RETURN bool update(void);

    EMP_RETURN time_c const& delta(void) const { return m_Delta; }

    EMP_RETURN time_c const& application_current(void) const { return m_Application; }
    EMP_RETURN bool system_current(time_c& a_rfTime);

    void pause(c_bool a_bValue) { m_bPaused = a_bValue; }

private:
    EMP_RETURN bool poll_current(void);
};
EMP_TYPEDEF(clock_c)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, watch_c)
private:
    time_c m_Last;
    time_c m_DeltaCur;
    time_c m_DeltaMin;
    time_c m_DeltaMax;
    time_c m_DeltaDelta;
    time_c m_DeltaAverage;
    time_c m_DeltaSum;
    size_t m_stSampleCount;

public:
    watch_c(void);

public:
    void start(void);
    void stop(void);

    EMP_RETURN bool has_sample(void) const { return m_stSampleCount >= 1; }

    EMP_RETURN time_c average(void) const { return m_DeltaAverage; }
    EMP_RETURN time_c current(void) const { return m_DeltaCur; }
    EMP_RETURN time_c min(void) const { return m_DeltaMin; }
    EMP_RETURN time_c max(void) const { return m_DeltaMax; }
    EMP_RETURN time_c delta(void) const { return m_DeltaDelta; }
    EMP_RETURN time_c sum(void) const { return m_DeltaSum; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
