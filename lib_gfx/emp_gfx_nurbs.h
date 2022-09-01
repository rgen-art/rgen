//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GFX_GL_NURBS_H
#define EMP_GFX_GL_NURBS_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_matrix.h"
#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { class nurbs_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, gl_nurbs_c)
private:
    size_t m_stIteration;
    EMP_POINTER(emp::mat::nurbs_c) m_pNurbs;
    bool m_bChanged;

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pLineVertices;
    buffer_t<ui8>* m_pLineIndices;

    v4f::type m_v4InColor;
    v4f::type m_v4OutColor;

public:
    gl_nurbs_c(void);
    ~gl_nurbs_c(void);

public:
    void create(void);
    void destroy(void);

public:
    void update(void);
    void render(c_m44f& a_m44Transform);

public:
    EMP_RETURN size_t point_count(void) const;

    void add_point(c_v3f& a_v3Point);
    void set_point(c_size a_stIndex, c_v3f& a_v3Point);

    void clear_points(void);

public:
    void set_iteration(c_size a_stValue) { m_stIteration = a_stValue; }

public:
    void set_in_color(c_v4f& a_v4Value);
    void set_out_color(c_v4f& a_v4Value);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

