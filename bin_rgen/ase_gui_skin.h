//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_GUI_SKIN_H
#define ASE_GUI_SKIN_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_skin.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class skin_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(keyboard_skin_s)
public:
    EMP_FIELD( emp::gui::c_label_skin_s*     , label     , nullptr )
    EMP_FIELD( emp::gui::c_container_skin_s* , spacing   , nullptr )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(score_skin_s)
public:
    EMP_FIELD(c_container_skin_s*   , container    , nullptr)
    EMP_FIELD(c_color_s*            , beat         , nullptr)
    EMP_FIELD(c_color_s*            , mesure       , nullptr)
    EMP_FIELD(c_color_s*            , time         , nullptr)
    EMP_FIELD(c_color_s*            , border       , nullptr)
    EMP_FIELD(c_color_s*            , success      , nullptr)
    EMP_FIELD(c_color_s*            , failure      , nullptr)
    EMP_FIELD(c_label_skin_s*       , label        , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(selection_skin_s)
public:
    EMP_FIELD(c_panel_skin_s*       , panel    , nullptr)
    EMP_FIELD(c_sizer_skin_s*       , sizer    , nullptr)
    EMP_FIELD(c_splitter_skin_s*    , splitter , nullptr)
    EMP_FIELD(c_list_skin_c*        , list     , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(compose_skin_s)
public:
    EMP_FIELD(c_label_skin_s*          , title                  , nullptr)
    EMP_FIELD(c_sizer_skin_s*          , header                 , nullptr)
    EMP_FIELD(c_sizer_skin_s*          , docker                 , nullptr)
    EMP_FIELD(c_score_skin_s*          , score                  , nullptr)
    // @@0 spacer EMP_FIELD(c_container_skin_s*      , spacer                 , nullptr)
    EMP_FIELD(c_picture_skin_s*        , picture                , nullptr)
    EMP_FIELD(c_picture_skin_s*        , picture_small          , nullptr)
    EMP_FIELD(emp::gfx::model_c const* , gfx_up                 , nullptr)
    EMP_FIELD(emp::gfx::model_c const* , gfx_down               , nullptr)
    EMP_FIELD(emp::gfx::model_c const* , gfx_remove             , nullptr)
    EMP_FIELD(c_list_skin_c*           , property               , nullptr)
    EMP_FIELD(c_splitter_skin_s*       , splitter               , nullptr)
    EMP_FIELD(c_toggle_skin_s*         , enabled                , nullptr)
    EMP_FIELD(c_toggle_skin_s*         , expanded               , nullptr)
    EMP_FIELD(c_toolbar_skin_s*        , toolbar                , nullptr)
    EMP_FIELD(c_toolbar_skin_s*        , popup                  , nullptr)
    EMP_FIELD(c_list_skin_c*           , list                   , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, skin_c)
public:
    emp::gui::skin_t<emp::gui::score_skin_s> score;
    emp::gui::skin_t<emp::gui::keyboard_skin_s> keyboard;
    emp::gui::skin_t<emp::gui::compose_skin_s> compose;
    emp::gui::skin_t<emp::gui::selection_skin_s> selection;

public:
    skin_c(void);
    ~skin_c(void);

public:
    void create(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

