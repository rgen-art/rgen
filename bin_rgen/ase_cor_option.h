//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_COR_OPTION_H
#define ASE_COR_OPTION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_game.h"

#include "emp_ose_compiler.h" // @@0 remove...

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui { struct skin_data_s; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class mapping_c;
class pool_list_c;
class pool_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define ASE_COR_MOD_E_NAME mod_e
#define ASE_COR_MOD_E_VALUES\
    enum_value( Easy   )\
    enum_value( Normal )\
    enum_value( Hard   )
#define enum_name ASE_COR_MOD_E_NAME
#define enum_values ASE_COR_MOD_E_VALUES
#define enum_type
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(keyboard_option_s)
public:
    bool visible = true;
private:
    bool m_bLetters = true;
    float m_fScale = 1.0f;
public:
    ase::cor::mapping_c* mapping = nullptr;
    emp::snd::layout_i* layout = nullptr;
    emp::snd::scale_i* m_pScale = nullptr;

public:
    EMP_RETURN bool operator==(keyboard_option_s const& a_rFrom) const;

    EMP_RETURN bool letters(void) const { return visible ? m_bLetters : false; }
    void set_letters(c_bool a_bValue) { if (visible) { m_bLetters = a_bValue; } }

    EMP_RETURN float scale(void) const { return m_fScale; }
    void set_scale(float const a_fValue) { if (visible) { m_fScale = a_fValue; } }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(score_option_s)
public:
    bool visible = true;

private:
    bool m_bLetters = true;
    bool m_bDurations = true;
    float m_fScale = 1.0f;
    float m_fTimeScale = 1.0f;

public:
    EMP_RETURN bool operator==(score_option_s const& a_rFrom) const;

    EMP_RETURN bool letters(void) const { return visible ? m_bLetters : false; }
    void set_letters(c_bool a_bValue) { if (visible) { m_bLetters = a_bValue; } }

    EMP_RETURN bool durations(void) const { return visible ? m_bDurations : false; }
    void set_durations(c_bool a_bValue) { if (visible) { m_bDurations = a_bValue; } }

    EMP_RETURN float scale(void) const { return m_fScale; }
    void set_scale(float const a_fValue) { if (visible) { m_fScale = a_fValue;} }

    EMP_RETURN float time_scale(void) const { return m_fTimeScale; }
    void set_time_scale(float const a_fValue) { if (visible) { m_fTimeScale = a_fValue; } }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(emphase_option_s)
public:
    mod_e mod = mod_e::Easy;
    bool shuffle = false;
    float bpm = 0;
    keyboard_option_s keyboard{};
    score_option_s score{};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(evolution_option_s)
public:
    si64 left = 256;
    si64 max = 256;
    ui64 consumed = 0;
    emp::ose::mutation_s mutation{};
    emp::ose::crossover_s crossover{};

    EMP_RETURN bool operator==(evolution_option_s const& a_rFrom) const
    { return mutation == a_rFrom.mutation && crossover == a_rFrom.crossover; }

    EMP_RETURN bool operator!=(evolution_option_s const& a_rFrom) const
    { return operator==(a_rFrom) == false; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(selection_option_s)
public:
    pool_list_c* pools = nullptr;
    pool_c* pool = nullptr;
    pool_c* trash = nullptr;
    pool_c* graveyard = nullptr;
    bool edition = false;
    ui32 gallery_width = 8;

public:
    EMP_RETURN bool operator==(selection_option_s const& a_rFrom) const
    {
        return
            pools == a_rFrom.pools &&
            pool == a_rFrom.pool &&
            trash == a_rFrom.trash &&
            gallery_width == a_rFrom.gallery_width;
    }

    EMP_RETURN bool operator!=(selection_option_s const& a_rFrom) const
    { return operator==(a_rFrom) == false; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(option_s)
public:
    evolution_option_s evolution{};
    selection_option_s selection{};
    emphase_option_s emphase{};
    emp::gui::skin_data_s* skin = nullptr;
    emp::loc::locale_c* locale = nullptr;
    float scale = 1.0f;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, user_c)
private:
    u8string m_su8Name;

public:
    option_s options;
    ase::mod::mode_i* init_mod = nullptr;


public:
    user_c(void);
    ~user_c(void);

public:
    EMP_RETURN c_u8string& name(void) const { return m_su8Name; }

    EMP_RETURN bool post_load(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, option_shell_c)
public:
    option_shell_c(void) {};
    
    EMP_RETURN user_c* user(void) const;
    void set_user(user_c* const a_pUser);

    EMP_RETURN bool evolution_visible(void);
    EMP_RETURN bool selection_visible(void);
    EMP_RETURN bool emphase_visible(void);

    EMP_RETURN bool literal_mutation_enabled(void);
    EMP_RETURN bool variant_mutation_enabled(void);
    EMP_RETURN bool call_mutation_enabled(void);
    EMP_RETURN bool data_mutation_enabled(void);
    EMP_RETURN bool trailing_mutation_enabled(void);
    EMP_RETURN bool crossover_enabled(void);

public:
    EMP_RETURN bool keyboard_visible(void) { return user()->options.emphase.keyboard.visible; }
    EMP_RETURN bool score_visible(void) { return user()->options.emphase.score.visible; }
    EMP_RETURN bool rhythm(void) { return user()->options.emphase.mod != ase::cor::mod_e::Easy; }
    EMP_RETURN bool shuffle_visible(void)
    {
        c_mod_e eMod = user()->options.emphase.mod;
        return eMod == ase::cor::mod_e::Easy ||
               eMod == ase::cor::mod_e::Normal ||
               eMod == ase::cor::mod_e::Hard;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

