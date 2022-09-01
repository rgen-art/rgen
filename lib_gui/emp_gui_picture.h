//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_PICTURE_H
#define EMP_GUI_PICTURE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { EMP_FORWARD(class, model_c) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(struct, button_skin_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(picture_skin_s)
public:
    EMP_FIELD( state_skin_t<c_color_s>      , enabled   ,{}          )
    EMP_FIELD( state_skin_t<c_color_s>      , disabled   ,{}         )
    EMP_FIELD( c_button_skin_s*             , button    , nullptr )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, picture_c, public, container_c)
public:
    state_t<bool, c_control_i*> on_push{};
    EMP_FIELD( bool                         , autoscale , false   )
    EMP_FIELD( float                        , size      , 0.02f   )

public:
    EMP_FIELD(bool                , push_on_focus         , false     )
    EMP_FIELD(bool                , push_on_repeat        , false     )
    EMP_FIELD(bool                , push_on_double_click  , false     )
    EMP_FIELD(emp::gfx::c_model_c*, model                 , nullptr   )
    EMP_FIELD(alignment_e         , alignment             , alignment_e::MiddleCenter);

private:
    alignment_e m_eAlignment{};
    m44f m_mModel{};
    c_picture_skin_s* skin = nullptr;
    v4f m_v4Color{};
    v2f m_v2Size{};
    float m_fSize{};
    
public:
    picture_c(void);
    virtual ~picture_c(void);

public:
    //>> control_i
    EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s&);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s&);
    //<< handler_i

public:
    picture_c& skin_(cpc_picture_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

