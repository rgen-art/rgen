//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_COR_EMPHASE_H
#define ASE_COR_EMPHASE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_option.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_cnt_array.h"

#include "emp_mem_pointer.h"

#include "emp_snd_constants.h"
#include "emp_snd_note.h"
#include "emp_snd_track.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//@@38 todo
//Ripple
//FlowField
//3d perlin noise
//Tentacles
//Caleidoscop
//Electriq smoke
//Voronoi
//Wind

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

class al_buffer_c;
class composition_c;
class envelope_c;
class layout_i;
class media_i;
class player_c;
class scale_i;
class track_c;

} }

namespace emp { namespace tim { struct time_c; } }

namespace emp { namespace app { EMP_FORWARD(struct, keyboard_event_s) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace sol { class music_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt { template <>
EMP_NOINSTANCE(class, key_t<emp::snd::note_c>)
public:
    static EMP_RETURN uintptr_t compute(emp::snd::note_c const& a_rValue) // @@0 bug not working if sizeof(uintptr_t) != sizeof(ui64)
    { return emp::ast::cast<uintptr_t>(a_rValue.time) + a_rValue.index; }

    static EMP_RETURN bool compare(
        emp::snd::note_c const& a_r0, emp::snd::note_c const& a_r1)
    { return a_r0.time == a_r1.time && a_r0.index == a_r1.index; }
}; } }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class mapping_c;
class matrix_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name music_e
#define enum_values\
    enum_value( Imitate   )\
    enum_value( Randomize )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name note_state_e
#define enum_values\
    enum_value( Before  )\
    enum_value( Current )\
    enum_value( After   )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN note_state_e is_valid_note(
    emp::snd::note_c const& a_rNote,
    emp::tim::time_c const& a_rTime,
    emp::tim::time_c const& a_rTolerance);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t get_notes(
        c_size a_stOffset,
        emp::snd::track_c const& a_rTrack,
        emp::tim::time_c const& a_rTime,
        emp::tim::time_c const& a_rTolerance,
        emp::cnt::vector<emp::snd::note_c>& a_rvNotes);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, matrix_c)
private:
    emp::cnt::array<size_t, emp::snd::constants::key_count, true> m_astNotes;

public:
    matrix_c(void);

public:
    void destroy(void);

public:
    void set_address(c_size a_stIndex, c_size a_stAddress);
    EMP_RETURN size_t get_address(c_size a_stIndex) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, mapping_c)
private:
    array<ui64, emp::snd::constants::key_count, true> m_aui64Chars;
    map_t<emp::app::key_e, ui32> m_mKeyCodeToEmphasis;

    astring m_sName;
    u8string m_su8Mapping;

public:
    mapping_c(void);
    ~mapping_c(void);

    EMP_RETURN bool post_load(void);

    void destroy(void);

public:
    EMP_RETURN c_astring& name(void) const { return m_sName; }
    EMP_RETURN c_u8string& mapping(void) { return m_su8Mapping; }

    EMP_RETURN bool keycode_to_emphasis(emp::app::c_key_e a_eKeyCode, ui32& a_rui32Emphasis) const;
    EMP_RETURN ui64 emphasis_to_keychar(c_ui32 a_ui32Emphasis) const;

    EMP_RETURN size_t emphasis_to_finger(c_ui32 a_ui32Emphasis) const;

    // @@0 remove EMP_RETURN ui32 emphasis_to_keycode(c_ui32 a_ui32Emphasis) const;
    // @@0 remove EMP_RETURN size_t keychar_to_emphasis(c_ui32 a_ui32Char) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(completion_data_s)
public:
    user_c* user = nullptr;
    float score = 0;
    ui64 time = 0;
    float success = 0;
    float failure = 0;

