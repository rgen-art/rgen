//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_improvize.h"

#include "ase_cor_emphase.h"

#include "ase_mod_music.h"

#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_snd_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::snd;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
improvize_c::improvize_c(void):
m_pModMusic(),
m_pTrack(),
m_pComposition(),
m_pMusic()
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
improvize_c::~improvize_c(void)
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::post_load(void)
{
    if (m_pModMusic == nullptr)
    {
        EMP_LOG_ERROR("Invalid music mod");
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::create(void)
{
    EMP_RET_FALSE_IF_NOT(m_pModMusic->create());

    m_pModMusic->set_improvize(true);

    scale_c* const pScale = EMP_STORE(scale_c)->get_object(ALITERAL("emp_snd_scale_chromatic.xml"));

    c_float fBeatPerMinute = 128;
    c_float fBeatDuration = 60000000.0f / fBeatPerMinute;

    m_pTrack->set_name(m_pComposition->name());
    m_pTrack->set_scale(pScale);
    m_pTrack->set_beat_duration(emp::ast::cast<ui64>(fBeatDuration));

    m_pComposition->set_name(U8LITERAL("improvize"));
    m_pComposition->add_track(m_pTrack);

    m_pMusic->set_composition(m_pComposition);
    m_pMusic->build_data();

    m_pModMusic->set_music(&*m_pMusic, false);

    return true;
}

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void improvize_c::destroy(void)
{
    // @@0 remove EMP_RFX_SAFE_DESTROY_DEALLOCATE(m_pModMusic);

    m_pTrack->destroy_track();
    m_pComposition->destroy();
    m_pMusic->destroy();
}

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::start(emp::tim::clock_c const& a_rClock)
{ return m_pModMusic->start(a_rClock); }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void improvize_c::stop(void)
{ m_pModMusic->stop(); }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::update(emp::tim::clock_c const& a_rClock)
{ return m_pModMusic->update(a_rClock); }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN emp::gui::registrar_i const& improvize_c::registrar(void) const
{ return m_pModMusic->registrar(); }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN emp::gui::registrar_i& improvize_c::registrar(void)
{ return m_pModMusic->registrar(); }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN emp::loc::string_s const& improvize_c::name(void) const
{ return m_pModMusic->name(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::set_name(emp::loc::string_s const& a_eValue)
{ return m_pModMusic->set_name(a_eValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN mode_i* improvize_c::next_mod(void) const
{ return m_pModMusic->next_mod(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN mode_i* improvize_c::prev_mod(void) const
{ return m_pModMusic->prev_mod(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::tim::time_c const& improvize_c::time(void) const
{ return m_pModMusic->time(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::fire_next(void)
{ return m_pModMusic->fire_next(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool improvize_c::fire_prev(void)
{ return m_pModMusic->fire_prev(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_c::set_next_mod(mode_i* const a_pValue)
{ m_pModMusic->set_next_mod(a_pValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_c::set_prev_mod(mode_i* const a_pValue)
{ m_pModMusic->set_prev_mod(a_pValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e improvize_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{ return m_pModMusic->process_keyboard(a_rEvent); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e improvize_c::process_mouse(emp::app::c_event_s& a_rEvent)
{ return m_pModMusic->process_mouse(a_rEvent); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

