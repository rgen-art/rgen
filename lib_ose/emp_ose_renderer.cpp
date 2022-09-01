//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_renderer.h"

#include "emp_ose_compiler.h"
#include "emp_ose_processor.h"
#include "emp_ose_program.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"

#include "emp_fs_file.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_skin.h"

#include <unistd.h> // @@0 hh
// #include <zlib.h>


#include "emp_png_util.h"
#include "emp_mat_random.h"


ui32 fuckfuck(ui32, ui8 const* const, ui32);
ui32 fuckfuck(ui32, ui8 const* const, ui32)
{
    return 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name im_operation_e
#define enum_type
#define enum_values\
    enum_value( none               )\
    enum_value( square             )\
    enum_value( triangle           )\
    enum_value( circle             )\
    enum_value( hsba               )\
    enum_value( hsb                )\
    enum_value( sba                )\
    enum_value( hs                 )\
    enum_value( hb                 )\
    enum_value( sb                 )\
    enum_value( sa                 )\
    enum_value( ba                 )\
    enum_value( hue                )\
    enum_value( saturation         )\
    enum_value( brightness         )\
    enum_value( alpha              )\
    enum_value( translate          )\
    enum_value( translate_xy       )\
    enum_value( translate_x        )\
    enum_value( translate_y        )\
    enum_value( rotate             )\
    enum_value( scale              )\
    enum_value( scale_xy           )\
    enum_value( scale_x            )\
    enum_value( scale_y            )\
    enum_value( flip               )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderlet_c::renderlet_c(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderlet_c::~renderlet_c(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderlet_c::reset(c_astring& a_rsMother, c_astring& a_rsFather, c_float a_fRatio)
{
    m_sMother = a_rsMother;
    m_sFather = a_rsFather;

    m_pProgram = nullptr;
    EMP_TT_MAX_VAR(m_stProgram);

    m_fUnit = im_stack_c::s_cfUnit;

    m_ui32Steps = 0;
    m_ui32Processed = 0;
    m_pImStack->clear();
    m_pImStack->scale(1, a_fRatio, 1);
    EMP_RET_FALSE_IF(m_pImStack->push() == return_e::Error);
    
    m_pvpImMeshes->resize(0);

    m_Stamp = emp::tim::time_c();

    m44f::identity(m_m44Transform);
    m44f::identity(m_m44PreviousTransform);

    m_v3Scale = {0, 0, 0};
    m_v3Rotate = {0, 0, 0};
    m_v3Translate = {0, 0, 0};
    m_v3Size = {0, 0, 0};

    m_v3PreviousScale = {0, 0, 0};
    m_v3PreviousRotate = {0, 0, 0};
    m_v3PreviousTranslate = {0, 0, 0};

    m_v4BackgroundColor = {0, 0, 0, s_fDefaultAlpha };
    m_dArea = 0;
    m_dAllArea = 0;

    m_bCompleted = false;
    m_bAutoScale = true;
    m_bFailed = false;

    if (emp::tt::is_not_max(m_uiTarget0))
    {
        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.unlock_framebuffer(m_uiTarget0));
        EMP_TT_MAX_VAR(m_uiTarget0);

    }
    else if (emp::tt::is_not_max(m_uiTarget1))
    {
        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.unlock_framebuffer(m_uiTarget1));
        EMP_TT_MAX_VAR(m_uiTarget1);
    }

    m_stTriangleAllCount = 0;
    m_stTriangleCount = 0;
    m_stBeautyCount = 0;
    m_stAllCount = 0;

    m_stCurrentTarget = 0;
    m_stAllocatedBytes = 0;
    m_stAllocatedCount = 0;

    m_stCrossCount = 0;
    m_stMutationCount = 0;

    m_Best = best_s();

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool im_stack_c::visible(void) const
{
    m44f const& rCurrent = current_matrix();
    c_float fRatio = 1.0f;//EMP_GUI_SKIN.resolution.current();
    c_float fMinX = 0.00001f * m_fMinScale;
    c_float fMinY = fMinX * fRatio;
    c_float fMaxX = 1000000;
    c_float fMaxY = fMaxX * fRatio;
    c_float fSizeX = rCurrent.f00 * rCurrent.f00 + rCurrent.f01 * rCurrent.f01 + rCurrent.f02 * rCurrent.f02;
    c_float fSizeY = rCurrent.f01 * rCurrent.f01 + rCurrent.f11 * rCurrent.f11 + rCurrent.f12 * rCurrent.f12;

    if (fSizeX < fMinX || fSizeX > fMaxX ||
        fSizeY < fMinY || fSizeY > fMaxY)
    { return false; }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderer_c::renderer_c(void):
    m_muiPixels(64),
    m_RefreshDirectoryTime(emp::tim::time_c::from_seconds(1.0f)),
    m_mInvalidPrograms(1024),
    m_mChangePrograms(8),
#if defined EMP_XX_DEBUG
    m_ui32RenderTriangles(256)
#else
    m_ui32RenderTriangles(4096)
#endif

{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_UNIT_TEST
EMP_RETURN bool renderer_c::unit_test(c_astring& a_sResourcePath, c_float a_fResolution, c_bool a_bKill, c_bool a_bLog, c_astring& a_sUnitFile, c_si32 a_si32Skip)
{
    EMP_RET_FALSE_IF_NOT(update(emp::tim::time_c::from_seconds(0.0/60.0f)));

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    struct unit_sig_s
    {
        astring m_sRawFilename{};
        astring m_sOseFilename{};
        astring m_sOsePath{};
        astring m_sPngPath{};

        ui64 m_ui64Name = 0;
        vector<ui32>* m_pvui32PngBuffer = nullptr;
        ui64 m_uiPngCrc = 0;

        astring m_sOseBuffer{};
        ui64 m_uiOseCrc = 0;

        ui32 width = 0;
        ui32 height = 0;
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    struct units_c
    {
        map_t<ui64, unit_sig_s> m_mCrcSigs{4096};
        vector<unit_sig_s> m_vSigs{};
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    auto const l_unit_test = [a_bKill, a_bLog, &a_sUnitFile, &a_sResourcePath, this, a_fResolution](c_astring& a_sError, auto const a_lUnitTest) EMP_RETURN -> bool
    {
        EMP_LOG_INFO("unit test : #0", a_sError);

        vector<astring> vsFilesU;
        astring sPath = STR_AFORMAT("#0ose/", a_sResourcePath);
        EMP_RET_FALSE_IF_NOT(emp::fs::list(sPath, ALITERAL("*.ose"), vsFilesU, false));

        auto const l_sort = [](unit_sig_s& a_rsLeft, unit_sig_s& a_rsRight) EMP_RETURN -> int
        {
            c_bool bLeftName = a_rsLeft.m_ui64Name > 0;
            c_bool bRightName = a_rsLeft.m_ui64Name > 0;

            if (bLeftName && bRightName)
            {
                if (a_rsLeft.m_ui64Name < a_rsRight.m_ui64Name)
                { return -1; }
                else if (a_rsLeft.m_ui64Name > a_rsRight.m_ui64Name)
                { return 1; }
                return 0;
            }
            else if (bLeftName && bRightName == false)
            {
                return 1;
            }
            else if (bLeftName == false && bRightName)
            {
                return -1;
            }

            c_astring& rsLeft = a_rsLeft.m_sOseFilename;
            c_astring& rsRight = a_rsRight.m_sOseFilename;
            c_size stLeftLen = rsLeft.len();
            c_size stRightLen = rsRight.len();
            if (stLeftLen > 0)
            {
                if (stRightLen > 0)
                {
                    c_size stCount = emp::tt::minof(stLeftLen, stRightLen);
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        auto const cLeft = rsLeft.get_char(stIndex);
                        auto const cRight = rsRight.get_char(stIndex);
                        if (cLeft == cRight)
                        {
                            continue;
                        }
                        else if (cLeft < cRight)
                        { return -1; }
                        else if (cLeft > cRight)
                        { return 1; }
                    }

                    if (stLeftLen > stRightLen)
                    { return 1; }
                    else if (stLeftLen < stRightLen)
                    { return -1; }
                    else
                    { return 0; }
                }
                else
                { return 1; }
            }
            else
            {
                if (stRightLen > 0)
                { return -1; }
                else
                { return 0; }
            }
        };

        units_c oSorted;
        c_size stCount = vsFilesU.count();

        astring sDna(ALITERAL("dna_"));
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_astring& sAdd = vsFilesU.ref(stIndex);
            EMP_RET_FALSE_IF_NOT(sAdd.len() > 0);

            astring sName(sAdd);

            ui64 ui64Name = 0;
            if (sAdd.len() > 8)
            {
                if (emp::str::ncmp(sAdd, sDna, 4))
                {
                    sName.trim_left(4);
                    sName.trim_right(4);
                    if (emp::str::to(sName, ui64Name) == false)
                    {
                        ui64Name = 0;
                    }
                }
            }


            unit_sig_s oSig;
            oSig.m_sOseFilename = sAdd;
            oSig.m_ui64Name = ui64Name;

            c_size stSortedCount = oSorted.m_vSigs.count();
            bool bInserted = false;
            if (stSortedCount > 0)
            {
                for (size_t stSorted = 0; stSorted < stSortedCount; ++stSorted)
                {
                    if (l_sort(oSig, oSorted.m_vSigs.ref(stSorted)) <= 0)
                    {
                        oSorted.m_vSigs.insert(stSorted, oSig);
                        bInserted = true;
                        break;
                    }
                }
            }
            if (bInserted == false)
            {
                oSorted.m_vSigs.add(oSig);
            }
        }

        EMP_RET_FALSE_IF(vsFilesU.count() != oSorted.m_vSigs.count());

        /*for (size_t stIndex = 0; stIndex < oSorted.m_vSigs.count(); ++stIndex)
        {
            EMP_LOG_INFO("SORT #0", oSorted.m_vSigs.ref(stIndex).m_sOseFilename);
        }*/
        vector<ui32> vui32Buffer;
        size_t stFailed = 0;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (a_sUnitFile.len() > 0 && stIndex > 0)
            { break; }

            auto const l_continue = [&a_sResourcePath, &vui32Buffer, a_bLog, &a_sUnitFile, &oSorted, a_fResolution, sPath, stCount, stIndex, &a_lUnitTest, this, &a_sError]() EMP_RETURN -> bool
            {
                renderlet_c oRenderlet;
                unit_sig_s& rSig = oSorted.m_vSigs.ref(stIndex);
                vui32Buffer.resize(0);
                rSig.m_pvui32PngBuffer = &vui32Buffer;

                rSig.m_sOseFilename = a_sUnitFile.len() > 0 ? a_sUnitFile : rSig.m_sOseFilename;
                rSig.m_sOsePath = STR_AFORMAT("#0#1", sPath, rSig.m_sOseFilename);


                rSig.m_sRawFilename = rSig.m_sOseFilename;
                EMP_RET_FALSE_IF_NOT(rSig.m_sRawFilename.len() >= 5);
                rSig.m_sRawFilename.trim_right(4);

                rSig.m_sPngPath = STR_AFORMAT("#0png/#1_#2x#3.png", a_sResourcePath, rSig.m_sRawFilename, m_ui32MaxWidth, m_ui32MaxHeight);

                if (a_bLog)
                { EMP_LOG_INFO("#0 #1/#2 : #3", a_sError, stIndex + 1, stCount, rSig.m_sOseFilename); }

                oRenderlet.m_sMother = rSig.m_sOseFilename;

                bool bDeallocRenderlet = false;
                {
                    auto const oFinalizeRenderlet = emp::tt::finally([&bDeallocRenderlet, &oRenderlet]()
                    { bDeallocRenderlet = oRenderlet.reset(astring(), astring(), 0); });

                    EMP_RET_FALSE_IF_NOT(load(oRenderlet, a_fResolution, false));
                    oRenderlet.m_pImStack->create();

                    EMP_RET_FALSE_IF(oRenderlet.m_pProgram == nullptr || emp::tt::is_max(oRenderlet.m_stProgram),
                                     "unit test failed #0 : renderlet creation failed", rSig.m_sOsePath);

                    bool bDealloc = true;
                    {
                        auto const oFinally = emp::tt::finally([&oRenderlet, &bDealloc, this]()
                        { bDealloc = m_pCompiler->dealloc_program(oRenderlet.m_stProgram); });
                        EMP_RET_FALSE_IF_NOT(a_lUnitTest(oSorted, oRenderlet, rSig, stIndex), "unit test failed #0 : #1", rSig.m_sOsePath, a_sError);
                    }

                    EMP_RET_FALSE_IF_NOT(bDealloc, "unit test failed #0: program destruction failed", rSig.m_sOsePath);
                }
                EMP_RET_FALSE_IF_NOT(bDeallocRenderlet, "unit test failed #0: renderlet destruction failed", rSig.m_sOsePath);

                EMP_RET_FALSE_IF_NOT(m_pCompiler->unit_test_leak());
                return true;
            };
            if (l_continue() == false)
            {
                ++stFailed;
                if (a_bKill) { break; }
            }
        }

        if (stFailed != 0)
        { EMP_LOG_ERROR("unit test failed #0 : #1 / #2", a_sError, stFailed, stCount); }
        else
        { EMP_LOG_INFO("unit test success #0 : #1 / #2", a_sError, stCount, stCount); }
        return stCount > 0 && stFailed == 0;
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    if (false == l_unit_test(ALITERAL("duplicate"), [](units_c& a_rvUnits, renderlet_c& a_rRenderlet EMP_UNUSED, unit_sig_s& a_rData, c_size a_stIndex EMP_UNUSED) EMP_RETURN -> bool
         {
            emp::fs::file_c oFile;
            astring sPath(a_rData.m_sOsePath);
            EMP_RET_FALSE_IF_NOT(oFile.create(sPath, emp::fs::file_e::Binary, fs::create_e::Read), "failed to open file '#0'", sPath);

            astring sBuffer;
            EMP_RET_FALSE_IF_NOT(oFile.read_all(sBuffer), "failed to read file '#0'", sPath);
            EMP_RET_FALSE_IF_NOT(sBuffer.len() > 0, "empty file '#0'", sPath);

             unsigned int uiLen;
             EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(sBuffer.len(), uiLen));
             a_rData.m_uiOseCrc = fuckfuck(0, reinterpret_cast<unsigned char const*>(sBuffer.cstr()), uiLen);

            vector<ui32> vui32Buffer;

            auto const it = a_rvUnits.m_mCrcSigs.get(a_rData.m_uiOseCrc);
            if (it.not_end())
            {
                auto& rSig = *it.value();
                EMP_RET_FALSE_IF_NOT(rSig.m_uiOseCrc == a_rData.m_uiOseCrc, "Crc mismatch #0, #1", rSig.m_sOsePath, a_rData.m_sOsePath);

                auto const l_png_crc = [](unit_sig_s const& a_rSig) EMP_RETURN -> ui64
                {
                    vector<ui8> vui8Data;
                    vector<ui8*> vpui8Data;
                    ui32 uiWidth = 0;
                    ui32 uiHeight = 0;
                    ui32 uiDepth = 0;

                    EMP_RET_X_IF_NOT(0u, emp::png::load(a_rSig.m_sPngPath, vui8Data, vpui8Data, uiWidth, uiHeight, uiDepth));
                    unsigned int uiCount = 0;
                    EMP_RET_X_IF_NOT(0u, emp::ast::try_cast(vui8Data.count(), uiCount));
                    EMP_RET_X_IF(0u, (uiCount != uiWidth * uiHeight * 3) || (uiCount > 0) == false);
                    return fuckfuck(0, reinterpret_cast<unsigned char const*>(vui8Data.first_ptr()), uiCount);
                };

                if (rSig.m_uiPngCrc == 0)
                { rSig.m_uiPngCrc = l_png_crc(rSig); }

                if (a_rData.m_uiPngCrc == 0)
                { a_rData.m_uiPngCrc = l_png_crc(a_rData); }

                EMP_RET_FALSE_IF_NOT(emp::fs::delete_file(a_rData.m_sOsePath));
                EMP_RET_FALSE_IF_NOT(emp::fs::delete_file(a_rData.m_sPngPath));
                EMP_RET_FALSE_IF_NOT(rSig.m_uiPngCrc == a_rData.m_uiPngCrc, "Crc mismatch #0, #1", rSig.m_sPngPath, a_rData.m_sPngPath);


                EMP_RET_FALSE_("duplicate crc #0, #1", rSig.m_sOseFilename, a_rData.m_sOseFilename);
            }
            a_rvUnits.m_mCrcSigs.set(a_rData.m_uiOseCrc, a_rData);
            return true;
         }))
    {
        EMP_LOG_ERROR("duplicate test failed...");
        if (a_bKill) { return false; }
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    size_t stIndex = 0;
    if (false == l_unit_test(ALITERAL("rename"), [this, a_sResourcePath, &stIndex](units_c& a_rvUnits EMP_UNUSED, renderlet_c& a_rRenderlet EMP_UNUSED, unit_sig_s& a_rData EMP_UNUSED, c_size a_stIndex EMP_UNUSED) EMP_RETURN -> bool
         {
             {
                 astring sName(a_rData.m_sRawFilename);
                 EMP_RET_FALSE_IF_NOT(sName.len() > 0);
                 if (sName.len() > 4 &&
                     emp::str::ncmp(sName, astring(ALITERAL("dna_")), 4))
                 {
                     sName.trim_left(4);
                     ui64 uiIndex = 0;
                     if (emp::str::to(sName, uiIndex))
                     { return true; }
                 }
             }

             astring sRawFilename;
             astring sOseFilename;
             astring sOsePath;
             astring sPngFilename;
             astring sPngPath;
             while (true)
             {
                 sRawFilename = STR_AFORMAT("dna_#0", stIndex);

                 sOseFilename = STR_AFORMAT("#0.ose", sRawFilename);
                 sOsePath = STR_AFORMAT("#0ose/#1", a_sResourcePath, sOseFilename);
                 bool bOseExists = false;
                 EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sOsePath, bOseExists));

                 sPngFilename = STR_AFORMAT("#0_#1x#2.png", sRawFilename, m_ui32MaxWidth, m_ui32MaxHeight);
                 sPngPath = STR_AFORMAT("#0png/#1", a_sResourcePath, sPngFilename);
                 bool bPngExists = false;
                 EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sPngPath, bPngExists));

                 EMP_RET_FALSE_IF(bPngExists && bOseExists == false);
                 if (bOseExists == false && bPngExists == false)
                 { break; }

                 ++stIndex;
             }

             EMP_LOG_INFO("MOVE #0 -> #1", a_rData.m_sOsePath, sOsePath);
             EMP_LOG_INFO("MOVE #0 -> #1", a_rData.m_sPngPath, sPngPath);
             EMP_RET_FALSE_IF_NOT(emp::fs::move_file(a_rData.m_sOsePath, sOsePath));
             EMP_RET_FALSE_IF_NOT(emp::fs::move_file(a_rData.m_sPngPath, sPngPath));
             a_rData.m_sRawFilename = sRawFilename;
             a_rData.m_sOseFilename = sOseFilename;
             a_rData.m_sOsePath = sOsePath;
             //a_rData.m_sPngFilename = sPngFilename;
             a_rData.m_sPngPath = sPngPath;
             return false;
         }))
    {
        EMP_LOG_ERROR("rename test failed...");
        if (a_bKill) { return false; }
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    auto const l_test_ose = [this](units_c& a_rvUnits EMP_UNUSED, renderlet_c& a_rRenderlet,
                                   unit_sig_s& a_rData, c_size a_stIndex EMP_UNUSED)
    {
        unit_sig_s oOseData;
        EMP_RET_FALSE_IF_NOT(save(a_rRenderlet, astring(), a_rData.m_sOseBuffer, false, true, true));
        emp::fs::file_c oOld;
        EMP_RET_FALSE_IF_NOT(oOld.create(a_rData.m_sOsePath, emp::fs::file_e::Binary, fs::create_e::Read), "failed to open file '#0'", a_rData.m_sOsePath);

        astring sOldBuffer;
        EMP_RET_FALSE_IF_NOT(oOld.read_all(sOldBuffer), "failed to read file '#0'", a_rData.m_sOsePath);
        if (emp::str::cmp(sOldBuffer, a_rData.m_sOseBuffer) == false)
        {
            astring sBuffer;
            EMP_RET_FALSE_IF_NOT(save(a_rRenderlet, a_rData.m_sOsePath, sBuffer, false, true, false));
            EMP_RET_FALSE_("ose mismatch #0", a_rData.m_sOsePath);
        }

        return true;
    };

    if (false == l_unit_test(ALITERAL("save"), l_test_ose))
    {
        EMP_LOG_ERROR("save test failed...");
        if (a_bKill) { return false; }
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    if (false == l_unit_test(ALITERAL("render"), [this, &a_sResourcePath, a_si32Skip, &l_test_ose](units_c& a_rvUnits EMP_UNUSED, renderlet_c& a_rRenderlet, unit_sig_s& a_rData, c_size a_stIndex) EMP_RETURN -> bool
    {
        auto const oDelta = emp::tim::time_c::from_seconds(1.0f/60.0f);

        if (a_si32Skip < 0)
        {
            bool bExists = false;
            EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(a_rData.m_sPngPath, bExists));
            if (bExists)
            { return true; }
        }
        else
        {
            size_t stSkip = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_si32Skip, stSkip));
            if (a_stIndex < stSkip)
            { return true; }
        }

        while (true)
        {
            EMP_RET_FALSE_IF_NOT(update(oDelta));
            reset();
            run(a_rRenderlet, sc_ui32RenderInstructions, sc_fRenderTime, sc_ui32RenderBytes);
            EMP_RET_FALSE_IF(a_rRenderlet.m_bFailed);

            transform(a_rRenderlet, {2, 2});

            m44f m44Identity;
            m44f::identity(m44Identity);
            EMP_RET_FALSE_IF_NOT(renderU(a_rRenderlet, m44Identity, m_ui32MaxWidth, m_ui32MaxHeight));

            EMP_GRAPHICS.end_of_frame();

            if (a_rRenderlet.m_bCompleted == true)
            { break; }

            usleep(100);
        }

        astring sPath;
        astring sFilename;

        EMP_RET_FALSE_IF(emp::tt::is_max(a_rRenderlet.m_uiTarget1));

        target_s oTarget;
        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.framebuffer(a_rRenderlet.m_uiTarget1, oTarget)); // @@0 not the final compo...
        a_rData.m_pvui32PngBuffer->resize(0);
        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.read_pixels(oTarget, *a_rData.m_pvui32PngBuffer, false));

        a_rData.width = oTarget.m_ui32Width;
        a_rData.height = oTarget.m_ui32Height;

#ifdef EMP_OSE_AUTOSCALE
        emp::tt::silent(l_test_ose(a_rvUnits, a_rRenderlet, a_rData));

#else
        EMP_RET_FALSE_IF_NOT(l_test_ose(a_rvUnits, a_rRenderlet, a_rData, a_stIndex));
#endif

        bool bExists = false;
        EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(a_rData.m_sPngPath, bExists));
