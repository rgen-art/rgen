//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_MANAGER_H
#define EMP_GUI_MANAGER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h" // @@0 include

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(class, manager_c)
EMP_FORWARD(class, window_c)
EMP_FORWARD(class, window_i)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* get_focusable(cpc_manager_c a_pManager, cpc_control_i a_pParent,
    c_ui32 a_stCurrent, c_bool a_bReverse, c_bool a_bDeep, ui32& a_rstOut);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, manager_c, public, container_c)
private:
    vector<window_i*> m_vpStack{};
    vector<window_i*> m_vpRenderStack{};
    map_t<window_i*, control_i*> m_mpFocuses;
    map_t<c_transformation_data_s*, float> hover_candidates;
    map_t<c_transformation_data_s*, bool> hover_quads;
    v2f mouse_position = {0, 0};

    EMP_POINTER(emp::mat::random_c) m_pRandom{};

    emp::cnt::vector<control_i*> m_vpNavigables{};
    //EMP_POINTER(window_c) m_pModal;
    size_t m_stTransition = 0;
    vector<v4f> m_vv4Scissors{};

    bool scaled = false;

public:
    emp::fct::function_t<void(*)(c_bool a_bUp)> on_scale{};

public:
    bool m_bDebug = true;
    v2f cursor_size{};

private:
    control_i* m_pFocus = nullptr;
    control_i* m_pModalFocus = nullptr;

public:
    manager_c(void);
    virtual ~manager_c(void);

    EMP_RETURN bool post_load(void);
    void destroy(void);

    void clean(void);

    void activate_focus(pc_control_i a_pControl, c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);

    void clear_focus(cpc_control_i a_pControl);
    void deactivate_focus(void);
    EMP_RETURN c_control_i* focus(void) const { return m_pFocus; }

    EMP_RETURN control_i* focus(window_i* const a_pValue);
    void set_focus(pc_window_i a_pWindow, pc_control_i a_pValue);

    EMP_RETURN bool try_remove_window(pc_window_i a_pWindow);
    EMP_RETURN bool show_window(pc_window_i a_pWindow, c_bool a_bTop, c_bool a_bSelect, c_change_e a_eChange, c_bool a_bClear);

    EMP_RETURN bool hide_window(pc_window_i a_pWindow);

    void next_transition(void);

    EMP_RETURN bool is_hover_candidate(cpc_control_i a_pControl)
    { return hover_candidates.get(&a_pControl->transformation()).not_end(); }

private:
    template <typename T>
    void find_hover_candidates(emp::app::c_event_s& a_rEvent, T const& a_rvpControls, c_bool a_bContinue);

public:
    EMP_RETURN bool modal(void) const;
    EMP_RETURN bool is_modal(cpc_control_i a_pWindow) const;

    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void push_scissor(c_bool a_bClear, c_transformation_data_s& a_rTransform, bool& a_rbEmpty, c_float a_fLeft, c_float a_fRight, c_float a_fBottom, c_float a_fTop);
    void pop_scissor(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN manager_c* manager(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

#define EMP_GUI_MANAGER emp::gui::manager()
#define EMP_GUI_MANAGER_CLIP(x_Name, x_bClear, x_Transform, x_Empty, x_Left, x_Right, x_Bottom, x_Top)\
    EMP_GUI_MANAGER->push_scissor(x_bClear, x_Transform, x_Empty, x_Left, x_Right, x_Bottom, x_Top);\
    auto const x_Name = emp::tt::finally([](){ EMP_GUI_MANAGER->pop_scissor(); });

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

