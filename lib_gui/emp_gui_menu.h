//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_MENU_H
#define EMP_GUI_MENU_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_button.h"
#include "emp_gui_splitter.h"
#include "emp_gui_toolbar.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(menu_skin_s)
public:
    EMP_FIELD(c_window_skin_s*    , window    , nullptr);
    EMP_FIELD(c_toolbar_skin_s*   , toolbar   , nullptr);
    EMP_FIELD(c_button_skin_s*    , button_tb , nullptr);
    EMP_FIELD(c_button_skin_s*    , button_m  , nullptr);
    EMP_FIELD(c_splitter_skin_s*  , splitter_h, nullptr);
    EMP_FIELD(c_splitter_skin_s*  , splitter_v, nullptr);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_INHERITS_VIA_DOMINANCE)
EMP_NOCOPY_BASE(class, menu_c, public, window_c)
public:
    toolbar_c toolbar{};
    docker_c docker{};

private:
    splitter_c splitter_h{};
    splitter_c splitter_v{};

    button_c button_tb{};
    button_c button_m{};

    c_menu_skin_s* skin = nullptr;

public:
    menu_c(void);
    virtual ~menu_c(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void center(void);

public:
    menu_c& user_(pc_control_i a_pValue);
    menu_c& skin_(cpc_menu_skin_s a_pValue);
};
// EMP_PRAGMA_POP_IGNORE(EMP_W_INHERITS_VIA_DOMINANCE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