/*        if (a_bSkip && bExists)
        { return true; }*/ // @@0 remove

        vector<ui8> vui8Data;
        vector<ui8*> vpui8Data;
        ui32 uiWidth = 0;
        ui32 uiHeight = 0;
        ui32 uiDepth = 0;

        if (bExists)
        {
            EMP_RET_FALSE_IF_NOT(emp::png::load(a_rData.m_sPngPath, vui8Data, vpui8Data, uiWidth, uiHeight, uiDepth));
            EMP_RET_FALSE_IF(a_rData.width != uiWidth || a_rData.height != uiHeight);
            c_size stCount = a_rData.m_pvui32PngBuffer->count();
            EMP_RET_FALSE_IF(((uiWidth * uiHeight) != stCount) || ((stCount > 0) == false));

            c_bool bPngDiff = [stCount, &a_rData, &vui8Data]() EMP_RETURN -> bool
            {
                for (size_t lstIndex = 0; lstIndex < stCount; ++lstIndex)
                {
                    c_size stOffset = lstIndex * 3;
                    c_ui32 ui32Pixel = a_rData.m_pvui32PngBuffer->val(lstIndex);
                    c_ui8 uiR = vui8Data.val(stOffset + 0);
                    c_ui8 uiG = vui8Data.val(stOffset + 1);
                    c_ui8 uiB = vui8Data.val(stOffset + 2);
                    if (uiR != ((ui32Pixel >> 24) & 0xFF) ||
                        uiG != ((ui32Pixel >> 16) & 0xFF) ||
                        uiB != ((ui32Pixel >>  8) & 0xFF))
                    { return true; }
                }
                return false;
            }();

            if (bPngDiff)
            {
                size_t lstIndex = 0;
                astring sOldPngPath;
                while (true)
                {
                    sOldPngPath = STR_AFORMAT("#0png/#1_#2x#3.old_#4.png", a_sResourcePath, a_rData.m_sRawFilename, a_rData.width, a_rData.height, lstIndex);
                    bool bOldExists = false;
                    EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sOldPngPath, bOldExists));
                    if (bOldExists == false)
                    { break; }
                    ++lstIndex;
                }
                EMP_RET_FALSE_IF_NOT(emp::fs::copy_file(a_rData.m_sPngPath, sOldPngPath));
                EMP_RET_FALSE_IF_NOT(emp::png::save(a_rData.m_sPngPath, *a_rData.m_pvui32PngBuffer, vui8Data, a_rData.width, a_rData.height, false, fs::create_e::Trunc, false));
                EMP_RET_FALSE_("png mismatch #0", a_rData.m_sPngPath);
            }
            return true;
        }
        else
        {
            EMP_RET_FALSE_IF_NOT(emp::png::save(a_rData.m_sPngPath, *a_rData.m_pvui32PngBuffer, vui8Data, a_rData.width, a_rData.height, false, fs::create_e::New, false));
            EMP_RET_FALSE_("png not found #0", a_rData.m_sPngPath);
        }
        return true;
    }))
    {
        EMP_LOG_ERROR("image test failed...");
        if (a_bKill) { return false; }
    }

    return true;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN im_operation_e first_im_operation(void);
