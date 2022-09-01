//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_REGISTRAR_H
#define EMP_GUI_REGISTRAR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_registrar_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, registrar_c, public, registrar_i)
private:
    emp::cnt::vector<window_i*> m_vpWindows;
    state_e m_eState;

public:
    registrar_c(void):
        m_vpWindows(),
        m_eState(state_e::none)
    {}

    virtual ~registrar_c(void)
    {}

public:
    void destroy(void)
    {
        m_vpWindows.resize(0);
        m_eState = state_e::none;
    }

public:
    //>> registrar_i
    void register_window(window_i* const a_pValue)
    {
        remove_window(a_pValue);
        m_vpWindows.add(a_pValue);
    }

    void remove_window(window_i* const a_pValue);

    EMP_RETURN state_e state(void) const { return m_eState; }

    void start(void) { m_eState = state_e::Started; }

    void stop(void);
    void update(void);

    EMP_RETURN bool is_modal_visible(void) const;
    //<< registrar_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

