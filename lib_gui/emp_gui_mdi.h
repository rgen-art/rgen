//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_MDI_H
#define EMP_GUI_MDI_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_panel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app { EMP_FORWARD(struct, mouse_event_s) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name control_state_e
#define enum_values\
    enum_value( none   )\
    enum_value( Move   )\
    enum_value( Resize )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, mdi_c, public, panel_c)
public:
    control_i* dragged = nullptr;
    EMP_FIELD(bool, automove, true);
    
private:
    v2f drag_position{};
    v2f drag_size{};
    size_t drag_quarter = 0;
    c_panel_skin_s* skin = nullptr;
    bool force = false;
    bool restart = false;

    control_state_e control_state = control_state_e::none;

public:
    mdi_c(void);
    virtual ~mdi_c(void);

public:
    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    mdi_c& skin_(cpc_panel_skin_s a_pValue);

private:
    EMP_RETURN return_e handle(emp::app::c_event_s& a_rEvent, c_bool a_bValue);
    void handle_move_resize_start(emp::app::c_mouse_event_s& a_rMouse);
    EMP_RETURN return_e handle_move_resize(emp::app::c_event_s& a_rEvent, pc_control_i a_pControl, bool* const a_pbPop, c_bool a_bForce);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

