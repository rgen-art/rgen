//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_SLIDER_H
#define EMP_GUI_SLIDER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_label.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { template <typename...> class buffer_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(slider_skin_s)
public:
    EMP_FIELD(c_label_skin_s*   , label , nullptr)
    EMP_FIELD(c_color_s*        , value , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,slider_c, public, label_c)
public:
    EMP_FIELD(float, min , 0.0f);
    EMP_FIELD(float, max , 0.0f);
    EMP_FIELD(float, step, 0.0f);

private:
    c_slider_skin_s* skin = nullptr;
    float m_fSlide = 0;
    float delta = 0;
    bool dragged = false;
    int axis = 0;

    EMP_POINTER(emp::gfx::buffer_t<v3f>) vertices;

public:
    slider_c(void);
    virtual ~slider_c(void);

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void slide(c_float a_fValue) { m_fSlide = slide(0, a_fValue); }

private:
    void accumulate(c_v2f& a_v2Value);
    EMP_RETURN float slide(c_float a_fSlide, c_float a_fValue);

public:
    slider_c& skin_(cpc_slider_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