EMP_RETURN im_operation_e first_im_operation(void)
{ return im_operation_e::none; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::create(void)
{
    EMP_RET_FALSE_IF_NOT(m_pCompiler->create());

    emp::ose::table_c& rTable = m_pCompiler->m_Table;

    emp::ose::rti::data_c const* const pRealType = rTable.real_type();

    c_size stFloatCount = 4;
    for (size_t stIndex = 0; stIndex < stFloatCount; ++stIndex)
    {
        emp::ose::rti::function_c* pType = rTable.new_type<emp::ose::rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);

        for (size_t stFloat = 0; stFloat <= stIndex; ++stFloat)
        { EMP_RET_FALSE_IF(pType->add_parameter(pRealType) == false); }
        EMP_RET_FALSE_IF(rTable.qualify_type(pType) == false);
        EMP_RET_FALSE_IF(rTable.reg_type<emp::ose::rti::function_c>(pType) == false);
        // @@0 remove EMP_LOG_INFO("REGISTERED TYPE #0", pType->name());
    }

#define XTMP_GUI_REG(x_Extension, x_Name, x_Type)\
    case im_operation_e::x_Extension:\
    {\
        if (m_pCompiler->m_Table.reg_extension(enum_to__(im_operation_e::x_Extension), ALITERAL(x_Name), ALITERAL(x_Type), false) == nullptr)\
        {\
            EMP_LOG_ERROR("failed to register instruction im_operation_e::#0, #1, #2",\
                ALITERAL(EMP_PP_STRING(x_Extension)), ALITERAL(x_Name), ALITERAL(x_Type));\
            return false;\
        }\
    }

    switch (first_im_operation())
    {
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
        case im_operation_e::none: {}
        XTMP_GUI_REG(square             , "square"    , "()()")
        XTMP_GUI_REG(triangle           , "triangle"  , "()()")
        XTMP_GUI_REG(circle             , "circle"    , "()()")
        XTMP_GUI_REG(hsba               , "hsba"      , "()(float,float,float,float,)")
        XTMP_GUI_REG(hsb                , "hsb"       , "()(float,float,float,)")
        XTMP_GUI_REG(sba                , "sba"       , "()(float,float,float,)")
        XTMP_GUI_REG(hs                 , "hs"        , "()(float,float,)")
        XTMP_GUI_REG(hb                 , "hb"        , "()(float,float,)")
        XTMP_GUI_REG(sb                 , "sb"        , "()(float,float,)")
        XTMP_GUI_REG(sa                 , "sa"        , "()(float,float,)")
        XTMP_GUI_REG(ba                 , "ba"        , "()(float,float,)")
        XTMP_GUI_REG(hue                , "h"         , "()(float,)")
        XTMP_GUI_REG(saturation         , "u"         , "()(float,)")
        XTMP_GUI_REG(brightness         , "b"         , "()(float,)")
        XTMP_GUI_REG(alpha              , "a"         , "()(float,)")
        XTMP_GUI_REG(translate          , "t"         , "()(float,)")
        XTMP_GUI_REG(translate_xy       , "t"         , "()(float,float,)")
        XTMP_GUI_REG(translate_x        , "x"         , "()(float,)")
        XTMP_GUI_REG(translate_y        , "y"         , "()(float,)")
        XTMP_GUI_REG(rotate             , "r"         , "()(float,)")
        XTMP_GUI_REG(scale              , "s"         , "()(float,)")
        XTMP_GUI_REG(scale_xy           , "s"         , "()(float,float,)")
        XTMP_GUI_REG(scale_x            , "sx"        , "()(float,)")
        XTMP_GUI_REG(scale_y            , "sy"        , "()(float,)")
        XTMP_GUI_REG(flip               , "f"         , "()(float,)")
        default:
        { break; }
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
    };
#undef XTMP_REG

    // @@0 rt EMP_RET_FALSE_IF(m_pCompiler->end_create() == false)

    m_pProcessor->set_extension(emp::fct::function(this, &renderer_c::process));
    m_pProcessor->set_push(emp::fct::function(this, &renderer_c::push));
    m_pProcessor->set_pop(emp::fct::function(this, &renderer_c::pop));


    m_sPath.clear();
    emp::tt::silent(STR_FORMATA(m_sPath, "#0ose/", EMP_APPLICATION.resource_path()));

    // @@0 remove m_vsFiles.resize(0);
    // @@0 remove EMP_RET_FALSE_IF_NOT(emp::fs::list(m_sPath, ALITERAL("*.ose"), m_vsFiles, false));


    //m_mInvalidPrograms.set(ALITERAL("test.ose"), 0);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::destroy(void)
{
    emp::cnt::foreach(m_vpImMeshes, emp::mem::safe_delete_reset_t());
    m_vpImMeshes.resize(0);
    m_stImMesh = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_LOG(x_Log)\
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO x_Log;)
#define XTMP_STACK(x_Stack, x_From)\
{\
    auto* pImStack = &*m_pImStack;\
    XTMP_LOG(("IM_OPERATION_E:: begin #0 #1, #2", ALITERAL(EMP_PP_STRING(x_Stack)), ALITERAL(x_From), pImStack->count()))\
    emp::ose::return_e eReturn = pImStack->x_Stack();\
    if (eReturn != emp::ose::return_e::Continue)\
    { return eReturn; }\
    XTMP_LOG(("IM_OPERATION_E:: end #0 #1, #2", ALITERAL(EMP_PP_STRING(x_Stack)), ALITERAL(x_From), pImStack->count()))\
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e renderer_c::push(void)
{
    XTMP_STACK(push, "SCOPE")
    return emp::ose::return_e::Continue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e renderer_c::pop(void)
{
    XTMP_STACK(pop, "SCOPE")
    return emp::ose::return_e::Continue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::print_ose(renderlet_c& a_rRenderlet)
{
    astring sSource;
    EMP_ASSERT_BOOL_CALL(print_ose(a_rRenderlet, sSource));
    printf("%s", sSource.cstr());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::print_ose(renderlet_c& a_rRenderlet, astring& a_rsOutput)
{ return m_pCompiler->print_ose(a_rRenderlet.m_pProgram->m_stAst, a_rsOutput); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::is_beauty(renderlet_c& a_rRenderlet, best_s& a_rBest, c_bool EMP_SILENT(a_bLog))
{
    target_s oTarget;
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.framebuffer(a_rRenderlet.m_uiTarget1, oTarget));
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.read_pixels(oTarget, m_vui32Pixels, false));
    m_muiPixels.clear();

    c_size stPixelCount = m_vui32Pixels.count();

    float fRed = 0;
    float fGreen = 0;
    float fBlue = 0;
    float fAlpha = 0;
    float EMP_TT_MAX_VAR(fMinAlpha);
    float EMP_TT_MIN_VAR(fMaxAlpha);

    for (size_t stPixel = 0; stPixel < stPixelCount; ++stPixel)
    {
        c_ui32 ui32Pixel = m_vui32Pixels.val(stPixel);
        c_float r = static_cast<float>((ui32Pixel >> 0) & 0xFF) / 255.0f;
        c_float g = static_cast<float>((ui32Pixel >> 8) & 0xFF) / 255.0f;
        c_float b = static_cast<float>((ui32Pixel >> 16) & 0xFF) / 255.0f;
        c_float a = static_cast<float>((ui32Pixel >> 24) & 0xFF) / 255.0f;
        fRed += r;
        fGreen += g;
        fBlue += b;
        fAlpha += a;
        fMinAlpha = emp::tt::minof(a, fMinAlpha);
        fMaxAlpha = emp::tt::maxof(a, fMaxAlpha);

        c_size stCount = m_muiPixels.has(ui32Pixel) ? *m_muiPixels.get(ui32Pixel).value() : 1u;
        m_muiPixels.set(ui32Pixel, stCount);
    }

    c_float fPixelCount = emp::ast::cast<float>(stPixelCount);
    fRed /= fPixelCount;
    fGreen /= fPixelCount;
    fBlue /= fPixelCount;
    fAlpha /= fPixelCount;

    float fVarRed = 0;
    float fVarGreen = 0;
    float fVarBlue = 0;
    float fVarAlpha = 0;
    for (size_t stPixel = 0; stPixel < stPixelCount; ++stPixel)
    {
        c_ui32 ui32Pixel = m_vui32Pixels.val(stPixel);
        c_float r = static_cast<float>((ui32Pixel >> 0) & 0xFF) / 255.0f;
        c_float g = static_cast<float>((ui32Pixel >> 8) & 0xFF) / 255.0f;
        c_float b = static_cast<float>((ui32Pixel >> 16) & 0xFF) / 255.0f;
        c_float a = static_cast<float>((ui32Pixel >> 24) & 0xFF) / 255.0f;

        fVarRed += (r - fRed) * (r - fRed);
        fVarGreen += (g - fGreen) * (g - fGreen);
        fVarBlue += (b - fBlue) * (b - fBlue);
        fVarAlpha += (a - fAlpha) * (a - fAlpha);
    }

    fVarRed /= (fPixelCount - 1);
    fVarGreen /= (fPixelCount - 1);
    fVarBlue /= (fPixelCount - 1);
    fVarAlpha /= (fPixelCount - 1);

    fVarRed = emp::mat::sqr(fVarRed);
    fVarGreen = emp::mat::sqr(fVarGreen);
    fVarBlue = emp::mat::sqr(fVarBlue);
    fVarAlpha = emp::mat::sqr(fVarAlpha);


    a_rBest.m_pProgram = a_rRenderlet.m_pProgram;
    a_rBest.m_stProgram = a_rRenderlet.m_stProgram;
    a_rBest.m_sMother = a_rRenderlet.m_sMother;
    a_rBest.m_sFather = a_rRenderlet.m_sFather;
    a_rBest.m_fRed = fRed;
    a_rBest.m_fGreen = fGreen;
    a_rBest.m_fBlue = fBlue;
    a_rBest.m_fAlpha = fAlpha;
    a_rBest.m_fVarRed = fVarRed;
    a_rBest.m_fVarGreen = fVarGreen;
    a_rBest.m_fVarBlue = fVarBlue;
    a_rBest.m_fVarAlpha = fVarAlpha;
    a_rBest.m_stCount = m_muiPixels.count();
    a_rBest.m_stTriangleCount = a_rRenderlet.m_stTriangleCount;
    a_rBest.m_Stamp = a_rRenderlet.m_Stamp;

    a_rBest.m_bBeauty = false;
   if (/*fAlpha > 0.1f && (fRed > 0.1f || fGreen > 0.1f || fBlue > 0.1f) &&*/
        a_rRenderlet.m_stTriangleAllCount > 64 && a_rBest.m_stCount > 1)
    { a_rBest.m_bBeauty = true ; }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::update(emp::tim::time_c const&  a_rDelta)
{
    c_f64 dScreenWidth = EMP_APPLICATION.platform().screen_pixel_width();
    c_f64 dScreenHeight = EMP_APPLICATION.platform().screen_pixel_height();
    EMP_RET_FALSE_IF(dScreenWidth <= 0 || dScreenHeight <= 0);

    c_ui32 ui32ScreenWidth = emp::ast::cast<ui32>(dScreenWidth);

    ui32 ui32Scale = 1;
    while (ui32ScreenWidth > static_cast<ui32>(1 << ui32Scale))
    { ++ui32Scale; }

    m_ui32MaxWidth = 1 << ui32Scale;
    m_ui32MaxHeight = 1 << ui32Scale;

    m_stImMesh = 0;
    m_DeltaTime = a_rDelta;
    emp::tim::time_c::add(m_RefreshDirectoryTime, m_DeltaTime, m_RefreshDirectoryTime);

    /* @@0 remove if (emp::tim::time_c::more(m_RefreshDirectoryTime, emp::tim::time_c::from_seconds(2.0f)))
    {
        m_vsFiles.resize(0);
        if (emp::fs::list(m_sPath, ALITERAL("*.ose"), m_vsFiles, false))
        {
            size_t stIndex = 0;
            while (stIndex < m_vsFiles.count())
            {
                auto const it = m_mInvalidPrograms.get(m_vsFiles.ref(stIndex));
                if (it != m_mInvalidPrograms.end())
                {
                    m_vsFiles.remove_unordered(stIndex);
                }
                else
                { ++stIndex; }
            }
            m_RefreshDirectoryTime = emp::tim::time_c();
        }
    }*/

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::run(renderlet_c& a_rRenderlet, c_size a_stCount, c_float a_fSeconds, c_size a_stBytes)
{
    auto oFinally = emp::tt::finally([this]{
        m_pRenderlet = nullptr;
        m_pStack = nullptr;
        m_pImStack = nullptr;
        m_pvpImMeshes = nullptr;
    });
    m_pRenderlet = nullptr;
    m_pStack = nullptr;
    m_pImStack = nullptr;
    m_pvpImMeshes = nullptr;

    emp::ose::program_c* const pProg = a_rRenderlet.m_pProgram;

    c_bool bProgRunning = pProg != nullptr && a_rRenderlet.m_bCompleted == false;
    c_bool bStackRunning = a_rRenderlet.m_pStack->instruction_pointer() != pProg->exit_point();
    c_bool bBytesRunning = a_rRenderlet.m_stAllocatedBytes < a_stBytes;
    c_bool bTimeRunning = a_rRenderlet.m_ui32Steps == 0 && emp::tim::time_c::to_seconds(a_rRenderlet.m_Stamp) < a_fSeconds;
    c_bool bStepsRunning = a_rRenderlet.m_ui32Steps > 0 && a_rRenderlet.m_ui32Processed < a_rRenderlet.m_ui32Steps;

//EMP_LOG_INFO("TIME #0", emp::tim::time_c::to_seconds(a_rRenderlet.m_Stamp));
    if (bProgRunning && bStackRunning && bBytesRunning && (bTimeRunning || bStepsRunning))
    {
        //static ui32 ui32Completed = 0;
        c_float fNewStep = (1.0f * static_cast<float>(a_rRenderlet.m_ui32Processed)) / static_cast<float>(a_rRenderlet.m_ui32Steps);
        c_float fNewTime = emp::tim::time_c::to_seconds(a_rRenderlet.m_Stamp) / a_fSeconds;
        a_rRenderlet.m_fCompleted = (a_rRenderlet.m_ui32Steps > 0) ? fNewStep : fNewTime;

        //c_ui32 ui32NewStep = static_cast<ui32>(fNewStep * 100.0f);
        //c_ui32 ui32NewTime = static_cast<ui32>(fNewTime * 100.0f);

        /*c_ui32 ui32New = static_cast<ui32>(a_rRenderlet.m_fCompleted * 100.0);
        if (ui32New != ui32Completed)
        {
    EMP_LOG_INFO("#0, STACK #1/#2, BYTES #3, TIME #4, STEPS #5, COMPLETED #6, #7, #8",
                     a_rRenderlet.m_sMother,
                     a_rRenderlet.m_pStack->instruction_pointer(), pProg->exit_point(),
                     static_cast<ui32>(a_rRenderlet.m_stAllocatedBytes * 100.0f / a_stBytes),
                     static_cast<ui32>(fNewTime * 100), static_cast<ui32>(fNewStep * 100), a_rRenderlet.m_fCompleted, a_rRenderlet.m_ui32Processed, a_rRenderlet.m_ui32Steps);
                      //ui32NewTime, ui32NewStep);
            ui32Completed = ui32New;
        }*/

        m_pRenderlet = &a_rRenderlet;
        m_pStack = m_pRenderlet->m_pStack;
        m_pImStack = m_pRenderlet->m_pImStack;
        m_pvpImMeshes = m_pRenderlet->m_pvpImMeshes;

        m_pImStack->m_fMinScale = a_rRenderlet.m_fUnit;

        c_size stCurrentBytes = EMP_ALLOCATOR.current_bytes();
        c_size stCurrentCount = EMP_ALLOCATOR.current_count();

        emp::tim::time_c::add(a_rRenderlet.m_Stamp, m_DeltaTime, a_rRenderlet.m_Stamp);

        // @@0 remove a_rRenderlet.m_pImStack->frame();
        c_size stTriangleCount = m_stTriangleCount;
        c_si32 siCount = m_pProcessor->run(*pProg, *a_rRenderlet.m_pStack, static_cast<ui32>(a_stCount), a_rRenderlet.m_ui32Steps, a_rRenderlet.m_ui32Processed);
        if (m_stTriangleCount > stTriangleCount)
        { a_rRenderlet.m_stTriangleCount += (m_stTriangleCount - stTriangleCount); }

        c_size stNextBytes = EMP_ALLOCATOR.current_bytes();
        c_size stNextCount = EMP_ALLOCATOR.current_count();

        a_rRenderlet.m_stAllocatedBytes += stNextBytes > stCurrentBytes ? stNextBytes - stCurrentBytes : 0;
        a_rRenderlet.m_stAllocatedCount += stNextCount > stCurrentCount ? stNextCount - stCurrentCount : 0;

        if (siCount < 0)
        {
            m_v4ClearColor = m_bColor ? v4f({ 1, 1, 0, 1 }) : v4f({ 0, 0, 0, 1 });
            EMP_LOG_ERROR("Failed to run program '#0' x '#1'", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather);
            a_rRenderlet.m_bCompleted = true;
            a_rRenderlet.m_bFailed = true;
        }
        else if (siCount == 0 && a_rRenderlet.m_pImStack->count() != 2)
        {
            m_v4ClearColor = m_bColor ? v4f({ 1, 0, 1, 1 }) : v4f({ 0, 0, 0, 1 });
            EMP_LOG_ERROR("Failed to run program '#0' x '#1', invalid stack #2", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather, a_rRenderlet.m_pImStack->count());
            a_rRenderlet.m_bCompleted = true;
            a_rRenderlet.m_bFailed = true;
        }
        else
        {
            m_v4ClearColor = a_rRenderlet.m_v4BackgroundColor;
        }
    }
    else if (a_rRenderlet.m_bCompleted == false && pProg != nullptr)
    {
        m_v4ClearColor = a_rRenderlet.m_v4BackgroundColor;
        //if (a_rRenderlet.m_bSimulated == false)
        //{ EMP_LOG_INFO("stopping : '#0' x '#1'", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather); }
        a_rRenderlet.m_bCompleted = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::load_valid(renderlet_c& a_rRenderlet, c_float a_fRatio)
{
    c_size stCount = m_pvsOse->count();
    m_mChangePrograms.resize(emp::tt::maxof<size_t>(stCount / (stCount > 4u ? 4u : 1u), 4u));
    m_mChangePrograms.clear();

    while (true)
    {
        a_rRenderlet.m_sMother = astring();
        if (load(a_rRenderlet, a_fRatio, true) == false ||
            a_rRenderlet.m_pProgram == nullptr)
        {
            if (a_rRenderlet.m_sMother.len() > 0)
            { m_mChangePrograms.set(a_rRenderlet.m_sMother, 0); }
            if (m_mChangePrograms.count() == stCount)
            { return false; }
        }
        else
        { return true; }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::load(renderlet_c& a_rRenderlet, c_float a_fRatio, c_bool a_bInvalid)
{ return load(a_rRenderlet, a_fRatio, mutation_s(), crossover_s(), a_bInvalid); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::load(renderlet_c& a_rRenderlet, c_float a_fRatio, c_mutation_s& a_rMutation, c_crossover_s& a_rCrossover, c_bool a_bInvalid)
{
    astring& rsMother = a_rRenderlet.m_sMother;
    if (a_rCrossover.m_bXMother || emp::str::cmp(rsMother, astring()))
    { change(rsMother); }

    astring& rsFather = a_rRenderlet.m_sFather;
    if (a_rCrossover.m_bActive && (a_rCrossover.m_bXFather || emp::str::cmp(rsFather, astring())))
    { change(rsFather); }

    EMP_RET_FALSE_IF_NOT(a_rRenderlet.reset(rsMother, rsFather, a_fRatio));

    bool bDealloc = true;
    size_t EMP_TT_MAX_VAR(stFather);
    auto oFinallyFather = emp::tt::finally([&a_rRenderlet, this, stFather, &bDealloc]()
    {
        if (m_pCompiler->m_vpPrograms.is_valid_index(stFather))
        {
            bDealloc = m_pCompiler->dealloc_program(stFather);
            EMP_LOG_ERROR("Failed to deallocate program");
        }
        a_rRenderlet.m_pProgram = nullptr;
        EMP_TT_MAX_VAR(a_rRenderlet.m_stProgram);
    });

    if (rsFather.len() > 0)
    {
        size_t stCross = 0;
        size_t stMutation = 0;
        mutation_s oMutation;
        crossover_s oCrossover;

        if (m_mInvalidPrograms.get(rsFather).end())
        {
            stFather = m_pCompiler->from_file(STR_AFORMAT("#0/#1", m_sPath, rsFather), emp::tt::max<size_t>::value, oMutation, stMutation, oCrossover, stCross, false, emp::tt::max<size_t>::value);
            if (m_pCompiler->m_vpPrograms.is_valid_index(stFather))
            {
                //EMP_LOG_ERROR("Failed to build father '#0'", a_sFather);
                m_mInvalidPrograms.set(rsFather, true);
            }
        }
    }

    size_t stCrossCount = 0;
    size_t stMutationCount = 0;

    size_t EMP_TT_MAX_VAR(stMother);

    if (rsMother.len() > 0)
    {
        c_bool bRaw = rsFather.len() == 0 && a_rMutation.active() == false;
        if (bRaw == false || m_mInvalidPrograms.get(rsMother).end())
        {
            stMother = m_pCompiler->from_file(STR_AFORMAT("#0/#1", m_sPath, rsMother),
                stFather, a_rMutation, stMutationCount, a_rCrossover, stCrossCount, false, emp::tt::max<size_t>::value);
            if (bRaw && m_pCompiler->m_vpPrograms.is_valid_index(stMother) == false && a_bInvalid)
            {
                m_mInvalidPrograms.set(rsMother, true);
                EMP_LOG_INFO("INVALID #0", rsMother);
            }
        }
    }

    auto* const pMother = m_pCompiler->m_vpPrograms.is_valid_index(stMother) ? m_pCompiler->m_vpPrograms.val(stMother) : nullptr;
    a_rRenderlet.m_pProgram = pMother;
    a_rRenderlet.m_stProgram = stMother;
    a_rRenderlet.m_stCrossCount = stCrossCount;
    a_rRenderlet.m_stMutationCount = stMutationCount;

    if (pMother == nullptr)
    { return false; }

    a_rRenderlet.m_pStack->copy(pMother);
    //if (a_rRenderlet.m_bSimulated == false)
    //{ EMP_LOG_INFO("starting : '#0' x '#1'", rsMother, rsFather); }

    auto* pProgram = a_rRenderlet.m_pProgram;

    #define XTMP_LOAD(x_Name, x_Var)\
    {\
        error_e const eResult = m_pCompiler->push_data(*pProgram, ALITERAL(x_Name));\
        EMP_RET_FALSE_IF(eResult == error_e::CompilerError);\
        if (eResult == error_e::none)\
        { EMP_RET_FALSE_IF_NOT(m_pCompiler->m_pStack->val(0, x_Var)); }\
    }

// @@0 autoscale
    a_rRenderlet.m_v3Scale = {0, 0, 0};
    XTMP_LOAD("viewport_scale_x float", a_rRenderlet.m_v3Scale.x);
    XTMP_LOAD("viewport_scale_y float", a_rRenderlet.m_v3Scale.y);
    XTMP_LOAD("viewport_scale_z float", a_rRenderlet.m_v3Scale.z);
#ifdef EMP_OSE_AUTOSCALE
    a_rRenderlet.m_v3Scale = {0, 0, 0}; // @@0 remove
#endif

    a_rRenderlet.m_v3Rotate = {0, 0, 0};
    XTMP_LOAD("viewport_rotate_x float", a_rRenderlet.m_v3Rotate.x);
    XTMP_LOAD("viewport_rotate_y float", a_rRenderlet.m_v3Rotate.y);
    XTMP_LOAD("viewport_rotate_z float", a_rRenderlet.m_v3Rotate.z);
#ifdef EMP_OSE_AUTOSCALE
    a_rRenderlet.m_v3Rotate = {0, 0, 0}; // @@0 remove
#endif

    a_rRenderlet.m_v3Translate = {0, 0, 0};
    XTMP_LOAD("viewport_translate_x float", a_rRenderlet.m_v3Translate.x);
    XTMP_LOAD("viewport_translate_y float", a_rRenderlet.m_v3Translate.y);
    XTMP_LOAD("viewport_translate_z float", a_rRenderlet.m_v3Translate.z);
#ifdef EMP_OSE_AUTOSCALE
    a_rRenderlet.m_v3Translate = {0, 0, 0}; // @@0 remove
#endif

    a_rRenderlet.m_fUnit = im_stack_c::s_cfUnit;
    XTMP_LOAD("unit float", a_rRenderlet.m_fUnit);
#ifdef EMP_OSE_AUTOSCALE
    a_rRenderlet.m_fUnit = im_stack_c::s_cfUnit;
#endif

    ui32 ui32Seed = 0;
    XTMP_LOAD("seed uint", ui32Seed);
    pProgram->m_pRandom->reset_seed(ui32Seed);

    a_rRenderlet.m_ui32Steps = 0;
    XTMP_LOAD("steps uint", a_rRenderlet.m_ui32Steps);
#ifdef EMP_OSE_AUTOSCALE
    a_rRenderlet.m_ui32Steps = 0; // @@0 remove
#endif

    a_rRenderlet.m_v4BackgroundColor = { 0, 0, 0, s_fDefaultAlpha};
    XTMP_LOAD("clear_r float", a_rRenderlet.m_v4BackgroundColor.x);
    XTMP_LOAD("clear_g float", a_rRenderlet.m_v4BackgroundColor.y);
    XTMP_LOAD("clear_b float", a_rRenderlet.m_v4BackgroundColor.z);
    XTMP_LOAD("clear_a float", a_rRenderlet.m_v4BackgroundColor.w);
#ifdef EMP_OSE_AUTOSCALE
    a_rRenderlet.m_v4BackgroundColor = {0, 0, 0, s_fDefaultAlpha};
#endif

#undef XTMP_LOAD

    a_rRenderlet.m_bAutoScale = a_rRenderlet.m_v3Scale.x <= 0 || a_rRenderlet.m_v3Scale.y <= 0 || a_rRenderlet.m_v3Scale.z <= 0;

    oFinallyFather.release();
    return bDealloc;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::get_dnapath(astring& a_rsPath, astring& a_rsFilename)
{
    size_t stIndex = 0;
    astring sPath;
    astring sFilename;

    while (true)
    {
        bool bExists = false;
        sFilename = STR_AFORMAT("dna_#0.ose", stIndex);
        sPath = STR_AFORMAT("#0/#1", m_sPath, sFilename);

        EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sPath, bExists));
        if (bExists)
        { ++stIndex; }
        else
        {
            a_rsPath = sPath;
            a_rsFilename = sFilename;
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::get_filepath(c_astring& a_sFile, astring& a_rsFilepath)
{
    a_rsFilepath = STR_AFORMAT("#0/#1", m_sPath, a_sFile);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::save(renderlet_c& a_rRenderlet, c_astring& a_sFilename, astring& a_rsBuffer, c_bool a_bLog, c_bool a_bOverwrite, c_bool a_bPatch)
{
    EMP_RET_FALSE_IF(a_rRenderlet.m_pProgram == nullptr);
    EMP_RET_FALSE_IF(emp::tt::is_max(a_rRenderlet.m_pProgram->m_stAst));

    if (a_bPatch)
    {
        auto const l_add_data = [&a_rRenderlet, this](c_astring& a_rsName, auto a_tValue) EMP_RETURN -> bool
        {
            auto* const pProgram = a_rRenderlet.m_pProgram;
            c_size stAst = pProgram->m_stAst;
            EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stAst));

            auto const* const pType = pProgram->m_Table.get_data_type<decltype(a_tValue)>();
            EMP_RET_FALSE_IF(pType == nullptr);
            auto* const pLexeme = pProgram->m_Table.get_lexeme(a_rsName, pType);
            if (pLexeme == nullptr)
            {
                /*c_size stDeclare = m_pCompiler->alloc_node();
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stDeclare));
                EMP_RET_FALSE_IF_NOT(m_pCompiler->insert_child_node(stAst, stDeclare, 0));
                EMP_RET_FALSE_IF_NOT(m_pCompiler->fill_node(stDeclare, m_pCompiler->m_pAssign, 0, 0)); // @@1 invalid line/column number*/

                c_size stName = m_pCompiler->alloc_node(pProgram);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stName));

                m_pCompiler->m_vNodes.ref(stName).m_eType = node_e::DefinitionU;
                EMP_RET_FALSE_IF_NOT(m_pCompiler->insert_child_node(stAst, stName, 0));
                data_c* const pData = pProgram->m_Table.reg_data(a_rsName, pType);
                EMP_RET_FALSE_IF(pData == nullptr);
                pData->m_bLocal = false;
                m_pCompiler->m_vNodes.ref(stName).m_sName = a_rsName;
                EMP_RET_FALSE_IF_NOT(m_pCompiler->fill_node(stName, pData, { 0, 0})); // @@1 invalid line/column number

                c_size stLeftBrace = m_pCompiler->alloc_node(pProgram);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stLeftBrace));
                EMP_RET_FALSE_IF_NOT(m_pCompiler->add_child_node(stName, stLeftBrace));
                EMP_RET_FALSE_IF_NOT(m_pCompiler->fill_node(stLeftBrace, m_pCompiler->m_pLeftBrace, {0, 0})); // @@1 invalid line/column number

                c_size stLiteral = m_pCompiler->alloc_node(pProgram);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stLiteral));
                EMP_RET_FALSE_IF_NOT(m_pCompiler->add_child_node(stLeftBrace, stLiteral));

                auto* pLiteral = to_literal<decltype(a_tValue)>(pProgram->m_Table.get_lexeme(a_rsName, pType));
                if (pLiteral == nullptr)
                { pLiteral = pProgram->m_Table.reg_literal<decltype(a_tValue)>(a_tValue); }

                EMP_RET_FALSE_IF(pLiteral == nullptr);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->fill_node(stLiteral, pLiteral, {0, 0})); // @@1 invalid line/column number
            }
            else
            {
                c_size stName = pProgram->find_node(m_pCompiler, pLexeme);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stName));
                node_c& rName = m_pCompiler->m_vNodes.ref(stName);
                EMP_RET_FALSE_IF(rName.m_pvstLeaves == nullptr || emp::tt::is_max(rName.m_stLeaves));
                EMP_RET_FALSE_IF(rName.m_pvstLeaves->count() != 1);

                c_size stLeftBrace = rName.m_pvstLeaves->val(0);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stLeftBrace));
                node_c& rLeftBrace = m_pCompiler->m_vNodes.ref(stLeftBrace);
                EMP_RET_FALSE_IF(rLeftBrace.m_pvstLeaves == nullptr || emp::tt::is_max(rLeftBrace.m_stLeaves));
                EMP_RET_FALSE_IF(rLeftBrace.m_pvstLeaves->count() != 1);

                size_t stLiteral = rLeftBrace.m_pvstLeaves->val(0);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stLiteral));
                node_c* pLiteral = m_pCompiler->m_vNodes.ptr(stLiteral);
                EMP_RET_FALSE_IF(pLiteral->m_pLexeme == nullptr);

                if (pLiteral->m_pLexeme->m_eType == lexeme_e::Literal)
                {
                    EMP_RET_FALSE_IF(pLiteral->m_pvstLeaves != nullptr || emp::tt::is_not_max(pLiteral->m_stLeaves));
                }
                else
                {
                    EMP_RET_FALSE_IF(pLiteral->m_pvstLeaves == nullptr || emp::tt::is_max(pLiteral->m_stLeaves));
                    stLiteral = m_pCompiler->alloc_node(pProgram);
                    EMP_RET_FALSE_IF_NOT(m_pCompiler->m_vNodes.is_valid_index(stLiteral));
                    pLiteral = m_pCompiler->m_vNodes.ptr(stLiteral);
                    EMP_RET_FALSE_IF_NOT(m_pCompiler->set_child_node(stLeftBrace, stLiteral, 0));
                }

                EMP_RET_FALSE_IF(pLiteral == nullptr);

                auto* const pLiteralLexeme = pProgram->m_Table.reg_literal<decltype(a_tValue)>(a_tValue);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->fill_node(stLiteral, pLiteralLexeme, {0, 0}));

                pLiteral->m_sName = pLiteralLexeme->m_sName;
                pLiteral->m_pLexeme = pLiteralLexeme;

                //m_pCompiler->fill_node(stNewNode);
                //m_pCompiler->m_vNodes.ref(stNewiNode)->m_pLexeme = pLexeme;
            }


            return true;
        };

