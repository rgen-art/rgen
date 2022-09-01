//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_player.h"

#include "emp_snd_media_i.h"

#include "emp_snd_mixer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

#include "emp_mat_evaluation.h"

#include "emp_mem_pointer.h"

#include "emp_tim_time.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
player_c::player_c(void):
    m_vpMedias(),
    m_vpRelease()
#ifdef EMP_SND_PROFILE_MEDIA
, m_pProfiler("media_c::move", emp::ast::cast<emp::tim::clock_c*>(nullptr))
#endif
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
player_c::~player_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::create(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::destroy(void)
{
    emp::cnt::foreach(m_vpRelease, emp::mem::safe_destroy_delete_reset_t());
    m_vpRelease.resize(0);
    m_vpMedias.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::update(void)
{
    size_t stIndexStop = 0;
    while (stIndexStop < m_vpMedias.count())
    {
        for (;stIndexStop < m_vpMedias.count(); ++stIndexStop)
        {
            media_i* const pMedia = m_vpMedias.val(stIndexStop);
            EMP_ASSERT(pMedia != nullptr);
            if (pMedia->state() == state_e::Stopped)
            {
                m_vpMedias.remove(stIndexStop);
                break;
            }
        }
    }
    size_t stIndexRelease = 0;
    while (stIndexRelease < m_vpRelease.count())
    {
        for (;stIndexRelease< m_vpRelease.count(); ++stIndexRelease)
        {
            media_i* const pMedia = m_vpRelease.val(stIndexRelease);
            EMP_ASSERT(pMedia != nullptr);
            if (pMedia->state() == state_e::Stopped)
            {
                pMedia->destroy();
                EMP_DEALLOCATE(pMedia);
                m_vpRelease.remove(stIndexRelease);
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::render(emp::snd::mixer_c& a_rMixer)
{
    { c_size stCount = m_vpMedias.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        media_i* const pMedia = m_vpMedias.val(stIndex);
        pMedia->pre_process();
    } }

    while (a_rMixer.free_samples())
    {
        EMP_ASSERT((a_rMixer.free_samples() % emp::mat::precision_c::data_stride) == 0);
        c_size stFree = a_rMixer.free_samples();
        float const fSampleDuration = a_rMixer.sample_duration();

        c_size stCount = m_vpMedias.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            media_i* const pMedia = m_vpMedias.val(stIndex);
            EMP_ASSERT(pMedia != nullptr);

            c_size stChannelCount = pMedia->channel_count();
            for (size_t stSample = 0; stSample < stFree;
                stSample+=emp::mat::precision_c::data_stride)
            {
                if (pMedia->state() == state_e::Stopped)
                { break; }

#ifdef EMP_SND_PROFILE_MEDIA
#define XTMP_RUNKILL
                //emp::mat::sse_v4f::test_sin();
    // @@0  : remove global
                m_pProfiler->set_time_manager(EMP_GAME().time_manager());
                while (true)
                {
                m_pProfiler->begin_sample();
                c_size stProfileCount = 44100
#ifdef XTMP_RUNKILL
                    * 60
#endif
                    ;
                for (size_t stProfile = 0; stProfile < stProfileCount;
                    stProfile+= emp::mat::precision_c::data_stride)
                { if (pMedia->move(fSampleDuration)) {} }
                m_pProfiler->end_sample();
                EMP_LOG_INFO(m_pProfiler->output(nullptr));
#ifdef XTMP_RUNKILL
                int *i = 0;
                *i = 0;
#endif
                }
#undef XTMP_RUNKILL
#endif //EMP_SND_PROFILE_MEDIA

                if (pMedia->move(fSampleDuration))
                {
                    if (stChannelCount == 1)
                    {
                        for (size_t stStride = 0;
                            stStride < emp::mat::precision_c::data_stride;
                            ++stStride)
                        {
                            float const fSample = pMedia->sample(stStride);

                            c_size stMixerOffset = stSample + stStride;
                            a_rMixer.add_to_sample(0, stMixerOffset, fSample);
                            a_rMixer.add_to_sample(1, stMixerOffset, fSample);
                        }
                    }
                    else if (stChannelCount == 2)
                    {
            //for (size_t stStride = 0;
            //    stStride < emp::mat::precision_c::data_stride * 2;
            //    ++stStride)
            //{
            //    a_rMixer.add_to_sample(
            //        stStride / emp::mat::precision_c::data_stride,
            //        stSample + (stStride % emp::mat::precision_c::data_stride),
            //        pMedia->sample(stStride));
            //}
                        for (size_t stStride = 0; stStride < emp::mat::precision_c::data_stride; ++stStride)
                        {
                            c_float fSample0 = pMedia->sample(stStride);
                            c_float fSample1 = pMedia->sample(stStride + emp::mat::precision_c::data_stride);

                            c_size stMixerOffset = stSample + stStride;
                            a_rMixer.add_to_sample(0, stMixerOffset, fSample0);
                            a_rMixer.add_to_sample(1, stMixerOffset, fSample1);
                        }
                    }
                }
            }
        }
        a_rMixer.render();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::play(emp::snd::media_i* const a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    c_size stCount = m_vpMedias.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        media_i* const pMedia = m_vpMedias.val(stIndex);
        EMP_ASSERT(pMedia != nullptr);
        if (pMedia == a_pValue)
        {
            EMP_ASSERT_UNREACHABLE();
            return;
        }
    }
    a_pValue->play();
    m_vpMedias.add(a_pValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::stop(emp::snd::media_i* const a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    c_size stCount = m_vpMedias.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        media_i* const pMedia = m_vpMedias.val(stIndex);
        EMP_ASSERT(pMedia != nullptr);
        if (pMedia == a_pValue)
        {
            //EMP_ASSERT(pMedia->state() == state_e::Playing);
            a_pValue->stop();
            return;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void player_c::release(emp::snd::media_i* const a_pValue)
{
    c_size stCount = m_vpRelease.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        media_i* const pMedia = m_vpRelease.val(stIndex);
        EMP_ASSERT(pMedia != nullptr);
        if (pMedia == a_pValue)
        {
            return;
        }
    }
    stop(a_pValue);
    m_vpRelease.add(a_pValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

