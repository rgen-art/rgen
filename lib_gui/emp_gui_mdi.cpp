//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_mdi.h"

#include "emp_gui_manager.h"
#include "emp_gui_scroller.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mdi_c::mdi_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mdi_c::~mdi_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e mdi_c::handle_move_resize(
    emp::app::c_event_s& a_rEvent,
    pc_control_i a_pControl,
    bool* const a_pbPop,
    c_bool a_bForce)
{
    auto const& rMouse = a_rEvent.mouse;
    c_bool bPressed = a_rEvent.mouse_pressed();
    c_bool bReleased = a_rEvent.mouse_released();
    c_bool bContains = a_pControl->state().hover;

    emp::app::c_button_e eButton = rMouse.m_eButton;

    if (bPressed)
    {
        if (a_pbPop != nullptr)
        { *a_pbPop = bContains; }
    }

    if (eButton == emp::app::button_e::Left)
    {
        emp::app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;
        c_bool bCtrl = rKeyboard.m_bCtrl;
        c_bool bShift = rKeyboard.m_bShift;

        if (bPressed)
        {
            if (a_bForce)
            {
                force = true;
                if (bShift == false && bCtrl == false)
                {
                    restart = true;
                    control_state = control_state_e::Move;
                }
            }
            dragged = bContains ? a_pControl : nullptr;
        }
        else if (bReleased)
        {
            if (a_bForce && force && bShift == false && bCtrl == false)
            {
                //EMP_LOG_INFO("ENONE 3");
                drag_position = emp::mat::v2f::zero();
                drag_size = emp::mat::v2f::zero();
                control_state = control_state_e::none;
                force = false;
            }
            dragged = nullptr;
        }
    }

    if (dragged != nullptr)
    {
        if (restart)
        {
            //EMP_LOG_INFO("______________________RESTART");
            handle_move_resize_start(rMouse);
            restart = false;
        }

        c_v2f v2MousePosition = rMouse.m_v2fPosition;
        //c_v2f v2RelPosition = { 1 + v2MousePosition.x, 2 - (1 + v2MousePosition.y) };

        transformation_data_s& rTransform = a_pControl->transformation();

        if (control_state == control_state_e::Move)
        {
            //EMP_LOG_INFO("MOUSE MOVE");
            rTransform.position_(emp::mat::v2f::sub(v2MousePosition, drag_position));
            /*float const x = emp::mat::v2f::get_x(rTransform.position());
            float const y = emp::mat::v2f::get_y(rTransform.position());
            float const w = emp::mat::v2f::get_x(rTransform.size());
            float const h = emp::mat::v2f::get_y(rTransform.size());

            if (w <= 0 || h <= 0)
            {
                int i = 0;
                ++i;
            }*/

            //EMP_LOG_INFO("MOVE : X #0, Y #1, W #2, H#3", x, y, w, h);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (control_state == control_state_e::Resize)
        {
            //EMP_LOG_INFO("MOUSE RESIZE");
            c_v2f v2Position = v2f::sub(v2MousePosition, rTransform.position);
            c_v2f v2DeltaSize = v2f::sub(v2f::sub(v2Position, drag_size), rTransform.size);

            c_float fDeltaX = v2DeltaSize.x;
            c_float fDeltaY = v2DeltaSize.y;

            v2f v2ClampedNewSize = v2f::zero();
            v2f v2ClampedDeltaPosition = v2f::zero();

            c_v2f v2Min = v2f::all(0.1f);
            c_v2f& v2Size = rTransform.size;

            if      (drag_quarter == 0) { return return_e::Unused; }
            else if (drag_quarter == 1) { v2ClampedNewSize = v2f::max(v2Min, v2f::sub(v2Size, v2DeltaSize)); }
            else if (drag_quarter == 2) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, { fDeltaX, -fDeltaY })); }
            else if (drag_quarter == 3) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, { -fDeltaX, fDeltaY })); }
            else if (drag_quarter == 4) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, v2DeltaSize)); }
            else if (drag_quarter == 5) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, { 0, -fDeltaY })); }
            else if (drag_quarter == 6) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, { 0, fDeltaY })); }
            else if (drag_quarter == 7) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, { -fDeltaX, 0 })); }
            else if (drag_quarter == 8) { v2ClampedNewSize = v2f::max(v2Min, v2f::add(v2Size, { fDeltaX, 0 })); }

            c_v2f v2ClampedDeltaSize = v2f::sub(v2Size, v2ClampedNewSize);

            if      (drag_quarter == 1) { v2ClampedDeltaPosition = v2ClampedDeltaSize; }
            else if (drag_quarter == 2) { v2ClampedDeltaPosition = { 0, v2ClampedDeltaSize.y }; }
            else if (drag_quarter == 3) { v2ClampedDeltaPosition = { v2ClampedDeltaSize.x, 0 }; }
            else if (drag_quarter == 4) { }
            else if (drag_quarter == 5) { v2ClampedDeltaPosition = { 0, v2ClampedDeltaSize.y }; }
            else if (drag_quarter == 6) { }
            else if (drag_quarter == 7) { v2ClampedDeltaPosition = { v2ClampedDeltaSize.x, 0 }; }
            else if (drag_quarter == 8) { }

            float fNewPosX = v2ClampedDeltaPosition.x;
            float fNewPosY = v2ClampedDeltaPosition.y;

            float fNewSizX = v2ClampedNewSize.x;
            float fNewSizY = v2ClampedNewSize.y;

            if (a_pControl->state().autosize.x)
            {
                fNewPosX = 0;
                fNewSizX = rTransform.width();
            }

            if (a_pControl->state().autosize.y)
            {
                fNewPosY = 0;
                fNewSizY = rTransform.height();
            }

            rTransform.size_(fNewSizX, fNewSizY).position_(v2f::add(rTransform.position, { fNewPosX, fNewPosY }));

            /*float const x = emp::mat::v2f::get_x(rTransform.position());
            float const y = emp::mat::v2f::get_y(rTransform.position());
            float const w = emp::mat::v2f::get_x(rTransform.size());
            float const h = emp::mat::v2f::get_y(rTransform.size());

            if (w <= 0 || h <= 0)
            {
                int i = 0;
                ++i;
            }*/

            //EMP_LOG_INFO("RESIZE : X #0, Y #1, W #2, H#3", x, y, w, h);

            drag_size = v2f::add(drag_size, v2ClampedDeltaPosition);

            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    //EMP_LOG_INFO("UNUSED 2");
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e mdi_c::handle(emp::app::c_event_s& a_rEvent, c_bool a_bForce)
{
    if (dragged != nullptr)
    {
        float fMinX = 0;
        float fMinY = 0;
        float fMaxX = 0;
        float fMaxY = 0;

        get_min_max(fMinX, fMinY, fMaxX, fMaxY);
        c_return_e eReturn = handle_move_resize(a_rEvent, dragged, nullptr, a_bForce);
        if (eReturn != return_e::Unused)
        {
            EMP_RET_X_IF_NOT(return_e::Error, update_scroll(fMinX, fMinY));
            return eReturn;
        }
    }
    else
    {
        dragged = nullptr;
        if (a_rEvent.mouse_pressed())
        {
            int kk = 0;
            kk++;
        }
        c_size stCount = scroller.control_count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            bool bPop = false;
            c_size stOffset = stCount - stIndex - 1;
            pc_control_i pControl = scroller.control(stOffset);
            EMP_ASSERT(pControl != nullptr);
            c_return_e eReturn = handle_move_resize(a_rEvent, pControl, &bPop, a_bForce);
            if (eReturn != return_e::Unused)
            {
                dragged = pControl;
                scroller.set_last(dragged);
                EMP_LOG_INFO("PIP #0", stOffset);
                return eReturn;
            }
            if (bPop)
            {
                EMP_LOG_INFO("POP #0", stOffset);
                EMP_GUI_MANAGER->activate_focus(pControl, focus_e::First, true, true, change_e::Mouse);
                scroller.set_last(pControl);
                break;
            }
        }
    }
    return return_e::Unused;
 }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mdi_c::handle_move_resize_start(emp::app::c_mouse_event_s& a_rMouse)
{
    if (dragged == nullptr)
    { return; }

    drag_quarter = 0;

    c_v2f v2MousePosition = a_rMouse.m_v2fPosition;

    transformation_data_s& rTransform = dragged->transformation();
    c_v2f v2Position = v2f::sub(v2MousePosition, rTransform.position);

    drag_size = v2f::sub(v2Position, rTransform.size);

    drag_position = v2f::sub(v2MousePosition, rTransform.position);

    c_v2f v2Pos = gui_to_gl(rTransform.matrix, v2f::zero()); // @@0 glcoord
    c_v2f v2Size = gui_to_gl(rTransform.matrix, rTransform.size); // @@0 glcoord

    c_float fPosX = v2Pos.x;
    c_float fPosY = v2Pos.y;
    c_float fWidth = v2Size.x - fPosX;
    c_float fHeight = v2Size.y - fPosY;
    c_float f13Width = 1 / 3.0f * fWidth;
    c_float f23Width = 2 / 3.0f * fWidth;
    c_float f13Height = 1 / 3.0f * fHeight;
    c_float f23Height = 2 / 3.0f * fHeight;

    c_float fX = v2MousePosition.x - fPosX;
    c_float fY = v2MousePosition.y - fPosY;

    if ((fX >= 0 && fX < f13Width) &&
        (fY >= 0 && fY < f13Height))
    { drag_quarter = 3; }
    else if ((fX >= f23Width && fX <= fWidth) &&
             (fY >= 0 && fY < f13Height))
    { drag_quarter = 4; }
    else if ((fX >= 0 && fX < f13Width) &&
             (fY >= f23Height && fY <= fHeight))
    { drag_quarter = 1; }
    else if ((fX >= f23Width && fX <= fWidth) &&
             (fY >= f23Height && fY <= fHeight))
    { drag_quarter = 2; }
    else if ((fX >= f13Width && fX < f23Width) &&
             (fY >= 0 && fY < f13Width))
    { drag_quarter = 6; }
    else if ((fX >= f13Width && fX < f23Width) &&
             (fY >= f23Height && fY <= fHeight))
    { drag_quarter = 5; }
    else if ((fX >= 0 && fX < f13Width) &&
             (fY >= f13Height && fY < f23Height))
    { drag_quarter = 7; }
    else if ((fX >= f23Width && fX <= fWidth) &&
             (fY >= f13Height && fY < f23Height))
    { drag_quarter = 8; }
    else
    {
        int i = 0;
        ++i;
    }
    // @@0  : remove global
    //EMP_GAME().gui_manager().activate_focus(
    //    dragged, focus_e::First, true, true,
    //        change_e::Mouse);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e mdi_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    auto const& rKeyboard = a_rEvent.keyboard;

    c_bool bPressed = rKeyboard.m_bPressed;

    emp::app::c_key_e eKey = rKeyboard.m_eKey;

    c_bool bShift = rKeyboard.m_bShift;
    c_bool bCtrl = rKeyboard.m_bCtrl;

    c_bool bShiftKey = eKey == emp::app::key_e::LeftShift || eKey == emp::app::key_e::RightShift;
    c_bool bCtrlKey = eKey == emp::app::key_e::LeftCtrl || eKey == emp::app::key_e::RightCtrl;

    if (bShiftKey && bPressed)
    {
        //EMP_LOG_INFO("KEY RESIZE");
        restart = true;
        control_state = control_state_e::Resize;
        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
    }
    else if (bShiftKey && bPressed == false)
    {
        restart = true;
        if (bCtrl || force)
        {
            //EMP_LOG_INFO("KEY MOVE");
            control_state = control_state_e::Move;
        }
        else
        {
            //EMP_LOG_INFO("ENONE 0");
            control_state = control_state_e::none;
        }
        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
    }
    else if (bCtrlKey && bPressed)
    {
        restart = true;
        control_state = control_state_e::Move;
        //EMP_LOG_INFO("KEY MOVE 0");
        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
    }
    else if (bCtrlKey && bPressed == false)
    {
        restart = true;
        if (bShift)
        {
            control_state = control_state_e::Resize;
            //EMP_LOG_INFO("KEY RESIZE 0");
        }
        else if (force)
        {
            control_state = control_state_e::Move;
            //EMP_LOG_INFO("KEY MOVE 1");
        }
        else
        {
            //EMP_LOG_INFO("ENONE 1");
            control_state = control_state_e::none;
        }
        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
    }
    else
    {
    }

    //return handle(a_rEvent, false);
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e mdi_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    //EMP_GUI_RETURN_IF_USED(handle(a_rEvent, false));
    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));
    if (automove)
    {
        EMP_GUI_RETURN_IF_USED(handle(a_rEvent, true));
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mdi_c& mdi_c::skin_(cpc_panel_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

