//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_picture.h"

#include "emp_gui_button.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_model.h"
#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
picture_c::picture_c(void)
{
    on_push.reset(this, false);
    child_(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
picture_c::~picture_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* picture_c::got_focus(c_focus_e, c_bool, c_bool a_bPush, c_change_e a_eChange)
{
    if (state().focusable)
    {
        if (push_on_focus && a_bPush)
        {
            if (on_push.is_valid())
            {
                emp::tt::silent(on_push.set_state(true, a_eChange));
                emp::tt::silent(on_push.set_state(false, a_eChange));
            }
        }

        if (emp::tt::is_not_max(state().index))
        { return this; }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool picture_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    if (model != nullptr)
    { m_v2Size = emp::mat::v4f::xy(model->size()); }

    cpc_resolution_data_s  pResolution = &EMP_GUI_SKIN.resolution;

    c_bool bAutoSizeX = state().autosize.x;
    c_bool bAutoSizeY = state().autosize.y;
    c_bool bAutoScale = skin != nullptr ? autoscale : false;

    if (model == nullptr)
    {
        if (bAutoScale)
        { }
        else if (bAutoSizeX || bAutoSizeY)
        { transformation().size_(0); }
    }
    else
    {
        cpc_padding_data_s pPadding = skin && skin->button ? skin->button->padding : nullptr;
        c_float fPaddingX = pPadding ? pPadding->x() : 0;
        c_float fPaddingY = pPadding ? pPadding->y() : 0;

        c_v2f v2Padding = { fPaddingX, fPaddingY };
        c_v2f v2FreeSize = v2f::sub(transformation().size, v2Padding);

        if (bAutoScale &&
            (bAutoSizeX == false && bAutoSizeY == false))
        {
            c_v2f v2ScaleSize = v2f::mul(m_v2Size, pResolution->scaled_ratio());
            c_v2f v2NewSize = v2f::div(v2FreeSize, v2ScaleSize);

            m_fSize = v2f::min(v2NewSize);
        }
        else if (bAutoSizeX || bAutoSizeY)
        {
            if (bAutoScale == false)
            { m_fSize = size; }
            c_v2f v2AutoSize = { bAutoSizeX ? 1.0f : 0.0f, bAutoSizeY ? 1.0f : 0.0f };
            c_v2f v2NewSize = v2f::select(v2AutoSize, v2f::mul(m_v2Size, v2f::mul(pResolution->scaled_ratio(), { m_fSize, m_fSize } )), v2FreeSize);

            transformation().size_(v2f::add(v2Padding, v2NewSize));
        }
        else
        { m_fSize = size; }
    }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool picture_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    matrix_stacker_c oStacker(a_rStack, transformation(), state().enable);

    c_size stCount = controls().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(controls().val(stIndex)->transform(a_rStack)); }
    update_colors_current(a_rStack.enabled.current());

    c_float fDepth = depth(transformation().fore_depth, 0.0041f);//c_float fDepth = -(emp::ast::cast<float>(a_rStack.previous_depth + 3) * 0.01f); // @@0 factor

    cpc_resolution_data_s pResolution = &EMP_GUI_SKIN.resolution;

    if (skin != nullptr)
    {
        color_s oColor;
        compute_picture_color(*skin, state(), a_rStack.enabled.current(), oColor);
        m_v4Color = oColor.rgba;
    }
    else
    { m_v4Color = {1, 0, 1, 1}; }

    if (model == nullptr)
    { return true; }

    c_v2f v2Min = v4f::xy(model->min());

    c_v2f v2Size = v2f::mul(pResolution->scaled_ratio(), { m_fSize, m_fSize} );
    float fOffsetX = -v2Min.x * v2Size.x;
    float fOffsetY = -v2Min.y * v2Size.y;

    cpc_padding_data_s pPadding = (skin && skin->button) ? skin->button->padding : nullptr;

    c_float fPaddingX = pPadding ? pPadding->x() : 0;
    c_float fPaddingY = pPadding ? pPadding->y() : 0;

    c_float fPaddingLeft = pPadding ? pPadding->left() : 0;
    c_float fPaddingRight = pPadding ? pPadding->right() : 0;
    c_float fPaddingTop = pPadding ? pPadding->top() : 0;
    c_float fPaddingBottom = pPadding ? pPadding->bottom() : 0;

    c_v2f v2PictureSize = v2f::mul(m_v2Size, v2Size);
    c_float fControlWidth = v2PictureSize.x;
    c_float fControlHeight = v2PictureSize.y;

    c_v2f v2ControlSize = transformation().size;
    c_float fWidth = v2ControlSize.x;
    c_float fHeight = v2ControlSize.y;

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

    //float fOffsetX = 0;
    if (bLeft)
    { fOffsetX += fPaddingLeft; }
    else if (bCenter)
    { fOffsetX += fPaddingLeft + (fWidth - fPaddingX - fControlWidth) * 0.5f;}
    else if (bRight)
    { fOffsetX += fWidth - fPaddingRight - fControlWidth; }

    //float fOffsetY = 0;
    if (bTop)
    { fOffsetY -= fPaddingTop + fControlHeight; }
    else if (bMiddle)
    { fOffsetY -= fPaddingTop + fControlHeight + (fHeight - fPaddingY - fControlHeight) * 0.5f; }
    else if (bBottom)
    { fOffsetY -= fHeight - fPaddingBottom; }


    c_v3f v3Position = { fOffsetX, fOffsetY/* - transformation().height()*/, fDepth };
    c_v3f v3Size = { v2Size.x, v2Size.y, 1.0f };

    m44f mModel;
    m44f::identity(mModel);
    m44f::translate(mModel, v3Position, mModel);
    m44f::scale(mModel, v3Size, mModel);

    /*m44f mDepth;
     m44f::identity(mDepth);
     m44f::translate(mDepth, {0, 0, fDepth}, mDepth);
     m44f::mul(mDepth, mModel, mModel);*/

    matrix_stacker_c oModelStacker(a_rStack, mModel, state().enable);
    m_mModel = a_rStack.matrices.current(); // @@0 stack

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e picture_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    auto const& rKeyboard = a_rEvent.keyboard;

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
                c_change_e eChange = change_e::Keyboard;
                EMP_GUI_MANAGER->activate_focus(this, focus_e::First, true, false, eChange);

                c_bool bPressed = rKeyboard.m_bPressed;
                if ((bPressed || on_push.value) &&
                    on_push.is_valid())
                { return on_push.set_state(bPressed, eChange); }
            }
        }
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e picture_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    if (state().hover &&
        state().enable &&
        parent_enable(this))
    {
        auto const& rMouse = a_rEvent.mouse;
        if (a_rEvent.keyboard.modified() == false &&
            rMouse.m_eButton == emp::app::button_e::Left)
        {
            c_change_e eChange = change_e::Mouse;
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
EMP_RETURN bool picture_c::render(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    if (model == nullptr)
    { return true; }

    /*color_data_s oColor;
    oColor.background().raster().set(EMP_GAME().gui_skin().data()->material_color(), emp::mat::v4f::make(0, 1, 1, 1));
    oColor.background().update(transformation().size());
    oColor.background().render(transformation().matrix(), alpha);*/

    emp::gfx::gl_device_c& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_test(true); // @@0 depth done
    rDevice.set_depth_mask(true);

    auto const& lRenderGlyph = [this, &rDevice](c_size a_stProgram, c_size a_stMesh, c_bool a_bBlend)
    {
        rDevice.set_blend(a_bBlend);
        rDevice.set_program(a_stProgram);
        rDevice.set_uniform(emp::gfx::uniform_e::v4Color, m_v4Color);

        emp::gfx::mesh_c const* const pMesh = model->meshes().val(a_stMesh);

        rDevice.set_vertex_buffer(pMesh->vertices());
        rDevice.set_index_buffer(pMesh->indices()); // @@1 global index buffer 8, 16, 32
        rDevice.set_uniform(emp::gfx::uniform_e::m44Transform, m_mModel);
        rDevice.draw(emp::gfx::primitive_e::Triangles);
    };

    lRenderGlyph(rDevice.color_uniform_program(), 0, false);
    lRenderGlyph(rDevice.path_program(), 1, true);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
picture_c& picture_c::skin_(emp::gui::cpc_picture_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_((a_pValue && a_pValue->button) ? a_pValue->button->container : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

