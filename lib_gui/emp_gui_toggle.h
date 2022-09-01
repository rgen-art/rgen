//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_TOGGLE_H
#define EMP_GUI_TOGGLE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_icon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(toggle_skin_s)
public:
    EMP_FIELD(c_icon_skin_s*           , icon    , nullptr)
    EMP_FIELD(emp::gfx::model_c const* , gfx_on  , nullptr)
    EMP_FIELD(emp::gfx::model_c const* , gfx_off , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,toggle_c, public, icon_c)
public:
    state_t<bool, toggle_c const*> on_check{};

private:
    emp::rfx::type_i* reflection = nullptr;
    emp::loc::string_s* text = nullptr;
    c_toggle_skin_s* skin = nullptr;

public:
    toggle_c(void);
    virtual ~toggle_c(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

private:
    EMP_RETURN bool get_value(void) const;

    void refresh_icon(c_bool a_bValue);

public:
    void set_check(c_bool a_bValue, c_change_e a_eChange);
    void set_text(emp::rfx::type_i* const a_pValue) { reflection = a_pValue; }
    void set_text(emp::loc::string_s const& a_rValue);
public:
    EMP_RETURN return_e on_button_pushed(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);

public:
    toggle_c& skin_(cpc_toggle_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

