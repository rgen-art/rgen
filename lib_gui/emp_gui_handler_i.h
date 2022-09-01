//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_HANDLER_I_H
#define EMP_GUI_HANDLER_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app { EMP_FORWARD(struct, event_s) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name return_e
#define enum_values\
    enum_value( Unused   )\
    enum_value( Consumed )\
    enum_value( Error    )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, handler_i)
public:
    handler_i(void) {}
    virtual ~handler_i(void) {}

    virtual EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent) = 0;
    virtual EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent) = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
