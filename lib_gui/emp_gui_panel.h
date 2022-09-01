//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_PANEL_H
#define EMP_GUI_PANEL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"
#include "emp_gui_scroller.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_visible(cpc_control_i a_pControl, c_float a_fOffsetX, c_float a_fOffsetY, c_float a_fUserWidth, c_float a_fUserHeight);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(panel_skin_s)
public:
    EMP_FIELD( c_container_skin_s* , user     , nullptr );
    //EMP_FIELD( c_spacing_data_s*   , spacing  , nullptr );
    EMP_FIELD( c_scroller_skin_s*  , scroller , nullptr );
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, panel_c, public, container_c)
public:
    container_c scroller{};
    container_c user{};
    scroller_c vsb{};

private:
    scroller_c hsb{};
    float offset_x = 0;
    float offset_y = 0;
    c_panel_skin_s* skin = nullptr;
    
public:
    panel_c(void);
    virtual ~panel_c(void);

public:
    //>> control_i
    EMP_RETURN bool unselect(c_bool a_bTransmit) { emp::tt::silent(tbase1::unselect(a_bTransmit)); return true; }
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void scroll_up(void);
    void scroll_down(void);

    void update_scroll(void);
    EMP_RETURN bool update_scroll(c_float a_fMinX, c_float a_fMinY);

    void ensure_visible(cpc_control_i a_pControl);
    EMP_RETURN bool refresh_scrolling(c_float a_fX, c_float a_fY);

    void get_min_max(float& a_rfMinX, float& a_rfMinY, float& a_rfMaxX, float& a_rfMaxY);

    EMP_RETURN float delta_user_w(void);
    EMP_RETURN float delta_docker_w(void);

public:
    panel_c& skin_(cpc_panel_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

