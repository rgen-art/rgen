//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_emphase.h"
#include "ase_cor_game.h"

#include "ase_cor_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_menu.h"
#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

#include "emp_gui_manager.h"
#include "emp_gui_registrar.h"
#include "emp_gui_skin.h"
#include "emp_gui_window.h"

#include "emp_rfx_factory.h"

#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mode_a::mode_a(void):
    m_pWindow(),
    m_peName(),
    m_peDesc(),
    m_pNextMod(),
    m_pPrevMod(),
    m_pRegistrar(),
    m_pOnNext(),
    m_pOnPrevious(),
    m_Time(),
    m_bPreviousFired(),
    m_bNextFired(),
    m_bPause(),
    m_fDuration()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mode_a::~mode_a(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mode_a::post_load(void)
{
    EMP_RET_FALSE_IF(m_pWindow == nullptr);

    return set_name(m_peName);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mode_a::create(void)
{
  //  m_pRegistrar->register_window(m_pWindow); //@@0 remove
  //  m_pRegistrar->register_window(m_pMenu);
    m_pWindow->create();
    //m_pWindow->state().set_index(0);
    return public_create();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mode_a::destroy(void)
{
    // m_fDuration = 0.0f; // @@0 remove
    // @@0 remove EMP_RFX_SAFE_DESTROY_DEALLOCATE(m_pWindow);
    m_pWindow = nullptr;

    m_peName = emp::loc::string_s(ALITERAL("emp::loc::string_s.notranslation"));
    m_peDesc = emp::loc::string_s(ALITERAL("emp_loc_no_translation"));

    m_pNextMod = nullptr;
    m_pPrevMod = nullptr;

    m_pRegistrar->destroy();
    m_Time = emp::tim::time_c();
    m_bPreviousFired = false;
    m_bNextFired = false;
    //*m_pOnNext = on_mod_f();
    //*m_pOnPrevious = on_mod_f();
    //m_pSound = nullptr;
    //m_fVolume = 1.0f;
    m_bPause = true;
    public_destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mode_a::start(emp::tim::clock_c const& EMP_SILENT(a_rClock))
{
    //m_pRegistrar->register_window(m_pMenu);
    m_pRegistrar->register_window(m_pWindow);

    ASE_GAME.user().init_mod = this; // @@0 save
    EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(&ASE_GAME.user()));


    m_pRegistrar->start();
    m_Time = emp::tim::time_c();
    m_bPreviousFired = false;
    m_bNextFired = false;
    m_bPause = true;

    emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pWindow, false, false, emp::gui::change_e::Code, false));

    return public_start();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mode_a::stop(void)
{
    //if (m_pMenu != nullptr)
    //{ m_pRegistrar->remove_window(m_pMenu); }
    //if (m_pWindow != nullptr)
    //{ m_pRegistrar->remove_window(m_pWindow); }
    m_pRegistrar->stop();
    public_stop();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::registrar_i const& mode_a::registrar(void) const
{ return *m_pRegistrar; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::registrar_i& mode_a::registrar(void)
{ return *m_pRegistrar; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mode_a::fire_next(void)
{
    if (m_bNextFired == false &&
        m_pNextMod != nullptr)
    {
        ASE_GAME.set_next_mod(m_pNextMod);
        m_bNextFired = true;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mode_a::fire_prev(void)
{
    if (m_bPreviousFired == false &&
        m_pPrevMod != nullptr)
    {
        ASE_GAME.set_next_mod(m_pPrevMod);
        m_bPreviousFired = true;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mode_a::update(emp::tim::clock_c const& a_rClock)
{
    if ((m_fDuration > 0 &&
        emp::tim::time_c::to_seconds(m_Time) > m_fDuration) ||
        condition_satisfied())
    {

        if (m_bPreviousFired == false)
        {
            if (next_mod() == nullptr && prev_mod() == nullptr)
            {
                EMP_APPLICATION.send_exit();
                return true;
            }
            else
            {
                emp::tt::silent(fire_next());
            }
        }
    }

    EMP_RET_FALSE_IF_NOT(public_update());

    m_pRegistrar->update();

    //if (m_pRegistrar->is_modal_visible() == false && m_bPause == false)
    { emp::tim::time_c::add(m_Time, a_rClock.delta(), m_Time); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

