//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_RGEN_H
#define ASE_MOD_RGEN_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class renderer_c;
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_NOCOPY_BASE(class, rgen_c, public, mode_a)
private:
    EMP_POINTER(emp::gui::renderer_c) renderer;

public:
    bool m_bMutate = true;
    bool m_bCross = true;
    bool m_bSwitchMother = true;
    bool m_bSwitchFather = true;
    bool m_bReplace = false;

public:
    rgen_c(void);
    ~rgen_c(void);

public:
    //>>mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void);
    void public_stop(void);
    EMP_RETURN bool public_update(void);
    EMP_RETURN bool condition_satisfied(void) const { return false; }
    //<<mode_a

    void set_file(c_astring& a_sFile);
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

