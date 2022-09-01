//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_ENTROPY_H
#define ASE_MOD_ENTROPY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui { class selection_c; } }
namespace ase { namespace cor { class selection_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, selection_c, public, mode_a)
private:
    EMP_POINTER(emp::gui::selection_c) m_pCtrlSelection;
    EMP_POINTER(ase::cor::selection_c) m_pSelection;

public:
    selection_c(void);
    ~selection_c(void);
    
public:
    using tbase1::post_load;
    
    //>>mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void);
    void public_stop(void) {}
    EMP_RETURN bool public_update(void) { return true; }
    EMP_RETURN bool condition_satisfied(void) const { return false; }
    //<<mode_a

    //>>handler_i
    using tbase1::process_mouse;
    using tbase1::process_keyboard;
    //<<handler_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

