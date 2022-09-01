//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_toolbar.h"

#include "emp_gui_button.h"
#include "emp_gui_icon.h"
#include "emp_gui_label.h"
#include "emp_gui_manager.h"
#include "emp_gui_registrar_i.h"
#include "emp_gui_skin.h"
#include "emp_gui_spacer.h"
#include "emp_gui_window.h"

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
toolbar_c::~toolbar_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void toolbar_c::destroy(void)
{
    emp::cnt::foreach(m_vpIcons, emp::mem::safe_delete_reset_t());
    m_vpIcons.resize(0);

    emp::cnt::foreach(states, emp::mem::safe_delete_reset_t());
    states.resize(0);

    // @@0  move in registrar?
    c_size stWindowCount = windows.count();
    if (stWindowCount)
    {
        // @@0  singleton
    // @@0  : remove global
        for (size_t stIndex = 0; stIndex < stWindowCount; ++stIndex)
        {
            window_c* const pWindow = windows.val(stIndex);
            if (pWindow)
            {
                emp::tt::silent(EMP_GUI_MANAGER->try_remove_window(pWindow));
                EMP_DEALLOCATE(pWindow);
            }
        }
        windows.resize(0);
    }

    toolbars.resize(0);
    controls().clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool toolbar_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_bool bAdjust = skin ? skin->adjust : false;

    float fMaxX = 0;
    float fMaxY = 0;

    c_size stButtonCount = m_vpIcons.count();
    for (size_t stIndex = 0; stIndex < stButtonCount; ++stIndex)
    {
        icon_c* const pIcon = m_vpIcons.val(stIndex);
        EMP_ASSERT(pIcon != nullptr);

        pIcon->state().autosize.xy(true);
        pIcon->label.state().autosize.x_(true);
        pIcon->splitter.state().autosize.xy(true);

        pIcon->autosize_label_x_(false);
        EMP_RET_FALSE_IF_NOT(pIcon->resize());
        pIcon->autosize_label_x_(autosize_label_x == false);

        pIcon->state().autosize.xy(false);

        fMaxX = emp::tt::maxof(fMaxX, pIcon->transformation().width());
        fMaxY = emp::tt::maxof(fMaxY, pIcon->transformation().height());
    }

    // @@0 spacer cpc_spacer_skin_s pSpacer = skin ? skin->spacer : nullptr;
    // @@0 spacing cpc_spacing_data_s pSpacing = pSpacer ? pSpacer->spacing : nullptr;
    c_float fSX = EMP_GUI_SKIN.spacing_x();//pSpacing ? pSpacing->size.x : 0;
    c_float fSY = EMP_GUI_SKIN.spacing_y();//pSpacing ? pSpacing->size.y : 0;

    fMaxX = emp::tt::maxof(fMaxX, (skin ? skin->min_width : 0) * EMP_GUI_SKIN.resolution.scaled_ratio().x);

    for (size_t stIndex = 0; stIndex < stButtonCount; ++stIndex)
    {
        icon_c* const pIcon = m_vpIcons.val(stIndex);
        if (bAdjust)
        { pIcon->transformation().width_(fMaxX); }
        pIcon->transformation().height_(fMaxY);
    }

    float fCurrent = 0; // @@0 glcoord

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();

    float fFinalWidth = 0;
    float fFinalHeight = 0;

    c_size stControlCount = control_count();
    for (size_t stIndex = 0; stIndex < stControlCount; ++stIndex)
    {
        size_t stOffset = stIndex;
        if (type == dock_e::Bottom ||
            type == dock_e::Right)
        { stOffset = stControlCount - stIndex - 1; }

        pc_control_i pControl = control(stOffset);
        EMP_ASSERT(pControl != nullptr);

        c_float fControlWidth = pControl->transformation().width();
        c_float fControlHeight = pControl->transformation().height();

        if (type == dock_e::Top)
        {
            pControl->transformation().position_(0, fCurrent);
            fCurrent -= fControlHeight;
        }
        else if (type == dock_e::Left)
        {
            pControl->transformation().position_(fCurrent, 0);
            fCurrent += fControlWidth;
        }
        else if (type == dock_e::Bottom)
        {
            fCurrent += fControlHeight;
            pControl->transformation().position_(0, fHeight - fCurrent);
        }
        else if (type == dock_e::Right)
        {
            fCurrent += fControlWidth;
            pControl->transformation().position_(fWidth - fCurrent, 0);
        }

        c_bool bLast = (stIndex + 1 >= stControlCount);
        c_bool bSpace = bLast == false && skin ? skin->spaced : false;
        c_float fSpaceX = bSpace ? fSX : 0.0f;
        c_float fSpaceY = bSpace ? fSY : 0.0f;

        if (type == dock_e::Left ||
            type == dock_e::Right)
        {
            fFinalWidth += fControlWidth + fSpaceX;
            fFinalHeight = emp::tt::maxof(fControlHeight, fFinalHeight);
            fCurrent += fSpaceX;
        }
        else if (
            type == dock_e::Top ||
            type == dock_e::Bottom)
        {
            fFinalWidth = emp::tt::maxof(fControlWidth, fFinalWidth);
            fFinalHeight += fControlHeight + fSpaceY;
            fCurrent += fSpaceY;
        }
    }

    if (state().autosize.x)
    { transformation().width_(fFinalWidth); }

    if (state().autosize.y)
    { transformation().height_(fFinalHeight); }

    if (emp::gui::focus_in_hierarchy(this) == false)
    {
        //EMP_LOG_INFO("REFRESH");
        // @@0  moveup in hierarchy?
        //set_visible(false);
    }

    for (size_t stIcon = 0; stIcon < stButtonCount; ++stIcon)
    {
        auto* const pIcon = m_vpIcons.val(stIcon);

        if (autosize_label_x)
        {
            pIcon->splitter.state().autosize.x_(true);
            pIcon->label.state().autosize.x_(true);
        }
        else
        {
            pIcon->splitter.state().autosize_(pIcon->state().autosize);
            pIcon->label.state().autosize.x_(pIcon->state().autosize.x);
        }
        EMP_RET_FALSE_IF_NOT(pIcon->resize());
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN size_t current_focus(emp::cnt::vector<icon_c*> const& a_rvpIcons);
EMP_INTERN EMP_RETURN size_t current_focus(emp::cnt::vector<icon_c*> const& a_rvpIcons)
{
    c_size stCount = a_rvpIcons.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        icon_c* const pLabelButton = a_rvpIcons.val(stIndex);
        EMP_ASSERT(pLabelButton != nullptr);
        if (focus_in_hierarchy(pLabelButton))
        { return stIndex; }
    }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN control_i* get_focus(emp::cnt::vector<icon_c*> const& a_rvpIcons, c_size a_stIndex);
EMP_INTERN EMP_RETURN control_i* get_focus(emp::cnt::vector<icon_c*> const& a_rvpIcons, c_size a_stIndex)
{
    c_size stCount = a_rvpIcons.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        icon_c* const pLabelButton = a_rvpIcons.val(stIndex);
        EMP_ASSERT(pLabelButton != nullptr);
        if (pLabelButton->state().index == a_stIndex)
        { return pLabelButton; }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool toolbar_c::set_selection(c_size a_stIndex, c_change_e a_eChange)
{
    pc_control_i pFocus = get_focus(m_vpIcons, a_stIndex);
    if (pFocus != nullptr)
    {
    // @@0  : remove global
        EMP_GUI_MANAGER->activate_focus(pFocus, focus_e::First, true, true, a_eChange);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN size_t get_next_selection(c_size a_stCurrent, c_size a_stCount);
EMP_INTERN EMP_RETURN size_t get_next_selection(c_size a_stCurrent, c_size a_stCount)
{ return emp::tt::is_not_max(a_stCurrent) ? (a_stCurrent + 1) % a_stCount : 0; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN size_t get_previous_selection(c_size a_stCurrent, c_size a_stCount);
EMP_INTERN EMP_RETURN size_t get_previous_selection(c_size a_stCurrent, c_size a_stCount)
{ return emp::tt::is_not_max(a_stCurrent) ? a_stCurrent > 0 ? a_stCurrent - 1 : a_stCount - 1 : a_stCount - 1; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e toolbar_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible && state().enable)
    {
        auto const& rKeyboard = a_rEvent.keyboard;
        if (rKeyboard.m_bPressed)
        {
            emp::app::c_key_e eKey = rKeyboard.m_eKey;
            if (eKey == emp::app::key_e::Escape)
            {
                if (on_escape.is_valid())
                {
                    if (on_escape(true, change_e::Keyboard) == return_e::Error)
                    { return EMP_GUI_RETURN(a_rEvent, return_e::Error); }
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else
            {
                c_size stCount = m_vpIcons.count();
                if (stCount > 0 &&
                    focus_in_hierarchy(this))
                {
                    c_size stCurrent = current_focus(m_vpIcons);
                    size_t EMP_TT_MAX_VAR(stNew);

                    if (rKeyboard.m_bCtrl == false &&
                        rKeyboard.m_bShift == false)
                    {
                        if (eKey == emp::app::key_e::Left ||
                            eKey == emp::app::key_e::Up)
                        { stNew = get_previous_selection(stCurrent, stCount); }
                        else if (
                            eKey == emp::app::key_e::Right ||
                            eKey == emp::app::key_e::Down)
                        { stNew = get_next_selection(stCurrent, stCount); }
                        else if (eKey == emp::app::key_e::Num_1)
                        { stNew = 0; }
                        else if (eKey == emp::app::key_e::Num_2)
                        { stNew = 1; }
                        else if (eKey == emp::app::key_e::Num_3)
                        { stNew = 2; }
                        else if (eKey == emp::app::key_e::Num_4)
                        { stNew = 3; }
                        else if (eKey == emp::app::key_e::Num_5)
                        { stNew = 4; }
                        else if (eKey == emp::app::key_e::Num_6)
                        { stNew = 5; }
                        else if (eKey == emp::app::key_e::Num_7)
                        { stNew = 6; }
                        else if (eKey == emp::app::key_e::Num_8)
                        { stNew = 7; }
                        else if (eKey == emp::app::key_e::Num_9)
                        { stNew = 8; }
                        else if (eKey == emp::app::key_e::Num_0)
                        { stNew = 9; }
                    }

                    if (emp::tt::is_not_max(stNew))
                    {
                        if (set_selection(stNew, change_e::Keyboard))
                        { return EMP_GUI_RETURN(a_rEvent, return_e::Consumed); }
                    }
                }
            }
        }
    }
    return tbase1::process_keyboard(a_rEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e toolbar_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    c_size stCount = m_vpIcons.count();
    if (stCount > 0 &&
        //focus_in_hierarchy(this) &&
        a_rEvent.keyboard.modified() == false)
    {
        change_e eChange = change_e::none;

        auto const& rMouse = a_rEvent.mouse;
        if (a_rEvent.mouse_pressed())
        {
            //m_stAbsTouchCount = emp::mat::maxof(m_stAbsTouchCount, rMouse.m_pmTouches->count());
            c_size stCurrent = current_focus(m_vpIcons);
            size_t EMP_TT_MAX_VAR(stNew);
            emp::app::c_button_e eButton = rMouse.m_eButton;
            if (eButton == emp::app::button_e::WheelUp)
            {
                stNew = get_previous_selection(stCurrent, stCount);
                eChange = change_e::Wheel;
            }
            else if (eButton == emp::app::button_e::WheelDown)
            {
                stNew = get_next_selection(stCurrent, stCount);
                eChange = change_e::Wheel;
            }

            if (emp::tt::is_not_max(stNew))
            {
                if (set_selection(stNew, eChange))
                { return EMP_GUI_RETURN(a_rEvent, return_e::Consumed); }
            }
        }
        else if (a_rEvent.mouse_dragged())
        {
            //m_stAbsTouchCount = emp::mat::maxof(m_stAbsTouchCount, rMouse.m_pmTouches->count());
            //EMP_LOG_INFO("DX #0, DY #1, D #2", rMouse.deltak().x, rMouse.deltak().y, v2f::length(rMouse.deltak()));
            if (rMouse.m_pmTouches->count() == 2 && changed)
            {
                for (auto it = rMouse.m_pmTouches->begin(); it != rMouse.m_pmTouches->end(); ++it)
                {
                    if (v2f::length(it.value()->mouse.distance()) < 0.01f)
                    { return return_e::Unused; }
                }
                changed = false;

                c_size stCurrent = current_focus(m_vpIcons);
                size_t EMP_TT_MAX_VAR(stNew);
                c_float fDeltaX = rMouse.distance().x;
                if (fDeltaX < 0)
                {
                    stNew = get_previous_selection(stCurrent, stCount);
                    EMP_LOG_INFO("DELTAX #0", fDeltaX);
                }
                else if (fDeltaX > 0)
                {
                    stNew = get_next_selection(stCurrent, stCount);
                    EMP_LOG_INFO("DELTAX #0", fDeltaX);
                }

                if (emp::tt::is_not_max(stNew))
                {
                    if (set_selection(stNew, change_e::Keyboard))
                    { return EMP_GUI_RETURN(a_rEvent, return_e::Consumed); }
                }
            }
        }
        else if (a_rEvent.mouse_released())
        {
            //c_size stAbsCount = m_stAbsTouchCount;
            if ((rMouse.m_pmTouches->count() > 0) == false)
            {
                //m_stAbsTouchCount = 0;
            }

            if (rMouse.m_stToucheCount > 1) // @@0 wtf
            {
                if (rMouse.m_pmTouches->count() < 2) // @@0 wtf
                {
                    changed = true;
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }

        }
    }
    return tbase1::process_mouse(a_rEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void toolbar_c::add_item(emp::loc::string_s const& a_eLabel,
    function_t<return_e (*)(c_size, c_bool, c_change_e)> const& a_rFunction, toolbar_c* const a_pToolbar)
{ add_item(a_eLabel, a_rFunction, a_pToolbar, m_vpIcons.count()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void toolbar_c::add_item(emp::loc::string_s const& a_eLabel,
    emp::fct::function_t<return_e (*)(c_size, c_bool, c_change_e)> const& a_rFunction,
    toolbar_c* const a_pToolbar, c_size a_stFocusIndex)
{
    // @@0  lib_str allocation
    icon_c* const pIcon = EMP_ALLOCATE(icon_c);

    pIcon->skin_(skin ? skin->icon : nullptr);
    pIcon->state().focusable_(true).navigable_(button_navigable).index_(emp::ast::cast<ui32>(a_stFocusIndex));
    pIcon->label.set_text(a_eLabel);
    pIcon->push_on_focus_(push_on_focus);
    pIcon->on_push.function_(emp::fct::function(this, &toolbar_c::on_item_pushed));
    m_vpIcons.add(pIcon);
    add_control(pIcon, this);

    typedef state_t<bool, size_t> state_d;
    state_d* const pState = EMP_ALLOCATE(state_d);
    pState->reset(a_stFocusIndex, false);
    pState->function_(a_rFunction);
    states.add(pState);

    if (a_pToolbar)
    {
        toolbars.add(a_pToolbar);

        // @@0  lib_str allocation
        window_c* pWindow = EMP_ALLOCATE(window_c);
        pWindow->create();
        pWindow->skin_(skin ? skin->window : nullptr);
        pWindow->state().index_(emp::ast::cast<ui32>(windows.count())).autosize.xy(true);
        pWindow->add_control(a_pToolbar);
        registrar->register_window(pWindow);
        windows.add(pWindow);
    }
    else
    {
        toolbars.add(nullptr);
        windows.add(nullptr);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
toolbar_c& toolbar_c::skin_(cpc_toolbar_skin_s a_pValue)
{
    skin = a_pValue;
    {
        c_size stCount = m_vpIcons.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpIcons.val(stIndex)->skin_(a_pValue ? a_pValue->icon : nullptr); }
    }

    {
        c_size stCount = windows.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            window_c* const pWindow = windows.val(stIndex);
            if (pWindow)
            { pWindow->skin_(a_pValue ? a_pValue->window : nullptr); }
        }
    }
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
return_e toolbar_c::on_item_pushed(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange)
{
    EMP_ASSERT(a_pSender != nullptr);

    size_t EMP_TT_MAX_VAR(stIcon);
    
    c_size stCount = m_vpIcons.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        icon_c const* const pIcon = m_vpIcons.val(stIndex);
        if (control_in_hierarchy(pIcon, a_pSender))
        {
            stIcon = stIndex;
            break;
        }
    }

    icon_c const* const pIcon = m_vpIcons.val(stIcon);
    state_t<bool, size_t>* const pState = states.val(stIcon);
    window_c* const pWindow = windows.val(stIcon);
    toolbar_c* const pToolbar = toolbars.val(stIcon);

    if (pWindow != nullptr && pToolbar->controls().count() > 0)
    {
        pWindow->hook_(pIcon);
        emp::tt::silent(EMP_GUI_MANAGER->show_window(pWindow, true, true, a_eChange, false));

        if (pState != nullptr &&
            pState->is_valid())
        { emp::tt::silent(pState->set_state(stIcon, a_bValue, a_eChange)); }

        //auto const* const pSpacing = skin->spacer->spacing;
        c_float fSX = EMP_GUI_SKIN.spacing_x();//pSpacing->x();
        c_float fSY = EMP_GUI_SKIN.spacing_y();//pSpacing->y();

        c_transformation_data_s& rTransform = pIcon->transformation();
        float fX = -1; // @@0 glcoord
        float fY = 1; // @@0 glcoord
        if (type == dock_e::Left)
        {
            fX = 0;
            fY = fSY;
        }
        else if (type == dock_e::Right)
        {
            fX = rTransform.width() - pToolbar->transformation().width();
            fY = fSY;
        }
        else if (type == dock_e::Top)
        {
           fX = rTransform.width() + fSX;
           fY = 0;
        }
        else if (type == dock_e::Bottom)
        {
           fX = rTransform.width() + fSX;
           fY = 0;
        }

        pWindow->transformation().position_(fX, fY);
        return emp::gui::return_e::Consumed;
    }
    else if (pState != nullptr)
    {
        if (a_bValue)
        {}
        else
        {
            emp::tt::silent(pState->set_state(stIcon, true, a_eChange));
            emp::tt::silent(pState->set_state(stIcon, false,a_eChange));
        }
        return emp::gui::return_e::Consumed;
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

