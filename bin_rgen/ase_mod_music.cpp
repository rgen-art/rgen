//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_music.h"

#include "ase_mod_emphase.h"

#include "ase_cor_reflection.rfx.h"

#include "ase_gui_score.h"
#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"
#include "emp_app_reflection.rfx.h"

#include "emp_gui_dialog.h"
#include "emp_gui_manager.h"
#include "emp_gui_property.h"
#include "emp_gui_registrar.h"
#include "emp_gui_skin.h"
#include "emp_gui_spacer.h"
#include "emp_gui_splitter.h"
#include "emp_gui_toggle.h"

#include "emp_snd_reflection.rfx.h"

#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::tt;
using namespace emp::fct;
using namespace emp::snd;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,keyboard_c, public, container_c)
public:
    bool m_bShowLetters = false;

private:
    float m_fScale = 1.0f;
    emp::cnt::vector<emp::gui::label_c*> m_vpTouches{};
    ase::cor::matrix_c const* m_pMatrix = nullptr;
    c_keyboard_skin_s* m_pSkin = nullptr;
    shell<emp::gui::pc_control_i> m_hpControls{};
    
public:
    keyboard_c(void);
    ~keyboard_c(void);

public:
    void create(void);
    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN emp::gui::return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN emp::gui::return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void set_mapping(ase::cor::mapping_c const* const a_pMapping);
    void set_matrix(ase::cor::matrix_c const* const a_pValue);

public:
    void scale_up(void) { scale_(m_fScale + 0.03f); }
    void scale_down(void) { scale_(m_fScale - 0.03f); }

    EMP_RETURN float scale(void) const { return m_fScale; }
    keyboard_c& scale_(c_float a_fValue);

    void set_skin(cpc_keyboard_skin_s a_pValue);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
