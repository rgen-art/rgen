//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_REGISTRAR_I_H
#define EMP_GUI_REGISTRAR_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class window_i;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name state_e
#define enum_values\
    enum_value( none     )\
    enum_value( Started  )\
    enum_value( Stopping )\
    enum_value( Stopped  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, registrar_i)
public:
    registrar_i(void) {}
    virtual ~registrar_i(void) = 0;

public:
    //>> registrar_i
    virtual void register_window(window_i* const a_pWindow) = 0;
    virtual void remove_window(window_i* const a_pWindow) = 0;
    virtual EMP_RETURN state_e state(void) const = 0;
    virtual void start(void) = 0;
    virtual void stop(void) = 0;
    virtual void update(void) = 0;
    virtual EMP_RETURN bool is_modal_visible(void) const = 0;
    //<< registrar_i
};
inline registrar_i::~registrar_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

