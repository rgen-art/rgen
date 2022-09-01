//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_control_i.h"

#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"
#include "emp_app_application.h"

#include "emp_gfx_text.h"
#include "emp_gfx_gl_device.h"

#include "emp_mat_intersect.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
control_i::~control_i(void)
{
    if (EMP_GUI_MANAGER != nullptr)
    { EMP_GUI_MANAGER->clear_focus(this); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_GUI_RETURN_HOOK
EMP_RETURN return_e return_hook(c_return_e a_eValue)
{
    switch (a_eValue)
    {
        case return_e::Unused:
        {
            return a_eValue;
        }
        case return_e::Consumed:
        {
            return a_eValue;
        }
        case return_e::Error:
        {
            return a_eValue;
        }
        default:
        {
            return a_eValue;
        }
    };
}

EMP_RETURN return_e return_hook(emp::app::event_s const& a_rEvent, c_return_e a_eValue)
{
    if (a_rEvent.mouse_pressed())
    {
        if (a_rEvent.mouse.m_stTapCount == 2)
        {
            int kk = 0;
            kk++;
        }
        int x = 0;
        ++x;
    }
    if (a_rEvent.mouse_released())
    {
        if (a_eValue == return_e::Consumed)
        {
            int x = 0;
            ++x;
        }
        else
        {
            int y = 0;
            ++y;
        }
    }
    switch (a_eValue)
    {
        case return_e::Unused:
        {
            return a_eValue;
        }
        case return_e::Consumed:
        {
            if (a_rEvent.mouse_pressed())
            {
                return a_eValue;
            }
            else if (a_rEvent.mouse_released())
            {
                if (a_rEvent.mouse.m_stTapCount == 2)
                {
                    int x = 0;
                    x++;
                }
                return a_eValue;
            }
            else if (a_rEvent.mouse_moved())
            {
                return a_eValue;
            }
            else if (a_rEvent.mouse_dragged())
            {
                return a_eValue;
            }
            return a_eValue;
        }
        case return_e::Error:
        {
            return a_eValue;
        }
        default:
        {
            return a_eValue;
        }
    };
}
#else
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void render(c_m44f& a_rMatrix, c_v2f a_v2LeftBottom, c_v2f a_v2RightTop, c_float a_fDepth, c_v4f& a_v4Color, buffer_i* const a_pIndices, emp::gfx::c_primitive_e a_ePrimitive);
void render(c_m44f& a_rMatrix, c_v2f a_v2LeftBottom, c_v2f a_v2RightTop, c_float a_fDepth, c_v4f& a_v4Color, buffer_i* const a_pIndices, emp::gfx::c_primitive_e a_ePrimitive)
{
    if (a_v4Color.x <= 0 &&
        a_v4Color.y <= 0 &&
        a_v4Color.z <= 0 &&
        a_v4Color.w <= 0)
    {
        return;
    }

    auto& rDevice = EMP_GRAPHICS;

    auto* const pBuffer = EMP_GRAPHICS.v3f_buffer();
    c_bool bLine = a_ePrimitive == primitive_e::Lines;

    c_float fOffsetX = 0; // @@0 scissor offset
    c_float fOffsetY = 0; // @@0 scissor offset
    c_float px = bLine ? emp::ast::cast<float>(fOffsetX / EMP_APPLICATION.view_pixel_width()) : 0;
    c_float py = bLine ? emp::ast::cast<float>(fOffsetY / EMP_APPLICATION.view_pixel_height()) : 0;

    /*auto const l_mod_pixel = [](c_float a_fValue) EMP_RETURN -> float
    {
        return a_fValue;
    };*/

    c_float fLeft = a_v2LeftBottom.x + px;
    c_float fBottom = a_v2LeftBottom.y + 2 * py;

    c_float fRight = a_v2RightTop.x - 2 * px;
    c_float fTop = a_v2RightTop.y - py;


    pBuffer->create(emp::gfx::usage_e::Dynamic);
    pBuffer->add( { fLeft  , fBottom , a_fDepth } );
    pBuffer->add( { fRight , fBottom , a_fDepth } );
    pBuffer->add( { fLeft  , fTop    , a_fDepth } );
    pBuffer->add( { fRight , fTop    , a_fDepth } );

    rDevice.set_blend(true);
    rDevice.set_depth_test(true);
    rDevice.set_depth_mask(true);
    rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
    rDevice.set_vertex_buffer(pBuffer);
    rDevice.set_index_buffer(a_pIndices);
    rDevice.set_program(rDevice.color_uniform_program());
    rDevice.set_uniform(uniform_e::m44Transform, a_rMatrix);
    rDevice.set_uniform(uniform_e::v4Color, a_v4Color);
    rDevice.draw(a_ePrimitive);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void render(c_m44f& a_rMatrix, c_v2f a_v2Size, c_float a_fDepth, c_v4f& a_v4Color, buffer_i* const a_pIndices, emp::gfx::c_primitive_e a_ePrimitive);
void render(c_m44f& a_rMatrix, c_v2f a_v2Size, c_float a_fDepth, c_v4f& a_v4Color, buffer_i* const a_pIndices, emp::gfx::c_primitive_e a_ePrimitive)
{
    return emp::gui::render(a_rMatrix, {0, -a_v2Size.y}, {a_v2Size.x, 0}, a_fDepth, a_v4Color, a_pIndices, a_ePrimitive);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void draw_quad_from_bounds(c_m44f& a_rMatrix, c_v2f a_v2LeftBottom, c_v2f a_v2RightTop, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire)
{
    emp::gfx::buffer_i* const pIndices = a_bWire ? EMP_GRAPHICS.border_index_buffer(enum_to(border_e::All)) : EMP_GRAPHICS.quad_index_buffer();
    auto const ePrimitive = a_bWire ? emp::gfx::primitive_e::Lines: emp::gfx::primitive_e::Triangles;
    emp::gui::render(a_rMatrix, a_v2LeftBottom, a_v2RightTop, a_fDepth, a_v4Color, pIndices, ePrimitive);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void draw_quad(c_m44f& a_rMatrix, c_v2f a_v2Size, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire)
{ return draw_quad_from_bounds(a_rMatrix, {0, -a_v2Size.y}, {a_v2Size.x, 0}, a_fDepth, a_v4Color, a_bWire); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void draw_quad(c_v2f a_v2Position, c_v2f& a_v2Size, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire)
{
    m44f mIdentity;
    m44f::identity(mIdentity);

    c_float fLeft = a_v2Position.x - a_v2Size.x;
    c_float fRight = a_v2Position.x + a_v2Size.x;
    c_float fBottom = a_v2Position.y - a_v2Size.y;
    c_float fTop = a_v2Position.y + a_v2Size.y;
    emp::gfx::buffer_i* const pIndices = a_bWire ? EMP_GRAPHICS.border_index_buffer(enum_to(border_e::All)) : EMP_GRAPHICS.quad_index_buffer();
    auto const ePrimitive = a_bWire ? emp::gfx::primitive_e::Lines: emp::gfx::primitive_e::Triangles;
    emp::gui::render(mIdentity, { fLeft, fBottom }, { fRight, fTop }, a_fDepth, a_v4Color, pIndices, ePrimitive);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void draw_quad(c_v2f a_v2Position, c_float a_fSize, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire)
{ draw_quad(a_v2Position, {a_fSize, a_fSize}, a_fDepth, a_v4Color, a_bWire); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void border_data_s::render(c_transformation_data_s& a_rTransform, c_float a_fDepth) const
{
    if (border == border_e::none)
    { return; }

    emp::gui::render(a_rTransform.matrix, a_rTransform.size, a_fDepth, color, EMP_GRAPHICS.border_index_buffer(enum_to(border)), emp::gfx::primitive_e::Lines);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void raster_data_s::render(c_transformation_data_s& a_rTransform EMP_UNUSED, c_float a_fDepth EMP_UNUSED) const
{
    emp::gui::render(a_rTransform.matrix, a_rTransform.size, a_fDepth, color, EMP_GRAPHICS.quad_index_buffer(), emp::gfx::primitive_e::Triangles);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool outside(cpc_control_i a_pControl, c_v2f a_v2Size)
{
    EMP_ASSERT(a_pControl != nullptr);
    if (a_pControl->state().visible == false ||
        a_pControl->state().renderable == false)
    { return true; }

    return emp::mat::rectangle_outside_rectangle(
        a_pControl->transformation().position,
        a_pControl->transformation().size,
        emp::mat::v2f::zero(),
        a_v2Size);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float font_data_s::line_height(void) const
{ return pointer->line_height(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float padding_data_s::left(void) const
{ return EMP_GUI_SKIN.padding_x(m_v2LeftTop.x, scale); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float padding_data_s::right(void) const
{ return EMP_GUI_SKIN.padding_x(m_v2RightBottom.x, scale); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float padding_data_s::top(void) const
{ return EMP_GUI_SKIN.padding_y(m_v2LeftTop.y, scale); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float padding_data_s::bottom(void) const
{ return EMP_GUI_SKIN.padding_y(m_v2RightBottom.y, scale); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool clipped(cpc_control_i a_pControl, c_v2f a_v2Size)
{
    EMP_ASSERT(a_pControl != nullptr);
    if (a_pControl->state().visible == false ||
        a_pControl->state().renderable == false)
    { return false; }

    return emp::mat::rectangle_inside_rectangle(
        a_pControl->transformation().position,
        a_pControl->transformation().size,
        emp::mat::v2f::zero(),
        a_v2Size) == false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool clipped(emp::cnt::shell<pc_control_i> const& a_rvpControls, c_v2f a_v2Size)
{
    c_size stCount = a_rvpControls.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_control_i pControl = a_rvpControls.val(stIndex);
        if (clipped(pControl, a_v2Size))
        { return true; }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::v2f::tret control_to_client(c_m44f& EMP_SILENT(a_mTransform), v2f::tparam a_v2Control)
{ return a_v2Control; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret gui_to_gl(c_m44f& a_mTransform, v2f::tparam a_v2Client)
{ return v4f::xy(m44f::mul_v4(a_mTransform, v4f::all(a_v2Client, 0.0f, 1.0f))); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret gl_to_gui(v2f::tparam a_v2Screen, c_m44f& a_rClient)
{
    c_v2f v2Screen = gui_to_gl(a_rClient, v2f::zero());
    c_v2f v2Delta = v2f::sub(a_v2Screen, v2Screen);
    c_v2f v2Client = { v2Delta.x, v2Delta.y };
    return v2Client;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool contains_mouse(c_m44f& a_mTransform, v2f::tparam a_v2Size, v2f::tparam a_v2Value)
{
    // @@4 : picking 3d;
    c_v2f v2TransformedMin = gui_to_gl(a_mTransform, {0, -a_v2Size.y});
    c_v2f v2TransformedMax = gui_to_gl(a_mTransform, {a_v2Size.x, 0});
    c_v2f v2Contains = v2f::mul(v2f::more_equal(a_v2Value, v2TransformedMin),
                                v2f::less_equal(a_v2Value, v2TransformedMax));

    return bool_and(v2Contains);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool focus(cpc_control_i a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    if (a_pValue == nullptr)
    { return false; }

    cpc_control_i pFocus = EMP_GUI_MANAGER->focus();
    if (pFocus == nullptr)
    { return false; }

    return a_pValue == pFocus;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool parent_enable(cpc_control_i a_pValue)
{
    c_control_i* pParent = a_pValue->parent();
    while (pParent)
    {
        if (pParent->state().enable == false)
        { return false; }
        pParent = pParent->parent();
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_control_i* current_focus(void)
{ return EMP_GUI_MANAGER->focus(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

