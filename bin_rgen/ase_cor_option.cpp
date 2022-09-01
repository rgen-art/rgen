//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_cor_option.h"

#include "ase_mod_selection.h"
#include "ase_mod_emphase.h"
#include "ase_mod_evolution.h"
#include "ase_mod_improvize.h"
#include "ase_mod_music.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
user_c::user_c(void):
    m_su8Name(),
    options()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
user_c::~user_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool user_c::post_load(void)
{
    auto& rEvolution = options.evolution;
    rEvolution.left = 256;

    auto& rMutation = rEvolution.mutation;
    auto const l_load = [](emp::ose::node_mutation_s& a_rMutation)
    {
        if (a_rMutation.m_ui32Count == 0)
        {
            a_rMutation.m_ui32Count = 8;
            a_rMutation.m_ui32Base = 0;
            a_rMutation.m_fScale = 1.0f;
            a_rMutation.m_fOffset = 0.3f;
        }
    };

    l_load(rMutation.literal);
    l_load(rMutation.variant);
    l_load(rMutation.call);
    l_load(rMutation.data);
    l_load(rMutation.trailing);


    auto& rCross = rEvolution.crossover;
    if (rCross.m_ui32Count == 0)
    {
        rCross.m_ui32Count = 8;
        rCross.m_ui32Base = 0;
        rCross.m_ui32Offset = 8;
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::evolution_visible(void)
{ return ASE_GAME.is_current_mod(ASE_GAME.mod_evolution()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::selection_visible(void)
{ return ASE_GAME.is_current_mod(ASE_GAME.mod_selection()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::emphase_visible(void)
{
    return
        ASE_GAME.is_current_mod(ASE_GAME.mod_emphase()) ||
        ASE_GAME.is_current_mod(ASE_GAME.mod_improvize()) ||
        ASE_GAME.is_current_mod(ASE_GAME.mod_music());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::literal_mutation_enabled(void)
{ return ASE_GAME.user().options.evolution.mutation.literal.m_bActive; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::variant_mutation_enabled(void)
{ return ASE_GAME.user().options.evolution.mutation.variant.m_bActive; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::call_mutation_enabled(void)
{ return ASE_GAME.user().options.evolution.mutation.call.m_bActive; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::data_mutation_enabled(void)
{ return ASE_GAME.user().options.evolution.mutation.data.m_bActive; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::trailing_mutation_enabled(void)
{ return ASE_GAME.user().options.evolution.mutation.trailing.m_bActive; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool option_shell_c::crossover_enabled(void)
{ return ASE_GAME.user().options.evolution.crossover.m_bActive; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool keyboard_option_s::operator==(c_keyboard_option_s& a_rFrom) const
{
    return
        visible == a_rFrom.visible &&
        m_bLetters == a_rFrom.m_bLetters &&
        emp::tt::equal_(m_fScale, a_rFrom.m_fScale);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_option_s::operator==(c_score_option_s& a_rFrom) const
{
    return
        visible == a_rFrom.visible &&
        m_bLetters == a_rFrom.m_bLetters &&
        m_bDurations == a_rFrom.m_bDurations &&
        emp::tt::equal_(m_fScale, a_rFrom.m_fScale) &&
        emp::tt::equal_(m_fTimeScale, a_rFrom.m_fTimeScale);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN user_c* option_shell_c::user(void) const
{ return &ASE_GAME.user(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void option_shell_c::set_user(user_c* const a_pValue)
{ ASE_GAME.set_user(a_pValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

