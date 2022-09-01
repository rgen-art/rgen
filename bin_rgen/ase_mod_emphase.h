//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_EMPHASE_H
#define ASE_MOD_EMPHASE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//namespace emp { namespace loc { class locale_c; } }

namespace emp {
    namespace gui {
        class designer_c;
        class dialog_c;
        class label_c;
        class list_c;
        class picture_c;
        class score_c;
        class splitter_c;
        EMP_FORWARD(enum, ui32, change_e)
        EMP_FORWARD(enum, ui32, return_e)
    }

    namespace rfx {
        class type_i;
    }
}

/*
namespace emp { namespace rfx { class type_i; } }
namespace emp { namespace rfx { template <typename> class type_t; } }

namespace emp { namespace snd { class layout_i; } }
namespace emp { namespace snd { class scale_i; } }

namespace emp { namespace str { class real_format_s; } }
namespace emp { namespace str { template <typename, typename> class formater_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor { class album_c; } }
namespace ase { namespace cor { class mapping_c; } }
namespace ase { namespace cor { class music_i; } }
namespace ase { namespace cor { class music_shell_c; } }
namespace ase { namespace cor { class option_shell_c; } }
namespace ase { namespace cor { class user_c; } }

namespace ase { namespace gui { class score_c; } }
namespace ase { namespace gui { class synthesize_c; } }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase {
    namespace cor {
        class album_c;
        class music_i;
        class music_shell_c;
    }

namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, emphase_c, public, mode_a)
private:
    // play
    EMP_POINTER(emp::gui::splitter_c) m_pPlay;
    EMP_POINTER(emp::gui::splitter_c) m_pSplitterPreview;
    EMP_POINTER(emp::gui::splitter_c) m_pSplitterList;

    EMP_POINTER(emp::gui::list_c) m_pPlayListAlbum;
    EMP_POINTER(emp::gui::list_c) m_pPlayListMusic;

    EMP_POINTER(emp::gui::picture_c) m_pPlayCover;
    EMP_POINTER(emp::gui::score_c) m_pPlayScore;

    float m_fScaleScore;
    float m_fScaleTime;
    float m_fScale;

    EMP_POINTER(emp::gui::designer_c) m_pDsnPlayOptions;
    EMP_POINTER(emp::gui::designer_c) m_pDsnSelectMusic;

    EMP_POINTER(emp::str::formater_t<emp::str::real_format_s, float>) m_pFmtPercent;
    EMP_POINTER(emp::str::formater_t<emp::str::real_format_s, float>) m_pFmtReal;

    emp::cnt::vector<ase::cor::music_shell_c*> m_vpShells;
    emp::cnt::vector<ase::cor::music_i*> m_vpPlaylist;

    emp::cnt::vector<ase::cor::album_c*> m_vpAlbums;
    emp::cnt::vector<emp::rfx::type_i*> m_vpMusics;

    size_t m_ui32CurrentAlbum;

    mode_i* m_pCurrentMod = nullptr;

public:
    emphase_c(void);
    ~emphase_c(void);

public:
    EMP_RETURN bool post_load(void);

    //>>mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void);
    void public_stop(void);
    EMP_RETURN bool public_update(void);
    EMP_RETURN bool condition_satisfied(void) const;
    //<<mode_a

    //>>handler_i
    using tbase1::process_mouse;
    using tbase1::process_keyboard;
    //<<handler_i

public:
    EMP_RETURN emp::gui::return_e on_dlg_exit_cancel(c_bool a_bValue, emp::gui::c_change_e a_eChange);
    EMP_RETURN emp::gui::return_e on_dlg_exit_ok(c_bool a_bValue, emp::gui::c_change_e a_eChange);

public:
    EMP_RETURN emp::gui::designer_c const& dsn_play_options(void) const { return *m_pDsnPlayOptions; }
    EMP_RETURN emp::gui::designer_c& dsn_play_options(void) { return *m_pDsnPlayOptions; }

private:
    EMP_RETURN bool update_score(void);

    void populate_album_list(c_ui32 a_ui32Selection,emp::gui::c_change_e a_eChange);
    void populate_music_list(c_ui32 a_ui32Index);

    void set_current_music(emp::cnt::vector<ase::cor::music_i*> const& a_rvpMusics, c_size a_stOffset);

    //>>gui
    EMP_RETURN bool album_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, emp::gui::c_change_e a_eChange);
    EMP_RETURN bool album_double_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, emp::gui::c_change_e a_eChange);
    EMP_RETURN bool music_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, emp::gui::c_change_e a_eChange);
    EMP_RETURN bool music_double_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, emp::gui::c_change_e a_eChange);
    //<<gui
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

