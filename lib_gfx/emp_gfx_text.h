//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GFX_TEXT_H
#define EMP_GFX_TEXT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_map.h"

#include "emp_mat_matrix.h"
#include "emp_mat_vector.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::mat;
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class mesh_c;
class model_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct glyph_s
{
public:
    ui32 EMP_TT_MAX_VAR(m_ui32Code);
    v2f m_v2Min;
    v2f m_v2Max;
    v2f m_v2Size;

    glyph_s(void) :
        EMP_TT_MAX_CTOR(m_ui32Code),
        m_v2Min(),
        m_v2Max(),
        m_v2Size()
    {}

    glyph_s(c_ui32 a_ui32Code, c_v2f& a_v2Min, c_v2f& a_v2Max, c_v2f& a_v2Size):
        m_ui32Code(a_ui32Code), m_v2Min(a_v2Min), m_v2Max(a_v2Max), m_v2Size(a_v2Size)
    {}
};

static_assert(std::is_standard_layout<glyph_s>::value, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, font_c)
private:
    map_t<ui32, size_t> m_mGlyphs{256};
    vector<glyph_s> m_vGlyphs{};
    vector<model_c*> m_vpModels{};

    float m_fLineHeight = 0;
    float m_fLineUnderHeight = 0;
    float m_fLineOverHeight = 0;
    float m_fLineSpace = 0.05f;
    float m_fCharWidth = 0.2f;
    float m_fCharSpace = 0.07f;
    float m_fSpaceWidth = 0.3f;
    bool m_bAspectRatio = false;
    astring m_sName{};

public:
    font_c(void) = default;
    ~font_c(void);

    EMP_RETURN bool post_load(void);
    void destroy(void);

    EMP_RETURN size_t get_glyph(c_ui32 a_ui32Code) const
    {
        auto const oIt = m_mGlyphs.get(a_ui32Code);
        if (oIt != m_mGlyphs.end())
        { return *oIt.value(); }
        return emp::tt::max<size_t>::value;
    }

    EMP_RETURN v3f::tret ratio(void) const;

    void add_code(c_ui32 a_ui32Code, emp::gfx::model_c* const a_pModel);

    void compute_bounds(void);

    EMP_RETURN c_astring& name(void) const { return m_sName; }
public:
    EMP_RETURN vector<glyph_s> const& glyphs(void) const { return m_vGlyphs; }
    EMP_RETURN vector<emp::gfx::model_c*> const& models(void) const { return m_vpModels; }
    //EMP_RETURN emp::cnt::vector<emp::gfx::gl_model_c*> const& gl_models(void) const { return m_vpGlModels; }

    EMP_RETURN float line_height(void) const { return m_fLineHeight; }
    EMP_RETURN float line_under_height(void) const { return m_fLineUnderHeight; }
    EMP_RETURN float line_over_height(void) const { return m_fLineOverHeight; }
    EMP_RETURN float line_space(void) const { return m_fLineSpace; }

    EMP_RETURN float char_width(void) const { return m_fCharWidth; }
    void set_char_width(c_float a_fValue) { m_fCharWidth = a_fValue; }

    EMP_RETURN float char_space(void) const { return m_fCharSpace; }
    void set_char_space(c_float a_fValue) { m_fCharSpace = a_fValue; }
    
    EMP_RETURN float space_width(void) const { return m_fSpaceWidth; }
    EMP_RETURN bool aspect_ratio(void) const { return m_bAspectRatio; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name word_e
#define enum_values\
    enum_value( none       )\
    enum_value( Text       )\
    enum_value( Newline    )\
    enum_value( Space      )\
    enum_value( Tabulation )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, word_c)
private:
    word_e m_eType = word_e::none;
    v2f m_v2Offset{};
    v2f m_v2Size{};
    vector<size_t> m_vstGlyphs{};

public:
    word_c(void) = default;
    ~word_c(void);

public:
    EMP_RETURN bool create(c_u8string& a_sWord, font_c const& a_rFont);
    void destroy(void);

public:
    EMP_RETURN word_e type(void) const;
    EMP_RETURN v2f::tret offset(void) const { return m_v2Offset; }
    EMP_RETURN v2f::type size(void) const { return m_v2Size; }
    EMP_RETURN vector<size_t> const& glyphs(void) const { return m_vstGlyphs; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, text_c)
private:
    u8string m_sText{};
    font_c const* m_pFont = nullptr;
    float m_fFontSizeX = 1.0f;
    float m_fFontSizeY = 1.0f;
    v2f m_v2Size{};
    v2f m_v2AutoSize{};
    v2f m_v2Bounds{};
    vector<word_c*> m_vpWords{};
    vector<size_t> m_vstLines{};
    bool m_bNeedRefresh = false;
    float m_fLineHeight = 0.0f;
    vector<v2f::type> m_vv2Positions{};
    vector<size_t> m_vstInside{};
    vector<size_t> m_vstOutside{};
    vector<float> m_vfLines{};
    size_t m_stWords = 0;
    si32 m_si32Alignment = 0;

public:
    text_c(void) = default;
    ~text_c(void);

public:
    EMP_RETURN bool refresh(void);
    void draw(v4f::tparam a_v4Color, m44f::tparam a_mModel, c_si32 a_stDepth) const;

public:
    void set_text(c_u8string& a_sText);

    void set_font(font_c const* const a_pValue);
    EMP_RETURN font_c const* font(void) const { return m_pFont; }

    void set_font_size_x(c_float a_fValue);
    EMP_RETURN float font_size_x(void) const { return m_fFontSizeX; }

    void set_font_size_y(c_float a_fValue);
    EMP_RETURN float font_size_y(void) const { return m_fFontSizeY; }

    EMP_RETURN v2f::tret size(void) const { return m_v2Size; }
    EMP_RETURN v2f::tret autosize(void) const { return m_v2AutoSize; }

    void set_bounds(v2f::tparam a_v2Value)
    {
        if (bool_or(v2f::not_equal(m_v2Bounds, a_v2Value)))
        {
            m_v2Bounds = a_v2Value;
            m_bNeedRefresh = true;
        }
    }

    void set_alignment(c_si32 a_si32Value)
    {
        if (a_si32Value != m_si32Alignment)
        {
            m_si32Alignment = a_si32Value;
            m_bNeedRefresh = true;
        }
    }

    EMP_RETURN v2f::tret bounds(void) const { return m_v2Bounds; }

    EMP_RETURN vector<word_c*> const& words(void) const { return m_vpWords; }

    //EMP_RETURN vector<size_t> const& lines(void) const { return m_vstLines; }

    EMP_RETURN vector<v2f::type> const& positions(void) const { return m_vv2Positions; }

public:
    EMP_RETURN bool can_render(void) const { return m_pFont != nullptr && m_bNeedRefresh == false; }

private:
    EMP_RETURN float get_unscaled_width(void);
    
    EMP_RETURN float get_width(void);
    EMP_RETURN float get_height(void);

    EMP_RETURN bool remove_line(void);
    EMP_RETURN bool add_line(void);

    EMP_RETURN bool refresh_text(font_c const* const a_pFont);
    void refresh_size(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