keyboard_c::keyboard_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
keyboard_c::~keyboard_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void keyboard_c::create(void)
{
    destroy();

    c_size stCount = emp::snd::constants::key_count;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gui::label_c* pLabelButton = EMP_ALLOCATE(emp::gui::label_c);

        // @@0 create pLabelButton->create();
        pLabelButton->state().autosize.xy(true);
        add_control(pLabelButton);
        m_vpTouches.add(pLabelButton);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void keyboard_c::destroy(void)
{
    emp::cnt::foreach(m_vpTouches, emp::mem::safe_delete_reset_t()); // @@0 create
    m_vpTouches.resize(0);

    m_pMatrix = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool keyboard_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    // @@0 use constanst;
    c_float fCountX = 10.0f;
    c_float fCountY = 4.0f;

    c_float fSX = EMP_GUI_SKIN.spacing_x();//m_pSkin->spacing->x();
    c_float fSY = EMP_GUI_SKIN.spacing_y();//m_pSkin->spacing->y();

    c_float fSW = (fCountX - 1.0f) * fSX;
    c_float fSH = (fCountY - 1.0f) * fSY;

    //float const fW = (m_pContainer->transformation().width() - fSW) / fCountX;
    //float const fH = (m_pContainer->transformation().height() - fSH) / fCountY;

    c_v2f v2ButtonSpace = { fSX, fSY };

    float fMaxX = 0;
    float fMaxY = 0;
    c_size stCount = emp::snd::constants::key_count;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gui::label_c* const pLabelButton = m_vpTouches.val(stIndex);
        EMP_ASSERT(pLabelButton != nullptr);

        pLabelButton->state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(pLabelButton->resize());
        fMaxX = pLabelButton->transformation().width();
        fMaxY = pLabelButton->transformation().height();
        pLabelButton->state().autosize.xy(false);
    }

    c_v2f v2ButtonSize = { fMaxX, fMaxY };

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stCount - stIndex - 1;
        emp::gui::label_c* const pLabelButton = m_vpTouches.val(stIndex);
        EMP_ASSERT(pLabelButton != nullptr);

        // @@0 use constants
        c_float fX = emp::ast::cast<float>(stOffset % 10);
        c_float fY = emp::ast::cast<float>(stOffset / 10);
        c_v2f v2Offset = { fX, fY };
        v2f const v2Position = v2f::add(v2f::mul(v2ButtonSize, v2Offset), v2f::mul(v2ButtonSpace, v2Offset));
        pLabelButton->transformation().position_(v2Position).size_(v2ButtonSize);

        c_size stAddress = m_pMatrix->get_address(stIndex);
        pLabelButton->state().select_(emp::tt::is_not_max(stAddress));
        EMP_RET_FALSE_IF_NOT(pLabelButton->resize());
        pLabelButton->render_text_(m_bShowLetters);
    }

    transformation().size_(fSW + fCountX * fMaxX, fSH + fCountY * fMaxY);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e keyboard_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false ||
        state().enable == false)
    { return emp::gui::return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    auto const& rKeyboard = a_rEvent.keyboard;
    emp::app::c_key_e eKey = rKeyboard.m_eKey;

    c_bool bPressed = rKeyboard.m_bPressed;
    c_bool bShift = rKeyboard.m_bShift;
    c_bool bAlt = rKeyboard.m_bAlt;
    c_bool bCtrl = rKeyboard.m_bCtrl;

    if (bShift == false &&
        bAlt == false &&
        bCtrl == false &&
        state().focus)
    {
        if (eKey == emp::app::key_e::Left ||
            eKey == emp::app::key_e::Up)
        {
            if (bPressed)
            { scale_up(); }
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
        else if (
            eKey == emp::app::key_e::Right ||
            eKey == emp::app::key_e::Down)
        {
            if (bPressed)
            { scale_down(); }
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e keyboard_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return emp::gui::return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    auto const& rMouse = a_rEvent.mouse;
    if (state().enable &&
        state().hover &&
        a_rEvent.mouse_pressed() &&
        a_rEvent.keyboard.modified() == false)
    {
        emp::app::c_button_e eButton = rMouse.m_eButton;
        if (eButton == emp::app::button_e::WheelUp)
        {
            scale_up();
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
        else if (eButton == emp::app::button_e::WheelDown)
        {
            scale_down();
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void keyboard_c::set_mapping(ase::cor::mapping_c const* const a_pMapping)
{
    EMP_ASSERT(a_pMapping != nullptr);
    c_ui32 ui32Count = emp::snd::constants::key_count;
    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    {
        emp::gui::label_c* const pLabelButton = m_vpTouches.val(ui32Index);
        EMP_ASSERT(pLabelButton != nullptr);


        pLabelButton->set_text(get_text(*a_pMapping, ui32Index));
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void keyboard_c::set_matrix(ase::cor::matrix_c const* const a_pValue)
{ m_pMatrix = a_pValue; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
keyboard_c& keyboard_c::scale_(c_float a_fValue)
{
    m_fScale = emp::tt::minof(3.0f, emp::tt::maxof(0.01f, a_fValue));
    c_size stCount = emp::snd::constants::key_count;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpTouches.val(stIndex)->scale_(m_fScale); }
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void keyboard_c::set_skin(cpc_keyboard_skin_s a_pValue)
{
    m_pSkin = a_pValue;
    c_size stCount = emp::snd::constants::key_count;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpTouches.val(stIndex)->skin_(a_pValue ? a_pValue->label : nullptr); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace ase::cor;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
music_c::music_c(void) :
    m_pCtrlCompletion(),
    m_pButtonScoreOriginal(),
    m_pCtrlScoreOriginal(),
    m_pButtonCenter(),
    m_pButtonKeyboard(),
    m_pCtrlKeyboard(),
    m_pEasyUpdater(),
    m_pNormalUpdater(),
    m_pHardUpdater(),
    m_pImprovizeUpdater(),
    m_pUpdater(),
    m_pMusic(),
    m_pDlgStart(),
    m_pDlgStartSplitter(),
    m_pDlgStartLabel(),
    m_pDlgStartProperty(),
    m_pLblStopYouRanked(),
    m_pLblStopRank(),
    m_pTglStopShowMyScores(),
    m_pSpcStop0(),
    m_pLvwStopHighScores(),
    m_pSpcStop1(),
    m_pTglStopSavePartition(),
    m_pDckStop(),
    m_pDlgStop(),
    m_pLblExit(),
    m_pSpcExit0(),
    m_pTglExitSavePartition(),
    m_pDckExit(),
    m_pDlgExit(),
    m_pDlgPause(),
    m_pDlgPauseLabel()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
music_c::~music_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool music_c::public_create(void)
{
    emp::gui::skin_c const& rSkin = EMP_GUI_SKIN;
    auto& rGame = ASE_GAME;

    m_pCtrlKeyboard->create();
    m_pCtrlKeyboard->set_skin(rGame.skin().keyboard.get(ALITERAL("kbd.")));
    m_pCtrlKeyboard->state().navigable_(true).focusable_(true).index_(0);

    m_pButtonKeyboard->skin_(EMP_GUI_SKIN_(button, "but.todo.zero.link"))
        .control_(m_pCtrlKeyboard)
        .state().index_(0).autosize.xy(true);

    m_pButtonCenter->skin_(EMP_GUI_SKIN_(button, ""))
        .control_(m_pButtonKeyboard)
        .state().dock_(emp::gui::dock_e::Bottom).index_(0).autosize.y_(true);

    m_pCtrlScoreOriginal->create();
    m_pCtrlScoreOriginal->set_skin(rGame.skin().score.get(ALITERAL("scr.")));
    m_pCtrlScoreOriginal->state().dock_(emp::gui::dock_e::Top).navigable_(true).focusable_(true).index_(0).autosize.y_(true);

    m_pButtonScoreOriginal->skin_(EMP_GUI_SKIN_(button, "but.todo.zero.link"))
        .control_(m_pCtrlScoreOriginal)
        .state().dock_(emp::gui::dock_e::Top).index_(1).autosize.y_(true);

    // @@0 create m_pCtrlCompletion->create();
    m_pCtrlCompletion->skin_(EMP_GUI_SKIN_(label, "lbl.ase.completion"));
    m_pCtrlCompletion->state().dock_(emp::gui::dock_e::Top).autosize.y_(true);
    m_pCtrlCompletion->set_text(U8LITERAL("0.00%"));

    window().add_control(m_pButtonScoreOriginal);
    window().add_control(m_pCtrlCompletion);
    window().add_control(m_pButtonCenter);

    //------------------------------------------------------------
    //m_pDlgStart
    //------------------------------------------------------------
    // @@0 create m_pDlgStartLabel->create();
    m_pDlgStartLabel->skin_(EMP_GUI_SKIN_(label, "lbl.todo.toolbar"));
    m_pDlgStartLabel->state().dock_(emp::gui::dock_e::Top).autosize.y_(true);
    m_pDlgStartLabel->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_dlg_start")));

    m_pDlgStartProperty->expanded_(true)
        .skin_(EMP_GUI_SKIN_(list, "lst."))
        .registrar_(&registrar())
        .designer_(&rGame.mod_emphase().dsn_play_options())
        .state().dock_(emp::gui::dock_e::Top).index_(0).navigable_(true).autosize.xy(true);
    EMP_RET_FALSE_IF_NOT(m_pDlgStartProperty->set_object(&rGame.rfx_options()));
    m_pDlgStartProperty->expanded_(false);
    EMP_RET_FALSE_IF_NOT(m_pDlgStartProperty->resize());

    m_pDlgStartSplitter->skin_(EMP_GUI_SKIN_(splitter, ""))
        .master_(m_pDlgStartLabel).slave_(m_pDlgStartProperty).orientation_(emp::gui::dock_e::Top)
        .state().index_(1).autosize.xy(true);
    m_pDlgStartSplitter->redock();

    m_pDlgStart->skin_(EMP_GUI_SKIN_(dialog, "dlg.todo"))
        .type_(emp::gui::dialog_e::OkCancel)
        .user_control_(m_pDlgStartSplitter)
        .on_ok_(function(this, &music_c::on_dlg_start_ok))
        .on_cancel_(function(this, &music_c::on_dlg_start_cancel))
        .state().index_(0).autosize.xy(true);

    registrar().register_window(m_pDlgStart);

    //------------------------------------------------------------
    //m_pDlgStop
    //------------------------------------------------------------
    // @@0 create m_pLblStopYouRanked->create();
    m_pLblStopYouRanked->skin_(EMP_GUI_SKIN_(label, "lbl.todo.toolbar"));
    m_pLblStopYouRanked->state().dock_(emp::gui::dock_e::Top).autosize.xy(true);
    m_pLblStopYouRanked->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_dlg_stop")));

    // @@0 create m_pLblStopRank->create();
    m_pLblStopRank->skin_(EMP_GUI_SKIN_(label, "lbl.todo.h4.link"));
    m_pLblStopRank->state().dock_(emp::gui::dock_e::Top).autosize.xy(true);
    m_pLblStopRank->set_text(U8LITERAL("0"));

    // @@0 create m_pTglStopShowMyScores->create();
    m_pTglStopShowMyScores->skin_(EMP_GUI_SKIN_(toggle, "tgl.enable"));
    m_pTglStopShowMyScores->state().dock_(emp::gui::dock_e::Top).navigable_(true).focusable_(true).index_(0).autosize.y_(true);
    m_pTglStopShowMyScores->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_show_my_scores")));
    m_pTglStopShowMyScores->on_check.function_(function(this, &music_c::on_tgl_show_my_scores_check));

    m_pSpcStop0->skin_(EMP_GUI_SKIN_(container, "")).state().dock_(emp::gui::dock_e::Top);

    m_pLvwStopHighScores->skin_(EMP_GUI_SKIN_(list, "lst."))
        .add_column_(emp::loc::string_s(ALITERAL("ase_cor_music_rank")))
        .add_column_(emp::loc::string_s(ALITERAL("ase_cor_music_user")))
        .add_column_(emp::loc::string_s(ALITERAL("ase_cor_music_completion")))
        .add_column_(emp::loc::string_s(ALITERAL("ase_cor_music_time")))
        .add_column_(emp::loc::string_s(ALITERAL("ase_cor_music_success")))
        .add_column_(emp::loc::string_s(ALITERAL("ase_cor_music_failure")))
        .autosize_count_(5)
        .state().dock_(emp::gui::dock_e::Top).navigable_(true).index_(1).autosize.x_(true);

    // @@0 spacer m_pSpcStop1->skin_(rSkin.spacer.get(ALITERAL("default"))).state().dock_(emp::gui::dock_e::Top);

    // @@0 create m_pTglStopSavePartition->create();
    m_pTglStopSavePartition->skin_(EMP_GUI_SKIN_(toggle, "tgl.enable"));
    m_pTglStopSavePartition->state().dock_(emp::gui::dock_e::Top).navigable_(true).focusable_(true).index_(2).autosize.y_(true);
    m_pTglStopSavePartition->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_save_partition")));
    m_pTglStopSavePartition->on_check.function_(function(this, &music_c::on_tgl_save_partition));

    m_pDckStop->skin_(EMP_GUI_SKIN_(sizer, "szr.todo"))
        .state().index_(1).autosize.xy(true);
    //m_pDckStop->state().set_dock(emp::gui::dock_e::Top);
    m_pDckStop->add_control(m_pLblStopYouRanked);
    m_pDckStop->add_control(m_pLblStopRank);
    m_pDckStop->add_control(m_pTglStopShowMyScores);
    m_pDckStop->add_control(m_pSpcStop0);
    m_pDckStop->add_control(m_pLvwStopHighScores);
    m_pDckStop->add_control(m_pSpcStop1);
    m_pDckStop->add_control(m_pTglStopSavePartition);

    m_pDlgStop->skin_(EMP_GUI_SKIN_(dialog, "dlg.todo"))
        .type_(emp::gui::dialog_e::Ok)
        .user_control_(m_pDckStop)
        .on_ok_(function(this, &music_c::on_dlg_stop_ok))
        .on_cancel_(function(this, &music_c::on_dlg_stop_ok))
        .state().index_(0).autosize.xy(true);

    registrar().register_window(m_pDlgStop);

    //------------------------------------------------------------
    //m_pDlgExit
    //------------------------------------------------------------
    // @@0 create m_pLblExit->create();
    m_pLblExit->skin_(EMP_GUI_SKIN_(label, "lbl.todotoolbar"));
    m_pLblExit->state().dock_(emp::gui::dock_e::Top).autosize.y_(true);
    m_pLblExit->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_dlg_exit")));

    // @@0 spacer m_pSpcExit0->skin_(rSkin.spacer.get(ALITERAL("default"))).state().dock_(emp::gui::dock_e::Top);

    // @@0 create m_pTglExitSavePartition->create();
    m_pTglExitSavePartition->skin_(EMP_GUI_SKIN_(toggle, "tgl.enable"));
    m_pTglExitSavePartition->state().dock_(emp::gui::dock_e::Top).navigable_(true).focusable_(true).index_(0).autosize.y_(true);
    m_pTglExitSavePartition->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_save_partition")));
    m_pTglExitSavePartition->on_check.function_(function(this, &music_c::on_tgl_save_partition));

    m_pDckExit->skin_(EMP_GUI_SKIN_(sizer, ""))
        .state().index_(1).autosize.xy(true);
    //m_pDckExit->state().set_dock(emp::gui::dock_e::Top);
    m_pDckExit->add_control(m_pLblExit);
    m_pDckExit->add_control(m_pSpcExit0);
    m_pDckExit->add_control(m_pTglExitSavePartition);

    m_pDlgExit->transformation().width_(0.6f * rSkin.resolution.scale());
    m_pDlgExit->skin_(EMP_GUI_SKIN_(dialog, "dlg.todo"))
        .type_(emp::gui::dialog_e::OkCancel)
        .user_control_(m_pDckExit)
        .on_ok_(function(this, &music_c::on_dlg_exit_ok))
        .on_cancel_(function(this, &music_c::on_dlg_exit_cancel))
        .state().index_(0).autosize.y_(true);

    registrar().register_window(m_pDlgExit);

    //------------------------------------------------------------
    //m_pDlgPause
    //------------------------------------------------------------
    // @@0 create m_pDlgPauseLabel->create();
    m_pDlgPauseLabel->skin_(EMP_GUI_SKIN_(label, "lbl.todo.toolbar"));
    m_pDlgPauseLabel->state().autosize.y_(true);
    m_pDlgPauseLabel->set_text(emp::loc::string_s(ALITERAL("ase_mod_music_dlg_pause")));

    m_pDlgPause->transformation().width_(0.6f * rSkin.resolution.scale());
    m_pDlgPause->skin_(EMP_GUI_SKIN_(dialog, "dlg."))
        .type_(emp::gui::dialog_e::Ok)
        .user_control_(m_pDlgPauseLabel)
        .on_ok_(function(this, &music_c::on_dlg_pause_ok))
        .on_cancel_(function(this, &music_c::on_dlg_pause_cancel))
        .state().index_(0).autosize.y_(true);

    registrar().register_window(m_pDlgPause);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_start_ok(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgStart));
        m_bStarted = true;
        m_bEnded = false;
        set_pause(false);

        EMP_RET_X_IF_NOT(emp::gui::return_e::Error, refresh_options());
        m_pUpdater->start();
    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_start_cancel(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgStart));
        emp::tt::silent(fire_prev());
    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void save_track(user_c const& a_rUser, track_c const& a_rTrack, c_u8string& a_sName);
EMP_INTERN void save_track(user_c const& a_rUser, track_c const& a_rTrack, c_u8string& a_sName)
{
    auto& rFactory = EMP_FACTORY;

    auto* const pComposition = rFactory.store<composition_c>()->new_object_pattern(ALITERAL("tmp"));
    pComposition->set_name(STR_U8FORMAT("#0 - #1", a_rUser.name(), a_sName));

    scale_c* const pScale = rFactory.store<scale_c>()->get_object(ALITERAL("chromatic.xml"));

    auto* const pMusic = rFactory.store<composition_music_c>()->new_object_pattern(ALITERAL("tmp"));
    pMusic->set_composition(pComposition);

    auto* const pAlbum = rFactory.store<album_c>()->get_object(ALITERAL("ase_cor_album_tmp.xml"));
    pAlbum->add_music(pMusic);

    c_float fBeatPerMinute = a_rTrack.beat_per_minute();
    c_float fBeatDuration = 60000000.0f / fBeatPerMinute;

    auto* const pTrack = rFactory.store<track_c>()->new_object_pattern(ALITERAL("emp_snd_track_tmp"));
    pTrack->set_name(pComposition->name());
    pTrack->set_scale(pScale);
    pTrack->set_beat_duration(emp::ast::cast<ui64>(fBeatDuration));

    pComposition->add_track(pTrack);

    c_size stNoteCount = a_rTrack.notes().count();
    for (size_t stNote = 0; stNote < stNoteCount; ++stNote)
    { pTrack->notes().add(a_rTrack.notes().ref(stNote)); }

    pMusic->build_data();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_stop_ok(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgStop));
        emp::tt::silent(fire_prev());
        m_bEnded = true;

        if (m_bSavePartition &&
            m_bSaveRandom)
        {
            m_pMusic->translate(emp::tim::time_c(), false);
            m_pMusic->build_copy();
            save_track(ASE_GAME.user(), *m_pMusic->track(), m_pMusic->name());
        }

    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_exit_ok(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        if (m_pUpdater)
        { m_pUpdater->stop(); }

        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgExit));
        emp::tt::silent(fire_prev());
        m_bExit = true;

        if (m_bSavePartition &&
            m_bSaveImpro)
        {
            auto& rGame = ASE_GAME;
            save_track(rGame.user(), *rGame.track(), U8LITERAL("Improvize"));
        }
    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_exit_cancel(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgExit));
        set_pause(false);
    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_pause_cancel(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgPause));
        set_pause(false);
    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_dlg_pause_ok(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange)) //-V524
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgPause));
        set_pause(false);
    }
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_tgl_show_my_scores_check(emp::gui::toggle_c const* const, c_bool a_bValue, emp::gui::c_change_e a_eChange)
{
    if (m_pUpdater == nullptr)
    { return emp::gui::return_e::Consumed; }

    m_bShowMyScores = a_bValue;

    m_pLvwStopHighScores->clear();

    ui32 ui32Rank = 1;
    ui32 ui32RankCount = 0;

    ui32 EMP_TT_MAX_VAR(ui32Bucket);
    emp::cnt::vector<size_t> vIndices;

    auto& rUser = ASE_GAME.user();

    c_completion_option_s oOptions(rUser.options);
    performance_c const& rPerformance = m_pMusic->performance();
    c_completion_data_s oCurrent = rPerformance.current();

    c_ui32 ui32Count = emp::ast::cast<ui32>(rPerformance.bucket_count());
    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    {
        completion_bucket_c const& rBucket = rPerformance.bucket(ui32Index);
        if (rBucket.options() == oOptions)
        {
            ui32Bucket = ui32Index;

            c_size stDataCount = rBucket.data_count();
            for (size_t stData = 0; stData < stDataCount; ++stData)
            {
                c_completion_data_s& rData = rBucket.data(stData);
                if (m_bShowMyScores == false ||
                    &rUser == rData.user)
                {
                    bool bFound = false;
                    c_size stIndexCount = vIndices.count();
                    for (size_t stI = 0; stI < stIndexCount; ++stI)
                    {
                        c_completion_data_s& rSort = rBucket.data(vIndices.val(stI));
                        if (rData.is_higher(rSort))
                        {
                            vIndices.insert(stI, stData);
                            bFound = true;
                            break;
                        }
                    }
                    if (bFound == false)
                    { vIndices.add(stData); }
                    ++ui32RankCount;
                }
            }
            break;
        }
    }

    emp::str::formater_t<emp::str::real_format_s, float> oFmtPercent;
    oFmtPercent.m_tParams.round(true).precision(1).percent(true);

    c_ui32 EMP_TT_MAX_VAR(ui32Max);
    c_ui32 ui32IndexCount = emp::ast::cast<ui32>(vIndices.count());
    for (ui32 ui32I = 0; ui32I < ui32IndexCount; ++ui32I)
    {
        completion_bucket_c const& rBucket = rPerformance.bucket(ui32Bucket);
        c_completion_data_s& rSort = rBucket.data(vIndices.val(ui32I));

        if (rSort == oCurrent)
        { ui32Rank = ui32I + 1; }

        emp::gui::line_c* const pLine = m_pLvwStopHighScores->new_line(ui32I);
        m_pLvwStopHighScores->add_item(pLine, STR_U8FORMAT("#0", ui32I + 1), ui32Max);
        m_pLvwStopHighScores->add_item(pLine, rSort.user->name(), ui32Max);

        c_float fScore = rSort.score;
        oFmtPercent.set_value(fScore);

        m_pLvwStopHighScores->add_item(pLine, oFmtPercent.to_u8string(), ui32Max);

        m_pLvwStopHighScores->add_item(pLine, emp::str::u8from(emp::tim::time_c(rSort.time)), ui32Max);

        c_float fSuccess = rSort.success;
        oFmtPercent.set_value(fSuccess);
        m_pLvwStopHighScores->add_item(pLine, oFmtPercent.to_u8string(), ui32Max);

        c_float fFailure = rSort.failure;
        oFmtPercent.set_value(fFailure);
        m_pLvwStopHighScores->add_item(pLine, oFmtPercent.to_u8string(), ui32Max);

        m_pLvwStopHighScores->add_line(pLine);
    }

    m_pLblStopRank->set_text(STR_U8FORMAT("#0/#1", ui32Rank, ui32RankCount));

    EMP_RET_X_IF_NOT(emp::gui::return_e::Error, m_pLvwStopHighScores->resize());
    m_pLvwStopHighScores->set_selection(ui32Rank, 0, true, a_eChange);
    m_pLvwStopHighScores->set_check(ui32Rank, true);
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::on_tgl_save_partition(emp::gui::toggle_c const* const, c_bool a_bValue,
    emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    m_bSavePartition = a_bValue;
    return emp::gui::return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void music_c::public_destroy(void)
{
    // @@0 create m_pCtrlCompletion->destroy();
    m_pCtrlScoreOriginal->destroy();
    m_pCtrlKeyboard->destroy();

    m_pUpdater = nullptr;

    m_pMusic = nullptr;

    // @@0 create m_pDlgStartLabel->destroy();
   // @@0 create  m_pDlgStartProperty->destroy();
    // @@0 create m_pLblStopYouRanked->destroy();
    // @@0 create m_pLblStopRank->destroy();
 // @@0 create    m_pLvwStopHighScores->destroy();
    // @@0 create m_pLblExit->destroy();
    // @@0 create m_pDlgPauseLabel->destroy();

    m_bExit = false;
    m_bStarted = false;
    m_bEnded = false;

    m_fTimeScale = 1.0f;
    m_fScoreScale = 1.0f;
    m_fScale = 1.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e music_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    // @@0 menu move to game_c
    emp::app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;

    if (rKeyboard.m_bPressed &&
        registrar().is_modal_visible() == false &&
        window().is_hiding_or_hidden() == false)
    {
        emp::gui::c_change_e eChange = emp::gui::change_e::Keyboard;
        emp::app::c_key_e eKey = rKeyboard.m_eKey;
        if (eKey == emp::app::key_e::Escape)
        {
            emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pDlgExit, true, false, eChange, true));
            EMP_RET_X_IF_NOT(emp::gui::return_e::Error, m_pDlgExit->resize());
            m_pDlgExit->center();
            set_pause(true);
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
        else if (
            eKey == emp::app::key_e::Enter)
        {
            emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pDlgPause, true, false, eChange, true));
            EMP_RET_X_IF_NOT(emp::gui::return_e::Error, m_pDlgPause->resize());
            m_pDlgPause->center();
            set_pause(true);
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
    }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void music_c::set_music(music_i* const a_pValue, c_bool a_bTouch)
{
    EMP_ASSERT(a_pValue != nullptr);

    //a_pValue->build();

    if (a_bTouch)
    {
        c_music_e eMusic = a_pValue->type();
        if (eMusic == music_e::Imitate ||
            eMusic == music_e::Randomize)
        { EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(a_pValue)); }
    }

    m_pMusic = a_pValue;

    ASE_GAME.user().options.emphase.bpm = m_pMusic->beat_per_minute();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool music_c::refresh_options(void)
{
    game_c& rGame = ASE_GAME;
    user_c& rUser = rGame.user();
    option_s& rOptions = rUser.options;
    c_emphase_option_s& rEmphase = rOptions.emphase;
    c_keyboard_option_s& rKeyboard = rEmphase.keyboard;
    c_score_option_s& rScore = rEmphase.score;

    c_float fBpm = rEmphase.bpm;
    if (emp::tt::equal_(m_pMusic->beat_per_minute(), fBpm) == false)
    {
        float const fFactor = m_pMusic->beat_per_minute() / fBpm;
        m_pMusic->scale(fFactor);
    }
    if (m_bImprovize)
    { m_pUpdater = m_pImprovizeUpdater; }
    else
    {
        c_mod_e eMod = rEmphase.mod;
        if (eMod == mod_e::Easy)
        { m_pUpdater = m_pEasyUpdater; }
        else if (eMod == mod_e::Normal)
        {
            float const fMesureDuration = 4 * 60.0f / m_pMusic->beat_per_minute();
            emp::tim::time_c const oDelta(emp::tim::time_c::from_seconds(fMesureDuration));
            m_pMusic->translate(oDelta, false);
            m_pUpdater = m_pNormalUpdater;
        }
        else if (eMod == mod_e::Hard)
        {
            float const fMesureDuration = 4 * 60.0f / m_pMusic->beat_per_minute();
            emp::tim::time_c const oDelta(emp::tim::time_c::from_seconds(fMesureDuration));
            m_pMusic->translate(oDelta, false);
            m_pUpdater = m_pHardUpdater;
        }
        else
        { m_pUpdater = nullptr; }
    }

    rGame.track()->copy(*m_pMusic->track());
    rGame.track()->clear_track();

    m_pCtrlCompletion->state().visible_(m_bImprovize == false);

    m_pMusic->build_copy();
    m_pUpdater->set_music(m_pMusic);
    rGame.set_layout(rUser.options.emphase.keyboard.layout);
    rGame.set_scale(rUser.options.emphase.keyboard.m_pScale);
    mapping_c const* const pMapping = rUser.options.emphase.keyboard.mapping;

    m_pCtrlScoreOriginal->reset();
    m_pCtrlScoreOriginal->set_compute_beat_size(true);
    EMP_RET_FALSE_IF_NOT(m_pCtrlScoreOriginal->set_track(m_pUpdater->track()));
    m_pCtrlScoreOriginal->set_interpretation(m_pUpdater->interpretation());
    EMP_RET_FALSE_IF_NOT(m_pCtrlScoreOriginal->set_mapping(pMapping));
    m_pCtrlScoreOriginal->set_compute_beat_size(false);
    m_pCtrlScoreOriginal->set_scale_x(rScore.scale());
    m_pCtrlScoreOriginal->set_scale_y(rScore.scale());
    m_pCtrlScoreOriginal->set_time_scale(rScore.time_scale());
    m_pCtrlScoreOriginal->set_show_durations(rScore.durations());
    m_pCtrlScoreOriginal->set_show_letters(rScore.letters());
    m_pCtrlScoreOriginal->state().visible_(rScore.visible);
    m_pCtrlScoreOriginal->set_start_size(m_pCtrlScoreOriginal->transformation().width() * 0.5f);

    m_pCtrlKeyboard->scale_(rKeyboard.scale());
    m_pCtrlKeyboard->set_mapping(pMapping);
    m_pCtrlKeyboard->set_matrix(rGame.matrix());
    m_pCtrlKeyboard->m_bShowLetters = rKeyboard.letters();
    m_pCtrlKeyboard->state().visible_(rKeyboard.visible);

    m_pSpcExit0->state().visible_(m_bImprovize);
    m_pTglExitSavePartition->state().visible_(m_bImprovize);
    m_bSaveImpro = m_bImprovize;

    c_bool bRandomize = m_bImprovize == false && m_pMusic->type() == music_e::Randomize;
    m_pSpcStop1->state().visible_(bRandomize);
    m_pTglStopSavePartition->state().visible_(bRandomize);
    m_bSaveRandom = bRandomize;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool music_c::public_start(void)
{
    m_bExit = false;
    m_bStarted = false;
    m_bEnded = false;

    emp::gui::c_change_e eChange = emp::gui::change_e::Code;
    m_pTglStopSavePartition->set_check(false, eChange);
    m_pTglExitSavePartition->set_check(false, eChange);

    m_bSavePartition = false;
    m_bSaveRandom = false;
    m_bSaveImpro = false;

    EMP_RET_FALSE_IF_NOT(refresh_options());

    EMP_RET_FALSE_IF_NOT(window().resize());

    m_pEasyUpdater->start();
    m_pNormalUpdater->start();
    m_pImprovizeUpdater->start();

    if (m_bImprovize == false)
    {
        // after window().refresh();
        m_pCtrlScoreOriginal->set_start_size(m_pCtrlScoreOriginal->transformation().width() * 0.5f);

        emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pDlgStart, true, false, eChange, true));

        m_pCtrlCompletion->set_text(U8LITERAL("0.00%"));
    }
    else
    { emp::tt::silent(on_dlg_start_ok(true, eChange)); }

    EMP_RET_FALSE_IF_NOT(m_pDlgStart->resize());
    m_pDlgStart->center();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool music_c::public_update(void)
{
    if (m_bStarted == false &&
        m_pDlgStart->is_visible())
    { EMP_RET_FALSE_IF_NOT(refresh_options()); }
    else if (m_pDlgStop->is_visible())
    {
    }
    else if (m_pDlgExit->is_visible())
    {
    }
    else if (m_pDlgPause->is_visible())
    {
    }
    else if (m_bStarted)
    {
        m_pUpdater->update(time());
        m_pCtrlScoreOriginal->set_time(m_pUpdater->music_time());
        m_pCtrlCompletion->m_su8Text.clear();
        m_pCtrlCompletion->set_text(STR_FORMATU8(m_pCtrlCompletion->m_su8Text, "#0%", REAL_FMT_R02(m_pUpdater->current_completion())));

        c_bool bEnded = m_pUpdater->condition_satisfied();
        if (bEnded == true &&
            m_bEnded == false &&
            m_bExit == false &&
            m_bStarted == true &&
            m_pDlgStop->is_visible() == false)
        {
            if (m_pUpdater)
            { m_pUpdater->stop(); }

            emp::gui::c_change_e eChange =emp::gui::change_e::Code;
            emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pDlgStop, true, false, eChange, true));

            m_pTglStopShowMyScores->set_check(m_bShowMyScores, eChange);
            EMP_RET_FALSE_IF_NOT(m_pDlgStop->resize());
            m_pDlgStop->center();
            m_pTglStopShowMyScores->set_check(m_bShowMyScores, eChange);
            set_pause(true);
        }

        user_c& rUser = ASE_GAME.user();
        keyboard_option_s& rKeyboard = rUser.options.emphase.keyboard;
        score_option_s& rScore = rUser.options.emphase.score;
        
        if (emp::tt::equal_(rScore.scale(), m_fScoreScale) == false)
        {
            m_fScoreScale = rScore.scale();
            m_pCtrlScoreOriginal->set_scale_x(m_fScoreScale);
            m_pCtrlScoreOriginal->set_scale_y(m_fScoreScale);
        }

        if (emp::tt::equal_(m_pCtrlScoreOriginal->scale_y(), m_fScoreScale) == false)
        {
            m_fScoreScale = m_pCtrlScoreOriginal->scale_y();
            rScore.set_scale(m_fScoreScale);
        }

        if (emp::tt::equal_(rScore.time_scale(), m_fTimeScale) == false)
        {
            m_fTimeScale = rScore.time_scale();
            m_pCtrlScoreOriginal->set_time_scale(m_fTimeScale);
        }

        if (emp::tt::equal_(m_pCtrlScoreOriginal->time_scale(), m_fTimeScale) == false)
        {
            m_fTimeScale = m_pCtrlScoreOriginal->time_scale();
            rScore.set_time_scale(m_fTimeScale);
        }

        if (emp::tt::equal_(m_fScale, EMP_GUI_SKIN.resolution.scale()) == false)
        {
            m_fScale = EMP_GUI_SKIN.resolution.scale();
            rUser.options.scale = m_fScale; // KJ : remove
        }

        if (emp::tt::equal_(m_fScale, rUser.options.scale) == false) // KJ : remove
        {
            m_fScale = rUser.options.scale; // KJ : remove
            EMP_GUI_SKIN.resolution.scale_(m_fScale);
        }

        if (emp::tt::equal_(rKeyboard.scale(), m_pCtrlKeyboard->scale()) == false)
        {
            rKeyboard.set_scale(m_pCtrlKeyboard->scale());
            m_pCtrlKeyboard->scale_(rKeyboard.scale());
        }

        rUser.options.scale = EMP_GUI_SKIN.resolution.scale(); // KJ : remove
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

