//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_manager.h"

#include "emp_gui_window.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
manager_c::manager_c(void):
m_mpFocuses(8),
hover_candidates(8),
hover_quads(8)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
manager_c::~manager_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::post_load(void)
{
    destroy();

    //m_pModal->create();
    //m_pModal->skin_(EMP_GUI_SKIN_(window, "wnd.modal"));
    //m_pModal->transformation().position_(0).size_(2);

    state().visible_(true).enable_(true).hover_(true);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::destroy(void)
{
    m_vpStack.resize(0);
    m_vpNavigables.resize(0);
    // @@0 remove *m_pFctOnAltEnter = emp::fct::function_t<void(*)(void)>();

    m_pFocus = nullptr;
    m_pModalFocus = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::update(emp::tim::clock_c const& a_rClock)
{
    cursor_size = v2f::mul(EMP_GUI_SKIN.resolution.ratio(), 0.02f);

    bool bFloating = false;
    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpStack.val(stIndex);
        EMP_ASSERT(pWindow != nullptr);
        EMP_RET_FALSE_IF_NOT(pWindow->update(a_rClock));

        if (pWindow->is_floating() &&
            pWindow->is_hidden() == false &&
            focus_in_hierarchy(pWindow) == false)
        { emp::tt::silent(hide_window(pWindow)); }
        if (pWindow->is_floating())
        { bFloating = true; }
    }

    if (modal() == false &&
        bFloating == false)
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            window_i* const pWindow = m_vpStack.val(stIndex);

            pc_control_i pFocus = focus(pWindow);
            if (pFocus != m_pFocus)
            { activate_focus(pFocus, focus_e::First, true, true, change_e::Code); }
        }
    }

    /*if (m_pModal->state().visible)
    {
    return m_pModal->update(a_rClock);
    }*/
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::resize(void)
{
    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpStack.val(stIndex);
        EMP_ASSERT(pWindow != nullptr);
        EMP_RET_FALSE_IF_NOT(pWindow->resize());
    }
    //return m_pModal->resize();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::transform(stack_c& a_rStack)
{
    {
        c_size stCount = m_vpStack.count();
        auto const l_render_stack = [this, stCount](c_bool a_bTop)
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                window_i* const pWindow = m_vpStack.val(stIndex);
                if (pWindow->is_top() == a_bTop)
                {
                    //if (pWindow->is_modal())
                    //{ m_vpRenderStack.add(m_pModal); }
                    m_vpRenderStack.add(pWindow);
                }
            }
        };
        m_vpRenderStack.resize(0);
        l_render_stack(false);
        l_render_stack(true);
    }

    {
        a_rStack.depth = 0;
        a_rStack.previous_depth = 0;
        depth_stacker_c oStacker(a_rStack, transformation());
        c_size stCount = m_vpRenderStack.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            auto* const pWindow = m_vpRenderStack.val(stIndex);
            EMP_RET_FALSE_IF_NOT(pWindow->stack(a_rStack));
            if (pWindow->state().overlap)
            { a_rStack.depth = a_rStack.previous_depth + 1; }
        }
    }

    {
        transformation().position_({-1, 1});
        matrix_stacker_c oStacker(a_rStack, transformation(), state().enable);
        c_size stCount = m_vpRenderStack.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            auto* const pWindow = m_vpRenderStack.val(stIndex);
            EMP_RET_FALSE_IF_NOT(pWindow->transform(a_rStack));
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::next_transition(void)
{ m_stTransition = m_pRandom->rand<size_t>(0, EMP_GRAPHICS.transition_programs_count()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::render(void)
{
    size_t EMP_TT_MAX_VAR(stModal);
    m44f m44Identity;
    m44f::identity(m44Identity);

    auto const l_draw_hovers = [this]() EMP_RETURN -> bool
    {
        //EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());
        //emp::gui::draw_quad(mouse_position, cursor_size	, -0.99f, {1, 0, 1, 1}, true);
        /*for (auto it = hover_quads.begin(); it.not_end(); ++it)
        {
            cpc_transformation_data_s pTransform = *it.key();
            emp::gui::draw_quad(pTransform->matrix, pTransform->size, 90, 0.0, {0, 1, 0, 1}, true);
        }*/

        hover_quads.clear();
        return true;
    };

    auto const l_draw_modal = [](window_i* const a_pWindow)
    {
        if (a_pWindow->is_modal() && a_pWindow->is_overlay() == false)
        {
            //c_float fDepth = depth(a_pWindow->transformation().back_depth, -0.0001f);
            //@@2 readd using skin option : emp::gui::draw_quad({0, 0}, 1, fDepth, EMP_GUI_SKIN.data->link_a50(), false);
        }
    };

    c_size stCount = m_vpRenderStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpRenderStack.val(stIndex);
        if (pWindow->is_modal())// == m_pModal)
        { stModal = stIndex; break; }
    }

    if (emp::tt::is_not_max(stModal))
    {
        auto& rDevice = EMP_GRAPHICS;

        ui32 uiW = 0;
        ui32 uiH = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(EMP_APPLICATION.view_pixel_width(), uiW));
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(EMP_APPLICATION.view_pixel_height(), uiH));

        vector<ui32> vstFramebuffers;

        for (size_t stIndex = 0; stIndex < stModal; ++stIndex)
        {
            ui32 EMP_TT_MAX_VAR(ui32Target);
            EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(uiW, uiH, true, ui32Target));

            target_s oTarget;
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target, oTarget));
            EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget));
            EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 0}));
            rDevice.set_blend(true);
            rDevice.set_depth_test(false);
            rDevice.set_depth_mask(false);
            rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
            vstFramebuffers.add(ui32Target);

            window_i* const pWindow = m_vpRenderStack.val(stIndex);
            l_draw_modal(pWindow);
            EMP_RET_FALSE_IF_NOT(pWindow->render());
        }

        ui32 EMP_TT_MAX_VAR(ui32Target0);
        target_s oTarget0;

        if (vstFramebuffers.count() > 1)
        {
            EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(uiW, uiH, true, ui32Target0));
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target0, oTarget0));
            EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget0));
            EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 0}));
            for (size_t stIndex = 0; stIndex < vstFramebuffers.count() - 1; ++stIndex)
            {
                rDevice.set_blend(true);
                rDevice.set_depth_test(false);
                rDevice.set_depth_mask(false);
                rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
                rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
                rDevice.set_index_buffer(rDevice.quad_index_buffer());
                rDevice.set_program(rDevice.transition_programs(m_stTransition));
                rDevice.set_uniform(uniform_e::m44Transform, m44Identity);

                target_s oTargetA;
                EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(vstFramebuffers.ref(stIndex), oTargetA));

                target_s oTargetB;
                EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(vstFramebuffers.ref((stIndex + 1)), oTargetB));
                rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTargetA.m_ui32Texture);
                rDevice.set_uniform(uniform_e::t2Texture_1, 1, oTargetB.m_ui32Texture);

                auto const* pWindow = m_vpRenderStack.val((stIndex + 1));
                c_float fFadeFactor = pWindow->fade_factor();
                c_float fFadeSpeed = pWindow->fade_speed();

                //EMP_LOG_DEBUG("FACTOR #0", fFadeFactor);
                rDevice.set_uniform(uniform_e::fProgress, emp::mat::saturate(fFadeFactor * fFadeSpeed));
                rDevice.draw(emp::gfx::primitive_e::Triangles);

                EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(vstFramebuffers.ref(stIndex)));
            }

            EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(vstFramebuffers.last_ref()));
        }
        else if (vstFramebuffers.count() > 0)
        {
            ui32Target0 = vstFramebuffers.val(0);
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target0, oTarget0));

        }
        else
        {
            EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(uiW, uiH, true, ui32Target0));
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target0, oTarget0));
            EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget0));
            EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 0}));
        }

        ui32 EMP_TT_MAX_VAR(ui32Target1);
        EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(uiW, uiH, true, ui32Target1));
        target_s oTarget1;
        EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target1, oTarget1));
        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget1));
        EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 0}));
        rDevice.set_blend(true);
        rDevice.set_depth_test(false);
        rDevice.set_depth_mask(false);
        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);

        for (size_t stIndex = stModal; stIndex < stCount; ++stIndex)
        {
            window_i* const pWindow = m_vpRenderStack.val(stIndex);
            l_draw_modal(pWindow);
            EMP_RET_FALSE_IF_NOT(pWindow->render());
        }

        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());
        //EMP_RET_FALSE_IF_NOT(rDevice.clear({0, 0, 0, 1 }));

        rDevice.set_blend(true);
        rDevice.set_depth_test(false);
        rDevice.set_depth_mask(false);
        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(rDevice.transition_programs(12));
        rDevice.set_uniform(uniform_e::m44Transform, m44Identity);
        rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTarget0.m_ui32Texture);
        rDevice.set_uniform(uniform_e::t2Texture_1, 1, oTarget1.m_ui32Texture);

        c_float fFadeFactor = m_vpRenderStack.val(stModal)->fade_factor();//m_pModal->fade_factor();
        //EMP_LOG_DEBUG("FADE_FACTOR #0", fFadeFactor);// @@0 remove
        rDevice.set_uniform(uniform_e::fProgress, emp::mat::saturate(fFadeFactor));
        rDevice.draw(emp::gfx::primitive_e::Triangles);

        EMP_RET_FALSE_IF_NOT(l_draw_hovers());

        EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(ui32Target0));
        EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(ui32Target1));
    }
    else
    {
        auto& rDevice = EMP_GRAPHICS;

        ui32 uiW = 0;
        ui32 uiH = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(EMP_APPLICATION.view_pixel_width(), uiW));
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(EMP_APPLICATION.view_pixel_height(), uiH));

        if (stCount > 1)
        {
            vector<ui32> vstFramebuffers;

            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                ui32 EMP_TT_MAX_VAR(ui32Target);
                EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(uiW, uiH, true, ui32Target));

                target_s oTarget;
                EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target, oTarget));
                EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget));
                EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 0}));
                vstFramebuffers.add(ui32Target);
                EMP_RET_FALSE_IF_NOT(m_vpRenderStack.val(stIndex)->render());
            }

            EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());
            //EMP_RET_FALSE_IF_NOT(rDevice.clear({0, 0, 0, 1 }));

            for (size_t stIndex = 0; stIndex < vstFramebuffers.count() - 1; ++stIndex)
            {
                rDevice.set_blend(true);
                rDevice.set_depth_test(false);
                rDevice.set_depth_mask(false);
                rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
                rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
                rDevice.set_index_buffer(rDevice.quad_index_buffer());
                rDevice.set_program(rDevice.transition_programs(m_stTransition));
                rDevice.set_uniform(uniform_e::m44Transform, m44Identity);

                target_s oTargetA;
                EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(vstFramebuffers.ref(stIndex), oTargetA));

                target_s oTargetB;
                EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(vstFramebuffers.ref((stIndex + 1)), oTargetB));
                rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTargetA.m_ui32Texture);
                rDevice.set_uniform(uniform_e::t2Texture_1, 1, oTargetB.m_ui32Texture);

                //EMP_LOG_INFO("FADE #0", m_vpRenderStack.val((stIndex + 1))->fade_factor());
                rDevice.set_uniform(uniform_e::fProgress, emp::mat::saturate(m_vpRenderStack.val((stIndex + 1))->fade_factor()));
                rDevice.draw(emp::gfx::primitive_e::Triangles);

                EMP_RET_FALSE_IF_NOT(l_draw_hovers());

                EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(vstFramebuffers.ref(stIndex)));
            }

            EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(vstFramebuffers.last_ref()));
        }
        else
        {
            EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());
            //EMP_RET_FALSE_IF_NOT(rDevice.clear({0, 0, 0, 1 }));
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { EMP_RET_FALSE_IF_NOT(m_vpRenderStack.val(stIndex)->render()); }

            EMP_RET_FALSE_IF_NOT(l_draw_hovers());

        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::modal(void) const
{
    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (m_vpStack.val(stIndex)->is_modal())
        { return true; }
    }
    return false;
    //return m_pModal->state().visible && m_pModal->state().enable;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::is_modal(cpc_control_i a_pWindow) const
{
    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i const* const pWindow = m_vpStack.val(stIndex);
        if (pWindow == a_pWindow)
        { return pWindow->is_modal(); }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::clean(void)
{
    for (size_t stIndex = 0; stIndex < m_vpStack.count();)
    {
        window_i* const pWindow = m_vpStack.val(stIndex);
        EMP_ASSERT(pWindow != nullptr);
        if (pWindow->is_hidden())
        {
            EMP_LOG_INFO("REMOVING");
            pWindow->state().visible_(false);
            m_vpStack.remove(stIndex);
            remove_control(pWindow);
            continue;
        }
        ++stIndex;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::try_remove_window(window_i* const a_pWindow)
{
    m_mpFocuses.set(a_pWindow, nullptr);
    bool bReturn = false;
    for (size_t stIndex = 0; stIndex < m_vpStack.count();)
    {
        if (m_vpStack.val(stIndex) == a_pWindow)
        {
            bReturn = true;
            m_vpStack.remove(stIndex);
            remove_control(a_pWindow);
            continue;
        }
        ++stIndex;
    }
    return bReturn;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::show_window(window_i* const a_pWindow, c_bool a_bTop, c_bool a_bSelect,c_change_e a_eChange, c_bool a_bClear)
{
    // @@0  facto try_remove_window
    bool bReturn = true;
    for (size_t stIndex = 0; stIndex < m_vpStack.count();)
    {
        if (m_vpStack.val(stIndex) == a_pWindow)
        {
            bReturn = false;
            if (a_bTop == true)
            {
                m_vpStack.remove(stIndex);
                remove_control(a_pWindow);
                continue;
            }
        }
        ++stIndex;
    }

    if (bReturn == true || a_bTop == true)
    {
        m_vpStack.add(a_pWindow);
        add_control(a_pWindow);
    }

    size_t stRemoved = 0;
    for (size_t stIndex = 0; stIndex < (m_vpStack.count() - stRemoved);)
    {
        window_i* const pWindow = m_vpStack.val(stIndex);
        if (pWindow->is_top() && stIndex < (m_vpStack.count() - 1))
        {
            m_vpStack.remove(stIndex);
            remove_control(pWindow);

            m_vpStack.add(pWindow);
            add_control(pWindow);
            ++stRemoved;
            continue;
        }
        ++stIndex;
    }

    c_bool bModal = a_pWindow->is_modal();
    if (bModal || a_pWindow->is_floating())
    { m_pModalFocus = m_pFocus; }

    a_pWindow->show_imp();
    //if (m_pModal->state().visible == false || a_bTop == true)
    if (modal() == false || a_bTop == true)
    {
        control_i* pFocus = focus(a_pWindow);
        if (a_bClear)
        {
            if (pFocus != nullptr)
            {
                emp::tt::silent(pFocus->unselect(true));
                pFocus->state().focus_(false);
            }
            pFocus = a_pWindow;
        }
        if (m_mpFocuses.get(a_pWindow).end())
        { m_mpFocuses.set(a_pWindow, pFocus); }
        activate_focus(pFocus, focus_e::First, a_bSelect, false, a_eChange);
        // @@0 device : activate_focus(pFocus, focus_e::First, a_bSelect, true, a_eChange);
    }
    /*if (bModal)
    {
        EMP_LOG_INFO("MODAL------------------------------------------_______________");
        m_pModal->show_imp();
    }*/

    return bReturn;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::hide_window(window_i* const a_pWindow)
{
    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (m_vpStack.val(stIndex) == a_pWindow)
        {
            if (m_pFocus != nullptr && control_in_hierarchy(a_pWindow, m_pFocus))
            { deactivate_focus(); }

            //if (a_pWindow->is_modal())
            //{ m_pModal->hide_imp(); }

            a_pWindow->hide_imp();

            if (m_pModalFocus != nullptr &&
                m_pFocus == nullptr)
            {
                activate_focus(m_pModalFocus, focus_e::First, false, true, change_e::Code);
                m_pModalFocus = nullptr;
            }
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* manager_c::focus(window_i* const a_pValue)
{
    emp::cnt::map_t<window_i*, control_i*>::iterator_c const oIt = m_mpFocuses.get(a_pValue);
    if (oIt != m_mpFocuses.end())
    {
        pc_control_i pFocus = *oIt.value();
        if (pFocus != nullptr)
        { return pFocus; }
    }
    return a_pValue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::set_focus(window_i* const a_pWindow, pc_control_i a_pValue)
{
    if (a_pWindow)
    { m_mpFocuses.set(a_pWindow, a_pValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::activate_focus(pc_control_i a_pControl, c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
{
    EMP_ASSERT(a_pControl != nullptr);

#ifdef EMP_XX_LOUSY
    EMP_LOG_DEBUG("trying to manager_c::activate_focus");
#endif

    if (m_pFocus != a_pControl)
    {
        deactivate_focus();
        control_i* pControl = a_pControl;
        while (m_pFocus == nullptr && pControl != nullptr)
        {
            if (pControl->state().enable &&
                emp::tt::is_not_max(pControl->state().index))
            { m_pFocus = pControl->got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange); }
            pControl = pControl->parent();
        }
    }

    if (m_pFocus == nullptr)
    {
#ifdef EMP_XX_LOUSY
        EMP_LOG_DEBUG("failed to manager_c::activate_focus");
#endif
    }
    else
    {
#ifdef EMP_XX_LOUSY
        EMP_LOG_DEBUG("succeed to manager_c::activate_focus");
#endif
        if (a_bSelect)
        {
            control_i* pParent = m_pFocus->parent();
            while (pParent != nullptr && pParent->unselect(true))
            { pParent = pParent->parent(); }
            m_pFocus->state().select_(true);
        }
        m_pFocus->state().focus_(true);

        control_i* pParent = m_pFocus;
        while (pParent != nullptr)
        {
            pParent->focused();
            pParent = pParent->parent();
        }
    }

    control_i* pParent = m_pFocus;
    while (pParent != nullptr && pParent->parent() != nullptr && (pParent->parent() != this))
    { pParent = pParent->parent(); }

    for (auto it = m_mpFocuses.begin(); it.not_end(); ++it)
    {
        window_i* const pWindow = *it.key();
        if (pWindow != nullptr &&
            pWindow == pParent)
        {
            if (pWindow == m_pFocus || m_pFocus == nullptr || control_in_hierarchy(pWindow, m_pFocus))
            { m_mpFocuses.set(pWindow, m_pFocus); }
        }
    }

    /*c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpStack.val(stIndex);

        if (pWindow != nullptr &&
            (pWindow == m_pFocus || m_pFocus == nullptr ||
            control_in_hierarchy(pWindow, m_pFocus)))
        { m_mpFocuses.set(pWindow, m_pFocus); }
    }*/
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::clear_focus(cpc_control_i a_pControl)
{
    emp::cnt::map_t<window_i*, control_i*>::iterator_c oIt = m_mpFocuses.begin();
    for (; oIt != m_mpFocuses.end(); ++oIt)
    {
        if (*oIt.value() == a_pControl)
        {
            m_mpFocuses.set(*oIt.key(), nullptr);
            //EMP_LOG_DEBUG("UNFOCUS");
        }
    }

    deactivate_focus();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::deactivate_focus(void)
{
#ifdef EMP_XX_LOUSY
    EMP_LOG_DEBUG("manager_c::deactivate_focus");
#endif

    if (m_pFocus != nullptr)
    {
        m_pFocus->state().focus_(false);
        m_pFocus = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool manager_c::dispatch(emp::cnt::vector<emp::app::event_s> const& a_rvEvents, handler_i* a_pHandler)
{
#define XTMP_PROCESS_EVENT(x_Process)\
    {\
        emp::gui::c_return_e eReturn = this->x_Process(rEvent);\
        if (eReturn == emp::gui::return_e::Error)\
        { return false; }\
        if (eReturn == emp::gui::return_e::Consumed)\
        { continue; }\
    }\
    if (a_pHandler != nullptr)\
    {\
        emp::gui::c_return_e eReturn =a_pHandler->x_Process(rEvent);\
        if (eReturn == emp::gui::return_e::Error)\
        { return false; }\
    }

    for (size_t stIndex = 0; stIndex < a_rvEvents.count(); ++stIndex)
    {
        emp::app::c_event_s& rEvent = a_rvEvents.ref(stIndex);
        emp::app::c_event_e eType = rEvent.m_eType;
        if (eType == emp::app::event_e::KeyboardPressed ||
            eType == emp::app::event_e::KeyboardReleased)
        { XTMP_PROCESS_EVENT(process_keyboard); }
        else if (
            eType == emp::app::event_e::MousePressed ||
            eType == emp::app::event_e::MouseReleased ||
            eType == emp::app::event_e::MouseMoved ||
            eType == emp::app::event_e::MouseDragged ||
            eType == emp::app::event_e::MouseEnter ||
            eType == emp::app::event_e::MouseLeave)
        { XTMP_PROCESS_EVENT(process_mouse); }
    }

#undef XTMP_PROCESS_EVENT
    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* get_focusable(
    manager_c const* const a_pManager,
    cpc_control_i a_pParent,
    c_ui32 a_stCurrent, c_bool a_bReverse,
    c_bool a_bDeep,
    ui32& a_rstOut)
{
    ui32 stMin = 0;
    ui32 stMax = 0;

    auto const* const pChildren = a_pParent ? &a_pParent->controls() : nullptr;

    c_size stCount = pChildren ? pChildren->count() : 0;
    if ((stCount > 0) == false)
    { return nullptr; }

    if (get_min_max_index(*pChildren, stMin, stMax))
    {
        ui32 stNext = a_stCurrent != emp::tt::max<ui32>::value ? a_stCurrent : (a_bReverse ? stMax : stMin);

        while (true)
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                pc_control_i pCurrent = pChildren->val(stIndex);
                if (pCurrent->state().visible &&
                    pCurrent->state().enable &&
                    pCurrent->state().index == stNext &&
                    pCurrent->state().index != a_stCurrent)
                {
                    if (a_pManager != nullptr &&
                        a_pManager->modal() &&
                        &a_pManager->controls() == pChildren &&
                        a_pManager->is_modal(pCurrent) == false)
                    { continue; }

                    if (pCurrent->state().focusable)
                    {
                        a_rstOut = emp::ast::cast<ui32>(stIndex);
                        return pCurrent;
                    }
                    else if (a_bDeep)
                    {
                        pc_control_i pNext = get_focusable(a_pManager, pCurrent, emp::tt::max<ui32>::value,
                            a_bReverse, a_bDeep, a_rstOut);
                        if (pNext != nullptr)
                        { return pNext; }
                    }
                }
            }
            if ((a_bReverse && (stNext > stMin) == false) ||
                (a_bReverse == false && (stNext >= stMax)))
            { break; }

            stNext = a_bReverse ? (stNext - 1) : (stNext + 1);
        }
    }

    if (a_bDeep && emp::tt::is_not_max(a_stCurrent))
    { return get_focusable(a_pManager, a_pParent->parent(), a_pParent->state().index, a_bReverse, a_bDeep, a_rstOut); }

    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void build_navigables(manager_c const* const a_pManager, cpc_control_i a_pParent, vector<control_i*>& a_rvpNavigables);
EMP_INTERN void build_navigables(manager_c const* const a_pManager, cpc_control_i a_pParent, vector<control_i*>& a_rvpNavigables)
{
    ui32 stMin = 0;
    ui32 stMax = 0;

    auto const* const pChildren = a_pParent ? &a_pParent->controls() : nullptr;

    c_size stCount = pChildren ? pChildren->count() : 0;
    if ((stCount > 0) == false)
    { return; }

    if (get_min_max_index(*pChildren, stMin, stMax))
    {
        ui32 stNext = stMin;

        while (stNext <= stMax)
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                pc_control_i pCurrent = pChildren->val(stIndex);
                if (pCurrent->state().visible &&
                    pCurrent->state().enable &&
                    pCurrent->state().index == stNext)
                {
                    if (a_pManager->modal() &&
                        &a_pManager->controls() == pChildren &&
                        a_pManager->is_modal(pCurrent) == false)
                    { continue; }

                    if (pCurrent->state().navigable)
                    { a_rvpNavigables.add(pCurrent); }

                    build_navigables(a_pManager, pCurrent, a_rvpNavigables);
                }
            }
            ++stNext;
        }
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN control_i* get_navigable_v2(manager_c const* const a_pManager, cpc_control_i a_pFocus,
    emp::cnt::vector<control_i*>& a_rvpNavigables, c_bool a_bReverse);
EMP_INTERN EMP_RETURN control_i* get_navigable_v2(manager_c const* const a_pManager, cpc_control_i a_pFocus,
    emp::cnt::vector<control_i*>& a_rvpNavigables,
    c_bool a_bReverse)
{
    a_rvpNavigables.resize(0);
    build_navigables(a_pManager, a_pManager, a_rvpNavigables);

    c_size stCount = a_rvpNavigables.count();
    if ((stCount > 0) == false)
    { return nullptr; }

   size_t stCurrent = 0;
    c_control_i* pCurrent = nullptr;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stCount - stIndex - 1;
        cpc_control_i pNavigable =
            a_rvpNavigables.val(stOffset);
        //EMP_LOG_DEBUG("________V2 #0, index #1, offset #2",
          //  pNavigable->name(), stIndex, stOffset);
        if (control_in_hierarchy(pNavigable, a_pFocus) ||
            pNavigable == a_pFocus)
        {
            if (pCurrent != nullptr)
            {
                if (control_in_hierarchy(pCurrent, pNavigable))
                {
                    //EMP_LOG_DEBUG("CONTAINS FOCUS");
                    stCurrent = stOffset;
                    pCurrent = pNavigable;
                }
            }
            else
            {
            //EMP_LOG_DEBUG("FOUND FOCUS #0, #1",pNavigable->name(), stOffset);
            stCurrent = stOffset;
            pCurrent = pNavigable;
            }
            //break;
        }
    }

    stCurrent = a_bReverse ?
        (stCurrent == 0 ? stCount : stCurrent) - 1 :
        (stCurrent + 1 >= stCount ? 0 : stCurrent + 1);

    //if (a_pFocus)
    //{
    //    EMP_LOG_INFO("FOCUS #0", a_pFocus->name());
    //}
    //EMP_LOG_INFO("CURRENT #0, #1",
        //a_rvpNavigables.val(stCurrent)->name(), stCurrent);
    return a_rvpNavigables.val(stCurrent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN control_i* get_navigable(
    manager_c const* const a_pManager,
    cpc_control_i a_pParent,
    c_size a_stCurrent, c_bool a_bReverse);
EMP_INTERN EMP_RETURN control_i* get_navigable(
    manager_c const* const a_pManager,
    cpc_control_i a_pParent,
    c_size a_stCurrent, c_bool a_bReverse)
{
    size_t stMin = 0;
    size_t stMax = 0;

    emp::cnt::vector<control_i*> const* const pChildren =
        a_pParent ? a_pParent->controls() : nullptr;

    c_size stCount = pChildren ? pChildren->count() : 0;
    if ((stCount > 0) == false)
    { return nullptr; }

    if (get_min_max_index(*pChildren, stMin, stMax))
    {
        size_t stNext = emp::tt::is_not_max(a_stCurrent) ? a_stCurrent : (a_bReverse ? stMax : stMin);

        while (true)
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                pc_control_i pCurrent = pChildren->val(stIndex);
                if (pCurrent->state().visible() &&
                    pCurrent->state().enable() &&
                    pCurrent->state().index() == stNext &&
                    pCurrent->state().index() != a_stCurrent)
                {
                    if (a_pManager->modal() &&
                        a_pManager->controls() == pChildren &&
                        a_pManager->is_modal(pCurrent) == false)
                    { continue; }

                    if (pCurrent->state().navigable())
                    {
                        return pCurrent;
                    }
                    else
                    {
                        pc_control_i pNext = get_navigable(a_pManager, pCurrent, emp::tt::max<size_t>::value, a_bReverse);
                        if (pNext != nullptr)
                        { return pNext; }
                    }
                }
            }
            if ((a_bReverse && (stNext > stMin) == false) ||
                (a_bReverse == false && (stNext >= stMax)))
            { break; }

            stNext = a_bReverse ? (stNext - 1) : (stNext + 1);
        }
    }

    if (emp::tt::is_not_max(a_stCurrent))
    {
        return get_navigable(
            a_pManager,
            a_pParent->parent(),
            a_pParent->state().index(), a_bReverse);
    }

    return nullptr;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e manager_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    auto const& rKeyboard = a_rEvent.keyboard;

    if (rKeyboard.m_bPressed)
    {
        c_bool bAlt = rKeyboard.m_bAlt;
        c_bool bShift = rKeyboard.m_bShift;
        c_bool bCtrl = rKeyboard.m_bCtrl;

        emp::app::c_key_e eKey = rKeyboard.m_eKey;

        if (eKey == emp::app::key_e::Enter)
        {
            if (bCtrl == false && bAlt && bShift == false)
            {
                // @@0 remove (*m_pFctOnAltEnter)();
                // @@0  hardcode
                // @@0  tbr
                //emp::gui::skin_c& rSkin = EMP_GAME().gui_skin();
                //EMP_GAME().set_skin(&rSkin);
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
        }
        else if (
            eKey == emp::app::key_e::Left ||
            eKey == emp::app::key_e::Up)
        {
            if (bCtrl == false && bShift && bAlt == false)
            {
                if (on_scale.is_valid())
                { on_scale(true); }
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
        }
        else if (
            eKey == emp::app::key_e::Right ||
            eKey == emp::app::key_e::Down)
        {
            if (bCtrl == false && bShift && bAlt == false)
            {
                if (on_scale.is_valid())
                { on_scale(false); }
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
        }
        else if (eKey == emp::app::key_e::Tabulation)
        {
            if (bAlt == false && m_pFocus != nullptr)
            {
                control_i* pControl = nullptr;

                c_focus_e eFocus = bShift ? focus_e::Last : focus_e::First;

                c_bool bPush = bCtrl == false;

                if (bCtrl)
                {
                    ui32 stOut = emp::tt::max<ui32>::value;
                    pControl = get_focusable(this, m_pFocus->parent(), m_pFocus->state().index, bShift, true, stOut);
                    if (pControl == nullptr &&
                        m_pModalFocus != nullptr &&
                        modal() == false)
                    { pControl =get_focusable(this,m_pModalFocus->parent(),m_pModalFocus->state().index,bShift,true,stOut); }

                    if (pControl == nullptr)
                    { pControl = get_focusable(this, this, emp::tt::max<ui32>::value, bShift, true, stOut); }
                }
                else
                { pControl = get_navigable_v2(this, m_pFocus ? m_pFocus : m_pModalFocus, m_vpNavigables, bShift); }

                if (pControl != nullptr)
                { activate_focus(pControl, eFocus, true, bPush, change_e::Keyboard); }
            }
        }
    }

    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpStack.val(stCount - stIndex - 1);
        //if (pWindow->is_modal())// == m_pModal)
        //{ break; }
        if (pWindow->state().visible &&
            pWindow->state().enable)
        {
            c_return_e eReturn = pWindow->process_keyboard(a_rEvent);
            if (eReturn != return_e::Unused)
            { return eReturn; }
        }

        if (pWindow->is_modal() && pWindow->is_overlay())
        { break; }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void manager_c::find_hover_candidates(emp::app::c_event_s& a_rEvent, T const& a_rvpControls, c_bool a_bContinue)
{
    c_size stCount = a_rvpControls.count();
    if (stCount > 25 && a_rEvent.keyboard.modified()) // @@0 remove
    {
        int kk = 0;
        kk++;
    }
    c_control_i* pMax = nullptr;
    float EMP_TT_MIN_VAR(fMax);

    auto const l_compute_area = [&a_rEvent, this](cpc_control_i a_pChild EMP_UNUSED) EMP_RETURN -> float
    {
        c_v2f v2LeftBottom = emp::gui::gui_to_gl(a_pChild->transformation().matrix, {0, -a_pChild->transformation().size.y } );
        c_v2f v2RightTop = emp::gui::gui_to_gl(a_pChild->transformation().matrix, {a_pChild->transformation().size.x, 0});

        c_v2f v2MouseLeftBottom = v2f::sub(a_rEvent.mouse.m_v2fPosition, cursor_size );
        c_v2f v2MouseRightTop = v2f::add(a_rEvent.mouse.m_v2fPosition, cursor_size);

        c_float fLastLeft = v2LeftBottom.x;
        c_float fLastBottom = v2LeftBottom.y;

        c_float fLastRight = v2RightTop.x;
        c_float fLastTop = v2RightTop.y;

        c_float fLeft = v2MouseLeftBottom.x;
        c_float fBottom = v2MouseLeftBottom.y;

        c_float fRight = v2MouseRightTop.x;
        c_float fTop = v2MouseRightTop.y;

        if (fRight < fLastLeft ||
            fLeft > fLastRight ||
            fTop < fLastBottom ||
            fBottom > fLastTop)
        { return emp::tt::min<float>::value; }

        if (fLeft >= fLastLeft && fRight <= fLastRight &&
            fBottom >= fLastBottom && fTop <= fLastTop)
        { return (fRight - fLeft) * (fTop - fBottom); }

        c_float fCurLeft = emp::mat::maxof(fLeft, fLastLeft);
        c_float fCurRight = emp::mat::minof(fRight, fLastRight);
        c_float fCurBottom = emp::mat::maxof(fBottom, fLastBottom);
        c_float fCurTop = emp::mat::minof(fTop, fLastTop);

        c_float fCurW = emp::mat::abs(fCurRight - fCurLeft);
        c_float fCurH = emp::mat::abs(fCurTop - fCurBottom);

        return fCurW * fCurH;
    };

    auto const l_find_hover_candidates = [this, &a_rEvent](cpc_control_i a_pCandidate, c_float a_fValue)
    {
        hover_candidates.set(&a_pCandidate->transformation(), a_fValue);
        find_hover_candidates(a_rEvent, a_pCandidate->controls(), false);
    };

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stCount - stIndex - 1;
        cpc_control_i pControl = a_rvpControls.val(stOffset);
        if (pControl->state().renderable)
        {
            float fCurrent = l_compute_area(pControl);
            if (a_bContinue)
            { l_find_hover_candidates(pControl, fCurrent); }
            else if (fCurrent > fMax && (state_focusable(pControl) || state_hoverable(pControl)))
            {
                pMax = pControl;
                fMax = fCurrent;
            }
        }
    }

    if (pMax != nullptr)
    { l_find_hover_candidates(pMax, fMax); }
}
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e manager_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (a_rEvent.mouse_pressed()) // @@0 remove
    {
        int x = 0;
        ++x;
    }
    //if (a_rEvent.mouse_moved())
    {
        hover_candidates.clear();

        if (a_rEvent.mouse_event())
        { find_hover_candidates(a_rEvent, m_vpStack, true); }

        for (auto it = hover_candidates.begin(); it.not_end(); ++it)
        { hover_quads.set(*it.key(), true); }

        if (a_rEvent.mouse_moved() ||
            a_rEvent.mouse_dragged())
        { mouse_position = a_rEvent.mouse.m_v2fPosition; }
    }

    emp::app::c_mouse_event_s& rMouse = a_rEvent.mouse;
    emp::app::c_button_e eButton = rMouse.m_eButton;

    emp::app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;
    if (a_rEvent.mouse_pressed() &&
        rKeyboard.m_bShift &&
        rKeyboard.m_bCtrl == false &&
        rKeyboard.m_bAlt == false)
    {
        if (eButton == emp::app::button_e::WheelUp)
        {
            if (on_scale.is_valid())
            { on_scale(true); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eButton == emp::app::button_e::WheelDown)
        {
            if (on_scale.is_valid())
            { on_scale(false); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    c_size stCount = m_vpStack.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        window_i* const pWindow = m_vpStack.val(stCount - stIndex - 1);
        //if (pWindow->is_modal())//== m_pModal)
        //{ break; }
        if (pWindow->state().visible &&
            pWindow->state().enable)
        {
        if (a_rEvent.mouse_pressed()) // @@0 remove
        {
        int kk = 0;
        ++kk;
        }
            c_return_e eReturn = pWindow->process_mouse(a_rEvent);
            if (eReturn != return_e::Unused)
            { return eReturn; }
        }
        if (pWindow->is_modal() && pWindow->is_overlay() == false && a_rEvent.type != emp::app::event_e::MouseLeave && a_rEvent.type != emp::app::event_e::MouseEnter)
        { break; }
    }

    if (eButton == emp::app::button_e::Left &&
        a_rEvent.mouse_released() && a_rEvent.mouse.m_stToucheCount == 1 && a_rEvent.mouse.m_stTapCount == 1)
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            window_i* const pWindow = m_vpStack.val(stIndex);
            if (pWindow->is_floating() && pWindow->is_hidden() == false &&
                pWindow->state().hover == false)
            { emp::tt::silent(hide_window(pWindow)); }
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::push_scissor(c_bool a_bClear, transformation_data_s const& a_rTransform, bool& a_rbEmpty, c_float a_fLeft, c_float a_fRight, c_float a_fBottom, c_float a_fTop)
{
    c_v4f v4Scissor = EMP_GRAPHICS.scissor();
    m_vv4Scissors.add(v4Scissor);

    c_float fW = emp::ast::cast<float>(EMP_APPLICATION.platform().view_pixel_width());
    c_float fH = emp::ast::cast<float>(EMP_APPLICATION.platform().view_pixel_height());

    v2f v2Client = a_bClear ? v2f({-1, 1}) : emp::gui::gui_to_gl(a_rTransform.matrix, {0, 0});
    v2f v2Size = v2f::mul(a_rTransform.size, 0.5f);
    v2f v2BottomLeft = v2f::max({0, 0}, v2f::sub(v2f::mul(v2f::add(v2Client, {1, 1}), 0.5f), {0, v2Size.y}));
    v2f v2TopRight = v2f::min({1, 1}, v2f::add(v2BottomLeft, v2Size));
    v2f v2FinalSize = v2f::sub(v2TopRight, v2BottomLeft);

    c_float fSX = v2BottomLeft.x * (fW - 1);
    c_float fSY = v2BottomLeft.y * (fH - 1);
    c_float fSW = v2FinalSize.x * fW;
    c_float fSH = v2FinalSize.y * fH;

    c_float fRight = fSX + fSW;
    c_float fTop = fSY + fSH;

    c_float fLastX = v4Scissor.x;
    c_float fLastY = v4Scissor.y;
    c_float fLastW = v4Scissor.z;
    c_float fLastH = v4Scissor.w;

    c_float fLastRight = fLastX + fLastW;
    c_float fLastTop = fLastY + fLastH;

    if (fRight < fLastX ||
         fSX > fLastRight ||
        fTop < fLastY ||
         fSY > fLastTop)
    {
        EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.scissor(v4Scissor, a_rbEmpty));
        return;
    }

    if (fSX >= fLastX && fRight <= fLastRight &&
        fSY >= fLastY && fTop <= fLastTop)
    {
        EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.scissor(fSX + a_fLeft, fSY + a_fBottom, fSW + a_fRight, fSH + a_fTop, a_rbEmpty));
        return;
    }

    c_float fCurLeft = a_bClear ? fSX : emp::mat::maxof(fSX, fLastX);
    c_float fCurRight = a_bClear ? fRight : emp::mat::minof(fRight, fLastRight);
    c_float fCurBottom = a_bClear ? fSY : emp::mat::maxof(fSY, fLastY);
    c_float fCurTop = a_bClear ? fTop : emp::mat::minof(fTop, fLastTop);

    c_float fCurW = fCurRight - fCurLeft;
    c_float fCurH = fCurTop - fCurBottom;

    EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.scissor(fCurLeft + a_fLeft, fCurBottom + a_fBottom, fCurW + a_fRight, fCurH + a_fTop, a_rbEmpty));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void manager_c::pop_scissor(void)
{
    bool bEmpty = false;
    EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.scissor(m_vv4Scissors.last_ref(), bEmpty));
    m_vv4Scissors.remove_last();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN manager_c* manager(void)
{ return EMP_APPLICATION.gui_manager(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

