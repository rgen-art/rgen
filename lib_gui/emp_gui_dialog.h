//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_DIALOG_H
#define EMP_GUI_DIALOG_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_resolution.h"
#include "emp_gui_splitter.h"
#include "emp_gui_toolbar.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name dialog_e
#define enum_values\
    enum_value( none        )\
    enum_value( Ok          )\
    enum_value( OkCancel    )\
    enum_value( YesNo       )\
    enum_value( YesNoCancel )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(dialog_skin_s)
public:
    EMP_FIELD(c_window_skin_s*     , window    , nullptr)
    EMP_FIELD(c_splitter_skin_s*   , splitter  , nullptr)
    EMP_FIELD(c_toolbar_skin_s*    , toolbar   , nullptr)
    EMP_FIELD(c_sizer_skin_s*      , sizer     , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_INHERITS_VIA_DOMINANCE)
EMP_NOCOPY_BASE(class, dialog_c, public, window_i, public, resolution_c)
public:
    typedef emp::fct::function_t<return_e (*)(c_bool, change_e)> on_push_d;

    EMP_FIELD(on_push_d, on_ok      ,{})
    EMP_FIELD(on_push_d, on_cancel  ,{})
    EMP_FIELD(on_push_d, on_yes     ,{})
    EMP_FIELD(on_push_d, on_no      ,{})

private:
    window_c window{};
    splitter_c splitter{};
    toolbar_c toolbar{};

    control_i* control = nullptr;

    dialog_e m_eType = dialog_e::none;


    emp::cnt::vector<on_push_d*> on_push{};

    c_dialog_skin_s* skin = nullptr;

public:
    dialog_c(void);
    virtual ~dialog_c(void);

public:
    //>> window_i
    EMP_RETURN bool is_showing  (void) const { return window.is_showing(); }
    EMP_RETURN bool is_hiding   (void) const { return window.is_hiding(); }
    EMP_RETURN bool is_hidden   (void) const { return window.is_hidden(); }
    EMP_RETURN bool is_modal    (void) const { return true; }
    EMP_RETURN bool is_top      (void) const { return window.is_top(); }
    EMP_RETURN bool is_floating (void) const { return window.is_floating(); }
    EMP_RETURN bool is_overlay  (void) const { return window.is_overlay(); }
    EMP_RETURN float fade_factor(void) const { return window.fade_factor(); }
    EMP_RETURN float fade_speed (void) const { return window.fade_speed(); }
    //<< window_i

private:
    //>> window_i
    void show_imp(void) { window.show_imp(); }
    void hide_imp(void) { window.hide_imp(); }
    //<< window_i

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    dialog_c& user_control_(pc_control_i a_pControl);
    dialog_c& skin_(cpc_dialog_skin_s a_pValue);
    dialog_c& type_(c_dialog_e a_eValue);
    EMP_RETURN dialog_e type(void) const { return m_eType; }

    void center(void);

    EMP_RETURN return_e button_pushed(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
};
// EMP_PRAGMA_POP_IGNORE(EMP_W_INHERITS_VIA_DOMINANCE)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

