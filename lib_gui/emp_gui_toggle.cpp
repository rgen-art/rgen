//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_toggle.h"

#include "emp_gui_icon.h"
#include "emp_gui_manager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
toggle_c::toggle_c(void)
{
    on_check.reset(this, false);
    on_push.function_(emp::fct::function(this, &toggle_c::on_button_pushed));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
toggle_c::~toggle_c(void)
{
    if (text != nullptr)
    {
        EMP_DEALLOCATE(text);
        text = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool toggle_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    c_bool bValue = get_value();
    if (on_check.value != bValue)
    { set_check(bValue, change_e::Code); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e toggle_c::on_button_pushed(cpc_control_i EMP_SILENT(a_pSender), c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue == false)
    {
        set_check(on_check.value == false, a_eChange);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Consumed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void toggle_c::set_check(c_bool a_bValue, c_change_e a_eChange)
{
    if (reflection != nullptr)
    {
        emp::rfx::type_t<bool>* const pClass = EMP_FACTORY.cast<bool>(reflection);
        emp::rfx::link_t<bool>& rLink = pClass->link();
        if (rLink.binder().is_valid())
        { rLink.value() = a_bValue; }
        else if (rLink.setter().is_valid())
        { emp::tt::silent(rLink.setter()(a_bValue)); }
    }

    refresh_icon(a_bValue);

    emp::tt::silent(on_check.set_state(a_bValue, a_eChange));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void toggle_c::set_text(emp::loc::string_s const& a_rValue)
{
    if (text == nullptr)
    { text = EMP_ALLOCATE(emp::loc::string_s); }
    *text = a_rValue;
    label.set_text(*text);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void toggle_c::refresh_icon(c_bool a_bValue)
{
    if (text != nullptr)
    { label.set_text(*text); }
    else
    { label.set_text(emp::loc::string_s(a_bValue ? ALITERAL("emp_gui_true") : ALITERAL("emp_gui_false"))); }
    picture_(skin ? (a_bValue ? skin->gfx_on : skin->gfx_off) : nullptr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool toggle_c::get_value(void) const
{
    if (reflection != nullptr)
    {
        emp::rfx::type_t<bool> const* const pClass = EMP_FACTORY.cast<bool>(reflection);
        emp::rfx::link_t<bool> const& rLink = pClass->link();
        if (rLink.binder().is_valid())
        { return rLink.value(); }
        else if (rLink.getter().is_valid())
        {
            bool bValue = false;
            EMP_ASSERT_BOOL_CALL(rLink.getter()(bValue));
            return bValue;
        }
    }

    return on_check.value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
toggle_c& toggle_c::skin_(cpc_toggle_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->icon : nullptr);
    refresh_icon(false);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

