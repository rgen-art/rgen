//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_APP_GAME_I_H
#define EMP_APP_GAME_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {
    struct event_s;
} }

namespace emp { namespace gui {
    class handler_i;
    EMP_FORWARD(enum, ui32, return_e)
} }

namespace emp { namespace tim { class manager_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, game_i)
public:
    game_i(void) {}
    virtual ~game_i(void) = 0;

public:
    //>> game_i
    virtual EMP_RETURN bool update(emp::tim::clock_c const& a_rClock) = 0;
    virtual void render(void) = 0;
    virtual EMP_RETURN emp::gui::handler_i* handler(void) const = 0;
    virtual EMP_RETURN emp::gui::return_e process_mouse(emp::app::event_s const& a_rEvent) = 0;
    virtual EMP_RETURN emp::gui::return_e process_keyboard(emp::app::event_s const& a_rEvent) = 0;
    //<< game_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline game_i::~game_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

