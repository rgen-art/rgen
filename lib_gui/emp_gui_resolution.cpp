//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_resolution.h"

#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
resolution_c::resolution_c(void):
    on_resolution_changed()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
resolution_c::~resolution_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool resolution_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_v2f v2Size = EMP_GUI_SKIN.resolution.size();
    c_float fScale = EMP_GUI_SKIN.resolution.scale();

    if (emp::mat::not_equal_(size.x, v2Size.x) ||
        emp::mat::not_equal_(size.y, v2Size.y) ||
        emp::mat::not_equal_(m_fScale, fScale))
    {
        c_v2f v2OldSize = transformation().size;

        EMP_RET_FALSE_IF_NOT(control->resize());

        c_float fNewScale = scale ? scale_ratio() : 1.0f;

        c_v2f v2Ratio = v2f::mul(v2f::all(fNewScale), size_ratio());

        transformation().size_(v2f::mul(v2OldSize, v2Ratio));

        if (center == false)
        { transformation().position_(v2f::mul(transformation().position, v2Ratio)); }

        if (on_resolution_changed.is_valid())
        { on_resolution_changed(this); }

        size = v2Size;
        m_fScale = fScale;
        ratio = EMP_GUI_SKIN.resolution.ratio();

        EMP_RET_FALSE_IF_NOT(control->resize());
    }

    EMP_RET_FALSE_IF_NOT(control->resize());

    if (center)
    { transformation().position_(v2f::mul({0.5f, -0.5f}, v2f::sub({2, 2}, transformation().size))); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::v2f::tret resolution_c::size_ratio(void) const
{ return v2f::div(EMP_GUI_SKIN.resolution.ratio(), ratio); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float resolution_c::scale_ratio(void) const
{ return EMP_GUI_SKIN.resolution.scale() / m_fScale; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

