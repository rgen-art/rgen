//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_gui_skin.h"

#include "ase_gui_score.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
skin_c::skin_c(void) :
    score(ALITERAL("emp::gui::score_skin_s")),
    keyboard(ALITERAL("emp::gui::keyboard_skin_s")),
    compose(ALITERAL("emp::gui::compose_skin_s")),
    selection(ALITERAL("emp::gui::selection_skin_s"))
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
skin_c::~skin_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void skin_c::create(void)
{
    //emp::gui::skin_c& rSkin = EMP_GUI_SKIN;

    // selection
    emp::gui::selection_skin_s* const pSelection = selection.allocate(ALITERAL("ent."));
    pSelection->
        sizer_   ( EMP_GUI_SKIN_(sizer   , ""     )).
        splitter_( EMP_GUI_SKIN_(splitter, ""     )).
        list_    ( EMP_GUI_SKIN_(list    , "lst." )).
        panel_   ( EMP_GUI_SKIN_(panel   , "pnl." ));

    //score
    emp::gui::score_skin_s* const pScore = score.allocate(ALITERAL("scr."));
    pScore->
        container_( EMP_GUI_SKIN_(container, "cnt.window"  )).
        beat_     ( EMP_GUI_SKIN_(color    , "col.gray20"  )).
        mesure_   ( EMP_GUI_SKIN_(color    , "col.gray50"  )).
        time_     ( EMP_GUI_SKIN_(color    , "col.link"    )).
        border_   ( EMP_GUI_SKIN_(color    , "col.link"    )).
        success_  ( EMP_GUI_SKIN_(color    , "col.link_a50")).
        failure_  ( EMP_GUI_SKIN_(color    , "col.red_a50" )).
        label_    ( EMP_GUI_SKIN_(label    , "lbl.h4"      ));

    //keyboard
    emp::gui::keyboard_skin_s* const pKeyboard = keyboard.allocate(ALITERAL("kbd."));
    pKeyboard->
        label_(  EMP_GUI_SKIN_(label, "lbl.evolution"));

    //compose
    emp::gui::compose_skin_s* const pCompose = compose.allocate(ALITERAL("cmp."));
    pCompose->
        header_         ( EMP_GUI_SKIN_(sizer     , "szr.column"            )).
        docker_         ( EMP_GUI_SKIN_(sizer     , "szr.window"            )).
        picture_        ( EMP_GUI_SKIN_(picture   , "pic."         )).
        picture_small_  ( EMP_GUI_SKIN_(picture   , "pic."         )).
        property_       ( EMP_GUI_SKIN_(list      , "lst."                  )).
        //splitter_       ( EMP_GUI_SKIN_(splitter  , ""                      )).
        title_          ( EMP_GUI_SKIN_(label     , "lbl.text_padded"       )).
        toolbar_        ( EMP_GUI_SKIN_(toolbar   , "tbr."                  )).
        popup_          ( EMP_GUI_SKIN_(toolbar   , "pop.todo.index.menu"   )).
        list_           ( EMP_GUI_SKIN_(list      , "lst."                  )).
        enabled_        ( EMP_GUI_SKIN_(toggle    , "tgl.enable"            )).
        expanded_       ( EMP_GUI_SKIN_(toggle    , "tgl.expand"            )).
        gfx_up_         ( EMP_GUI_SKIN.data->gfx_up      ).
        gfx_down_       ( EMP_GUI_SKIN.data->gfx_down    ).
        gfx_remove_     ( EMP_GUI_SKIN.data->gfx_check   ).
        score_          ( pScore);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

