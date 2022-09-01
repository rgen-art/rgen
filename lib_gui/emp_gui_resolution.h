//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_RESOLUTION_H
#define EMP_GUI_RESOLUTION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, resolution_c, public virtual, control_i)
public:
    EMP_FIELD(bool, center, false)
    EMP_FIELD(bool, scale , false)

private:
    control_i* control = nullptr;
    vector<control_i*> m_vpControls{};

    v2f size = { 0, 0 };
    v2f ratio = { 1, 1 };
    float m_fScale = 1;

public:
    emp::fct::function_t<void(*)(resolution_c const* const)> on_resolution_changed;

public:
    resolution_c(void);
    virtual ~resolution_c(void);

public:
    //>> control_i
    //>> control_i
    virtual EMP_RETURN state_data_s& state(void) { return control->state(); }
    virtual EMP_RETURN c_state_data_s& state(void) const { return control->state(); }

    virtual EMP_RETURN transformation_data_s& transformation(void) { return control->transformation(); }
    virtual EMP_RETURN c_transformation_data_s& transformation(void) const { return control->transformation(); }

    virtual EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
    { return control->got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange); }

    virtual void focused(void) { control->focused(); }
    virtual EMP_RETURN bool unselect(c_bool a_bTransmit) { return control->unselect(a_bTransmit); }

    virtual EMP_RETURN bool update(emp::tim::clock_c const& a_rClock) { return control->update(a_rClock); }

    virtual EMP_RETURN bool resize(void);

    virtual EMP_RETURN bool stack(stack_c& a_rStack) { return control->stack(a_rStack); }
    virtual EMP_RETURN bool transform(stack_c& a_rStack) { return control->transform(a_rStack); }
    virtual EMP_RETURN bool render(void) { return control->render(); }

    virtual void set_parent(pc_control_i a_pValue) { control->set_parent(a_pValue); }
    virtual EMP_RETURN c_control_i* parent(void) const { return control->parent(); }
    virtual EMP_RETURN control_i* parent(void) { return control->parent(); }

    virtual EMP_RETURN vector<control_i*>& controls(void) { return m_vpControls; }
    virtual EMP_RETURN vector<control_i*> const& controls(void) const { return m_vpControls; }
    //<< control_i

    //>> handler_i
    virtual EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent) { return control->process_keyboard(a_rEvent); }
    virtual EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent)
    {
        process_mouse_on_control(*this, a_rEvent);
        return control->process_mouse(a_rEvent);
    }
    //<< handler_i

public:
    void set_control(pc_control_i a_pValue)
    {
        control = a_pValue;
        m_vpControls.resize(0);
        m_vpControls.add(control);
    }

public:
    EMP_RETURN emp::mat::v2f::tret size_ratio(void) const;
    EMP_RETURN float scale_ratio(void) const;
};
EMP_FORWARD(class, resolution_c)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

