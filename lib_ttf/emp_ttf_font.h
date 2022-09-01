//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TTF_FONT_H
#define EMP_TTF_FONT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fct { template <typename> class function_t; } }

namespace emp { namespace mat { struct glyph_s; } }
namespace emp { namespace mat { struct outline_s; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ttf {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(struct, font_s)
public:
    astring name;
    vector<emp::mat::glyph_s*> glyphs;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void normalize(font_s& a_rFont);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read(c_astring& a_sFilename, font_s& a_rFont, c_size a_stSize, c_size a_stBegin, c_size a_stEnd,
    emp::fct::function_t<emp::mat::glyph_s* (*)()> const& a_rFctAllocGlyph,
    emp::fct::function_t<emp::mat::outline_s* (*)()> const& a_rFctAllocOutline);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

