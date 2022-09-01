//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_TEXT_H
#define ASE_MOD_TEXT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class button_c;
    EMP_FORWARD(class, control_i)
    class label_c;
    EMP_FORWARD(enum, ui32, change_e)
} }

namespace emp { namespace loc { struct string_s; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, text_c, public, mode_a)
private:
    emp::gui::button_c* m_pButton;
    emp::gui::label_c* m_pLabel;

public:
    text_c(void);
    ~text_c(void);

public:
    EMP_RETURN bool post_load(void);

    //>>mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void);
    void public_stop(void) {}
    EMP_RETURN bool public_update(void) { return true; }
    EMP_RETURN bool condition_satisfied(void) const { return false; }
    EMP_RETURN bool set_name(emp::loc::string_s const& a_eValue);
    //<<mode_a

    EMP_RETURN emp::gui::return_e button_pushed(emp::gui::cpc_control_i a_pSender, c_bool a_bValue, emp::gui::c_change_e a_eChange);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