// @@0 remove        if (a_rRenderlet.m_v3Scale.x > 0 && a_rRenderlet.m_v3Scale.y > 0 && a_rRenderlet.m_v3Scale.z > 0)
         // @@0 unlink scale / rotate / translate
        {
            if (emp::mat::not_equal_(a_rRenderlet.m_v3Scale.x, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_scale_x"), a_rRenderlet.m_v3Scale.x)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Scale.y, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_scale_y"), a_rRenderlet.m_v3Scale.y)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Scale.z, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_scale_z"), a_rRenderlet.m_v3Scale.z)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Rotate.x, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_rotate_x"), a_rRenderlet.m_v3Rotate.x)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Rotate.y, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_rotate_y"), a_rRenderlet.m_v3Rotate.y)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Rotate.z, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_rotate_z"), a_rRenderlet.m_v3Rotate.z)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Translate.x, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_translate_x"), a_rRenderlet.m_v3Translate.x)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Translate.y, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_translate_y"), a_rRenderlet.m_v3Translate.y)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v3Translate.z, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("viewport_translate_z"), a_rRenderlet.m_v3Translate.z)); }

            //TODO:RG if (a_rRenderlet.m_pProgram->m_pRandom->seed != 0)
            //{ EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("seed"),a_rRenderlet.m_pProgram->m_pRandom->seed())); }

            if (emp::mat::not_equal_(a_rRenderlet.m_fUnit, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("unit"), a_rRenderlet.m_fUnit)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v4BackgroundColor.x, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("clear_r"), a_rRenderlet.m_v4BackgroundColor.x)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v4BackgroundColor.y, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("clear_g"), a_rRenderlet.m_v4BackgroundColor.y)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v4BackgroundColor.z, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("clear_b"), a_rRenderlet.m_v4BackgroundColor.z)); }

            if (emp::mat::not_equal_(a_rRenderlet.m_v4BackgroundColor.w, 0.0f))
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("clear_a"), a_rRenderlet.m_v4BackgroundColor.w)); }

            if (a_rRenderlet.m_ui32Steps == 0)
            { a_rRenderlet.m_ui32Steps = a_rRenderlet.m_ui32Processed; }
            if (a_rRenderlet.m_ui32Steps != 0)
            { EMP_RET_FALSE_IF_NOT(l_add_data(ALITERAL("steps"), a_rRenderlet.m_ui32Steps)); }
        }
    }

    //EMP_RET_FALSE_IF_NOT(m_pCompiler->print_ast(a_rRenderlet.m_pProgram->m_stAst, 0));

    EMP_RET_FALSE_IF_NOT(m_pCompiler->print_ose(a_rRenderlet.m_pProgram->m_stAst, a_rsBuffer));
    //EMP_LOG_DEBUG("ose : #0", sSource);

    emp::tt::silent(a_rsBuffer.cstr());

    if (a_sFilename.len() > 0)
    {
        emp::fs::file_c oFile;
        fs::create_e eCreate = fs::create_e::New;
        bool bExists = false;
        if (a_bOverwrite)
        {
            EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(a_sFilename, bExists)); // TODO:RG remove ?
        }
        if (bExists)
        { eCreate = fs::create_e::Trunc; }
        EMP_RET_FALSE_IF_NOT(oFile.create(a_sFilename, emp::fs::file_e::Binary, eCreate)); // @@0 save in devdir
        EMP_RET_FALSE_IF_NOT(oFile.write(a_rsBuffer));
    }

    if (a_bLog)
    { EMP_LOG_INFO("Saved : #0 ('#1' x '#2'), #3", a_sFilename, a_rRenderlet.m_sMother, a_rRenderlet.m_sFather, m_pvsOse->count()); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::change(astring& a_sFile)
{
    a_sFile = astring();
    while (true)
    {
        c_size stFileCount = m_pvsOse->count();
        if (stFileCount == 0)
        { break; }
        c_size stFile = stFileCount > 0 ? m_pRandom->rand<size_t>(0, stFileCount) : 0;
        if (m_pvsOse->is_valid_index(stFile))
        {
            c_astring& sFile = m_pvsOse->ref(stFile);
            //if (m_mInvalidPrograms.get(sFile) == m_mInvalidPrograms.end()) // @@0 bug when only invalid programs available
            {
                a_sFile = sFile;
                break;
            }
        }
    }

    //a_sFile = astring(ALITERAL("tgen_0000.txt"));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::transform(renderlet_c& a_rRenderlet, c_v2f& EMP_SILENT(a_v2Size))
{
    /*auto* const pVertices = &*a_rRenderlet.m_pBackgroundVertices;
    pVertices->create(emp::gfx::usage_e::Dynamic);

    c_float fRight = a_v2Size.x;
    c_float fTop = a_v2Size.y;
    pVertices->add(v2f::zero());
    pVertices->add({ fRight, 0 });
    pVertices->add({ 0, fTop });
    pVertices->add(a_v2Size);*/

    bool bChanged = false;
    m44f::identity(a_rRenderlet.m_m44PreviousTransform);

    auto const* const pImStack = &*a_rRenderlet.m_pImStack;

    if (a_rRenderlet.m_bAutoScale)
    {
        auto const v2Min = pImStack->min();
        if (bool_equal(v2Min, emp::tt::max<v2f>::value) == false)
        {
            c_float fX = pImStack->min().x;
            c_float fY = pImStack->min().y;
            c_float fWidth = pImStack->size().x;
            c_float fHeight = pImStack->size().y;

            c_float fNewWidth = 2 / fWidth;
            c_float fNewHeight = 2 / fHeight;
            c_float fNewSize = emp::tt::minof(fNewWidth, fNewHeight);

            c_float fNewX = /*1 / fNewSize*/ - (fX + 0.5f * fWidth);
            c_float fNewY = /*1 / fNewSize*/ - (fY + 0.5f * fHeight);

            a_rRenderlet.m_v3PreviousScale = a_rRenderlet.m_v3Scale;
            a_rRenderlet.m_v3PreviousRotate = a_rRenderlet.m_v3Rotate;
            a_rRenderlet.m_v3PreviousTranslate = a_rRenderlet.m_v3Translate;

            a_rRenderlet.m_v3Scale = {fNewSize, fNewSize, 1};
            a_rRenderlet.m_v3Rotate = {0, 0, 0};
            a_rRenderlet.m_v3Translate = {fNewX, fNewY, 0};
            a_rRenderlet.m_v3Size = {fWidth, fHeight, 2};
        }
        else
        {
            a_rRenderlet.m_v3Scale = {1, 1, 1};
            a_rRenderlet.m_v3Rotate = {0, 0, 0};
            a_rRenderlet.m_v3Translate = {0, 0, 0};
            a_rRenderlet.m_v3Size = {2, 2, 2};
        }

        if (emp::tt::equal_(a_rRenderlet.m_v3PreviousScale, {0, 0, 0}) == false &&
            (emp::tt::equal_(a_rRenderlet.m_v3PreviousScale, a_rRenderlet.m_v3Scale) == false ||
             emp::tt::equal_(a_rRenderlet.m_v3PreviousRotate, a_rRenderlet.m_v3Rotate) == false ||
             emp::tt::equal_(a_rRenderlet.m_v3PreviousTranslate, a_rRenderlet.m_v3Translate) == false))
        {
            m44f scaling;
            m44f::identity(scaling);
            c_float fInvScale = 1 / a_rRenderlet.m_v3PreviousScale.x;
            m44f::scale(scaling, { fInvScale, fInvScale, 1 }, scaling);

            m44f translate;
            m44f::identity(translate);
            m44f::translate(translate, v3f::neg(a_rRenderlet.m_v3PreviousTranslate), translate);

            m44f::mul(translate, scaling, a_rRenderlet.m_m44PreviousTransform);

            bChanged = true;
        }
    }

    m44f::type scaling;
    m44f::identity(scaling);
    m44f::scale(scaling, a_rRenderlet.m_v3Scale, scaling);

    m44f translate;
    m44f::identity(translate);
    m44f::translate(translate, a_rRenderlet.m_v3Translate, translate);

    m44f::mul(scaling, translate, a_rRenderlet.m_m44Transform);

    if (a_rRenderlet.m_bAutoScale)
    {
        if (bChanged)
        { m44f::mul(a_rRenderlet.m_m44Transform, a_rRenderlet.m_m44PreviousTransform, a_rRenderlet.m_m44PreviousTransform); }
    }
    else
    { m44f::identity(a_rRenderlet.m_m44PreviousTransform); }// @@0 remove = a_rRenderlet.m_m44Transform; }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::renderU(renderlet_c& a_rRenderlet, m44f const& a_rTransform, c_ui32 a_ui32W, c_ui32 a_ui32H)
{
    auto& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_mask(false); // @@0 depth
    rDevice.set_depth_test(false);
    rDevice.set_stencil_test(false);
    rDevice.set_dither(false);
    rDevice.set_polygon_smooth(false);
    rDevice.set_scissor(true);

    c_size stMeshCount = a_rRenderlet.m_pvpImMeshes->count();
    if (stMeshCount > 0)
    {
        static ui32 frames = 0;
        ++frames;
        //if (frames == 1000)
        //a_rRenderlet.m_bCompleted = true;
        m44f mCurrent;
        m44f::mul(a_rTransform, a_rRenderlet.m_m44Transform, mCurrent);

        m44f mPrevious;
        m44f::mul(a_rTransform, a_rRenderlet.m_m44PreviousTransform, mPrevious);
        //mPrevious = a_rTransform;

        emp::mat::swap(a_rRenderlet.m_uiTarget0, a_rRenderlet.m_uiTarget1);

        EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(a_ui32W, a_ui32H, false, a_rRenderlet.m_uiTarget1));
        target_s oTarget1;
        EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(a_rRenderlet.m_uiTarget1, oTarget1));
        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget1));
        EMP_RET_FALSE_IF_NOT(rDevice.clear_target(v4f::max({0, 0, 0, s_fDefaultAlpha}, m_v4ClearColor)));

        rDevice.set_blend(true);
        rDevice.set_depth_test(false); // @@0 depth
        rDevice.set_depth_mask(false);
        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(rDevice.texture_program());
        rDevice.set_uniform(uniform_e::m44Transform, mPrevious);
        rDevice.set_uniform(uniform_e::v4Color, v4f({1, 1, 1, 1}));


        if (emp::tt::is_not_max(a_rRenderlet.m_uiTarget0))
        {
            target_s oTarget0;
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(a_rRenderlet.m_uiTarget0, oTarget0));
            rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTarget0.m_ui32Texture);
            rDevice.draw(emp::gfx::primitive_e::Triangles);
            EMP_RET_FALSE_IF_NOT(rDevice.unlock_framebuffer(a_rRenderlet.m_uiTarget0));
            EMP_TT_MAX_VAR(a_rRenderlet.m_uiTarget0);
        }

        rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
        rDevice.set_program(rDevice.color_attribute_program());
        rDevice.set_uniform(uniform_e::m44Transform, mCurrent);

        for (size_t stMesh = 0; stMesh < stMeshCount; ++stMesh)
        {
            auto& rMesh = *a_rRenderlet.m_pvpImMeshes->val(stMesh);
            auto& rVertices = std::get<1>(rMesh);
            if (rVertices.count() > 0)
            {
                auto& rIndices = std::get<0>(rMesh);
                if (rIndices.count() > 0)
                {
                    rDevice.set_vertex_buffer(&rVertices);
                    rDevice.set_index_buffer(&rIndices);
                    rDevice.draw(emp::gfx::primitive_e::Triangles);
                }
            }
        }

        a_rRenderlet.m_stTriangleAllCount += a_rRenderlet.m_stTriangleCount;
        a_rRenderlet.m_dAllArea += a_rRenderlet.m_dArea;
        /*EMP_LOG_INFO("#0, #1, triangles : #2, #3, area : #4, #5",
            a_rRenderlet.m_sMother, a_rRenderlet.m_fSize,
            a_rRenderlet.m_stTriangleCount, a_rRenderlet.m_stTriangleAllCount,
            a_rRenderlet.m_dArea, a_rRenderlet.m_dAllArea);*/
        a_rRenderlet.m_stTriangleCount = 0;
        a_rRenderlet.m_dArea = 0;
        a_rRenderlet.m_pvpImMeshes->resize(0);
    }
    else if (emp::tt::is_max(a_rRenderlet.m_uiTarget1))
    {
        EMP_RET_FALSE_IF_NOT(rDevice.lock_framebuffer(a_ui32W, a_ui32H, false, a_rRenderlet.m_uiTarget1));
        target_s oTarget1;
        EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(a_rRenderlet.m_uiTarget1, oTarget1));
        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oTarget1));
        EMP_RET_FALSE_IF_NOT(rDevice.clear_target(v4f::max({0, 0, 0, s_fDefaultAlpha}, m_v4ClearColor)));
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::save(c_ui32 a_ui32Target, c_astring& a_sPath)
{
    target_s oTarget;
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.framebuffer(a_ui32Target, oTarget)); // @@0 not the final compo...
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.read_pixels(oTarget, m_vui32Pixels, false));
    EMP_RET_FALSE_IF_NOT(a_sPath.len() > 0);