public:
    EMP_RETURN bool operator==(completion_data_s const& a_rFrom) const
    {
        return
            user == a_rFrom.user &&
            emp::tt::equal_(score, a_rFrom.score) &&
            time == a_rFrom.time &&
            emp::tt::equal_(success, a_rFrom.success) &&
            emp::tt::equal_(failure, a_rFrom.failure);
    }

    EMP_RETURN bool is_higher(completion_data_s const& a_rFrom) const
    {
        if (score > a_rFrom.score) { return true; }
        if (score < a_rFrom.score) { return false; }

        if (time < a_rFrom.time) { return true; }
        if (time > a_rFrom.time) { return false; }

        if (success > a_rFrom.success) { return true; }
        if (success < a_rFrom.success) { return false; }

        if (failure > a_rFrom.failure) { return true; }
        if (failure < a_rFrom.failure) { return false; }
    
        if (user->name().len() < a_rFrom.user->name().len()) { return true; }
        return false;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(user_data_s)
public:
    user_c* user = nullptr;
    ui32 played = 0;
    ui32 rating = 0;

public:
    EMP_RETURN bool operator==(user_data_s const& a_rFrom) const
    {
        return user == a_rFrom.user &&
            played == a_rFrom.played &&
            rating == a_rFrom.rating;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(completion_option_s)
public:
    bool shuffle = false;
    bool score_visible = true;
    bool score_letters = true;
    bool score_durations = true;
    bool keyboard_visible = true;
    bool keyboard_letters = true;
    mod_e mod = mod_e::Easy;

    explicit completion_option_s(c_option_s& a_rOptions):
        shuffle(a_rOptions.emphase.shuffle),
        score_visible(a_rOptions.emphase.score.visible),
        score_letters(a_rOptions.emphase.score.letters()),
        score_durations(a_rOptions.emphase.score.durations()),
        keyboard_visible(a_rOptions.emphase.keyboard.visible),
        keyboard_letters(a_rOptions.emphase.keyboard.letters()),
        mod(a_rOptions.emphase.mod)
    {}

    EMP_RETURN bool operator==(completion_option_s const& a_rFrom) const
    {
        return
            mod == a_rFrom.mod &&
            shuffle == a_rFrom.shuffle &&
            score_visible == a_rFrom.score_visible &&
            score_letters == a_rFrom.score_letters &&
            score_durations == a_rFrom.score_durations &&
            keyboard_visible == a_rFrom.keyboard_visible &&
            keyboard_letters == a_rFrom.keyboard_letters;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, completion_bucket_c)
private:
    completion_option_s m_Options;
    emp::cnt::vector<completion_data_s> m_vData;

public:
    completion_bucket_c(void):
        m_Options(),
        m_vData()
    {}

    completion_bucket_c(c_completion_option_s& a_rOptions):
        m_Options(a_rOptions),
        m_vData()
    {}

public:
    EMP_RETURN c_completion_option_s& options(void) const { return m_Options; }

    EMP_RETURN size_t data_count(void) const { return m_vData.count(); }

    EMP_RETURN c_completion_data_s& data(c_size a_stIndex) const { return m_vData.ref(a_stIndex); }

public:
    void add(c_completion_data_s& a_rData);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, performance_c)
private:
    vector<completion_bucket_c*> m_vpBuckets;
    vector<user_data_s> m_vUserData;
    completion_data_s m_Current;

public:
    performance_c(void);
    ~performance_c(void);

public:
    EMP_RETURN c_user_data_s* get_user_data(user_c* const a_pUser) const;
    EMP_RETURN user_data_s* get_user_data(user_c* const a_pUser);

public:
    EMP_RETURN c_completion_data_s& current(void) const { return m_Current; }

public:
    void add(c_option_s& a_rOptions, c_completion_data_s& a_rData);

    void clear(void);

    EMP_RETURN c_completion_data_s* highest(user_c* const a_pUser, c_option_s& a_rOptions) const;

    EMP_RETURN size_t bucket_count(void) const { return m_vpBuckets.count(); }

    EMP_RETURN completion_bucket_c const& bucket(c_size a_stIndex) const { return *m_vpBuckets.val(a_stIndex); }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, music_i)
public:
    music_i(void) {}
    virtual ~music_i(void) = 0;

public:
    //>> music_i
    virtual EMP_RETURN c_u8string& name(void) const = 0;

    virtual EMP_RETURN music_e type(void) const = 0;

    virtual void set_played(user_c* const a_pUser,emp::tim::time_c const& a_rTime, c_completion_data_s& a_rCompletion) = 0;

    virtual EMP_RETURN float beat_per_minute(void) const = 0;

    virtual EMP_RETURN emp::snd::track_c const* track(void) const = 0;

    virtual void scale(float const a_fFactor) = 0;
    virtual void translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack) = 0;
    virtual void build_copy(void) = 0;
    virtual void build_data(void) = 0;
    virtual void clear(void) = 0;

    virtual EMP_RETURN c_user_data_s* get_user_data(user_c* const a_pUser) const = 0;

    virtual EMP_RETURN user_data_s* get_user_data(user_c* const a_pUser) = 0;

    virtual EMP_RETURN c_completion_data_s* highest(user_c* const a_pUser, c_option_s& a_rOptions) const = 0;

    virtual EMP_RETURN performance_c const& performance(void) const = 0;
    //<< music_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline music_i::~music_i(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, composition_music_c, public, music_i)
