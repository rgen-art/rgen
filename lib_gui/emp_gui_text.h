//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_TEXT_H
#define EMP_GUI_TEXT_H

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
EMP_STRUCT(text_skin_s)
public:
    EMP_FIELD( c_label_skin_s*  , label  , nullptr)
    EMP_FIELD( c_color_s*       , carret , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, text_c, public, label_c)
public:
    size_t m_stCarret = 0;

    state_t<bool, text_c const*> on_edit{};
    state_t<bool, text_c const*> on_valid{};
    state_t<bool, text_c const*> on_cancel{};

private:
    u8string m_su8Text{};
    u8string m_su8Edition{};

    emp::rfx::type_i* reflection = nullptr;

    float m_fCarret = 0;
    v4f color = {0, 0, 0, 1};
    c_text_skin_s* skin = nullptr;

    EMP_POINTER(emp::gfx::buffer_t<v3f>) vertices;

public:
    text_c(void);
    virtual ~text_c(void);

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    //<< handler_i

    void move_left(c_size a_stCount);
    void move_right(c_size a_stCount);
    void move_up(c_size a_stCount);
    void move_down(c_size a_stCount);

public:
    void set_text(c_u8string& a_sValue);
    EMP_RETURN c_u8string& text(void) const { return m_su8Text; }

public:
    void set_reflection(emp::rfx::type_i* const a_pValue);

public:
    void set_on_edit(c_bool a_bValue, c_change_e a_eChange);
    void set_on_valid(c_bool a_bValue, c_change_e a_eChange);
    void set_on_cancel(c_bool a_bValue, c_change_e a_eChange);

public:
    text_c& skin_(cpc_text_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

