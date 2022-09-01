//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_selection.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_selection.h"
#include "ase_cor_game.h"
#include "ase_cor_option.h"
#include "ase_cor_reflection.rfx.h"

#include "ase_gui_skin.h"

#include "ase_mod_evolution.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

#include "emp_fs_file.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_list.h"
#include "emp_gui_manager.h"
#include "emp_gui_mdi.h"
#include "emp_gui_menu.h"
#include "emp_gui_picture.h"
#include "emp_gui_window.h"

#include "emp_ose_processor.h"
#include "emp_ose_program.h"
#include "emp_ose_renderer.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose { class renderlet_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::tim;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class selection_c;
class thumbnail_c;
class viewer_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, viewer_c, public, emp::gui::docker_c)
public:
    picture_c pic_crop{};
    picture_c pic_remove{};
    picture_c pic_variant{};

    spacer_c spacer_0{};
    spacer_c spacer_1{};

    emp::tim::time_c current_time{};
    emp::tim::time_c magnified_slide_time{};
    emp::tim::time_c magnified_zoom_last_wheel_down{};

    thumbnail_c* magnified = nullptr;
    selection_c* selection = nullptr;

    size_t EMP_TT_MAX_VAR(remove_variant);

    size_t EMP_TT_MAX_VAR(magnified_index);

    v2f magnified_scale_fit = {0.0f, 0.0f};
    v2f magnified_size = {0, 0};
    v2f magnified_drag_origin = {0, 0};
    v2f magnified_drag_destination = {0,0};
    v2f magnified_drag_velocity = {0, 0};
    v2f magnified_drag_speed = {0, 0};
    v2f magnified_center = {0, 0};
    v3f magnified_offset = {-1, 1, 0};

    float magnified_scale = 1;
    float magnified_old_scale = 1;
    float magnified_direction = 0;
    float magnified_speed = 1.0f;
    float magnified_time = 0.0f;
    float magnified_scale_ratio = 0;
    float magnified_fit_width = 0;
    float magnified_fit_height = 0;

    bool magnified_fit = false;
    bool magnified_zoom = false;
    bool magnified_slide_horz = false;

    viewer_c(void)
    {}

    EMP_RETURN bool create(selection_c* const a_pSelection);
    void destroy(void);

    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool transform(emp::gui::stack_c& a_rStack)
    {
        EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));
        magnified_offset.z = emp::gui::depth(transformation().back_depth, 0);
        return true;
    }

    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    //<< handler_i

    void start(void);

    EMP_RETURN return_e on_new_variant(cpc_control_i a_pControl, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_remove_variant(cpc_control_i a_pControl, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_crop_variant(cpc_control_i a_pControl, c_bool a_bValue, c_change_e a_echange);

    void clear_magnified(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, selection_c, public, splitter_c)
private:
    friend class thumbnail_c;
    friend class viewer_c;

    viewer_c viewer{};
    ase::cor::pool_c* temp = nullptr;
    emp::gui::line_c* temp_line = nullptr;
    bool temp_used = false;
    thumbnail_c* dragged = nullptr;
    size_t EMP_TT_MAX_VAR(dragged_index);

    thumbnail_c* m_pRunning = nullptr;

    v2f dragged_delta{};
    float scroll = 0.0f;

    bool previous_edition = false;

    registrar_i* registrar = nullptr;

    bool cut = false;
    bool discard = false;

    list_c list{};
    mdi_c mdi{};

    emp::tim::time_c discard_time{};

    c_selection_skin_s* skin = nullptr;

    vector<emp::rfx::type_i*> m_vpReflections{};
    vector<ase::cor::pool_c*> m_vpPools{};

    map_t<astring, bool> m_msFiles;

    vector<thumbnail_c*> m_vpThumbnails{};

    emp::ose::renderlet_c m_Renderlet{};
    //size_t m_stCount = 0;

    EMP_POINTER(emp::gfx::buffer_t<v3f, v2f>) m_pVertexBuffer{};

public:
    selection_c(void);
    ~selection_c(void);

public:
    EMP_RETURN bool create(registrar_i* const a_pRegistrar);
    void destroy(void);

public:
    void restore_pool(void);

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(emp::gui::stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void lock_running(thumbnail_c* const a_pValue);
    void unlock_running(thumbnail_c* const a_pValue);
    void unlock_running(void);

public:
    void evolution(c_astring& a_sFile);

public:
    selection_c& skin_(cpc_selection_skin_s a_pValue);

    void populate_pool_list(c_ui32 a_ui32Selection, c_change_e a_eChange);

    void on_source_removed(c_astring& a_sFile);

    void on_user_changed(ase::cor::user_c* const a_pPrevious, ase::cor::user_c* const a_pCurrent);
    EMP_RETURN bool on_pool_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);
    EMP_RETURN bool on_pool_double_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);

private:
    void clear_drag();

    EMP_RETURN bool insert_source(c_size a_stIndex, c_astring& a_sSource, c_bool a_bPreload);
    void remove_source(c_astring& a_sFile);

    EMP_RETURN thumbnail_c* insert_thumbnail(c_size a_stIndex, c_astring& a_rsFile);
    EMP_RETURN thumbnail_c* new_thumbnail(c_astring& a_rsFile, c_size a_stSource);


    EMP_RETURN bool drag(thumbnail_c* const a_pValue, c_size a_stIndex);
    EMP_RETURN emp::gui::line_c* add_pool(ase::cor::pool_c* const a_pPool, c_change_e a_eChange);
    void remove_pool(ase::cor::pool_c* const a_pPool);

    EMP_RETURN bool set_value(c_size a_stIndex, c_u8string& a_rsu8Value);
    EMP_RETURN bool get_value(c_size a_stIndex, u8string& a_rsu8Value);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, thumbnail_c, public, docker_c)
public:
    docker_c header{};
    picture_c remove{};
    EMP_FIELD(bool, cut, false)
    bool preload = false;

private:
    friend class selection_c;
    friend class viewer_c;

private:
    selection_c* selection = nullptr;
    emp::ose::renderlet_c* renderlet = nullptr;

    ui32 texture = 0;
    bool failed = false;
    float failed_color = 0;
    ui32 m_ui32CurrentScale = 0;

    astring m_sOse{};
    astring m_sFilename{};
    astring m_sCurrentPngPath{};
    astring m_sMaxPngPath{};

    float m_fWidth = 0;
    float m_fHeight = 0;

public:
    thumbnail_c(void)
    {}

    ~thumbnail_c(void)
    { destroy(); }

