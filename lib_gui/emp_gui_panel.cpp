//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_panel.h"

#include "emp_gui_manager.h"
#include "emp_gui_scroller.h"
#include "emp_gui_skin.h"

#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
panel_c::panel_c(void)
{
    scroller.state().hover_through_(true).index_(0);

    user.add_control(&scroller);
    user.state().navigable_(true).index_(0);

    vsb.vertical_(true).state().visible_(false).index_(1);
    hsb.vertical_(false).state().visible_(false).index_(2);

    add_control(&user);
    add_control(&vsb);
    add_control(&hsb);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
panel_c::~panel_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool panel_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(scroller.resize());

    c_float fUserWidth = user.transformation().width();
    c_float fUserHeight = user.transformation().height();

    float fMinX = 0.0f;
    float fMinY = 0.0f;
    float fMaxX = fUserWidth;
    float fMaxY = fUserHeight;

    c_size stCount = scroller.control_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = scroller.control(stIndex);
        EMP_ASSERT(pControl != nullptr);
        c_float fX = pControl->transformation().x();
        c_float fY = pControl->transformation().y();
        fMinX = emp::tt::minof(fMinX, fX);
        fMinY = emp::tt::minof(fMinY, -fY);
        fMaxX = emp::tt::maxof(fMaxX, fX + pControl->transformation().width());
        fMaxY = emp::tt::maxof(fMaxY, -fY + pControl->transformation().height());
    }

    EMP_RET_FALSE_IF_NOT(refresh_scrolling(fMaxX - fMinX, fMaxY - fMinY));
    EMP_RET_FALSE_IF_NOT(refresh_scrolling(fMaxX - fMinX, fMaxY - fMinY));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_visible(
    cpc_control_i a_pControl,
    c_float a_fOffsetX EMP_UNUSED,
    c_float a_fOffsetY EMP_UNUSED,
    c_float a_fUserWidth,
    c_float a_fUserHeight)
{
    c_float fX = a_pControl->transformation().x() + a_fOffsetX;
    c_float fY = a_pControl->transformation().y() + a_fOffsetY;
    c_float fW = a_pControl->transformation().width();
    c_float fH = a_pControl->transformation().height();

    if (fX > a_fUserWidth)
    { return false; }
    if (fX + fW < 0)
    { return false; }
    if (fY < -a_fUserHeight)
    { return false; }
    if (fY - fH > 0)
    { return false; }
//    EMP_LOG_INFO("OFFSET X : #0, OFFSET Y : #1",
  //      a_fOffsetX, a_fOffsetY);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool panel_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    c_size stCount = scroller.control_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = scroller.control(stIndex);
        pControl->state().renderable_(is_visible(
            pControl, scroller.transformation().x(), scroller.transformation().y(),
            user.transformation().width(), user.transformation().height()));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool panel_c::refresh_scrolling(c_float a_fX, c_float a_fY)
{
    hsb.sum_(a_fX);
    EMP_RET_FALSE_IF_NOT(hsb.resize());
    hsb.state().visible_((a_fX - user.transformation().width()) > 0.001f);

    vsb.sum_(a_fY);
    EMP_RET_FALSE_IF_NOT(vsb.resize());
    vsb.state().visible_((a_fY - user.transformation().height()) > 0.001f);

    c_float fSX = EMP_GUI_SKIN.spacing_x();//(skin && skin->spacing) ? skin->spacing->x() : 0;
    c_float fSY = EMP_GUI_SKIN.spacing_y();//(skin && skin->spacing) ? skin->spacing->y() : 0;

    c_float fVSBWidth = vsb.transformation().width();
    c_float fRemX = (vsb.state().visible ? fVSBWidth + fSX : 0);

    float fFinalWidth = 0;
    float fFinalUserWidth = 0;
    if (state().autosize.x)
    {
        fFinalUserWidth = a_fX;
        fFinalWidth = fFinalUserWidth + fRemX;
    }
    else
    {
        fFinalWidth = transformation().width();
        fFinalUserWidth = fFinalWidth - fRemX;
    }

    c_float fHSBHeight = hsb.transformation().height();
    c_float fRemY = (hsb.state().visible ? fHSBHeight + fSY : 0);

    float fFinalHeight = 0;
    float fFinalUserHeight = 0;
    if (state().autosize.y)
    {
        fFinalUserHeight = a_fY;
        fFinalHeight = fFinalUserHeight + fRemY;
    }
    else
    {
        fFinalHeight = transformation().height();
        fFinalUserHeight = fFinalHeight - fRemY;
    }

    transformation().size_(fFinalWidth, fFinalHeight);
    user.transformation().size_(fFinalUserWidth, fFinalUserHeight);

    vsb.page_(fFinalUserHeight)
        .current_(vsb.current())
        .transformation().position_(fFinalWidth - fVSBWidth, 0).height_(fFinalUserHeight);
    EMP_RET_FALSE_IF_NOT(vsb.resize());

    hsb.page_(fFinalUserWidth)
        .current_(hsb.current())
        .transformation().position_(0, -fFinalHeight + fHSBHeight).width_(fFinalWidth); // @@0 glcoord width or size
    EMP_RET_FALSE_IF_NOT(hsb.resize());

    scroller.transformation().position_(offset_x - hsb.offset(), offset_y + vsb.offset()).size_(a_fX, a_fY);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void panel_c::get_min_max(float& a_rfMinX, float& a_rfMinY, float& a_rfMaxX, float& a_rfMaxY)
{
    EMP_TT_MAX_VAR(a_rfMinX);
    EMP_TT_MAX_VAR(a_rfMinY);

    EMP_TT_MIN_VAR(a_rfMaxX);
    EMP_TT_MIN_VAR(a_rfMaxY);

    c_size stCount = scroller.control_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_control_i pControl = scroller.control(stIndex);
        EMP_ASSERT(pControl != nullptr);
        c_float fX = pControl->transformation().x();
        c_float fY = pControl->transformation().y();
        a_rfMinX = emp::tt::minof(a_rfMinX, fX);
        a_rfMinY = emp::tt::minof(a_rfMinY, -fY);
        a_rfMaxX = emp::tt::maxof(a_rfMaxX, fX + pControl->transformation().width());
        a_rfMaxY = emp::tt::maxof(a_rfMaxY, -fY + pControl->transformation().height());
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void set_scroll(c_float a_fOldOffset, c_float a_fMin, c_float a_fNewMin, scroller_c& a_rScroller, float& a_rfOffset);
void set_scroll(c_float a_fOldOffset, c_float a_fMin, c_float a_fNewMin, scroller_c& a_rScroller, float& a_rfOffset)
{
    c_float fDeltaMin = a_fNewMin - a_fMin;
    float fOffset = 0;

    if (fDeltaMin < 0 && a_fNewMin < 0)
    { fOffset = a_fMin > 0 ? a_fNewMin : fDeltaMin; }

    if (fDeltaMin > 0 && a_fMin < 0)
    { fOffset = a_fNewMin > 0 ? -a_fMin : fDeltaMin; }

    a_rfOffset -= fOffset;
    a_rScroller.offset_(a_fOldOffset - fOffset);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void panel_c::scroll_up(void)
{
    if (vsb.state().visible)
    { vsb.scroll_up(); }
    else if (hsb.state().visible)
    { hsb.scroll_up(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void panel_c::scroll_down(void)
{
    if (vsb.state().visible)
    { vsb.scroll_down(); }
    else if (vsb.state().visible)
    { vsb.scroll_down(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e panel_c::process_mouse(emp::app::c_event_s &a_rEvent)
{
    if (a_rEvent.keyboard.modified()) // @@0 remove
    {
        int kk = 0;
        kk++;
    }
    auto const eReturn = tbase1::process_mouse(a_rEvent);
    if (eReturn != emp::gui::return_e::Unused)
    { return EMP_GUI_RETURN(a_rEvent, eReturn); }

    if (state().enable &&
        state().hover &&
        a_rEvent.keyboard.modified() == false)
    {
        emp::app::c_button_e eButton = a_rEvent.mouse.m_eButton;
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
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void panel_c::ensure_visible(cpc_control_i a_pControl)
{
    c_float fOffsetV = vsb.offset();
    c_float fOffsetH = hsb.offset();

    c_float fX = a_pControl->transformation().x() - fOffsetH;
    c_float fY = -a_pControl->transformation().y() - fOffsetV;
    c_float fW = a_pControl->transformation().width();
    c_float fH = a_pControl->transformation().height();

    {
        c_float fOffset = fX;
        if (fOffset < 0)
        { hsb.offset_(fOffsetH - fOffset);  }
    }

    {
        c_float fOffset = fX + fW - user.transformation().width();
        if (fOffset > 0)
        { hsb.offset_(fOffsetH + fOffset); }
    }

    {
        c_float fOffset = fY;
        if (fOffset < 0)
        { vsb.offset_(fOffsetV + fOffset); }
    }

    {
        c_float fOffset = fY + fH - user.transformation().height();
        if (fOffset > 0)
        { vsb.offset_(fOffsetV + fOffset); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool panel_c::update_scroll(c_float a_fMinX, c_float a_fMinY)
{
    float fNewMinX = 0;
    float fNewMinY = 0;
    float fNewMaxX = 0;
    float fNewMaxY = 0;
    get_min_max(fNewMinX, fNewMinY, fNewMaxX, fNewMaxY);

    c_float fOldOffsetX = hsb.offset();
    c_float fOldOffsetY = vsb.offset();

    //@@perf
    EMP_RET_FALSE_IF_NOT(resize());
    EMP_RET_FALSE_IF_NOT(resize());

    set_scroll(fOldOffsetX, a_fMinX, fNewMinX, hsb, offset_x);
    set_scroll(fOldOffsetY, a_fMinY, fNewMinY, vsb, offset_y);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float panel_c::delta_user_w(void)
{
    c_float fUserW = user.transformation().width();
    c_float fScrollX = scroller.transformation().x();
    c_float fScrollW = hsb.sum;
    c_float fDeltaW = fUserW - fScrollX - fScrollW;
    c_float fWidth = scroller.transformation().width();
    scroller.transformation().width_(fWidth + emp::tt::maxof(0.0f, fDeltaW));
    return fDeltaW;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float panel_c::delta_docker_w(void)
{
    c_float fUserW = transformation().width();
    c_float fScrollX = scroller.transformation().x();
    c_float fScrollW = hsb.sum;
    c_float fDeltaW = fUserW - fScrollX - fScrollW;
    c_float fWidth = scroller.transformation().width();
    scroller.transformation().width_(fWidth + emp::tt::maxof(0.0f, fDeltaW));
    return fDeltaW;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
panel_c& panel_c::skin_(cpc_panel_skin_s a_pValue)
{
    skin = a_pValue;
    user.skin_(a_pValue ? a_pValue->user : nullptr);
    vsb.skin_(a_pValue ? a_pValue->scroller : nullptr);
    hsb.skin_(a_pValue ? a_pValue->scroller : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

