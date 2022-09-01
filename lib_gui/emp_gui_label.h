//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_LABEL_H
#define EMP_GUI_LABEL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { EMP_FORWARD(class, text_c) } }
namespace emp { namespace rfx { EMP_FORWARD(class, type_i) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(struct, button_skin_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(label_skin_s)
public:
    EMP_FIELD(c_font_data_s*       , font       , nullptr)
    EMP_FIELD(c_button_skin_s*     , button     , nullptr)
    state_skin_t<c_color_s> enabled{};
    state_skin_t<c_color_s> disabled{};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, label_c, public, container_c)
public:
    state_t<bool, c_control_i*> on_push{};

public:
    EMP_FIELD(bool                  , push_on_focus         , false     )
    EMP_FIELD(bool                  , push_on_repeat        , false     )
    EMP_FIELD(bool                  , push_on_double_click  , false     )
    EMP_FIELD(bool                  , render_text           , true      )
    EMP_FIELD(float                 , scale_x               , 1.0f      )
    EMP_FIELD(float                 , scale_y               , 1.0f      )
    EMP_FIELD(float                 , alpha                 , 1.0f      )
    EMP_FIELD(emp::str::formater_i* , formater              , nullptr   )
    EMP_FIELD(alignment_e           , alignment             , alignment_e::MiddleCenter);
    EMP_FIELD(bool                  , scissor               , true      )
    u8string m_su8Text{};

private:
    m44f m_mModel;
    skin_data_t<label_skin_s> skin{};
    EMP_POINTER(emp::gfx::text_c) m_pText;
    emp::loc::string_s* m_peString;
    u8string m_sText;
    emp::rfx::type_i* m_pReflection;
    bool m_bRenderText;
    color_s m_Color;

public:
    label_c(void);
    virtual ~label_c(void);

    EMP_RETURN bool post_load(void);

    //void create(void);
    //void destroy(void);

public:
    //>> control_i
    EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s&);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s&);
    //<< handler_i

public:
    label_c& scale_(c_float a_fValue) { return scale_(a_fValue, a_fValue); }
    label_c& scale_(c_float a_fX, c_float a_fY) { return scale_x_(a_fX).scale_y_(a_fY); }

public:
    label_c& skin_(cpc_label_skin_s a_pValue);

    void set_text(c_u8string& a_sValue);
    void set_text(emp::loc::string_s const& a_eValue);
    void set_text(emp::rfx::type_i* const a_pValue);

    EMP_RETURN emp::cnt::vector<v2f> const& positions(void) const;
    EMP_RETURN c_m44f& text_matrix(void) const { return m_mModel; }

    EMP_RETURN emp::rfx::type_i* reflection(void) const { return m_pReflection; }
    EMP_RETURN emp::mat::v2f::tret compute_font_size(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

