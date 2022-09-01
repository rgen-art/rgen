//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_track.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_algebra.h"

#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
track_c::track_c(void):
    m_su8Name(),
    m_pScale(),
    m_pLayout(),
    m_ui64BeatDuration(),
    m_fVolume(1),
    m_bMute(false),
    m_bLoop(),
    m_vNotes()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
track_c::~track_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool track_c::post_load(void)
{
    set_beat_duration(m_ui64BeatDuration);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::destroy_track(void)
{
    m_su8Name = u8string();
    m_pScale = nullptr;
    m_pLayout = nullptr;
    m_ui64BeatDuration = 0;
    m_fVolume = 1;
    m_bMute = false;
    m_bLoop = false;
    clear_track();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::clear_track(void)
{
    m_vNotes.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::copy(track_c const& a_rFrom)
{
    destroy_track();

    m_su8Name = a_rFrom.m_su8Name;
    m_pScale = a_rFrom.m_pScale;
    m_pLayout = a_rFrom.m_pLayout;
    m_ui64BeatDuration = a_rFrom.m_ui64BeatDuration;
    m_bLoop = a_rFrom.m_bLoop;
    c_size stCount = a_rFrom.m_vNotes.count();
    m_vNotes.resize(stCount);
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vNotes.set(stIndex, a_rFrom.m_vNotes.ref(stIndex)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_beat_duration(c_ui64 a_ui64Value)
{ m_ui64BeatDuration = a_ui64Value; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float track_c::beat_per_minute(void) const
{ return 60*1000*1000.0f/emp::ast::cast<float>(m_ui64BeatDuration); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_beat_per_minute(float const a_fValue)
{
    c_ui64 uiValue = emp::ast::cast<ui64>(60*1000*1000.0/emp::ast::cast<double>(a_fValue));
    if (m_ui64BeatDuration > 0 &&
        m_ui64BeatDuration != uiValue)
    {
        float const fFactor =
            emp::ast::cast<float>(
            emp::ast::cast<double>(uiValue) /
            emp::ast::cast<double>(m_ui64BeatDuration));
        scale(fFactor);
    }
    m_ui64BeatDuration = uiValue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::scale(c_double a_fFactor)
{
    if (emp::tt::equal_(a_fFactor, 1.0))
    { return; }

    set_beat_duration(emp::ast::cast<ui64>(emp::ast::cast<double>(beat_duration()) * a_fFactor));

    c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        note_c& rNote = m_vNotes.ref(stIndex);
        rNote.time = emp::ast::cast<ui64>(emp::ast::cast<float>(rNote.time) * a_fFactor);
        rNote.duration = emp::ast::cast<ui64>(emp::ast::cast<float>(rNote.duration) * a_fFactor);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack)
{
    emp::tim::time_c oDelta = a_rDelta;
    emp::tim::time_c const oStart = start();
    if (a_bBack)
    {
        if (emp::tim::time_c::more(oDelta, oStart))
        { oDelta = oStart; }
    }

    if (emp::tim::time_c::equal(oDelta, emp::tim::time_c()) == false)
    {
        c_size stCount = m_vNotes.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            note_c& rNote = m_vNotes.ref(stIndex);
            emp::tim::time_c oTime(rNote.time);
            if (a_bBack)
            {
                EMP_ASSERT(emp::tim::time_c::more_equal(oTime, oDelta));
                emp::tim::time_c::sub(oTime, oDelta, oTime);
            }
            else
            { emp::tim::time_c::add(oTime, oDelta, oTime); }
            rNote.time = oTime.usec();
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::tim::time_c track_c::start(void) const
{ return m_vNotes.count() ? emp::tim::time_c(m_vNotes.first_const_ref().time) : emp::tim::time_c(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::tim::time_c track_c::duration(void) const
{
    if (m_vNotes.count())
    {
        note_c const& rNote = m_vNotes.last_ref();

        emp::tim::time_c oDuration;
        emp::tim::time_c::add(
            emp::tim::time_c(rNote.time),
            emp::tim::time_c(rNote.duration), oDuration);
        return oDuration;
    }
    return emp::tim::time_c();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::tim::time_c track_c::completion(void) const
{
    emp::tim::time_c oCompletion;
    c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        note_c const& rNote = m_vNotes.ref(stIndex);
        emp::tim::time_c::add(oCompletion,
            emp::tim::time_c(rNote.duration), oCompletion);
    }
    return oCompletion;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t track_c::max_index(void) const
{
    size_t stMax = 0;
    c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { stMax = emp::tt::maxof<size_t>(stMax, m_vNotes.ref(stIndex).index); }
    return stMax;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui64 track_c::snap_time(c_ui64 a_uiTime) const
{
    c_ui64 uiBeat = m_ui64BeatDuration / beat_division();
    return (a_uiTime / uiBeat) * uiBeat;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui64 track_c::snap_duration(c_ui64 a_uiDuration) const
{ return snap_time(a_uiDuration) + m_ui64BeatDuration / beat_division() - 100;}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t track_c::get_note(c_size a_stNote, emp::tim::time_c const& a_rTime, emp::tim::time_c const& a_rDuration, c_size a_stIndex) const
{
    c_ui64 uiTime = a_rTime.usec();
    c_ui64 uiDuration = a_rDuration.usec();
    c_ui64 uiEnd = uiTime + uiDuration;

    c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        note_c const& rNote = m_vNotes.ref(stIndex);
        c_ui64 uiNoteBegin = rNote.time;
        c_ui64 uiNoteEnd = uiNoteBegin + rNote.duration;

        if (stIndex == a_stNote ||
            rNote.index != a_stIndex ||
            (uiTime > uiNoteEnd || uiEnd < uiNoteBegin))
        { continue; }
        return stIndex;
    }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::remove_note(c_size a_stNote)
{ m_vNotes.remove(a_stNote); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t track_c::add_note(
    emp::tim::time_c const& a_rTime,
    emp::tim::time_c const& a_rDuration,
    c_size a_stIndex,
    c_bool a_bSnap)
{
    c_ui64 uiTime = a_rTime.usec();
    c_ui64 uiDuration = a_rDuration.usec();

    c_ui64 uiSnapTime = a_bSnap ? snap_time(uiTime) : uiTime;
    c_ui64 uiSnapDuration = a_bSnap ? snap_duration(uiDuration) : uiDuration;

    note_c const oNote(emp::ast::cast<ui32>(a_stIndex), uiSnapTime, uiSnapDuration);

    c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        note_c const& rNote = m_vNotes.ref(stIndex);
        if (uiSnapTime < rNote.time)
        {
            m_vNotes.insert(stIndex, oNote);
            return stIndex;
        }
    }
    m_vNotes.add(oNote);
    return stCount;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t track_c::move_note(c_size a_stNote, emp::tim::time_c const& a_rTime, c_size a_stIndex, c_bool a_bSnap)
{
    note_c const oNote(m_vNotes.ref(a_stNote));

    c_ui64 uiTime = a_rTime.usec();
    c_ui64 uiSnap = a_bSnap ? snap_time(uiTime) : uiTime;

    emp::tim::time_c const oTime(uiSnap);
    emp::tim::time_c const oDuration(oNote.duration);

    c_size stFound = get_note(a_stNote, oTime, oDuration, a_stIndex);
    if (stFound != a_stNote &&
        emp::tt::is_not_max(stFound))
    { return a_stNote; }

    m_vNotes.remove(a_stNote);
    return add_note( oTime, oDuration, a_stIndex, a_bSnap);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t track_c::scale_note(c_size a_stNote, emp::tim::time_c const& a_rTime, c_size a_stIndex, c_bool a_bSnap)
{
    note_c const oCurrent(m_vNotes.ref(a_stNote));

    c_ui64 uiCurrent = a_rTime.usec();

    c_ui64 uiCurrentBegin = oCurrent.time;
    //c_ui64 uiCurrentEnd = uiCurrentBegin + oCurrent.duration;

    if (uiCurrent < uiCurrentBegin)
    {
        /*c_size stFound = get_note(a_stNote, a_rTime, emp::tim::time_c(uiCurrentEnd - uiCurrent), a_stIndex);
        if (emp::tt::is_max(stFound))
        {
            c_size stNew = move_note(a_stNote, a_rTime, a_stIndex, a_bSnap);
            return scale_note(stNew, emp::tim::time_c(uiCurrentEnd), a_stIndex, a_bSnap);
        }*/
    }
    else //if (uiCurrent > uiCurrentEnd)
    {
        c_ui64 uiDuration = uiCurrent - uiCurrentBegin;
        c_ui64 uiSnap = a_bSnap ? snap_duration(uiDuration) : uiDuration;
        c_size stNew = move_note(a_stNote, emp::tim::time_c(uiCurrentBegin), a_stIndex, a_bSnap);
        c_size stFound = get_note(stNew, emp::tim::time_c(oCurrent.time), emp::tim::time_c(uiSnap), a_stIndex);
        if (emp::tt::is_max(stFound))
        { m_vNotes.ref(stNew).duration = uiSnap; }
        return stNew;
    }

    return a_stNote;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

