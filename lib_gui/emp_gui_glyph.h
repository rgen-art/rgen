//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_GLYPH_H
#define EMP_GUI_GLYPH_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_coordinate.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { class icon_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(glyph_skin_s)
public:
    EMP_FIELD( c_coordinate_skin_s*, coordinate, nullptr)
    EMP_FIELD( c_color_s*          , glyph     , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 rename to icon_c
EMP_NOCOPY_BASE(class, glyph_c, public, coordinate_c)
public:
    EMP_FIELD(emp::gfx::icon_c*, gfx_icon, nullptr)

private:
    c_glyph_skin_s* skin = nullptr;
    
public:
    glyph_c(void);
    virtual ~glyph_c(void);

public:
    //> control_i
    EMP_RETURN bool transform(stack_c& a_rStack);
    //< control_i

public:
    EMP_RETURN bool auto_resize(void);

private:
    void render_glyph(frame_c const& a_rSender);

public:
    glyph_c& skin_(cpc_glyph_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

