//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_WINDOW_I_H
#define EMP_GUI_WINDOW_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_control_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_LAYOUT_CHANGE_VD2)

EMP_NOCOPY_BASE(class, window_i, public virtual, control_i)
private:
    friend class manager_c;

public:
    window_i(void) {}
    virtual ~window_i(void) = 0;

public:
    EMP_RETURN bool is_visible(void) const
    { return state().visible || is_showing() || is_hiding(); }

    EMP_RETURN bool is_hiding_or_hidden(void) const
    { return is_hiding() || is_hidden(); }

public:
    //>> window_i
    virtual EMP_RETURN bool is_showing(void) const = 0;
    virtual EMP_RETURN bool is_hiding(void) const = 0;
    virtual EMP_RETURN bool is_hidden(void) const = 0;
    virtual EMP_RETURN bool is_modal(void) const = 0;
    virtual EMP_RETURN bool is_top(void) const = 0;
    virtual EMP_RETURN bool is_floating(void) const = 0;
    virtual EMP_RETURN bool is_overlay(void) const = 0;
    virtual EMP_RETURN float fade_factor(void) const = 0;
    virtual EMP_RETURN float fade_speed(void) const = 0;
    //<< window_i

private:
    //>> window_i
    virtual void show_imp(void) = 0;
    virtual void hide_imp(void) = 0;
    //<< window_i
};
// EMP_PRAGMA_POP_IGNORE(EMP_W_LAYOUT_CHANGE_VD2)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline window_i::~window_i(void)
{
    int kk = 0; // @@0 remove
    kk++;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

