//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_LAYOUT_H
#define EMP_SND_LAYOUT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_constants.h"
#include "emp_snd_layout_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd { class synth_i; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,layout_c, public, layout_i)
private:
    astring m_sName;
    vector<cell_c> m_vCells;

public:
    layout_c(void);
    ~layout_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    void destroy(void);

public:
    //>> layout_i
    EMP_RETURN c_astring& name(void) const { return m_sName; }
    EMP_RETURN size_t cell_count(void) const { return m_vCells.count(); }
    EMP_RETURN cell_c const cell(c_size a_stIndex) { return m_vCells.ref(a_stIndex); }
    //<< layout_i

    /*EMP_RETURN emp::cnt::vector<cell_c> const& cells(void) const
    { return m_vCells; }
    EMP_RETURN emp::cnt::vector<cell_c>& cells(void)
    { return m_vCells; }*/

public:
    //void set_default(emp::snd::synth_i* const a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

