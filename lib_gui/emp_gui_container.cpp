//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

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
EMP_RETURN bool contains_control(vector<control_i*> const& a_rvpControls, cpc_control_i a_pValue)
{
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (a_rvpControls.val(stIndex) == a_pValue)
        { return true; }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e process_controls_keyboard(emp::cnt::vector<control_i*> const& a_rvpControls, emp::app::c_event_s& a_rEvent)
{
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stCount - stIndex - 1;
        pc_control_i pControl = a_rvpControls.val(stOffset);
        EMP_ASSERT(pControl != nullptr);
        EMP_GUI_RETURN_IF_USED(pControl->process_keyboard(a_rEvent));
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e process_controls_mouse(emp::cnt::vector<control_i*> const& a_rvpControls, emp::app::c_event_s& a_rEvent)
{
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stCount - stIndex - 1;
        pc_control_i pControl = a_rvpControls.val(stOffset);
        EMP_ASSERT(pControl != nullptr);
        EMP_GUI_RETURN_IF_USED(pControl->process_mouse(a_rEvent));
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void process_mouse_on_control(control_i& a_rControl, emp::app::c_event_s& a_rEvent)
{
    if (a_rEvent.mouse_pressed() == false &&
        a_rEvent.mouse_released() == false &&
        a_rEvent.mouse_moved() == false &&
        a_rEvent.mouse_dragged() == false)
    { return; }

    state_data_s& rState = a_rControl.state();

#if defined EMP_XX_DEBUG
    if (a_rControl.debug_id() == 38 && a_rEvent.mouse_moved()) // @@0 remove
    {
        int kk = 0;
        ++kk;
    }
    #endif
    if (rState.visible && rState.renderable)
    {
        bool bContains = false;
        pc_control_i pParent = a_rControl.parent();
        if (pParent->state().hover)
        {
            if (rState.hover_through)
            { bContains = true; }
            else
            {
                bContains = EMP_GUI_MANAGER->is_hover_candidate(&a_rControl);
                // bContains = emp::gui::contains_mouse(rTransform.matrix, rTransform.size, a_rEvent.mouse.m_v2fPosition);
            }
        }
        if (bContains) // @@0 remove
        {
            int kk = 0;
            ++kk;
        }
        rState.hover_(bContains);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void add_control(emp::cnt::vector<control_i*>& a_rvpControls, pc_control_i a_pValue, pc_control_i a_pParent)
{
    EMP_ASSERT(a_pValue != nullptr);
#ifdef EMP_XX_ASSERT
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_control_i pControl = a_rvpControls.val(stIndex);
        EMP_ASSERT(pControl != nullptr);
        if (pControl == a_pValue)
        {
            EMP_ASSERT_UNREACHABLE();
            return;
        }
    }
#endif
    a_rvpControls.add(a_pValue);
    a_pValue->set_parent(a_pParent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void insert_control(emp::cnt::vector<control_i*>& a_rvpControls, pc_control_i a_pValue, pc_control_i a_pParent, c_size a_stIndex)
{
    EMP_ASSERT(a_pValue != nullptr);
#ifdef EMP_XX_ASSERT
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_control_i pControl = a_rvpControls.val(stIndex);
        EMP_ASSERT(pControl != nullptr);
        if (pControl == a_pValue)
        {
            EMP_ASSERT_UNREACHABLE();
            return;
        }
    }
#endif
    a_rvpControls.insert(a_stIndex, a_pValue);
    a_pValue->set_parent(a_pParent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool try_remove_control(emp::cnt::vector<control_i*>& a_rvpControls, cpc_control_i a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_control_i pControl = a_rvpControls.val(stIndex);
        EMP_ASSERT(pControl != nullptr);
        if (pControl == a_pValue)
        {
            a_rvpControls.remove(stIndex);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void remove_control(emp::cnt::vector<control_i*>& a_rvpControls, cpc_control_i a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    EMP_ASSERT_BOOL_CALL(try_remove_control(a_rvpControls, a_pValue));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
container_c::container_c(void):
    m_State(),
    m_Transform(),
    m_Color(),
    m_vpControls()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
container_c::~container_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::post_load(void)
{
    c_size stCount = m_vpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpControls.val(stIndex)->set_parent(this); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* container_c::got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
{
    control_i* pFocus = nullptr;
    if (emp::tt::is_not_max(m_State.index))
    {
        ui32 EMP_TT_MAX_VAR(stSelection);
        pFocus = got_focus_controls(m_vpControls, a_eFocus, a_bSelect, a_bPush, stSelection, a_eChange);
        if (pFocus == nullptr && m_State.focusable)
        { pFocus = this; }
    }
    return pFocus;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::unselect(c_bool a_bTransmit)
{
    m_State.select_(false);
    if (a_bTransmit)
    {
        c_size stCount = m_vpControls.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(m_vpControls.val(stIndex)->unselect(a_bTransmit)); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::update(emp::tim::clock_c const& a_rClock)
{
    if (m_State.visible)
    {
        c_size stCount = m_vpControls.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(m_vpControls.val(stIndex)->update(a_rClock)); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::resize(void)
{
    if (m_State.visible)
    {
        c_size stCount = m_vpControls.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(m_vpControls.val(stIndex)->resize()); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::stack(stack_c& a_rStack)
{
    if (m_State.visible)
    {
        depth_stacker_c oStacker(a_rStack, transformation());

        c_size stCount = m_vpControls.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            auto* const pControl = m_vpControls.val(stIndex);
            if (pControl->state().overlap)
            { a_rStack.depth = a_rStack.previous_depth + 1; }
            pControl->transformation().back_depth_(a_rStack.depth);
            EMP_RET_FALSE_IF_NOT(pControl->stack(a_rStack));
            //pControl->transformation().fore_depth_(a_rStack.previous_depth);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::transform(stack_c& a_rStack)
{
    if (m_State.visible)
    {
        matrix_stacker_c oStacker(a_rStack, m_Transform, m_State.enable);
        c_size stCount = m_vpControls.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(m_vpControls.val(stIndex)->transform(a_rStack)); }
        update_colors_current(a_rStack.enabled.current());
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void container_c::update_colors_current(c_bool a_bEnable)
{
    state_data_s oChild;
    if (child)
    {
        c_bool bChildFocus = state_focus(this);
        c_bool bChildSelect = state_select(this);
        c_bool bChildHover = state_hover(this);
        c_bool bChildFocusable = foreach_control(this, [](cpc_control_i a_pControl) EMP_RETURN -> bool { return a_pControl->state().focusable; });

        oChild.focus_(bChildFocus).select_(bChildSelect).hover_(bChildHover).focusable_(bChildFocusable);
    }

    if (skin != nullptr)
    { compute_container_color(*skin, m_State, oChild, a_bEnable, m_Color); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool container_c::render(void)
{
    if (m_State.visible &&
        m_State.renderable)
    {
        c_si32 siBack = m_Transform.back_depth;
        {
            bool bEmpty = false;
            EMP_GUI_MANAGER_CLIP(oScissor, false, m_Transform, bEmpty, 0, -1, 0, 0);
            if (bEmpty)
            { return true; }

           m_Color.background.raster.render(m_Transform, depth(siBack, 0.001f));
        }

        bool bEmpty = false;
        EMP_GUI_MANAGER_CLIP(oScissor, false, m_Transform, bEmpty, 0, 0, 0, 0);
        if (bEmpty)
        { return true; }

        c_size stCount = m_vpControls.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            pc_control_i pControl = m_vpControls.val(stIndex);
            if (outside(pControl, m_Transform.size) == false)
            { EMP_RET_FALSE_IF_NOT(pControl->render()); }
        }

        c_si32 siFore = m_Transform.fore_depth;
        c_float fState = (state_select(this) || state_focus(this)) ? 0.0001f : 0.0f;
        m_Color.background.border.render(m_Transform, depth(siBack, 0.002f + fState));
        m_Color.foreground.raster.render(m_Transform, depth(siFore, 0.003f));
        m_Color.foreground.border.render(m_Transform, depth(siFore, 0.004f));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e container_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (m_State.visible &&
        m_State.enable)
    { EMP_GUI_RETURN_IF_USED(process_controls_keyboard(m_vpControls, a_rEvent)); }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e container_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (a_rEvent.mouse_moved()) // @@0 remove
    {
        int k = 0;
        ++k;
    }
    else if (a_rEvent.mouse_dragged()) // @@0 remove
    {
        int kk = 0;
        ++kk;
    }
    else if (a_rEvent.mouse_pressed())
    {
        int kk = 0;
        ++kk;
    }
    process_mouse_on_control(*this, a_rEvent);
    if (m_State.visible)
    { return emp::gui::process_controls_mouse(m_vpControls, a_rEvent); }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void container_c::set_first(pc_control_i a_pValue)
{
    if (m_vpControls.count())
    {
        if (m_vpControls.val(0) != a_pValue)
        {
            remove_control(a_pValue);
            if (m_vpControls.count())
            { m_vpControls.insert(0, a_pValue); }
            else
            { m_vpControls.add(a_pValue); }
        }
    }
    else
    { EMP_ASSERT_UNREACHABLE(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void container_c::set_last(pc_control_i a_pValue)
{
    if (m_vpControls.count())
    {
        if (m_vpControls.last_val() != a_pValue)
        {
            remove_control(a_pValue);
            m_vpControls.add(a_pValue);
        }
    }
    else
    { EMP_ASSERT_UNREACHABLE(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

