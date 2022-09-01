//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_dialog.h"

#include "emp_gui_manager.h"
#include "emp_gui_toolbar.h"
#include "emp_gui_splitter.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::fct;
using namespace emp::loc;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dialog_c::dialog_c(void)
{
    set_control(&window);

    toolbar.autosize_label_x_(true).type_(dock_e::Right).state().index_(0).autosize.y_(true);

    splitter.slave_(&toolbar).orientation_(dock_e::Top).state().index_(0).dock_(dock_e::Top);
    splitter.redock();

    window.create();
    window.top_(true).modal_(true).add_control(&splitter);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dialog_c::~dialog_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool dialog_c::resize(void)
{
    if (state().visible)
    {
        if (splitter.master != nullptr)
        { state().autosize = splitter.master->state().autosize; }
        splitter.state().autosize = state().autosize;
        EMP_RET_FALSE_IF_NOT(tbase2::resize());
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e dialog_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible && state().enable)
    {
        EMP_GUI_RETURN_IF_USED(tbase2::process_keyboard(a_rEvent));

        auto const& rKeyboard = a_rEvent.keyboard;
        if (rKeyboard.m_bPressed &&
            rKeyboard.m_bRepeat == false)
        {
            emp::app::c_key_e eCode = rKeyboard.m_eKey;
            if (eCode == emp::app::key_e::Escape)
            {
                if (on_cancel.is_valid())
                {
                    if (on_cancel(true, change_e::Keyboard) == return_e::Error)
                    { return EMP_GUI_RETURN(a_rEvent, return_e::Error); }
                    if (on_cancel(false, change_e::Keyboard) == return_e::Error)
                    { return EMP_GUI_RETURN(a_rEvent, return_e::Error); }
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dialog_c& dialog_c::type_(c_dialog_e a_eValue)
{
    if (m_eType == a_eValue)
    { return *this; }

    m_eType = a_eValue;

    on_push.resize(0);
    toolbar.skin_(skin ? skin->toolbar : nullptr).type_(dock_e::Right).state().navigable_(true).index_(0).autosize.y_(true);

    size_t stFocusIndex = 0;
    if (m_eType == dialog_e::Ok ||
        m_eType == dialog_e::OkCancel)
    {
        on_push.add(&on_ok);
        toolbar.add_item(string_s(ALITERAL("emp_gui_dialog_ok")),
            function(this, &dialog_c::button_pushed), nullptr, stFocusIndex);
        ++stFocusIndex;
    }

    if (m_eType == dialog_e::YesNo ||
        m_eType == dialog_e::YesNoCancel)
    {
        on_push.add(&on_yes);
        toolbar.add_item(string_s(ALITERAL("emp_gui_dialog_yes")),
            function(this, &dialog_c::button_pushed), nullptr, stFocusIndex);
        ++stFocusIndex;

        on_push.add(&on_no);
        toolbar.add_item(string_s(ALITERAL("emp_gui_dialog_no")),
            function(this, &dialog_c::button_pushed), nullptr, stFocusIndex);
        ++stFocusIndex;
    }

    if (m_eType == dialog_e::OkCancel ||
        m_eType == dialog_e::YesNoCancel)
    {
        on_push.add(&on_cancel);
        toolbar.add_item(string_s(ALITERAL("emp_gui_dialog_cancel")),
            function(this, &dialog_c::button_pushed), nullptr, stFocusIndex);
        ++stFocusIndex;
    }

    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void dialog_c::center(void)
{ transformation().position_(v2f::mul(v2f::sub({2, 2}, transformation().size), {0.5f, -0.5f})); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dialog_c& dialog_c::skin_(dialog_skin_s const* const a_pValue)
{
    skin = a_pValue;

    window.skin_(a_pValue ? a_pValue->window : nullptr);
    splitter.skin_(a_pValue ? a_pValue->splitter : nullptr);
    toolbar.skin_(a_pValue ? a_pValue->toolbar : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dialog_c& dialog_c::user_control_(pc_control_i a_pControl)
{ splitter.master_(a_pControl).redock(); return *this; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e dialog_c::button_pushed(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange)
{ return (*on_push.val(a_stIndex))(a_bValue, a_eChange); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

