//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GFX_MESH_H
#define EMP_GFX_MESH_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"

#include "emp_cnt_vector.h"


#include "emp_tt_logical.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx { struct type_s; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, mesh_c)
    buffer_i* m_pVertices;
    buffer_i* m_pIndices;

    primitive_e m_ePrimitive;

    v4f m_v4Min;
    v4f m_v4Max;
    v4f m_v4Size;
    v4f m_v4Center;

    ui32 m_ui32Min;
    ui32 m_ui32Max;
    ui32 m_ui32Size;
    ui32 m_ui32Center;

public:
    mesh_c(void):
        m_pVertices(nullptr),
        m_pIndices(nullptr),
        m_ePrimitive(primitive_e::none),
        m_v4Min(),
        m_v4Max(),
        m_v4Size(),
        m_v4Center(),
        m_ui32Min(),
        m_ui32Max(),
        m_ui32Size(),
        m_ui32Center()
    {}

    ~mesh_c(void)
    { destroy(); }

public:
    EMP_RETURN bool post_load(void);

public:
    EMP_RETURN bool create(c_primitive_e a_eType, buffer_i* const a_pVertices, buffer_i* const a_pIndices)
    {
        m_ePrimitive = a_eType;
        m_pVertices = a_pVertices;
        m_pIndices = a_pIndices;
        return m_pVertices != nullptr && m_ePrimitive != primitive_e::none;
    }

    void destroy(void);

public:
    EMP_RETURN bool compute_bounds(void);

public:
    EMP_RETURN c_v4f& min(void) const { return m_v4Min; }
    EMP_RETURN c_v4f& max(void) const { return m_v4Max; }
    EMP_RETURN c_v4f& size(void) const { return m_v4Size; }
    EMP_RETURN c_v4f& center(void) const { return m_v4Center; }

public:
    EMP_RETURN buffer_i* vertices(void) const { return m_pVertices; }
    EMP_RETURN buffer_i* indices(void) const { return m_pIndices; }
    
public:
    EMP_RETURN bool is_empty(void) const
    { return ((m_pVertices->count() > 0) && (m_pIndices->count() > 0)) == false; }

    EMP_RETURN bool is_not_empty(void) const
    { return is_empty() == false; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, model_c)
private:
    emp::cnt::vector<mesh_c*> m_vpMeshes;

    v4f m_v4Min;
    v4f m_v4Max;
    v4f m_v4Size;
    v4f m_v4Center;

public:
    model_c(void):
        m_vpMeshes(),
        EMP_TT_MAX_CTOR(m_v4Min),
        EMP_TT_MIN_CTOR(m_v4Max),
        m_v4Size(),
        m_v4Center()
    {}

    ~model_c(void)
    { destroy(); }

public:
    EMP_RETURN bool post_load(void);

public:
    void destroy(void);
    EMP_RETURN bool compute_bounds(void);

public:
    EMP_RETURN c_v4f& min(void) const { return m_v4Min; }
    EMP_RETURN c_v4f& max(void) const { return m_v4Max; }
    EMP_RETURN c_v4f& size(void) const { return m_v4Size; }
    EMP_RETURN c_v4f& center(void) const { return m_v4Center; }

    EMP_RETURN emp::cnt::vector<mesh_c*> const& meshes(void) const { return m_vpMeshes; }

    void add_mesh(mesh_c* const a_pMesh) { m_vpMeshes.add(a_pMesh); }

private:
    void translate(emp::mat::v3f::tparam a_v3Value);
    void align_left(void) { EMP_ASSERT_NOT_IMPLEMENTED(); }

    //EMP_RETURN emp::cnt::vector<mesh_c*>& meshes(void) { return m_vpMeshes; }
    //EMP_RETURN size_t vertex_count(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

