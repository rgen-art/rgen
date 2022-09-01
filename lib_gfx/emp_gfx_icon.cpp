//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gfx_icon.h"

#include "emp_gfx_gl_device.h"
#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_glyph.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
icon_c::icon_c(void):
    m_sName(),
    m_vpGlyphs(),
    m_pModel()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
icon_c::~icon_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool icon_c::create(void)
{
    m_pModel->destroy();
    auto oFinally = emp::tt::finally([this] { m_pModel->destroy(); });

    emp::cnt::vector<emp::cnt::shell<v3f>> vsPoints; //@@1 perf

    auto& rGraphics = EMP_GRAPHICS;
    auto& rvfScratchPad = rGraphics.m_vfScratchPad;

    c_size stCount = m_vpGlyphs.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::glyph_s const* const pGlyph = m_vpGlyphs.val(stIndex);

        auto const& lTriangulate = [this, pGlyph](auto const& a_rTriangulate, auto const& a_rGet, vector<float>& a_rvfScratchPad) EMP_RETURN -> bool
        {
            a_rvfScratchPad.clear();

            size_t stStride = 0;
            EMP_RET_FALSE_IF_NOT(a_rTriangulate(pGlyph->outlines, a_rvfScratchPad, stStride));
            EMP_RET_FALSE_IF_NOT(stStride > 0);

            c_size stFloatCount = a_rvfScratchPad.count();
            EMP_RET_FALSE_IF_NOT((stFloatCount % stStride) != 0);

            c_size stVertexCount = stFloatCount / stStride;
            EMP_RET_FALSE_IF_NOT(stVertexCount > 0);

            buffer_t<v4f, v4f>* pBuffer = EMP_ALLOCATE((buffer_t<v4f, v4f>));
            pBuffer->create(usage_e::Static);
            for (size_t stVertex = 0; stVertex < stVertexCount; ++stVertex)
            { a_rGet(a_rvfScratchPad, stVertex * stStride, *pBuffer); }

            mesh_c* pMesh = EMP_ALLOCATE(mesh_c);
            m_pModel->add_mesh(pMesh);

            EMP_RET_FALSE_IF_NOT(pMesh->create(primitive_e::Triangles, pBuffer, nullptr));
            EMP_RET_FALSE_IF_NOT(pMesh->compute_bounds());

            return true;
        };

        {
            auto const& lGetInside = [](vector<float> const& a_rvfValues, c_size a_stOffset, buffer_t<v4f, v4f>& a_rBuffer)
            {
                a_rBuffer.add(
                    { a_rvfValues.val(a_stOffset + 0), a_rvfValues.val(a_stOffset + 1), 0, 0 },
                    { 0, 0, a_rvfValues.val(a_stOffset + 2), 0 } );
            };

            EMP_RET_FALSE_IF_NOT(lTriangulate(emp::mat::triangulate_inside, lGetInside, rvfScratchPad));
        }

        {
            auto const& lGetOutside = [](vector<float> const& a_rvfValues, c_size a_stOffset, buffer_t<v4f, v4f>& a_rBuffer)
            { a_rBuffer.add({ a_rvfValues.val(a_stOffset + 0), a_rvfValues.val(a_stOffset + 1), 0, 0 }, { 0, 0, 0, 0 }); };

            EMP_RET_FALSE_IF_NOT(lTriangulate(emp::mat::triangulate_outside, lGetOutside, rvfScratchPad));
        }

        EMP_RET_FALSE_IF_NOT(m_pModel->compute_bounds());
        // @@0 if (a_bAlignLeft)
        // @@0 { m_pModel->align_left(); }
    }

    oFinally.release();

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void icon_c::destroy(void)
{
    m_sName = astring();
    m_vpGlyphs.resize(0);
    m_pModel = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

