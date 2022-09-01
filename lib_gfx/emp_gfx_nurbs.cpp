//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gfx_nurbs.h"

#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_nurbs.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
gl_nurbs_c::gl_nurbs_c(void):
    m_stIteration(0),
    m_pNurbs(),
    m_bChanged(true),
    m_pLineVertices(),
    m_pLineIndices(),
    m_v4InColor(v4f::all(1)),
    m_v4OutColor(v4f::all(1))
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
gl_nurbs_c::~gl_nurbs_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::create(void)
{
    destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::destroy(void)
{
    m_stIteration = 0;
    m_pNurbs->destroy();
    m_bChanged = true;
    m_pLineVertices->destroy();
    m_v4InColor = v4f::all(1);
    m_v4OutColor = v4f::all(1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::update(void)
{
    if (m_bChanged)
    {
        c_float fDuration = 1.0f;
        c_size stSampleCount = m_stIteration;
        c_float fStep = fDuration / emp::ast::cast<float>(stSampleCount);

        m_pLineVertices->create(emp::gfx::usage_e::Dynamic);

        auto const lVertex = [this, fStep](c_size a_stIndex)
        {
            c_float fNext = emp::ast::cast<float>(a_stIndex) * fStep;
            c_float fInvNext = 1.0f - fNext;
            c_v4f v4Sample = m_pNurbs->sample(fNext);
            c_v4f v4Color = v4f::add(v4f::mul(m_v4InColor, v4f::all(fInvNext)), v4f::mul(m_v4OutColor, v4f::all(fNext)));
            m_pLineVertices->add(v4f::xyz(v4Sample), v4Color);
        };

        size_t stIndex = 0;
        for (; stIndex < stSampleCount; ++stIndex)
        { lVertex(stIndex); }
        lVertex(stIndex);

        m_pLineIndices = EMP_GRAPHICS.line_index_buffer(stSampleCount);

        m_bChanged = false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::render(m44f::tparam a_m44Transform)
{
    auto& rDevice = EMP_GRAPHICS;
    rDevice.set_depth_test(true); // @@0 depth
    rDevice.set_depth_mask(true);
    rDevice.set_vertex_buffer(&*m_pLineVertices);
    rDevice.set_index_buffer(m_pLineIndices);// line_strip
    rDevice.set_program(rDevice.color_attribute_program());
    rDevice.set_uniform(uniform_e::m44Transform, a_m44Transform);
    rDevice.draw(emp::gfx::primitive_e::Lines);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t gl_nurbs_c::point_count(void) const
{ return m_pNurbs->points().count(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::add_point(c_v3f& a_v3Position)
{
    m_pNurbs->add_point(v4f::all(a_v3Position, 1), true);
    m_bChanged = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::set_point(c_size a_stIndex, c_v3f& a_v3Position)
{
    c_v4f v4Point = m_pNurbs->points().ref(a_stIndex);
    c_v3f v3Point = v4f::xyz(v4Point);

    if (bool_and(v3f::equal(v3Point, a_v3Position)) == false)
    {
        m_pNurbs->set_point(a_stIndex, v4f::all(a_v3Position, 1));
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::clear_points(void)
{ m_pNurbs->create(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::set_in_color(v4f::tparam a_v4Value)
{
    if (bool_and(v4f::equal(m_v4InColor, a_v4Value)) == false)
    {
        m_v4InColor = a_v4Value;
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_nurbs_c::set_out_color(v4f::tparam a_v4Value)
{
    if (bool_and(v4f::equal(m_v4OutColor, a_v4Value)) == false)
    {
        m_v4OutColor = a_v4Value;
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

