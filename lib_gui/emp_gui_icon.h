//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_ICON_H
#define EMP_GUI_ICON_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_button.h"
#include "emp_gui_label.h"
#include "emp_gui_picture.h"
#include "emp_gui_splitter.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(icon_skin_s)
public:
    EMP_FIELD( c_splitter_skin_s*  , splitter , nullptr )
    EMP_FIELD( c_picture_skin_s*   , picture  , nullptr )
    EMP_FIELD( c_label_skin_s*     , label    , nullptr )
    EMP_FIELD( c_button_skin_s*    , button   , nullptr )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, icon_c, public, button_c)
public:
    EMP_FIELD(bool, autosize_label_x, true)

    picture_c picture;
    label_c label;
    splitter_c splitter;

private:
    c_icon_skin_s* skin = nullptr;

public:
    icon_c(void);
    virtual ~icon_c(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    icon_c& picture_(emp::gfx::cpc_model_c a_pValue);
    icon_c& skin_(cpc_icon_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

