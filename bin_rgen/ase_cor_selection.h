//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_COR_ENTROPY_H
#define ASE_COR_ENTROPY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

#define ASE_COR_POOL_E_NAME pool_e
#define ASE_COR_POOL_E_VALUES\
    enum_value( none      )\
    enum_value( Trash     )\
    enum_value( Temporary )\
    enum_value( Unused    )\
    enum_value( All       )\
    enum_value( Graveyard )
#define enum_name ASE_COR_POOL_E_NAME
#define enum_type
#define enum_values ASE_COR_POOL_E_VALUES
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, pool_c)
public:
    pool_e type = pool_e::none;
    astring id{};
    u8string name{};

private:
    vector<astring> m_vsOse{};

public:
    map_t<astring, size_t> files;

    pool_c(void) : files(2) {}

    EMP_RETURN bool post_load(void);

    void add_source(c_astring& a_sValue);
    void clear(void);

    void remove_source(c_size a_stIndex);
    void insert_source(c_size a_stIndex, c_astring& a_sSource);

    EMP_RETURN vector<astring> const& sources(void) const { return m_vsOse; }
    EMP_RETURN size_t remove_source(map_t<astring, size_t>::iterator_c const& a_rIt);
    EMP_RETURN size_t remove_source(c_astring& a_rsFile);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, pool_list_c)
public:
    vector<pool_c*> pools{};

    pool_list_c(void) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, selection_c)
public:
    selection_c(void) {}
    ~selection_c(void) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

