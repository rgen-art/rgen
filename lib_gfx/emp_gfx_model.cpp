//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gfx_model.h"

#include "emp_gfx_gl_device.h"
#include "emp_gfx_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

/*constexpr array<attribute_s, 1, true> declaration_t<v2f>::sc_aAttributes;
constexpr array<attribute_s, 2, true> declaration_t<v2f, v2f>::sc_aAttributes;
constexpr array<attribute_s, 2, true> declaration_t<v2f, v4f>::sc_aAttributes;
constexpr array<attribute_s, 2, true> declaration_t<v4f, v4f>::sc_aAttributes;
constexpr array<attribute_s, 1, true> declaration_t<ui8>::sc_aAttributes;
constexpr array<attribute_s, 1, true> declaration_t<ui16>::sc_aAttributes;

constexpr shell<attribute_s const> const declaration_t<v2f>::sc_hAttributes;
constexpr shell<attribute_s const> const declaration_t<v2f, v2f>::sc_hAttributes;
constexpr shell<attribute_s const> const declaration_t<v2f, v4f>::sc_hAttributes;
constexpr shell<attribute_s const> const declaration_t<v4f, v4f>::sc_hAttributes;
constexpr shell<attribute_s const> const declaration_t<ui8>::sc_hAttributes;
constexpr shell<attribute_s const> const declaration_t<ui16>::sc_hAttributes;*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void unlock_buffer(c_size a_stBuffer)
{ EMP_GRAPHICS.unlock_buffer(a_stBuffer); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mesh_c::post_load(void)
{
    return compute_bounds();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mesh_c::destroy(void)
{
    //EMP_FACTORY.deallocate(m_pIndices);
    //EMP_FACTORY.deallocate(m_pVertices);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mesh_c::compute_bounds(void)
{
    if (m_pIndices != nullptr)
    {
        EMP_RET_FALSE_IF_NOT(m_pIndices->compute_bounds());
        EMP_RET_FALSE_IF_NOT(m_pIndices->get_bounds(0, m_ui32Min, m_ui32Max, m_ui32Size, m_ui32Center));
    }
    if (m_pVertices != nullptr)
    {
        EMP_RET_FALSE_IF_NOT(m_pVertices->compute_bounds());
        EMP_RET_FALSE_IF_NOT(m_pVertices->get_bounds(0, m_v4Min, m_v4Max, m_v4Size, m_v4Center));
        EMP_RET_FALSE_IF(emp::tt::is_not_valid(m_v4Min) || emp::tt::is_not_valid(m_v4Max) || emp::tt::is_not_valid(m_v4Center));
        EMP_RET_FALSE_IF(emp::tt::is_max(m_v4Min) || emp::tt::is_min(m_v4Max));
    }

    return true;
}

/*
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mesh_c::translate(emp::mat::v3f::tparam a_v3Value) // @@0 use declaration
{
    EMP_ASSERT(m_ui32Stride != 0);
    EMP_ASSERT((m_vfVertices.count() % m_ui32Stride) == 0);
    EMP_ASSERT((m_vui32Indices.count() * m_ui32Stride) == m_vfVertices.count());

    c_size stCount = m_vui32Indices.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = m_vui32Indices.val(stIndex) * m_ui32Stride;
        c_v3f v3Offset =
        emp::mat::v3f::add(a_v3Value,
                           emp::mat::v3f::make(
                                               m_vfVertices.val(stOffset + 0),
                                               m_vfVertices.val(stOffset + 1),
                                               m_vfVertices.val(stOffset + 2)));
        m_vfVertices.set(stOffset + 0, emp::mat::v3f::get_x(v3Offset));
        m_vfVertices.set(stOffset + 1, emp::mat::v3f::get_y(v3Offset));
        m_vfVertices.set(stOffset + 2, emp::mat::v3f::get_z(v3Offset));
    }
    compute_bounds();
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool model_c::post_load(void)
{ return compute_bounds(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void model_c::destroy(void)
{
    /*c_size stCount = m_vpMeshes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_FACTORY.deallocate(m_vpMeshes.ref(stIndex)); }*/
    m_vpMeshes.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool model_c::compute_bounds(void)
{
    EMP_TT_MAX_VAR(m_v4Min);
    EMP_TT_MIN_VAR(m_v4Max);

    m_v4Center = v4f::all(0);
    m_v4Size = v4f::all(0);

    c_size stCount = m_vpMeshes.count();
    if (stCount > 0)
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            mesh_c const* const pMesh = m_vpMeshes.val(stIndex);
            if (pMesh->vertices()->count() > 0)
            {
                EMP_RET_FALSE_IF_NOT(pMesh->indices()->count() > 0);
                m_v4Max = v4f::max(m_v4Max, pMesh->max());
                m_v4Min = v4f::min(m_v4Min, pMesh->min());
            }
            else
            { EMP_RET_FALSE_IF(pMesh->indices()->count() > 0); }
        }

        m_v4Size = v4f::sub(m_v4Max, m_v4Min);
        m_v4Center = v4f::add(m_v4Min, v4f::mul(m_v4Size, v4f::all(0.5)));
    }
    else
    {
        m_v4Min = v4f::all(0);
        m_v4Max = v4f::all(0);
    }

    EMP_RET_FALSE_IF(emp::tt::is_not_valid(m_v4Min) || emp::tt::is_not_valid(m_v4Max) || emp::tt::is_not_valid(m_v4Center) || emp::tt::is_not_valid(m_v4Size));
    EMP_RET_FALSE_IF(emp::tt::is_max(m_v4Min) || emp::tt::is_min(m_v4Max));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

