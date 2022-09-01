//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_spacer.h"

#include "emp_gui_container.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool spacer_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_float fX = EMP_GUI_SKIN.spacing_x();//skin != nullptr ? skin->spacing->x() : 0;
    c_float fY = EMP_GUI_SKIN.spacing_y();//skin != nullptr ? skin->spacing->y() : 0;

    c_dock_e eDock = state().dock;

    if (state().autosize.y)
    { transformation().height_(0); }
    if (state().autosize.x)
    { transformation().width_(0); }

    if (eDock == dock_e::Top ||
        eDock == dock_e::Bottom)
    { transformation().height_(fY); }
    else if (
        eDock == dock_e::Left ||
        eDock == dock_e::Right)
    { transformation().width_(fX); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*spacer_c& spacer_c::skin_(cpc_container_skin_s a_pValue)
{
    tbase1::skin_(a_pValue);
    return *this;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

