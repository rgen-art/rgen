//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_scroller.h"

#include "emp_gui_button.h"
#include "emp_gui_manager.h"
#include "emp_gui_picture.h"
#include "emp_gui_skin.h"
#include "emp_gui_spacer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
scroller_c::scroller_c(void):
    center(),
    up(),
    down(),
    scroll()
{
    up.state().focusable_(true).index_(0);
    up.push_on_repeat_(true).push_on_double_click_(true).alignment_(alignment_e::BottomCenter)
        .on_push.function_(emp::fct::function(this, &scroller_c::button_up_pushed));

    down.state().focusable_(true).index_(2);
    down.push_on_repeat_(true).push_on_double_click_(true).alignment_(alignment_e::TopCenter)
        .on_push.function_(emp::fct::function(this, &scroller_c::button_down_pushed));

    scroll.state().focusable_(true).navigable_(true).index_(0);
    scroll.on_push.function_(emp::fct::function(this, &scroller_c::button_scroll_pushed));

    center.state().index_(1);
    center.add_control(&scroll);

    add_control(&up);
    add_control(&center);
    add_control(&down);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
scroller_c::~scroller_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool scroller_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    if (dragged)
    {
        c_float fDelta = position - drag;
        current_(m_fOffset + (vertical() ? fDelta : -fDelta));
    }

    c_bool bVertical = m_bVertical;
    up.state().autosize.xy(true);
    down.state().autosize.xy(true);
    scroll.state().autosize.xy(bVertical, bVertical == false);
    //center.autosize().set(bVertical, bVertical == false);

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    up.state().autosize.xy(false);
    down.state().autosize.xy(false);
    scroll.state().autosize.xy(false);
    //center.autosize().set(false);

    float fMaxX = 0.0f;
    float fMaxY = 0.0f;

    //auto const* const pSpacing = skin ? skin->spacing : nullptr;
    c_float fSX = EMP_GUI_SKIN.spacing_x();//pSpacing ? pSpacing->x() : 0;
    c_float fSY = EMP_GUI_SKIN.spacing_y();//pSpacing ? pSpacing->y() : 0;

    if (bVertical)
    {
        fMaxX = emp::tt::maxof(up.transformation().width(),
            emp::tt::maxof(scroll.transformation().width(), down.transformation().width()));
        fMaxY = emp::tt::maxof(up.transformation().height(), down.transformation().height());
    }
    else
    {
        fMaxX = emp::tt::maxof(up.transformation().width(), down.transformation().width());
        fMaxY = emp::tt::maxof(up.transformation().height(),
            emp::tt::maxof(scroll.transformation().height(), down.transformation().height()));
    }

    up.transformation().size_(fMaxX, fMaxY);
    down.transformation().size_(fMaxX, fMaxY);

    if (bVertical)
    {
        float fY = 0;

        up.transformation().position_(0, fY);
        fY -= up.transformation().size.y + fSY;

        center.transformation().position_(0, fY);
        center.transformation().size_(fMaxX, transformation().height() - 2 * fMaxY - 2 * fSY);
        fY -= center.transformation().height() + fSY;

        down.transformation().position_(0, fY);

        scroll.transformation().position_(0, -current());
        scroll.transformation().size_(fMaxX, center.transformation().height() * scroll_ratio());

        transformation().width_(fMaxX);
    }
    else
    {
        float fX = 0;

        up.transformation().position_(fX, 0);
        fX += up.transformation().width() + fSX;

        center.transformation().position_(fX, 0);
        center.transformation().size_(transformation().width() - 2 * fMaxX - 2 * fSX, fMaxY);
        fX += center.transformation().width() + fSX;

        down.transformation().position_(fX, 0);

        scroll.transformation().position_(current(), 0);
        scroll.transformation().size_(center.transformation().width() * scroll_ratio(), fMaxY);

        transformation().height_(fMaxY);
    }

    return tbase1::resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e scroller_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false || state().enable == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    auto const& rKeyboard = a_rEvent.keyboard;
    if (focus_in_hierarchy(this) &&
        rKeyboard.m_bPressed)
    {
        emp::app::c_key_e eKey = rKeyboard.m_eKey;
        if (eKey == emp::app::key_e::Left ||
            eKey == emp::app::key_e::Up)
        {
            scroll_up();
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (
            eKey == emp::app::key_e::Right ||
            eKey == emp::app::key_e::Down)
        {
            scroll_down();
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e scroller_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    c_bool bFocus = emp::gui::focus_in_hierarchy(this);

    auto const& rMouse = a_rEvent.mouse;
    c_v2f v2Position = rMouse.m_v2fPosition;
    c_float fPosition = vertical() ? v2Position.y : v2Position.x;

    if (state().enable &&
        (state().hover || bFocus) &&
         a_rEvent.keyboard.modified() == false)
    {
        emp::app::c_button_e eButton = rMouse.m_eButton;
        if (eButton == emp::app::button_e::WheelUp)
        {
            scroll_up();
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eButton == emp::app::button_e::WheelDown)
        {
            scroll_down();
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }

        if (eButton == emp::app::button_e::Left)
        {
            if (a_rEvent.mouse_pressed())
            {
                if (scroll.state().visible &&
                    scroll.state().enable &&
                    scroll.state().hover &&
                    dragged == false)
                {
                    dragged = true;
                    drag = fPosition;
                    position = drag;
                    m_fOffset = m_fCurrent;
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (a_rEvent.mouse_released())
            {
                dragged = false;
                drag = 0;
                position = 0;
                m_fOffset = 0;
                return return_e::Unused;
            }
            else if (a_rEvent.mouse_dragged())
            {
                if (dragged)
                {
                    drag = fPosition;
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
scroller_c& scroller_c::current_(c_float a_fValue)
{
    m_fCurrent = a_fValue;

    c_bool bVertical = m_bVertical;

    c_transformation_data_s& rCenterTransform = center.transformation();
    c_float fSize = bVertical ? rCenterTransform.height() : rCenterTransform.width();

    c_transformation_data_s& rButtonTransform = scroll.transformation();
    c_float fScrollSize = bVertical ? rButtonTransform.height() : rButtonTransform.width();

    m_fCurrent = emp::tt::maxof(0.0f, emp::tt::minof(m_fCurrent, fSize - fScrollSize));
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
scroller_c& scroller_c::offset_(c_float a_fValue)
{
    c_float fScrollRatio = scroll_ratio();
    if (fScrollRatio >= 1)
    {
        current_(0);
        return *this;
    }
    c_float fOffsetMax = offset_max();
    if (fOffsetMax <= 0)
    {
        current_(0);
        return *this;
    }
    current_(a_fValue * scroll_max() / fOffsetMax);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float scroller_c::offset(void) const
{
    c_float fScrollRatio = scroll_ratio();
    if (fScrollRatio >= 1)
    { return 0; }
    float const fScrollMax = scroll_max();
    if (fScrollMax <= 0)
    { return 0; }
    return current() * offset_max() / fScrollMax;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float scroller_c::offset_max(void) const
{ return sum - page; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
scroller_c& scroller_c::vertical_(c_bool a_bValue)
{
    m_bVertical = a_bValue;
    return skin_(skin);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
scroller_c& scroller_c::skin_(cpc_scroller_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    center.skin_(a_pValue ? a_pValue->center : nullptr);
    up.skin_(a_pValue ? a_pValue->picture_icon : nullptr);
    down.skin_(a_pValue ? a_pValue->picture_icon : nullptr);
    scroll.skin_(a_pValue ? a_pValue->picture_center : nullptr);
    scroll.model_(a_pValue ? a_pValue->gfx_scroll : nullptr);

    emp::gfx::model_c const* const pUp = a_pValue ? (vertical() ? a_pValue->gfx_up : a_pValue->gfx_left) : nullptr;
    emp::gfx::model_c const* const pDown = a_pValue ? (vertical() ? a_pValue->gfx_down : a_pValue->gfx_right) : nullptr;

    up.model_(pUp);
    down.model_(pDown);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float scroller_c::scroll_max(void) const
{
    c_bool bVertical = vertical();
    c_float fCenterSize = bVertical ? center.transformation().height() : center.transformation().width();
    c_float fScrollMax = (1 - scroll_ratio()) * fCenterSize;
    return fScrollMax;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float scroller_c::scroll_ratio(void) const
{ return sum > 0 ? emp::tt::minof(1.0f, page / sum) : 0; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e scroller_c::button_up_pushed(cpc_control_i, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    {
        scroll_up();
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e scroller_c::button_down_pushed(cpc_control_i, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    {
        scroll_down();
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e scroller_c::button_scroll_pushed(cpc_control_i, c_bool, c_change_e)
{ return return_e::Unused; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

