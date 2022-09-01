//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_CONTAINER_H
#define EMP_GUI_CONTAINER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_control_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e process_controls_keyboard(emp::cnt::vector<control_i*> const& a_rvpControls, emp::app::c_event_s& a_rEvent);
EMP_RETURN return_e process_controls_mouse(emp::cnt::vector<control_i*> const& a_rvpControls, emp::app::c_event_s& a_rEvent);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void process_mouse_on_control(control_i& a_rControl, emp::app::c_event_s& a_rEvent);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void enable_controls(emp::cnt::vector<control_i*> const& a_rvpControls, c_bool a_bValue);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool get_min_max_index(T const& a_pvpChildren, ui32& a_rui32Min, ui32& a_rui32Max)
{
    ui32 EMP_TT_MAX_VAR(ui32Min);
    ui32 EMP_TT_MIN_VAR(ui32Max);

    c_size stCount = a_pvpChildren.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_control_i pControl = a_pvpChildren.val(stIndex);
        if (pControl->state().visible &&
            pControl->state().enable)
        {
            c_ui32 ui32FocusIndex = pControl->state().index;
            if (emp::tt::is_not_max(ui32FocusIndex))
            {
                ui32Min = emp::tt::minof(ui32Min, ui32FocusIndex);
                ui32Max = emp::tt::maxof(ui32Max, ui32FocusIndex);
            }
        }
    }

    a_rui32Min = ui32Min;
    a_rui32Max = ui32Max;

    return emp::tt::is_not_max(a_rui32Min);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN control_i* got_focus_controls(T const& a_rvpControls, c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush,
    ui32& a_rui32Index, c_change_e a_eChange)
{
    EMP_TT_MAX_VAR(a_rui32Index);
    ui32 stMin = 0;
    ui32 stMax = 0;
    if (a_eFocus == focus_e::First ||
        a_eFocus == focus_e::Last)
    {
        if (get_min_max_index(a_rvpControls, stMin, stMax) == false)
        { return nullptr; }
    }

    c_size stCount = a_rvpControls.count();
    if (a_bSelect)
    {
        for (ui32 stIndex = 0; stIndex < stCount; ++stIndex)
        {
            pc_control_i pControl = a_rvpControls.val(stIndex);
            EMP_ASSERT(pControl != nullptr);

            if (state_select(pControl))
            {
                pc_control_i pFocus = pControl->got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange);
                if (pFocus != nullptr)
                {
                    a_rui32Index = stIndex;
                    return pFocus;
                }
            }
        }
    }

    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        pc_control_i pControl = a_rvpControls.val(ui32Index);
        EMP_ASSERT(pControl != nullptr);

        control_i* pFocus = nullptr;
        if (a_eFocus == focus_e::First)
        {
            if (pControl->state().index == stMin)
            { pFocus = pControl->got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange); }
        }
        else if (a_eFocus == focus_e::Last)
        {
            if (pControl->state().index == stMax)
            { pFocus = pControl->got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange); }
        }
        else
        { EMP_ASSERT_UNREACHABLE(); }

        if (pFocus != nullptr)
        {
            a_rui32Index = ui32Index;
            return pFocus;
        }
    }

    if (a_bSelect)
    { return got_focus_controls(a_rvpControls, a_eFocus, false, a_bPush, a_rui32Index, a_eChange); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void add_control(vector<control_i*>& a_rvpControls, pc_control_i a_pValue, pc_control_i a_pParent);
void insert_control(vector<control_i*>& a_rvpControls, pc_control_i a_pValue, pc_control_i a_pParent, c_size a_stIndex);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool contains_control(vector<control_i*> const& a_rvpControls, cpc_control_i a_pValue);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool try_remove_control(vector<control_i*>& a_rvpControls, cpc_control_i a_pValue);
void remove_control(vector<control_i*>& a_rvpControls, cpc_control_i a_pValue);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(container_skin_s)
public:
    state_skin_t<c_control_skin_s> enabled_background{};
    state_skin_t<c_control_skin_s> disabled_background{};
    state_skin_t<c_control_skin_s> enabled_foreground{};
    state_skin_t<c_control_skin_s> disabled_foreground{};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, container_c, public virtual, control_i)
public:
    EMP_FIELD(bool                , child   , false   )
    EMP_FIELD(c_container_skin_s* , skin    , nullptr )

private:
    control_i* m_pParent = nullptr;

    state_data_s m_State;
    transformation_data_s m_Transform;

    color_data_s m_Color;
    emp::cnt::vector<control_i*> m_vpControls;

public:
    container_c(void);
    virtual ~container_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    //void destroy(void);

    void set_first(pc_control_i a_pValue);
    void set_last(pc_control_i a_pValue);

public:
    //>> control_i
    virtual EMP_RETURN state_data_s& state(void) { return m_State; }
    virtual EMP_RETURN c_state_data_s& state(void) const { return m_State; }

    virtual EMP_RETURN transformation_data_s& transformation(void) { return m_Transform; }
    virtual EMP_RETURN c_transformation_data_s& transformation(void) const { return m_Transform; }

    virtual EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);

    virtual void focused(void) {}

    virtual EMP_RETURN bool unselect(c_bool a_bTransmit);

    virtual EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    virtual EMP_RETURN bool resize(void);
    virtual EMP_RETURN bool stack(stack_c& a_rStack);
    virtual EMP_RETURN bool transform(stack_c& a_rStack);
    virtual EMP_RETURN bool render(void);

    virtual void set_parent(pc_control_i a_pValue) { m_pParent = a_pValue; }
    virtual EMP_RETURN c_control_i* parent(void) const { return m_pParent; }
    virtual EMP_RETURN control_i* parent(void) { return m_pParent; }

    virtual EMP_RETURN vector<control_i*>& controls(void) { return m_vpControls; }
    virtual EMP_RETURN vector<control_i*> const& controls(void) const { return m_vpControls; }
    //<< control_i

    //>> handler_i
    virtual EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    virtual EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    EMP_RETURN size_t control_count(void) const { return m_vpControls.count(); }

    void add_control(pc_control_i a_pValue) { add_control(a_pValue, this); }
    void add_control(pc_control_i a_pValue, pc_control_i a_pParent)
    { emp::gui::add_control(m_vpControls, a_pValue, a_pParent); }

    void insert_control(c_size a_stIndex, pc_control_i a_pValue) { insert_control(a_stIndex, a_pValue, this); }
    void insert_control(c_size a_stIndex, pc_control_i a_pValue, pc_control_i a_pParent)
    { emp::gui::insert_control(m_vpControls, a_pValue, a_pParent, a_stIndex); }

    void remove_control(cpc_control_i a_pValue) { emp::gui::remove_control(m_vpControls, a_pValue); }
    void remove_control(c_size a_stIndex) { m_vpControls.remove(a_stIndex); }

    EMP_RETURN bool try_remove_control(cpc_control_i a_pValue)
    { return emp::gui::try_remove_control(m_vpControls, a_pValue); }

    void clear_controls(void) { m_vpControls.resize(0); }

    void update_colors_current(c_bool a_bEnable);

    EMP_RETURN control_i* control(c_size a_stIndex) { return m_vpControls.val(a_stIndex); }
    EMP_RETURN c_control_i* control(c_size a_stIndex) const { return m_vpControls.val(a_stIndex); }

    EMP_RETURN color_data_s& color(void) { return m_Color; }
    EMP_RETURN c_color_data_s& color(void) const { return m_Color; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TYPEDEF(container_c)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

