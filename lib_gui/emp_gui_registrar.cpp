//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_registrar.h"

#include "emp_gui_window_i.h"
#include "emp_gui_manager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void registrar_c::stop(void)
{
    m_eState = state_e::Stopping;
    c_size stCount = m_vpWindows.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gui::window_i* const pWindow = m_vpWindows.val(stIndex);
        EMP_ASSERT(pWindow != nullptr);
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(pWindow));
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void registrar_c::update(void)
{
    if (m_eState == state_e::Stopping)
    {
        state_e eState = state_e::Stopped;
        for (size_t stIndex = 0; stIndex < m_vpWindows.count();)
        {
            emp::gui::window_i* const pWindow = m_vpWindows.val(stIndex);
            EMP_ASSERT(pWindow != nullptr);
            if (pWindow->is_visible())
            {
                eState = state_e::Stopping;
                ++stIndex;
            }
            else
            { m_vpWindows.remove_unordered(stIndex); }
        }
        m_eState = eState;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool registrar_c::is_modal_visible(void) const
{
    c_size stCount = m_vpWindows.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpWindows.val(stIndex);
        EMP_ASSERT(pWindow != nullptr);
        if (pWindow->is_visible() &&
            pWindow->is_modal())
        { return true; }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void registrar_c::remove_window(window_i* const a_pValue)
{
    c_size stCount = m_vpWindows.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gui::window_i* const pWindow = m_vpWindows.val(stIndex);
        if (pWindow == a_pValue)
        {
            m_vpWindows.remove(stIndex);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

