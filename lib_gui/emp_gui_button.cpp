//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_button.h"

#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
button_c::button_c(void)
{
    on_push.reset(this, false);
    child_(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool button_c::post_load(void)
{
    skin.data = EMP_GUI_SKIN.button.get(skin.name);
    // @@0 uncomment EMP_RET_FALSE_IF(skin.data == nullptr, "Failed to load skin '#0'", skin.name);
    EMP_RET_FALSE_IF_NOT(control_count() > 0, "Inner control not found");

    control = tbase1::control(0);

    skin_(skin.data);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* button_c::got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
{
    c_bool bFocusable = state().focusable;
    if (bFocusable)
    {
        if (push_on_focus && a_bPush)
        {
            if (on_push.is_valid())
            {
                emp::tt::silent(on_push.set_state(true, a_eChange));
                emp::tt::silent(on_push.set_state(false, a_eChange));
            }
        }
    }

    return tbase1::got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool button_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    cpc_padding_data_s pPadding = skin.data != nullptr ? skin.data->padding : nullptr;

    c_autosize_data_s& rAutosize = state().autosize;
    c_autosize_data_s* pControlAutosize = control ? &control->state().autosize : nullptr;

    c_bool bAutosizeX = rAutosize.x;
    c_float fPaddingX = pPadding != nullptr ? pPadding->x() * scale : 0;
    if (bAutosizeX == false && (pControlAutosize == nullptr || pControlAutosize->x == false))
    { control->transformation().width_(transformation().width() - fPaddingX); }

    c_bool bAutosizeY = rAutosize.y;
    c_float fPaddingY = pPadding != nullptr ? pPadding->y() * scale : 0;
    if (bAutosizeY == false && (pControlAutosize == nullptr || pControlAutosize->y == false))
    { control->transformation().height_(transformation().height() - fPaddingY); }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    c_float fPaddingLeft = pPadding != nullptr ? pPadding->left() * scale : 0;
    c_float fPaddingRight = pPadding != nullptr ? pPadding->right() * scale : 0;
    c_float fPaddingTop = pPadding != nullptr ? pPadding->top() * scale : 0;
    c_float fPaddingBottom = pPadding != nullptr ? pPadding->bottom() * scale : 0;

    c_float fControlWidth = control ? control->transformation().width() : 0;
    c_float fControlHeight = control ? control->transformation().height() : 0;

    if (bAutosizeX)
    { transformation().width_(fControlWidth + fPaddingX); }
    if (bAutosizeY)
    { transformation().height_(fControlHeight + fPaddingY); }

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();

    c_bool bLeft =
        alignment == alignment_e::TopLeft ||
        alignment == alignment_e::MiddleLeft ||
        alignment == alignment_e::BottomLeft;

    c_bool bCenter =
        alignment == alignment_e::TopCenter ||
        alignment == alignment_e::MiddleCenter ||
        alignment == alignment_e::BottomCenter;

    c_bool bRight =
        alignment == alignment_e::TopRight ||
        alignment == alignment_e::MiddleRight ||
        alignment == alignment_e::BottomRight;

    c_bool bTop =
        alignment == alignment_e::TopLeft ||
        alignment == alignment_e::TopCenter ||
        alignment == alignment_e::TopRight;

    c_bool bMiddle =
        alignment == alignment_e::MiddleLeft ||
        alignment == alignment_e::MiddleCenter ||
        alignment == alignment_e::MiddleRight;

    c_bool bBottom =
        alignment == alignment_e::BottomLeft ||
        alignment == alignment_e::BottomCenter ||
        alignment == alignment_e::BottomRight;

    float fOffsetX = 0; // @@0 glcoord
    if (bLeft)
    { fOffsetX += fPaddingLeft; }
    else if (bCenter)
    { fOffsetX += fPaddingLeft + (fWidth - fPaddingX - fControlWidth) * 0.5f; }
    else if (bRight)
    { fOffsetX = fWidth - fPaddingRight - fControlWidth; }

    float fOffsetY = 0; // @@0 glcoord
    if (bTop)
    { fOffsetY -= fHeight - fPaddingTop - fControlHeight; }
    else if (bMiddle)
    { fOffsetY -= (fPaddingTop + (fHeight - fPaddingY - fControlHeight) * 0.5f); }
    else if (bBottom)
    { fOffsetY -= fPaddingBottom; }

    if (control)
    { control->transformation().position_(fOffsetX, fOffsetY); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e button_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    emp::app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;

    if (state().enable &&
        rKeyboard.modified() == false &&
        (push_on_repeat || rKeyboard.m_bRepeat == false))
    {
        emp::app::c_key_e eKey = rKeyboard.m_eKey;
        if (eKey == emp::app::key_e::Enter ||
            eKey == emp::app::key_e::Space)
        {
            if (focus_in_hierarchy(this))
            {
                EMP_GUI_MANAGER->activate_focus(this, focus_e::First, true, false, change_e::Keyboard);

                c_bool bPressed = rKeyboard.m_bPressed;
                if ((bPressed || on_push.value) && on_push.is_valid())
                {
                    return on_push.set_state(bPressed, change_e::Keyboard);
                }
            }
        }
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e button_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    c_change_e eChange = change_e::Mouse;

    if (state().hover &&
        state().enable &&
        parent_enable(this))
    {
        emp::app::c_mouse_event_s& rMouse = a_rEvent.mouse;
        emp::app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;
        if (rKeyboard.m_bShift == false &&
            rKeyboard.m_bCtrl == false &&
            rMouse.m_eButton == emp::app::button_e::Left)
        {
            if (a_rEvent.mouse_pressed())
            {
                if (focus_in_hierarchy(this) == false)
                { EMP_GUI_MANAGER->activate_focus(this, focus_e::First, true, false, eChange); }

                if (push_on_double_click || rMouse.m_stTapCount == 1)
                {
                    if (on_push.is_valid())
                    { return on_push.set_state(true, eChange); }
                }
            }
            else if (a_rEvent.mouse_released())
            {
                if (on_push.value)
                {
                    if (push_on_double_click || rMouse.m_stTapCount == 1)
                    {
                        if (on_push.is_valid())
                        { return on_push.set_state(false, eChange); }
                    }
                }
            }
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
button_c& button_c::skin_(cpc_button_skin_s a_pValue)
{
    skin.data = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

