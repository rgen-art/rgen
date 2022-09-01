//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_text.h"

#include "emp_gui_label.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"
#include "emp_gfx_text.h"

#include "emp_rfx_type_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <cmath>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
text_c::text_c(void):
vertices{}
{
    on_edit.reset(this, false);
    on_valid.reset(this, false);
    on_cancel.reset(this, false);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
text_c::~text_c(void)
{ vertices->destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::update(emp::tim::clock_c const& a_rClock)
{
    EMP_RET_FALSE_IF_NOT(tbase1::update(a_rClock));

    float const fDelta = emp::tim::time_c::to_seconds(a_rClock.delta());
    m_fCarret += fDelta * 6.0f;
    m_fCarret = emp::mat::mod(m_fCarret, emp::mat::pi<float>());

    // @@0 replace mat::sin
    color = v4f::mul(skin->carret->rgba, v4f::all(emp::mat::abs(sinf(m_fCarret))));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    if (on_edit.value)
    {
        EMP_ASSERT(m_stCarret < positions().count());

        c_v2f v2Position = positions().val(m_stCarret);

        cpc_label_skin_s pLabel = skin->label;
        cpc_font_data_s pFontData = pLabel->font;
        emp::gfx::font_c const* const pFont = pFontData->pointer;

        if (pFont != nullptr)
        {
            c_float fX = v2Position.x;

            c_float fDepth = depth(transformation().fore_depth, 0);

            //resolution_data_s const& rResolution = *pLabel->resolution();

           // float const fFontSize = pFontData->size() * rResolution.scale();
            //float const fFontSizeX = fFontSize * rResolution.ratio_x();
           // float const fFontSizeY = fFontSize * rResolution.ratio_y();

            //float const fY = emp::mat::v2f::get_y(v2Position);/* -
            //m_Label.font().pointer()->line_under_height() *
            //fFontSize;*/

            //emp::mat::v2f::tret v2FontSize=m_Label.compute_font_size();
            c_float fY = pFont->line_under_height();
            c_float fH = pFont->line_height();

            vertices->create(emp::gfx::usage_e::Dynamic);
            vertices->add({ fX, -fY     , fDepth });
            vertices->add({ fX, -fY + fH, fDepth });
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool text_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false)
    { return true; }

    //if (m_OnEdit.value())
    {
        EMP_RET_FALSE_IF_NOT(tbase1::render());

        auto& rDevice = EMP_GRAPHICS;
        rDevice.set_depth_test(false); // @@0 depth
        rDevice.set_depth_mask(false);
        rDevice.set_vertex_buffer(&*vertices);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_program(rDevice.color_uniform_program());
        rDevice.set_uniform(uniform_e::m44Transform, text_matrix());
        rDevice.set_uniform(uniform_e::v4Color, color);
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::move_left(c_size a_stCount)
{ m_stCarret = (m_stCarret - emp::tt::minof(m_stCarret, a_stCount)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::move_right(c_size a_stCount)
{
    c_size stCarret = m_stCarret + a_stCount;
    c_size stEditionCount = m_su8Edition.len();
    c_size stMax = stEditionCount > 0 ? stEditionCount : 0;
    m_stCarret = emp::tt::minof(stCarret, stMax);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::move_up(c_size a_stCount)
{ move_left(a_stCount); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::move_down(c_size a_stCount)
{ move_right(a_stCount); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_text(c_u8string& a_su8Value)
{
    u8string su8Current(a_su8Value);

    if (reflection != nullptr)
    {
        if (reflection->from_string(a_su8Value) == false)
        { emp::tt::silent(reflection->from_string(m_su8Text)); }
        su8Current.clear();
        EMP_ASSERT_BOOL_CALL(reflection->to_string(formater, su8Current));
    }
    m_su8Edition = su8Current;
    m_su8Text = su8Current;
    tbase1::set_text(su8Current);/// @@0 xcode detect infinite recursion
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_reflection(emp::rfx::type_i* const a_pValue)
{
    reflection = a_pValue;
    u8string su8Current;
    EMP_ASSERT_BOOL_CALL(reflection->to_string(formater, su8Current));
    set_text(su8Current);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_on_edit(c_bool a_bValue, c_change_e a_eChange)
{
    emp::tt::silent(on_edit.set_state(a_bValue, a_eChange));
    if (on_edit.value)
    { m_stCarret = m_su8Edition.len(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_on_valid(c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue)
    { set_text(m_su8Edition); }

    emp::tt::silent(on_valid.set_state(a_bValue, a_eChange));
    if (on_valid.value)
    { emp::tt::silent(on_valid.set_state(false, a_eChange)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void text_c::set_on_cancel(c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue)
    { set_text(m_su8Text); }

    emp::tt::silent(on_cancel.set_state(a_bValue, a_eChange));
    if (on_cancel.value)
    { emp::tt::silent(on_cancel.set_state(false, a_eChange)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e text_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false ||
        state().enable == false)
    { return return_e::Unused; }

    auto const& rKeyboard = a_rEvent.keyboard;
    if (rKeyboard.m_bPressed)
    {
        if (emp::gui::focus_in_hierarchy(this))
        {
            c_change_e eChange = change_e::Keyboard;

            emp::app::c_key_e eCode = rKeyboard.m_eKey;
            if (eCode == emp::app::key_e::Enter)
            {
                if (on_edit.value)
                {
                    set_on_valid(true, eChange);
                    set_on_edit(false, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                else
                {
                    set_on_edit(true, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (eCode == emp::app::key_e::Escape)
            {
                if (on_edit.value)
                {
                    set_on_cancel(true, eChange);
                    set_on_edit(false, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (eCode == emp::app::key_e::Left)
            {
                if (on_edit.value)
                {
                    move_left(1);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (eCode == emp::app::key_e::Right)
            {
                if (on_edit.value)
                {
                    move_right(1);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (eCode == emp::app::key_e::Up)
            {
                if (on_edit.value)
                {
                    move_up(1);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (eCode == emp::app::key_e::Down)
            {
                if (on_edit.value)
                {
                    move_down(1);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
            else if (eCode == emp::app::key_e::Backspace)
            {
                if (on_edit.value == false)
                { set_on_edit(true, eChange); }

                if (m_stCarret > 0)
                {
                    m_su8Edition.slow_remove(m_stCarret - 1);
                    set_text(m_su8Edition);
                    move_left(1);
                }
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
            else if (eCode == emp::app::key_e::Delete)
            {
                if (on_edit.value == false)
                { set_on_edit(true, eChange); }

                if (m_su8Edition.len() > 1) // @@0 string used to be count
                {
                    m_su8Edition.slow_remove(m_stCarret);
                    set_text(m_su8Edition);
                }
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
            else if (eCode == emp::app::key_e::none ||
                     eCode == emp::app::key_e::Space ||
                     eCode == emp::app::key_e::Num_0 ||
                     eCode == emp::app::key_e::Num_1 ||
                     eCode == emp::app::key_e::Num_2 ||
                     eCode == emp::app::key_e::Num_3 ||
                     eCode == emp::app::key_e::Num_4 ||
                     eCode == emp::app::key_e::Num_5 ||
                     eCode == emp::app::key_e::Num_6 ||
                     eCode == emp::app::key_e::Num_7 ||
                     eCode == emp::app::key_e::Num_8 ||
                     eCode == emp::app::key_e::Num_9)
            {
                if (on_edit.value == false)
                { set_on_edit(true, eChange); }

                c_ui64 cChar = rKeyboard.m_ui64Char;

                emp::gui::cpc_font_data_s pFontData = skin->label->font;
                emp::gfx::font_c const* const pFont = pFontData->pointer;

                if (pFont != nullptr)
                {
                    c_size stGlyph = pFont->get_glyph(emp::ast::cast<ui32>(cChar));
                    if (emp::tt::is_not_max(stGlyph))
                    {
                        m_su8Edition.slow_insert(m_stCarret, cChar);
                        set_text(m_su8Edition);
                        move_right(1);
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
            }
        }
    }

    return tbase1::process_keyboard(a_rEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
text_c& text_c::skin_(cpc_text_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->label : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

