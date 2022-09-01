//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_MODE_I_H
#define ASE_MOD_MODE_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_handler_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class control_i;
    class registrar_i;
    class window_c;
} }

namespace emp { namespace loc { struct string_s; } }

namespace emp { namespace tim {
    class manager_c;
    struct time_c;
} }

namespace ase { namespace cor { class game_c; } }
namespace ase { namespace gui { class skin_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,mode_i, public, emp::gui::handler_i)
private:
public:
    mode_i(void) {}
    virtual ~mode_i(void);

public:
    //>>mode_i
    virtual EMP_RETURN bool create(void) = 0;
    virtual void destroy(void) = 0;
    virtual EMP_RETURN bool start(emp::tim::clock_c const& a_rClock) = 0;
    virtual void stop(void) = 0;
    virtual EMP_RETURN bool update(emp::tim::clock_c const& a_rClock) = 0;
    virtual EMP_RETURN emp::gui::registrar_i const& registrar(void) const = 0;
    virtual EMP_RETURN emp::gui::registrar_i& registrar(void) = 0;
    virtual EMP_RETURN emp::loc::string_s const& name(void) const = 0;
    virtual EMP_RETURN bool set_name(emp::loc::string_s const& a_eValue) = 0;
    virtual EMP_RETURN mode_i* next_mod(void) const = 0;
    virtual EMP_RETURN mode_i* prev_mod(void) const = 0;
    virtual EMP_RETURN emp::tim::time_c const& time(void) const = 0;
    virtual EMP_RETURN bool fire_next(void) = 0;
    virtual EMP_RETURN bool fire_prev(void) = 0;
    //<<mode_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline mode_i::~mode_i(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

