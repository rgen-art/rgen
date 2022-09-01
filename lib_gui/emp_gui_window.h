//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_WINDOW_H
#define EMP_GUI_WINDOW_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_docker.h"
#include "emp_gui_window_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(window_skin_s)
public:
    EMP_FIELD(c_sizer_skin_s*, sizer, nullptr)
    EMP_FIELD(float, alpha, 0)
    EMP_FIELD(float, duration, 0)
    EMP_FIELD(float, speed, 1)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_INHERITS_VIA_DOMINANCE)
EMP_NOCOPY_BASE(class, window_c, public, window_i, public, docker_c)
private:
    friend class menu_c;
    friend class dialog_c;
    friend class manager_c;

public:
    EMP_FIELD(bool         , overlay , false  );
    EMP_FIELD(bool         , modal   , false  );
    EMP_FIELD(bool         , top     , false  );
    EMP_FIELD(c_control_i* , hook    , nullptr);

public:
#if defined EMP_XX_DEBUG
    size_t EMP_TT_MAX_VAR(m_stDebugId);
    virtual EMP_RETURN size_t debug_id(void) const { return m_stDebugId; }
#endif

private:
    v2f m_v2DragPosition{};
    v2f m_v2DragSize{};
    int m_siDragQuarter = 0;
    float m_fFadeTime = 0;
    float m_fFadeDirection = -1;
    float m_fFadeSpeed = 1;

    bool m_bShowing = false;
    bool m_bHiding = false;

    skin_data_t<window_skin_s> skin{};

public:
    window_c(void) = default;
    virtual ~window_c(void) {}

    EMP_RETURN bool post_load(void);

    void create(void);

public:
    //>> window_i
    EMP_RETURN bool is_showing(void) const { return m_bShowing; }
    EMP_RETURN bool is_hiding(void) const { return m_bHiding; }
    EMP_RETURN bool is_hidden(void) const { return fade_time() <= 0 && fade_direction() < 0; }
    EMP_RETURN bool is_modal(void) const { return modal; }
    EMP_RETURN bool is_top(void) const { return top; }
    EMP_RETURN bool is_floating(void) const { return hook != nullptr; }
    EMP_RETURN bool is_overlay(void) const { return overlay; }
    // << window_i

private:
    // >> window_i
    void show_imp(void)
    {
        m_fFadeDirection = 1.0f;
        m_bHiding = false;
        m_bShowing = true;
        state().visible_(true).enable_(true);
    }

    void hide_imp(void)
    {
        m_fFadeDirection = -1.0f;
        m_bHiding = true;
        m_bShowing = false;
    }
    //<< window_i

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool transform(stack_c& a_rStack);
    //<< control_i

public:
    EMP_RETURN float fade_time(void) const { return m_fFadeTime; }
    EMP_RETURN float fade_factor(void) const;
    EMP_RETURN float fade_direction(void) const { return m_fFadeDirection; }
    EMP_RETURN float fade_speed(void) const { return skin.data ? skin.data->speed : 1; }

    //void release(void);

public:
    window_c& skin_(cpc_window_skin_s a_pValue);
};
// EMP_PRAGMA_POP_IGNORE(EMP_W_INHERITS_VIA_DOMINANCE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

