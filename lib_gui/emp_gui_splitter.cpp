//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_splitter.h"

#include "emp_gui_spacer.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
splitter_c::splitter_c(void):
    spacer()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool splitter_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_bool bAutosizeX = state().autosize.x;
    c_bool bAutosizeY = state().autosize.y;

    c_bool bMasterAutosizeX = master->state().autosize.x;
    c_bool bMasterAutosizeY = master->state().autosize.y;

    c_bool bSlaveAutosizeX = slave->state().autosize.x;
    c_bool bSlaveAutosizeY = slave->state().autosize.y;

    if (orientation == dock_e::Left)
    {
        if (bAutosizeY)
        {
            master->transformation().height_(0);
            slave->transformation().height_(0);
            master->state().autosize.y_(true);
            slave->state().autosize.y_(true);
        }
    }
    else if (orientation == dock_e::Right)
    {
        if (bAutosizeY)
        {
            master->transformation().height_(0);
            slave->transformation().height_(0);
            master->state().autosize.y_(true);
            slave->state().autosize.y_(true);
        }
    }
    else if (orientation == dock_e::Top)
    {
        if (bAutosizeX)
        {
            // @@0  regression ?
            master->transformation().width_(0);
            slave->transformation().width_(0);
            master->state().autosize.x_(true);
            slave->state().autosize.x_(true);
        }
    }
    else if (orientation == dock_e::Bottom)
    {
        if (bAutosizeX)
        {
            master->transformation().width_(0);
            slave->transformation().width_(0);
            master->state().autosize.x_(true);
            slave->state().autosize.x_(true);
        }
    }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    transformation_data_s& rMaster = master->transformation();
    c_bool bMasterVisible = master->state().visible && (hide_master == false);
    c_float fMasterWidth = bMasterVisible ? rMaster.width() : 0;
    c_float fMasterHeight = bMasterVisible ? rMaster.height() : 0;

    transformation_data_s& rSlave = slave->transformation();
    c_bool bSlaveVisible = slave->state().visible;
    c_float fSlaveWidth = bSlaveVisible ? rSlave.width() : 0;
    c_float fSlaveHeight = bSlaveVisible ? rSlave.height() : 0;

    c_float fMaxY = emp::tt::maxof((hide_master == false ? fMasterHeight : 0), fSlaveHeight);
    c_float fMaxX = emp::tt::maxof((hide_master == false ? fMasterWidth : 0), fSlaveWidth);

    c_float fSX = EMP_GUI_SKIN.spacing_x();//skin && skin->spacer && skin->spacer->spacing ? skin->spacer->spacing->x() : 0;
    c_float fSY = EMP_GUI_SKIN.spacing_y();//skin && skin->spacer && skin->spacer->spacing ? skin->spacer->spacing->y() : 0;

    c_float fWidth = (hide_master == false ? fMasterWidth + fSX : 0) + fSlaveWidth;
    c_float fHeight = (hide_master == false ? fMasterHeight + fSY : 0) + fSlaveHeight;

    transformation_data_s& rSpacer = spacer.transformation();

    //float const fHalfMasterH = (fMaxY - fMasterHeight) * 0.5f;
    //float const fHalfMasterW = (fMaxX - fMasterWidth) * 0.5f;
    //float const fHalfSlaveH = (fMaxY - fSlaveHeight) * 0.5f;
    //float const fHalfSlaveW = (fMaxX - fSlaveWidth) * 0.5f;

    c_float fCurrentWidth = transformation().width();
    c_float fCurrentHeight = transformation().height();

    if (orientation == dock_e::Left)
    {
        c_float fNewHeight = bAutosizeY ? fMaxY : fCurrentHeight;

        float fX = 0;
        if (hide_master == false)
        {
            rMaster.position_(0).height_(fNewHeight);
            master->state().autosize.xy(bMasterAutosizeX, bMasterAutosizeY);
            EMP_RET_FALSE_IF_NOT(master->resize());

            fX = rMaster.width();

            rSpacer.position_(fX, 0).size_(fSX, fNewHeight);
            fX += fSX;
        }

        rSlave.position_(fX, 0).size_(bAutosizeX ? fSlaveWidth : fCurrentWidth - fX, fNewHeight);

        transformation().size_(bAutosizeX ? fWidth : fCurrentWidth, fNewHeight);
    }
    else if (orientation == dock_e::Right)
    {
        c_float fNewHeight = bAutosizeY ? fMaxY : fCurrentHeight;
        c_float fNewWidth = bAutosizeX ? fWidth : fCurrentWidth;

        float fX = fNewWidth;

        if (hide_master == false)
        {
            rMaster.height_(fNewHeight);
            master->state().autosize.xy(bMasterAutosizeX, bMasterAutosizeY);
            EMP_RET_FALSE_IF_NOT(master->resize());

            fX -= rMaster.width();
            rMaster.position_(fX, 0);

            fX -= fSX;
            rSpacer.position_(fX, 0).size_(fSX, fNewHeight);
        }

        rSlave.position_(0).size_(fX, fNewHeight);

        transformation().size_(fNewWidth, fNewHeight);
        master->state().autosize.xy(bMasterAutosizeX, bMasterAutosizeY);
        EMP_RET_FALSE_IF_NOT(master->resize());
    }
    else if (orientation == dock_e::Top)
    {
        c_float fNewWidth = bAutosizeX ? fMaxX : fCurrentWidth;

        float fY = 0;

        if (hide_master == false)
        {
            rMaster.position_(0).width_(fNewWidth);
            fY += fMasterHeight;

            rSpacer.position_(0, -fY).size_(fNewWidth, fSY);
            fY += fSY;
        }

        rSlave.position_(0, -fY).size_(fNewWidth, bAutosizeY ? fSlaveHeight : fCurrentHeight - fY);

        transformation().size_(fNewWidth, bAutosizeY ? fHeight : fCurrentHeight);
        master->state().autosize.xy(bMasterAutosizeX, bMasterAutosizeY);
        EMP_RET_FALSE_IF_NOT(master->resize());
    }
    else if (orientation == dock_e::Bottom)
    {
        c_float fNewWidth = bAutosizeX ? fMaxX : fCurrentWidth;
        c_float fNewHeight = bAutosizeY ? fHeight : fCurrentHeight;

        float fY = fNewHeight;

        if (hide_master == false)
        {
            fY -= fMasterHeight;
            rMaster.position_(0, -fY).width_(fNewWidth);

            fY -= fSY;
            rSpacer.position_(0, -fY).size_(fNewWidth, fSY);
        }

        rSlave.position_(0).size_(fNewWidth, fY);

        transformation().size_(fNewWidth, fNewHeight);
        master->state().autosize.xy(bMasterAutosizeX, bMasterAutosizeY);
        EMP_RET_FALSE_IF_NOT(master->resize());
    }
    else
    {
        EMP_ASSERT_UNREACHABLE();
        return false;
    }


    EMP_RET_FALSE_IF_NOT(spacer.resize());

    slave->state().autosize.xy(bSlaveAutosizeX, bSlaveAutosizeY);
    EMP_RET_FALSE_IF_NOT(slave->resize());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void splitter_c::redock(void)
{
    clear_controls();
    if (hide_master == false)
    {
        if (master != nullptr)
        { add_control(master); }
        add_control(&spacer);
    }
    if (slave != nullptr)
    { add_control(slave); }

    spacer.state().dock_(orientation);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
splitter_c& splitter_c::skin_(cpc_splitter_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    spacer.skin_(a_pValue ? a_pValue->spacer : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

