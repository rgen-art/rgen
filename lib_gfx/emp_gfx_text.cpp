//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gfx_text.h"

#include "emp_gfx_gl_device.h"

#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
font_c::~font_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool font_c::post_load(void)
{
    EMP_RET_FALSE_IF(m_mGlyphs.count() > 0); // @@1 map serialization ?

    c_size stCount = m_vGlyphs.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        glyph_s const& rGlyph = m_vGlyphs.ref(stIndex);
        m_mGlyphs.set(rGlyph.m_ui32Code, stIndex);
    }

    compute_bounds();

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void font_c::destroy(void)
{
    m_vpModels.resize(0);

    m_mGlyphs.clear();

    m_bAspectRatio = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void font_c::add_code(c_ui32 a_ui32Code, emp::gfx::model_c* const a_pModel)
{
    m_vpModels.add(a_pModel);
    m_vGlyphs.add(glyph_s({ a_ui32Code, v2f::zero(), v2f::zero(), v2f::zero() }));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void font_c::compute_bounds(void)
{
    c_float fCharWidth = char_width();
    float fLineOverHeight = 0;
    float fLineUnderHeight = 0;

    c_size stCount = m_vpModels.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gfx::model_c const* const pModel = m_vpModels.val(stIndex);

        c_v4f& v4Min = pModel->min();
        c_float fMinY = v4Min.y;
        if (fMinY < 0)
        { fLineUnderHeight = emp::tt::maxof(fLineUnderHeight, -fMinY); }

        c_v4f& v4Max = pModel->max();
        c_float fMaxY = v4Max.y;
        if (fMaxY > 0)
        { fLineOverHeight = emp::tt::maxof(fLineOverHeight, fMaxY); }
    }

    m_fLineUnderHeight = fLineUnderHeight;
    m_fLineOverHeight = fLineOverHeight;
    m_fLineHeight = m_fLineUnderHeight + m_fLineOverHeight;

    c_float fLineHeight = line_height();

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::gfx::model_c const* const pModel = m_vpModels.val(stIndex);
        glyph_s& rGlyph = m_vGlyphs.ref(stIndex);

        c_v4f& v4Min = pModel->min();
        c_v4f& v4Max = pModel->max();

        c_float fMaxX = v4Max.x;
        c_float fMaxY = v4Max.y;
        c_float fMinX = v4Min.x;
        c_float fMinY = v4Min.y;
        
        c_v4f v4Res = v4f::select(v4f::more(v4f::all(fMaxX), v4f::all(fMinX)),
            { fMinX, fMinY, fMaxX, fMaxY }, { 0, 0, fCharWidth, fLineHeight });

        rGlyph.m_v2Min = v4f::xy(v4Res);
        rGlyph.m_v2Max = v4f::zw(v4Res);
        rGlyph.m_v2Size = v2f::sub(rGlyph.m_v2Max, rGlyph.m_v2Min);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
word_c::~word_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool word_c::create(c_u8string& a_sWord, font_c const& a_rFont)
{
    destroy();

    float fX = 0;
    float fY = 0;

    float EMP_TT_MAX_VAR(fMinY);
    float EMP_TT_MIN_VAR(fMaxY);

    c_float fCharWidth = a_rFont.char_width();
    c_float fSpaceWidth = a_rFont.space_width();
    u8string sWord(a_sWord);
    c_size stLen = sWord.len();
    size_t stCurrentLen = stLen;
    while (stCurrentLen > 0)
    {
        c_ui64 ui64Char = sWord.get_first_char();
        sWord.trim_left();
        stCurrentLen = sWord.len();
        if (emp::str::is_newline(ui64Char))
        {
            EMP_RET_FALSE_IF_NOT(stLen == 1);
            m_eType = word_e::Newline;
        }
        else if (emp::str::is_tabulation(ui64Char))
        {
            EMP_RET_FALSE_IF_NOT(stLen == 1);
            m_eType = word_e::Tabulation;
            fX += fCharWidth * 4;
        }
        else if (emp::str::is_space(ui64Char))
        {
            EMP_RET_FALSE_IF_NOT(stLen == 1);
            m_eType = word_e::Space;
            fX += fSpaceWidth;
        }
        else
        {
            m_eType = word_e::Text;

            c_size stGlyph = a_rFont.get_glyph(u8string::utf8_to_codepoint(ui64Char));
            EMP_RET_FALSE_IF(emp::tt::is_max(stGlyph), "Glyph #0 not found in font : #1", ui64Char, a_rFont.name());

            m_vstGlyphs.add(stGlyph);

            glyph_s const& rGlyph = a_rFont.glyphs().ref(stGlyph);

            // @@0 bug ? .x instead of .y fMinY = emp::tt::minof(fMinY, rGlyph.min().x);
            fMinY = emp::tt::minof(fMinY, rGlyph.m_v2Min.y);
            fMaxY = emp::tt::maxof(fMaxY, rGlyph.m_v2Max.y);

            fX += rGlyph.m_v2Size.x;
            fY = emp::tt::maxof(fY, fMaxY - fMinY);
        }
    }

    if (type() == word_e::Text && stLen > 0)
    {
        c_float fCharSpace = a_rFont.char_space();
        fX += fCharSpace * emp::ast::cast<float>(stLen - 1);
    }

    m_v2Offset = { 0, fMinY };
    m_v2Size = { fX, fY };
    EMP_RET_FALSE_IF_NOT(fX > 0 || m_eType !=  word_e::Text);
    EMP_RET_FALSE_IF_NOT(type() != word_e::none);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void word_c::destroy(void)
{
    m_eType = word_e::none;
    m_v2Offset = v2f::zero();
    m_v2Size = v2f::zero();
    m_vstGlyphs.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN word_e word_c::type(void) const
{
    EMP_ASSERT(m_eType == word_e::Text || m_vstGlyphs.count() == 0);
    return m_eType;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
text_c::~text_c(void)
{
    EMP_ASSERT_BOOL_CALL(foreach(m_vpWords, emp::mem::safe_destroy_delete_reset_t()));
    m_vpWords.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INTERN EMP_RETURN bool is_separator(T const a_tChar);
template <typename T> EMP_INTERN EMP_RETURN bool is_separator(T const a_tChar)
{
    return
        emp::str::is_space(a_tChar) ||
        emp::str::is_newline(a_tChar) ||
        emp::str::is_tabulation(a_tChar);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool text_c::create(void)
{
    destroy();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::destroy(void)
{

    m_stWords = 0;
    m_sText = u8string();
    m_fFontSizeX = 1.0f;
    m_fFontSizeY = 1.0f;
    m_v2Size = v2f::zero();
    m_v2AutoSize = v2f::zero();
    m_v2Bounds = v2f::zero();
    m_vpWords.resize(0);
    m_vstLines.resize(0);
    m_bNeedRefresh = false;
    m_vv2Positions.resize(0);
    m_vstInside.resize(0);
    m_vstOutside.resize(0);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_text(c_u8string& a_sValue)
{
    if (emp::str::cmp(m_sText, a_sValue) == false)
    {
        m_sText.clear();
        u8string sTmp(a_sValue);
        while (sTmp.len())
        {
            m_sText.append_char(sTmp.get_first_char());
            sTmp.trim_left();
        }
        m_bNeedRefresh = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_font(font_c const* const a_pValue)
{
    if (m_pFont != a_pValue)
    {
        m_pFont = a_pValue;
        m_bNeedRefresh = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_font_size_x(float const a_fValue)
{
    if (emp::tt::equal_(m_fFontSizeX, a_fValue) == false)
    {
        m_fFontSizeX = a_fValue;
        m_bNeedRefresh = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_font_size_y(float const a_fValue)
{
    if (emp::tt::equal_(m_fFontSizeY, a_fValue) == false)
    {
        m_fFontSizeY = a_fValue;
        m_bNeedRefresh = true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN word_c* get_word(vector<word_c*>& a_rvpWords, c_size a_stIndex);
EMP_RETURN word_c* get_word(vector<word_c*>& a_rvpWords, c_size a_stIndex)
{
    if (a_stIndex < a_rvpWords.count())
    { return a_rvpWords.val(a_stIndex); }
    word_c* pWord = EMP_ALLOCATE(word_c);
    a_rvpWords.add(pWord);
    return pWord;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::refresh_text(font_c const* const a_pFont)
{
    u8string sText(m_sText);
    while (sText.len())
    {
        while ( sText.len() > 0 &&
               is_separator(sText.get_first_char()))
        {
            c_size stLine = m_stWords;
            word_c* const pWord = get_word(m_vpWords, m_stWords);
            ++m_stWords;

            u8string sWord(sText);
            sWord.trim_count(0, 1);
            EMP_RET_FALSE_IF_NOT(pWord->create(sWord, *a_pFont));
            if (pWord->type() == word_e::Newline)
            { m_vstLines.add(stLine); }
            sText.trim_left();
        }

        size_t stCharCount = 0;
        u8string sWord(sText);
        while (sText.len() > 0 &&
               is_separator(sText.get_first_char()) == false)
        {
            ++stCharCount;
            sText.trim_left();
        }

        if (stCharCount)
        {
            word_c* const pWord = get_word(m_vpWords, m_stWords);
            ++m_stWords;

            sWord.trim_count(0, stCharCount);
            EMP_RET_FALSE_IF_NOT(pWord->create(sWord, *a_pFont));
            EMP_RET_FALSE_IF_NOT(pWord->size().x > 0);
        }
    }

    m_vstLines.add(m_stWords);
    m_fLineHeight = emp::ast::cast<float>(m_vstLines.count()) * m_fFontSizeY * a_pFont->line_height();
    m_v2Size = { get_width(), get_height() };
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float text_c::get_height(void)
{
    m_fLineHeight = emp::ast::cast<float>(m_vstLines.count()) * m_fFontSizeY * m_pFont->line_height();
    return m_fLineHeight;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float text_c::get_unscaled_width(void)
{
    size_t stLineIndex = 0;
    m_vfLines.resize(0);

    float fX = 0;
    float fSpaceCount = 0;
    auto const l_add_word = [this](c_float a_fX, c_float a_fSpaceCount)
    { m_vfLines.add((a_fX + emp::tt::maxof<float>(a_fSpaceCount - 1, 0) * m_pFont->char_space())); };

    for (size_t stIndex = 0; stIndex < m_stWords; ++stIndex)
    {
        c_size stLine = stLineIndex < m_vstLines.count() ? m_vstLines.val(stLineIndex) : emp::tt::max<size_t>::value;
        if (stLine == stIndex)
        {
            l_add_word(fX, fSpaceCount);
            fX = 0;
            fSpaceCount = 0;
            ++stLineIndex;
        }

        word_c const* const pWord = words().val(stIndex);
        if (pWord->type() != word_e::Newline)
        {
            fX += pWord->size().x;
            ++fSpaceCount;
        }
    }
    l_add_word(fX, fSpaceCount);

    float fMaxX = 0;
    c_size stCount = m_vfLines.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { fMaxX = emp::tt::maxof(fMaxX, m_vfLines.val(stIndex)); }
    return fMaxX;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float text_c::get_width(void)
{ return get_unscaled_width() * m_fFontSizeX; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::remove_line(void)
{
    float const fBoundWidth =
    bounds().x;
    EMP_ASSERT(fBoundWidth > 0);

    c_size stCount = m_vstLines.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stWord = m_vstLines.val(stIndex);
        if (stWord < m_stWords)
        {
            word_c const* const pWord = m_vpWords.val(stWord);
            EMP_ASSERT(pWord != nullptr);
            if (pWord->type() != word_e::Newline)
            {
                m_vstLines.remove(stIndex);
                float const fX = get_width();
                if (fX < fBoundWidth)
                { return true; }
                m_vstLines.insert(stIndex, stWord);
            }
        }

    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::add_line(void)
{
    c_float fBoundWidth = bounds().x;
    EMP_ASSERT(fBoundWidth > 0);

    c_float fFontSizeX = m_fFontSizeX;

    float fX = 0;

    size_t stWordCount = 0;
    size_t stWordIndex = 0;
    size_t stLineIndex = 0;

    c_size stCount = m_stWords;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stLine = stLineIndex < m_vstLines.count() ? m_vstLines.val(stLineIndex) : emp::tt::max<size_t>::value;

        c_size stInsert = stLineIndex;
        if (stLine == stIndex)
        {
            fX = 0;
            stWordCount = 0;
            ++stLineIndex;
        }

        word_c const* const pWord = words().val(stIndex);
        EMP_ASSERT(pWord != nullptr);
        if (pWord->type() == word_e::Text)
        {
            stWordIndex = stIndex;
            ++stWordCount;
        }
        fX += pWord->size().x * fFontSizeX;
        //@@0 : HC fpmath sse 387 precision issue
        if (fX > (fBoundWidth + 0.0001f) && stWordCount > 1)
        {
            //EMP_LOG_INFO("FX #0, FW #1", fX, fBoundWidth);
            if (m_vstLines.count() > 0)
            { m_vstLines.insert(stInsert, stWordIndex); }
            else
            { m_vstLines.add(stIndex); }
            //EMP_LOG_INFO("ADD_LINE");
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::refresh_size(void)
{
    c_float fBoundWidth = bounds().x;
    c_float fBoundHeight = bounds().y;

    c_bool bFitWidth = fBoundWidth > 0;
    c_bool bFitHeight = fBoundHeight > 0;

    float fCurrentWidth = 0;
    if (bFitWidth)
    { fCurrentWidth = get_width(); }

    float fCurrentHeight = 0;
    if (bFitHeight)
    { fCurrentHeight = get_height(); }

    float fLastWidth = fCurrentWidth;
    float fLastHeight = fCurrentHeight;

    while (true)
    {
        if (bFitWidth)
        {
            while (fCurrentWidth > fBoundWidth && add_line())
            { fCurrentWidth = get_width(); }
            while (fCurrentWidth < fBoundWidth && remove_line())
            { fCurrentWidth = get_width(); }
        }

        if (bFitHeight)
        {
            //   while (fCurrentHeight > fBoundHeight && remove_line())
            // { fCurrentHeight = get_height(); }
            // while (fCurrentHeight < fBoundHeight && add_line())
            //{ fCurrentHeight = get_height(); }
        }

        bool bBreakWidth = true;
        if (bFitWidth)
        {
            fCurrentWidth = get_width();
            bBreakWidth = emp::tt::equal_(fLastWidth, fCurrentWidth);
            //EMP_LOG_INFO("WIDTH #0, #1, #2", fLastWidth, fCurrentWidth,
            //bBreakWidth ? "true" : "false");
            fLastWidth = fCurrentWidth;
        }

        bool bBreakHeight = true;
        if (bFitHeight)
        {
            fCurrentHeight = get_height();
            bBreakHeight = emp::tt::equal_(fLastHeight, fCurrentHeight);
            //EMP_LOG_INFO("HEIGHt #0, #1", fLastHeight, fCurrentHeight);
            fLastHeight = fCurrentHeight;
        }

        if (bBreakWidth && bBreakHeight)
        { break; }
    }

    c_float fWidth = get_width();
    c_float fHeight = get_height();

    /*float fWidth = fCurrentWidth; @@0 ??
     //if (bFitWidth)
     { fWidth = get_width(); }

     float fHeight = fCurrentHeight; @@0 ??
     //if (bFitHeight)
     { fHeight = get_height(); }*/

    m_v2AutoSize = { fWidth, fHeight };
    //EMP_LOG_INFO("SIZE : #0, #1", fWidth, fHeight);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::refresh(void)
{
    //if (m_bNeedRefresh)
    {
        m_stWords = 0;

        m_v2Size = v2f::zero();
        m_vstLines.resize(0);
        EMP_RET_FALSE_IF_NOT(refresh_text(m_pFont));

        m_v2AutoSize = v2f::zero();

        refresh_size();
        EMP_RET_FALSE_IF_NOT(m_v2AutoSize.y > 0 || m_sText.len() == 0);

        m_vv2Positions.resize(0);
        m_vstInside.resize(0);
        m_vstOutside.resize(0);

        font_c const* const pFont = m_pFont;

        c_float fFontSizeX = 1.0f;//m_fFontSizeX;
        c_float fFontSizeY = 1.0f;//m_fFontSizeY;

        c_v2f v2FontSize = { fFontSizeX, fFontSizeY };

        c_size stLineCount = m_vstLines.count();
        if ((stLineCount > 0) == false)
        { return true; }

        c_float fLineHeight = pFont->line_height();

        c_float fWidth = get_unscaled_width();

        auto const l_get_alignment = [this, fWidth](c_size a_stLineIndex) EMP_RETURN ->  float
        {
            if (m_si32Alignment < 0)
            { return 0; }
            else if (m_si32Alignment > 0)
            { return fWidth - m_vfLines.val(a_stLineIndex); }
            else
            { return (fWidth - m_vfLines.val(a_stLineIndex)) * 0.5f; }
        };

        size_t stLineIndex = 0;
        float fX = l_get_alignment(stLineIndex);
        float fY = -emp::ast::cast<float>(stLineCount - 1) * fLineHeight;

        float fDeltaX = 0;
        float fDeltaY = 0;

        c_size stCount = m_stWords;

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stLine = stLineIndex < stLineCount ? m_vstLines.val(stLineIndex) : emp::tt::max<size_t>::value;
            if (stLine == stIndex)
            {
                fDeltaY = fLineHeight;
                ++stLineIndex;
                fX = l_get_alignment(stLineIndex);
            }

            word_c const* const pWord = words().val(stIndex);
            c_word_e eType = pWord->type();
            if (eType == word_e::Newline)
            { }
            else if (eType == word_e::Tabulation ||
                     eType == word_e::Space ||
                     eType == word_e::Text)
            { fDeltaX = pWord->size().x; }
            else
            { EMP_ASSERT_UNREACHABLE(); }

            fY += fDeltaY;
            fDeltaY = 0;

            float fGlyphX = fX;
            EMP_ASSERT_NUMBER(fGlyphX);

            float fGlyphY = -fY;

            if (eType == word_e::Text)
            {
                c_float fCharSpace = pFont->char_space();

                size_t stGlyphCount = pWord->glyphs().count();
                for (size_t stGlyph = 0; stGlyph < stGlyphCount; ++stGlyph)
                {
                    c_size stGlyphIndex = pWord->glyphs().val(stGlyph);

                    glyph_s const& rGlyph = pFont->glyphs().ref(stGlyphIndex);
                    c_float fGlyphSizeX = rGlyph.m_v2Size.x;
                    EMP_ASSERT_NUMBER(fGlyphSizeX);

                    emp::gfx::model_c const* const pModel = pFont->models().ref(stGlyphIndex);
                    c_size stMeshCount = pModel->meshes().count();
                    EMP_ASSERT(stMeshCount > 0 && stMeshCount <= 2);

                    m_vv2Positions.add(v2f::mul(v2FontSize, { fGlyphX, fGlyphY }));

                    if (stMeshCount > 0 && stMeshCount <= 2)
                    {
                        for (size_t stMesh = 0; stMesh < stMeshCount; ++stMesh)
                        {
                            emp::gfx::mesh_c const* const pMesh = pModel->meshes().val(stMesh);
                            if (pMesh->is_not_empty())
                            {
                                if (stMesh == 0)
                                { m_vstInside.add(stGlyphIndex); }
                                else
                                { m_vstOutside.add(stGlyphIndex); }
                            }
                            else
                            {
                                if (stMesh == 0)
                                { m_vstInside.add_max(); }
                                else
                                { m_vstOutside.add_max(); }
                            }
                        }
                    }
                    else
                    {
                        m_vstInside.add_max();
                        m_vstOutside.add_max();
                    }

                    fGlyphX += fGlyphSizeX + fCharSpace;
                    EMP_ASSERT_NUMBER(fGlyphX);
                }
            }
            else
            {
                m_vv2Positions.add(v2f::mul(v2FontSize, { fGlyphX, fGlyphY }));

                m_vstInside.add_max();
                m_vstOutside.add_max();
            }

            fX += fDeltaX;
            fDeltaX = 0;
        }

        m_vv2Positions.add(v2f::mul(v2FontSize, { fX, -fY }));

        m_vstInside.add_max();
        m_vstOutside.add_max();

        m_bNeedRefresh = false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
//@@3 old
void text_c::transform(void)
{
    if (emp::str::cmp(m_sTransformText, m_sText) &&
        v2f::bool_equal(m_v2TransformSize, m_v2Size))
    { return; }

    m_sTransformText = m_sText;
    m_v2TransformSize = m_v2Size;

    font_c const* const pFont = m_pFont;

    c_size EMP_TT_MAX_VAR(stMax);
    
    EMP_ASSERT(m_vstInside.count() == m_vstOutside.count());

    m_vInsideVertices.create();
    m_vInsideVertices.create();

    m_vOutsideVertices.create();
    m_vOutsideIndices.create();

    c_size stCount = m_vv2Positions.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_v2f v2Position = m_vv2Positions.val(stIndex);

        c_float fX = v2f::get_x(v2Position);
        c_float fY = v2f::get_y(v2Position);

        c_size stInsideGlyph = m_vstInside.val(stIndex);
        if (stInsideGlyph != stMax)
        {
            emp::gfx::model_c const* const pModel = pFont->models().ref(stInsideGlyph);
            emp::gfx::mesh_c const* const pMesh = pModel->meshes().val(0);

            size_t stInsideIndex = m_vInsideIndices.count();

            c_size stVertexCount = m_vInsideVertices.count();
            c_size stStride = pMesh->stride();
            for (size_t stVertex = 0; stVertex < stVertexCount; stVertex += stStride)
            {
                m_vfInside.add(vfVertices.val(stVertex + 0) + fX);
                m_vfInside.add(vfVertices.val(stVertex + 1) + fY);
                m_vfInside.add(vfVertices.val(stVertex + 2));
                m_vfInside.add(vfVertices.val(stVertex + 3));
                m_vfInside.add(vfVertices.val(stVertex + 4));
                m_vfInside.add(vfVertices.val(stVertex + 5));
                m_vfInside.add(vfVertices.val(stVertex + 6));
                m_vfInside.add(vfVertices.val(stVertex + 7));
                m_vuiInside.add(emp::ast::cast<uint>(stInsideIndex++));
            }
        }

        c_size stOutsideGlyph = m_vstOutside.val(stIndex);
        if (stOutsideGlyph != stMax)
        {
            emp::gfx::model_c const* const pModel = pFont->models().ref(stOutsideGlyph);
            emp::gfx::mesh_c const* const pMesh = pModel->meshes().val(1);

            size_t stOutsideIndex = m_vuiOutside.count();

            vector<float> const& vfVertices = pMesh->vertices();
            c_size stVertexCount = vfVertices.count();
            c_size stStride = pMesh->stride();
            for (size_t stVertex = 0; stVertex < stVertexCount; stVertex += stStride)
            {
                m_vfOutside.add(vfVertices.val(stVertex + 0) + fX);
                m_vfOutside.add(vfVertices.val(stVertex + 1) + fY);
                m_vfOutside.add(vfVertices.val(stVertex + 2));
                m_vfOutside.add(vfVertices.val(stVertex + 3));
                m_vfOutside.add(vfVertices.val(stVertex + 4));
                m_vfOutside.add(vfVertices.val(stVertex + 5));
                m_vfOutside.add(vfVertices.val(stVertex + 6));
                m_vfOutside.add(vfVertices.val(stVertex + 7));
                m_vuiOutside.add(emp::ast::cast<uint>(stOutsideIndex++));
            }
        }
    }
}
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::draw(v4f::tparam a_v4Color, m44f::tparam a_mTransform, c_si32 a_stDepth) const
{
    emp::gfx::gl_device_c& rDevice = EMP_GRAPHICS;

    c_size stCount = m_vv2Positions.count();

    m44f m44Transform;

    c_float fDepth = -(emp::ast::cast<float>(a_stDepth) * 0.01f); // @@0 factor


    auto const& lRenderGlyph = [this, fDepth, &rDevice, stCount, &a_v4Color, &a_mTransform, &m44Transform](c_size a_stProgram, c_size a_stMesh, c_bool a_bBlend)
    {
        rDevice.set_blend(a_bBlend);
        rDevice.glBlendFunc_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rDevice.set_program(a_stProgram);
        rDevice.set_uniform(emp::gfx::uniform_e::v4Color, a_v4Color);

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_v3f v3Position = v3f::all(m_vv2Positions.ref(stIndex), fDepth);

            m44f::identity(m44Transform);
            m44f::translate(m44Transform, v3Position, m44Transform);
            m44f::mul(a_mTransform, m44Transform, m44Transform);

            c_size stGlyph = m_vstInside.val(stIndex);
            if (emp::tt::is_max(stGlyph))
            { continue; }

            emp::gfx::model_c const* const pModel = m_pFont->models().ref(stGlyph);
            emp::gfx::mesh_c const* const pMesh = pModel->meshes().val(a_stMesh);

            rDevice.set_vertex_buffer(pMesh->vertices());
            rDevice.set_index_buffer(pMesh->indices()); // @@1 global index buffer 8, 16, 32
            rDevice.set_uniform(emp::gfx::uniform_e::m44Transform, m44Transform);
            rDevice.draw(emp::gfx::primitive_e::Triangles);
        }
    };

    rDevice.set_depth_test(true); // @@0 depth done
    rDevice.set_depth_mask(true);

    lRenderGlyph(rDevice.color_uniform_program(), 0, false);
    lRenderGlyph(rDevice.path_program(), 1, true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

