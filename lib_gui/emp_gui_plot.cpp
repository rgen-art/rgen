//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_plot.h"

#include "emp_gui_coordinate.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::fct;
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
plot_c::plot_c(void):
    m_pQuadMeshVertices(),
    m_pQuadMeshIndices(),
    m_pLineMeshVertices(),
    m_pLineMeshIndices()
{
    frame.on_render = function(this, &plot_c::render_plot);

    // @@0 remove
    m_pQuadMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pQuadMeshIndices->create(emp::gfx::usage_e::Dynamic);
    m_pLineMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pLineMeshIndices->create(emp::gfx::usage_e::Dynamic);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
plot_c::~plot_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 refactor
EMP_INTERN EMP_RETURN bool contains_position(v2f::tparam a_v2Client, v2f::tparam a_v2Point, v2f::tparam a_v2Size);
EMP_INTERN EMP_RETURN bool contains_position(v2f::tparam a_v2Client, v2f::tparam a_v2Point, v2f::tparam a_v2Size)
{
    c_float fWidth = a_v2Size.x;
    c_float fHeight = a_v2Size.y;

    c_float fHalfWidth = fWidth * 0.5f;
    c_float fHalfHeight = fHeight * 0.5f;

    c_float fPointX = a_v2Point.x;
    c_float fPointY = a_v2Point.y;

    c_float fX0 = fPointX - fHalfWidth;
    c_float fX1 = fPointX + fHalfWidth;

    c_float fY0 = fPointY - fHalfHeight;
    c_float fY1 = fPointY + fHalfHeight;

    c_float fX = a_v2Client.x;
    c_float fY = a_v2Client.y;

    if (fX >= fX0 &&
        fX <= fX1 &&
        fY >= fY0 &&
        fY <= fY1)
    { return true; }
    return false;
}

// @@0 refactor
EMP_INTERN EMP_RETURN size_t contains_position(emp::cnt::vector<v2f> const& a_rvv2Points, v2f::tparam a_v2Position,
    v2f::tparam a_v2Size);
EMP_INTERN EMP_RETURN size_t contains_position(emp::cnt::vector<v2f> const& a_rvv2Points, v2f::tparam a_v2Position,
    v2f::tparam a_v2Size)
{
    c_size stCount = a_rvv2Points.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (contains_position(a_v2Position, a_rvv2Points.ref(stIndex), a_v2Size))
        { return stIndex; }
    }
    return emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool plot_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    c_v2f v2PointSize = point_size();
    c_v2f v2HalfPointSize = v2f::mul(v2PointSize, v2f::all(0.5f));

    c_v4f v4PlotHover  = (skin && skin->plot_hover)  ? skin->plot_hover->rgba  : v4f::all(0);
    c_v4f v4PlotNormal = (skin && skin->plot_normal) ? skin->plot_normal->rgba : v4f::all(0);

    m_pLineMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pLineMeshIndices->create(emp::gfx::usage_e::Dynamic);

    m_pQuadMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pQuadMeshIndices->create(emp::gfx::usage_e::Dynamic);

    c_float fDepth = depth(transformation().fore_depth, 0.0001f);

    c_size stCount = points.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_v2f v2Point = points.ref(stIndex);
        c_v2f v2FramePoint = frame.frame_to_client(v2Point);

        {
            c_v2f v2Client = frame.client_to_frame(frame.client_position());
            c_bool bInside = contains_position(v2Client, v2Point, get_point_size());
            c_v4f& v4PlotColor = bInside ? v4PlotHover : v4PlotNormal;

            c_v2f v2LeftBottom = v2f::sub(v2FramePoint, v2HalfPointSize);
            c_v2f v2RightTop = v2f::add(v2LeftBottom, v2PointSize);

            c_float fLeft = v2LeftBottom.x;
            c_float fRight = v2RightTop.x;
            c_float fBottom = v2LeftBottom.y;
            c_float fTop = v2RightTop.y;

            emp::gfx::emit_quad(m_pQuadMeshVertices->count(), *m_pQuadMeshIndices);
            m_pQuadMeshVertices->add({ fLeft  , fBottom , fDepth }, v4PlotColor);
            m_pQuadMeshVertices->add({ fRight , fBottom , fDepth }, v4PlotColor);
            m_pQuadMeshVertices->add({ fLeft  , fTop    , fDepth }, v4PlotColor);
            m_pQuadMeshVertices->add({ fRight , fTop    , fDepth }, v4PlotColor);
        }

        {
            emp::gfx::emit_line(m_pLineMeshVertices->count(), *m_pLineMeshIndices);
            m_pLineMeshVertices->add(v3f::all(v2FramePoint, fDepth));
            m_pLineMeshVertices->add(v3f::all(frame.frame_to_client(points.ref((stIndex + 1) % stCount)), fDepth));
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v2f plot_c::point_size(void) const
{ return v2f::mul(EMP_GUI_SKIN.resolution.ratio(), 0.02f); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v2f plot_c::get_point_size(void) const
{ return get_point_size(point_size()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret plot_c::get_point_size(v2f::tparam a_v2Client) const
{
    c_float fScaleX = frame.data.final_scale_x();
    c_float fScaleY = frame.data.final_scale_y();

    v2f::tret v2Scale = { fScaleX, fScaleY };
    v2f::tret v2Size = v2f::div(a_v2Client, v2Scale);

    return v2Size;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret plot_c::get_point_delta(c_size a_stIndex, v2f::tparam a_v2Delta)
{
    c_v2f v2CurvePoint = points.ref(a_stIndex);
    c_v2f v2ClientPoint = frame.frame_to_client(v2CurvePoint);

    c_v2f v2DestPoint = v2f::add(v2ClientPoint, a_v2Delta);

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();

    float fNewX = v2DestPoint.x;
    float fNewY = v2DestPoint.y;

    if (fNewX < 0)
    { fNewX = 0; }
    if (fNewY < 0)
    { fNewY = 0; }
    if (fNewX > fWidth)
    { fNewX = fWidth; }
    if (fNewY > fHeight)
    { fNewY = fHeight; }

    c_v2f v2NewPoint = { fNewX, fNewY };
    c_v2f v2Offset = v2f::sub(v2DestPoint, v2NewPoint);
    c_v2f v2NewDelta = v2f::sub(a_v2Delta, v2Offset);

    return get_point_size(v2NewDelta);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t plot_c::move_point(c_size a_stIndex, v2f::tparam a_v2Delta)
{
    c_v2f v2NewDelta = get_point_delta(a_stIndex, a_v2Delta);
    return on_point_moved(a_stIndex, v2NewDelta);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void plot_c::add_point(c_size a_stIndex, v2f::tparam a_v2Position)
{ on_point_added(a_stIndex, a_v2Position); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void plot_c::remove_point(c_size a_stIndex)
{ on_point_removed(a_stIndex); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e plot_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false ||
        state().enable == false)
    { return return_e::Unused; }

    auto const& rMouse = a_rEvent.mouse;

    c_v2f v2Position = rMouse.m_v2fPosition;
    c_v2f v2RelPosition = v2f::add(v2f::one(), v2Position);

    c_bool bContains = state().hover;
    c_bool bPressed = a_rEvent.mouse_pressed();
    c_bool bReleased = a_rEvent.mouse_released();
    c_bool bDragged = a_rEvent.mouse_dragged();

    c_bool bModified = a_rEvent.keyboard.modified();
    c_size stTapCount = rMouse.m_stTapCount;

    emp::app::c_button_e eButton = rMouse.m_eButton;
    if (eButton == emp::app::button_e::Left &&
        bModified == false)
    {
        if (stTapCount == 1)
        {
            if (bContains)
            {
                c_size stPlot = contains_position(points, frame.client_to_frame(
                    frame.client_position()), get_point_size());
                if (emp::tt::is_not_max(stPlot) &&
                    bPressed)
                {
                    if (emp::tt::is_max(m_stPlot))
                    {
                        m_v2PointOrigin = v2RelPosition;
                        m_v2PointDelta = emp::mat::v2f::zero();
                        m_stPlot = stPlot;
                    }
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                else if (emp::tt::is_not_max(m_stPlot) &&
                         bReleased)
                {
                    m_stPlot = move_point(m_stPlot, m_v2PointDelta);
                    EMP_TT_MAX_VAR(m_stPlot);
                    m_v2PointOrigin = emp::mat::v2f::zero();
                    m_v2PointDelta = emp::mat::v2f::zero();
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                else if (emp::tt::is_not_max(m_stPlot) &&
                         bDragged)
                {
                    m_v2PointDelta = v2f::sub(v2RelPosition, m_v2PointOrigin);
                    m_v2PointOrigin = v2RelPosition;
                    m_stPlot = move_point(m_stPlot, m_v2PointDelta);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else
            {
                if (emp::tt::is_not_max(m_stPlot))
                {
                    m_stPlot = move_point(m_stPlot, m_v2PointDelta);
                    EMP_TT_MAX_VAR(m_stPlot);
                    m_v2PointOrigin = emp::mat::v2f::zero();
                    m_v2PointDelta = emp::mat::v2f::zero();
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }
        else if (bPressed && bContains)
        {
            c_v2f v2Curve = frame.client_to_frame(frame.client_position());
            c_size stPoint = contains_position(points, v2Curve, get_point_size());
            if (emp::tt::is_not_max(stPoint))
            { remove_point(stPoint); }
            else
            { add_point(stPoint, v2Curve); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    else if (
        eButton == emp::app::button_e::Right &&
        bModified == false)
    {
        if (stTapCount == 1 && bContains && bPressed &&
            on_point_right_clicked.is_valid())
        {
            c_size stPlot = contains_position(points, frame.client_to_frame(frame.client_position()), get_point_size());
            if (emp::tt::is_not_max(stPlot))
            { on_point_right_clicked(stPlot); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    return tbase1::process_mouse(a_rEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool plot_c::auto_resize(void)
{
    c_size stCount = points.count();
    if ((stCount > 0) == false)
    { return true; }

    float EMP_TT_MAX_VAR(fMinX);
    float EMP_TT_MAX_VAR(fMinY);
    
    float EMP_TT_MIN_VAR(fMaxX);
    float EMP_TT_MIN_VAR(fMaxY);

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_v2f& v2Point = points.ref(stIndex);
        c_float fX = v2Point.x;
        c_float fY = v2Point.y;

        fMinX = emp::tt::minof(fMinX, fX);
        fMinY = emp::tt::minof(fMinY, fY);

        fMaxX = emp::tt::maxof(fMaxX, fX);
        fMaxY = emp::tt::maxof(fMaxY, fY);
    }

    c_v2f v2Size = { fMaxX - fMinX, fMaxY - fMinY };
    c_v2f v2Scale = { frame.data.final_scale_x(), frame.data.final_scale_y() };

    EMP_RET_FALSE_IF_NOT(tbase1::auto_resize(v2f::add(v2f::mul(v2Size, v2Scale), v2Scale)));

    c_v2f v2ModelCenter = v2f::add({ fMinX, fMinY }, v2f::mul(v2Size, v2f::half()));

    frame.center_origin();
    frame.set_origin(v2f::sub(frame.origin(), v2f::mul(v2ModelCenter, v2Scale)));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void plot_c::render_plot(frame_c const& a_rSender)
{
    c_m44f& rMatrix = a_rSender.transformation().matrix;

    auto& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_test(true); // @@0 depth
    rDevice.set_depth_mask(true);

    rDevice.set_vertex_buffer(&*m_pQuadMeshVertices);
    rDevice.set_index_buffer(&*m_pQuadMeshIndices);
    rDevice.set_program(rDevice.color_attribute_program());
    rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
    rDevice.draw(emp::gfx::primitive_e::Triangles);

    if (skin && skin->line)
    {
        rDevice.set_vertex_buffer(&*m_pLineMeshVertices);
        rDevice.set_index_buffer(&*m_pLineMeshIndices);
        rDevice.set_program(rDevice.color_uniform_program());
        rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
        rDevice.set_uniform(uniform_e::v4Color, skin->line->rgba);
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }
    on_render(*this);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
plot_c& plot_c::skin_(cpc_plot_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->coordinate : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

