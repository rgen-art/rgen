//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_menu.h"

#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::fct;
using namespace emp::tt;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
menu_c::menu_c(void)
{
    toolbar.push_on_focus_(true).autosize_label_x_(true)
        .state().dock_(emp::gui::dock_e::Fill).index_(0).navigable_(true).autosize.xy(true);

    button_tb.control_(&toolbar).alignment_(alignment_e::MiddleLeft)
        .state().dock_(emp::gui::dock_e::none).index_(0).navigable_(true).autosize.xy(true, true);

    button_m.alignment_(alignment_e::MiddleCenter)
        .state().dock_(emp::gui::dock_e::none).index_(0).navigable_(true).autosize.xy(true, false);

    splitter_h.slave_(&button_tb).master_(&button_m).orientation_(dock_e::Right)
        .state().index_(0).autosize.xy(true);

    splitter_h.redock();

    docker.state().dock_(dock_e::Fill);
    
    splitter_v.master_(&splitter_h).slave_(&docker).orientation_(dock_e::Top)
        .state().dock_(dock_e::Fill).index_(0);

    splitter_v.redock();

    modal_(true).top_(true);
    transformation().position_(0).size_(2);
    state().index_(0).autosize.xy(false, false);
    add_control(&splitter_v);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
menu_c::~menu_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool menu_c::resize(void)
{
    if (state().visible)
    {
        splitter_v.state().autosize = state().autosize;
        splitter_h.state().autosize = { false, true };//state().autosize;
        EMP_RET_FALSE_IF_NOT(tbase1::resize());
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e menu_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    return_e const eReturn = tbase2::process_mouse(a_rEvent);
    if (eReturn != return_e::Unused)
    { return EMP_GUI_RETURN(a_rEvent, eReturn); }

    if (state().visible &&
        a_rEvent.mouse_pressed())
    {
        auto const& rMouse = a_rEvent.mouse;
        if (rMouse.m_eButton == emp::app::button_e::Left &&
            rMouse.m_stToucheCount == 1)
        {
            EMP_RET_X_IF_NOT(return_e::Error, EMP_GUI_MANAGER->hide_window(this));
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    /* @@0 remove if (state().visible() &&
        a_rEvent.mouse.m_stMaxTouches == 2 &&
        a_rEvent.mouse.m_eButton == emp::app::button_e::Left)
    {
        if (a_rEvent.mouse_pressed())
        { m_bPressed = true; }
        else if (a_rEvent.mouse_released() && m_bPressed)
        {
            m_bPressed = false;
            if (eEMP_GUI_MANAGER->hide_window(this) == false)
            { return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Error); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (a_rEvent.mouse_dragged())
        { m_bPressed = false;  }
    }*/

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void menu_c::center(void)
{ transformation().position_(v2f::mul(v2f::sub(v2f::all(2), transformation().size), v2f::all(0.5f))); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
menu_c& menu_c::user_(pc_control_i a_pValue)
{
    button_m.control_(a_pValue);
    splitter_h.master_(&button_m).redock();
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
menu_c& menu_c::skin_(cpc_menu_skin_s a_pValue)
{
    tbase1::skin_(a_pValue ? a_pValue->window : nullptr);
    toolbar.skin_(a_pValue ? a_pValue->toolbar : nullptr);
    button_tb.skin_(a_pValue ? a_pValue->button_tb : nullptr);
    button_m.skin_(a_pValue ? a_pValue->button_m : nullptr);
    splitter_h.skin_(a_pValue ? a_pValue->splitter_h : nullptr);
    splitter_v.skin_(a_pValue ? a_pValue->splitter_v : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

