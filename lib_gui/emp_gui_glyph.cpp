//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_glyph.h"

#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_icon.h"
#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::fct;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
glyph_c::glyph_c(void)
{
    frame.on_render = function(this, &glyph_c::render_glyph);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
glyph_c::~glyph_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool glyph_c::transform(stack_c& a_rStack)
{
    if (state().visible)
    {
        EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));
        EMP_RET_FALSE_IF_NOT(gfx_icon->create());
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void glyph_c::auto_resize(void)
{
    c_v2f v2Scale = v2f::make(m_Coordinate.scaled_unit_size_x(), m_Coordinate.scaled_unit_size_y());
    c_v2f v2Size = v3f::xy(m_pGlyph->model()->size());

    cpc_frame_skin_s pFrameSkin = m_pSkin->coordinate()->frame();
    EMP_ASSERT(pFrameSkin);

    c_v2f v2Unit = v2f::make(pFrameSkin->unit_x(), pFrameSkin->unit_y());

    m_Coordinate.auto_resize(v2f::add(v2f::div(v2f::mul(v2Size, v2Scale), v2Unit), v2Scale));
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool glyph_c::auto_resize(void)
{
    c_v2f v2Size = v4f::xy(gfx_icon->model()->size());
    c_v2f v2Scale = { frame.data.final_scale_x(), frame.data.final_scale_y() };

    EMP_RET_FALSE_IF_NOT(tbase1::auto_resize(v2f::add(v2f::mul(v2Size, v2Scale), v2Scale)));

    c_v2f v2ModelCenter = v4f::xy(gfx_icon->model()->center());

    frame.center_origin();
    frame.set_origin(v2f::sub(frame.origin(), v2f::mul(v2ModelCenter, v2Scale)));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void glyph_c::render_glyph(frame_c const& EMP_SILENT(a_rSender))
{
    //@@0 device m_pGlyph->model()->render(m_pSkin->glyph()->rgba(), a_rSender.model());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
glyph_c& glyph_c::skin_(cpc_glyph_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->coordinate : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