public:
    EMP_RETURN bool create(selection_c* a_pSelection, c_astring& a_sOse)
    {
        destroy();

        m_sOse = a_sOse;

        astring sExtension;
        emp::str::extract_E(m_sOse, sExtension);
        EMP_RET_FALSE_IF_NOT(emp::str::cmp(astring(ALITERAL("ose")), sExtension));

        m_sFilename.clear();
        m_sFilename = m_sOse;
        m_sFilename.trim_right(4);

        c_f64 dScreenWidth = EMP_APPLICATION.platform().screen_pixel_width();
        c_f64 dScreenHeight = EMP_APPLICATION.platform().screen_pixel_height();
        EMP_RET_FALSE_IF(dScreenWidth <= 0 || dScreenHeight <= 0);

        m_sMaxPngPath = STR_AFORMAT("#0png/#1_#2x#3.png", EMP_APPLICATION.resource_path(), m_sFilename, ASE_GAME.ose_renderer().m_ui32MaxWidth, ASE_GAME.ose_renderer().m_ui32MaxHeight);

        selection = a_pSelection;

        remove.state().dock_(dock_e::Right).index_(0).focusable_(true).autosize.xy(true, true);
        remove.on_push.function = emp::fct::function(this, &thumbnail_c::on_source_removed);

        header.state().dock_(dock_e::Top).index_(0).navigable_(true).autosize.xy(false, true);
        header.add_control(&remove);

        add_control(&header);

        EMP_RET_FALSE_IF(selection == nullptr);
        EMP_RET_FALSE_IF((m_sOse.len() > 0) == false);

        return true;
    }

    EMP_RETURN return_e on_source_removed(cpc_control_i, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
    {
        if (a_bValue)
        {
            selection->on_source_removed(astring(m_sOse));
            return return_e::Consumed;
        }
        return return_e::Unused;
    }

    void destroy(void)
    {
        if (selection != nullptr && this == selection->m_pRunning)
        { selection->unlock_running(this); }

        unload_texture();
        selection = nullptr;
    }

public:
    EMP_RETURN c_astring& ose(void) const { return m_sOse; }

public:
    //>> control_i
    /*EMP_RETURN bool resize(void)
    {
        if (state().visible == false)
        { return true; }

        // @@0 ??? c_float fX = skin != nullptr ? skin->spacing->x() : 0;
        // @@0 ??? c_float fY = skin != nullptr ? skin->spacing->y() : 0;

        c_float fX = 0;
        c_float fY = 0;

        c_dock_e eDock = state().dock;

        if (state().autosize.y)
        { transformation().height_(0); }
        if (state().autosize.x)
        { transformation().width_(0); }

        if (eDock == dock_e::Top ||
            eDock == dock_e::Bottom)
        { transformation().height_(fY); }
        else if (
            eDock == dock_e::Left ||
            eDock == dock_e::Right)
        { transformation().width_(fX); }

        return true;
    }*/

    void unload_texture(void)
    {
        if (texture != 0 && emp::tt::is_not_max(texture))
        {
            EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.unlock_texture(texture));
            texture = 0;
        }

        if (selection != nullptr && this == selection->m_pRunning)
        { selection->unlock_running(this); }

        m_fWidth = 0;
        m_fHeight = 0;
        m_ui32CurrentScale = 0;
    }

    EMP_RETURN bool reload_texture(c_float a_fW, c_float a_fH)
    {
        if (a_fW <= m_fWidth && a_fH <= m_fHeight)
        { return true; }

        if (a_fW <= 0 || a_fH <= 0)
        { return true; }

        auto oFinally = emp::tt::finally([this]()
        {
            if (this == selection->m_pRunning)
            { selection->unlock_running(this); }
        });

        c_f64 dThumbWidth = 0.5 * a_fW * EMP_APPLICATION.platform().view_pixel_width();

        c_f64 dScreenWidth = EMP_APPLICATION.platform().screen_pixel_width();
        c_f64 dScreenHeight = EMP_APPLICATION.platform().screen_pixel_height();

        ui32 ui32ScreenWidth = 0;;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dScreenWidth, ui32ScreenWidth));
        ui32 ui32ScreenHeight = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dScreenHeight, ui32ScreenHeight));

        ui32 ui32Scale = 1;
        while (dThumbWidth > (1 << ui32Scale))
        { ++ui32Scale; }

        c_ui32 ui32TargetWidth = 1 << ui32Scale;
        c_ui32 ui32TargetHeight = 1 << ui32Scale;

        if (ui32Scale != m_ui32CurrentScale)
        { m_sCurrentPngPath = STR_AFORMAT("#0png/#1_#2x#3.png", EMP_APPLICATION.resource_path(), m_sFilename, ui32TargetWidth, ui32TargetHeight); }

        if ((texture == 0 || (emp::tt::is_not_max(texture) && ui32Scale !=  m_ui32CurrentScale)) /*&& selection->m_pRunning == nullptr*/)
        {
            unload_texture();

            //selection->lock_running(this);

            bool bExists = false;
            EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(m_sMaxPngPath, bExists));
            if (bExists)
            {
                if (ui32TargetWidth >= ui32ScreenWidth ||
                    ui32TargetWidth >= ui32ScreenHeight)
                {
                    EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.lock_texture(m_sMaxPngPath, texture, EMP_GUI_SKIN.data->ui32_link, true));
                    m_ui32CurrentScale = ui32Scale;
                    m_fWidth = a_fW;
                    m_fHeight = a_fH;
                }
                else
                {
                    astring sThumbFile = STR_AFORMAT("#0_#1x#2.png", m_sFilename, ui32TargetWidth, ui32TargetHeight);
                    astring sThumbPath = STR_AFORMAT("#0png/thumbs/#1", EMP_APPLICATION.resource_path(), sThumbFile);
                    bool bThumbExists = false;
                    EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sThumbPath, bThumbExists));
                    if (bThumbExists == false)
                    {
                        auto& rDevice = EMP_GRAPHICS;
                        gl_framebuffer_s oFramebuffer = rDevice.current_framebuffer();

                        emp::tt::silent(EMP_GRAPHICS.lock_texture(m_sMaxPngPath, texture, EMP_GUI_SKIN.data->ui32_link, true));

                        ui32 EMP_TT_MAX_VAR(ui32Target);
                        EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(ui32TargetWidth, ui32TargetHeight, false, ui32Target));

                        target_s oTarget;
                        EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(ui32Target, oTarget));
                        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget));
                        EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 0}));

                        rDevice.set_blend(true);
                        rDevice.set_depth_test(false);
                        rDevice.set_depth_mask(false);
                        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
                        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
                        rDevice.set_index_buffer(rDevice.quad_index_buffer());
                        rDevice.set_program(rDevice.texture_program());

                        m44f m44Identity;
                        m44f::identity(m44Identity);
                        rDevice.set_uniform(uniform_e::m44Transform, m44Identity);
                        rDevice.set_uniform(uniform_e::t2Texture_0, 0, texture);
                        rDevice.set_uniform(uniform_e::v4Color, v4f({1, 1, 1, 1}));
                        rDevice.draw(emp::gfx::primitive_e::Triangles);

                        EMP_RET_FALSE_IF_NOT(ASE_GAME.ose_renderer().save(ui32Target, sThumbPath));
                        EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(ui32Target));
                        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oFramebuffer.m_ui32Index, oFramebuffer.m_siW, oFramebuffer.m_siH));

                        unload_texture();
                    }
                    else
                    {
                        //selection->unlock_running(this);
                    }

                    EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sThumbPath, bThumbExists));
                    if (bThumbExists)
                    {
                        emp::tt::silent(EMP_GRAPHICS.lock_texture(sThumbPath, texture, EMP_GUI_SKIN.data->ui32_link, true));
                        m_ui32CurrentScale = ui32Scale;
                        m_fWidth = a_fW;
                        m_fHeight = a_fH;
                        //emp::tt::silent(EMP_GRAPHICS.lock_texture(path, texture, EMP_GUI_SKIN.data->ui32_link));
                    }
                }
            }
            else
            {
                unload_texture();
                EMP_TT_MAX_VAR(texture);
            }
        }

        if (emp::tt::is_max(texture) && failed == false &&
        (selection->m_pRunning == nullptr || selection->m_pRunning == this))
        {
//            if (this != selection->m_pRunning)
            {
  //              if (selection->m_pRunning != nullptr)
    //            { selection->unlock_running(); }
                selection->lock_running(this);
            }

            if (renderlet != nullptr)
            {
                if (renderlet->m_bCompleted == false)
                {
                    if (renderlet->m_pProgram == nullptr)
                    {
                        renderlet->m_sMother = m_sOse;
#if defined EMP_OSE_RENDERING
                        if (ASE_GAME.ose_renderer().load(*renderlet, 1.0f/*EMP_GUI_SKIN.resolution.current()*/, true) == false)
                        { failed = true; }
#endif
                        if (renderlet->m_pProgram == nullptr)
                        { selection->unlock_running(this); }
                        else
                        {
                            //m_Renderlet.m_pStack->copy(m_Renderlet.m_pProgram);
                        }
                    }

                    if (renderlet != nullptr && renderlet->m_pProgram != nullptr)
                    {
                        ASE_GAME.ose_renderer().reset();
                        ASE_GAME.ose_renderer().run(*renderlet,
                                                    emp::ose::renderer_c::sc_ui32RenderInstructions,
                                                    emp::ose::renderer_c::sc_fRenderTime,
                                                    emp::ose::renderer_c::sc_ui32RenderBytes);
                        ASE_GAME.ose_renderer().transform(*renderlet, transformation().size);

                        auto& rDevice = EMP_GRAPHICS;
                        gl_framebuffer_s oFramebuffer = rDevice.current_framebuffer();

                        m44f mModel;
                        m44f::identity(mModel);

                        EMP_RET_FALSE_IF_NOT(ASE_GAME.ose_renderer().renderU(*renderlet, mModel, ASE_GAME.ose_renderer().m_ui32MaxWidth, ASE_GAME.ose_renderer().m_ui32MaxHeight));
                        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oFramebuffer.m_ui32Index, oFramebuffer.m_siW, oFramebuffer.m_siH));
                    }
                }
                else
                {
                    if (renderlet->m_pProgram != nullptr)
                    {
                        {
                            astring sOsePath;
                            ASE_GAME.ose_renderer().get_filepath(renderlet->m_sMother, sOsePath);
                            astring sBuffer;
                            EMP_RET_FALSE_IF_NOT(ASE_GAME.ose_renderer().save(*renderlet, sOsePath, sBuffer, true, true, true));
                        }

                        c_ui32 uiTarget = emp::tt::is_not_max(renderlet->m_uiTarget0) ? renderlet->m_uiTarget0 : renderlet->m_uiTarget1;
                        EMP_RET_FALSE_IF(emp::tt::is_max(uiTarget));
                        EMP_RET_FALSE_IF_NOT(ASE_GAME.ose_renderer().save(uiTarget, m_sMaxPngPath));
                        renderlet->m_bCompleted = false;
                        selection->unlock_running(this);
                        EMP_ASSERT(emp::tt::is_max(texture));
                        texture = 0;
                    }
                }
            }
        }

        oFinally.release();
        return true;
    }

    EMP_RETURN bool transform(stack_c& a_rStack)
    {
        if (preload == false)
        {
            unload_texture();
            return true;
        }

        EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

        emp::gui::matrix_stacker_c oStacker(a_rStack, transformation(), state().enable);
        //emp::gui::compute_container_color(*skin, state(), state(), a_rStack.enabled.current(), color());
        update_colors_current(a_rStack.enabled.current());

        return reload_texture(transformation().size.x, transformation().size.y);
    }

    void render_thumb(c_m44f& a_rTransform, c_float a_fAlpha)
    {
        if (emp::tt::is_max(texture) && (renderlet == nullptr ||
            (emp::tt::is_max(renderlet->m_uiTarget0) &&
            emp::tt::is_max(renderlet->m_uiTarget1))))
        { return; }

        auto& rDevice = EMP_GRAPHICS;
        rDevice.set_blend(true);
        rDevice.set_depth_test(true);
        rDevice.set_depth_mask(true);
        rDevice.glBlendFunc_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rDevice.set_vertex_buffer(selection->m_pVertexBuffer);
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(rDevice.texture_program());
        rDevice.set_uniform(uniform_e::m44Transform, a_rTransform);

        if (emp::tt::is_not_max(texture))
        { rDevice.set_uniform(uniform_e::t2Texture_0, 0, texture); }
        else
        {
            c_ui32 uiTarget = emp::tt::is_not_max(renderlet->m_uiTarget0) ? renderlet->m_uiTarget0 : renderlet->m_uiTarget1;

            target_s oTarget;
            EMP_ASSERT_BOOL_CALL(rDevice.framebuffer(uiTarget, oTarget));
            rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTarget.m_ui32Texture);
        }

        if (a_fAlpha < 1.0f) // @@0 remove
        {
            int fuck = 0;
            ++fuck;
        }
        rDevice.set_uniform(uniform_e::v4Color, v4f({1, 1, 1, a_fAlpha}));
        rDevice.draw(emp::gfx::primitive_e::Triangles);
    }

    EMP_RETURN bool render(void)
    {
        if (state().visible == false || state().renderable == false)
        { return true; }

        bool bEmpty = false;
        EMP_GUI_MANAGER_CLIP(oScissor, false, transformation(), bEmpty, 0, 0, 0, 0);
        if (bEmpty)
        { return true; }

        c_float fDepth = emp::gui::depth(transformation().back_depth, 0.0015f);

        if (texture > 0 && emp::tt::is_not_max(texture))
        {
            m44f mTransform;
            m44f::identity(mTransform);
            m44f::translate(mTransform, {0, 0, fDepth }, mTransform);
            m44f::mul(transformation().matrix, mTransform, mTransform);

            if (cut) // @@0 remove
            {
                int kk = 0;
                ++kk;
            }
            render_thumb(mTransform, cut ? 0.2f : 1.0f);
        }
        else
        {
            if (renderlet != nullptr) // @@0 remove
            {
                int kk = 0;
                ++kk;
            }
            c_v2f& v2Size = transformation().size;
            c_float fWidth = (renderlet != nullptr && renderlet->m_bCompleted == false) ?
                renderlet->m_fCompleted : (failed ? 1 : 0);

            if (fWidth > 0)
            {
                c_v2f v2GlPos =  { 0                    , 0 - v2Size.y};
                c_v2f v2GlSize = { 0 + v2Size.x * fWidth, 0           };
                c_v4f v4Color = failed ? v4f({failed_color, (1 - failed_color), 1, 1}) : EMP_GUI_SKIN.data->link_a38();
                failed_color = 1 - failed_color;
                emp::gui::draw_quad_from_bounds(transformation().matrix, v2GlPos, v2GlSize, fDepth, v4Color, false);
            }
        }

       EMP_RET_FALSE_IF_NOT(tbase1::render());

        //m_Color.background.raster.render(transformation().matrix());
        //color().background.border.render(transformation(), depth(transformation().back_depth, 0.001f));

//        EMP_RET_FALSE_IF_NOT(tbase1::render());

        return true;
    }
    //<< control_i

