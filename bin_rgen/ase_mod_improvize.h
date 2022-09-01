//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_IMPROVIZE_H
#define ASE_MOD_IMPROVIZE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd { class composition_c; } }
namespace emp { namespace snd { class track_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor { class composition_music_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class music_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,improvize_c, public, mode_i)
private:
    music_c* m_pModMusic;

    EMP_POINTER(emp::snd::track_c) m_pTrack;
    EMP_POINTER(emp::snd::composition_c) m_pComposition;
    EMP_POINTER(ase::cor::composition_music_c) m_pMusic;

public:
    improvize_c(void);
    ~improvize_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    EMP_RETURN music_c* mod_music(void) const { return m_pModMusic; }

public:
    // >> mode_i
    EMP_RETURN bool create(void);
    void destroy(void);
    EMP_RETURN bool start(emp::tim::clock_c const& a_rClock);
    void stop(void);
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN emp::gui::registrar_i const& registrar(void) const;
    EMP_RETURN emp::gui::registrar_i& registrar(void);
    EMP_RETURN emp::loc::string_s const& name(void) const;
    EMP_RETURN bool set_name(emp::loc::string_s const& a_eValue);
    EMP_RETURN mode_i* next_mod(void) const;
    EMP_RETURN mode_i* prev_mod(void) const;
    EMP_RETURN emp::tim::time_c const& time(void) const;
    EMP_RETURN bool fire_next(void);
    EMP_RETURN bool fire_prev(void);
    // << mode_i

    void set_next_mod(mode_i* const a_pValue);
    void set_prev_mod(mode_i* const a_pValue);

    // >> handler_i
    virtual EMP_RETURN emp::gui::return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    virtual EMP_RETURN emp::gui::return_e process_mouse(emp::app::c_event_s& a_rEvent);
    // << handler_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

