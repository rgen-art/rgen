//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_SYNTHESIZE_H
#define ASE_MOD_SYNTHESIZE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui { class synthesize_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,synthesize_c, public, mode_a)
private:
    EMP_POINTER(emp::gui::synthesize_c) m_pCtrlSynthesize;

public:
    synthesize_c(void);
    ~synthesize_c(void);

public:
    //>>mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void) { return true; }
    void public_stop(void) {}
    EMP_RETURN bool public_update(void);
    EMP_RETURN bool condition_satisfied(void) const { return false; }
    //<<mode_a

    //>>handler_i
    //<<handler_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
