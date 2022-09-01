//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ttf_font.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_glyph.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0
#include "emp_ttf_font.h"
#include "emp_ttf_glyph.h"
#include "emp_ttf_outline.h"
#include "emp_ttf_point.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_fct_functor.h"

#include "emp_str_format.h"
#include "emp_str_from.h"
#include "emp_str_string.h"

#include "emp_xml_writer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ttf {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_CLASS(fterrno)
public:
    fterrno(FT_Error const& a_rftError):
    m_ftError(a_rftError)
    {}

public:
    EMP_RETURN FT_Error value(void) const
    { return m_ftError; }

private:
    FT_Error m_ftError;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace str {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_RETURN utf32_string from(
    utf32_string* const a_psOutput,
    emp::ttf::fterrno const& a_Errno, base_e::CValue EMP_SILENT(a_eBase))
{
    emp::str::utf32_string sTmp;
    emp::str::utf32_string* psOutput = a_psOutput ? a_psOutput : &sTmp;
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  case v: { psOutput->append_string(s); }
#define FT_ERROR_START_LIST     switch (a_Errno.value()) {
#define FT_ERROR_END_LIST       default: { EMP_ASSERT_UNREACHABLE(); break; } }
        #include FT_ERRORS_H
    return *psOutput;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_RETURN utf32_string format_parameter(emp::ttf::fterrno const& a_Errno)
{ return emp::str::from(nullptr, a_Errno); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ttf {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_CLASS(parser)
private:
    glyph_c& m_rGlyph;
    outline_c* m_pCurrentOutline;

    emp::fct::functor<outline_c* (*)()> const* m_pFctAllocOutline;

public:
    parser(glyph_c& a_rGlyph,
        emp::fct::functor<outline_c* (*)()> const* const a_pFctAllocOutline):
    m_rGlyph(a_rGlyph),
    m_pCurrentOutline(nullptr),
    m_pFctAllocOutline(a_pFctAllocOutline)
    {
    }

public:
    void move_to(FT_Vector const& a_rftTo)
    {
        m_pCurrentOutline = (*m_pFctAllocOutline)();
        m_rGlyph.outlines().add(m_pCurrentOutline);
        line_to(a_rftTo);
    }

    void line_to(FT_Vector const& a_rftTo)
    { point_to(a_rftTo, false); }

    void conic_to(  FT_Vector const& a_rftControl_0,
                    FT_Vector const& a_rftTo)
    {
        point_to(a_rftControl_0, true);
        point_to(a_rftTo, false);
    }

    void cubic_to(  FT_Vector const& a_rftControl_0,
                    FT_Vector const& a_rftControl_1,
                    FT_Vector const& a_rftTo)
    {
        point_to(a_rftControl_0, true);
        point_to(a_rftControl_1, true);
        point_to(a_rftTo, false);
    }

private:
    void point_to(FT_Vector const& a_rftTo, c_bool a_bControl)
    {
        EMP_ASSERT(m_pCurrentOutline);
        m_pCurrentOutline->points().add(
            emp::ttf::point_c(a_bControl,
                emp::mtx::v2f::make(
                    static_cast<float>(a_rftTo.x),
                    static_cast<float>(a_rftTo.y))));
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN int move_to(FT_Vector const* a_ftTo,
                                void* a_pUser)
{
    EMP_ASSERT(a_ftTo);
    EMP_ASSERT(a_pUser);

    parser* pParser = reinterpret_cast<parser*>(a_pUser);
    pParser->move_to(*a_ftTo);

    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN int line_to(FT_Vector const* a_ftTo,
                                void* a_pUser)
{
    EMP_ASSERT(a_ftTo);
    EMP_ASSERT(a_pUser);

    parser* pParser = reinterpret_cast<parser*>(a_pUser);
    pParser->line_to(*a_ftTo);

    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN int conic_to(   FT_Vector const* a_ftControl_0,
                                    FT_Vector const* a_ftTo,
                                    void* a_pUser)
{
    EMP_ASSERT(a_ftControl_0);
    EMP_ASSERT(a_ftTo);
    EMP_ASSERT(a_pUser);

    parser* pParser = reinterpret_cast<parser*>(a_pUser);
    pParser->conic_to(*a_ftControl_0, *a_ftTo);

    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN int cubic_to(   FT_Vector const* a_ftControl_0,
                                    FT_Vector const* a_ftControl_1,
                                    FT_Vector const* a_ftTo,
                                    void* a_pUser)
{
    EMP_ASSERT(a_ftControl_0);
    EMP_ASSERT(a_ftControl_1);
    EMP_ASSERT(a_ftTo);
    EMP_ASSERT(a_pUser);

    parser* pParser = reinterpret_cast<parser*>(a_pUser);
    pParser->cubic_to(*a_ftControl_0, *a_ftControl_1, *a_ftTo);

    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read(
    emp::str::utf32_string const& a_sFilename,
    font_c& a_rFont,
    c_size a_stSize,
    c_size a_stBegin,
    c_size a_stEnd,
    emp::fct::functor<emp::ttf::glyph_c* (*)()> const& a_rFctAllocGlyph,
    emp::fct::functor<emp::ttf::outline_c* (*)()> const& a_rFctAllocOutline)
{
    emp::str::utf8_string sFilename(a_sFilename);

    if (sFilename.is_empty())
    {
        EMP_LOG_ERROR("Libttf does not handle empty filenames");
        return false;
    }

    if (sFilename.is_ascii() == false)
    {
        EMP_LOG_ERROR("Libttf does not handle non ascii filenames : #0",
            a_sFilename);
        return false;
    }

    FT_Library pftLibrary;
    FT_Error ftError = FT_Init_FreeType(&pftLibrary);
    if (ftError)
    {
        EMP_LOG_ERROR("FT_Error #0 : #1", ftError, fterrno(ftError));
        return false;
    }

    EMP_ASSERT(pftLibrary);

    FT_Face ftFace = nullptr;

    ftError = FT_New_Face(pftLibrary,
        reinterpret_cast<char const*>(sFilename.c_str()), 0, &ftFace);
    if (ftError == FT_Err_Unknown_File_Format)
    {
        EMP_LOG_ERROR("FT_Error #0, #1 : #2",
            a_sFilename, ftError, fterrno(ftError));
        FT_Done_FreeType(pftLibrary);
        return false;
    }
    else if (ftError)
    {
        EMP_LOG_ERROR("FT_Error #0, #1 : #2",
            a_sFilename, ftError, fterrno(ftError));
        FT_Done_FreeType(pftLibrary);
        return false;
    }

    FT_Outline_Funcs ftOutlineFuncs;
    ftOutlineFuncs.move_to = &move_to;
    ftOutlineFuncs.line_to = &line_to;
    ftOutlineFuncs.conic_to = &conic_to;
    ftOutlineFuncs.cubic_to = &cubic_to;
    ftOutlineFuncs.shift = 0;
    ftOutlineFuncs.delta = 0;

    FT_F26Dot6 const oFontSize =
        static_cast<FT_F26Dot6>(static_cast<float>(a_stSize) * 64.0f);
    EMP_LOG_DEBUG("ttf size : #0", a_stSize);

    FT_UInt const uiFontDpi = 72;

    FT_Set_Char_Size(ftFace, oFontSize, oFontSize, uiFontDpi, uiFontDpi);

    FT_UInt uiGlyphIndex = 0;
    FT_ULong ulCharCode = FT_Get_First_Char(ftFace, &uiGlyphIndex);

    emp::cnt::vector<size_t> vstExtra;
    vstExtra.add(792);
    vstExtra.add(1073);
    vstExtra.add(2139);
    vstExtra.add(2425);
    vstExtra.add(2481);
    vstExtra.add(2527);
    vstExtra.add(2596);
    vstExtra.add(2624);
    vstExtra.add(2670);
    vstExtra.add(2671);
    vstExtra.add(2672);
    vstExtra.add(2694);

    size_t stIndex = 0;
    while (uiGlyphIndex != 0)
    {
        bool bFound = false;
        for (size_t stExtra = 0; stExtra < vstExtra.count(); ++stExtra)
        {
            if (vstExtra.val(stExtra) == stIndex)
            {
                bFound = true;
                break;
            }
        }
        if (stIndex >= a_stBegin &&
            (a_stEnd == 0 || stIndex < a_stEnd || bFound == true))
        {
            ftError = FT_Load_Char(ftFace, ulCharCode, FT_LOAD_DEFAULT);
            if (ftError)
            {
                EMP_LOG_ERROR("FT_Error #0 : #1", ftError, fterrno(ftError));
                FT_Done_Face(ftFace);
                FT_Done_FreeType(pftLibrary);
                return false;
            }

            if (FT_GLYPH_FORMAT_OUTLINE != ftFace->glyph->format)
            {
                EMP_LOG_ERROR("FT not an outline format #0", a_sFilename);
                FT_Done_Face(ftFace);
                FT_Done_FreeType(pftLibrary);
                return false;
            }

            // HC : only one specific char code
            //if (ulCharCode == 231)
            {
                emp::str::utf32_string sCode;
                // TODO : 32/64
                uint32_t ui32CharCode = static_cast<uint32_t>(ulCharCode);
                sCode.append_char(ui32CharCode);
                glyph_c* pGlyph = a_rFctAllocGlyph();
                pGlyph->set_name(sCode);

                a_rFont.glyphs().add(pGlyph);

                FT_Outline ftOutline = ftFace->glyph->outline;
                parser oParser(*pGlyph, &a_rFctAllocOutline);

                ftError = FT_Outline_Decompose(
                    &ftOutline, &ftOutlineFuncs, &oParser);
                if (ftError)
                {
                    EMP_LOG_ERROR("FT_Error #0, #1 : #2",
                        a_sFilename, ftError, fterrno(ftError));
                    FT_Done_Face(ftFace);
                    FT_Done_FreeType(pftLibrary);
                    return false;
                }
            }
        }

        ulCharCode = FT_Get_Next_Char(ftFace, ulCharCode, &uiGlyphIndex);
        ++stIndex;
    }

    FT_Done_Face(ftFace);
    FT_Done_FreeType(pftLibrary);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ttf {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normalize(font_s& a_rFont)
{
    c_size stGlyphCount = a_rFont.glyphs.count();
    for (size_t stGlyph = 0; stGlyph < stGlyphCount; ++stGlyph)
    {
        emp::mat::glyph_s* const pGlyph = a_rFont.glyphs.val(stGlyph);

        c_size stOutlineCount = pGlyph->outlines.count();
        for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
        {
            emp::mat::outline_s* const pOutline = pGlyph->outlines.val(stOutline);

            c_size stPointCount = pOutline->points.count();
            for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
            {
                mat::v2f& rv2fPoint = pOutline->points.ref(stPoint);
                rv2fPoint = mat::v2f::mul(rv2fPoint, emp::mat::v2f::all(1 / (64.0f * 64.0f)));
            }
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

