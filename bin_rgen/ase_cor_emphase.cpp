//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_cor_emphase.h"

#include "ase_sol_music.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

#include "emp_snd_media.h"
#include "emp_snd_player.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"
#include "emp_gfx_reflection.rfx.h"
#include "emp_snd_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::rfx;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN note_state_e is_valid_note(
    emp::snd::note_c const& a_rNote,
    emp::tim::time_c const& a_rTime,
    emp::tim::time_c const& a_rTolerance)
{
    emp::tim::time_c const oTime(a_rNote.time);
    emp::tim::time_c const oDuration(a_rNote.duration);

    emp::tim::time_c oMinTime;
    if (emp::tim::time_c::more_equal(a_rTime, a_rTolerance))
    { emp::tim::time_c::sub(a_rTime, a_rTolerance, oMinTime); }

    emp::tim::time_c oMaxTime;
    emp::tim::time_c::add(a_rTime, a_rTolerance, oMaxTime);

    if (emp::tim::time_c::less(oMaxTime, oTime))
    { return note_state_e::After; }

    if (emp::tim::time_c::equal(oDuration, emp::tim::time_c()))
    { return note_state_e::Current; }

    emp::tim::time_c oEndTime;
    emp::tim::time_c::add(oTime, oDuration, oEndTime);

    if (emp::tim::time_c::more(oMaxTime, oEndTime))
    { return note_state_e::Before; }
    return note_state_e::Current;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t get_notes(
    c_size a_stOffset,
    emp::snd::track_c const& a_rTrack,
    emp::tim::time_c const& a_rTime,
    emp::tim::time_c const& a_rTolerance,
    emp::cnt::vector<emp::snd::note_c>& a_rvNotes)
{
    a_rvNotes.resize(0);

    emp::cnt::vector<emp::snd::note_c> const& rvNotes = a_rTrack.notes();
    c_size stNoteCount = rvNotes.count();
    size_t stNote = a_stOffset;
    size_t stOffset = a_stOffset;
    bool bFound = false;
    while (true)
    {
        if (stNote >= stNoteCount)
        { break; }

        emp::snd::note_c const oNote(rvNotes.ref(stNote));
        c_note_state_e eNote = is_valid_note(oNote, a_rTime, a_rTolerance);
        if (eNote == note_state_e::Before)
        { }
        else if (eNote == note_state_e::After)
        { break; }
        else
        {
            emp::snd::note_c& rCopy = a_rvNotes.push();
            rCopy = oNote;

            emp::tim::time_c oDuration(oNote.duration);
            if (emp::tim::time_c::equal(oDuration, emp::tim::time_c()))
            {
                emp::tim::time_c const oTime(oNote.time);
                emp::tim::time_c::sub(a_rTime, oTime, oDuration);
            }
            rCopy.duration = oDuration.usec();

            if (bFound == false)
            {
                stOffset = stNote;
                bFound = true;
            }
        }
        ++stNote;
    }
    return stOffset;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
composition_music_c::composition_music_c(void):
    m_Performance(),
    m_pComposition(),
    m_pTrack(),
    m_fScale(1.0f),
    m_bBack(false),
    m_bChanged(false),
    m_Translate()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
composition_music_c::~composition_music_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool composition_music_c::post_load(void)
{
    build_data();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void composition_music_c::destroy(void)
{
    m_Performance.clear();
    m_pComposition = nullptr;
    m_pTrack->destroy_track();
    m_fScale = 1.0f;
    m_Translate = emp::tim::time_c();
    m_bBack = false;
    m_bChanged = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::str::c_u8string& composition_music_c::name(void) const
{ return m_pComposition->name(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void composition_music_c::set_played(user_c* const a_pUser, emp::tim::time_c const& a_rTime, c_completion_data_s& a_rCompletion)
{
    user_data_s* const pUserData = m_Performance.get_user_data(a_pUser);
    ++(pUserData->played);

    completion_data_s oCompletionData(a_rCompletion);
    oCompletionData.user = a_pUser;
    oCompletionData.time = a_rTime.usec();

    m_Performance.add(a_pUser->options, oCompletionData);

    m_fScale = 1.0f;
    m_Translate = emp::tim::time_c();
    m_bBack = false;
    m_bChanged = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float composition_music_c::beat_per_minute(void) const
{ return m_pComposition->beat_per_minute(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void composition_music_c::scale(c_float a_fFactor)
{
    if (emp::tt::equal_(a_fFactor, 1.0f) == false)
    {
        m_fScale = a_fFactor;
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void composition_music_c::translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack)
{
    if (emp::tim::time_c::equal(m_Translate, a_rDelta) == false ||
        m_bBack != a_bBack)
    {
        m_Translate = a_rDelta;
        m_bBack = a_bBack;
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void composition_music_c::build_copy(void)
{
    if (m_bChanged)
    {
        m_pComposition->merge(*m_pTrack);
        m_pTrack->translate(m_Translate, m_bBack);
        m_pTrack->scale(m_fScale);
        m_bChanged = false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void composition_music_c::build_data(void)
{ m_pComposition->merge(*m_pTrack); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
random_music_c::random_music_c(void) :
    m_pComposition(),
    m_pTrack(),
    m_bFullTime(),
    m_bBack(false),
    m_bChanged(false),
    m_su8Name(),
    m_Performance(),
    m_ui32MaxTouches(),
    m_ui32MaxLeftTouches(),
    m_ui32MaxRightTouches(),
    m_fBeatPerMinute(),
    m_fMaxDuration(),
    m_fScale(1),
    m_Translate()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
random_music_c::~random_music_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool random_music_c::post_load(void)
{
    build_data();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void random_music_c::set_played(user_c* const a_pUser, emp::tim::time_c const& a_rTime, c_completion_data_s& a_rCompletion)
{
    user_data_s* const pUserData = m_Performance.get_user_data(a_pUser);
    ++(pUserData->played);

    completion_data_s oCompletionData(a_rCompletion);
    oCompletionData.user = a_pUser;
    oCompletionData.time = a_rTime.usec();

    m_Performance.add(a_pUser->options, oCompletionData);

    m_fScale = 1.0f;
    m_Translate = emp::tim::time_c();
    m_bBack = false;
    m_bChanged = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void random_music_c::scale(c_float a_fFactor)
{
    if (emp::tt::equal_(a_fFactor, 1.0f) == false)
    {
        m_fScale = a_fFactor;
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void random_music_c::translate(emp::tim::time_c const& a_rDelta, c_bool a_bBack)
{
    if (emp::tim::time_c::equal(m_Translate, a_rDelta) == false ||
        m_bBack != a_bBack)
    {
        m_Translate = a_rDelta;
        m_bBack = a_bBack;
        m_bChanged = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void random_music_c::build_copy(void)
{
    if (m_bChanged)
    {
        m_pTrack->copy(*m_pComposition);
        m_pTrack->translate(m_Translate, m_bBack);
        m_pTrack->scale(m_fScale);
        m_bChanged = false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void random_music_c::build_data(void)
{
    m_pComposition->destroy_track();
    m_pComposition->set_beat_per_minute(m_fBeatPerMinute);

    c_float fBeatDuration = 60.0f / m_fBeatPerMinute;

    c_float fMaxDuration = m_fMaxDuration;

    c_size stMaxTouches = m_ui32MaxTouches;
    c_size stMaxLeftTouches = m_ui32MaxLeftTouches;
    c_size stMaxRightTouches = m_ui32MaxRightTouches;

    c_float fDuration = fBeatDuration * 0.64f;

    // @@0 fixme bug mod decrypt si meme temps
    float fLeftTime = fDuration;
    float fRightTime = 0;

    bool bFullTime = m_bFullTime;

    emp::cnt::vector<emp::snd::note_c>& rvNotes = m_pComposition->notes();

    emp::mat::random_c& rRandom = ASE_GAME.random();

    while (fLeftTime < fMaxDuration && fRightTime < fMaxDuration)
    {
        c_size stCount = bFullTime ? stMaxTouches : rRandom.rand<size_t>(0, stMaxTouches + 1);
        c_size stFirstHandCount = rRandom.rand<size_t>(0, stMaxTouches + 1);
        c_size stLastHandCount = stCount > stFirstHandCount ? stCount - stFirstHandCount : 0;

        c_bool bHand = rRandom.rand<ui32>(0, 1) == 1;

        c_size stLeftCount = emp::tt::minof(bHand ? stFirstHandCount : stLastHandCount, stMaxLeftTouches);
        c_size stRightCount = emp::tt::minof(bHand ? stLastHandCount : stFirstHandCount, stMaxRightTouches);

        size_t stSum = stCount;

        // @@0 fixme bug mod decrypt si meme temps
        if (emp::mat::abs(fLeftTime - fRightTime) < 0.1f)
        { fLeftTime += 0.1f; }

        c_float fLT = fLeftTime;
        c_float fRT = fRightTime;

        auto const l_fill_hand = [&rRandom, &stSum, fDuration, &rvNotes](c_size a_stMaxHand, c_float a_fTime, c_bool a_bRight)
        {
            array<bool, 5, true> abUsed(true);
            for (size_t stIndex = 0; stIndex < 5; ++stIndex)
            { abUsed.set(stIndex, false); }

            size_t stMaxHand = a_stMaxHand;

            while (stSum > 0 && stMaxHand)
            {
                ui32 ui32Finger = 0;

                c_ui32 x = rRandom.rand<ui32>(0, 5);
                c_ui32 ui32Count = emp::ast::cast<ui32>(abUsed.count());
                for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
                {
                    ui32Finger = (x + ui32Index) % ui32Count;
                    if (abUsed.val(ui32Finger) == false)
                    {
                        abUsed.set(ui32Finger, true);
                        break;
                    }
                }

                ui32 ui32Emphasis = rRandom.rand<ui32>(0, 4) * 10 + ui32Finger + (a_bRight ? 5 : 0);

                emp::tim::time_c const oTime(emp::tim::time_c::from_seconds(a_fTime));
                emp::tim::time_c const oDuration(emp::tim::time_c::from_seconds(fDuration));

                rvNotes.add(emp::snd::note_c(ui32Emphasis, oTime.usec(), oDuration.usec()));

                --stMaxHand;
                --stSum;
            }
        };

        if (fLeftTime < fRightTime)
        {
            l_fill_hand(stLeftCount, fLT, false);
            l_fill_hand(stRightCount, fRT, true);
        }
        else if (fLeftTime > fRightTime)
        {
            l_fill_hand(stRightCount, fRT, true);
            l_fill_hand(stLeftCount, fLT, false);
        }
        else if (bHand)
        {
            l_fill_hand(stLeftCount, fLT, false);
            l_fill_hand(stRightCount, fRT, true);
        }
        else
        {
            l_fill_hand(stRightCount, fRT, true);
            l_fill_hand(stLeftCount, fLT, false);
        }

        fLeftTime += fBeatDuration;
        fRightTime += fBeatDuration;
    }

    m_pTrack->copy(*m_pComposition);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
emphase_c::emphase_c(void):
m_papMedias(true),
m_fCurrentFrequency(1.0f)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
emphase_c::~emphase_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::create(void)
{
    destroy();

    m_papMedias->resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::destroy(void)
{ clear(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::clear(void)
{
    c_size stCount = m_papMedias->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { stop(stIndex); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::translate(
    emp::app::c_keyboard_event_s& a_rEvent,
    emp::tim::time_c const& a_rTime,
    emp::snd::track_c& a_rTrack,
    matrix_c& a_rMatrix,
    mapping_c const& a_rMapping,
    c_bool a_bPlay)
{

    c_ui64 cChar = a_rEvent.m_ui64Char;
    #if defined EMP_XX_LOG
    c_ui32 ui32Code = a_rEvent.m_ui32Code;
    #endif

#ifdef EMP_XX_LOUSY
    EMP_RET_FALSE_IF(cChar == 0, "Invalid key a : 0x#0 (#1)", INT_FMT_0H(ui32Code), ui32Code);
#else
    if (cChar == 0)
    { return false; } 
#endif

    ui32 EMP_TT_MAX_VAR(ui32Emphasis);
    EMP_RET_FALSE_IF(a_rMapping.keycode_to_emphasis(a_rEvent.m_eKey, ui32Emphasis) == false ||
        ui32Emphasis >= emp::snd::constants::key_count,
        "Invalid key b : 0x#0 (#1), #2", INT_FMT_0H(ui32Code), ui32Code, cChar);

    EMP_RET_FALSE_IF(a_rEvent.m_bRepeat);

    emp::cnt::vector<emp::snd::note_c>& rvNotes = a_rTrack.notes();

    c_size stAddress = a_rMatrix.get_address(ui32Emphasis);

    emp::tim::time_c const& oEventTime = a_rTime;

    if (a_rEvent.m_bPressed)
    {
        c_size stNote = rvNotes.count();
        rvNotes.add(emp::snd::note_c(ui32Emphasis, oEventTime.usec(), 0));

        if (a_bPlay)
        { play(a_rTrack.layout(), a_rTrack.scale(), ui32Emphasis, a_rTrack.volume(), a_rTrack.mute()); }

        a_rMatrix.set_address(ui32Emphasis, stNote);
        return true;
    }
    else
    {
        if (emp::tt::is_not_max(stAddress))
        {
            emp::snd::note_c& rCurrentNote = rvNotes.ref(stAddress);
            EMP_RET_FALSE_IF_NOT(rCurrentNote.index == ui32Emphasis);

            emp::tim::time_c const oTime(rCurrentNote.time);
            emp::tim::time_c oDuration;
            emp::tim::time_c::sub(oEventTime, oTime, oDuration);

            rCurrentNote.duration = emp::tt::maxof<ui64>(oDuration.usec(),16000u);

            a_rMatrix.set_address(ui32Emphasis, emp::tt::max<size_t>::value);

            if (a_bPlay)
            { stop(ui32Emphasis); }

            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void dump_note(ase::sol::note_c const& a_rSol, c_ui32 EMP_XX_IF_LOG(a_ui32Emphasis), ase::cor::mapping_c const* const EMP_XX_IF_LOG(a_pMapping),
    c_size EMP_XX_IF_LOG(a_stIndex));
EMP_INTERN void dump_note(ase::sol::note_c const& a_rSol, c_ui32 EMP_XX_IF_LOG(a_ui32Emphasis), ase::cor::mapping_c const* const EMP_XX_IF_LOG(a_pMapping),
    c_size EMP_XX_IF_LOG(a_stIndex))
{
#define XTMP_LOG(x_From, x_Value)\
    if (x_From == x_Value)\
    { EMP_LOG_INFO(EMP_PP_STRING(x_Value)); }

    EMP_LOG_INFO("--------------------------------------------------------");
    EMP_LOG_INFO("--index : #0, emphasis : #1, char : #2", a_stIndex, a_ui32Emphasis, a_pMapping->emphasis_to_keychar(a_ui32Emphasis));
    EMP_LOG_INFO("--------------------------------------------------------");
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::none)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::Do)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::Re)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::Mi)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::Fa)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::Sol)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::La)
    XTMP_LOG(a_rSol.hauteur, ase::sol::hauteur_e::Si)

    XTMP_LOG(a_rSol.alteration, ase::sol::alteration_e::Bemol)
    XTMP_LOG(a_rSol.alteration, ase::sol::alteration_e::DoubleBemol)
    XTMP_LOG(a_rSol.alteration, ase::sol::alteration_e::Diese)
    XTMP_LOG(a_rSol.alteration, ase::sol::alteration_e::DoubleDiese)

    XTMP_LOG(a_rSol.octave, ase::sol::octave_e::n0)
    XTMP_LOG(a_rSol.octave, ase::sol::octave_e::n1)
    XTMP_LOG(a_rSol.octave, ase::sol::octave_e::n2)
    XTMP_LOG(a_rSol.octave, ase::sol::octave_e::n3)

    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::none)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::Carree)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::Ronde)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::Blanche)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::Noire)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::Croche)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::DoubleCroche)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::TripleCroche)
    XTMP_LOG(a_rSol.duree, ase::sol::duree_e::QuadrupleCroche)

    XTMP_LOG(a_rSol.pointee, ase::sol::pointee_e::True)

#undef XTMP_LOG
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool build_track(
    emp::snd::track_c* const a_pTrack, ase::sol::music_c const& a_rMusic,
    emp::cnt::vector<ase::sol::note_c> const& a_rvNotes,
    emp::cnt::vector<ase::sol::note_c> const& a_rvAlterations,
    ase::cor::mapping_c const* const a_pMapping);
EMP_INTERN EMP_RETURN bool build_track(
    emp::snd::track_c* const a_pTrack, ase::sol::music_c const& a_rMusic,
    emp::cnt::vector<ase::sol::note_c> const& a_rvNotes,
    emp::cnt::vector<ase::sol::note_c> const& a_rvAlterations,
    ase::cor::mapping_c const* const a_pMapping)
{
    using namespace ase::sol;

    enum_array_of(int, hauteur_e) asiHauteurs(true);
    asiHauteurs.set<enum_to__(hauteur_e::Do)>(1);
    asiHauteurs.set<enum_to__(hauteur_e::Re)>(3);
    asiHauteurs.set<enum_to__(hauteur_e::Mi)>(5);
    asiHauteurs.set<enum_to__(hauteur_e::Fa)>(6);
    asiHauteurs.set<enum_to__(hauteur_e::Sol)>(8);
    asiHauteurs.set<enum_to__(hauteur_e::La)>(10);
    asiHauteurs.set<enum_to__(hauteur_e::Si)>(12);

    enum_array_of(int, alteration_e) asiAlterations(true);
    asiAlterations.set<enum_to__(alteration_e::none)>(0);
    asiAlterations.set<enum_to__(alteration_e::Bemol)>(-1);
    asiAlterations.set<enum_to__(alteration_e::DoubleBemol)>(-2);
    asiAlterations.set<enum_to__(alteration_e::Diese)>(1);
    asiAlterations.set<enum_to__(alteration_e::DoubleDiese)>(2);

    enum_array_of(float, duree_e) afDurees(true);
    afDurees.set<enum_to__(duree_e::none)>(0);
    afDurees.set<enum_to__(duree_e::Carree)>(8);
    afDurees.set<enum_to__(duree_e::Ronde)>(4);
    afDurees.set<enum_to__(duree_e::Blanche)>(2);
    afDurees.set<enum_to__(duree_e::Noire)>(1);
    afDurees.set<enum_to__(duree_e::Croche)>(1/2.0f);
    afDurees.set<enum_to__(duree_e::DoubleCroche)>(1/4.0f);
    afDurees.set<enum_to__(duree_e::TripleCroche)>(1/8.0f);
    afDurees.set<enum_to__(duree_e::QuadrupleCroche)>(1/16.0f);

    array<float, 2, true> afPointees(true);
    afPointees.set<0>(0.0f);
    afPointees.set<1>(0.5f);


    c_float fBeatPerMinute = a_rMusic.beat_per_minute;
    c_float fBeatRatio = afDurees.val(enum_to(a_rMusic.duree));
    c_float fBeatDuration = 60000000.0f / (fBeatRatio * fBeatPerMinute);
    if (fBeatDuration < 0)
    {
        EMP_LOG_ERROR("Invalid beat duration in #0", fBeatDuration);
        return false;
    }

    a_pTrack->set_beat_duration(emp::ast::cast<ui64>(fBeatDuration));

    c_float fDelayDuration = fBeatDuration * afDurees.val<enum_to__(duree_e::QuadrupleCroche)>();

    EMP_LOG_INFO("DELAY #0", fDelayDuration);
    c_int siBase = asiHauteurs.val<enum_to__(hauteur_e::La)>();

    float fTime = 0;
    c_size stCount = a_rvNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        ase::sol::note_c const& rSolNote = a_rvNotes.ref(stIndex);

        c_float fRatio = afDurees.val(enum_to(rSolNote.duree));
        c_float fCount = 1.0f + afPointees.val(enum_to(rSolNote.pointee));
        c_float fDuration = emp::tt::maxof(0.0f, fBeatDuration * fRatio * fCount - fDelayDuration);
        if (rSolNote.hauteur != hauteur_e::none)
        {
            int siAlteration = asiAlterations.val(enum_to(rSolNote.alteration));

            if (siAlteration == 0)
            {
                c_size stAlterationCount = a_rvAlterations.count();
                for (size_t stAlteration = 0; stAlteration < stAlterationCount; ++stAlteration)
                {
                    ase::sol::note_c const& rAlteration = a_rvAlterations.ref(stAlteration);
                    if (rAlteration.hauteur == rSolNote.hauteur)
                    {
                        siAlteration = asiAlterations.val(enum_to(rAlteration.alteration));
                        break;
                    }
                }
            }
            c_int siOctave = enum_to_type(rSolNote.octave, int) * 12;
            c_int siHauteur = asiHauteurs.val(enum_to(rSolNote.hauteur));
            c_int siEmphasis = siOctave + siHauteur + siAlteration - siBase;

            EMP_RET_FALSE_IF(siEmphasis < 0);

            c_ui32 ui32Emphasis = emp::ast::cast<ui32>(siEmphasis);

            EMP_RET_FALSE_IF(fTime < 0.0f, "Invalid note time #0", fTime);
            EMP_RET_FALSE_IF(fDuration < 0.0f, "Invalid note duration #0", fDuration);

            a_pTrack->notes().add(emp::snd::note_c(ui32Emphasis, emp::ast::cast<ui64>(fTime), emp::ast::cast<ui64>(fDuration)));

            dump_note(rSolNote, ui32Emphasis, a_pMapping, stIndex);
        }

       fTime += fDuration + fDelayDuration;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool emphase_c::create_music(c_astring& a_sFilename, c_u8string& a_su8Name, ase::sol::music_c const& a_rMusic,
    ase::cor::mapping_c const* const a_pMapping)
{
    using namespace ase::sol;
    using namespace emp::snd;

    EMP_LOG_INFO("CREATE_MUSIC #0", a_sFilename);

    auto& rFactory = EMP_FACTORY;

    auto* const pScale = rFactory.store<emp::snd::scale_c>()->get_or_new_object(ALITERAL("chromatic.xml"));
    pScale->destroy();
    pScale->set_name(ALITERAL("Chromatic"));
    pScale->set_origin(440);
    pScale->set_tones(12);
    pScale->set_ratio(2);
    pScale->set_offset(0);

    auto* const pTrack = rFactory.store<emp::snd::track_c>()->get_or_new_object(STR_AFORMAT("#0_00.xml", a_sFilename));
    pTrack->destroy_track();
    pTrack->set_name(STR_U8FORMAT("#0 00", a_su8Name));
    pTrack->set_scale(pScale);
    pTrack->set_loop(false);

    auto* const pComposition = rFactory.store<emp::snd::composition_c>()->get_or_new_object(STR_AFORMAT("#0.xml", a_sFilename));
    pComposition->destroy();
    pComposition->set_name(a_su8Name);
    pComposition->add_track(pTrack);

    return build_track(pTrack, a_rMusic, a_rMusic.left_notes, a_rMusic.left_alterations, a_pMapping);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void emphase_c::create_layout(c_astring& a_sName, c_size a_stOffset)
{
    auto& rFactory = EMP_FACTORY;
    emp::rfx::store_t<emp::snd::layout_c>* pLayoutStore = rFactory.<emp::snd::layout_c>();
    EMP_ASSERT(pLayoutStore);

    emp::rfx::store_t<emp::snd::instrument_c>* pInstrumentStore = rFactory.store<emp::snd::instrument_c>();
    EMP_ASSERT(pInstrumentStore);

    emp::rfx::store_t<emp::snd::sample_c>* pSoundStore = rFactory.store<emp::snd::sample_c>();
    EMP_ASSERT(pSoundStore);

    emp::rfx::store_t<emp::snd::envelope_c>* pEnvelopeStore = rFactory.store<emp::snd::envelope_c>();
    EMP_ASSERT(pEnvelopeStore);

    emp::rfx::store_t<emp::snd::scale_c>* pScaleStore = rFactory.store<emp::snd::scale_c>();
    EMP_ASSERT(pScaleStore);

    emp::rfx::store_t<emp::snd::quality_c>* pQualityStore = rFactory.store<emp::snd::quality_c>();
    EMP_ASSERT(pQualityStore);

    emp::snd::layout_c* pLayout = pLayoutStore->get_or_allocate(STR_AFORMAT("emp_snd_layout_#0.xml", a_sName));
    EMP_ASSERT(pLayout);
    pLayoutStore->set_changed(pLayout);

    pLayout->create();
    c_size stCount = emp::snd::constants::key_count;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stI = a_stOffset + stIndex;

        c_astring sInstrument = STR_AFORMAT("emp_snd_instrument_#0_#1.xml", a_sName, stI);
        emp::snd::instrument_c* pInstrument = pInstrumentStore->get_or_allocate(sInstrument);
        pInstrument->set_name(sInstrument);
        pInstrumentStore->set_changed(pInstrument);

        c_astring sSound = STR_AFORMAT("emp_snd_sound_#0_#1.xml", a_sName, stI);
        emp::snd::sample_c* pSound = pSoundStore->get_or_allocate(sSound);
        pSound->set_name(sSound);

        c_astring sSoundFilename = STR_AFORMAT("#0_#1.wav", a_sName, INT_FMT_0D3(stI));
        pSound->set_filename(sSoundFilename);
        pSoundStore->set_changed(pSound);

        emp::snd::envelope_c* pEnvelope = pEnvelopeStore->get_object("emp_snd_envelope_00.xml");
        EMP_ASSERT(pEnvelope);
        pEnvelopeStore->set_changed(pEnvelope);

        emp::snd::scale_c* pScale = pScaleStore->get_object("emp_snd_scale_00.xml");
        EMP_ASSERT(pScale);
        pScaleStore->set_changed(pScale);

        emp::snd::quality_c* pQuality = pQualityStore->get_object("emp_snd_quality_00.xml");
        EMP_ASSERT(pQuality);
        pQualityStore->set_changed(pQuality);

        pInstrument->set_sound(pSound);
        pInstrument->set_envelope(pEnvelope);
        pInstrument->set_scale(pScale);
        pInstrument->set_quality(pQuality);

        pLayout->set_instrument(stIndex, pInstrument);
    }
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool get_sound(emp::snd::layout_i* const a_pLayout, emp::snd::scale_i const* const a_pScale,
    c_size a_stIndex, float& a_rfFrequency, emp::snd::sound_i*& a_rSound);
EMP_INTERN EMP_RETURN bool get_sound(emp::snd::layout_i* const a_pLayout, emp::snd::scale_i const* const a_pScale,
    c_size a_stIndex, float& a_rfFrequency, emp::snd::sound_i*& a_rSound)
{
    emp::snd::cell_c const oCell = a_pLayout->cell(a_stIndex);

    float fFrequencyMaster = 1;
    if (a_pScale != nullptr &&
        a_pScale->frequency(emp::ast::cast<ui32>(oCell.index), fFrequencyMaster) == false)
    {
        EMP_LOG_WARNING("Failed to compute master frequency");
        return false;
    }

    float fFrequencySlave = 1;
    emp::snd::scale_i* const pScale = oCell.scale;
    if (pScale != nullptr &&
        pScale->frequency(emp::ast::cast<ui32>(oCell.index), fFrequencySlave) == false)
    {
        EMP_LOG_WARNING("Failed to compute slave frequency");
        return false;
    }

    a_rfFrequency *= fFrequencyMaster * fFrequencySlave;

    emp::snd::synth_i* const pSynth = oCell.synth;
    if (pSynth != nullptr)
    {
        a_rSound = pSynth->sound(a_rfFrequency);
        if (a_rSound == nullptr)
        {
            EMP_LOG_WARNING("Failed to compute sound");
            return false;
        }

        if ((a_rSound->data()->envelopes().count() > 0) == false)
        {
            EMP_LOG_WARNING("No envelope");
            return false;
        }
        return true;
    }
    else
    {
        emp::snd::layout_i* const pLayout = oCell.layout;
        if (pLayout != nullptr)
        {
            return get_sound(
                pLayout, nullptr, oCell.index, a_rfFrequency, a_rSound);
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::play(emp::snd::layout_i* const a_pLayout, emp::snd::scale_i const* const a_pScale,
    c_size a_stIndex, c_float a_fVolume, c_bool a_bMute)
{
    if (m_papMedias->val(a_stIndex) != nullptr)
    { stop(a_stIndex); }

    float fFrequency = 1;
    emp::snd::sound_i* pSound = nullptr;
    if (get_sound(a_pLayout, a_pScale, a_stIndex, fFrequency, pSound) == false)
    {
        EMP_LOG_WARNING("Failed to get sound #0", a_stIndex);
        return;
    }

    emp::snd::media_c* const pMedia = EMP_ALLOCATE(emp::snd::media_c);

    //@@2 sndfreq
    c_float fVolume = a_bMute ? 0 : emp::tt::maxof(0.0f, a_fVolume);
    pMedia->create(pSound, fVolume);

    //EMP_APPLICATION.snd_player().play(pMedia);
    m_papMedias->set(a_stIndex, pMedia);

    m_fCurrentFrequency = fFrequency;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void emphase_c::stop(c_size a_stIndex) const
{
    if (m_papMedias->val(a_stIndex) != nullptr)
    { EMP_APPLICATION.snd_player().release(m_papMedias->val(a_stIndex)); }
    m_papMedias->set(a_stIndex, nullptr);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
album_c::album_c(void):
m_su8Name(),
m_pGfxCover(),
m_vpMusics()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
album_c::~album_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void album_c::create(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void album_c::destroy(void)
{
    m_su8Name = u8string();
    m_vpMusics.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN music_i const* album_c::try_get_music(c_size a_stIndex) const
{
    return m_vpMusics.is_valid_index(a_stIndex) ?
        m_vpMusics.val(a_stIndex) : nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN music_i* album_c::try_get_music(c_size a_stIndex)
{
    return m_vpMusics.is_valid_index(a_stIndex) ?
        m_vpMusics.val(a_stIndex) : nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN music_i const& album_c::get_music(c_size a_stIndex) const
{ return *m_vpMusics.ref(a_stIndex); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN music_i& album_c::get_music(c_size a_stIndex)
{ return *m_vpMusics.ref(a_stIndex); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mapping_c::mapping_c(void):
m_aui64Chars(),
m_mKeyCodeToEmphasis(32),
m_sName(),
m_su8Mapping()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mapping_c::~mapping_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mapping_c::post_load(void)
{
    EMP_RET_FALSE_IF(m_su8Mapping.len() != emp::snd::constants::key_count, "Invalid mapping : #0", m_sName);

    m_aui64Chars.resize(emp::snd::constants::key_count);
    u8string sMapping(m_su8Mapping);
    while (true)
    {
        c_size stLen = sMapping.len();
        if ((stLen > 0) == false)
        { break; }

        c_ui64 cChar = sMapping.get_first_char();
        sMapping.trim_left();

        m_aui64Chars.set(stLen - 1, cChar);
    }

#define XTMP_KEY(x_Name) { m_mKeyCodeToEmphasis.set(emp::app::key_e::x_Name, ui32Index++); }
    ui32 ui32Index = 0;
    XTMP_KEY( Slash       )
    XTMP_KEY( Period      )
    XTMP_KEY( Comma       )
    XTMP_KEY( Alpha_M     )
    XTMP_KEY( Alpha_N     )
    XTMP_KEY( Alpha_B     )
    XTMP_KEY( Alpha_V     )
    XTMP_KEY( Alpha_C     )
    XTMP_KEY( Alpha_X     )
    XTMP_KEY( Alpha_Z     )
    XTMP_KEY( SemiColon   )
    XTMP_KEY( Alpha_L     )
    XTMP_KEY( Alpha_K     )
    XTMP_KEY( Alpha_J     )
    XTMP_KEY( Alpha_H     )
    XTMP_KEY( Alpha_G     )
    XTMP_KEY( Alpha_F     )
    XTMP_KEY( Alpha_D     )
    XTMP_KEY( Alpha_S     )
    XTMP_KEY( Alpha_A     )
    XTMP_KEY( Alpha_P     )
    XTMP_KEY( Alpha_O     )
    XTMP_KEY( Alpha_I     )
    XTMP_KEY( Alpha_U     )
    XTMP_KEY( Alpha_Y     )
    XTMP_KEY( Alpha_T     )
    XTMP_KEY( Alpha_R     )
    XTMP_KEY( Alpha_E     )
    XTMP_KEY( Alpha_W     )
    XTMP_KEY( Alpha_Q     )
    XTMP_KEY( Num_0       )
    XTMP_KEY( Num_9       )
    XTMP_KEY( Num_8       )
    XTMP_KEY( Num_7       )
    XTMP_KEY( Num_6       )
    XTMP_KEY( Num_5       )
    XTMP_KEY( Num_4       )
    XTMP_KEY( Num_3       )
    XTMP_KEY( Num_2       )
    XTMP_KEY( Num_1       )
#undef XTMP_KEY

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void mapping_c::destroy(void)
{
    m_aui64Chars.resize(0);
    m_mKeyCodeToEmphasis.clear();
    m_sName = astring();
    m_su8Mapping = u8string();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mapping_c::keycode_to_emphasis(emp::app::c_key_e a_eKeyCode, ui32& a_rui32Emphasis) const
{
    auto const oIt = m_mKeyCodeToEmphasis.get(a_eKeyCode);
    EMP_RET_FALSE_IF(oIt == m_mKeyCodeToEmphasis.end());
    a_rui32Emphasis = *oIt.value();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui64 mapping_c::emphasis_to_keychar(c_ui32 a_ui32Emphasis) const
{ return m_aui64Chars.val(a_ui32Emphasis); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//@@2 hardcoded
EMP_RETURN size_t mapping_c::emphasis_to_finger(c_ui32 a_ui32Emphasis) const
{ return a_ui32Emphasis % 10; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_RETURN ui32 mapping_c::emphasis_to_keycode(c_ui32 a_ui32Emphasis) const
//{ return m_aui32Codes.val(a_ui32Emphasis); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN size_t mapping_c::keychar_to_emphasis(c_ui32 a_ui32Code) const
{
    //use emp::cnt::map
    c_size stCount = m_acChars.count();
    for (size_t stIndex  = 0; stIndex < stCount; ++stIndex)
    {
        if (a_ui32Code == m_acChars.val(stIndex))
        {
            EMP_ASSERT(m_sMapping.get_char(stIndex) == a_ui32Code);
            return stIndex;
        }
    }

    if (a_ui32Code)
    {
        EMP_LOG_DEBUG("Char not found : 0x#0, #1", INT_FMT_0H(a_ui32Code), emp::str::fmt(a_ui32Code));
    }
    else
    {
        EMP_LOG_DEBUG("Char not found : 0x#0", INT_FMT_0H(a_ui32Code));
    }
    return emp::snd::constants::key_count;
}*/


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
matrix_c::matrix_c(void):
m_astNotes(true)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void matrix_c::destroy(void)
{
    c_size stCount = m_astNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_astNotes.set(stIndex, size_t(emp::tt::max<size_t>::value)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void matrix_c::set_address(c_size a_stIndex, c_size a_stAddress)
{ m_astNotes.set(a_stIndex, a_stAddress); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t matrix_c::get_address(c_size a_stIndex) const
{ return m_astNotes.val(a_stIndex); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
music_shell_c::music_shell_c(void):
    m_pMusic(),
    m_sEmpty()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
music_shell_c::music_shell_c(
    music_i const* const a_pMusic):
    m_pMusic(a_pMusic),
    m_sEmpty()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_u8string& music_shell_c::name(void) const
{ return m_pMusic->name(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float music_shell_c::beat_per_minute(void) const
{ return m_pMusic->beat_per_minute(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::tim::time_c music_shell_c::duration(void) const
{ return m_pMusic->track()->duration(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui32 music_shell_c::rating(void) const
{
    cpc_user_data_s pUserData = m_pMusic->get_user_data(&ASE_GAME.user());
    return pUserData ? pUserData->rating : 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui32 music_shell_c::played(void) const
{
    cpc_user_data_s pUserData = m_pMusic->get_user_data(&ASE_GAME.user());
    return pUserData ? pUserData->played : 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_completion_data_s* music_shell_c::highest(void) const
{
    user_c const& rUser = ASE_GAME.user();
    cpc_completion_data_s pHighest = m_pMusic->highest(nullptr, rUser.options);
    return pHighest;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_u8string& music_shell_c::highest_user(void) const
{
    cpc_completion_data_s pHighest = highest();
    return pHighest ? pHighest->user->name() : m_sEmpty;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float music_shell_c::highest_completion(void) const
{
    cpc_completion_data_s pHighest = highest();
    return pHighest ? pHighest->score : 0.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::tim::time_c music_shell_c::highest_time(void) const
{
    cpc_completion_data_s pHighest = highest();
    return pHighest ? emp::tim::time_c(pHighest->time) : emp::tim::time_c();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float music_shell_c::highest_success(void) const
{
    cpc_completion_data_s pHighest = highest();
    return pHighest ? pHighest->success : 0.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float music_shell_c::highest_failure(void) const
{
    cpc_completion_data_s pHighest = highest();
    return pHighest ? pHighest->failure : 0.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
interpretation_c::interpretation_c(void):
    m_Completion(),
    m_mSuccess(100),
    m_Success(),
    m_mFailure(100),
    m_Failure()
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
interpretation_c::~interpretation_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void interpretation_c::destroy(void)
{
    m_Completion = completion_data_s();

    m_mSuccess.clear();
    m_Success.destroy_track();

    m_mFailure.clear();
    m_Failure.destroy_track();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void interpretation_c::compute_completion(emp::snd::track_c const& a_rTrack)
{
    emp::tim::time_c const oCompletion = a_rTrack.completion();
    emp::tim::time_c const oSuccess = m_Success.completion();
    emp::tim::time_c const oFailure = m_Failure.completion();

    c_float fCompletion = emp::tim::time_c::to_seconds(oCompletion);
    c_float fRatio = fCompletion > 0 ? 100.0f / fCompletion : 0;
    c_float fSuccess = emp::tim::time_c::to_seconds(oSuccess);
    c_float fFailure = emp::tim::time_c::to_seconds(oFailure);

    m_Completion.time = a_rTrack.duration().usec();
    m_Completion.score = fRatio * (fSuccess - fFailure);
    m_Completion.success = fRatio * fSuccess;
    m_Completion.failure = fRatio * fFailure;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void interpretation_c::remove_success(emp::snd::note_c const& a_rValue)
{
    emp::cnt::map_t<emp::snd::note_c, size_t>::iterator_c const oIt =
        m_mSuccess.get(a_rValue);
    if (oIt != m_mSuccess.end())
    { m_Success.notes().set(*oIt.value(), emp::snd::note_c()); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void interpretation_c::set_success(emp::snd::note_c const& a_rValue)
{
    emp::cnt::map_t<emp::snd::note_c, size_t>::iterator_c const oIt =
        m_mSuccess.get(a_rValue);
    if (oIt == m_mSuccess.end())
    {
        m_mSuccess.set(a_rValue, m_Success.notes().count());
        m_Success.notes().add(a_rValue);
    }
    else
    { m_Success.notes().set(*oIt.value(), a_rValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void interpretation_c::set_failure(emp::snd::note_c const& a_rValue)
{
    emp::cnt::map_t<emp::snd::note_c, size_t>::iterator_c const oIt =
        m_mFailure.get(a_rValue);
    if (oIt == m_mFailure.end())
    {
        m_mFailure.set(a_rValue, m_Failure.notes().count());
        m_Failure.notes().add(a_rValue);
    }
    else
    { m_Failure.notes().set(*oIt.value(), a_rValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void completion_bucket_c::add(c_completion_data_s& a_rData)
{
    c_size stCount = m_vData.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_completion_data_s& rData = m_vData.ref(stIndex);
        if (rData == a_rData)
        { return; }
    }
    m_vData.add(a_rData);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
performance_c::performance_c(void):
m_vpBuckets(),
m_vUserData(),
m_Current()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
performance_c::~performance_c(void)
{ clear(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void performance_c::add(c_option_s& a_rOptions, c_completion_data_s& a_rValue)
{
    m_Current = a_rValue;
    c_completion_option_s oOptions(a_rOptions);

    c_size stCount = m_vpBuckets.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        completion_bucket_c* const pBucket = m_vpBuckets.val(stIndex);
        if (pBucket->options() == oOptions)
        {
            pBucket->add(a_rValue);
            return;
        }
    }

    completion_bucket_c* const pBucket = EMP_ALLOCATE(completion_bucket_c, oOptions);
    pBucket->add(a_rValue);
    m_vpBuckets.add(pBucket);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void performance_c::clear(void)
{
    //emp::cnt::foreach(m_vpBuckets, emp::mem::safe_delete_reset_t());
    m_vpBuckets.resize(0);
    m_vUserData.resize(0);
    m_Current = completion_data_s();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN size_t get_user_data_index(user_c const* const a_pUser, emp::cnt::vector<user_data_s> const& a_rvUser);
EMP_INTERN EMP_RETURN size_t get_user_data_index(user_c const* const a_pUser, emp::cnt::vector<user_data_s> const& a_rvUser)
{
    if (a_pUser != nullptr)
    {
        c_size stCount = a_rvUser.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_user_data_s& rUserData = a_rvUser.ref(stIndex);
            if (rUserData.user == a_pUser)
            { return stIndex; }
        }
    }
    return emp::tt::get_max<size_t>();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_user_data_s* performance_c::get_user_data(user_c* const a_pUser) const
{
    c_size stIndex = get_user_data_index(a_pUser, m_vUserData);
    if (emp::tt::is_not_max(stIndex))
    { return m_vUserData.ptr(stIndex); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN user_data_s* performance_c::get_user_data(user_c* const a_pUser)
{
    c_size stIndex = get_user_data_index(a_pUser, m_vUserData);
    if (emp::tt::is_not_max(stIndex))
    { return m_vUserData.ptr(stIndex); }

    user_data_s& rUserData = m_vUserData.push();
    rUserData.user = a_pUser;
    return &rUserData;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_completion_data_s* performance_c::highest(user_c* const a_pUser, c_option_s& a_rOptions) const
{
    c_completion_option_s oOptions(a_rOptions);

    c_completion_data_s* pHighest = nullptr;

    c_size stCount = m_vpBuckets.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        completion_bucket_c const* const pBucket = m_vpBuckets.val(stIndex);
        if (pBucket->options() == oOptions)
        {
            c_size stDataCount = pBucket->data_count();
            for (size_t stData = 0; stData < stDataCount; ++stData)
            {
                c_completion_data_s& rData = pBucket->data(stData);
                if ((a_pUser == nullptr || (a_pUser == rData.user)) &&
                    (pHighest == nullptr || (rData.is_higher(*pHighest))))
                { pHighest = &rData; }
            }
        }
    }

    return pHighest;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
easy_updater_c::easy_updater_c(void):
    m_pInterpretation(),
    m_bEnded(),
    m_pInterpretMusic(nullptr),
    m_Time(),
    m_MusicTime(),
    m_stMusicNote(),
    m_stUserNote(),
    m_vMusicNotes(),
    m_vUserNotes()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void easy_updater_c::destroy(void)
{
    m_pInterpretMusic = nullptr;
    m_bEnded = false;
    m_Time = emp::tim::time_c();
    m_MusicTime = emp::tim::time_c();
    m_stMusicNote = 0;
    m_stUserNote = 0;
    m_vMusicNotes.resize(0);
    m_vUserNotes.resize(0);
    m_pInterpretation->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void easy_updater_c::start(void)
{
    m_pInterpretation->destroy();
    m_bEnded = false;

    m_Time = emp::tim::time_c();
    m_MusicTime = emp::tim::time_c();

    m_stMusicNote = 0;
    m_stUserNote = 0;

    m_vMusicNotes.resize(0);
    m_vUserNotes.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void easy_updater_c::stop(void)
{
    m_pInterpretation->compute_completion(*track());
    m_pInterpretMusic->set_played(&ASE_GAME.user(), m_Time, m_pInterpretation->completion());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void easy_updater_c::update(emp::tim::time_c const& a_rTime)
{
    m_Time = a_rTime;

    m_stUserNote = get_notes(m_stUserNote, *ASE_GAME.track(), a_rTime, emp::tim::time_c(), m_vUserNotes);

    emp::snd::track_c const* const pTrack = track();

    m_stMusicNote = get_notes(m_stMusicNote, *pTrack, m_MusicTime, emp::tim::time_c(), m_vMusicNotes);

    size_t stUser = 0;
    while (stUser < m_vUserNotes.count())
    {
        emp::snd::note_c const oUser(m_vUserNotes.ref(stUser));

        bool bRemoved = false;

        size_t stMusic = 0;
        while (stMusic < m_vMusicNotes.count())
        {
            emp::snd::note_c const oMusic(m_vMusicNotes.ref(stMusic));
            if (oUser.index == oMusic.index)
            {
                m_vMusicNotes.remove(stMusic);
                ++m_stMusicNote;

                m_vUserNotes.remove_unordered(stUser);
                ++m_stUserNote;

                m_pInterpretation->set_success(oMusic);
                bRemoved = true;
                break;
            }
            else
            { ++stMusic; }
        }

        if (bRemoved == false)
        {
            ++m_stMusicNote;
            ++m_stUserNote;
            ++stUser;
        }
    }

    if (m_vUserNotes.count() > 0)
    {
        size_t stFailure = 0;
        while (stFailure < m_vMusicNotes.count())
        {
            m_pInterpretation->set_failure(m_vMusicNotes.ref(stFailure));
            m_vMusicNotes.remove(stFailure);
        }
    }

    m_pInterpretation->compute_completion(*pTrack);

    emp::tim::time_c const oTrackDuration(pTrack->duration());
    emp::cnt::vector<emp::snd::note_c> const& rvNotes = pTrack->notes();
    c_size stNoteCount = rvNotes.count();
    m_MusicTime = m_stMusicNote < stNoteCount ? emp::tim::time_c(rvNotes.ref(m_stMusicNote).time) : oTrackDuration;
    m_bEnded = emp::tim::time_c::more_equal(m_MusicTime, oTrackDuration);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool easy_updater_c::condition_satisfied(void) const
{ return m_bEnded; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float easy_updater_c::current_completion(void) const
{ return m_pInterpretation->completion().score; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float easy_updater_c::music_time(void) const
{ return emp::tim::time_c::to_seconds(m_MusicTime); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::snd::track_c const* easy_updater_c::track(void) const
{ return m_pInterpretMusic->track(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void easy_updater_c::set_music(music_i* const a_pValue)
{ m_pInterpretMusic = a_pValue; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
improvize_updater_c::improvize_updater_c(void):
    m_pInterpretation(),
    m_Time(),
    m_pInterpretMusic(),
    m_stIterator()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_updater_c::destroy(void)
{
    m_pInterpretation->destroy();
    m_Time = emp::tim::time_c();
    m_pInterpretMusic = nullptr;
    m_stIterator = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_updater_c::start(void)
{ m_pInterpretation->destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_updater_c::stop(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_updater_c::update(emp::tim::time_c const& a_rTime)
{
    m_Time = a_rTime;

    emp::snd::track_c const* const pTrack = track();
    emp::cnt::vector<emp::snd::note_c> const& rvNotes = pTrack->notes();
    c_size stNoteCount = rvNotes.count();

    bool bStoreIt = true;

    c_size stBegin = m_stIterator;
    for (size_t stNote = stBegin; stNote < stNoteCount; ++stNote)
    {
        emp::snd::note_c const oNote = rvNotes.ref(stNote);
        note_state_e eState = is_valid_note(oNote, a_rTime, emp::tim::time_c());
        if (eState == note_state_e::Before)
        { continue; }
        else if (eState == note_state_e::After)
        { break; }

        EMP_ASSERT(eState == note_state_e::Current);
        if (bStoreIt)
        {
            m_stIterator = stNote;
            bStoreIt = false;
        }

        emp::tim::time_c const oTime(oNote.time);
        emp::tim::time_c oDuration(oNote.duration);

        if (emp::tim::time_c::equal(oDuration, emp::tim::time_c()))
        { emp::tim::time_c::sub(a_rTime, oTime, oDuration); }

        emp::snd::note_c oCopy(oNote);
        oCopy.duration = oDuration.usec();
        m_pInterpretation->set_success(oCopy);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool improvize_updater_c::condition_satisfied(void) const
{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float improvize_updater_c::current_completion(void) const
{ return 100.0f; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float improvize_updater_c::music_time(void) const
{ return emp::tim::time_c::to_seconds(m_Time); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::snd::track_c const* improvize_updater_c::track(void) const
{ return ASE_GAME.track(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void improvize_updater_c::set_music(music_i* const a_pValue)
{
    m_pInterpretMusic = a_pValue;
    m_stIterator = 0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
hard_updater_c::hard_updater_c(void):
    m_pInterpretMusic(nullptr),
    m_bEnded(),
    m_Time(),
    m_stNote(),
    m_vNotes(),
    m_pInterpretation()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void hard_updater_c::destroy(void)
{
    m_pInterpretMusic = nullptr;
    m_bEnded = false;
    m_Time = emp::tim::time_c();
    m_stNote = 0;
    m_vNotes.resize(0);
    m_pInterpretation->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void hard_updater_c::start(void)
{
    m_pInterpretation->destroy();
    m_bEnded = false;
    m_Time = emp::tim::time_c();
    m_stNote = 0;
    m_vNotes.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void hard_updater_c::stop(void)
{
    emp::snd::track_c const* const pTrack = track();
    m_pInterpretation->compute_completion(*pTrack);
    m_pInterpretMusic->set_played(&ASE_GAME.user(), pTrack->duration(), m_pInterpretation->completion());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void hard_updater_c::update(emp::tim::time_c const& a_rTime)
{
    m_Time = a_rTime;

    m_stNote = get_notes(m_stNote, *ASE_GAME.track(), a_rTime, emp::tim::time_c(), m_vNotes);
    emp::snd::track_c const* const pTrack = track();
    emp::cnt::vector<emp::snd::note_c> const& rvNotes = pTrack->notes();
    c_size stNoteCount = rvNotes.count();
    for (size_t stNote = 0; stNote < stNoteCount; ++stNote)
    {
        emp::snd::note_c const oEmp(rvNotes.ref(stNote));
        emp::tim::time_c const oEmpStart(oEmp.time);
        emp::tim::time_c const oEmpDuration(oEmp.duration);

        emp::tim::time_c oEmpEnd;
        emp::tim::time_c::add(oEmpStart, oEmpDuration, oEmpEnd);

        bool bEnded = true;

        size_t stIndex = 0;
        while (stIndex < m_vNotes.count())
        {
            emp::snd::note_c const oUser(m_vNotes.ref(stIndex));
            emp::tim::time_c const oUserStart(oUser.time);
            emp::tim::time_c const oUserDuration(oUser.duration);
            emp::tim::time_c oUserEnd;
            emp::tim::time_c::add(oUserStart, oUserDuration, oUserEnd);

            if (emp::tim::time_c::more_equal(oEmpStart, oUserEnd))
            {
                ++stIndex;
                continue;
            }

            bEnded = false;

            if (oEmp.index != oUser.index ||
                emp::tim::time_c::less_equal(oEmpEnd, oUserStart))
            {
                ++stIndex;
                continue;
            }

            emp::tim::time_c oEnd;
            emp::tim::time_c::min(oEmpEnd, oUserEnd, oEnd);

            emp::tim::time_c oStart;
            emp::tim::time_c::max(oEmpStart, oUserStart, oStart);

            emp::tim::time_c oDuration;
            emp::tim::time_c::sub(oEnd, oStart, oDuration);

            emp::snd::note_c oLeft(oUser);
            c_bool bLeft = emp::tim::time_c::equal(oStart, oUserStart) == false;
            if (bLeft)
            {
                emp::tim::time_c oLeftDuration;
                emp::tim::time_c::sub(oStart, oUserStart, oLeftDuration);

                oLeft.time = oUserStart.usec();
                oLeft.duration = oLeftDuration.usec();
            }

            emp::snd::note_c oRight(oUser);
            c_bool bRight = emp::tim::time_c::equal(oEnd, oUserEnd) == false;
            if (bRight)
            {
                emp::tim::time_c oRightDuration;
                emp::tim::time_c::sub(oUserEnd, oEnd, oRightDuration);

                oRight.time = oEnd.usec();
                oRight.duration = oRightDuration.usec();
            }

            if (bLeft && bRight)
            {
                m_vNotes.set(stIndex, oLeft);
                m_vNotes.add(oRight);
            }
            else if (bLeft)
            { m_vNotes.set(stIndex, oLeft); }
            else if (bRight)
            { m_vNotes.set(stIndex, oRight); }
            else
            { m_vNotes.remove_unordered(stIndex); }

            emp::snd::note_c oSuccess(oUser);
            oSuccess.time = oStart.usec();
            oSuccess.duration = oDuration.usec();
            m_pInterpretation->set_success(oSuccess);
        }

        if (bEnded)
        { break; }
    }

    for (size_t stNote = 0; stNote < stNoteCount; ++stNote)
    {
        emp::snd::note_c const oEmp(rvNotes.ref(stNote));
        emp::tim::time_c const oEmpStart(oEmp.time);
        emp::tim::time_c const oEmpDuration(oEmp.duration);
        emp::tim::time_c oEmpEnd;
        emp::tim::time_c::add(oEmpStart, oEmpDuration, oEmpEnd);

        bool bEnded = true;

        size_t stIndex = 0;
        while (stIndex < m_vNotes.count())
        {
            emp::snd::note_c const oUser(m_vNotes.ref(stIndex));
            emp::tim::time_c const oUserStart(oUser.time);
            emp::tim::time_c const oUserDuration(oUser.duration);
            emp::tim::time_c oUserEnd;
            emp::tim::time_c::add(oUserStart, oUserDuration, oUserEnd);

            if (emp::tim::time_c::more_equal(oEmpStart, oUserEnd))
            {
                ++stIndex;
                continue;
            }

            bEnded = false;

            if (emp::tim::time_c::less_equal(oEmpEnd, oUserStart))
            {
                ++stIndex;
                continue;
            }

            emp::tim::time_c oEnd;
            emp::tim::time_c::min(oEmpEnd, oUserEnd, oEnd);

            emp::tim::time_c oStart;
            emp::tim::time_c::max(oEmpStart, oUserStart, oStart);

            emp::tim::time_c oDuration;
            emp::tim::time_c::sub(oEnd, oStart, oDuration);

            emp::snd::note_c oLeft(oUser);
            c_bool bLeft = emp::tim::time_c::equal(oStart, oUserStart) == false;
            if (bLeft)
            {
                emp::tim::time_c oLeftDuration;
                emp::tim::time_c::sub(oStart, oUserStart, oLeftDuration);

                oLeft.time = oUserStart.usec();
                oLeft.duration = oLeftDuration.usec();
            }

            emp::snd::note_c oRight(oUser);
            c_bool bRight = emp::tim::time_c::equal(oEnd, oUserEnd) == false;
            if (bRight)
            {
                emp::tim::time_c oRightDuration;
                emp::tim::time_c::sub(oUserEnd, oEnd, oRightDuration);

                oRight.time = oEnd.usec();
                oRight.duration = oRightDuration.usec();
            }

            if (bLeft)
            { m_vNotes.add(oLeft); }
            if (bRight)
            { m_vNotes.add(oRight); }

            emp::snd::note_c oFailure(oUser);
            oFailure.time = oStart.usec();
            oFailure.duration = oDuration.usec();
            m_vNotes.set(stIndex, oFailure);

            if (bLeft == false && bRight == false)
            { ++stIndex; }
        }

        if (bEnded)
        { break; }
    }

    { c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_pInterpretation->set_failure(m_vNotes.ref(stIndex)); } }

    m_pInterpretation->compute_completion(*pTrack);

    emp::tim::time_c oDuration = pTrack->duration();
    m_bEnded = emp::tim::time_c::more_equal(a_rTime, oDuration);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool hard_updater_c::condition_satisfied(void) const
{ return m_bEnded; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float hard_updater_c::current_completion(void) const
{ return m_pInterpretation->completion().score; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float hard_updater_c::music_time(void) const
{ return emp::tim::time_c::to_seconds(m_Time); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::snd::track_c const* hard_updater_c::track(void) const
{ return m_pInterpretMusic->track(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void hard_updater_c::set_music(music_i* const a_pValue)
{ m_pInterpretMusic = a_pValue; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
normal_updater_c::normal_updater_c(void):
    m_pInterpretMusic(nullptr),
    m_bEnded(),
    m_Time(),
    m_stNote(),
    m_vNotes(),
    m_pInterpretation()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normal_updater_c::destroy(void)
{
    m_pInterpretMusic = nullptr;
    m_bEnded = false;
    m_Time = emp::tim::time_c();
    m_stNote = 0;
    m_vNotes.resize(0);
    m_pInterpretation->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normal_updater_c::start(void)
{
    m_pInterpretation->destroy();
    m_bEnded = false;
    m_Time = emp::tim::time_c();
    m_stNote = 0;
    m_vNotes.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normal_updater_c::stop(void)
{
    emp::snd::track_c const* const pTrack = track();
    m_pInterpretation->compute_completion(*pTrack);
    m_pInterpretMusic->set_played(&ASE_GAME.user(), pTrack->duration(), m_pInterpretation->completion());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normal_updater_c::update(emp::tim::time_c const& a_rTime)
{
    m_Time = a_rTime;

    m_stNote = get_notes(m_stNote, *ASE_GAME.track(), a_rTime, emp::tim::time_c(), m_vNotes);

    emp::snd::track_c const* const pTrack = track();
    emp::cnt::vector<emp::snd::note_c> const& rvNotes = pTrack->notes();
    c_size stNoteCount = rvNotes.count();
    for (size_t stNote = 0; stNote < stNoteCount; ++stNote)
    {
        emp::snd::note_c const oEmp(rvNotes.ref(stNote));
        emp::tim::time_c const oEmpStart(oEmp.time);
        emp::tim::time_c const oEmpDuration(oEmp.duration);
        emp::tim::time_c oEmpEnd;
        emp::tim::time_c::add(oEmpStart, oEmpDuration, oEmpEnd);

        bool bEnded = true;

        size_t stIndex = 0;
        while (stIndex < m_vNotes.count())
        {
            emp::snd::note_c const oUser(m_vNotes.ref(stIndex));
            emp::tim::time_c const oUserStart(oUser.time);
            emp::tim::time_c const oUserDuration(oUser.duration);
            emp::tim::time_c oUserEnd;
            emp::tim::time_c::add(oUserStart, oUserDuration, oUserEnd);

            if (emp::tim::time_c::more_equal(oEmpStart, oUserEnd))
            {
                ++stIndex;
                continue;
            }

            bEnded = false;

            if (oEmp.index != oUser.index ||
                emp::tim::time_c::less_equal(oEmpEnd, oUserStart))
            {
                ++stIndex;
                continue;
            }

            emp::tim::time_c oEnd;
            emp::tim::time_c::min(oEmpEnd, oUserEnd, oEnd);

            emp::tim::time_c oStart;
            emp::tim::time_c::max(oEmpStart, oUserStart, oStart);

            emp::tim::time_c oDuration;
            emp::tim::time_c::sub(oEnd, oStart, oDuration);

            emp::snd::note_c oLeft(oUser);
            c_bool bLeft = emp::tim::time_c::equal(oStart, oUserStart) == false;
            if (bLeft)
            {
                emp::tim::time_c oLeftDuration;
                emp::tim::time_c::sub(oStart, oUserStart, oLeftDuration);

                oLeft.time = oUserStart.usec();
                oLeft.duration = oLeftDuration.usec();
            }

            emp::snd::note_c oRight(oUser);
            c_bool bRight = emp::tim::time_c::equal(oEnd, oUserEnd) == false;
            if (bRight)
            {
                emp::tim::time_c oRightDuration;
                emp::tim::time_c::sub(oUserEnd, oEnd, oRightDuration);

                oRight.time = oEnd.usec();
                oRight.duration = oRightDuration.usec();
            }

            if (bLeft && bRight)
            {
                m_vNotes.set(stIndex, oLeft);
                m_vNotes.add(oRight);
            }
            else if (bLeft)
            { m_vNotes.set(stIndex, oLeft); }
            else if (bRight)
            { m_vNotes.set(stIndex, oRight); }
            else
            { m_vNotes.remove_unordered(stIndex); }

            m_pInterpretation->set_success(oEmp);
        }

        if (bEnded)
        { break; }
    }

    for (size_t stNote = 0; stNote < stNoteCount; ++stNote)
    {
        emp::snd::note_c const oEmp(rvNotes.ref(stNote));
        emp::tim::time_c const oEmpStart(oEmp.time);
        emp::tim::time_c const oEmpDuration(oEmp.duration);
        emp::tim::time_c oEmpEnd;
        emp::tim::time_c::add(oEmpStart, oEmpDuration, oEmpEnd);

        bool bEnded = true;

        size_t stIndex = 0;
        while (stIndex < m_vNotes.count())
        {
            emp::snd::note_c const oUser(m_vNotes.ref(stIndex));
            emp::tim::time_c const oUserStart(oUser.time);
            emp::tim::time_c const oUserDuration(oUser.duration);
            emp::tim::time_c oUserEnd;
            emp::tim::time_c::add(oUserStart, oUserDuration, oUserEnd);

            if (emp::tim::time_c::more_equal(oEmpStart, oUserEnd))
            {
                ++stIndex;
                continue;
            }

            bEnded = false;

            if (emp::tim::time_c::less_equal(oEmpEnd, oUserStart))
            {
                ++stIndex;
                continue;
            }

            emp::tim::time_c oEnd;
            emp::tim::time_c::min(oEmpEnd, oUserEnd, oEnd);

            emp::tim::time_c oStart;
            emp::tim::time_c::max(oEmpStart, oUserStart, oStart);

            emp::tim::time_c oDuration;
            emp::tim::time_c::sub(oEnd, oStart, oDuration);

            emp::snd::note_c oLeft(oUser);
            c_bool bLeft = emp::tim::time_c::equal(oStart, oUserStart) == false;
            if (bLeft)
            {
                emp::tim::time_c oLeftDuration;
                emp::tim::time_c::sub(oStart, oUserStart, oLeftDuration);

                oLeft.time = oUserStart.usec();
                oLeft.duration = oLeftDuration.usec();
            }

            emp::snd::note_c oRight(oUser);
            c_bool bRight = emp::tim::time_c::equal(oEnd, oUserEnd) == false;
            if (bRight)
            {
                emp::tim::time_c oRightDuration;
                emp::tim::time_c::sub(oUserEnd, oEnd, oRightDuration);

                oRight.time = oEnd.usec();
                oRight.duration = oRightDuration.usec();
            }

            if (bLeft)
            { m_vNotes.add(oLeft); }
            if (bRight)
            { m_vNotes.add(oRight); }

            m_vNotes.set(stIndex, oEmp);

            if (bLeft == false && bRight == false)
            { ++stIndex; }
        }

        if (bEnded)
        { break; }
    }

    { c_size stCount = m_vNotes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_pInterpretation->set_failure(m_vNotes.ref(stIndex)); } }

    m_pInterpretation->compute_completion(*pTrack);

    emp::tim::time_c oDuration = pTrack->duration();
    m_bEnded = emp::tim::time_c::more_equal(a_rTime, oDuration);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool normal_updater_c::condition_satisfied(void) const
{ return m_bEnded; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float normal_updater_c::current_completion(void) const
{ return m_pInterpretation->completion().score; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float normal_updater_c::music_time(void) const
{ return emp::tim::time_c::to_seconds(m_Time); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::snd::track_c const* normal_updater_c::track(void) const
{ return m_pInterpretMusic->track(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normal_updater_c::set_music(music_i* const a_pValue)
{ m_pInterpretMusic = a_pValue; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

