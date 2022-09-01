//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_TRACK_H
#define EMP_SND_TRACK_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_note.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class layout_i;
class scale_i;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, track_c)
private:
    u8string m_su8Name;
    scale_i* m_pScale;
    layout_i* m_pLayout;
    ui64 m_ui64BeatDuration;
    float m_fVolume;
    bool m_bMute;
    bool m_bLoop;
    vector<note_c> m_vNotes;

public:
    track_c(void);
    ~track_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    void destroy_track(void);
    void clear_track(void);

    void copy(track_c const& a_rFrom);

    void scale(c_double a_fFactor);

    void translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack);

public:
    EMP_RETURN emp::tim::time_c start(void) const;
    EMP_RETURN emp::tim::time_c duration(void) const;
    EMP_RETURN emp::tim::time_c completion(void) const;
    EMP_RETURN size_t max_index(void) const;

public:
    EMP_RETURN c_u8string& name(void) const { return m_su8Name; }
    void set_name(c_u8string& a_su8Value) { m_su8Name = a_su8Value; }

    EMP_RETURN scale_i const* scale(void) const { return m_pScale; }
    EMP_RETURN scale_i* scale(void) { return m_pScale; }
    void set_scale(scale_i* const a_pValue) { m_pScale = a_pValue; }

    EMP_RETURN layout_i const* layout(void) const { return m_pLayout; }
    EMP_RETURN layout_i* layout(void) { return m_pLayout; }
    void set_layout(layout_i* const a_pValue) { m_pLayout = a_pValue; }

    EMP_RETURN ui64 beat_duration(void) const { return m_ui64BeatDuration; }
    void set_beat_duration(c_ui64 a_ui64Value);

    EMP_RETURN float beat_per_minute(void) const;
    void set_beat_per_minute(c_float a_fValue);

    EMP_RETURN float volume(void) const { return m_fVolume; }
    void set_volume(c_float a_fValue) { m_fVolume = a_fValue; }

    EMP_RETURN bool mute(void) const { return m_bMute; }
    void set_mute(c_bool a_bValue) { m_bMute = a_bValue; }

    EMP_RETURN bool loop(void) const { return m_bLoop; }
    void set_loop(c_bool a_bValue) { m_bLoop = a_bValue; }

    EMP_RETURN size_t beat_division(void) const { return 4; }

public:
    EMP_RETURN ui64 snap_time(c_ui64 a_uiTime) const;
    EMP_RETURN ui64 snap_duration(c_ui64 a_uiDuration) const;

public:
    EMP_RETURN size_t get_note(c_size a_stNote, emp::tim::time_c const& a_rTime, emp::tim::time_c const& a_rDuration,
        c_size a_stIndex) const;

    void remove_note(c_size a_stNote);

    EMP_RETURN size_t add_note(emp::tim::time_c const& a_rTime, emp::tim::time_c const& a_rDuration, c_size a_stIndex,
        c_bool a_bSnap);

    EMP_RETURN size_t move_note(c_size a_stNote, emp::tim::time_c const& a_rTime, c_size a_stIndex, c_bool a_bSnap);

    EMP_RETURN size_t scale_note(c_size a_stNote, emp::tim::time_c const& a_rTime, c_size a_stIndex, c_bool a_bSnap);

public:
    EMP_RETURN vector<note_c>& notes(void) { return m_vNotes; }
    EMP_RETURN vector<note_c> const& notes(void) const { return m_vNotes; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

