//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_DEBUG_H
#define ASE_MOD_DEBUG_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class mdi_c;
    class property_c;
    class splitter_c;
    EMP_FORWARD(enum, ui32, change_e)
} }

namespace emp { namespace rfx { class type_i; } }
namespace emp { namespace rfx { class type_i; } }
namespace emp { namespace rfx { template <typename> class type_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace gui { class viewer_i; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN emp::rfx::type_t<T>* new_reflection(emp::cnt::vector<emp::rfx::type_i*>& a_rvpReflections);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, debug_c, public, mode_a)
private:
    EMP_POINTER(emp::gui::splitter_c) m_pDebugSplitter;
    EMP_POINTER(emp::gui::mdi_c) m_pDebugPanel;
    EMP_POINTER(emp::gui::property_c) m_pDebugProperty;
    emp::cnt::vector<emp::rfx::type_i*> m_vpReflections;
    emp::cnt::vector<ase::gui::viewer_i*> m_vpViewers;

public:
    debug_c(void);
    ~debug_c(void);

public:
    //>>mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void);
    void public_stop(void);
    EMP_RETURN bool public_update(void);
    EMP_RETURN bool condition_satisfied(void) const;
    //<<mode_a

    //>>handler_i
    using tbase1::process_mouse;
    using tbase1::process_keyboard;
    //<<handler_i

private:
    EMP_RETURN bool vector_changed(void);
    EMP_RETURN bool object_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, emp::gui::c_change_e a_eChange);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

