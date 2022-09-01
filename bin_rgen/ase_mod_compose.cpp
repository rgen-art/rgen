//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_compose.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_emphase.h"
#include "ase_cor_game.h"
#include "ase_cor_reflection.rfx.h"

#include "ase_gui_score.h"
#include "ase_gui_skin.h"
#include "ase_gui_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_popup.h"
#include "emp_gui_property.h"
#include "emp_gui_skin.h"
#include "emp_gui_toggle.h"
#include "emp_gui_toolbar.h"
#include "emp_gui_window.h"

#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name state_e
#define enum_values\
    enum_value( Stop  )\
    enum_value( Play  )\
    enum_value( Pause )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, track_c)
public:
    EMP_FIELD(emp::snd::track_c*, track, nullptr);
    score_c score{};
    label_c title{};
    spacer_c title_spacer{};
    picture_c picture_up{};
    picture_c picture_down{};
    picture_c picture_remove{};
    docker_c header{};
    property_c property{};

private:
    ase::cor::emphase_c emphase{};
    toggle_c toggle_expanded{};
    emp::rfx::type_t<bool> toggle_expanded_rfx{};
    spacer_c picture_spacer{};
    toggle_c toggle_enabled{};
    emp::rfx::type_t<bool> toggle_enabled_rfx{};
    EMP_POINTER(emp::rfx::type_t<track_c>) reflection{};
    enum_t<emp::snd::layout_i> enum_layout{};
    enum_t<emp::snd::scale_i> enum_scale{};
    designer_c designer{8};
    bool expanded = false;
    size_t note = 0;
    emp::cnt::vector<emp::snd::note_c> m_vNotes{};
    emp::cnt::vector<emp::snd::note_c> m_vToStop{};
    emp::cnt::vector<emp::snd::note_c> m_vToPlay{};
    emp::cnt::vector<emp::snd::note_c> m_vEdition{};

public:
    track_c(void);
    ~track_c(void);

public:
    EMP_RETURN bool create(container_c* const a_pParent, registrar_i* const a_pRegistar, c_size a_stBaseIndex);
    void destroy(void);

    void register_control(container_c* const a_pParent);

public:
    void start(void);
    void stop(void);

public:
    void update(emp::tim::time_c const& a_rTime, emp::tim::time_c const& a_rDelta, c_state_e a_eState);

public:
    EMP_RETURN bool set_track(emp::snd::track_c* const a_pValue, ase::cor::mapping_c const* const a_pMapping);
    void set_skin(compose_skin_s const* const a_pValue);

public:
    EMP_RETURN c_u8string& name(void) const;
    void set_name(c_u8string& a_sValue);

    EMP_RETURN emp::snd::layout_i* layout(void) const;
    void set_layout(emp::snd::layout_i* const a_pValue);

    EMP_RETURN emp::snd::scale_i* scale(void) const;
    void set_scale(emp::snd::scale_i* const a_pValue);

    EMP_RETURN float beat_per_minute(void) const;
    void set_beat_per_minute(float const a_fValue);

    EMP_RETURN float volume(void) const;
    void set_volume(float const a_fValue);

    EMP_RETURN bool enabled(void) const;
    void set_enabled(c_bool a_bValue);

    EMP_RETURN bool get_enabled_rfx(bool& a_rbValue) { a_rbValue = enabled(); return true; }
    EMP_RETURN bool set_enabled_rfx(c_bool& a_bValue);

    EMP_RETURN bool get_expanded_rfx(bool& a_rbValue) { a_rbValue = expanded; return true; }
    EMP_RETURN bool set_expanded_rfx(c_bool& a_bValue);

public:
    void add_edition_note(emp::snd::note_c const& a_rNote);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, compose_c, public, splitter_c)
private:
    registrar_i* registrar = nullptr;

    list_c list;
    popup_c popup;
    container_c container;

    toolbar_c toolbar;

    shell<pc_control_i> m_hpControls{};

    emp::cnt::vector<track_c*> m_vpTracks;

    emp::snd::composition_c* composition;
    compose_skin_s const* skin;

    emp::cnt::vector<emp::snd::composition_c*> m_vpCompositions;

    emp::cnt::vector<emp::rfx::type_i*> m_vpReflections;

    emp::tim::time_c m_Time;

    bool m_bDragLeft;
    bool m_bDragRight;

    bool m_bLeftPressed;
    bool m_bLeftReleased;
    bool m_bLeftDoubleClick;

    bool m_bRightPressed;
    bool m_bRightReleased;
    bool m_bRightDoubleClick;

    emp::snd::note_c m_Note;

    v2f m_v2DragOrigin;
    v2f m_v2DragDelta;
    v2f m_v2Offset;

    state_e m_eState;
    size_t note;

public:
    compose_c(void);
    virtual ~compose_c(void);

public:
    void create(registrar_i* const a_pRegistrar);
    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    EMP_RETURN bool set_value(c_size a_stIndex, c_u8string& a_rsValue);
    EMP_RETURN bool get_value(c_size a_stIndex, u8string& a_rsValue);

