//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_MODE_A_H
#define ASE_MOD_MODE_A_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_loc_string.h"

#include "emp_mem_pointer.h"

#include "emp_tim_time.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fct { template <typename> class function_t; } }

namespace emp { namespace gui { class registrar_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef emp::fct::function_t<void(*)(void)> on_mod_f;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,mode_a, public, mode_i)
private:
    // serialized
    emp::gui::window_c* m_pWindow;
    emp::loc::string_s m_peName;
    emp::loc::string_s m_peDesc;
    mode_i* m_pNextMod;
    mode_i* m_pPrevMod;

    // runtime
    EMP_POINTER(emp::gui::registrar_c)  m_pRegistrar;
    EMP_POINTER(on_mod_f) m_pOnNext;
    EMP_POINTER(on_mod_f) m_pOnPrevious;

    emp::tim::time_c m_Time;

    bool m_bPreviousFired;
    bool m_bNextFired;
    bool m_bPause;

    float m_fDuration;

public:
    mode_a(void);
    virtual ~mode_a(void);

public:
    EMP_RETURN bool post_load(void);

    // mode_i
    EMP_RETURN bool create(void);
    void destroy(void);
    EMP_RETURN bool start(emp::tim::clock_c const& a_rClock);
    void stop(void);
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN emp::gui::registrar_i const& registrar(void) const;
    EMP_RETURN emp::gui::registrar_i& registrar(void);
    EMP_RETURN emp::loc::string_s const& name(void) const { return m_peName; }
    virtual EMP_RETURN bool set_name(emp::loc::string_s const& a_eValue) { m_peName = a_eValue; return true; }
    virtual EMP_RETURN mode_i* next_mod(void) const { return m_pNextMod; }
    virtual EMP_RETURN mode_i* prev_mod(void) const { return m_pPrevMod; }
    EMP_RETURN bool fire_next(void);
    EMP_RETURN bool fire_prev(void);
    EMP_RETURN emp::tim::time_c const& time(void) const { return m_Time; }
    // mode_i

public:
    //>>mode_a
    virtual EMP_RETURN bool public_create(void) = 0;
    virtual void public_destroy(void) = 0;
    virtual EMP_RETURN bool public_start(void) = 0;
    virtual void public_stop(void) = 0;
    virtual EMP_RETURN bool public_update(void) = 0;
    virtual EMP_RETURN bool condition_satisfied(void) const = 0;
    //<<mode_a

public:
    //>> handler_i
    virtual EMP_RETURN emp::gui::return_e process_keyboard(emp::app::c_event_s&) { return emp::gui::return_e::Unused; }
    virtual EMP_RETURN emp::gui::return_e process_mouse(emp::app::c_event_s&) { return emp::gui::return_e::Unused; } //-V524
    //<< handler_i

public:
    void set_next_mod(mode_i* const a_pValue) { m_pNextMod = a_pValue; }
    void set_prev_mod(mode_i* const a_pValue) { m_pPrevMod = a_pValue; }

public:
    EMP_RETURN emp::gui::window_c const& window(void) const { return *m_pWindow; }
    EMP_RETURN emp::gui::window_c& window(void) { return *m_pWindow; }

public:
    void set_pause(c_bool a_bValue) { m_bPause = a_bValue; }

public:
    EMP_RETURN emp::loc::string_s const& desc(void) const { return m_peDesc; }

    EMP_RETURN float duration(void) const { return  m_fDuration; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