/* 
c_size stPixelCount = emp::ast::cast<size_t>(rTarget.m_siWidth * rTarget.m_siHeight);
m_vui32Pixels.resize(stPixelCount);
EMP_GRAPHICS.glBindTexture_(GL_TEXTURE_2D, rTarget.m_ui32Texture);
EMP_GRAPHICS.glGetTexImage_(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, m_vui32Pixels.first_ptr());
*/

    bool bExists = false;
    EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(a_sPath, bExists));
    EMP_RET_FALSE_IF(bExists, "file exists #0", a_sPath);

    EMP_RET_FALSE_IF_NOT(a_sPath.len() > 0);
    EMP_RET_FALSE_IF_NOT(emp::png::save(a_sPath, m_vui32Pixels, m_vui8Data, oTarget.m_ui32Width, oTarget.m_ui32Height, true, fs::create_e::Trunc, false)); // @@0 save in devdir
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e renderer_c::process(emp::ose::program_c const& EMP_SILENT(a_rProgram), emp::ose::stack_c& a_rStack,
    c_ui32 a_ui32User)
{
    auto const& l_add_mesh = [this]()
    {
        if (m_pvpImMeshes->count() == 0 || std::get<0>(*m_pvpImMeshes->last_val()).count() >= 2048 * 3)
        {
            std::tuple<buffer_t<ui16>, buffer_t<v3f, v4f>>* pImMesh = get_mesh();
            m_pvpImMeshes->add(pImMesh);
            m_pImMeshIndices = &std::get<0>(*pImMesh);
            m_pImMeshVertices = &std::get<1>(*pImMesh);
            m_pImMeshIndices->create(emp::gfx::usage_e::Dynamic);
            m_pImMeshVertices->create(emp::gfx::usage_e::Dynamic);
        }
    };

    c_im_operation_e eImOperation = static_cast<im_operation_e>(a_ui32User); //@@0 enum_cast
    switch (eImOperation)
    {
        case im_operation_e::square:
        {
            XTMP_LOG(("IM_OPERATION_E::SQUARE"))
            XTMP_STACK(append, "SQUARE")
            //if (m_bProfile == false)
            {
                l_add_mesh();
                if (false == m_pImStack->emit_quad(
                    *m_pImMeshIndices, *m_pImMeshVertices, m_stTriangleCount, m_pRenderlet->m_dArea, m_stMaxTriangles))
                { return return_e::Break; }
            }
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::triangle:
        {
            XTMP_LOG(("IM_OPERATION_E::TRIANGLE"))
            XTMP_STACK(append, "TRIANGLE")
            //if (m_bProfile == false)
            {
                l_add_mesh();
                if (false == m_pImStack->emit_triangle(
                    *m_pImMeshIndices, *m_pImMeshVertices, m_stTriangleCount, m_pRenderlet->m_dArea, m_stMaxTriangles))
                { return return_e::Break; }
            }
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::circle:
        {
            XTMP_LOG(("IM_OPERATION_E::CIRCLE"))
            XTMP_STACK(append, "CIRCLE");
            //if (m_bProfile == false)
            {
                l_add_mesh();
                if (false == m_pImStack->emit_circle(
                    *m_pImMeshIndices, *m_pImMeshVertices, m_stTriangleCount, m_pRenderlet->m_dArea, m_stMaxTriangles))
                { return return_e::Break; }
            }
            return emp::ose::return_e::Continue;
        }
#undef XTMP_STACK

#define XTMP_POP_FLOAT(x_Name)\
        float x_Name = 0;\
        EMP_RET_X_IF_NOT(emp::ose::return_e::Error, a_rStack.pop(x_Name));

        case im_operation_e::hsba:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HSVA #0, #1, #2, #3", fH, fS, fV, fA))
            m_pImStack->update_color(fH, fS, fV, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hsb:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HSV #0, #1, #2", fH, fS, fV))
            m_pImStack->update_color(fH, fS, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::sba:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::SVA #0, #1, #2", fS, fV, fA))
            m_pImStack->update_color(0, fS, fV, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hs:
        {
            XTMP_POP_FLOAT(fS);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HS #0, #1", fH, fS))
            m_pImStack->update_color(fH, fS, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hb:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HV #0, #1", fH, fV))
            m_pImStack->update_color(fH, 0, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::sb:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::SV #0, #1", fS, fV))
            m_pImStack->update_color(0, fS, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::sa:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::SA #0, #1", fS, fA))
            m_pImStack->update_color(0, fS, 0, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::ba:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fV);
            XTMP_LOG(("IM_OPERATION_E::VA #0, #1", fV, fA))
            m_pImStack->update_color(0, 0, fV, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hue:
        {
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::H #0", fH))
            m_pImStack->update_color(fH, 0, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::saturation:
        {
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::S #0", fS))
            m_pImStack->update_color(0, fS, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::brightness:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_LOG(("IM_OPERATION_E::V #0", fV))
            m_pImStack->update_color(0, 0, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::alpha:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_LOG(("IM_OPERATION_E::ALPHA #0", fA))
            m_pImStack->update_color(0, 0, 0, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate:
        {
            XTMP_POP_FLOAT(fXY);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE #0", fXY))
            m_pImStack->translate(fXY, fXY, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate_xy:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE_XY #0 #1", fX, fY))
            m_pImStack->translate(fX, fY, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate_x:
        {
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE_X #0", fX))
            m_pImStack->translate(fX, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate_y:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE_Y #0", fY))
            m_pImStack->translate(0, fY, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::rotate:
        {
            XTMP_POP_FLOAT(fR);
            XTMP_LOG(("IM_OPERATION_E::ROTATE #0", fR))
            m_pImStack->rotate(fR * emp::mat::to_rad<float>());
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale:
        {
            XTMP_POP_FLOAT(fXY);
            XTMP_LOG(("IM_OPERATION_E::SCALE #0", fXY))
            m_pImStack->scale(fXY, fXY, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale_xy:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::SCALE_XY #0, #1", fX, fY))
            m_pImStack->scale(fX, fY, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale_x:
        {
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::SCALE_X #0", fX))
            m_pImStack->scale(fX, 1, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale_y:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_LOG(("IM_OPERATION_E::SCALE_Y #0", fY))
            m_pImStack->scale(1, fY, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::flip:
        {
            XTMP_POP_FLOAT(fF);
            XTMP_LOG(("IM_OPERATION_E::FLIP #0", fF))
            m_pImStack->flip(fF * emp::mat::to_rad<float>());
            return emp::ose::return_e::Continue;
        }
#undef XTMP_LOG
#undef XTMP_POP_FLOAT
        case im_operation_e::none:
        default:
        {
            EMP_LOG_ERROR("invalid im_operation_e #0", a_ui32User);
            return emp::ose::return_e::Error;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
