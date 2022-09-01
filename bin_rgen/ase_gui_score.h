//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_GUI_SCORE_H
#define ASE_GUI_SCORE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp {
    namespace gfx {
        template <typename...> class buffer_t;
    }

    namespace gui {
        EMP_FORWARD(class, label_c)
        EMP_FORWARD(struct, label_skin_s)
    }

    namespace snd {
        EMP_FORWARD(class, track_c)
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase {
    namespace cor {
        class interpretation_c;
        class mapping_c;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN u8string get_text(ase::cor::mapping_c const& a_rMapping, c_ui32 a_ui32Emphasis);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, score_c, public, container_c)
private:
    float m_fBeatPerMinute;
    float m_fBeatPerMesure;

    float m_fTimeScale;

    float m_fTime;

    float m_fStartSize;

    emp::cnt::vector<label_c*> m_vpLabels;
    emp::cnt::vector<size_t> m_vAddresses;

    emp::snd::track_c const* m_pTrack;
    emp::snd::track_c* m_pEditionTrack;

    ase::cor::interpretation_c const* m_pInterpretation;

    EMP_HOLDER(ase::cor::mapping_c const) m_pMapping;

    bool m_bShowLetters;
    bool m_bShowDurations;
    bool m_bEdition;

    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pTimeMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pTimeMeshIndices;

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pImMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pImMeshIndices;

    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pSuccessMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pSuccessMeshIndices;

    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pFailureMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pFailureMeshIndices;

    size_t m_stCurrentBeat;
    size_t m_stCurrentMesure;

    emp::gui::c_score_skin_s* m_pSkin;

    EMP_POINTER(label_c) m_pLabel;
    float m_fMinHeight;
    size_t m_stMinNoteCount;
    bool m_bComputeBeatSize;
    emp::cnt::vector<float> m_vfOffsets;
    emp::cnt::vector<float> m_vfIndices;

    float m_fScaleX;
    float m_fScaleY;

public:
    score_c(void);
    ~score_c(void);

public:
    void create(void);
    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

private:
    EMP_RETURN size_t get_current_index(c_size a_stIndex, c_float a_fDuration) const;
    EMP_RETURN label_c* add_label(c_size a_stAddress, c_u8string& a_sNote, c_float a_fScaleX, c_float a_fScaleY);
    EMP_RETURN float get_time_offset(c_size a_stIndex, c_float a_fDuration) const;

public:
    EMP_RETURN float time_to_size(c_float a_fTime) const { return a_fTime * compute_time_scale(); }
    EMP_RETURN float size_to_time(float const a_fSize) const { return a_fSize / compute_time_scale(); }

public:
    void reset(void);

    void set_start_size(float const a_fValue) { m_fStartSize = a_fValue; }

    void set_time(float const a_fValue) { m_fTime = a_fValue; }

    EMP_RETURN float start_duration(void) const { return size_to_time(m_fStartSize); }
    EMP_RETURN float start_time(void) const { return m_fTime - start_duration(); }

    EMP_RETURN float end_size(void) const { return transformation().width() - m_fStartSize; }
    EMP_RETURN float end_duration(void) const { return size_to_time(end_size()); }
    EMP_RETURN float end_time(void) const { return m_fTime + end_duration(); }

    EMP_RETURN float beat_duration(void) const { return 60.0f / m_fBeatPerMinute; }
    EMP_RETURN float mesure_duration(void) const { return beat_duration() * m_fBeatPerMesure; }

    void set_beat_per_minute(float const a_fValue);
    void set_beat_per_mesure(float const a_fValue);

    EMP_RETURN float compute_time_scale(void) const;
    EMP_RETURN float time_scale(void) const { return m_fTimeScale; }
    void set_time_scale(float const a_fValue);

    EMP_RETURN bool set_track(emp::snd::track_c const* const a_pValue);
    EMP_RETURN bool set_edition_track(emp::snd::track_c* const a_pValue);

    void set_interpretation(ase::cor::interpretation_c const* const a_pValue) { m_pInterpretation = a_pValue; }

    EMP_RETURN bool set_mapping(ase::cor::mapping_c const* const a_pValue)
    {
        m_pMapping = a_pValue;
        EMP_RET_FALSE_IF_NOT(compute_beat_size());
        return true;
    }

    void set_show_letters(c_bool a_bValue) { m_bShowLetters = a_bValue; }
    void set_show_durations(c_bool a_bValue) { m_bShowDurations = a_bValue; }
    void set_edition(c_bool a_bValue) { m_bEdition = a_bValue; }

public:
    void set_compute_beat_size(c_bool a_bValue) { m_bComputeBeatSize = a_bValue; }
    void set_min_height(c_float a_fValue) { m_fMinHeight = a_fValue; }
    void set_min_note_count(c_size a_stValue) { m_stMinNoteCount = a_stValue; }

    EMP_RETURN bool compute_beat_size(void);

public:
    EMP_RETURN float space_height(void) const;
    EMP_RETURN float note_height(void) const;

public:
    EMP_RETURN float scale_x(void) const;
    void set_scale_x(float const a_fValue);

    EMP_RETURN float scale_y(void) const;
    void set_scale_y(float const a_fValue);

public:
    void scale_up_x(void) { set_scale_x(scale_x() + 0.03f); }
    void scale_down_x(void) { set_scale_x(scale_x() - 0.03f); }

public:
    void scale_up_y(void) { set_scale_y(scale_y() + 0.03f); }
    void scale_down_y(void) { set_scale_y(scale_y() - 0.03f); }

public:
    void time_scale_up(void) { set_time_scale(time_scale() + 0.03f); }
    void time_scale_down(void) { set_time_scale(time_scale() - 0.03f); }

    void set_skin(emp::gui::score_skin_s const* const a_pValue);

public:
    EMP_RETURN size_t max_note(void) const;

public:
    EMP_RETURN size_t get_note(c_size a_stNote, v2f::tparam a_v2Position) const;
    EMP_RETURN v2f::tret get_note_position(c_size a_stNote) const;

    void remove_note(c_size a_stNote);
    EMP_RETURN size_t add_note(v2f::tparam a_v2Position);
    EMP_RETURN size_t move_note(c_size a_stNote, v2f::tparam a_v2Origin, v2f::tparam a_v2Position);
    EMP_RETURN size_t scale_note(c_size a_stNote, v2f::tparam a_v2Position);

private:
    void get_time_and_index(v2f::tparam a_v2Position,emp::tim::time_c& a_rTime, size_t& a_rstIndex) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

