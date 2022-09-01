//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_window.h"

#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tim_time.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool window_c::post_load(void)
{
    skin.data = EMP_GUI_SKIN.window.get(skin.name);
    // @@0 uncomment EMP_RET_FALSE_IF(skin.data == nullptr, "Failed to load skin '#0'", skin.name);
    skin_(skin.data);
    create();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void window_c::create(void)
{
    child_(true);
    state().visible_(false).enable_(false);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float window_c::fade_factor(void) const
{
    c_float fFadeDuration = skin.data ? skin.data->duration : 0;
    return fFadeDuration > 0 ? fade_time() / fFadeDuration : 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool window_c::update(emp::tim::clock_c const& a_rTime)
{
    if (state().visible == false)
    { return true; }

    c_float fDeltaTime = emp::tim::time_c::to_seconds(a_rTime.delta());
    c_float fDeltaDirection = emp::mat::minof(0.1f, fDeltaTime) * m_fFadeDirection;

    m_fFadeTime += fDeltaDirection;

    c_float fFadeDuration = skin.data ? skin.data->duration : 0.0f;

    /*if (m_fFadeDirection >= 1.0f)
    {
        EMP_LOG_INFO("SHOWING #0", m_fFadeTime);
        if (m_fFadeTime > fFadeDuration)
        {
            m_bShowing = false;
        }
    }
    else if (m_fFadeDirection <= -1.0f)
    {
        EMP_LOG_INFO("HIDING #0", m_fFadeTime);
        if (m_fFadeTime <= 0)
        {
            m_bHiding = false;
            state().visible_(false).enable_(false);
        }
    }*/

    if (m_bShowing)
    {
        if (m_fFadeDirection >= 1.0f)
        {
            //EMP_LOG_DEBUG("SHOWING #0", m_fFadeTime);
            if (m_fFadeTime > fFadeDuration)
            {
                m_bShowing = false;
            }
        }
    }
    else if (m_bHiding)
    {
        if (m_fFadeDirection <= -1.0f)
        {
            //EMP_LOG_DEBUG("HIDING #0", m_fFadeTime);
            if (m_fFadeTime <= 0)
            {
                m_bHiding = false;
                state().visible_(false).enable_(false);
            }
        }
    }

/*    if (m_fFadeTime <= fFadeDuration && m_fFadeDirection  <= -1.0f)
    {}
    else if (m_fFadeTime >= 0.0f && m_fFadeTime <= fFadeDuration &&
        m_fFadeDirection >= 1.0f)
    {}
    else
    { EMP_ASSERT(emp::tt::equal_(m_fFadeDirection, 0.0f)); }

    if (m_fFadeTime <= 0.0f && m_fFadeDirection <= -1.0f)
    {
        m_bHiding = false;
        state().set_visible(false);
        state().set_enable(false);
    }
    else if (m_fFadeTime > fFadeDuration && m_fFadeDirection >= 1.0f &&
        m_bShowing)
    { m_bShowing = false; }*/



    m_fFadeTime = emp::tt::minof(emp::tt::maxof(0.0f, m_fFadeTime), fFadeDuration);

    return docker_c::update(a_rTime);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool window_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    if (hook != nullptr)
    {
        emp::gui::matrix_stacker_c oStacker(a_rStack, hook->transformation().matrix, state().enable);
        EMP_RET_FALSE_IF_NOT(docker_c::transform(a_rStack));
    }
    else
    { EMP_RET_FALSE_IF_NOT(docker_c::transform(a_rStack)); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
window_c& window_c::skin_(cpc_window_skin_s a_pValue)
{
    skin.data = a_pValue;
    docker_c::skin_(a_pValue ? a_pValue->sizer : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

