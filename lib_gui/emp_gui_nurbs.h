//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_NURBS_H
#define EMP_GUI_NURBS_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_plot.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { EMP_FORWARD(class, gl_nurbs_c) } }

namespace emp { namespace mat { EMP_FORWARD(class, nurbs_c) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(nurbs_skin_s)
public:
    EMP_FIELD(plot_skin_s* , plot , nullptr)
    EMP_FIELD(c_color_s*   , line , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, nurbs_c, public, plot_c)
public:
    EMP_FIELD(emp::mat::nurbs_c*, nurbs, nullptr);

private:
    EMP_POINTER(emp::gfx::gl_nurbs_c) gfx_nurbs;
    c_nurbs_skin_s* skin = nullptr;

public:
    nurbs_c(void);
    virtual ~nurbs_c(void);

public:
    //> control_i
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    //< control_i

public:
    void add_point(c_size a_stIndex, c_v2f& a_v2Position);

private:
    void render_nurbs(plot_c const& a_rSender);

    EMP_RETURN size_t move_point(c_size a_stIndex, c_v2f& a_v2Delta);
    void remove_point(c_size a_stIndex);

public:
    nurbs_c& skin_(cpc_nurbs_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