public:
    //>> handler_i
    /*EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent)
    {
        if (a_rEvent.mouse_pressed()) // @@0 remove
        {
            int k = 0;
            k++;
        }
        EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));
        if (a_rEvent.mouse_released() && (this == selection->dragged || selection->dragged == nullptr))
        {
            auto const& rMouse = a_rEvent.mouse;
            if (rMouse.m_eButton == emp::app::button_e::Left &&
                rMouse.m_stToucheCount == 1 &&
                rMouse.m_stTapCount == 2 &&
                cut == false)
            {
                if (state().hover)
                {
                    selection->evolution(m_sFile);
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
            }
        }
        return return_e::Unused;
    }*/
    //<< handler_i

    thumbnail_c& skin_(cpc_sizer_skin_s a_pValue)
    {
        //skin = a_pValue;
        tbase1::skin_(a_pValue);
        return *this;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool viewer_c::create(selection_c* const a_pSelection)
{
    selection = a_pSelection;

    pic_variant.state().dock_(dock_e::Top).index_(0).focusable_(true).autosize.xy(true, true);
    pic_variant.on_push.function = emp::fct::function(this, &viewer_c::on_new_variant);
    pic_variant.skin_(EMP_GUI_SKIN_(picture, "pic.column"));
    pic_variant.model_(EMP_GUI_SKIN.data->gfx_plus);
    add_control(&pic_variant);

    spacer_0.state().dock_(dock_e::Top);
    add_control(&spacer_0);

    pic_crop.state().dock_(dock_e::Top).index_(0).focusable_(true).autosize.xy(true, true);
    pic_crop.on_push.function = emp::fct::function(this, &viewer_c::on_crop_variant);
    pic_crop.skin_(EMP_GUI_SKIN_(picture, "pic.column"));
    pic_crop.model_(EMP_GUI_SKIN.data->gfx_off);
    add_control(&pic_crop);

    spacer_1.state().dock_(dock_e::Top);
    add_control(&spacer_1);

    pic_remove.state().dock_(dock_e::Top).index_(0).focusable_(true).autosize.xy(true, true);
    pic_remove.on_push.function = emp::fct::function(this, &viewer_c::on_remove_variant);
    pic_remove.skin_(EMP_GUI_SKIN_(picture, "pic.column"));
    pic_remove.model_(EMP_GUI_SKIN.data->gfx_minus);
    add_control(&pic_remove);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void viewer_c::destroy(void)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e viewer_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    auto const& rMouse = a_rEvent.mouse;
    auto const eButton = rMouse.m_eButton;
    c_size stToucheCount = rMouse.m_stToucheCount;

    switch (eButton)
    {
        case emp::app::button_e::WheelDown:
        case emp::app::button_e::WheelUp:
        {
            if (/*magnified && */a_rEvent.mouse_dragged() == false && a_rEvent.mouse_released())
            {
                if (magnified_fit)
                {
                    if (eButton == emp::app::button_e::WheelDown)
                    {
                        if (emp::tt::equal_(magnified_direction, -1.0f) == false)
                        { magnified_time = 0; }
                        magnified_direction = -1;
                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                    else if (eButton == emp::app::button_e::WheelUp)
                    {
                        if (emp::tt::equal_(magnified_direction, 1.0f) == false)
                        { magnified_time = 0; }
                        magnified_direction = 1;
                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                }
                else if (magnified_zoom)
                {
                    magnified_center = { + a_rEvent.mouse.m_v2fPosition.x,  + a_rEvent.mouse.m_v2fPosition.y };
                    if (eButton == emp::app::button_e::WheelDown)
                    {
                        if (emp::tt::equal_(magnified_direction, -1.0f) == false)
                        { magnified_time = 0; }
                        if (magnified_scale <= magnified_scale_fit.x)
                        {
                            if (emp::tim::time_c::to_seconds(emp::tim::time_c::sub(current_time, magnified_zoom_last_wheel_down)) > 0.1f)
                            {
                                magnified_zoom = false;
                                magnified_fit = true;
                                magnified_direction = 0;
                            }
                            else
                            { magnified_direction = -1; }
                        }
                        else
                        {
                            magnified_direction = -1;
                            magnified_speed += 3.0f;
                        }
                        magnified_speed = emp::tt::minof(50.0f, magnified_speed);
                        magnified_zoom_last_wheel_down = current_time;
                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                    else if (eButton == emp::app::button_e::WheelUp)
                    {
                        magnified_speed += 3.0f;
                        magnified_speed = emp::tt::minof(50.0f, magnified_speed);
                        magnified_direction = 1;
                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                }
            }
            break;
        }
        case emp::app::button_e::Left:
        {
            if (a_rEvent.mouse_pressed())
            {
                magnified_slide_time = emp::tim::time_c();
                magnified_drag_origin = {magnified_offset.x, magnified_offset.y };
                magnified_drag_velocity = {0, 0};
                magnified_drag_speed = {0, 0};
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
            else if (a_rEvent.mouse_dragged())
            {
                if (emp::mat::equal_(magnified_scale, magnified_scale_fit.x))
                {
                    if (magnified_slide_horz == false)
                    {
                        c_v2f v2Delta = a_rEvent.mouse.distance();
                        c_float fAbsX = emp::mat::abs(v2Delta.x);
                        c_float fAbsY = emp::mat::abs(v2Delta.y);
                        if (fAbsX > 0 || fAbsY > 0)
                            { magnified_slide_horz = true; }
                    }

                    if (magnified_slide_horz)
                    { magnified_drag_destination.x = magnified_drag_origin.x + a_rEvent.mouse.distance().x; }
                }
                else
                {
                    magnified_slide_horz = false;
                    magnified_drag_destination = v2f::add(magnified_drag_origin, a_rEvent.mouse.distance());
                }
                magnified_drag_velocity = a_rEvent.mouse.velocity();
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
            else if (a_rEvent.mouse_released())
            {
                if (stToucheCount == 1)
                {
                    /*if (magnified_zoom)
                    {
                        if (magnified_slide_horz)
                        {
                            magnified_slide_time = current_time;
                            magnified_drag_destination.x = -1;
                        }
                        else if (magnified_slide_vert)
                        {
                            magnified_slide_time = current_time;
                            magnified_drag_destination.y = 1;
                        }
                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                }
                else
                {
                    */if (magnified_zoom)
                    {
                        if (magnified_slide_horz)
                        {
                            magnified_slide_time = current_time;
                            c_float fVX = magnified_drag_velocity.x;
                            c_float fDX = a_rEvent.mouse.distance().x;
                            if (emp::mat::abs(fVX) > 0.007f || emp::mat::abs(fDX) > 1.0f)
                            { magnified_drag_destination.x = fVX > 0.0f ? 1.0f : -3.0f; }
                            else
                            { magnified_drag_destination.x = -1.0f; }
                            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                        }
                    }
                }
            }
        }
        case emp::app::button_e::Right:
        case emp::app::button_e::Center:
        case emp::app::button_e::Invalid:
        default:
        { break; }
    }
            
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e viewer_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    auto const& rKeyboard = a_rEvent.keyboard;
    c_bool bPressed = rKeyboard.m_bPressed;
    emp::app::c_key_e eKey = rKeyboard.m_eKey;

    // @@0 : not working, escape already consumed
    if (bPressed == true &&
        rKeyboard.modified() == false &&
        rKeyboard.m_bRepeat == false &&
        eKey == emp::app::key_e::Escape)
    {
        if (/*magnified && */magnified_direction > 0)
        {
            magnified_direction = -1;
            magnified_time = 0.0f;
        }
        else if ((EMP_GUI_MANAGER->modal() == false && ASE_GAME.menu().is_hidden()) || ASE_GAME.menu().is_hiding())
        {
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed); // @@0 : wtf
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool viewer_c::update(emp::tim::clock_c const& a_rClock)
{
//EMP_LOG_INFO("MAG #0, REM #1", magnified_index, remove_variant);
    time_c::add(current_time, a_rClock.delta(), current_time);

    if (emp::tt::is_not_max(remove_variant))
    {
        if (selection->m_vpThumbnails.is_valid_index(remove_variant))
        {
            if (magnified_index == ((remove_variant + 1) % selection->m_vpThumbnails.count()))
            {
                thumbnail_c* const pThumb = selection->m_vpThumbnails.val(remove_variant);

                selection->remove_source(pThumb->m_sOse);
                magnified_index = remove_variant;
                EMP_TT_MAX_VAR(remove_variant);

                c_size stCount = selection->m_vpThumbnails.count();
                if (stCount > 0)
                {
                    if (magnified_index >= stCount)
                    { magnified_index = stCount - 1; }
                    magnified = selection->m_vpThumbnails.val(magnified_index);
                }
                else
                    EMP_TT_MAX_VAR(magnified_index);

                /*
                 ui32 ui32Scale = 1;
                 do
                 {
                 astring sThumbFile = STR_AFORMAT("#0_#1x#2.png", sSource, ui32Scale, ui32Scale);
                 astring sThumbPath = STR_AFORMAT("#0png/#1.png", EMP_APPLICATION.resource_path(), sThumbFile);
                 bool bThumbExists = false;
                 EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sThumbPath, bThumbExists));
                 if (bThumbExists)
                 { EMP_RET_FALSE_IF_NOT(emp::fs::delete_file(sThumbPath)); }
                 ui32Scale <<= 1;
                 }
                 while (ui32Scale <= 8192);*/
            }
        }
        else
        { EMP_TT_MAX_VAR(remove_variant); }
    }

    if (magnified != nullptr)
    {
        auto const& rMagnified = magnified->transformation();
        magnified_scale_ratio = (magnified_scale - 1) / (magnified_scale_fit.x - 1);

        magnified_fit_width = rMagnified.size.x * magnified_scale;
        magnified_fit_height = rMagnified.size.y * magnified_scale;

        magnified_time += emp::tim::time_c::to_seconds(a_rClock.delta());
        magnified_drag_speed = v2f::mul(magnified_drag_speed, 0.9f);
        if (magnified_slide_horz == false)
        { magnified_drag_destination = v2f::add(magnified_drag_destination, magnified_drag_speed); }
        magnified_old_scale = magnified_scale;

        c_v3f v3fOldOffset = magnified_offset;

        if (magnified_fit)
        {
            c_float fDistance = magnified_scale_fit.x - 1.0f;
            c_float fTime = emp::mat::pow(emp::tt::minof(1.0f, magnified_time / 0.4f), 2.0f);
            if (magnified_direction < 0)
            {
                magnified_scale = 1.0f + (1 - fTime) * fDistance;
                if (magnified_scale <= 1)
                {
                    magnified_scale = 1;
                    //magnified->unload_texture();
                    magnified = nullptr;
                }
            }
            else if (magnified_direction > 0)
            {
                magnified_scale = 1.0f + fTime * fDistance;
                if (magnified_scale >= magnified_scale_fit.x && magnified_direction > 0)
                {
                    magnified_scale = emp::tt::minof(magnified_scale, magnified_scale_fit.x);
                    magnified_old_scale = magnified_scale;
                    magnified_direction = 0.0f;
                    magnified_fit = false;
                    magnified_zoom = true;
                }
            }
            else
            {
            }

            c_float fX = rMagnified.matrix.f30;
            c_float fY = rMagnified.matrix.f31;
            if (magnified_scale >= magnified_scale_fit.x)
            {
                magnified_offset.x = magnified_drag_destination.x;
                magnified_offset.y = magnified_drag_destination.y;
            }
            else
            {
                magnified_offset.x = fX + (-1 - fX) * magnified_scale_ratio;
                magnified_offset.y = fY + (1 - fY) * magnified_scale_ratio;
                magnified_drag_destination = {-1, 1};
                magnified_drag_origin = magnified_drag_destination;
            }
        }
        else if (magnified_zoom)
        {
            //EMP_LOG_INFO("ZOOMA #0, #1, #2, #3", magnified_direction, magnified_scale, magnified_offset.x, magnified_offset.y);
            magnified_scale += emp::tim::time_c::to_seconds(a_rClock.delta()) * magnified_direction * magnified_speed;
            magnified_speed *= 0.6f;
            if (magnified_direction < 0)
            {
                if (magnified_scale <= magnified_scale_fit.x)
                { magnified_scale = magnified_scale_fit.x; }
            }
            else if (magnified_direction > 0)
            {
                if (magnified_scale >= 50.0f)
                {
                    magnified_scale = emp::tt::minof(magnified_scale, 50.0f);
                    magnified_direction = 0.0f;
                }
            }
            else
            {
            }

            if (emp::mat::equal_(magnified_scale, magnified_old_scale))
            {
                if (magnified_slide_horz == false)
                {
                    magnified_offset.x = magnified_drag_destination.x;
                    magnified_offset.y = magnified_drag_destination.y;
                    if (magnified_offset.x > -1.0)
                    { magnified_offset.x = -1.0f; }
                    if (magnified_offset.x + magnified_fit_width < 1.0f)
                    { magnified_offset.x = 1.0f - magnified_fit_width;}
                    if (magnified_offset.y < 1)
                    { magnified_offset.y = 1; }
                    if (magnified_offset.y - rMagnified.size.y * magnified_scale > -1)
                    { magnified_offset.y = rMagnified.size.y * magnified_scale - 1; }
                }
                else
                {
                    c_float fSlide = emp::tt::minof(1.0f, time_c::to_seconds(time_c::sub(current_time, magnified_slide_time)) / 2.0f);
                    if (magnified_slide_horz)
                    {
                        magnified_offset.x += (magnified_drag_destination.x - magnified_offset.x) * fSlide;
                        if (magnified_offset.x <= -3)
                        {
                            magnified_offset.x = -1;
                            magnified_drag_destination.x = -1;
                            magnified_index = (magnified_index + 1) % selection->m_vpThumbnails.count();
                            magnified_slide_horz = false;
                            magnified_drag_speed = {0, 0};
                            magnified = selection->m_vpThumbnails.val(magnified_index);
                            magnified->preload = true;
                        }
                        else if (magnified_offset.x >= 1)
                        {
                            magnified_offset.x = -1;
                            magnified_drag_destination.x = -1;
                            magnified_index = ((magnified_index > 0) ? magnified_index : selection->m_vpThumbnails.count()) - 1;
                            magnified_slide_horz = false;
                            magnified_drag_speed = {0, 0};
                            magnified = selection->m_vpThumbnails.val(magnified_index);
                            magnified->preload = true;
                        }

                        magnified_offset.y = magnified_drag_destination.y;

                        if (emp::mat::abs(magnified_offset.x + 1) <= 0.001f && emp::mat::equal_(v3fOldOffset.x, magnified_offset.x))
                        { magnified_slide_horz = false; }

                        if (magnified_offset.y < 1)
                        { magnified_offset.y = 1; }
                        if (magnified_offset.y - magnified_fit_height > -1)
                        { magnified_offset.y = magnified_fit_height - 1; }
                    }
                    else
                    {
                        magnified_offset.x = magnified_drag_destination.x;
                        magnified_offset.y = magnified_drag_destination.y;
                    }
                }
            }
            else
            {
                magnified_offset.x = (magnified_offset.x - magnified_center.x) * magnified_scale / magnified_old_scale + magnified_center.x;
                magnified_offset.y =  (magnified_offset.y - magnified_center.y) * magnified_scale / magnified_old_scale + magnified_center.y;
                magnified_drag_destination = { magnified_offset.x, magnified_offset.y };
                if (magnified_offset.x > -1.0)
                { magnified_offset.x = -1.0f; }
                if (magnified_offset.x + magnified_fit_width < 1.0f)
                { magnified_offset.x = 1.0f - magnified_fit_width;}
                if (magnified_offset.y < 1)
                { magnified_offset.y = 1; }
                if (magnified_offset.y - rMagnified.size.y * magnified_scale > -1)
                { magnified_offset.y = rMagnified.size.y * magnified_scale - 1; }
            }

            magnified_center = {0, 0};

            //EMP_LOG_INFO("ZOOMB #0, #1, #2, #3", magnified_direction, magnified_scale, magnified_offset.x, magnified_offset.y);

        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool viewer_c::render(void)
{
    if (magnified == nullptr)
    { return true; }

    bool bEmpty;
    EMP_GUI_MANAGER_CLIP(oScissor, true, transformation(), bEmpty, 0, 0, 0, 0);
    if (bEmpty)
    { return true; }

    m44f mTranslate;
    m44f mTransform;

    m44f mScale;
    m44f::identity(mScale);
    m44f::scale(mScale, {magnified_scale, magnified_scale, 1}, mScale);

    auto const l_render_thumb = [this, &mTranslate, &mScale, &mTransform](thumbnail_c* const a_pThumbnail, c_v3f a_v3Offset) EMP_RETURN -> bool
    {
        //EMP_LOG_INFO("thumb : #0", a_pThumbnail->m_sThumb);

        m44f::identity(mTranslate);
        m44f::translate(mTranslate, a_v3Offset, mTranslate);

        m44f::identity(mTransform);
        m44f::mul(mTranslate, mScale, mTransform);

        EMP_RET_FALSE_IF_NOT(a_pThumbnail->reload_texture(2, 2));
        a_pThumbnail->render_thumb(mTransform, emp::tt::minof(1.0f, magnified_scale_ratio));
        return true;
    };

    if (magnified_slide_horz)
    {
        c_size stCount = selection->m_vpThumbnails.count();
        if (stCount > 0)
        {
            auto* const pCurrent = selection->m_vpThumbnails.val(magnified_index);
            EMP_RET_FALSE_IF_NOT(l_render_thumb(pCurrent, magnified_offset));

            if (magnified_offset.x < -1)
            {
                if (pCurrent == selection->m_pRunning)
                { selection->unlock_running(); }

                c_size stNext = (magnified_index + 1) % stCount;
                EMP_RET_FALSE_IF_NOT(l_render_thumb(selection->m_vpThumbnails.val(stNext), {magnified_offset.x + magnified_fit_width, magnified_offset.y, magnified_offset.z} ));
            }

            if (magnified_offset.x > -1)
            {
                if (pCurrent == selection->m_pRunning)
                { selection->unlock_running(); }

                c_size stPrevious = ((magnified_index > 0) ? magnified_index : stCount) - 1;
                EMP_RET_FALSE_IF_NOT(l_render_thumb(selection->m_vpThumbnails.val(stPrevious), {magnified_offset.x - magnified_fit_width, magnified_offset.y, magnified_offset.z}));
            }
        }
    }
    else
    {
        EMP_RET_FALSE_IF_NOT(l_render_thumb(selection->m_vpThumbnails.val(magnified_index), magnified_offset));
    }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e viewer_c::on_new_variant(cpc_control_i, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue == false)
    {
        auto const l_get_variant_name = [](c_astring& a_sVariant, astring& a_rsOse, ui64& a_ruiVariant) EMP_RETURN -> bool
        {
            ui64 uiIndex = 0;
            while (true)
            {
                astring sVariant(STR_AFORMAT("#0_v#1", a_sVariant, uiIndex));
                astring sOse(STR_AFORMAT("#0.ose", sVariant));
                c_astring sOsePath = STR_AFORMAT("#0ose/#1", EMP_APPLICATION.resource_path(), sOse);

                bool bExists = false;
                EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sOsePath, bExists));
                if (bExists == false)
                {
                    a_rsOse = sOse;
                    a_ruiVariant = uiIndex;

                    return true;
                }

                ++uiIndex;
            }
        };

        thumbnail_c* pThumb = selection->m_vpThumbnails.val(magnified_index);

        selection->unlock_running();

        EMP_RET_X_IF(return_e::Error, pThumb == nullptr);

        auto const stMagnified = magnified_index + 1;

        astring sOse;
        ui64 EMP_TT_MAX_VAR(uiVariant);
        EMP_RET_X_IF_NOT(return_e::Error, l_get_variant_name(pThumb->m_sFilename, sOse, uiVariant));

        magnified_drag_destination.x = -3;
        magnified_slide_horz = true;
        magnified_slide_time = current_time;

        EMP_RET_X_IF_NOT(return_e::Error, selection->insert_source(stMagnified, sOse, true));

        emp::ose::renderlet_c oRenderlet;
        oRenderlet.m_sMother = pThumb->m_sOse;
        EMP_RET_X_IF_NOT(return_e::Error, ASE_GAME.ose_renderer().load(oRenderlet, 1.0f/*EMP_GUI_SKIN.resolution.current()*/, true));

        astring sOsePath;
        ASE_GAME.ose_renderer().get_filepath(sOse, sOsePath);
        astring sBuffer;
        EMP_RET_X_IF_NOT(return_e::Error, ASE_GAME.ose_renderer().save(oRenderlet, sOsePath, sBuffer, true, false, true));

        return return_e::Consumed;
    }
    return return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e viewer_c::on_remove_variant(cpc_control_i, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    EMP_LOG_INFO("ON_REMOVE_VARIANT #0", a_bValue);
    if (a_bValue == false && emp::tt::is_max(remove_variant) && selection->m_vpThumbnails.is_valid_index(magnified_index))
    {
        remove_variant = magnified_index;
        magnified_drag_destination.x = -3;
        magnified_slide_horz = true;
        magnified_slide_time = current_time;
    }
    return return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e viewer_c::on_crop_variant(cpc_control_i, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue == false)
    {
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void viewer_c::clear_magnified(void)
{
    if (magnified != nullptr)
    { magnified->preload = false; }
    magnified = nullptr;
    EMP_TT_MAX_VAR(magnified_index);
    magnified_drag_speed = magnified_drag_velocity;
    magnified_drag_velocity = {0, 0};
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void viewer_c::start(void)
{
    magnified = selection->dragged;
    magnified->preload = true;
    magnified_index = selection->dragged_index;
    magnified_scale = 1;
    magnified_direction = 1;
    magnified_time = 0;
    magnified_speed = 1;
    magnified_fit = true;
    magnified_zoom = false;

    auto const& rMagnified = magnified->transformation();
    c_float fCX = rMagnified.matrix.f00;
    c_float fCY = rMagnified.matrix.f11;
    c_float fW = rMagnified.size.x;
    c_float fH = rMagnified.size.y;
    c_float fTW = /*emp::tt::maxof(fW, */fCX * 2.0f / fW/*)*/;
    c_float fTH = /*emp::tt::maxof(fH, */fCY * 2.0f / fH/*)*/;
    magnified_scale_fit = {fTW, fTH };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
selection_c::selection_c(void):
    m_msFiles(8192)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
selection_c::~selection_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::create(emp::gui::registrar_i* const a_pRegistrar)
{
    destroy();

    registrar = a_pRegistrar;

    list.add_column_(emp::loc::string_s(ALITERAL(".pools."), U8LITERAL(".pools.")))
        .registrar_(registrar)
        .on_selection_changed_(emp::fct::function(this, &emp::gui::selection_c::on_pool_changed))
        .on_selection_double_click_(emp::fct::function(this, &emp::gui::selection_c::on_pool_double_click))
        .state().navigable_(true).index_(0).autosize.x_(true);

    m_Renderlet.m_pImStack->create();

    mdi.automove_(false).state().navigable_(true).focusable_(true).index_(1);

    master_(&list).hide_master_(true).slave_(&mdi).orientation_(emp::gui::dock_e::Left)
        .state().index_(0).dock_(emp::gui::dock_e::Fill);

    redock();

    EMP_RET_FALSE_IF_NOT(viewer.create(this));
    viewer.state().index_(0).navigable_(true).overlap_(true);
    viewer.transformation().size_(2, 2);

    ASE_GAME.add_on_user_changed(emp::fct::function(this, &selection_c::on_user_changed));

    return true;
}


void get_object_name(u8string& a_rsu8Name, u8format_c const& a_ru8Format, vector<ase::cor::pool_c*>& a_rvpPools);
void get_object_name(u8string& a_rsu8Name, u8format_c const& a_ru8Format, vector<ase::cor::pool_c*>& a_rvpPools)
{
    size_t stPattern = 0;

ASE_GUI_TMP_LABEL_RESTART:
    c_size stCount = a_rvpPools.count();
    a_rsu8Name = emp::str::xformat(a_ru8Format, stPattern);
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (emp::str::cmp(a_rsu8Name, a_rvpPools.val(stIndex)->name))
        {
            ++stPattern;
            goto ASE_GUI_TMP_LABEL_RESTART;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::on_source_removed(c_astring& a_sFile)
{
    auto& rSelection = ASE_GAME.user().options.selection;
    ase::cor::pool_c* const pPool = rSelection.pool;

    auto const l_add_trash = [this, &rSelection](ase::cor::pool_c*& a_rpPool, c_u8string& a_sName, c_astring& a_sPattern, ase::cor::pool_e a_eType)
    {
        if (a_rpPool == nullptr)
        {
            a_rpPool = EMP_FACTORY.store<ase::cor::pool_c>()->new_object_pattern(a_sPattern);
            a_rpPool->type = a_eType;
            a_rpPool->name = a_sName;
            rSelection.pools->pools.add(a_rpPool);
            EMP_ASSERT_BOOL_CALL(add_pool(a_rpPool, change_e::Code) != nullptr);
            EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(ASE_GAME.user().options.selection.pools));
        }
    };

    l_add_trash(rSelection.trash, u8string(U8LITERAL("Trash")), astring(ALITERAL("ase_cor_pool_trash")), ase::cor::pool_e::Trash);
    l_add_trash(rSelection.graveyard, u8string(U8LITERAL("Graveyard")), astring(ALITERAL("ase_cor_pool_graveyard")), ase::cor::pool_e::Graveyard);

    if (pPool == rSelection.trash)
    {
        EMP_XX_IF_ASSERT(c_size stCount = pPool->files.count());
        remove_source(a_sFile);
        EMP_ASSERT(stCount == (pPool->files.count() + 1));
        rSelection.graveyard->add_source(a_sFile);
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(pPool));
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(rSelection.graveyard));
    }
    else if (pPool == rSelection.graveyard)
    {
    }
    else
    {
        EMP_XX_IF_ASSERT(c_size stCount = pPool->files.count());
        remove_source(a_sFile);
        EMP_ASSERT(stCount == (pPool->files.count() + 1));
        rSelection.trash->add_source(a_sFile);
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(pPool));
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(rSelection.graveyard));
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::clear_drag(void)
{
    mdi.dragged = nullptr;

    dragged = nullptr;
    EMP_TT_MAX_VAR(dragged_index);

    dragged_delta = v2f::zero();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::remove_source(c_astring& a_sFile)
{
    ase::cor::pool_c* const pPool = ASE_GAME.user().options.selection.pool;

    auto const it = pPool->files.get(a_sFile);
    EMP_ASSERT(it.not_end());
    EMP_XX_IF_ASSERT(c_size stCount = pPool->files.count());
    if (it.not_end())
    {
        emp::tt::silent(pPool->remove_source(it));

        size_t stIndex = 0;
        while (stIndex < m_vpThumbnails.count())
        {
            thumbnail_c* pThumb = m_vpThumbnails.val(stIndex);
            if (emp::str::cmp(pThumb->m_sOse, a_sFile))
            {
                mdi.scroller.remove_control(pThumb);

                if (pThumb == m_pRunning)
                { unlock_running(); }

                m_vpThumbnails.remove(stIndex);
                EMP_GUI_MANAGER->clear_focus(pThumb);

                EMP_DEALLOCATE(pThumb);
            }
            else
            { ++stIndex; }
        }

        clear_drag();
        viewer.clear_magnified();

        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(pPool));
    }
    EMP_ASSERT(stCount == (pPool->files.count() + 1));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::insert_source(c_size a_stIndex, c_astring& a_sSource, c_bool a_bPreload)
{
    ase::cor::pool_c* const pPool = ASE_GAME.user().options.selection.pool;

    auto const it = pPool->files.get(a_sSource);
    EMP_RET_FALSE_IF_NOT(it.end());
    c_size stCount = pPool->files.count();
    if (it.end())
    {
        clear_drag();

        pPool->insert_source(a_stIndex, a_sSource);
        EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(pPool));
        thumbnail_c* pThumbnail = insert_thumbnail(a_stIndex, a_sSource);
        EMP_RET_FALSE_IF(pThumbnail == nullptr);
        pThumbnail->preload = a_bPreload;

        unlock_running();
    }
    EMP_RET_FALSE_IF_NOT((stCount + 1) == pPool->files.count());

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::on_user_changed(ase::cor::user_c* const EMP_SILENT(a_pPrevious), ase::cor::user_c* const EMP_SILENT(a_pCurrent))
{
    populate_pool_list(0, emp::gui::change_e::Code);
    EMP_LOG_INFO("ON USER CHANGED");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::on_pool_changed(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_stItem), c_change_e EMP_SILENT(a_eChange))
{
    ASE_GAME.ose_renderer().clear_invalids();
    //EMP_GUI_MANAGER->clear_focus();

    clear_drag();
    viewer.clear_magnified();

    dragged_delta = v2f::zero();
    temp = nullptr;
    cut = false;

    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    EMP_RET_FALSE_IF((a_ui32Line > 0) == false);

    m_msFiles.clear();

    auto& rSelection = ASE_GAME.user().options.selection;
    auto* const pPool = rSelection.pools->pools.val(a_ui32Line - 1);
    rSelection.pool = pPool;
    ASE_GAME.ose_renderer().m_pvsOse = &pPool->sources();

    unlock_running();

    emp::cnt::foreach(m_vpThumbnails, emp::mem::safe_delete_reset_t());
    m_vpThumbnails.resize(0);

    mdi.scroller.clear_controls();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::on_pool_double_click(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e EMP_SILENT(a_eChange))
{
    EMP_RET_FALSE_IF(emp::tt::is_max(a_ui32Line));
    EMP_RET_FALSE_IF_NOT(a_ui32Line > 0);

    emp::tt::silent(ASE_GAME.mod_selection().fire_next()); // @@0 global hack
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::restore_pool(void)
{
    clear_drag();
    viewer.clear_magnified();

    ASE_GAME.ose_renderer().m_pvsOse = &ASE_GAME.user().options.selection.pool->sources();
    //m_vsTmpFiles.resize(0);

    EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(&ASE_GAME.user()));
    populate_pool_list(0, c_change_e::Code);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::destroy(void)
{
    if (m_Renderlet.m_pProgram != nullptr)
    {
        EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(m_Renderlet.m_stProgram));
        m_Renderlet.m_pProgram = nullptr;
        EMP_TT_MAX_VAR(m_Renderlet.m_stProgram);
    }

    viewer.destroy();

    registrar = nullptr;

    // @@0 create list->destroy();

    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    foreach(m_vpThumbnails, emp::mem::safe_delete_reset_t());
    m_vpThumbnails.resize(0);

    skin = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::lock_running(thumbnail_c* const a_pValue)
{
    EMP_ASSERT(m_pRunning != a_pValue && m_pRunning == nullptr);
    if (m_pRunning == nullptr)
    {
        if (m_Renderlet.m_pProgram != nullptr)
        {
            EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(m_Renderlet.m_stProgram));
            m_Renderlet.m_pProgram = nullptr;
            EMP_TT_MAX_VAR(m_Renderlet.m_stProgram);
        }
        m_pRunning = a_pValue;
        a_pValue->renderlet = &m_Renderlet;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::unlock_running(thumbnail_c* const a_pValue)
{
    EMP_ASSERT(m_pRunning == a_pValue && m_pRunning != nullptr);
    if (m_pRunning == a_pValue)
    {
        m_pRunning = nullptr;
        a_pValue->renderlet = nullptr;
        if (m_Renderlet.m_pProgram != nullptr)
        {
            EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(m_Renderlet.m_stProgram));
            m_Renderlet.m_pProgram = nullptr;
            EMP_TT_MAX_VAR(m_Renderlet.m_stProgram);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::unlock_running(void)
{
    if (m_pRunning != nullptr)
    { unlock_running(m_pRunning); }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::evolution(c_astring& a_sFile)
{
    ASE_GAME.mod_evolution().set_file(a_sFile);

    unlock_running();

    /* @@0 remove auto& rSelection = ASE_GAME.user().m_Options.m_Selection;
    rSelection.m_bMutate = false;
    rSelection.m_bCross = true;
    rSelection.m_bChangeMother = false;
    rSelection.set_xfather(true);*/
    EMP_ASSERT_BOOL_CALL(ASE_GAME.mod_selection().fire_next());

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::render(void)
{
    EMP_RET_FALSE_IF_NOT(tbase1::render());

    if (dragged && viewer.magnified == nullptr)
    {
        m44f mTransform;
        m44f::identity(mTransform);
        m44f::translate(mTransform, { dragged_delta.x, dragged_delta.y, -0.98f}, mTransform);
        m44f::mul(dragged->transformation().matrix, mTransform, mTransform);
        dragged->render_thumb(mTransform, 0.3f);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN thumbnail_c* selection_c::new_thumbnail(c_astring& a_rsFile, c_size a_stSource)
{
    thumbnail_c* const pThumbnail = EMP_ALLOCATE(thumbnail_c);

    EMP_RET_NULL_IF_NOT(pThumbnail->create(this, a_rsFile));
    pThumbnail->remove.skin_(EMP_GUI_SKIN_(picture, "pic.column"));
    pThumbnail->remove.model_(EMP_GUI_SKIN.data->gfx_check);
    pThumbnail->remove.state().visible_(hide_master == false);
    pThumbnail->skin_(EMP_GUI_SKIN_(sizer, "szr.column")).
    state().navigable_(true).focusable_(true).index_(emp::ast::cast<ui32>(a_stSource));
    return pThumbnail;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::update(emp::tim::clock_c const& a_rClock)
{
    using namespace emp::tim;

    if (state().visible == false)
    { return true; }

    time_c::add(discard_time, a_rClock.delta(), discard_time);

    if (emp::tim::time_c::more(discard_time, time_c::from_seconds(0.5f)))
    { discard = true; }

    emp::tt::silent(try_remove_control(&viewer));
    if (viewer.magnified)
    { add_control(&viewer); }

    c_bool current_edition = ASE_GAME.user().options.selection.edition;

    if (current_edition != previous_edition)
    {
        hide_master_(current_edition == false);
        c_size stCount = m_vpThumbnails.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            thumbnail_c* const pThumb = m_vpThumbnails.val(stIndex);
            pThumb->remove.state().visible_(current_edition);
        }
        redock();
    }

    previous_edition = current_edition;


    auto const& rvpPools = ASE_GAME.user().options.selection.pools->pools;
    if (m_vpPools.count() != rvpPools.count())
    { populate_pool_list(0, emp::gui::change_e::Code); }

    ase::cor::pool_c* const pPool = ASE_GAME.user().options.selection.pool;
    size_t stIndex = 0;
    bool bChanged = false;
    while (stIndex < pPool->sources().count())
    {
        bool bExists = false;
        c_astring sFile = pPool->sources().ref(stIndex);
        astring sPath = STR_AFORMAT("#0ose/#1", EMP_APPLICATION.resource_path(), sFile);
        EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sPath, bExists));
        auto const it = m_msFiles.get(sFile);
        if (bExists)
        {
            if (it.end())
            {
                EMP_RET_FALSE_IF(insert_thumbnail(stIndex, sFile) == nullptr);
                bChanged = true;
            }
            ++stIndex;
        }
        else
        {
            remove_source(sFile);
            bChanged = true;
        }
    }

    if (bChanged)
    { EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(pPool)); }

    return tbase1::update(a_rClock);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN thumbnail_c* selection_c::insert_thumbnail(c_size a_stIndex, c_astring& a_rsFile)
{
    m_msFiles.set(a_rsFile, true);
    thumbnail_c* const pThumbnail = new_thumbnail(a_rsFile, a_stIndex);
    EMP_RET_NULL_IF(pThumbnail == nullptr);

    m_vpThumbnails.insert(a_stIndex, pThumbnail);
    mdi.scroller.insert_control(a_stIndex, pThumbnail);
    return pThumbnail;;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    for (size_t stPass = 0; stPass < 2; ++stPass)
    {
        c_float fOffsetX = mdi.scroller.transformation().x();
        c_float fOffsetY = mdi.scroller.transformation().y();
        c_float fPanelWidth = mdi.user.transformation().width();
        c_float fPanelHeight = mdi.user.transformation().height();

        c_float fWidth = mdi.user.transformation().size.x;
        //c_float fHeight = transformation().size().y;

        c_size stThumbCount = emp::tt::maxof<ui32>(1u, ASE_GAME.user().options.selection.gallery_width);
        float fThumbCount = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(stThumbCount, fThumbCount));

        c_float fSpacingX = EMP_GUI_SKIN.spacing_x();
        c_float fTW = (fWidth - (fThumbCount - 1) * fSpacingX) / fThumbCount;
        c_float fTH = fTW;
        c_float fDepth = 0.0f;//emp::gui::depth(transformation().fore_depth, 0);

        m_pVertexBuffer->create(emp::gfx::usage_e::Dynamic);

        m_pVertexBuffer->add({ 0   , -fTH, fDepth }, { 0, 0 } );
        m_pVertexBuffer->add({ fTW , -fTH, fDepth }, { 1, 0 } );
        m_pVertexBuffer->add({ 0   , 0   , fDepth }, { 0, 1 } );
        m_pVertexBuffer->add({ fTW , 0   , fDepth }, { 1, 1 } );

        viewer.magnified_size = { fTW, fTH };

        c_size stCount = m_vpThumbnails.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_float fX =  /*fSpacingX +*/ (fSpacingX + fTW) * static_cast<float>(stIndex % stThumbCount);
            c_float fY =  - (EMP_GUI_SKIN.spacing_y() + fTH) * static_cast<float>(stIndex / stThumbCount);

            thumbnail_c* const pThumbnail = m_vpThumbnails.val(stIndex);
            pThumbnail->transformation().position_(fX, fY).size_(fTW, fTH);
            if (pThumbnail != viewer.magnified)
            {
                pThumbnail->preload = emp::gui::is_visible(pThumbnail, fOffsetX, fOffsetY - fTH, fPanelWidth, fPanelHeight + fTH);
            }
        }
        EMP_RET_FALSE_IF_NOT(tbase1::resize());

    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::transform(emp::gui::stack_c& a_rStack)
{
    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    if (contains_control(controls(), &viewer))
    {
        //a_rStack.depth = transformation().fore_depth + 2;
        //EMP_RET_FALSE_IF_NOT(viewer.transform(a_rStack));
        //viewer.magnified_offset.z = emp::gui::depth(viewer.transformation().fore_depth, 0);
    }
/*    {
        si32 siMaxDepth = 0;
        c_size stCount = controls().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { siMaxDepth = emp::mat::maxof(controls().val(stIndex)->transformation().fore_depth, siMaxDepth); }
        a_rStack.depth = siMaxDepth;
        EMP_RET_FALSE_IF_NOT(viewer.transform(a_rStack));
    }
*/
    if (m_pRunning != nullptr && m_pRunning->preload == false)
    {
        m_pRunning->unload_texture();
        EMP_LOG_INFO("Unloading locked renderlet");
    }

    {
        c_size stCount = m_vpThumbnails.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            auto* const pThumb = m_vpThumbnails.val(stIndex);
            if (pThumb->preload == false && ((pThumb->texture != 0 && emp::tt::is_not_max(pThumb->texture)) || (pThumb->renderlet != nullptr)))
            {
                EMP_LOG_INFO("Unloading #0", stIndex);
                pThumb->unload_texture();
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
selection_c& selection_c::skin_(cpc_selection_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->splitter : nullptr);
    list.skin_(a_pValue ? a_pValue->list : nullptr);
    mdi.skin_(a_pValue ? a_pValue->panel : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::populate_pool_list(c_ui32 a_ui32Selection, c_change_e a_eChange)
{
    //EMP_GUI_MANAGER->clear_focus();

    m_vpPools.resize(0);
    list.clear();

    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    auto& rSelection = ASE_GAME.user().options.selection;
    ase::cor::pool_list_c* const pPools = rSelection.pools;
    auto& vpPools = pPools->pools;

    ase::cor::pool_c* pAll = nullptr;
    ase::cor::pool_c* pUnused = nullptr;
    for (size_t stIndex = 0; stIndex < vpPools.count(); ++stIndex)
    {
        auto* const pPool = vpPools.val(stIndex);
        if (pPool->type == ase::cor::pool_e::Unused)
        { pUnused = pPool; }
        else if (pPool->type == ase::cor::pool_e::All)
        { pAll = pPool; }
    }

    if (pUnused == nullptr)
    {
        pUnused = EMP_FACTORY.store<ase::cor::pool_c>()->new_object_pattern(ALITERAL("ase_cor_pool_unused"));
        pUnused->type = ase::cor::pool_e::Unused;
        pUnused->name = u8string(U8LITERAL("Unused"));
        vpPools.add(pUnused);
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(pPools));
    }

    if (pAll == nullptr)
    {
        pAll = EMP_FACTORY.store<ase::cor::pool_c>()->new_object_pattern(ALITERAL("ase_cor_pool_all"));
        pAll->type = ase::cor::pool_e::All;
        pAll->name = u8string(U8LITERAL("All"));
        vpPools.add(pAll);
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(pPools));
    }

    {
        vector<astring> vsFiles; //@@0 alloc
        astring sPath = STR_AFORMAT("#0ose/", EMP_APPLICATION.resource_path());
        if (emp::fs::list(sPath, ALITERAL("*.ose"), vsFiles, false))
        {
            for (size_t stIndex = 0; stIndex < vsFiles.count(); ++stIndex)
            {
                c_astring& rsFile = vsFiles.ref(stIndex);
                {
                    auto const it = pUnused->files.get(rsFile);
                    if (it.end())
                    {
                        bool bFound = false;
                        for (size_t stPool = 0; stPool < vpPools.count(); ++stPool)
                        {
                            auto* const pPool = vpPools.val(stPool);
                            if (pPool != pUnused && pPool != pAll)
                            {
                                if (pPool->files.get(rsFile).not_end())
                                {
                                    bFound = true;
                                    break;
                                }
                            }
                        }
                        if (bFound == false)
                        { pUnused->add_source(rsFile); }
                    }
                }

                {
                    auto const it = pAll->files.get(rsFile);
                    if (it.end())
                    { pAll->add_source(rsFile); }
                }
            }

            for (size_t stIndex = 0; stIndex < pUnused->sources().count();)
            {
                c_astring& sFile = pUnused->sources().ref(stIndex);
                bool bRemoved = false;
                for (size_t stPool = 0; stPool < vpPools.count(); ++stPool)
                {
                    auto* const pPool = vpPools.val(stPool);
                    if (pPool != pUnused && pPool != pAll)
                    {
                        if (pPool->files.get(sFile).not_end())
                        {
                            pUnused->remove_source(stIndex);
                            bRemoved = true;
                            break;
                        }
                    }
                }
                if (bRemoved == false)
                { ++stIndex; }
            }
        }
    }

    c_ui32 ui32Count = emp::ast::cast<ui32>(vpPools.count());

    ui32 ui32Selection = a_ui32Selection;

    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    {
        ase::cor::pool_c* pPool = vpPools.val(ui32Index);
        if (pPool == rSelection.pool)
        { ui32Selection = ui32Index; }
        EMP_ASSERT_BOOL_CALL(add_pool(vpPools.val(ui32Index), a_eChange) != nullptr);
        if (pPool->type == ase::cor::pool_e::Trash)
        { rSelection.trash = pPool; }
        else if (pPool->type == ase::cor::pool_e::Graveyard)
        { rSelection.graveyard = pPool; }
    }

    if (ui32Selection < ui32Count)
    {
        rSelection.pool = vpPools.val(ui32Selection);
        list.set_selection(ui32Selection + 1, 0, true, a_eChange);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::line_c* selection_c::add_pool(ase::cor::pool_c* const a_pPool, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpPools.count());
    m_vpPools.add(a_pPool);

    emp::rfx::type_t<u8string>* const pType = EMP_ALLOCATE(emp::rfx::type_t<u8string>,
        emp::fct::bind(emp::fct::function(this, &selection_c::get_value), ui32Index));

    m_vpReflections.add(pType);

    line_c* const pLine = list.new_line(ui32Index);
    list.add_item(nullptr, pLine, emp::rfx::field_s(pType), nullptr);
    list.insert_line(ui32Index, pLine);

    if (EMP_FACTORY.write_changes(a_pPool) == false)
    { return nullptr; }

    return pLine;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::remove_pool(ase::cor::pool_c* const a_pPool)
{
    for (size_t stIndex = 0; stIndex < m_vpPools.count(); ++stIndex)
    {
        if (m_vpPools.val(stIndex) == a_pPool)
        {
            m_vpPools.remove(stIndex);
            EMP_DEALLOCATE(m_vpReflections.val(stIndex));
            m_vpReflections.remove(stIndex);
            list.remove_line(stIndex);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::set_value(c_size a_stIndex, c_u8string& a_rsu8Value)
{
    ase::cor::pool_c* const pPool = m_vpPools.val(a_stIndex);
    pPool->name = a_rsu8Value;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::drag(thumbnail_c* const a_pValue, c_size a_stIndex)
{
    if (state_hover(a_pValue))
    {
        dragged = a_pValue;
        dragged_delta = v2f::zero();
        dragged->cut_(cut);
        dragged_index = a_stIndex;

        if (cut) // @@0 remove
        {
            EMP_LOG_DEBUG("DRAG CUT");
        }
        else
        {
            EMP_LOG_DEBUG("DRAG");
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::get_value(c_size a_stIndex, u8string& a_rsu8Value)
{
    ase::cor::pool_c const* const pPool = m_vpPools.val(a_stIndex);
    a_rsu8Value = pPool->name;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e selection_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    auto const& rKeyboard = a_rEvent.keyboard;
    auto const& rMouse = a_rEvent.mouse;

    auto const eButton = rMouse.m_eButton;

    c_size stTapCount = rMouse.m_stTapCount;
    c_size stToucheCount = rMouse.m_stToucheCount;

    temp_used = false;

    auto* const pPools = ASE_GAME.user().options.selection.pools;
    auto& rvpPools = pPools->pools;
    c_size stPoolCount = rvpPools.count();

    auto l_remove_temp = [this, stPoolCount, &rvpPools, pPools]()
    {
        if (temp_used == false)
        {
            if (temp != nullptr)
            {
                for (size_t stIndex = 0; stIndex < stPoolCount; ++stIndex)
                {
                    if (temp == rvpPools.val(stIndex))
                    {
                        remove_pool(temp);
                        rvpPools.remove(stIndex);

                        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(pPools));

                        temp = nullptr;
                        temp_line = nullptr;
                        temp_used = false;
                        break;
                    }
                }
            }
        }
    };

    if (a_rEvent.mouse_moved() ||
        a_rEvent.mouse_dragged())
    {
        if (state_hover(&list) == false)
        { l_remove_temp(); }

        if (v2f::length(a_rEvent.mouse.distance()) > 0.1f)
        {
            discard_time = emp::tim::time_c();
            discard = true;
        }
    }

    switch (eButton)
    {
        case emp::app::button_e::WheelDown:
        case emp::app::button_e::WheelUp:
        {
            if (rKeyboard.m_bShift == false &&
                rKeyboard.m_bCtrl &&
                rKeyboard.m_bAlt == false)
            {
                auto& rui32GalleryWidth = ASE_GAME.user().options.selection.gallery_width;
                if (eButton == emp::app::button_e::WheelDown)
                { rui32GalleryWidth = emp::tt::minof<ui32>(rui32GalleryWidth + 1, 16); }
                else if (eButton == emp::app::button_e::WheelUp)
                { if (rui32GalleryWidth > 0) { --rui32GalleryWidth; } }
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
            break;
        }
        case emp::app::button_e::Left:
        {
            if (a_rEvent.mouse_pressed())
            {
                if (stTapCount == 1)
                { discard = false; }

                discard_time = time_c();
                scroll = mdi.vsb.offset();

                if (stToucheCount == 1 &&
                    (stTapCount == 1 ||
                     stTapCount == 2))
                {
                    if (dragged == nullptr && viewer.magnified == nullptr)
                    {
                        cut = stTapCount == 2;
                        if (ASE_GAME.user().options.selection.edition)
                        {
                            c_size stCount = m_vpThumbnails.count();
                            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                            {
                                thumbnail_c* const pThumbnail = m_vpThumbnails.val(stIndex);
                                if (drag(pThumbnail, stIndex))
                                { break; }
                            }
                        }

                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                }
            }
            else if (a_rEvent.mouse_dragged())
            {
                if (dragged && viewer.magnified == nullptr)
                {
                    dragged_delta = a_rEvent.mouse.distance();

                    bool bLineHover = false;
                    c_size stCount = list.lines().count();
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        line_c* const pLine = list.lines().val(stIndex);
                        if (state_hover(pLine) && pLine != temp_line)
                        {
                            bLineHover = true;
                            break;
                        }
                    }

                    if (bLineHover)
                    { l_remove_temp(); }

                    else if (state_hover(&list) && temp == nullptr)
                    {
                        temp = EMP_FACTORY.store<ase::cor::pool_c>()->new_object_pattern(ALITERAL("ase_cor_pool_temp"));
                        get_object_name(temp->name, U8FORMAT("temp_#0", 1), rvpPools);

                        rvpPools.add(temp);
                        temp_line = add_pool(temp, change_e::Code);
                        EMP_RET_X_IF_NOT(return_e::Error, EMP_FACTORY.write_changes(ASE_GAME.user().options.selection.pools));
                    }

                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
                else if (ASE_GAME.user().options.selection.edition)
                { mdi.vsb.offset_(scroll + a_rEvent.mouse.distance().y); }
            }
            else if (a_rEvent.mouse_released())
            {
                if (stTapCount == 1 && viewer.magnified == nullptr && dragged)
                {
                    viewer.start();
                    dragged = nullptr;
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
                if (stToucheCount == 1)
                {
                    if (state_hover(&list) && dragged && viewer.magnified == nullptr)
                    {
                        auto& rSelection = ASE_GAME.user().options.selection;
                        c_size stCount = list.lines().count();
                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        {
                            if (state_hover(list.lines().val(stIndex)))
                            {
                                auto* const pPool = rSelection.pools->pools.val(stIndex);
                                c_astring& sFile = dragged->ose();
                                pPool->add_source(sFile);
                                temp_used = temp_used || pPool == temp;

                                if (stTapCount == 2)
                                {
                                    clear_drag();
                                    remove_source(sFile);
                                }

                                if (EMP_FACTORY.write_changes(pPool) == false)
                                { return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Error); }
                                break;
                            }
                        }
                    }
                    else
                    {
                        size_t EMP_TT_MAX_VAR(stDragged);
                        size_t EMP_TT_MAX_VAR(stHover);
                        if (state_hover(&mdi) && dragged && viewer.magnified == nullptr)
                        {
                            c_size stCount = m_vpThumbnails.count();

                            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                            {
                                auto* const pThumb = m_vpThumbnails.val(stIndex);
                                if (pThumb == dragged)
                                {
                                    stDragged = stIndex;
                                    if (emp::tt::is_not_max(stHover))
                                    { break; }
                                }

                                if (state_hover(pThumb))
                                {
                                    stHover = stIndex;
                                    if (emp::tt::is_not_max(stDragged))
                                    { break; }
                                }
                            }
                        }

                        if (emp::tt::is_not_max(stDragged) && emp::tt::is_not_max(stHover) && stDragged != stHover)
                        {
                            c_size stOffset = stHover - (stHover < stDragged ? 0 : 1);
                            ASE_GAME.user().options.selection.pool->remove_source(stDragged);
                            ASE_GAME.user().options.selection.pool->insert_source(stOffset, dragged->ose());
                            m_vpThumbnails.remove(stDragged);
                            m_vpThumbnails.insert(stOffset, dragged);
                            mdi.scroller.remove_control(stDragged);
                            mdi.scroller.insert_control(stOffset, dragged);
                            EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(ASE_GAME.user().options.selection.pool));
                        }
                        else if (discard == false)
                        {
                            if (stTapCount == 2)
                            {
                                if (state_hover(&mdi)/* @@0 remove  && (transition_frame == false)*/)
                                {
                                    c_size stCount = m_vpThumbnails.count();
                                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                                    {
                                        thumbnail_c* const pThumbnail = m_vpThumbnails.val(stIndex);
                                        if (state_hover(pThumbnail) && (pThumbnail == dragged || dragged == nullptr))
                                        {
                                            evolution(pThumbnail->ose());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                l_remove_temp();
                if (dragged)
                { dragged->cut_(false); }

                clear_drag();

                temp = nullptr;
                cut = false;
                discard = false;
                // @@0 remove transition_frame = false;
            }
        }
        case emp::app::button_e::Right:
        case emp::app::button_e::Center:
        case emp::app::button_e::Invalid:
        default:
        { break; }
    }
            
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
selection_c::selection_c(void) :
    m_pCtrlSelection(),
    m_pSelection()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
selection_c::~selection_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::public_create(void)
{
    EMP_RET_FALSE_IF_NOT(m_pCtrlSelection->create(&registrar()));
    m_pCtrlSelection->skin_(ASE_GAME.skin().selection.get(ALITERAL("ent.")));
    m_pCtrlSelection->populate_pool_list(0, emp::gui::change_e::Code);

    window().add_control(m_pCtrlSelection);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void selection_c::public_destroy(void)
{ m_pCtrlSelection->destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool selection_c::public_start(void)
{
    m_pCtrlSelection->restore_pool();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

