//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_popup.h"

#include "emp_gui_manager.h"
#include "emp_gui_registrar_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_loc_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
popup_c::popup_c(void):
    toolbar_0(),
    toolbar_1(),
    window_0(),
    window_1()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
popup_c::~popup_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
popup_c& popup_c::create(registrar_i* const a_pRegistrar, pc_control_i a_pHook)
{
    destroy();

    create_toolbar(toolbar_0, window_0, a_pRegistrar, a_pHook);
    create_toolbar(toolbar_1, window_1, a_pRegistrar, a_pHook);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void popup_c::destroy(void)
{
    if (EMP_GUI_MANAGER != nullptr)
    {
        emp::tt::silent(EMP_GUI_MANAGER->try_remove_window(&window_0));
        emp::tt::silent(EMP_GUI_MANAGER->try_remove_window(&window_1));
    }

    toolbar_0.destroy();
    toolbar_1.destroy();

    popup = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
popup_c& popup_c::skin_(cpc_toolbar_skin_s a_pValue)
{
    window_0.skin_(a_pValue ? a_pValue->window : nullptr);
    window_1.skin_(a_pValue ? a_pValue->window : nullptr);
    toolbar_0.skin_(a_pValue);
    toolbar_1.skin_(a_pValue);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void popup_c::show(emp::mat::v2f::tparam a_v2Position, c_change_e a_eChange)
{
    if (popup != nullptr)
    { emp::tt::silent(EMP_GUI_MANAGER->hide_window(popup)); }
    if (popup == &window_0)
    { popup = &window_1; }
    else
    { popup = &window_0; }
    popup->transformation().position_(a_v2Position);
    emp::tt::silent(EMP_GUI_MANAGER->show_window(popup, true, true, a_eChange, false));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void popup_c::hide(void)
{ emp::tt::silent(EMP_GUI_MANAGER->hide_window(popup)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::v2f::tret popup_c::position(void) const
{ return popup->transformation().position; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
popup_c& popup_c::add_item(c_astring& a_sLocale, function_f const& a_rFunction)
{
    toolbar_0.add_item(emp::loc::string_s(a_sLocale), a_rFunction, nullptr);
    toolbar_1.add_item(emp::loc::string_s(a_sLocale), a_rFunction, nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void popup_c::create_toolbar(toolbar_c& a_rToolbar, window_c& a_rWindow, registrar_i* const a_pRegistrar, pc_control_i a_pHook)
{
    a_rToolbar.registrar_(a_pRegistrar).type_(dock_e::Top).state().index_(0).autosize.xy(true);

    a_rWindow.create();
    a_rWindow.state().index_(0).autosize.xy(true);
    a_rWindow.hook_(a_pHook);
    emp::tt::silent(a_rWindow.try_remove_control(&a_rToolbar));
    a_rWindow.add_control(&a_rToolbar);
    a_pRegistrar->register_window(&a_rWindow);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

