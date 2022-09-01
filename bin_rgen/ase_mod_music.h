//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_MUSIC_H
#define ASE_MOD_MUSIC_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_a.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class button_c;
    class dialog_c;
    class docker_c;
    class label_c;
    class list_c;
    class property_c;
    class renderer_c;
    class spacer_c;
    class splitter_c;
    class score_c;
    class toggle_c;
    EMP_FORWARD(enum, ui32, change_e)
} }

namespace emp { namespace gui { class keyboard_c; } }

namespace ase { namespace cor { class music_i; } }
namespace ase { namespace cor { class updater_i; } }

namespace ase { namespace cor { class easy_updater_c; } }
namespace ase { namespace cor { class normal_updater_c; } }
namespace ase { namespace cor { class hard_updater_c; } }

namespace ase { namespace cor { class improvize_updater_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,music_c, public, mode_a)
private:
    EMP_POINTER(emp::gui::label_c) m_pCtrlCompletion;
    EMP_POINTER(emp::gui::button_c) m_pButtonScoreOriginal;
    EMP_POINTER(emp::gui::score_c) m_pCtrlScoreOriginal;
    EMP_POINTER(emp::gui::button_c) m_pButtonCenter;
    EMP_POINTER(emp::gui::button_c) m_pButtonKeyboard;
    EMP_POINTER(emp::gui::keyboard_c) m_pCtrlKeyboard;
    EMP_POINTER(ase::cor::easy_updater_c) m_pEasyUpdater;
    EMP_POINTER(ase::cor::normal_updater_c) m_pNormalUpdater;
    EMP_POINTER(ase::cor::hard_updater_c) m_pHardUpdater;
    EMP_POINTER(ase::cor::improvize_updater_c) m_pImprovizeUpdater;

    EMP_HOLDER(ase::cor::updater_i) m_pUpdater;

    EMP_HOLDER(ase::cor::music_i) m_pMusic;

    EMP_POINTER(emp::gui::dialog_c) m_pDlgStart;
    EMP_POINTER(emp::gui::splitter_c) m_pDlgStartSplitter;
    EMP_POINTER(emp::gui::label_c) m_pDlgStartLabel;
    EMP_POINTER(emp::gui::property_c) m_pDlgStartProperty;

    EMP_POINTER(emp::gui::label_c) m_pLblStopYouRanked;
    EMP_POINTER(emp::gui::label_c) m_pLblStopRank;
    EMP_POINTER(emp::gui::toggle_c) m_pTglStopShowMyScores;
    EMP_POINTER(emp::gui::spacer_c) m_pSpcStop0;
    EMP_POINTER(emp::gui::list_c) m_pLvwStopHighScores;
    EMP_POINTER(emp::gui::spacer_c) m_pSpcStop1;
    EMP_POINTER(emp::gui::toggle_c) m_pTglStopSavePartition;
    EMP_POINTER(emp::gui::docker_c) m_pDckStop;
    EMP_POINTER(emp::gui::dialog_c) m_pDlgStop;

    EMP_POINTER(emp::gui::label_c) m_pLblExit;
    EMP_POINTER(emp::gui::spacer_c)m_pSpcExit0;
    EMP_POINTER(emp::gui::toggle_c) m_pTglExitSavePartition;
    EMP_POINTER(emp::gui::docker_c) m_pDckExit;
    EMP_POINTER(emp::gui::dialog_c) m_pDlgExit;

    EMP_POINTER(emp::gui::dialog_c) m_pDlgPause;
    EMP_POINTER(emp::gui::label_c) m_pDlgPauseLabel;

    bool m_bExit = false;
    bool m_bStarted = false;
    bool m_bEnded = false;

    bool m_bImprovize = false;

    bool m_bShowMyScores = false;
    bool m_bSavePartition = false;
    bool m_bSaveRandom = false;
    bool m_bSaveImpro = false;

    float m_fTimeScale = 1.0f;
    float m_fScoreScale = 1.0f;
    float m_fScale = 1.0f;

public:
    music_c(void);
    ~music_c(void);

public:
    //>> mode_a
    EMP_RETURN bool public_create(void);
    void public_destroy(void);
    EMP_RETURN bool public_start(void);
    void public_stop(void) {}
    EMP_RETURN bool public_update(void);
    EMP_RETURN bool condition_satisfied(void) const { return m_bEnded; }
    //<< mode_a

public:
    //>> handler_i
    EMP_RETURN emp::gui::return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    EMP_RETURN emp::gui::return_e on_dlg_start_ok(c_bool a_bValue, emp::gui::c_change_e a_eChange);
    EMP_RETURN emp::gui::return_e on_dlg_start_cancel(c_bool a_bValue, emp::gui::c_change_e a_eChange);

    EMP_RETURN emp::gui::return_e on_dlg_stop_ok(c_bool a_bValue, emp::gui::c_change_e a_eChange);

    EMP_RETURN emp::gui::return_e on_dlg_exit_ok(c_bool a_bValue, emp::gui::c_change_e a_eChange);
    EMP_RETURN emp::gui::return_e on_dlg_exit_cancel(c_bool a_bValue, emp::gui::c_change_e a_eChange);

    EMP_RETURN emp::gui::return_e on_dlg_pause_cancel(c_bool a_bValue, emp::gui::c_change_e a_eChange);
    EMP_RETURN emp::gui::return_e on_dlg_pause_ok(c_bool a_bValue, emp::gui::c_change_e a_eCHange);

    EMP_RETURN emp::gui::return_e on_tgl_show_my_scores_check(emp::gui::toggle_c const* const a_pSender, c_bool a_bValue,
        emp::gui::c_change_e a_eChange);

    EMP_RETURN emp::gui::return_e on_tgl_save_partition(emp::gui::toggle_c const* const a_pSender, c_bool a_bValue,
        emp::gui::c_change_e a_eChange);

public:
    void set_music(ase::cor::music_i* const a_pValue, c_bool a_bTouch);
    void set_improvize(c_bool a_bValue) { m_bImprovize = a_bValue; }

private:
    EMP_RETURN bool refresh_options(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

