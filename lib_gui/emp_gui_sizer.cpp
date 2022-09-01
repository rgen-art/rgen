//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_sizer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool sizer_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    refresh_size(false, false);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void sizer_c::refresh_size(c_bool EMP_SILENT(a_bX), c_bool EMP_SILENT(a_bY))
{
    v2f EMP_TT_MAX_VAR(v2Min);
    v2f EMP_TT_MIN_VAR(v2Max);

    size_t stVisibleCount = 0;
    c_size stCount = control_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        EMP_ASSERT(pControl != nullptr);
        if (pControl->state().visible == false)
        { continue; }

        c_transformation_data_s& rTransform = pControl->transformation();

        c_v2f& v2Position = rTransform.position;
        c_v2f& v2Size = rTransform.size;

        v2Min = v2f::min(v2Min, {v2Position.x           , -v2Position.y            });
        v2Max = v2f::max(v2Max, {v2Position.x + v2Size.x, -v2Position.y + v2Size.y });
        ++stVisibleCount;
    }

    /*float const fMinX = emp::mat::v2f::get_x(v2Min);
    float const fMinY = emp::mat::v2f::get_y(v2Min);
    v2Min = emp::mat::v2f::make(
        a_bX ? emp::tt::minof(0.0f, fMinX) : fMinX,
        a_bY ? emp::tt::minof(0.0f, fMinY) : fMinY);*/

    /*float const fMaxX = transformation().width();
    float const fMaxY = transformation().height();
    v2Max = emp::mat::v2f::make(
        a_bX ? emp::tt::maxof(fWidth, fMaxX) : fMaxX,
        a_bY ? emp::tt::maxof(fHeight, fMaxY) : fMaxY);*/

    if (stVisibleCount > 0)
    {
        c_bool bAutoSizeX = state().autosize.x;
        c_bool bAutoSizeY = state().autosize.y;

        cpc_padding_data_s pPadding = skin ? skin->padding : nullptr;

        c_v2f v2Size = v2f::sub(v2Max, v2Min);
        c_v2f v2Padding = pPadding ? pPadding->xy() : v2f({0, 0 });
        c_v2f v2NewSize = v2f::select({ bAutoSizeX ? 1.0f : 0.0f, bAutoSizeY ? 1.0f : 0.0f },
            v2f::add(v2Padding, v2Size), transformation().size);

        transformation().size_(v2NewSize);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
sizer_c& sizer_c::skin_(cpc_sizer_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