private:
    performance_c m_Performance;
    emp::snd::composition_c* m_pComposition;
    EMP_POINTER(emp::snd::track_c) m_pTrack;
    float m_fScale;
    bool m_bBack;
    bool m_bChanged;
    emp::tim::time_c m_Translate;

public:
    composition_music_c(void);
    ~composition_music_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    void destroy(void);

public:
    //>> music_i
    EMP_RETURN c_u8string& name(void) const;
    EMP_RETURN music_e type(void) const { return music_e::Imitate; }
    void set_played(user_c* const a_pUser, emp::tim::time_c const& a_rTime, c_completion_data_s& a_rCompletion);
    EMP_RETURN float beat_per_minute(void) const;
    EMP_RETURN emp::snd::track_c const* track(void) const { return m_pTrack; }
    void scale(float const a_fFactor);
    void translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack);
    void build_data(void);
    void build_copy(void);
    void clear(void) { m_Performance.clear(); }
    EMP_RETURN c_user_data_s* get_user_data(user_c* const a_pUser) const { return m_Performance.get_user_data(a_pUser); }
    EMP_RETURN user_data_s* get_user_data(user_c* const a_pUser) { return m_Performance.get_user_data(a_pUser); }
    EMP_RETURN c_completion_data_s* highest(user_c* const a_pUser, c_option_s& a_rOption) const { return m_Performance.highest(a_pUser, a_rOption); }
    EMP_RETURN performance_c const& performance(void) const { return m_Performance; }
    //<< music_i

public:
    void set_composition(emp::snd::composition_c* const a_pValue)
    { m_pComposition = a_pValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,random_music_c, public, music_i)
private:
    EMP_POINTER(emp::snd::track_c) m_pComposition;
    EMP_POINTER(emp::snd::track_c) m_pTrack;

    bool m_bFullTime;
    bool m_bBack;
    bool m_bChanged;

    u8string m_su8Name;

    performance_c m_Performance;

    ui32 m_ui32MaxTouches;
    ui32 m_ui32MaxLeftTouches;
    ui32 m_ui32MaxRightTouches;

    float m_fBeatPerMinute;
    float m_fMaxDuration;
    float m_fScale;

    emp::tim::time_c m_Translate;

public:
    random_music_c(void);
    ~random_music_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    //>> music_i
    EMP_RETURN c_u8string& name(void) const { return m_su8Name; }
    EMP_RETURN music_e type(void) const { return music_e::Randomize; }
    void set_played(user_c* const a_pUser, emp::tim::time_c const& a_rTime, c_completion_data_s& a_rCompletion);
    EMP_RETURN float beat_per_minute(void) const { return m_fBeatPerMinute; }
    EMP_RETURN emp::snd::track_c const* track(void) const { return m_pTrack; }
    void scale(float const a_fFactor);
    void translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack);
    void build_data(void);
    void build_copy(void);
    void clear(void) { m_Performance.clear(); }
    EMP_RETURN c_user_data_s* get_user_data( user_c* const a_pUser) const { return m_Performance.get_user_data(a_pUser); }
    EMP_RETURN user_data_s* get_user_data(user_c* const a_pUser) { return m_Performance.get_user_data(a_pUser); }
    EMP_RETURN c_completion_data_s* highest(user_c* const a_pUser, c_option_s& a_rOption) const
    { return m_Performance.highest(a_pUser, a_rOption); }
    EMP_RETURN performance_c const& performance(void) const { return m_Performance; }
    //<< music_i
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, music_shell_c)
private:
    music_i const* m_pMusic;
    u8string m_sEmpty;

public:
    music_shell_c(void);
    music_shell_c(music_i const* const a_pMusic);

