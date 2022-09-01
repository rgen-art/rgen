//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_icon.h"

#include "emp_gui_button.h"
#include "emp_gui_label.h"
#include "emp_gui_picture.h"
#include "emp_gui_splitter.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
icon_c::icon_c(void):
    picture(),
    label(),
    splitter()
{
    picture.state().focusable_(true);

   // @@0 create  label.create();

    splitter.master_(&picture).slave_(&label).state().index_(0);
    splitter.redock();

    control_(&splitter);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
icon_c::~icon_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
icon_c& icon_c::picture_(emp::gfx::model_c const* const a_pValue)
{
    splitter.hide_master_(a_pValue == nullptr);
    picture.model_(a_pValue);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool icon_c::resize(void)
{
    if (state().visible)
    {
        splitter.hide_master_(picture.model == nullptr);
        picture.state().autosize.x_(true);

        if (autosize_label_x)
        {
            splitter.state().autosize = state().autosize;
            label.state().autosize.x_(state().autosize.x);
        }

        EMP_RET_FALSE_IF_NOT(tbase1::resize());
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
icon_c& icon_c::skin_(icon_skin_s const* const a_pValue)
{
    skin = a_pValue;
    splitter.skin_(a_pValue ? a_pValue->splitter : nullptr);
    tbase1::skin_(a_pValue ? a_pValue->button : nullptr);
    picture.skin_(a_pValue ? a_pValue->picture : nullptr);
    label.skin_(a_pValue ? a_pValue->label : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

