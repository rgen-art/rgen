//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_docker.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool docker_c::resize(void)
{
    if (state().visible)
    {
        if (state().autosize.x ||
            state().autosize.y)
        { EMP_RET_FALSE_IF_NOT(redock(true)); }
        refresh_size(false, false);
        EMP_RET_FALSE_IF_NOT(redock(false));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool docker_c::redock(c_bool a_bFirst)
{
    c_size stCount = control_count();

    cpc_padding_data_s pPadding = skin ? skin->padding : nullptr;

    c_float fLeft = pPadding ? pPadding->left() : 0;
    c_float fTop = pPadding ? pPadding->top() : 0;

    c_float fRight = pPadding ? pPadding->right() : 0;
    c_float fBottom = pPadding ? pPadding->bottom() : 0;

    c_transformation_data_s& rTransform = transformation();

    c_float fWidth = rTransform.width();
    c_float fHeight = rTransform.height();

    c_v2f v2Size = rTransform.size;

    c_bool bAutoSizeX = state().autosize.x;
    c_bool bAutoSizeY = state().autosize.y;

    float fOffsetLeft = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        auto& rState = pControl->state();
        if (rState.visible == false ||
            rState.dock != dock_e::Left)
        { continue; }

        c_float fNewX = fLeft + fOffsetLeft;
        c_float fNewY = -fTop;
        c_float fNewHeight = a_bFirst ? 0 : v2Size.y - fTop - fBottom;

        transformation_data_s& rCtrlTransform = pControl->transformation().position_(fNewX, fNewY).height_(fNewHeight);

        c_bool bAutoY = rState.autosize.y;
        if (a_bFirst)
        { rState.autosize.y_(bAutoSizeY || bAutoY); }
        EMP_RET_FALSE_IF_NOT(pControl->resize());
        rState.autosize.y_(bAutoY);

        fOffsetLeft += rCtrlTransform.width();
    }

    float fOffsetRight = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        auto& rState = pControl->state();
        if (rState.visible == false ||
            rState.dock != dock_e::Right)
        { continue; }

        auto& rCtrlTransform = pControl->transformation();
        c_float fNewX = fWidth - rCtrlTransform.width() - fRight - fOffsetRight;
        c_float fNewY = -fTop;
        c_float fNewHeight = a_bFirst ? 0 : v2Size.y - fTop - fBottom;
        pControl->transformation().position_(fNewX, fNewY).height_(fNewHeight);

        c_bool bAutoY = rState.autosize.y;
        if (a_bFirst)
        { rState.autosize.y_(bAutoSizeY || bAutoY); }
        EMP_RET_FALSE_IF_NOT(pControl->resize());
        rState.autosize.y_(bAutoY);

        fOffsetRight += rCtrlTransform.width();
    }

    float fOffsetTop = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        auto& rState = pControl->state();
        if (rState.visible == false ||
            rState.dock != dock_e::Top)
        { continue; }

        c_float fNewX = fLeft;
        c_float fNewY = -fTop - fOffsetTop;
        c_float fNewWidth = v2Size.x - fLeft - fRight;

        transformation_data_s& rCtrlTransform = pControl->transformation().position_(fNewX, fNewY);

        c_bool bAutoX = rState.autosize.x;
        if (a_bFirst == false || (bAutoX && bAutoSizeX))
        { rCtrlTransform.width_(fNewWidth); }

        if (a_bFirst)
        { rState.autosize.x_(bAutoSizeX || bAutoX); }
        EMP_RET_FALSE_IF_NOT(pControl->resize());
        rState.autosize.x_(bAutoX);

        if (bAutoSizeX)
        {
            c_float fCenterX = (fWidth - rCtrlTransform.width()) * 0.5f - 1;
            rCtrlTransform.x_(fCenterX);
        }

        fOffsetTop += rCtrlTransform.height();
    }

    float fOffsetBottom = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        auto& rState = pControl->state();
        if (rState.visible == false ||
            rState.dock != dock_e::Bottom)
        { continue; }

        transformation_data_s& rCtrlTransform = pControl->transformation();
        c_float fNewX = fLeft;
        c_float fNewY = -fHeight + rCtrlTransform.height() + fBottom + fOffsetBottom;
        c_float fNewWidth = v2Size.x - fLeft - fRight;

        rCtrlTransform.position_(fNewX, fNewY);
        c_bool bAutoX = rState.autosize.x;
        if (a_bFirst == false || (bAutoX && bAutoSizeX))
        { rCtrlTransform.width_(fNewWidth); }

        if (a_bFirst)
        { rState.autosize.x_(bAutoSizeX || bAutoX); }
        EMP_RET_FALSE_IF_NOT(pControl->resize());
        rState.autosize.x_(bAutoX);

        if (bAutoSizeX)
        {
            c_float fCenterX = (fWidth - rCtrlTransform.width()) * 0.5f - 1;
            rCtrlTransform.x_(fCenterX);
        }

        fOffsetBottom += rCtrlTransform.height();
    }

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        auto& rState = pControl->state();
        if (rState.visible == false ||
            rState.dock != dock_e::Fill)
        { continue; }

        c_float fNewX = fLeft + fOffsetLeft;
        c_float fNewY = -fTop - fOffsetTop;
        c_float fNewWidth = v2Size.x - fNewX - fRight - fOffsetRight;
        c_float fNewHeight = v2Size.y + fNewY - fBottom - fOffsetBottom;

        transformation_data_s& rCtrlTransform = pControl->transformation().position_(fNewX, fNewY);

        c_bool bAutoX = rState.autosize.x;
        c_bool bAutoY = rState.autosize.y;

        c_v2f v2NewSize = v2f::select(
            { (a_bFirst == false || (bAutoX && bAutoSizeX) ? 1.0f : 0.0f),
              (a_bFirst == false || (bAutoY && bAutoSizeY) ? 1.0f : 0.0f) },
            { fNewWidth, fNewHeight}, rCtrlTransform.size);

        rCtrlTransform.size_(v2NewSize);

        if (a_bFirst)
        { rState.autosize.xy(bAutoSizeX || bAutoX, bAutoSizeY || bAutoY); }
        EMP_RET_FALSE_IF_NOT(pControl->resize());
        rState.autosize.xy(bAutoX, bAutoY);
    }

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        pc_control_i pControl = control(stIndex);
        c_dock_e eDock = pControl->state().dock;

        if (pControl->state().visible &&
            eDock != dock_e::Fill &&
            eDock != dock_e::Left &&
            eDock != dock_e::Right &&
            eDock != dock_e::Top &&
            eDock != dock_e::Bottom)
        { EMP_RET_FALSE_IF_NOT(pControl->resize()); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

