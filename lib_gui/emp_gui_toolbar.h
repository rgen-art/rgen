//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_TOOLBAR_H
#define EMP_GUI_TOOLBAR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(class , icon_c)
EMP_FORWARD(struct, icon_skin_s)
EMP_FORWARD(class , registrar_i)
EMP_FORWARD(class , window_c)
EMP_FORWARD(struct, window_skin_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_STRUCT(toolbar_skin_s)
public:
    // @@0 spacer EMP_FIELD(c_container_skin_s* , spacer    , nullptr )
    EMP_FIELD(bool                , adjust    , false   )
    EMP_FIELD(bool                , spaced    , false   )
    EMP_FIELD(float               , min_width , 0.0     )
    EMP_FIELD(c_icon_skin_s*      , icon      , nullptr )
    EMP_FIELD(c_window_skin_s*    , window    , nullptr )
    EMP_FIELD(c_container_skin_s* , container , nullptr )
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_NOCOPY_BASE(class, toolbar_c, public, container_c)
public:
    typedef emp::fct::function_t<return_e (*)(c_bool, change_e)> on_push_d;
    EMP_FIELD(on_push_d, on_escape, {})

public:
    EMP_FIELD(bool, autosize_label_x, false)
    EMP_FIELD(dock_e, type, dock_e::Left);
    EMP_FIELD(registrar_i*, registrar, nullptr);
    EMP_FIELD(bool, push_on_focus, false);
    EMP_FIELD(bool, button_navigable, false);

private:
    vector<icon_c*> m_vpIcons{};
    vector<window_c*> windows{};
    vector<toolbar_c*> toolbars{};
    vector<state_t<bool, size_t>*> states{};
    c_toolbar_skin_s* skin = nullptr;
    bool changed = false;

public:
    toolbar_c(void) = default;
    virtual ~toolbar_c(void);

public:
    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    virtual EMP_RETURN bool transform(stack_c& a_rStack)
    {
        return tbase1::transform(a_rStack);
    }
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void add_item(
        emp::loc::string_s const& a_eLabel,
        emp::fct::function_t<return_e (*)(c_size, c_bool, c_change_e)> const& a_rFunction, toolbar_c* const a_pToolbar);

    void add_item(
        emp::loc::string_s const& a_eLabel,
        emp::fct::function_t<return_e (*)(c_size, c_bool, c_change_e)> const& a_rFunction, toolbar_c* const a_pToolbar,
        c_size a_stFocusIndex);

public:
    EMP_RETURN vector<icon_c*> const& icons(void) const { return m_vpIcons; }

public:
    EMP_RETURN bool set_selection(c_size a_stIndex, c_change_e a_eChange);

    toolbar_c& skin_(cpc_toolbar_skin_s a_pValue);

private:
    return_e on_item_pushed(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

