//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_outline.h"

#include "emp_gui_coordinate.h"
#include "emp_gui_manager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_gl_device.h"

#include "emp_mat_glyph.h"

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
outline_c::outline_c(void):
    m_pLineMeshVertices(),
    m_pLineMeshIndices(),
    m_pQuadMeshVertices(),
    m_pQuadMeshIndices()
{
    on_render              = function(this, &outline_c::render_outline);
    on_point_moved         = function(this, &outline_c::move_point);
    on_point_added         = function(this, &outline_c::add_point);
    on_point_removed       = function(this, &outline_c::remove_point);
    on_point_right_clicked = function(this, &outline_c::right_click_point);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
outline_c::~outline_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool outline_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    points.resize(0);

    auto const& rvPoints = outline->points;
    c_size stCount = rvPoints.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { points.add(rvPoints.ref(stIndex)); }

    return tbase1::resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool outline_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    m_pLineMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pLineMeshIndices->create(emp::gfx::usage_e::Dynamic);

    m_pQuadMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pQuadMeshIndices->create(emp::gfx::usage_e::Dynamic);

    c_float fDepth = depth(transformation().fore_depth, 0);

    c_size stCount = outline->points.count();
    if (stCount)
    {
        {
            c_float fDuration = 1.0f;
            c_size stSampleCount = 100;
            c_float fStep = fDuration / emp::ast::cast<float>(stSampleCount);
            v2f v2FirstSample = v2f::zero();
            if (outline->sample(0.0f, v2FirstSample))
            {
                v2f v2Current = frame.frame_to_client(v2FirstSample);
                for (size_t stIndex = 0; stIndex < stSampleCount; ++stIndex)
                {
                    c_float fNext = emp::ast::cast<float>(stIndex + 1) * fStep;
                    v2f v2NextSample = v2f::zero();
                    if (outline->sample(fNext, v2NextSample))
                    {
                        c_v2f v2Next = frame.frame_to_client(v2NextSample);
                        emp::gfx::emit_line(m_pLineMeshVertices->count(), *m_pLineMeshIndices);
                        m_pLineMeshVertices->add(v3f::all(v2Current, fDepth));
                        m_pLineMeshVertices->add(v3f::all(v2Next   , fDepth));
                        v2Current = v2Next;
                    }
                }
            }
        }
        

        {
            c_v2f v2PointSize = point_size();
            c_v2f v2HalfPointSize = v2f::mul(v2PointSize, 0.5f);
            c_v4f v4Point = skin->control->rgba;
            c_v4f v4Zero = v4f::all(0);
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_v2f& rPoint = outline->points.ref(stIndex);
                c_bool bControl = outline->controls.val(stIndex);
                c_v2f v2Point = frame.frame_to_client(rPoint);
                c_v4f& v4Color = bControl ? v4Point : v4Zero;
                c_v2f v2LeftBottom = v2f::sub(v2Point, v2HalfPointSize);
                c_v2f v2RightTop = v2f::add(v2LeftBottom, v2PointSize);

                emp::gfx::emit_quad(m_pQuadMeshVertices->count(), *m_pQuadMeshIndices);
                m_pQuadMeshVertices->add(v3f::all(v2LeftBottom, fDepth), v4Color);
                m_pQuadMeshVertices->add({ v2RightTop.x, v2LeftBottom.y, fDepth }, v4Color);
                m_pQuadMeshVertices->add({ v2LeftBottom.x, v2RightTop.y, fDepth }, v4Color);
                m_pQuadMeshVertices->add(v3f::all(v2RightTop, fDepth), v4Color);
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void outline_c::render_outline(plot_c const& a_rSender)
{
    c_m44f& rMatrix = a_rSender.frame.transformation().matrix;

    auto& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_test(true); // @@0 depth
    rDevice.set_depth_mask(true);

    rDevice.set_vertex_buffer(&*m_pLineMeshVertices);
    rDevice.set_index_buffer(&*m_pLineMeshIndices);
    rDevice.set_program(rDevice.color_uniform_program());
    rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
    rDevice.set_uniform(uniform_e::v4Color, skin->line->rgba);
    rDevice.draw(emp::gfx::primitive_e::Lines);

    rDevice.set_vertex_buffer(&*m_pQuadMeshVertices);
    rDevice.set_index_buffer(&*m_pQuadMeshIndices);
    rDevice.set_program(rDevice.color_attribute_program());
    rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
    rDevice.draw(emp::gfx::primitive_e::Triangles);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t outline_c::move_point(c_size a_stIndex, v2f::tparam a_v2Delta)
{
    v2f& rPoint = outline->points.ref(a_stIndex);
    rPoint = v2f::add(rPoint, a_v2Delta);
    return a_stIndex;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void outline_c::add_point(c_size EMP_SILENT(a_stIndex), v2f::tparam a_v2Position)
{
    float EMP_TT_MIN_VAR(fMin);
    size_t EMP_TT_MAX_VAR(stMin);

    {
        emp::cnt::vector<v2f> const& rvPoints = outline->points;
        c_size stCount = rvPoints.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size st0 = stIndex % stCount;
            c_size st1 = (stIndex + 1) % stCount;

            if (st0 == st1)
            {
                EMP_TT_MAX_VAR(stMin);
                break;
            }

            c_v2f& p0 = rvPoints.ref(st0);
            c_v2f& p1 = rvPoints.ref(st1);

            float fCurrent = 0.0f;
            if (emp::mat::point_segment_sq_distance(a_v2Position, p0, p1, fCurrent))
            {
                if (fCurrent < fMin)
                {
                    fMin = fCurrent;
                    stMin = st0;
                }
            }
        }
    }

    emp::cnt::vector<v2f>& rvPoints = outline->points;
    c_v2f oPoint = a_v2Position;
    
    if (emp::tt::is_max(stMin))
    { rvPoints.add(oPoint); }
    else
    {
        c_size stIndex = stMin + 1;
        if (stIndex >= rvPoints.count())
        { rvPoints.add(oPoint); }
        else
        { rvPoints.insert(stIndex, oPoint); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void outline_c::remove_point(c_size a_stIndex)
{ outline->points.remove(a_stIndex); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void outline_c::right_click_point(c_size a_stIndex)
{
    EMP_LOG_INFO("RIGHT");
    bool& rbControl = outline->controls.ref(a_stIndex);
    rbControl = rbControl == false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
outline_c& outline_c::skin_(cpc_outline_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->plot : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

