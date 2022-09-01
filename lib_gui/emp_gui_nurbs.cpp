//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_nurbs.h"

#include "emp_gui_coordinate.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_nurbs.h"

#include "emp_mat_nurbs.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::fct;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nurbs_c::nurbs_c(void):
    gfx_nurbs()
{
    on_render = function(this, &nurbs_c::render_nurbs);
    on_point_moved = function(this, &nurbs_c::move_point);
    on_point_added = function(this, &nurbs_c::add_point);
    on_point_removed = function(this, &nurbs_c::remove_point);

    gfx_nurbs->create();
    gfx_nurbs->set_iteration(100);

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nurbs_c::~nurbs_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool nurbs_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    points.resize(0);

    c_size stCount = nurbs->points().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_v2f v2Point = v4f::xy(nurbs->points().ref(stIndex));
        c_v2f v2Clamped = v2f::max(frame.data.min, v2f::min(frame.data.max, v2Point));

        points.add(v2Clamped);
    }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool nurbs_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    c_float fDepth = depth(transformation().fore_depth, 0);

    c_size stCount = nurbs->points().count();
    if (stCount !=  gfx_nurbs->point_count())
    {
        gfx_nurbs->clear_points();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { gfx_nurbs->add_point(v3f::all(frame.frame_to_client(v4f::xy(nurbs->points().ref(stIndex))), fDepth)); }
    }
    else
    {
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { gfx_nurbs->set_point(stIndex, v3f::all(frame.frame_to_client(v4f::xy(nurbs->points().ref(stIndex))), fDepth)); }
    }

    {
        c_v4f v4Color = (skin && skin->line) ? skin->line->rgba : v4f::all(0);
        gfx_nurbs->set_in_color(v4Color);
        gfx_nurbs->set_out_color(v4Color);
    }

    gfx_nurbs->update();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::render_nurbs(plot_c const& a_rSender)
{ gfx_nurbs->render(a_rSender.frame.transformation().matrix); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t nurbs_c::move_point(c_size a_stIndex, v2f::tparam a_v2Delta)
{ return nurbs->move_point(a_stIndex, v4f::all(a_v2Delta, 0, 1)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::add_point(c_size EMP_SILENT(a_stIndex), v2f::tparam a_v2Position)
{ nurbs->insert_point(v4f::all(a_v2Position, 0, 1)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::remove_point(c_size a_stIndex)
{ nurbs->remove_point(a_stIndex); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nurbs_c& nurbs_c::skin_(cpc_nurbs_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->plot : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

