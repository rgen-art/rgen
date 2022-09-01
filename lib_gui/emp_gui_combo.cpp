//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_combo.h"

#include "emp_gui_list.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
combo_c::combo_c(void)
{
    on_edit.reset(this, false);
    on_valid.reset(this, false);
    on_cancel.reset(this, false);

    expand_last_column_(true);
    on_selection_changed_(emp::fct::function(this, &combo_c::on_selection_changed));
    on_item_click_(emp::fct::function(this, &combo_c::on_item_click));
    on_selection_double_click_(emp::fct::function(this, &combo_c::on_selection_double_click));
    add_column_(emp::loc::string_s(ALITERAL("values"), U8LITERAL("values")));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
combo_c::~combo_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* combo_c::got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
{
    if (reflection != nullptr)
    {
        ui32 EMP_TT_MAX_VAR(ui32Selection);
        if (a_bSelect)
        { ui32Selection = emp::ast::cast<ui32>(reflection->enum_current_index()); }
        else if (a_eFocus == focus_e::First)
        { ui32Selection = 0; }
        else if (a_eFocus == focus_e::Last)
        { ui32Selection = emp::ast::cast<ui32>(reflection->enum_value_count()); }

        if (emp::tt::is_not_max(ui32Selection))
        {
            set_selection(ui32Selection, 0, false, a_eChange);
            return lines().val(ui32Selection)->got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange);
        }
    }
    return tbase1::got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e combo_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false &&
        state().enable == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    emp::app::c_keyboard_event_s& rKeyboard = a_rEvent.keyboard;

    if (rKeyboard.modified() == false &&
        rKeyboard.m_bRepeat == false &&
        rKeyboard.m_bPressed)
    {
        if (rKeyboard.m_eKey == emp::app::key_e::Escape)
        {
            if (on_edit.value)
            { set_on_cancel(true, change_e::Keyboard); }
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void combo_c::set_on_edit(c_bool a_bValue, c_change_e a_eChange)
{
    emp::tt::silent(on_edit.set_state(a_bValue, a_eChange));
    if (on_edit.value)
    {}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void combo_c::set_on_valid(c_bool a_bValue, c_change_e a_eChange)
{
    emp::tt::silent(on_valid.set_state(a_bValue, a_eChange));
    if (on_valid.value)
    {
        if (reflection != nullptr)
        {
            if (selection < reflection->enum_value_count())
            { reflection->enum_from_index(selection); }
        }

        emp::tt::silent(on_valid.set_state(false, a_eChange));
        EMP_TT_MAX_VAR(selection);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void combo_c::set_on_cancel(c_bool a_bValue,c_change_e a_eChange)
{
    emp::tt::silent(on_cancel.set_state(a_bValue, a_eChange));
    if (on_cancel.value)
    {
        emp::tt::silent(on_cancel.set_state(false, a_eChange));
        EMP_TT_MAX_VAR(selection);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void combo_c::set_reflection(emp::rfx::enum_i* const a_pValue)
{
    reflection = a_pValue;
    clear();
    if (reflection != nullptr)
    {
        c_size stCount = reflection->enum_value_count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { add_line(reflection->enum_to_string(stIndex)); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
combo_c& combo_c::skin_(cpc_list_skin_c a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool combo_c::on_selection_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e EMP_SILENT(a_eChange))
{
    if (emp::tt::is_not_max(a_ui32Line) &&
        a_ui32Line > 0 &&
        a_ui32Item == 0)
    { selection = a_ui32Line - 1; }
    else
    { EMP_TT_MAX_VAR(selection); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool combo_c::on_selection_double_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange)
{
    EMP_RET_FALSE_IF_NOT(on_selection_changed(a_ui32Line, a_ui32Item, a_eChange));
    if (on_edit.value)
    { set_on_valid(true, a_eChange); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e combo_c::on_item_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange)
{
    if (on_edit.value)
    {
        set_selection(a_ui32Line, a_ui32Item, true, a_eChange);
        set_on_valid(true, a_eChange);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

