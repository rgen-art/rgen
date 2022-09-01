//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_slider.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"

#include "emp_rfx_type_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
slider_c::slider_c(void):
    vertices()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
slider_c::~slider_c(void)
{ vertices->destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool slider_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false ||
        skin == nullptr ||
        skin->value == nullptr)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    c_float fHeight = transformation().height();
    c_float fDepth = emp::gui::depth(transformation().fore_depth, 0.01f);
    c_float fSlide = slide(m_fSlide, delta * (max - min) / transformation().width());
    emp::gui::draw_quad_from_bounds(transformation().matrix, {0, -fHeight}, { fSlide * transformation().width() / max, 0}, fDepth, skin->value->rgba, false);

    /*auto& rDevice = EMP_GRAPHICS;
    rDevice.set_blend(true);
    rDevice.set_depth_test(true);
    rDevice.set_depth_mask(true);
    rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
    rDevice.set_vertex_buffer(&*vertices);
    rDevice.set_index_buffer(rDevice.quad_index_buffer());
    rDevice.set_program(rDevice.color_uniform_program());
    rDevice.set_uniform(uniform_e::m44Transform, transformation().matrix);
    rDevice.set_uniform(uniform_e::v4Color, skin->value->rgba);
    rDevice.draw(emp::gfx::primitive_e::Triangles);*/

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
slider_c& slider_c::skin_(cpc_slider_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->label : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool slider_c::update(emp::tim::clock_c const& a_rClock)
{
    if (state().visible == false)
    { return true; }

    emp::rfx::type_i* const pReflection = reflection();
    if (pReflection != nullptr)
    {
        if (dragged)
        {
            m_su8Text.clear();
            c_float fSlide = slide(m_fSlide, delta * (max - min) / transformation().width());
            emp::rfx::c_type_e eType = pReflection->id()->m_eType;
            if (eType == emp::rfx::type_e::Float32)
            {
                EMP_ASSERT_BOOL_CALL(pReflection->from_string(emp::str::from(m_su8Text, fSlide)));
            }
            else if (eType == emp::rfx::type_e::UInt32)
            {
                ui32 ui32Slide = 0;
                EMP_ASSERT_BOOL_CALL(emp::ast::try_cast(fSlide, ui32Slide));
                EMP_ASSERT_BOOL_CALL(pReflection->from_string(emp::str::from(m_su8Text, ui32Slide)));
            }
        }
        else
        {
            u8string sSlide;
            c_bool bResult = pReflection->to_string(nullptr, sSlide);
            EMP_ASSERT(bResult);
            if (bResult && emp::str::cmp(m_su8Text, sSlide) == false)
            { emp::tt::silent(emp::str::to(sSlide, m_fSlide)); } // @@0 assert sSlide = "bound" wtf ??
        }
    }

    EMP_RET_FALSE_IF_NOT(tbase1::update(a_rClock));

    /*c_float fMax = max - min;
    c_float fSlide = slide(m_fSlide, delta);
    c_float fW = (fSlide - min) * transformation().width() / fMax;
    c_float fH = transformation().height();

    c_float fDepth = -(emp::ast::cast<float>(transformation().back_depth) * 0.01f); // @@0 factor

    vertices->create(emp::gfx::usage_e::Dynamic);
    vertices->add({ 0  , -fH , fDepth});
    vertices->add({ fW , -fH , fDepth});
    vertices->add({ 0  , 0   , fDepth});
    vertices->add({ fW , 0   , fDepth});*/

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e slider_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    if (state().enable == false)
    { return return_e::Unused; }

    return tbase1::process_keyboard(a_rEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e slider_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false ||
        state().enable == false ||
        a_rEvent.keyboard.modified())
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    auto const& rMouse = a_rEvent.mouse;
    //c_v2f v2fPosition = rMouse.m_v2fPosition;
    //c_v2f v2fRelPosition = v2f::add(v2fPosition, v2f::one());

    c_bool bContains = state().hover;
    c_bool bPressed = a_rEvent.mouse_pressed();
    c_bool bReleased = a_rEvent.mouse_released();
    c_bool bDragged = a_rEvent.mouse_dragged();

    emp::app::c_button_e eButton = rMouse.m_eButton;
    if (eButton == emp::app::button_e::Left)
    {
        if (rMouse.m_stTapCount == 1 &&
            rMouse.m_stToucheCount == 1)
        {
            if (bPressed)
            {
                if (bContains)
                {
                    axis = 0;
                    if (dragged == false)
                    {
                        dragged = true;
                        delta = 0;
                        emp::rfx::type_i const* const pReflection = reflection();
                        if (pReflection != nullptr)
                        {
                            u8string sSlide;
                            c_bool bResult = pReflection->to_string(nullptr, sSlide);
                            EMP_ASSERT(bResult);
                            if (bResult && emp::str::to(sSlide, m_fSlide) == false)
                            { m_fSlide = slide(0, 0); }
                        }
                        else
                        { m_fSlide = slide(0, 0); }
                    }

                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (bReleased)
            {
                m_fSlide = slide(m_fSlide, delta * (max - min) / transformation().width());
                delta = 0;
                if (dragged)
                {
                    dragged = false;
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (bDragged)
            {
                if (dragged)
                {
                    accumulate(a_rEvent.mouse.distance());
                    EMP_LOG_INFO("slided in #0, #1, #2", delta, a_rEvent.mouse.distance().x, a_rEvent.mouse.distance().y);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }

        //dragged = false;
        //m_fSlide = 0;
        //delta = 0;
    }
    else if (eButton == emp::app::button_e::WheelUp)
    {
        if (bContains)
        { return EMP_GUI_RETURN(a_rEvent, return_e::Consumed); }
    }
    else if (eButton == emp::app::button_e::WheelDown)
    {
        if (bContains)
        { return EMP_GUI_RETURN(a_rEvent, return_e::Consumed); }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void slider_c::accumulate(v2f::tparam a_v2Value)
{
    if (axis == 0)
    {
        c_v2f v2fDelta = v2f::abs(a_v2Value);
        axis = v2fDelta.x > v2fDelta.y ? 1 : -1;
    }
    delta = axis > 0 ? a_v2Value.x : a_v2Value.y;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float slider_c::slide(c_float a_fSlide, c_float a_fValue)
{ return emp::tt::minof(emp::tt::maxof(min, a_fSlide + a_fValue), max); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

