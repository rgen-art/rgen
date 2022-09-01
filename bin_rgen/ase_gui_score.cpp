//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_gui_score.h"

#include "ase_cor_emphase.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_cnt_foreach.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_frame.h"
#include "emp_gui_label.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

#include "emp_snd_layout_i.h"
#include "emp_snd_track.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
score_c::score_c(void):
m_fBeatPerMinute(100),
m_fBeatPerMesure(4),
m_fTimeScale(1),
m_fTime(0),
m_fStartSize(0),
m_vpLabels(),
m_vAddresses(),
m_pTrack(),
m_pEditionTrack(),
m_pInterpretation(nullptr),
m_pMapping(nullptr),
m_bShowLetters(true),
m_bShowDurations(true),
m_bEdition(false),
m_pTimeMeshVertices(),
m_pTimeMeshIndices(),
m_pImMeshVertices(),
m_pImMeshIndices(),
m_pSuccessMeshVertices(),
m_pSuccessMeshIndices(),
m_pFailureMeshVertices(),
m_pFailureMeshIndices(),
m_stCurrentBeat(0),
m_stCurrentMesure(0),
m_pSkin(),
m_pLabel(),
m_fMinHeight(0),
m_stMinNoteCount(0),
m_bComputeBeatSize(true),
m_vfOffsets(),
m_vfIndices(),
m_fScaleX(1.0f),
m_fScaleY(1.0f)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
score_c::~score_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::create(void)
{
    destroy();

    // @@0 create m_pLabel->create();
    m_pLabel->state().autosize.xy(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::destroy(void)
{
    reset();

    m_fBeatPerMinute = 100;
    m_fBeatPerMesure = 4;

    m_fTimeScale = 1.0f;
    m_fTime = 0;

    m_fStartSize = 0;

    emp::cnt::foreach(m_vpLabels, emp::mem::safe_delete_reset_t()); // @@0 create
    m_vpLabels.resize(0);
    m_vAddresses.resize(0);

    m_pTrack = nullptr;
    m_pEditionTrack = nullptr;
    m_pInterpretation = nullptr;
    m_pMapping = nullptr;

    m_bShowLetters = true;
    m_bShowDurations = true;
    m_bEdition = false;

    m_pTimeMeshVertices->destroy();
    m_pTimeMeshIndices->destroy();

    m_pImMeshVertices->destroy();
    m_pImMeshIndices->destroy();

    m_pSuccessMeshVertices->destroy();
    m_pSuccessMeshIndices->destroy();

    m_pFailureMeshVertices->destroy();
    m_pFailureMeshVertices->destroy();

    m_stCurrentBeat = 0;
    m_stCurrentMesure = 0;

    m_pSkin = nullptr;

    //m_pLabel->destroy();

    m_fMinHeight = 0;
    m_bComputeBeatSize = true;

    m_vfOffsets.resize(0);
    m_vfIndices.resize(0);

    m_fScaleX = 1.0f;
    m_fScaleY = 1.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN u8string get_text(ase::cor::mapping_c const& a_rMapping, c_ui32 a_ui32Emphasis)
{
    u8string sText; // @@0 cache
    sText.append_char(a_rMapping.emphasis_to_keychar(a_ui32Emphasis));
    return sText;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_float fStartTime = start_time();
    c_float fEndTime = end_time();

    { c_size stLabelCount = m_vpLabels.count();
    for (size_t stIndex = 0; stIndex < stLabelCount; ++stIndex)
    {
        emp::gui::label_c* pLabel = m_vpLabels.val(stIndex);
        pLabel->state().visible_(false);
        m_vAddresses.set(stIndex, emp::tt::max<size_t>::value);
    } }

    m_vfOffsets.resize(0);
    m_vfIndices.resize(0);

    c_float fWidth = transformation().width();

    c_float fSpace = space_height();
    c_float fNoteHeight = note_height();
    float fMaxY = fNoteHeight + fSpace;

    emp::snd::track_c const* const pTrack = m_pTrack;
    if (pTrack)
    {
        c_size stNoteCount = pTrack->notes().count();
        for (size_t stIndex = 0; stIndex < stNoteCount; ++stIndex)
        {
            emp::snd::note_c const oNote(pTrack->notes().ref(stIndex));

            emp::tim::time_c const oTime(oNote.time);
            emp::tim::time_c const oDuration(oNote.duration);

            c_float fTime = emp::tim::time_c::to_seconds(oTime);
            c_float fDuration = emp::tim::time_c::to_seconds(oDuration);

            c_float fTimeDelta = m_fTime - fTime;
            c_float fFinalDuration = emp::tt::equal_(fDuration, 0.0f) ? emp::tt::maxof(fTimeDelta, 0.0f) : fDuration;

            u8string sNote = get_text(*m_pMapping, oNote.index);
            m_pLabel->set_text(sNote);
            EMP_RET_FALSE_IF_NOT(m_pLabel->resize());

            c_float fSizeDuration = size_to_time(m_pLabel->transformation().width());

            float fNoteStart = fTime;
            float fNoteEnd = fTime + emp::tt::maxof(fFinalDuration, fSizeDuration);

            c_bool bEnded = fNoteStart > fEndTime;
            if (bEnded)
            { break; }

            c_bool bStarted = fNoteEnd < fStartTime;

            if (m_bEdition)
            { m_vfIndices.add(emp::ast::cast<float>(max_note() - oNote.index)); }
            else
            {
                bool bFound = false;
                c_size stOffsetCount = m_vfOffsets.count();
                for (size_t stOffset = 0; stOffset < stOffsetCount; ++stOffset)
                {
                    float fOffset = m_vfOffsets.val(stOffset);
                    if (fNoteStart > fOffset)
                    {
                        m_vfOffsets.set(stOffset, fNoteEnd);
                        m_vfIndices.add(emp::ast::cast<float>(stOffset));
                        bFound = true;
                        break;
                    }
                }
                if (bFound == false)
                {
                    m_vfIndices.add(emp::ast::cast<float>(m_vfOffsets.count()));
                    m_vfOffsets.add(fNoteEnd);
                }
            }

            if (bStarted == false)
            {
                emp::gui::label_c* const pLabel = add_label(stIndex, sNote, m_fScaleX, m_fScaleY);

                c_float fOffset = fEndTime - fTime;
                c_float fW = time_to_size(fFinalDuration);
                c_float fX = fWidth - time_to_size(fOffset);
                c_float fY = m_vfIndices.val(stIndex)*(2*fSpace+fNoteHeight)+fSpace;

                pLabel->transformation().width_(fW).position_(fX, -fY); // @@0 glcoord
                EMP_RET_FALSE_IF_NOT(pLabel->resize());

                fMaxY = emp::tt::maxof(fMaxY, fY+pLabel->transformation().height());
            }
        }
    }

    { c_size stLabelCount = m_vpLabels.count();
    emp::gui::label_c const* pFirstLabel = nullptr;
    for (size_t stIndex = 0; stIndex < stLabelCount; ++stIndex)
    {
        emp::gui::label_c* const pLabel = m_vpLabels.val(stIndex);
        if (pLabel == pFirstLabel)
        { break; }
        if (pLabel->state().visible == false)
        {
            if (pFirstLabel == nullptr)
            { pFirstLabel = pLabel; }
            m_vpLabels.remove(stIndex);
            m_vpLabels.add(pLabel);

            c_size stAddress = m_vAddresses.val(stIndex);
            m_vAddresses.remove(stIndex);
            m_vAddresses.add(stAddress);
        }
    } }

    if (state().autosize.y)
    {
        c_float fNewHeight = emp::tt::maxof<float>(fMaxY + fSpace,
            emp::ast::cast<float>(m_stMinNoteCount) * (2 * fSpace + fNoteHeight));
        transformation().height_(fNewHeight);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false)
    { return true; }

    bool bEmpty = false;
    EMP_GUI_MANAGER_CLIP(oScissor, false, transformation(), bEmpty, 0, 0, 0, 0);
    if (bEmpty)
    { return true; }

    auto const& rMatrix = transformation().matrix;
    auto& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_test(true); // @@0 depth
    rDevice.set_depth_mask(true);

    rDevice.set_vertex_buffer(&*m_pImMeshVertices);
    rDevice.set_index_buffer(&*m_pImMeshIndices);
    rDevice.set_program(rDevice.color_attribute_program());
    rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
    rDevice.draw(emp::gfx::primitive_e::Lines);

    if (m_pSkin && m_pSkin->success)
    {
        rDevice.set_vertex_buffer(&*m_pSuccessMeshVertices);
        rDevice.set_index_buffer(&*m_pSuccessMeshIndices);
        rDevice.set_program(rDevice.color_uniform_program());
        rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
        rDevice.set_uniform(uniform_e::v4Color, m_pSkin->success->rgba);
        rDevice.draw(emp::gfx::primitive_e::Triangles);
    }

    if (m_pSkin && m_pSkin->failure)
    {
        rDevice.set_vertex_buffer(&*m_pFailureMeshVertices);
        rDevice.set_index_buffer(&*m_pFailureMeshIndices);
        rDevice.set_program(rDevice.color_uniform_program());
        rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
        rDevice.set_uniform(uniform_e::v4Color, m_pSkin->failure->rgba);
        rDevice.draw(emp::gfx::primitive_e::Triangles);
    }

    color().background.border.render(transformation(), depth(transformation().back_depth, 0));
    c_size stCount = m_vpLabels.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(m_vpLabels.val(stIndex)->render()); }

    if (m_pSkin && m_pSkin->time)
    {
        rDevice.set_vertex_buffer(&*m_pTimeMeshVertices);
        rDevice.set_index_buffer(&*m_pTimeMeshIndices);
        rDevice.set_program(rDevice.color_uniform_program());
        rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
        rDevice.set_uniform(uniform_e::v4Color, m_pSkin->time->rgba);
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e score_c::process_keyboard(emp::app::c_event_s& a_rEvent)
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
        state().focus)
    {
        if (eKey == emp::app::key_e::Left ||
            eKey == emp::app::key_e::Up)
        {
            if (bPressed)
            {
                if (bCtrl)
                { time_scale_up(); }
                else
                {
                    if (m_bEdition == false)
                    { scale_up_x(); }
                    scale_up_y();
                }
            }
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
        else if (
            eKey == emp::app::key_e::Right ||
            eKey == emp::app::key_e::Down)
        {
            if (bPressed)
            {
                if (bCtrl)
                { time_scale_down(); }
                else
                {
                    if (m_bEdition == false)
                    { scale_down_x(); }
                    scale_down_y();
                }
            }
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e score_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return emp::gui::return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    auto const& rMouse = a_rEvent.mouse;
    if (state().enable &&
        state().hover &&
        a_rEvent.mouse_pressed())
    {
        auto const& rKeyboard = a_rEvent.keyboard;
        if (rKeyboard.m_bAlt == false &&
            rKeyboard.m_bShift == false)
        {
            emp::app::c_button_e eButton = rMouse.m_eButton;
            if (eButton == emp::app::button_e::WheelUp)
            {
                if (rKeyboard.m_bCtrl)
                { time_scale_up(); }
                else
                {
                    if (m_bEdition == false)
                    { scale_up_x(); }
                    scale_up_y();
                }
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
            else if (eButton == emp::app::button_e::WheelDown)
            {
                if (rKeyboard.m_bCtrl)
                { time_scale_down(); }
                else
                {
                    if (m_bEdition == false)
                    { scale_down_x(); }
                    scale_down_y();
                }
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::reset(void)
{
    m_fTime = 0;

    m_stCurrentBeat = 0;
    m_stCurrentMesure = 0;

    m_fStartSize = 0;

    c_size stCount = m_vpLabels.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gui::label_c* const pLabel = m_vpLabels.val(stIndex);
        pLabel->state().visible_(false).enable_(false);

        m_vAddresses.set(stIndex, emp::tt::max<size_t>::value);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
size_t score_c::get_current_index(c_size a_stIndex, c_float a_fDuration) const
{
    c_float fTime = m_fTime;
    size_t stIndex = a_stIndex;
    while (true)
    {
        c_float fIndex = emp::ast::cast<float>(stIndex + 1);
        c_float fIndexTime = fIndex * a_fDuration;
        if (fIndexTime >= fTime)
        {
            EMP_ASSERT(stIndex >= a_stIndex);
            return stIndex;
        }

        ++stIndex;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::label_c* score_c::add_label(c_size a_stAddress, c_u8string& a_sNote, c_float a_fScaleX, c_float a_fScaleY)
{
    emp::gui::label_c* pFound = nullptr;
    c_size stCount = m_vpLabels.count();

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (m_vAddresses.ref(stIndex) == a_stAddress)
        { pFound = m_vpLabels.val(stIndex); }
    }

    if (pFound == nullptr)
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::gui::label_c* const pLabel = m_vpLabels.val(stIndex);
            if (pLabel->state().visible == false)
            {
                m_vAddresses.set(stIndex, a_stAddress);
                pFound = pLabel;
                break;
            }
        }
    }

    if (pFound == nullptr)
    {
        pFound = EMP_ALLOCATE(emp::gui::label_c);
        // @@0 create pFound->create();
        pFound->state().autosize.y_(true);
        pFound->alignment_(alignment_e::MiddleLeft);
        pFound->scissor_(false);
        add_control(pFound);
        m_vpLabels.add(pFound);
        m_vAddresses.add(a_stAddress);
     }

    pFound->skin_(m_pSkin ? m_pSkin->label : nullptr).scale_(a_fScaleX, a_fScaleY).state().visible_(true);
    pFound->render_text_(m_bShowLetters);
    pFound->set_text(a_sNote);
    EMP_RET_NULL_IF_NOT(pFound->resize());

    return pFound;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_c::transform(emp::gui::stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    m_pImMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pImMeshIndices->create(emp::gfx::usage_e::Dynamic);

    m_stCurrentBeat = get_current_index(m_stCurrentBeat, beat_duration());
    m_stCurrentMesure = get_current_index(m_stCurrentMesure, mesure_duration());

    c_float fHeight = transformation().height();
    c_float fWidth = transformation().width();
    c_float fDepth = depth(transformation().fore_depth, 0);

    {
        c_float fSpace = space_height();
        c_float fNoteHeight = note_height();
        c_v4f v4Color = (m_pSkin && m_pSkin->beat) ? m_pSkin->beat->rgba : v4f::all(0);

        float fY = 0;//-fHeight;
        c_float fX = 0;
        while (fY < fHeight)
        {
            emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
            m_pImMeshVertices->add({     fX, -fY , fDepth}, v4Color);
            m_pImMeshVertices->add({ fWidth, -fY , fDepth}, v4Color);
           fY += fNoteHeight + 2 * fSpace;
        }
    }

    auto const lRenderBm = [this, fWidth, fHeight, fDepth](c_size a_stIndex, c_float a_fDuration, v4f::tparam a_v4Color)
    {
        c_float fSampleTime = emp::ast::cast<float>(a_stIndex) * a_fDuration;
        c_float fTimeOffset = m_fTime - fSampleTime;
        c_float fOffsetSize = time_to_size(fTimeOffset);
        c_float fStartPosition = m_fStartSize - fOffsetSize;

        {
            size_t stCount = 0;
            while (true)
            {
                c_float fCount = emp::ast::cast<float>(stCount);
                c_float fX = fStartPosition - time_to_size(a_fDuration * fCount);
                if (fX < 0)
                { break; }

                emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
                m_pImMeshVertices->add({ fX,        0, fDepth }, a_v4Color);
                m_pImMeshVertices->add({ fX, -fHeight, fDepth }, a_v4Color);
                ++stCount;
            }
        }

        {
            size_t stCount = 0;
            while (true)
            {
                c_float fCount = emp::ast::cast<float>(stCount + 1);
                c_float fX = fStartPosition + time_to_size(a_fDuration * fCount);
                if (fX > fWidth)
                { break; }

                emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
                m_pImMeshVertices->add({ fX,        0, fDepth }, a_v4Color);
                m_pImMeshVertices->add({ fX, -fHeight, fDepth }, a_v4Color);
                ++stCount;
            }
        }
    };

    lRenderBm(m_stCurrentBeat, beat_duration(), (m_pSkin && m_pSkin->beat) ? m_pSkin->beat->rgba : v4f::all(0));
    lRenderBm(m_stCurrentMesure, mesure_duration(), (m_pSkin && m_pSkin->beat) ? m_pSkin->mesure->rgba : v4f::all(0));

    c_float fStartSize = m_fStartSize;

    m_pTimeMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pTimeMeshIndices->create(emp::gfx::usage_e::Dynamic);
    emp::gfx::emit_line(0, *m_pTimeMeshIndices);
    m_pTimeMeshVertices->add({ fStartSize,        0, fDepth });
    m_pTimeMeshVertices->add({ fStartSize, -fHeight, fDepth });

    c_size stLabelCount = m_vpLabels.count();

    c_float fStartTime = start_time();
    c_float fEndTime = end_time();

    m_pFailureMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pFailureMeshIndices->create(emp::gfx::usage_e::Dynamic);

    m_pSuccessMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pSuccessMeshIndices->create(emp::gfx::usage_e::Dynamic);

    if (m_pTrack == nullptr)
    { return true; }

    if (m_pInterpretation != nullptr)
    {
        emp::snd::track_c const& rTrack = m_pInterpretation->failure();
        c_size stCount = rTrack.notes().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::snd::note_c const oNote(rTrack.notes().ref(stIndex));
            if (emp::tt::is_max(oNote.index))
            { continue; }

            emp::tim::time_c const oNoteTime(oNote.time);
            emp::tim::time_c const oNoteDuration(oNote.duration);
            emp::tim::time_c oNoteEnd;
            emp::tim::time_c::add(oNoteTime, oNoteDuration, oNoteEnd);

            c_float fNoteStart = emp::tim::time_c::to_seconds(oNoteTime);
            c_float fNoteEnd = emp::tim::time_c::to_seconds(oNoteEnd);

            if (fNoteEnd < fStartTime)
            { continue; }
            if (fNoteStart > fEndTime)
            { break; }

            c_float fNoteDuration = emp::tim::time_c::to_seconds(oNoteDuration);

            size_t EMP_TT_MAX_VAR(stBestFit);
            emp::tim::time_c oBestFit = emp::tim::time_c::max();

            for (size_t stLabel = 0; stLabel < stLabelCount; ++stLabel)
            {
                c_size stAddress = m_vAddresses.val(stLabel);
                if (emp::tt::is_max(stAddress))
                { break; }

                emp::snd::note_c const oPlayed(m_pTrack->notes().ref(stAddress));

                emp::tim::time_c const oPlayedTime(oPlayed.time);
                emp::tim::time_c const oPlayedDuration(oPlayed.duration);
                emp::tim::time_c oPlayedEnd;
                emp::tim::time_c::add(oPlayedTime, oPlayedDuration, oPlayedEnd);

                if (emp::tim::time_c::more_equal(oNoteTime, oPlayedTime) &&
                    (emp::tim::time_c::equal(oPlayedEnd, oPlayedTime) ||
                     emp::tim::time_c::less_equal(oNoteEnd, oPlayedEnd)) &&
                    emp::tim::time_c::less(oPlayedDuration, oBestFit))
                {
                    stBestFit = stLabel;
                    oBestFit = oPlayedDuration;
                }
            }

            c_float fLeft = fWidth - time_to_size(fEndTime - fNoteStart);
            c_float fRight = fLeft + time_to_size(fNoteDuration);
            float fBottom = 0;
            float fTop = 0;

            if (emp::tt::is_not_max(stBestFit))
            {
                auto const& rTransform = m_vpLabels.val(stBestFit)->transformation();
                fBottom = rTransform.y() - rTransform.height();
                fTop = rTransform.y();
            }
            else
            {
                fBottom = -space_height() - note_height();
                fTop = -space_height();;
            }

            emp::gfx::emit_quad(m_pFailureMeshVertices->count(), *m_pFailureMeshIndices);
            m_pFailureMeshVertices->add({ fLeft , fBottom, fDepth });
            m_pFailureMeshVertices->add({ fRight, fBottom, fDepth });
            m_pFailureMeshVertices->add({ fLeft , fTop   , fDepth });
            m_pFailureMeshVertices->add({ fRight, fTop   , fDepth });
        }
    }

    if (m_pInterpretation != nullptr)
    {
        emp::snd::track_c const& rTrack = m_pInterpretation->success();
        emp::cnt::vector<emp::snd::note_c> const& rvNotes = rTrack.notes();
        c_size stCount = rvNotes.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::snd::note_c const oNote(rvNotes.ref(stIndex));
            if (emp::tt::is_max(oNote.index))
            { continue; }

            emp::tim::time_c const oNoteTime(oNote.time);
            emp::tim::time_c const oNoteDuration(oNote.duration);
            emp::tim::time_c oNoteEnd;
            emp::tim::time_c::add(oNoteTime, oNoteDuration, oNoteEnd);

            c_float fNoteStart = emp::tim::time_c::to_seconds(oNoteTime);
            c_float fNoteEnd = emp::tim::time_c::to_seconds(oNoteEnd);

            if (fNoteEnd < fStartTime)
            { continue; }
            if (fNoteStart > fEndTime)
            { break; }

            c_float fNoteDuration = emp::tim::time_c::to_seconds(oNoteDuration);

            for (size_t stLabel = 0; stLabel < stLabelCount; ++stLabel)
            {
                c_size stAddress = m_vAddresses.ref(stLabel);
                if (emp::tt::is_max(stAddress))
                { break; }

                emp::snd::note_c const oPlayed(m_pTrack->notes().ref(stAddress));
                if (oNote.index != oPlayed.index)
                { continue; }

                emp::tim::time_c const oPlayedTime(oPlayed.time);
                emp::tim::time_c const oPlayedDuration(oPlayed.duration);
                emp::tim::time_c oPlayedEnd;
                emp::tim::time_c::add(oPlayedTime, oPlayedDuration, oPlayedEnd);

                if (emp::tim::time_c::more_equal(oNoteTime, oPlayedTime) &&
                    (emp::tim::time_c::equal(oPlayedEnd, oPlayedTime) ||
                     emp::tim::time_c::less_equal(oNoteEnd, oPlayedEnd)))
                {
                    c_float fLeft = fWidth - time_to_size(fEndTime - fNoteStart);
                    c_float fRight = fLeft + time_to_size(fNoteDuration);

                    auto const& rTransform = m_vpLabels.val(stLabel)->transformation();
                    c_float fBottom = rTransform.y() - rTransform.height();
                    c_float fTop = rTransform.y();

                    emp::gfx::emit_quad(m_pSuccessMeshVertices->count(), *m_pSuccessMeshIndices);
                    m_pSuccessMeshVertices->add({ fLeft , fBottom , fDepth });
                    m_pSuccessMeshVertices->add({ fRight, fBottom , fDepth });
                    m_pSuccessMeshVertices->add({ fLeft , fTop    , fDepth });
                    m_pSuccessMeshVertices->add({ fRight, fTop    , fDepth });
                    break;
                }
            }
        }
    }

    c_v4f v4BorderColor = (m_pSkin && m_pSkin->border) ? m_pSkin->border->rgba : v4f::all(0);
    c_v4f v4RasterColor = v4f::mul((m_pSkin && m_pSkin->success) ? m_pSkin->success->rgba : v4f::all(0), { 1, 1, 1, 0.3f });

    for (size_t stIndex = 0; stIndex < stLabelCount; ++stIndex)
    {
        emp::gui::label_c const* const pLabel = m_vpLabels.val(stIndex);
        if (pLabel->state().visible == false)
        {
            EMP_ASSERT(emp::tt::is_max(m_vAddresses.ref(stIndex)));
            break;
        }

        // @@0  : llvm unused ?
        //emp::snd::note_c const oNote(
          //  m_pTrack->notes().ref(m_vAddresses.ref(stIndex)));

        auto const& rLabelTransform = pLabel->transformation();

        c_float fLabelWidth = rLabelTransform.width();
        // @@0 remove c_float fLabelHeight = rLabelTransform.height();

        c_float fLeft = rLabelTransform.x();
        c_float fRight = fLeft + fLabelWidth;

        c_float fBottom = rLabelTransform.y() - rLabelTransform.height();
        c_float fTop = rLabelTransform.y();

        if (m_bEdition)
        {
            emp::gfx::emit_quad(m_pSuccessMeshVertices->count(), *m_pSuccessMeshIndices);
            m_pSuccessMeshVertices->add({ fLeft , fBottom , fDepth });
            m_pSuccessMeshVertices->add({ fRight, fBottom , fDepth });
            m_pSuccessMeshVertices->add({ fLeft , fTop    , fDepth });
            m_pSuccessMeshVertices->add({ fRight, fTop    , fDepth });
        }

        emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
        m_pImMeshVertices->add({ fLeft, fTop    , fDepth }, v4BorderColor);
        m_pImMeshVertices->add({ fLeft, fBottom , fDepth }, v4BorderColor);

        if (m_bShowDurations)
        {
            emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
            m_pImMeshVertices->add({ fLeft , fTop , fDepth }, v4BorderColor);
            m_pImMeshVertices->add({ fRight, fTop , fDepth }, v4BorderColor);

            emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
            m_pImMeshVertices->add({ fLeft , fBottom , fDepth }, v4BorderColor);
            m_pImMeshVertices->add({ fRight, fBottom , fDepth }, v4BorderColor);

            if (m_bEdition)
            {
                c_float fBeat = emp::tim::time_c::to_seconds(
                    emp::tim::time_c(m_pTrack->beat_duration() / m_pTrack->beat_division()));
                c_float fBeatSize = time_to_size(fBeat);
                float fBeatX = fLeft + fBeatSize;
                while (fBeatX < fRight)
                {
                    emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
                    m_pImMeshVertices->add({ fBeatX, fTop   , fDepth }, v4RasterColor);
                    m_pImMeshVertices->add({ fBeatX, fBottom, fDepth }, v4RasterColor);
                    fBeatX += fBeatSize;
                }
                emp::gfx::emit_line(m_pImMeshVertices->count(), *m_pImMeshIndices);
                m_pImMeshVertices->add({ fRight, fTop   , fDepth }, v4BorderColor);
                m_pImMeshVertices->add({ fRight, fBottom, fDepth }, v4BorderColor);
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float score_c::get_time_offset(c_size a_stIndex, float const a_fDuration) const
{
    float const fIndex = emp::ast::cast<float>(a_stIndex);
    float const fTime = m_fTime;
    float const fCurrent = fIndex * a_fDuration;
    EMP_ASSERT(fTime >= fCurrent);

    float const fOffset = fTime - fCurrent;
    EMP_ASSERT(fOffset <= a_fDuration);
    return a_fDuration - fOffset;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::set_beat_per_minute(float const a_fValue)
{
    EMP_ASSERT(a_fValue > 0);
    float const fCurrent = m_fBeatPerMinute;
    if (emp::tt::not_equal_(a_fValue, fCurrent))
    {
        m_fBeatPerMinute = emp::tt::maxof(0.0f, a_fValue);
        m_stCurrentBeat = get_current_index(0, beat_duration());
        m_stCurrentMesure = get_current_index(0, mesure_duration());
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::set_beat_per_mesure(float const a_fValue)
{
    EMP_ASSERT(a_fValue > 0);
    float const fCurrent = m_fBeatPerMesure;
    if (emp::tt::not_equal_(a_fValue, fCurrent))
    {
        m_fBeatPerMesure = a_fValue;

        m_stCurrentBeat = get_current_index(0, beat_duration());
        m_stCurrentMesure = get_current_index(0, mesure_duration());
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_c::compute_beat_size(void)
{
    if (m_bComputeBeatSize == false)
    { return true; }

    // @@0 bug min, min, max => below min, max, max
    float EMP_TT_MIN_VAR(fMaxY);
    float EMP_TT_MIN_VAR(fMaxWidth);
    float EMP_TT_MAX_VAR(fMinDuration);
    
    if (m_pTrack != nullptr &&
        m_pMapping != nullptr)
    {
        emp::snd::track_c const* const pTrack = m_pTrack;
        emp::cnt::vector<emp::snd::note_c> const& rvNotes = pTrack->notes();
        c_size stNoteCount = rvNotes.count();
        for (size_t stIndex = 0; stIndex < stNoteCount; ++stIndex)
        {
            emp::snd::note_c const oNote(rvNotes.ref(stIndex));
            m_pLabel->set_text(get_text(*m_pMapping, oNote.index));
            EMP_RET_FALSE_IF_NOT(m_pLabel->resize());

            fMaxWidth =emp::tt::maxof(fMaxWidth, m_pLabel->transformation().width());

            fMinDuration = emp::tt::minof(fMinDuration, emp::tim::time_c::to_seconds(oNote.duration));

            fMaxY = emp::tt::maxof(fMaxY, m_pLabel->transformation().height());
        }
    }

    if (m_pMapping != nullptr)
    {
        if (emp::tt::is_min(fMaxY) ||
            emp::tt::is_max(fMaxWidth) ||
            emp::tt::is_max(fMinDuration))
        {
            c_ui32 ui32Count = emp::snd::constants::key_count;
            for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
            {
                m_pLabel->set_text(get_text(*m_pMapping, ui32Index));
                EMP_RET_FALSE_IF_NOT(m_pLabel->resize());

                fMaxWidth = emp::tt::maxof(fMaxWidth, m_pLabel->transformation().width());
                fMaxY = emp::tt::maxof(fMaxY, m_pLabel->transformation().height());
            }
        }
    }

    if (emp::tt::is_not_min(fMaxY) &&
        emp::tt::is_not_min(fMaxWidth))
    {
        //float const fBeatDuration = beat_duration();
        //float const fDuration =
        //    (emp::tt::equal_(fMinDuration,
        //          emp::tt::max<float>::get_value()) ?
        //        fBeatDuration : fMinDuration);
        //set_time_scale(fMaxWidth / fDuration);

        set_time_scale(0.3f * m_fBeatPerMinute / 128.0f);
        m_fMinHeight = fMaxY / (EMP_GUI_SKIN.resolution.scaled_ratio().y * m_fScaleY);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float score_c::space_height(void) const
{ return 0.01f; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float score_c::note_height(void) const
{ return m_fMinHeight * EMP_GUI_SKIN.resolution.scaled_ratio().y * m_fScaleY; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float score_c::compute_time_scale(void) const
{ return m_fTimeScale * EMP_GUI_SKIN.resolution.scaled_ratio().x * m_fScaleX; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::set_time_scale(float const a_fValue)
{
    float const fCurrent = emp::tt::minof(3.0f, emp::tt::maxof(0.03f, a_fValue));
    if (emp::tt::not_equal_(m_fTimeScale, fCurrent))
    {
        m_fTimeScale = fCurrent;

        m_stCurrentBeat = get_current_index(0, beat_duration());
        m_stCurrentMesure = get_current_index(0, mesure_duration());
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_c::set_track(emp::snd::track_c const* const a_pValue)
{
    m_pEditionTrack = nullptr;
    m_pTrack = a_pValue;
    if (a_pValue)
    { set_beat_per_minute(m_pTrack->beat_per_minute()); }
    EMP_RET_FALSE_IF_NOT(compute_beat_size());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool score_c::set_edition_track(emp::snd::track_c* const a_pValue)
{
    EMP_RET_FALSE_IF_NOT(set_track(a_pValue));
    m_pEditionTrack = a_pValue;
    c_size stCellCount = m_pEditionTrack->layout()->cell_count();
    set_min_note_count(emp::tt::is_max(stCellCount) ? 8 : stCellCount);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::set_skin(cpc_score_skin_s a_pValue)
{
    m_pSkin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    m_pLabel->skin_(a_pValue ? a_pValue->label : nullptr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t score_c::max_note(void) const
{
    return m_stMinNoteCount > 0 ?
        emp::tt::maxof(m_stMinNoteCount - 1, m_pTrack->max_index()) :
            m_pTrack->max_index();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float score_c::scale_x(void) const
{ return m_pLabel->scale_x; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float score_c::scale_y(void) const
{ return m_pLabel->scale_y; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::set_scale_x(float const a_fValue)
{
    m_fScaleX = emp::tt::minof(3.0f, emp::tt::maxof(0.01f, a_fValue));
    m_pLabel->scale_x_(m_fScaleX);
    c_size stCount = m_vpLabels.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpLabels.val(stIndex)->scale_x_(m_fScaleX); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::set_scale_y(float const a_fValue)
{
    m_fScaleY = emp::tt::minof(3.0f, emp::tt::maxof(0.01f, a_fValue));
    m_pLabel->scale_y_(m_fScaleY);
    c_size stCount = m_vpLabels.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpLabels.val(stIndex)->scale_y_(m_fScaleY); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::get_time_and_index(v2f::tparam a_v2Position, emp::tim::time_c& a_rTime, size_t& a_rstIndex) const
{
    c_v2f v2Client = emp::gui::gl_to_gui(a_v2Position, transformation().matrix);

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();

    c_float fX = emp::tt::minof(emp::tt::maxof(0.0f, v2Client.x), fWidth);
    c_float fY = emp::tt::minof(emp::tt::maxof(0.0f, fHeight + v2Client.y), fHeight);

    c_float fTimeOffset = size_to_time(fX);
    c_float fTime = m_fTime + fTimeOffset;

    c_ui64 uiTime = emp::tim::time_c::from_seconds(fTime).usec();

    c_size stNote = emp::ast::cast<size_t>(fY / (2 * space_height() + note_height()));

    c_size stIndex = max_note() - emp::tt::minof(max_note(), stNote);
    a_rTime = emp::tim::time_c(uiTime);
    a_rstIndex = stIndex;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t score_c::get_note(c_size a_stNote, v2f::tparam a_v2Position) const
{
    emp::tim::time_c oTime;
    size_t EMP_TT_MAX_VAR(stIndex);
    get_time_and_index(a_v2Position, oTime, stIndex);
    return m_pTrack->get_note(a_stNote, oTime, emp::tim::time_c(0), stIndex);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret score_c::get_note_position(c_size a_stNote) const
{
    emp::snd::note_c const& rNote = m_pTrack->notes().ref(a_stNote);
    return emp::gui::gui_to_gl(transformation().matrix,
        { time_to_size(emp::tim::time_c::to_seconds(rNote.time) - m_fTime), 0 });
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void score_c::remove_note(c_size a_stNote)
{ m_pEditionTrack->remove_note(a_stNote); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t score_c::add_note(v2f::tparam a_v2Position)
{
    emp::tim::time_c oTime;
    size_t EMP_TT_MAX_VAR(stIndex);
    get_time_and_index(a_v2Position, oTime, stIndex);

    emp::tim::time_c const oDuration(m_pTrack->beat_duration() / m_pTrack->beat_division() - 100);
    return m_pEditionTrack->add_note(oTime, oDuration, stIndex, true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t score_c::move_note(c_size a_stNote, v2f::tparam a_v2Offset, v2f::tparam a_v2Position)
{
    emp::tim::time_c oTime;
    size_t EMP_TT_MAX_VAR(stIndex);
    get_time_and_index(a_v2Position, oTime, stIndex);

    emp::tim::time_c const oOffset(emp::tim::time_c::from_seconds(size_to_time(a_v2Offset.x)));
    emp::tim::time_c const oSnapOffset(m_pTrack->snap_time(oOffset.usec()));

    return m_pEditionTrack->move_note(a_stNote,emp::tim::time_c::sub(oTime, oSnapOffset), stIndex, true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t score_c::scale_note(c_size a_stNote, v2f::tparam a_v2Position)
{
    emp::tim::time_c oTime;
    size_t EMP_TT_MAX_VAR(stIndex);
    get_time_and_index(a_v2Position, oTime, stIndex);
    return m_pEditionTrack->scale_note(a_stNote, oTime, stIndex, true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

