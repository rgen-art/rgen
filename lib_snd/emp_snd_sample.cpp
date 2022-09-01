//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_sample.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

#include "emp_fs_file.h"

#include "emp_hh_math.h"

#include "emp_mat_algebra.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
sample_c::sample_c(void):
    m_sName(),
    m_sFilename(),
    m_fDuration(0.0f),
    m_stSampleRate(),
    m_vpvfSamples()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool check_header_id(emp::fs::file_c& a_rFile, c_char a_c0, c_char a_c1, c_char a_c2, c_char a_c3);
EMP_INTERN EMP_RETURN bool check_header_id(emp::fs::file_c& a_rFile, c_char a_c0, c_char a_c1, c_char a_c2, c_char a_c3)
{
    char c0 = 0;
    char c1 = 0;
    char c2 = 0;
    char c3 = 0;

    EMP_RET_FALSE_IF_NOT(a_rFile.read_char(c0));
    EMP_RET_FALSE_IF_NOT(a_rFile.read_char(c1));
    EMP_RET_FALSE_IF_NOT(a_rFile.read_char(c2));
    EMP_RET_FALSE_IF_NOT(a_rFile.read_char(c3));

    if (c0 != a_c0 || c1 != a_c1 || c2 != a_c2 || c3 != a_c3)
    {
        // @@0 alloc
        astring sLookingId;
        sLookingId.append_char(c0);
        sLookingId.append_char(c1);
        sLookingId.append_char(c2);
        sLookingId.append_char(c3);

        // @@0 alloc
        astring sHeaderId;
        sHeaderId.append_char(a_c0);
        sHeaderId.append_char(a_c1);
        sHeaderId.append_char(a_c2);
        sHeaderId.append_char(a_c3);

        EMP_LOG_INFO("Failed to read header id '#0', found '#1'", sHeaderId, sLookingId);

        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
File:
   Name:          resources/13_mcg_mf_033.wav
   File Size:     4201004
Format:
   Type:          Microsoft PCM
   Channels:      2
   Sample Rate:   44100 Hz
   Avg bytes/sec: 176400
   Block Align:   4 bytes
   Bit Width:     16
   Channel Mask:  0x003
Data:
   Start:         44
   Data Size:     4200960
   Samples:       1050240
   Playing Time:  23.81 sec
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

EMP_RETURN bool sample_c::post_load(void)
{
    // @@0  no preloading
    return load();
}

EMP_RETURN bool sample_c::load(void)
{
    if (m_vpvfSamples.count() > 0)
    {
        return true;
    }

    emp::fs::file_c oFile;
    // @@0 alloc
    c_astring sFilename = STR_AFORMAT("data/#0", m_sFilename);        //"mcg_ff_044.wav");

    EMP_LOG_INFO("Loading #0", sFilename);
    if (oFile.create(sFilename, emp::fs::file_e::Binary, fs::create_e::Read) == false)
    {
        EMP_LOG_ERROR("Failed to open wave file : #0", sFilename);
        return false;
    }

    //RIFF HEADER
    //id 4
    EMP_RET_FALSE_IF_NOT(check_header_id(oFile, 'R', 'I', 'F', 'F'));

    //file size 4
    ui32 ui32RiffSize = 0;
    if (oFile.read_uint32(ui32RiffSize) == false)
    {
        EMP_LOG_ERROR("Failed to read size : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("File size : #0", ui32RiffSize);

    //WAVE HEADER
    //id 4
    EMP_RET_FALSE_IF_NOT(check_header_id(oFile, 'W', 'A', 'V', 'E'));

    //FORMAT HEADER
    //id 4
    EMP_RET_FALSE_IF_NOT(check_header_id(oFile, 'f', 'm', 't', ' '));

    //size 4
    ui32 ui32FormatSize = 0;
    if (oFile.read_uint32(ui32FormatSize) == false)
    {
        EMP_LOG_ERROR("Failed to read size : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("Size : #0", ui32FormatSize);

    //format 2
    ui16 ui16FormatFormat = 0;
    if (oFile.read_uint16(ui16FormatFormat) == false)
    {
        EMP_LOG_ERROR("Failed to read format : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("Format : #0", ui16FormatFormat);

    //channels 2
    ui16 ui16FormatChannels = 0;
    if (oFile.read_uint16(ui16FormatChannels) == false)
    {
        EMP_LOG_ERROR("Failed to read channels : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("Channels : #0", ui16FormatChannels);
    m_vpvfSamples.resize(ui16FormatChannels);
    c_size stChannelCount = m_vpvfSamples.count();
    // @@0 analyze stChannelCount is always 0 ?
    for (size_t stIndex = 0; stIndex < stChannelCount; ++stIndex)
    { m_vpvfSamples.set(stIndex,EMP_ALLOCATE(emp::cnt::vector<float>)); }

    //sample rate 4
    ui32 ui32FormatSampleRate = 0;
    if (oFile.read_uint32(ui32FormatSampleRate) == false)
    {
        EMP_LOG_ERROR("Failed to read sample rate : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("Sample rate : #0", ui32FormatSampleRate);
    m_stSampleRate = ui32FormatSampleRate;
    //average sample rate 4
    ui32 ui32FormatAverageSampleRate = 0;
    if (oFile.read_uint32(ui32FormatAverageSampleRate) == false)
    {
        EMP_LOG_ERROR("Failed to read average sample rate : #0", sFilename);
        return false;
    }

    //block align 2
    ui16 ui16FormatBlockAlign = 0;
    if (oFile.read_uint16(ui16FormatBlockAlign) == false)
    {
        EMP_LOG_ERROR("Failed to read block align : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("Block align : #0", ui16FormatBlockAlign);

    if ((ui16FormatBlockAlign % stChannelCount) != 0)
    {
        EMP_LOG_INFO("Invalid block align #0", ui16FormatBlockAlign);
        return false;
    }

    //bps 2
    ui16 ui16FormatBitPerSample = 0;
    if (oFile.read_uint16(ui16FormatBitPerSample) == false)
    {
        EMP_LOG_ERROR("Failed to read bit per sample : #0", sFilename);
        return false;
    }
    //EMP_LOG_INFO("Bit per sample : #0", ui16FormatBitPerSample);

    if (ui16FormatBitPerSample != 16)
    {
        EMP_LOG_ERROR("cannot handle #0 bit per sample", ui16FormatBitPerSample);
        return false;
    }

    //DATA HEADER
    //id 4
    EMP_RET_FALSE_IF_NOT(check_header_id(oFile, 'd', 'a', 't', 'a'));

    //size 4
    ui32 ui32DataSize = 0;
    if (oFile.read_uint32(ui32DataSize) == false)
    {
        EMP_LOG_ERROR("Failed to read data size : #0", sFilename);
        return false;
    }

    if ((ui32DataSize % stChannelCount) != 0)
    {
        EMP_LOG_ERROR("Invalid data size : #0", ui32DataSize);
        return false;
    }

    c_size stSizePerChannel = ui32DataSize / stChannelCount;

    c_size stBytePerSample = ui16FormatBitPerSample / 8u;
    c_size stComputedBPS = ui16FormatBlockAlign / stChannelCount;
    if (stBytePerSample != stComputedBPS)
    {
        EMP_LOG_INFO("Byte per sample count mismatch #0 != #1", stBytePerSample, stComputedBPS);
        return false;
    }

    if ((stSizePerChannel % stBytePerSample) != 0)
    {
        EMP_LOG_INFO("Invalid size per channel : #0", stSizePerChannel);
        return false;
    }

    c_size stSampleCount = stSizePerChannel / stBytePerSample;
    emp::cnt::vector<si16> vsiSamples;
    vsiSamples.resize(ui32DataSize);
    EMP_RET_FALSE_IF_NOT(oFile.read_sint16(vsiSamples.ref(0), ui32DataSize));

    for (size_t stChannel = 0; stChannel < stChannelCount; ++stChannel)
    { m_vpvfSamples.val(stChannel)->resize(stSampleCount); }

    for (size_t stSample = 0; stSample < stSampleCount; ++stSample)
    {
        for (size_t stChannel = 0; stChannel < stChannelCount; ++stChannel)
        {
            emp::cnt::vector<float> *pvfSamples = m_vpvfSamples.val(stChannel);
            EMP_ASSERT(pvfSamples != nullptr);

            /*float const fPI = 3.14f;
            float const f2PI = fPI * 2.0f;
            float const fFreq = 220.0f;
            float const fRate = 44100.0f;
            float const fTime =
                emp::ast::cast<float>(stSample) * f2PI * fFreq / fRate;
            //c_si16 si16Sample =
            //  emp::ast::cast<si16>(sin(fTime) * 20000.0f);*/
            c_si16 si16Sample = vsiSamples.val(stSample * stChannelCount + stChannel);

            //float const fSample = emp::ast::cast<float>(sin(fTime));
            c_float fSample = emp::ast::cast<float>(si16Sample) / 32768.0f;
            pvfSamples->set(stSample, fSample);
           // EMP_LOG_INFO("SAMPLE : #0", si16Sample);
            //if (stChannel == 0 &&
            //    stSample > 2200)
            //{ return false; }
        }
    }

/*    for (size_t stSample = 0; stSample < stSampleCount; ++stSample)
    {
        EMP_LOG_INFO("SAMPLE : #0", m_vpvfSamples.val(0)->val(stSample));
        if (stSample > 22000)
        { return false; }
    }*/

    m_fDuration = emp::ast::cast<float>(stSampleCount) / emp::ast::cast<float>(ui32FormatSampleRate);
    //data
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void sample_c::destroy(void)
{
    emp::cnt::foreach(m_vpvfSamples, emp::mem::safe_delete_reset_t());
    m_vpvfSamples.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_RETURN float sample_c::duration(void) const
//{ return m_fDuration; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t sample_c::sample_count(void) const
{
    if (m_vpvfSamples.count() > 0)
    { return m_vpvfSamples.val(0)->count(); }
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t sample_c::channel_count(void) const
{ return m_vpvfSamples.count(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float sample_c::solve(c_size a_stChannel, c_size a_stSample) const
{
    emp::cnt::vector<float>* pvfSamples = m_vpvfSamples.val(a_stChannel);
    EMP_ASSERT(pvfSamples != nullptr);

    c_size stSampleCount = pvfSamples->count();
    EMP_ASSERT(a_stSample < stSampleCount);

    c_size stSample = emp::tt::minof(stSampleCount, emp::tt::maxof<size_t>(a_stSample, 0));

    /*float const f2PI = 6.28f;
    float const fFreq = 220.0f;
    float const fRate = 44100.0f;
    float const fSampleTime =
        emp::ast::cast<float>(stSample) * f2PI * fFreq / fRate;
    float const fSample = emp::ast::cast<float>(sin(fSampleTime));*/

    float const fSample = pvfSamples->val(stSample);
    return fSample;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN emp::mat::v4f::tret sample_c::solve(
    c_size a_stChannel, float const a_fTime) const
{*/
/*    EMP_ASSERT(a_fTime >= 0.0f);
    EMP_ASSERT(a_fTime < m_fDuration);

    float const f2PI = 6.28f;
    float const fFreq = 220.0f;
    float const fRate = 44100.0f;
    float const fTime0 = f2PI * a_fTime * fFreq;
    float const fSample0 = emp::ast::cast<float>(sin(fTime0) * 0.3f);

    float const fTime =
        emp::tt::minof(m_fDuration, emp::tt::maxof(a_fTime, 0.0f));

    emp::cnt::vector<float>* pvfSamples = m_vpvfSamples.val(a_stChannel);
    EMP_ASSERT(pvfSamples);

    c_size stSampleCount = pvfSamples->count();
    c_size stSampleIndex = emp::ast::cast<size_t>(fTime *
        emp::ast::cast<float>(m_stSampleRate));
    EMP_ASSERT(stSampleIndex < stSampleCount);

    c_size stSample = emp::tt::minof(stSampleCount,
        emp::tt::maxof(stSampleIndex, 0u));

    float const fSampleTime =
        emp::ast::cast<float>(stSample) * f2PI * fFreq / fRate;
    //c_si16 si16Sample = emp::ast::cast<si16>(sin(fTime) * 20000.0f);
    //c_si16 si16Sample = vsiSamples.val(stSample);

            float const fSample1 = emp::ast::cast<float>(sin(fSampleTime) * 0.3f);
    EMP_LOG_INFO("SAMPLE #0, #1, #2", fSample0, fSample1, stSampleIndex);
    return emp::mat::v4f::make_all(fSample1);*/

 /*float const fTime =
        emp::tt::minof(m_fDuration, emp::tt::maxof(a_fTime, 0.0f));

    emp::cnt::vector<float>* pvfSamples = m_vpvfSamples.val(a_stChannel);
    EMP_ASSERT(pvfSamples);

    c_size stSampleCount = pvfSamples->count();
    c_size stSampleIndex = emp::ast::cast<size_t>(fTime *
        emp::ast::cast<float>(m_stSampleRate));
    EMP_ASSERT(stSampleIndex < stSampleCount);

    c_size stSample = emp::tt::minof(stSampleCount,
        emp::tt::maxof(stSampleIndex, 0u));

    float const fSample = pvfSamples->val(stSample);
    return emp::mat::v4f::make_all(fSample);

}
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

