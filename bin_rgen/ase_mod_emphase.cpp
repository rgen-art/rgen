//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_emphase.h"

#include "ase_mod_debug.h"
#include "ase_mod_music.h"

#include "ase_cor_emphase.h"
#include "ase_cor_reflection.rfx.h"

#include "ase_gui_score.h"
#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"
#include "emp_app_reflection.rfx.h"

#include "emp_gui_dialog.h"
#include "emp_gui_label.h"
#include "emp_gui_list.h"
#include "emp_gui_manager.h"
#include "emp_gui_property.h"
#include "emp_gui_registrar_i.h"

#include "emp_gui_skin.h"
#include "emp_gui_splitter.h"
#include "emp_gui_reflection.rfx.h"

#include "emp_loc_reflection.rfx.h"

#include "emp_snd_reflection.rfx.h"

#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::fct;
using namespace emp::gui;
using namespace ase::cor;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
emphase_c::emphase_c(void) :
m_pPlay(),
m_pSplitterPreview(),
m_pSplitterList(),
m_pPlayListAlbum(),
m_pPlayListMusic(),
m_pPlayCover(),
m_pPlayScore(),
m_fScaleScore(),
m_fScaleTime(),
m_fScale(),
m_pDsnPlayOptions(4u),
m_pDsnSelectMusic(4u),
m_pFmtPercent(),
m_pFmtReal(),
m_vpShells(),
m_vpPlaylist(),
m_vpAlbums(),
m_vpMusics(),
m_ui32CurrentAlbum(0)
{
    m_pCurrentMod = this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
emphase_c::~emphase_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::post_load(void)
{
    if (tbase1::post_load() == false)
    { return false; }
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::public_create(void)
{
    using namespace emp::loc;

    m_pFmtPercent->m_tParams.round(true).precision(0).percent(true);
    m_pFmtReal->m_tParams.round(true).precision(2);

    m_pDsnPlayOptions->copy(ASE_GAME.dsn_options());
    //m_pDsnPlayOptions->get<emphase_option_s>().get(ALITERAL("bpm"))->set_visibility(visibility_e::Reflect);
    //m_pDsnPlayOptions->get<emphase_option_s>().get(ALITERAL("bpm"))->enability_function() =
    //    function(&*m_pPlayOptions, &option_shell_c::rhythm);
    {
        class_c& rClass = m_pDsnSelectMusic->push<music_shell_c>(4u);

        rClass.push(ALITERAL("beat_per_minute"))   .formater_(m_pFmtReal);
        rClass.push(ALITERAL("highest_completion")).formater_(m_pFmtPercent);
        rClass.push(ALITERAL("highest_success"))   .formater_(m_pFmtPercent);
        rClass.push(ALITERAL("highest_failure"))   .formater_(m_pFmtPercent);
    }

    m_pPlayCover->skin_(EMP_GUI_SKIN_(picture, "pic.column")).state().focusable_(true).autosize.x_(true);
    m_pPlayCover->size_(0.2f);

    m_pPlayScore->create();
    m_pPlayScore->set_skin(ASE_GAME.skin().score.get(ALITERAL("scr.")));

    m_pSplitterPreview->skin_(EMP_GUI_SKIN_(splitter, ""))
        .master_(m_pPlayCover).slave_(m_pPlayScore).orientation_(dock_e::Left)
        .state().index_(0).autosize.y_(true);
    m_pSplitterPreview->transformation().width_(2.0f);
    m_pSplitterPreview->redock();

    m_pPlayListAlbum->skin_(EMP_GUI_SKIN_(list, "lst."))
        .add_column_(string_s(ALITERAL("ase_cor_album")))
        .on_selection_changed_(function(this, &ase::mod::emphase_c::album_changed))
        .on_selection_double_click_(function(this, &ase::mod::emphase_c::album_double_click))
        .state().navigable_(true).index_(0).autosize.x_(true);
    EMP_RET_FALSE_IF_NOT(m_pPlayListAlbum->resize());

    m_pPlayListMusic->skin_(EMP_GUI_SKIN_(list, "lst."))
        .designer_(m_pDsnSelectMusic)
        .add_column_(string_s(ALITERAL("ase_cor_music_title")))
        .add_column_(string_s(ALITERAL("ase_cor_music_duration")))
        .add_column_(string_s(ALITERAL("ase_cor_music_beat_per_minute")))
        .add_column_(string_s(ALITERAL("ase_cor_music_rating")))
        .add_column_(string_s(ALITERAL("ase_cor_music_played_count")))
        .add_column_(string_s(ALITERAL("ase_cor_music_user")))
        .add_column_(string_s(ALITERAL("ase_cor_music_completion")))
        .add_column_(string_s(ALITERAL("ase_cor_music_time")))
        .add_column_(string_s(ALITERAL("ase_cor_music_success")))
        .add_column_(string_s(ALITERAL("ase_cor_music_failure")))
        .show_columns_(true)
        .on_selection_changed_(function(this, &ase::mod::emphase_c::music_changed))
        .on_selection_double_click_(function(this, &ase::mod::emphase_c::music_double_click))
        .state().navigable_(true).index_(1);

    m_pSplitterList->skin_(EMP_GUI_SKIN_(splitter, ""))
        .master_(m_pPlayListAlbum).slave_(m_pPlayListMusic).orientation_(dock_e::Left)
        .state().index_(1);
    m_pSplitterList->redock();

    m_pPlay->skin_(EMP_GUI_SKIN_(splitter, ""))
        .master_(m_pSplitterPreview).slave_(m_pSplitterList).orientation_(dock_e::Top)
        .state().dock_(dock_e::Fill).index_(0);
    m_pPlay->redock();

    populate_album_list(0, change_e::Code);

    window().add_control(m_pPlay);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::public_destroy(void)
{
    // @@0 create m_pPlayListMusic->destroy();
    // @@0 create m_pPlayListAlbum->destroy();

    m_pPlayScore->destroy();

    foreach(m_vpShells, emp::mem::safe_delete_reset_t());
    m_vpShells.resize(0);

    m_vpPlaylist.resize(0);

    m_vpAlbums.resize(0);

    m_ui32CurrentAlbum = 0;

    foreach(m_vpMusics, emp::mem::safe_delete_reset_t());
    m_vpMusics.resize(0);

    m_fScaleScore = 1.0f;
    m_fScaleTime = 1.0f;
    m_fScale = 1.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::public_start(void)
{ return m_pPlayScore->set_mapping(ASE_GAME.user().options.emphase.keyboard.mapping); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::public_stop(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::update_score(void)
{
    auto& rGame = ASE_GAME;
    auto& rUser = rGame.user();
    auto& rScore = rUser.options.emphase.score;

    m_pPlayScore->set_compute_beat_size(false);
    EMP_RET_FALSE_IF_NOT(m_pPlayScore->set_mapping(rUser.options.emphase.keyboard.mapping));

    if (emp::tt::equal_(m_fScaleTime, m_pPlayScore->time_scale()) == false)
    {
        m_fScaleTime = m_pPlayScore->time_scale();
        rScore.set_time_scale(m_fScaleTime);
    }

    c_float fOptTimeScale = rScore.time_scale();
    if (emp::tt::equal_(m_fScaleTime, fOptTimeScale) == false)
    {
        m_fScaleTime = fOptTimeScale;
        m_pPlayScore->set_time_scale(m_fScaleTime);
    }

    if (emp::tt::equal_(m_fScaleScore, m_pPlayScore->scale_y()) == false)
    {
        m_fScaleScore = m_pPlayScore->scale_y();
        rScore.set_scale(m_fScaleScore);
    }

    c_float fOptScoreScale = rScore.scale();
    if (emp::tt::equal_(m_fScaleScore, fOptScoreScale) == false)
    {
        m_fScaleScore = fOptScoreScale;
        m_pPlayScore->set_scale_x(m_fScaleScore);
        m_pPlayScore->set_scale_y(m_fScaleScore);
    }

    auto& rSkin = EMP_GUI_SKIN;
    c_float fScale = rSkin.resolution.scale();
    if (emp::tt::equal_(m_fScale, fScale) == false)
    {
        m_fScale = fScale;
        rUser.options.scale = m_fScale; // KJ : remove
    }

    c_float fOptScale = rUser.options.scale; // KJ : remove
    if (emp::tt::equal_(m_fScale, fOptScale) == false)
    {
        m_fScale = fOptScale;
        rSkin.resolution.scale_(m_fScale);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::public_update(void)
{
    if (m_pPlay->state().visible)
    {
        auto& rGame = ASE_GAME;
        rGame.set_layout(nullptr);
        rGame.set_scale(nullptr);
    }

    return update_score();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::condition_satisfied(void) const
{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN album_c* get_current_album(vector<album_c*> const& a_rvpAlbums, c_size a_stIndex);
EMP_INTERN EMP_RETURN album_c* get_current_album(vector<album_c*> const& a_rvpAlbums, c_size a_stIndex)
{
    if (a_rvpAlbums.is_valid_index(a_stIndex))
    { return a_rvpAlbums.val(a_stIndex); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::music_changed(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e EMP_SILENT(a_eChange))
{
    if (emp::tt::is_not_max(a_ui32Line) &&
        a_ui32Line != 0)
    {
        album_c* const pAlbum = get_current_album(m_vpAlbums, m_ui32CurrentAlbum);

        music_i* const pMusic = &pAlbum->get_music(a_ui32Line - 1);
        EMP_ASSERT(pMusic != nullptr);

        pMusic->build_data();
        EMP_RET_FALSE_IF_NOT(m_pPlayScore->set_track(pMusic->track()));
        m_pPlayScore->reset();
    }
    else
    {
        //m_pPlayScore->set_music(nullptr);
        //m_pPlayScore->reset();
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::album_double_click(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e EMP_SILENT(a_eChange))
{
    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    album_c* const pAlbum = get_current_album(m_vpAlbums, a_ui32Line - 1);

    m_vpPlaylist.resize(0);
    c_size stCount = pAlbum->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        music_i* const pMusic = &pAlbum->get_music(stIndex);
        EMP_ASSERT(pMusic != nullptr);
        m_vpPlaylist.add(pMusic);
    }

    set_current_music(m_vpPlaylist, 0);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::music_double_click(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e EMP_SILENT(a_eChange))
{
    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    album_c* const pAlbum = get_current_album(m_vpAlbums, m_ui32CurrentAlbum);

    m_vpPlaylist.resize(0);
    c_size stCount = pAlbum->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        music_i* const pMusic = &pAlbum->get_music(stIndex);
        EMP_ASSERT(pMusic != nullptr);
        m_vpPlaylist.add(pMusic);
    }

    //EMP_LOG_INFO("SET_CURRENT_MUSIC : #0", a_stIndex);
    set_current_music(m_vpPlaylist, a_ui32Line - 1);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::populate_album_list(c_ui32 a_ui32Selection, c_change_e a_eChange)
{
    m_vpAlbums.resize(0);
    m_pPlayListAlbum->clear();

    {
        c_size stCount = EMP_STORE(album_list_c)->object_count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            album_list_c* const pAlbumList = EMP_STORE(album_list_c)->get_object(stIndex);
            EMP_ASSERT(pAlbumList != nullptr);

            c_size stAlbumCount = pAlbumList->albums().count();
            for (size_t stAlbum = 0; stAlbum < stAlbumCount; ++stAlbum)
            {
                album_c* const pAlbum = pAlbumList->albums().val(stAlbum);
                EMP_ASSERT(pAlbum != nullptr);
                m_vpAlbums.add(pAlbum);
            }
        }
    }

    {
        c_ui32 ui32Count = emp::ast::cast<ui32>(m_vpAlbums.count());
        for (size_t ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
        {
            album_c const* const pAlbum = m_vpAlbums.val(ui32Index);
            EMP_ASSERT(pAlbum != nullptr);
            m_pPlayListAlbum->add_line(pAlbum->name());
        }
        if (a_ui32Selection < ui32Count)
        { m_pPlayListAlbum->set_selection(a_ui32Selection + 1, 0, true, a_eChange); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::album_changed(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e a_eChange)
{
    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    m_ui32CurrentAlbum = a_ui32Line - 1;
    m_pPlayListMusic->clear();

    foreach(m_vpShells, emp::mem::safe_delete_reset_t());
    m_vpShells.resize(0);

    foreach(m_vpMusics, emp::mem::safe_delete_reset_t());
    m_vpMusics.resize(0);

    album_c* const pAlbum = m_vpAlbums.val(m_ui32CurrentAlbum);
    m_pPlayCover->model_(pAlbum->cover());

    c_size stMusicCount = pAlbum->count();
    for (size_t stIndex = 0; stIndex < stMusicCount; ++stIndex)
    {
        music_i& rMusic = pAlbum->get_music(stIndex);

        music_shell_c* const pMusicShell = EMP_ALLOCATE(music_shell_c, &rMusic);
        m_vpShells.add(pMusicShell);

        emp::rfx::type_t<music_shell_c>* const pType = EMP_ALLOCATE(emp::rfx::type_t<music_shell_c>);
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.reflect_object(*pType));
        pType->link().set_value(pMusicShell);

        m_pPlayListMusic->add_line(pType);
        m_vpMusics.add(pType);
    }

    if (stMusicCount)
    { m_pPlayListMusic->set_selection(1, 0, true, a_eChange); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::set_current_music(vector<music_i*> const& a_rvpMusics, c_size a_stOffset)
{
    ASE_GAME.mod_music().set_music(a_rvpMusics.val(a_stOffset), true);
    emp::tt::silent(fire_next());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

