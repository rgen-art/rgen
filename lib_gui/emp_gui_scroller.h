//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_SCROLLER_H
#define EMP_GUI_SCROLLER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"
#include "emp_gui_picture.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(scroller_skin_s)
public:
    EMP_FIELD( c_container_skin_s*      , container      , nullptr )
    EMP_FIELD( c_container_skin_s*      , center         , nullptr )
    EMP_FIELD( c_picture_skin_s*        , picture_center        , nullptr )
    EMP_FIELD( c_picture_skin_s*        , picture_icon        , nullptr )
    EMP_FIELD( emp::gfx::c_model_c*     , gfx_up         , nullptr )
    EMP_FIELD( emp::gfx::c_model_c*     , gfx_down       , nullptr )
    EMP_FIELD( emp::gfx::c_model_c*     , gfx_left       , nullptr )
    EMP_FIELD( emp::gfx::c_model_c*     , gfx_right      , nullptr )
    EMP_FIELD( emp::gfx::c_model_c*     , gfx_scroll     , nullptr )
    //EMP_FIELD( c_spacing_data_s*        , spacing        , nullptr )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, scroller_c, public, container_c)
public:
    EMP_FIELD(float, sum, 0.0f);
    EMP_FIELD(float, page, 0.0f);

private:
    container_c center;

    picture_c up;
    picture_c down;
    picture_c scroll;

    size_t focus_index = 0;

    float m_fCurrent = 0.0f;

    bool m_bVertical = false;

    bool dragged = false;
    float drag = 0.0f;
    float position = 0.0f;
    float m_fOffset = 0.0f;

    c_scroller_skin_s* skin = nullptr;

public:
    scroller_c(void);
    virtual ~scroller_c(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void scroll_up(void) { offset_(offset() - 0.05f); }
    void scroll_down(void) { offset_(offset() + 0.05f); }

public:
    EMP_RETURN float current(void) const { return m_fCurrent; }
    scroller_c& current_(c_float a_fValue);

    EMP_RETURN bool vertical(void) const { return m_bVertical; }
    scroller_c& vertical_(c_bool a_bValue);

    EMP_RETURN float offset(void) const;
    scroller_c& offset_(c_float a_fValue);

public:
    EMP_RETURN float offset_max(void) const;

public:
    scroller_c& skin_(cpc_scroller_skin_s a_pValue);

private:
    EMP_RETURN float scroll_max(void) const;
    EMP_RETURN float scroll_ratio(void) const;

    EMP_RETURN return_e button_up_pushed(cpc_control_i  a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e button_down_pushed(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e button_scroll_pushed(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

