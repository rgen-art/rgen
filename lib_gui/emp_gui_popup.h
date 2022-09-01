//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_POPUP_H
#define EMP_GUI_POPUP_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_toolbar.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, popup_c)
private:
    typedef emp::fct::function_t<return_e (*)(size_t, bool, change_e)> function_f;

private:
    toolbar_c toolbar_0;
    toolbar_c toolbar_1;
    window_c window_0;
    window_c window_1;
    window_c* popup = nullptr;

public:
    popup_c(void);
    ~popup_c(void);

    popup_c& create(registrar_i* const a_pRegistrar, pc_control_i a_pHook);
    void destroy(void);

    void show(v2f::tparam a_v2Position, c_change_e a_eChange);
    void hide(void);

    popup_c& add_item(c_astring& a_sLocale, function_f const& a_rFunction);

    popup_c& skin_(toolbar_skin_s const* const a_pValue);

    EMP_RETURN v2f::tret position(void) const;

private:
    void create_toolbar(toolbar_c& a_rToolbar, window_c& a_rWindow, registrar_i* const a_pRegistrar, pc_control_i a_pHook);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

