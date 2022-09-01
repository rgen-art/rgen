//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_SPLITTER_H
#define EMP_GUI_SPLITTER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"
#include "emp_gui_spacer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(splitter_skin_s)
public:
    EMP_FIELD(c_container_skin_s*   , container , nullptr)
    EMP_FIELD(c_container_skin_s*   , spacer    , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, splitter_c, public, container_c)
public:
    EMP_FIELD(control_i* , master        , nullptr       )
    EMP_FIELD(control_i* , slave         , nullptr       )
    EMP_FIELD(dock_e     , orientation   , dock_e::Left  )
    EMP_FIELD(bool       , hide_master   , false         )

private:
    spacer_c spacer;
    c_splitter_skin_s* skin = nullptr;

public:
    splitter_c(void);
    virtual ~splitter_c(void) {}

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    splitter_c& skin_(cpc_splitter_skin_s a_pValue);

public:
    void redock(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