public:
    EMP_RETURN bool add_track(emp::snd::track_c* const a_pTrack);
    void add_composition(emp::snd::composition_c* const a_pComposition, c_change_e a_eChange);

public:
    void populate_composition_list(c_ui32 a_ui32Selection, c_change_e a_eChange);

public:
    EMP_RETURN return_e on_new_composition(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_rename_composition(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_remove_composition(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);

public:
    EMP_RETURN bool on_composition_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);

public:
    EMP_RETURN return_e on_new_track_pushed(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_play_pause_pushed(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_stop_pushed(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);

public:
    EMP_RETURN return_e on_remove_track(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_move_up_track(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_move_down_track(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);

public:
    void set_skin(compose_skin_s const* const a_pValue);

private:
    EMP_RETURN track_c* get_active_track(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::track_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::snd::layout_i* , ( EMP_RFX_RW , layout          , layout          ) )\
    enum_value_ex( emp::snd::scale_i*  , ( EMP_RFX_RW , scale           , scale           ) )\
    enum_value_ex( float               , ( EMP_RFX_RW , beat_per_minute , beat_per_minute ) )\
    enum_value_ex( float               , ( EMP_RFX_RW , volume          , volume          ) )\
    enum_value_ex( bool                , ( EMP_RFX_RW , enabled         , enabled         ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_THIS_IN_MIL)
track_c::track_c(void):
    toggle_expanded_rfx(fct::function(this, &track_c::get_expanded_rfx), fct::function(this, &track_c::set_expanded_rfx)),
    toggle_enabled_rfx(fct::function(this, &track_c::get_enabled_rfx), fct::function(this, &track_c::set_enabled_rfx)),
    designer(5u)
{}
EMP_PRAGMA_POP_IGNORE(EMP_W_THIS_IN_MIL)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
track_c::~track_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool track_c::create(container_c* const a_pParent, registrar_i* const a_pRegistrar, c_size a_stBaseIndex)
{
    destroy();

    // @@0 create toggle_expanded->create();
    toggle_expanded.state().dock_(dock_e::Left).index_(0).focusable_(true).autosize.x_(true);
    toggle_expanded.set_text(&toggle_expanded_rfx);
    toggle_expanded.label.state().visible_(false);

    picture_up.state().dock_(dock_e::Left).index_(1).focusable_(true).autosize.x_(true);

    picture_down.state().dock_(dock_e::Left).index_(2).focusable_(true).autosize.x_(true);

    picture_spacer.state().dock_(dock_e::Left);

    // @@0 create toggle_enabled->create();
    toggle_enabled.state().dock_(dock_e::Left).index_(3).focusable_(true).autosize.x_(true);
    toggle_enabled.set_text(&toggle_enabled_rfx);
    toggle_enabled.label.state().visible_(false);
    
    // @@0 create title->create();
    title.state().dock_(dock_e::Left).autosize.xy(true);

    title_spacer.state().dock_(dock_e::Left);

    picture_remove.state().dock_(dock_e::Left).index_(4).focusable_(true).autosize.x_(true);

    header.state().dock_(dock_e::Top).index_(emp::ast::cast<ui32>(a_stBaseIndex + 0)).navigable_(true).autosize.xy(true);

    header.add_control(&toggle_expanded);
    header.add_control(&picture_up);
    header.add_control(&picture_down);
    //header->add_control(picture_spacer);
    header.add_control(&toggle_enabled);
    header.add_control(&title);
    header.add_control(&picture_remove);
    header.add_control(&title_spacer);

    reflection->link().set_value(this);
    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.reflect_object(*reflection));

    designer.push<snd::layout_i>(2u).field.expanded_(false).visibility_(visibility_e::Visible)
        .enumeration_(&enum_layout).on_new_item_(function(&enum_layout, &emp::gui::enum_t<snd::layout_i>::new_item));

    designer.push<snd::scale_i>(2u).field.expanded_(false).visibility_(visibility_e::Visible)
        .enumeration_(&enum_scale).on_new_item_(function(&enum_scale, &emp::gui::enum_t<snd::scale_i>::new_item));

    {
        auto& rClass = designer.push<track_c>(4u);
        rClass.field.expanded_(true).visibility_(visibility_e::Visible);
        rClass.push(ALITERAL("volume")).range_(0, 2, 0.01f);
        rClass.push(ALITERAL("beat_per_minute")).range_(8, 512, 1);
    }

    property.registrar_(a_pRegistrar).designer_(&designer).expand_last_column_(true)
        .state().dock_(dock_e::Top).index_(emp::ast::cast<ui32>(a_stBaseIndex + 1)).navigable_(true).autosize.xy(true);

    score.create();
    score.set_show_letters(false);
    score.set_edition(true);
    score.set_min_note_count(4);
    score.set_scale_x(1);
    score.state().dock_(dock_e::Top).index_(emp::ast::cast<ui32>(a_stBaseIndex + 2)).autosize.y_(true);

    register_control(a_pParent);

    emphase.create();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::register_control(container_c* const a_pParent)
{
    a_pParent->add_control(&header);
    a_pParent->add_control(&property);
    a_pParent->add_control(&score);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::destroy(void)
{
    stop();

    emphase.destroy();
    track = nullptr;
    note = 0;
   // @@0 create  title->destroy();
   // @@0 create  property->destroy();
    score.destroy();
    expanded = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::start(void)
{
    stop();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::stop(void)
{
    emphase.clear();
    m_vNotes.resize(0);
    m_vToStop.resize(0);
    m_vToPlay.resize(0);
    m_vEdition.resize(0);
    note = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::update(emp::tim::time_c const& a_rTime, emp::tim::time_c const& a_rDelta, c_state_e a_eState)
{
    score.set_time(emp::tim::time_c::to_seconds(a_rTime));
    if (a_eState == state_e::Play)
    {
        m_vToPlay.resize(0);
        note = ase::cor::get_notes(note, *track, a_rTime, emp::tim::time_c(0), m_vToPlay);

        m_vToStop.resize(m_vNotes.count());
        for (size_t stIndex = 0; stIndex < m_vNotes.count(); ++stIndex)
        { m_vToStop.set(stIndex, m_vNotes.ref(stIndex)); }

        m_vNotes.resize(0);

        size_t stTmp = 0;
        while (stTmp < m_vToPlay.count())
        {
            bool bFound = false;
            emp::snd::note_c const oTmp(m_vToPlay.ref(stTmp));
            c_size stPlayed = m_vToStop.count();
            for (size_t stNote = 0; stNote < stPlayed; ++stNote)
            {
                emp::snd::note_c const oNote(m_vToStop.ref(stNote));
                if (oNote == oTmp)
                {
                    bFound = true;
                    m_vToStop.remove_unordered(stNote);
                    m_vNotes.add(oNote);
                    break;
                }
            }
            if (bFound)
            {
                m_vToPlay.remove_unordered(stTmp);
                continue;
            }
            ++stTmp;
        }

        { c_size stCount = m_vToStop.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emphase.stop(m_vToStop.ref(stIndex).index);
        } }

        { c_size stCount = m_vToPlay.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::snd::note_c const& rNote = m_vToPlay.ref(stIndex);
            emphase.play(track->layout(), track->scale(), rNote.index, track->volume(), track->mute());
            m_vNotes.add(rNote);
        } }
    }
    else if (a_eState == state_e::Stop)
    {
        c_ui64 uiDelta = a_rDelta.usec();
        c_size stCount = m_vEdition.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            snd::note_c& rNote = m_vEdition.ref(stIndex);
            c_ui64 uiDuration = rNote.duration;
            c_ui64 uiNewDuration = uiDuration > uiDelta ? uiDuration - uiDelta : 0;
            if (uiNewDuration > 0)
            { emphase.stop(rNote.index); }
            rNote.duration = uiNewDuration;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::add_edition_note(emp::snd::note_c const& a_rNote)
{
    emphase.play(track->layout(), track->scale(), a_rNote.index, track->volume(), track->mute());
    m_vEdition.add(a_rNote);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool track_c::set_track(emp::snd::track_c* const a_pTrack, ase::cor::mapping_c const* const a_pMapping)
{
    track = a_pTrack;
    property.expanded_(true);
    EMP_RET_FALSE_IF_NOT(property.set_object(reflection));
    property.expanded_(false);
    EMP_RET_FALSE_IF_NOT(score.set_mapping(a_pMapping));
    EMP_RET_FALSE_IF_NOT(score.set_edition_track(a_pTrack));
    score.set_compute_beat_size(false);
    EMP_RET_FALSE_IF_NOT(set_enabled_rfx(enabled()));
    bool bExpanded = false;
    EMP_RET_FALSE_IF_NOT(get_expanded_rfx(bExpanded));
    EMP_RET_FALSE_IF_NOT(set_expanded_rfx(bExpanded));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_u8string& track_c::name(void) const
{ return title.m_su8Text; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_name(c_u8string& a_rsValue)
{ title.set_text(a_rsValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN snd::layout_i* track_c::layout(void) const
{ return track->layout(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_layout(emp::snd::layout_i* const a_pValue)
{ track->set_layout(a_pValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::snd::scale_i* track_c::scale(void) const
{ return track->scale(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_scale(emp::snd::scale_i* const a_pValue)
{ track->set_scale(a_pValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float track_c::beat_per_minute(void) const
{ return track->beat_per_minute(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_beat_per_minute(float const a_fValue)
{
    track->set_beat_per_minute(a_fValue);
    score.set_beat_per_minute(a_fValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float track_c::volume(void) const
{ return track->volume(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_volume(float const a_fValue)
{ track->set_volume(a_fValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool track_c::enabled(void) const
{ return track->mute() == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_enabled(c_bool a_bValue)
{ track->set_mute(a_bValue == false); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool track_c::set_enabled_rfx(c_bool& a_bValue)
{
    set_enabled(a_bValue);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool track_c::set_expanded_rfx(c_bool& a_bValue)
{
    expanded = a_bValue;
    property.state().visible_(a_bValue);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void track_c::set_skin(compose_skin_s const* const a_pValue)
{
    title.skin_(a_pValue ? a_pValue->title : nullptr);
    // @@0 spacer title_spacer->skin_(a_pValue ? a_pValue->spacer : nullptr);
    header.skin_(a_pValue ? a_pValue->header : nullptr);
    property.skin_(a_pValue ? a_pValue->property : nullptr);
    score.set_skin(a_pValue ? a_pValue->score : nullptr);
    picture_up.skin_(a_pValue ? a_pValue->picture_small : nullptr);
    picture_up.model_(a_pValue ? a_pValue->gfx_up : nullptr);
    // @@0 spacer picture_spacer->skin_(a_pValue ? a_pValue->spacer : nullptr);
    picture_down.skin_(a_pValue ? a_pValue->picture_small : nullptr);
    picture_down.model_(a_pValue ? a_pValue->gfx_down : nullptr);
    toggle_expanded.skin_(a_pValue ? a_pValue->expanded : nullptr);
    toggle_enabled.skin_(a_pValue ? a_pValue->enabled : nullptr);
    picture_remove.skin_(a_pValue?a_pValue->picture : nullptr);
    picture_remove.model_(a_pValue ? a_pValue->gfx_remove : nullptr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compose_c::compose_c(void):
    registrar(),
    list(),
    popup(),
    container(),
    toolbar(),
    m_vpTracks(),
    composition(),
    skin(),
    m_vpCompositions(),
    m_vpReflections(),
    m_Time(),
    m_bDragLeft(),
    m_bDragRight(),
    m_bLeftPressed(),
    m_bLeftReleased(),
    m_bLeftDoubleClick(),
    m_bRightPressed(),
    m_bRightReleased(),
    m_bRightDoubleClick(),
    m_Note(),
    m_v2DragOrigin(),
    m_v2DragDelta(),
    m_v2Offset(),
    m_eState(),
    note()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compose_c::~compose_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::create(registrar_i* const a_pRegistrar)
{
    destroy();

    registrar = a_pRegistrar;

    toolbar.push_on_focus_(false).state().dock_(dock_e::Top).navigable_(true).index_(0).autosize.xy(true);

    toolbar.add_item(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_new_track")),fct::function(this,&compose_c::on_new_track_pushed), nullptr);
    toolbar.add_item(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_play_track")),fct::function(this,&compose_c::on_play_pause_pushed),nullptr);
    toolbar.add_item(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_stop_track")), fct::function(this, &compose_c::on_stop_pushed), nullptr);

    container.state().index_(1);
    container.add_control(&toolbar);

    popup.create(a_pRegistrar, &list);
    popup.add_item(ALITERAL("ase::gui::compose_c::on_new_composition"), fct::function(this, &compose_c::on_new_composition));
    popup.add_item(ALITERAL("ase::gui::compose_c::on_rename_composition"), fct::function(this, &compose_c::on_rename_composition));
    popup.add_item(ALITERAL("ase::gui::compose_c::on_remove_composition"), fct::function(this, &compose_c::on_remove_composition));

    list.popup_(&popup).registrar_(registrar)
        .add_column_(emp::loc::string_s(ALITERAL(".compositions."), U8LITERAL(".compositions.")))
        .on_selection_changed_(fct::function(this, &compose_c::on_composition_changed))
        .state().navigable_(true).index_(0).autosize.x_(true);

    master_(&list).slave_(&container).orientation_(dock_e::Left)
        .state().index_(0).dock_(dock_e::Fill);
    redock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::destroy(void)
{
    // @@0 remove ASE_GAME.set_layout(nullptr);
    // @@0 remove ASE_GAME.set_scale(nullptr);

    registrar = nullptr;

    // @@0 create list->destroy();
    popup.destroy();

    toolbar.destroy();

    foreach(m_vpTracks, emp::mem::safe_delete_reset_t());
    m_vpTracks.resize(0);

    composition = nullptr;
    m_vpCompositions.resize(0);

    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    m_Time = emp::tim::time_c(0);

    m_bDragLeft = false;
    m_bDragRight = false;

    m_bLeftPressed = false;
    m_bLeftReleased = false;
    m_bLeftDoubleClick = false;

    m_bRightPressed = false;
    m_bRightReleased = false;
    m_bRightDoubleClick = false;

    m_v2DragOrigin = v2f::zero();
    m_v2DragDelta = v2f::zero();

    EMP_TT_MAX_VAR(note);
    m_Note = emp::snd::note_c();
    m_v2Offset = v2f::zero();

    skin = nullptr;

    m_eState = state_e::Stop;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::update(emp::tim::clock_c const& a_rClock)
{
    if (state().visible == false)
    { return true; }

    if (m_eState == state_e::Play)
    { m_Time = emp::tim::time_c::add(m_Time, a_rClock.delta()); }
    track_c* const pTrack = get_active_track();
    ASE_GAME.set_layout(pTrack ? pTrack->track->layout() : nullptr);
    ASE_GAME.set_scale(pTrack ? pTrack->track->scale() : nullptr);
    if (pTrack)
    {

        snd::note_c oPreviousNote = emp::tt::is_not_max(note) ? pTrack->track->notes().ref(note) : emp::snd::note_c();

        c_v2f v2Position = v2f::add(m_v2DragOrigin, m_v2DragDelta);

        score_c* const pScore = pTrack ? &pTrack->score : nullptr;
        if (m_bLeftDoubleClick ||
            m_bRightDoubleClick)
        {
            c_size stNote = note;
            note = pScore->get_note(emp::tt::max<size_t>::value, v2Position);
            if (emp::tt::is_not_max(stNote) &&
                stNote == note)
            {
                pScore->remove_note(note);
                EMP_TT_MAX_VAR(note);
            }
            m_bLeftDoubleClick = false;
            m_bRightDoubleClick = false;
        }

        if (m_bDragLeft)
        {
            if (m_bLeftPressed)
            {
                note = pScore->get_note(emp::tt::max<size_t>::value, v2Position);
                if (emp::tt::is_max(note))
                {
                    note = pScore->add_note(v2Position);
                    oPreviousNote = pTrack->track->notes().ref(note);
                    pTrack->add_edition_note(oPreviousNote);
                }
                m_v2Offset = v2f::sub(v2Position, pScore->get_note_position(note));
                m_bLeftPressed = false;
            }
            else if (m_bLeftReleased)
            { m_bLeftReleased = false; }
            else
            { note = pScore->move_note(note, m_v2Offset, v2Position); }
        }
        else if (m_bDragRight)
        {
            if (m_bRightPressed)
            {
                note = pScore->get_note(emp::tt::max<size_t>::value, v2Position);
                if (emp::tt::is_max(note))
               { note = pScore->add_note(v2Position); }
                m_bRightPressed = false;
            }
            else if (m_bRightReleased)
            { m_bRightReleased = false; }
            else
            { note = pScore->scale_note(note, v2Position); }
        }
        emp::snd::note_c const oCurrentNote = emp::tt::is_not_max(note) ? pTrack->track->notes().ref(note) : emp::snd::note_c();

        if (oCurrentNote.index != oPreviousNote.index &&
            oCurrentNote.index != emp::tt::max<ui32>::value)
        { pTrack->add_edition_note(oCurrentNote); }
    }

    { c_size stCount = m_vpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpTracks.val(stIndex)->update(m_Time, a_rClock.delta(), m_eState); } }
    return tbase1::update(a_rClock);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_float fWidth = container.transformation().width();

    EMP_RET_FALSE_IF_NOT(toolbar.resize());
    toolbar.transformation().position_(0).width_(fWidth);

    c_float fSX = EMP_GUI_SKIN.spacing_x();//skin->spacer->spacing->x();
    c_float fSY = EMP_GUI_SKIN.spacing_y();//skin->spacer->spacing->y();
    float fY = 0;

    fY -= toolbar.transformation().height();
    fY -= fSY;

    c_size stCount = m_vpTracks.count();

    float fMaxX = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        track_c* const pTrack = m_vpTracks.val(stIndex);

        pTrack->title.state().dock_(dock_e::Left);
        pTrack->title_spacer.state().dock_(dock_e::Left);
        pTrack->picture_remove.state().dock_(dock_e::Left);

        docker_c* const pHeader = &pTrack->header;
        pHeader->state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(pHeader->resize());
        fMaxX = emp::tt::maxof(fMaxX, pHeader->transformation().width());
        pHeader->state().autosize.x_(false);

        pTrack->title.state().dock_(dock_e::Fill);
        pTrack->picture_remove.state().dock_(dock_e::Right);
        pTrack->title_spacer.state().dock_(dock_e::Right);

        property_c* const pProperty = &pTrack->property;
        pProperty->transformation().width_(0);
        pProperty->state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(pProperty->resize());
        fMaxX = emp::tt::maxof(fMaxX, pProperty->transformation().width());
        pProperty->state().autosize.x_(false);

        score_c* const pScore = &pTrack->score;
        pScore->state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(pScore->resize());
        pScore->state().autosize.x_(false);
    }

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        float fMaxY = 0;

        track_c* const pTrack = m_vpTracks.val(stIndex);

        docker_c* const pHeader = &pTrack->header;
        pHeader->transformation().width_(fMaxX).position_(0, fY - fMaxY);
        fMaxY += pHeader->transformation().height();
        fMaxY += fSY;

        property_c* const pProperty = &pTrack->property;
        if (pProperty->state().visible)
        {
            pProperty->transformation().width_(fMaxX).position_(0, fY - fMaxY);
            EMP_RET_FALSE_IF_NOT(pProperty->resize());
            fMaxY += pProperty->transformation().height();
            fMaxY += fSY;
        }

        score_c* const pScore = &pTrack->score;
        c_float fScoreX = fMaxX + fSX;
        pScore->transformation().width_(fWidth - fScoreX).position_(fScoreX, fY);
        fY -= emp::tt::maxof(fMaxY, pScore->transformation().height() + fSY);
    }

    return tbase1::resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void scale_up_y(vector<track_c*> const& a_rvpTracks);
EMP_INTERN void scale_up_y(vector<track_c*> const& a_rvpTracks)
{
    c_size stCount = a_rvpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_rvpTracks.val(stIndex)->score.scale_up_y(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void scale_down_y(vector<track_c*> const& a_rvpTracks);
EMP_INTERN void scale_down_y(vector<track_c*> const& a_rvpTracks)
{
    c_size stCount = a_rvpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_rvpTracks.val(stIndex)->score.scale_down_y(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void time_scale_up(vector<track_c*> const& a_rvpTracks);
EMP_INTERN void time_scale_up(vector<track_c*> const& a_rvpTracks)
{
    c_size stCount = a_rvpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_rvpTracks.val(stIndex)->score.time_scale_up(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void time_scale_down(vector<track_c*> const& a_rvpTracks);
EMP_INTERN void time_scale_down(vector<track_c*> const& a_rvpTracks)
{
    c_size stCount = a_rvpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_rvpTracks.val(stIndex)->score.time_scale_down(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false ||
        state().enable == false ||
        focus_in_hierarchy(this) == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;
    app::c_key_e eKey = rKeyboard.m_eKey;

    c_bool bPressed = rKeyboard.m_bPressed;
    c_bool bAlt = rKeyboard.m_bAlt;

    if (bPressed && bAlt)
    {
        if (eKey == app::key_e::Left)
        {
            time_scale_down(m_vpTracks);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eKey == app::key_e::Right)
        {
            time_scale_up(m_vpTracks);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eKey == app::key_e::Up)
        {
            scale_down_y(m_vpTracks);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eKey == app::key_e::Down)
        {
            scale_up_y(m_vpTracks);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Unused);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    auto const& rMouse = a_rEvent.mouse;
    app::c_button_e eButton = rMouse.m_eButton;

    c_bool bEnable = state().enable;
    c_bool bContains = state().hover;

    c_bool bCtrl = a_rEvent.keyboard.m_bCtrl;
    c_bool bPressed = a_rEvent.mouse_pressed();
    c_bool bReleased = a_rEvent.mouse_released();
    c_bool bDragged = a_rEvent.mouse_dragged();

    c_size stTapCount = rMouse.m_stTapCount;

    c_v2f v2RelPosition = rMouse.m_v2fPosition;
    if ((m_vpTracks.count() > 0) == false)
    { return tbase1::process_mouse(a_rEvent); }

    {
        v2f::type const v2Client = gl_to_gui(v2RelPosition, container.transformation().matrix);

        c_float fHeight = transformation().height();
        c_float fClientX = v2Client.x;
        c_float fClientY = fHeight + v2Client.y;

        track_c const* const pTrack = m_vpTracks.first_val();
        if (fClientX < pTrack->score.transformation().x() ||
            fClientY < pTrack->score.transformation().y())
        { return tbase1::process_mouse(a_rEvent); }
    }

    m_bLeftPressed = false;
    m_bLeftReleased = false;

    emp::tim::time_c const oDeltaTime(emp::tim::time_c::from_seconds(0.2f));
    if (eButton == emp::app::button_e::WheelUp)
    {
        if (bEnable && bContains && bPressed)
        {
            if (bCtrl)
            { time_scale_up(m_vpTracks); }
            else
            {
                if (emp::tim::time_c::more_equal(m_Time, oDeltaTime))
                { m_Time = emp::tim::time_c::sub(m_Time, oDeltaTime); }
            }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    else if (eButton == emp::app::button_e::WheelDown)
    {
        if (bEnable && bContains && bPressed)
        {
            if (bCtrl)
            { time_scale_down(m_vpTracks); }
            else
            { m_Time = emp::tim::time_c::add(m_Time, oDeltaTime); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    else if (eButton == emp::app::button_e::Left)
    {
        if (stTapCount == 1)
        {
            if (bContains)
            {
                if (bPressed)
                {
                    if (m_bDragLeft == false)
                    {
                        m_bDragLeft = true;
                        m_v2DragOrigin = v2RelPosition;
                        m_v2DragDelta = v2f::zero();
                        m_bLeftPressed = true;
                    }

                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                else if (bReleased)
                {
                    if (m_bDragLeft)
                    {
                        m_bDragLeft = false;
                        m_v2DragOrigin = v2f::add(m_v2DragOrigin, m_v2DragDelta);
                        m_v2DragDelta = v2f::zero();
                        m_bLeftReleased = true;
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
                else if (bDragged)
                {
                    if (m_bDragLeft)
                    {
                        m_v2DragDelta = v2f::sub(v2RelPosition, m_v2DragOrigin);
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
            }
            else
            {
                if (m_bDragLeft)
                {
                    m_bDragLeft = false;
                    m_v2DragDelta = v2f::zero();
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }
        else if (stTapCount == 2 && bPressed && bContains)
        {
            m_bLeftDoubleClick = true;
        }
    }
    else if (eButton == emp::app::button_e::Right)
    {
        if (stTapCount == 1)
        {
            if (bContains)
            {
                if (bPressed)
                {
                    if (m_bDragRight == false)
                    {
                        m_bDragRight = true;
                        m_v2DragOrigin = v2RelPosition;
                        m_v2DragDelta = v2f::zero();
                        m_bRightPressed = true;
                    }

                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                else if (bReleased)
                {
                    if (m_bDragRight)
                    {
                        m_bDragRight = false;
                        m_v2DragOrigin = v2f::add(m_v2DragOrigin, m_v2DragDelta);
                        m_v2DragDelta = v2f::zero();
                        m_bRightReleased = true;
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
                else if (bDragged)
                {
                    if (m_bDragRight)
                    {
                        m_v2DragDelta = v2f::sub(v2RelPosition, m_v2DragOrigin);
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
            }
            else
            {
                if (m_bDragRight)
                {
                    m_bDragRight = false;
                    m_v2DragDelta = v2f::zero();
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }
        else if (stTapCount == 2 && bPressed && bContains)
        {
            m_bRightDoubleClick = true;
        }
    }

    return tbase1::process_mouse(a_rEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::add_track(emp::snd::track_c* const a_pTrack)
{
    track_c* const pTrack = EMP_ALLOCATE(track_c);
    EMP_RET_FALSE_IF_NOT(pTrack->create(&container, registrar, m_vpTracks.count() * 3 + 1));
    pTrack->picture_up.on_push.function_(fct::function(this, &compose_c::on_move_up_track));
    pTrack->picture_down.on_push.function_(fct::function(this, &compose_c::on_move_down_track));
    pTrack->picture_remove.on_push.function_(fct::function(this, &compose_c::on_remove_track));
    pTrack->set_skin(skin);
    EMP_RET_FALSE_IF_NOT(pTrack->set_track(a_pTrack, ASE_GAME.user().options.emphase.keyboard.mapping));
    pTrack->set_name(a_pTrack->name());
    if (m_vpTracks.count() > 0)
    {
        track_c const* const pFirst = m_vpTracks.first_val();
        score_c const* const pScore = &pFirst->score;
        pTrack->score.set_time_scale(pScore->time_scale());
        pTrack->score.set_scale_x(pScore->scale_x());
        pTrack->score.set_scale_y(pScore->scale_y());
    }
    else
    { pTrack->score.set_scale_y(0.3f); }

    m_vpTracks.add(pTrack);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::set_value(c_size a_stIndex, c_u8string& a_rsu8Value)
{
    snd::composition_c* const pComposition = m_vpCompositions.val(a_stIndex);
    pComposition->set_name(a_rsu8Value);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::get_value(c_size a_stIndex, u8string& a_rsu8Value)
{
    snd::composition_c const* const pComposition = m_vpCompositions.val(a_stIndex);
    a_rsu8Value = pComposition->name();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::add_composition(emp::snd::composition_c* const a_pComposition, c_change_e EMP_SILENT(a_eChange))
{
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpCompositions.count());
    m_vpCompositions.add(a_pComposition);

    rfx::type_t<u8string>* const pType = EMP_ALLOCATE(rfx::type_t<u8string>,
        bind(fct::function(this, &compose_c::get_value), ui32Index),
        bind(fct::function(this, &compose_c::set_value), ui32Index));

    m_vpReflections.add(pType);

    line_c* const pLine = list.new_line(ui32Index);
    list.add_item(nullptr, pLine, emp::rfx::field_s(pType), nullptr);
    list.insert_line(ui32Index, pLine);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::set_skin(cpc_compose_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->splitter : nullptr);
    popup.skin_(a_pValue ? a_pValue->popup : nullptr);
    list.skin_(a_pValue ? a_pValue->list : nullptr);
    container.skin_((a_pValue && a_pValue->docker) ? a_pValue->docker->container : nullptr);
    toolbar.skin_(a_pValue ? a_pValue->toolbar : nullptr);
    c_size stCount = m_vpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpTracks.val(stIndex)->set_skin(a_pValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN track_c* compose_c::get_active_track(void)
{
    c_size stCount = m_vpTracks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        track_c* const pTrack = m_vpTracks.val(stIndex);
        if (pTrack->score.state().hover)
        { return pTrack; }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::populate_composition_list(c_ui32 a_ui32Selection, c_change_e a_eChange)
{
    m_vpCompositions.resize(0);
    list.clear();

    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    auto* const pStore = EMP_STORE(snd::composition_c);
    c_ui32 ui32Count = emp::ast::cast<ui32>(pStore->object_count());
    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    { add_composition(pStore->get_object(ui32Index), a_eChange); }

    if (a_ui32Selection < ui32Count)
    { list.set_selection(a_ui32Selection + 1, 0, true, a_eChange); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_new_composition(c_size EMP_SILENT(a_stIndex), c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue)
    {
        popup.hide();

        auto* const pComposition = EMP_STORE(snd::composition_c)->new_object_pattern(ALITERAL(""));
        pComposition->set_name(U8LITERAL("__no_name__"));
        add_composition(pComposition, a_eChange);
        list.set_selection(emp::ast::cast<ui32>(m_vpCompositions.count()), 0, true, a_eChange);
        return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_rename_composition(c_size EMP_SILENT(a_stIndex), c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue)
    {
        popup.hide();
        emp::tt::silent(list.on_edit_push(nullptr, false, a_eChange));
        return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_remove_composition(c_size EMP_SILENT(a_stIndex), c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        popup.hide();
        return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::on_composition_changed(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e EMP_SILENT(a_eChange))
{
    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    container.clear_controls();
    container.add_control(&toolbar);

    foreach(m_vpTracks, emp::mem::safe_delete_reset_t());
    m_vpTracks.resize(0);

    composition = m_vpCompositions.val(a_ui32Line - 1);
    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(composition));

    c_size stCount = composition->track_count();
    if (stCount)
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::snd::track_c* const pTrack = composition->track(stIndex);
            EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(pTrack));
            EMP_RET_FALSE_IF_NOT(add_track(pTrack));
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_new_track_pushed(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue == false &&
        composition != nullptr)
    {
        auto& rFactory = EMP_FACTORY;
        auto* const pTrack = rFactory.store<emp::snd::track_c>()->new_object_pattern(ALITERAL(""));
        pTrack->set_name(U8LITERAL("__no_name__"));
        pTrack->set_layout(rFactory.store<snd::soft_synth_c>()->get_object(0u));        // @@0 hardcode
        pTrack->set_scale(rFactory.store<emp::snd::scale_c>()->get_object(ALITERAL("chromatic.xml")));         // @@0 hardcode
        pTrack->set_beat_per_minute(composition->track_count() > 0 ? composition->beat_per_minute() : 128);
        composition->add_track(pTrack);
        EMP_RET_X_IF_NOT(return_e::Error, add_track(pTrack));
    }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_play_pause_pushed(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        if (m_eState == state_e::Play)
        {
            m_eState = state_e::Pause;
            toolbar.icons().val(0)->state().enable_(false);
            toolbar.icons().val(1)->label.set_text(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_play_track")));
        }
        else if (m_eState == state_e::Pause)
        {
            m_eState = state_e::Play;
            toolbar.icons().val(0)->state().enable_(false);
            toolbar.icons().val(1)->label.set_text(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_pause_track")));
        }
        else if (m_eState == state_e::Stop)
        {
            m_eState = state_e::Play;
            m_Time = emp::tim::time_c(0);
            toolbar.icons().val(0)->state().enable_(false);
            toolbar.icons().val(1)->label.set_text(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_pause_track")));
        }
    }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_stop_pushed(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        m_eState = state_e::Stop;
        m_Time = emp::tim::time_c(0);
        toolbar.icons().val(0)->state().enable_(true);
        toolbar.icons().val(1)->label.set_text(emp::loc::string_s(ALITERAL("ase::gui::compose_c::on_play_track")));
        c_size stCount = m_vpTracks.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpTracks.val(stIndex)->stop(); }
    }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_remove_track(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue && a_pSender)
    {
        c_size stCount = m_vpTracks.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (control_in_hierarchy(&m_vpTracks.val(stIndex)->header, a_pSender))
            {
                EMP_LOG_INFO("REMOVE_TRACK #0", stIndex);
                m_vpTracks.remove(stIndex);
                composition->remove_track(stIndex);
                EMP_RET_X_IF_NOT(return_e::Error, on_composition_changed(list.selection_line(), 0, a_eChange));
                break;
            }
        }
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_move_up_track(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue && a_pSender)
    {
        c_size stCount = m_vpTracks.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (control_in_hierarchy(&m_vpTracks.val(stIndex)->header, a_pSender))
            {
                if (stIndex > 0)
                {
                    c_size stUp = stIndex - 1;
                    composition->swap_tracks(stIndex, stUp);
                    EMP_RET_X_IF_NOT(return_e::Error, on_composition_changed(list.selection_line(), 0, a_eChange));
                    break;
                }
            }
        }
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e compose_c::on_move_down_track(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue && a_pSender)
    {
        c_size stCount = m_vpTracks.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (control_in_hierarchy(&m_vpTracks.val(stIndex)->header, a_pSender))
            {
                c_size stDown = stIndex + 1;
                if (stDown < stCount)
                {
                    composition->swap_tracks(stIndex, stDown);
                    EMP_RET_X_IF_NOT(return_e::Error, on_composition_changed(list.selection_line(), 0, a_eChange));
                    break;
                }
            }
        }
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compose_c::compose_c(void) :
m_pCtrlCompose()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compose_c::~compose_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::public_create(void)
{
    m_pCtrlCompose->create(&registrar());
    m_pCtrlCompose->set_skin(ASE_GAME.skin().compose.get(ALITERAL("cmp.")));
    m_pCtrlCompose->state().dock_(emp::gui::dock_e::Fill);
    m_pCtrlCompose->populate_composition_list(0, emp::gui::change_e::Code);
    window().add_control(m_pCtrlCompose);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::public_destroy(void)
{ m_pCtrlCompose->destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::public_start(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compose_c::public_stop(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::public_update(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compose_c::condition_satisfied(void) const
{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