public:
    EMP_RETURN c_u8string& name(void) const;
    EMP_RETURN emp::tim::time_c duration(void) const;

    EMP_RETURN float beat_per_minute(void) const;

    EMP_RETURN ui32 rating(void) const;
    EMP_RETURN ui32 played(void) const;

    EMP_RETURN c_u8string& highest_user(void) const;
    EMP_RETURN float highest_completion(void) const;
    EMP_RETURN emp::tim::time_c highest_time(void) const;
    EMP_RETURN float highest_success(void) const;
    EMP_RETURN float highest_failure(void) const;

private:
    EMP_RETURN c_completion_data_s* highest(void) const;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, interpretation_c)
private:
    completion_data_s m_Completion;
    emp::cnt::map_t<emp::snd::note_c, size_t> m_mSuccess;
    emp::snd::track_c m_Success;
    emp::cnt::map_t<emp::snd::note_c, size_t> m_mFailure;
    emp::snd::track_c m_Failure;

public:
    interpretation_c(void);
    ~interpretation_c(void);

public:
    void destroy(void);

public:
    void compute_completion(emp::snd::track_c const& a_rTrack);

    c_completion_data_s& completion(void) const { return m_Completion; }

    void remove_success(emp::snd::note_c const& a_rValue);
    void set_success(emp::snd::note_c const& a_rValue);
    EMP_RETURN emp::snd::track_c const& success(void) const { return m_Success; }

    void set_failure(emp::snd::note_c const& a_rValue);
    EMP_RETURN emp::snd::track_c const& failure(void) const { return m_Failure; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, updater_i)
public:
    updater_i(void) {}
    virtual ~updater_i(void) = 0;

public:
    //>> updater_i
    virtual void start(void) = 0;
    virtual void stop(void) = 0;
    virtual void update(emp::tim::time_c const& a_rTime) = 0;
    virtual EMP_RETURN bool condition_satisfied(void) const = 0;
    virtual EMP_RETURN float current_completion(void) const = 0;
    virtual EMP_RETURN float music_time(void) const = 0;
    virtual EMP_RETURN emp::snd::track_c const* track(void) const = 0;
    virtual EMP_RETURN interpretation_c* interpretation(void) = 0;
    virtual void set_music(music_i* const a_pValue) = 0;
    //<< updater_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline updater_i::~updater_i(void) {}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,easy_updater_c, public, updater_i)
private:
    EMP_POINTER(interpretation_c) m_pInterpretation;
    bool m_bEnded;

    ase::cor::music_i* m_pInterpretMusic;

    emp::tim::time_c m_Time;
    emp::tim::time_c m_MusicTime;

    size_t m_stMusicNote;
    size_t m_stUserNote;

    emp::cnt::vector<emp::snd::note_c> m_vMusicNotes;
    emp::cnt::vector<emp::snd::note_c> m_vUserNotes;

public:
    easy_updater_c(void);

public:
    void destroy(void);

public:
    //>>updater_i
    void start(void);
    void stop(void);
    void update(emp::tim::time_c const& a_rTime);
    EMP_RETURN bool condition_satisfied(void) const;
    EMP_RETURN float current_completion(void) const;
    EMP_RETURN float music_time(void) const;
    EMP_RETURN emp::snd::track_c const* track(void) const;
    EMP_RETURN interpretation_c* interpretation(void)
    { return m_pInterpretation; }
    void set_music(music_i* const a_pValue);
    //<<updater_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, hard_updater_c, public, updater_i)
private:
    music_i* m_pInterpretMusic;
    bool m_bEnded;
    emp::tim::time_c m_Time;

    size_t m_stNote;
    emp::cnt::vector<emp::snd::note_c> m_vNotes;
    EMP_POINTER(interpretation_c) m_pInterpretation;

public:
    hard_updater_c(void);

    void destroy(void);

    //>>updater_i
    void start(void);
    void stop(void);
    void update(emp::tim::time_c const& a_rTime);
    EMP_RETURN bool condition_satisfied(void) const;
    EMP_RETURN float current_completion(void) const;
    EMP_RETURN float music_time(void) const;
    EMP_RETURN emp::snd::track_c const* track(void) const;
    EMP_RETURN interpretation_c* interpretation(void)
    { return m_pInterpretation; }
    void set_music(music_i* const a_pValue);
    //<<updater_i
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,improvize_updater_c, public, updater_i)
private:
    EMP_POINTER(interpretation_c) m_pInterpretation;
    emp::tim::time_c m_Time;
    music_i* m_pInterpretMusic;
    size_t m_stIterator;

