//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_COORDINATE_H
#define EMP_GUI_COORDINATE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_frame.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(coordinate_skin_s)
public:
    EMP_FIELD(c_container_skin_s*   , container , nullptr)
    EMP_FIELD(c_frame_skin_s*       , frame     , nullptr)
    EMP_FIELD(c_label_skin_s*       , decade    , nullptr)
    EMP_FIELD(c_label_skin_s*       , unit      , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, coordinate_c, public, container_c)
public:
    frame_c frame;
    EMP_FIELD(bool, show_coordinates, false)

private:
    container_c label_x;
    container_c label_y;

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pLineMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pLineMeshIndices;

    vector<label_c*> m_vpUnitLabelX;
    vector<label_c*> m_vpUnitLabelY;

    vector<float> m_vfLabelX;
    vector<float> m_vfLabelY;

    float m_fMaxX = 0;
    float m_fMaxY = 0;

    c_coordinate_skin_s* skin = nullptr;

public:
    coordinate_c(void);
    virtual ~coordinate_c(void);

    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    EMP_RETURN bool auto_resize(emp::mat::v2f::tparam a_v2Value);

public:
    coordinate_c& skin_(cpc_coordinate_skin_s a_pValue);

private:
    EMP_RETURN bool resize_labels(c_bool a_bResize, emp::mat::v2f& a_rv2Out);
    EMP_RETURN float FEX(void) const;
    EMP_RETURN float FEY(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

