//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_COMBO_H
#define EMP_GUI_COMBO_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_list.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, combo_c, public, list_c)
public:
    state_t<bool, combo_c const*> on_edit{};
    state_t<bool, combo_c const*> on_valid{};
    state_t<bool, combo_c const*> on_cancel{};

private:
    emp::rfx::enum_i* reflection = nullptr;

    ui32 EMP_TT_MAX_VAR(selection);
    
public:
    combo_c(void);
    virtual ~combo_c(void);

public:
    //>> control_i
    EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void set_on_edit(c_bool a_bValue, c_change_e a_eChange);
    void set_on_valid(c_bool a_bValue, c_change_e a_eChange);
    void set_on_cancel(c_bool a_bValue, c_change_e a_eChange);

public:
    void set_reflection(emp::rfx::enum_i* const a_pValue);
    combo_c& skin_(cpc_list_skin_c a_pValue);

private:
    EMP_RETURN bool on_selection_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);
    EMP_RETURN bool on_selection_double_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);
    EMP_RETURN return_e on_item_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

