//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_OUTLINE_H
#define EMP_GUI_OUTLINE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_plot.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { struct outline_s; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(outline_skin_s)
public:
    EMP_FIELD(c_color_s*     , line    , nullptr)
    EMP_FIELD(c_color_s*     , control , nullptr)
    EMP_FIELD(c_plot_skin_s* , plot    , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, outline_c, public, plot_c)
public:
    EMP_FIELD(emp::mat::outline_s*, outline, nullptr);

private:
    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pLineMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pLineMeshIndices;

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pQuadMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pQuadMeshIndices;

    c_outline_skin_s* skin = nullptr;

public:
    outline_c(void);
    virtual ~outline_c(void);

public:
    //> control_i
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    //< control_i

private:
    void render_outline(plot_c const& a_rSender);

private:
    EMP_RETURN size_t move_point(c_size a_stIndex, c_v2f& a_v2Delta);
    void add_point(c_size a_stIndex, c_v2f& a_v2Position);
    void remove_point(c_size a_stIndex);
    void right_click_point(c_size a_stIndex);

public:
    outline_c& skin_(cpc_outline_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