public:
    improvize_updater_c(void);

    void destroy(void);

    //>>updater_i
    void start(void);
    void stop(void);
    void update(emp::tim::time_c const& a_rTime);
    EMP_RETURN bool condition_satisfied(void) const;
    EMP_RETURN float current_completion(void) const;
    EMP_RETURN float music_time(void) const;
    EMP_RETURN emp::snd::track_c const* track(void) const;
    EMP_RETURN interpretation_c* interpretation(void)
    { return m_pInterpretation; }
    void set_music(music_i* const a_pValue);
    //<<updater_i
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,normal_updater_c, public, updater_i)
private:
    music_i* m_pInterpretMusic;

    bool m_bEnded;

    emp::tim::time_c m_Time;

    size_t m_stNote;
    emp::cnt::vector<emp::snd::note_c> m_vNotes;
    EMP_POINTER(interpretation_c) m_pInterpretation;

public:
    normal_updater_c(void);

    void destroy(void);

    //>>updater_i
    void start(void);
    void stop(void);
    void update(emp::tim::time_c const& a_rTime);
    EMP_RETURN bool condition_satisfied(void) const;
    EMP_RETURN float current_completion(void) const;
    EMP_RETURN float music_time(void) const;
    EMP_RETURN emp::snd::track_c const* track(void) const;
    EMP_RETURN interpretation_c* interpretation(void)
    { return m_pInterpretation; }
    void set_music(music_i* const a_pValue);
    //<<updater_i
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, emphase_c)
private:
    EMP_POINTER(emp::cnt::array<emp::snd::media_i*, emp::snd::constants::key_count, true>) m_papMedias;
    float m_fCurrentFrequency;

public:
    emphase_c(void);
    ~emphase_c(void);

public:
    void create(void);
    void destroy(void);

public:
    void clear(void);

public:
    EMP_RETURN bool translate(
        emp::app::c_keyboard_event_s& a_rEvent,
        emp::tim::time_c const& a_rTime,
        emp::snd::track_c& a_rTrack,
        matrix_c& a_rMatrix,
        mapping_c const& a_rMapping,
        c_bool a_bPlay);

    EMP_RETURN bool create_music(
        c_astring& a_sFilename,
        c_u8string& a_su8Name,
        ase::sol::music_c const& a_rMusic,
        ase::cor::mapping_c const* const a_pMapping);

    //void create_layout(c_astring& a_sName, c_size a_stOffset);

    void play(
        emp::snd::layout_i* const a_pLayout,
        emp::snd::scale_i const* const a_pScale,
        c_size a_stIndex,
        c_float a_fVolume,
        c_bool a_bMute);
    void stop(c_size a_stIndex) const;

public:
    EMP_RETURN float current_frequency(void) const { return m_fCurrentFrequency; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, album_c)
private:
    emp::str::u8string m_su8Name;
    emp::gfx::model_c* m_pGfxCover;
    emp::cnt::vector<music_i*> m_vpMusics;

public:
    album_c(void);
    ~album_c(void);

public:
    void create(void);
    void destroy(void);

public:
    EMP_RETURN music_i const* try_get_music(c_size a_stIndex) const;
    EMP_RETURN music_i* try_get_music(c_size a_stIndex);
    EMP_RETURN music_i const& get_music(c_size a_stIndex) const;
    EMP_RETURN music_i& get_music(c_size a_stIndex);
    EMP_RETURN size_t count(void) const { return m_vpMusics.count(); }

public:
    EMP_RETURN emp::str::c_u8string& name(void) const { return m_su8Name; }
    EMP_RETURN emp::gfx::model_c const* cover(void) const { return m_pGfxCover; }

public:
    void add_music(music_i* const a_pMusic) { m_vpMusics.add(a_pMusic); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, album_list_c)
private:
    emp::cnt::vector<album_c*> m_vpAlbums;

public:
    album_list_c(void):
        m_vpAlbums()
    {}

public:
    emp::cnt::vector<album_c*> const& albums(void) const
    { return m_vpAlbums; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

