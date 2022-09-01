//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_BUTTON_H
#define EMP_GUI_BUTTON_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(button_skin_s)
public:
    EMP_FIELD(c_container_skin_s*, container, nullptr);
    EMP_FIELD(c_padding_data_s*  , padding  , nullptr);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, button_c, public, container_c)
public:
    EMP_FIELD(bool        , push_on_focus          , false )
    EMP_FIELD(bool        , push_on_repeat         , false )
    EMP_FIELD(bool        , push_on_double_click   , false )
    EMP_FIELD(float       , scale                  , 1.0f  )
    EMP_FIELD(alignment_e , alignment              , alignment_e::MiddleCenter);


    state_t<bool, c_control_i*> on_push{};

private:
    control_i* control = nullptr;
    skin_data_t<emp::gui::button_skin_s> skin{};
    
public:
    button_c(void);
    virtual ~button_c(void) {}

    EMP_RETURN bool post_load(void);

public:
    //>> control_i
    virtual EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);
    virtual EMP_RETURN bool resize(void);
    //<< control_i

    //>> handler_i
    virtual EMP_RETURN return_e process_keyboard(emp::app::c_event_s&);
    virtual EMP_RETURN return_e process_mouse(emp::app::c_event_s&);
    //<< handler_i

public:
    button_c& control_(pc_control_i a_pValue)
    {
        clear_controls();
        add_control(a_pValue, this);
        control = a_pValue;
        return *this;
    }

public:
    button_c& skin_(cpc_button_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

