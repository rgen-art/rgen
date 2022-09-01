//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_list.h"

#include "emp_gui_combo.h"
#include "emp_gui_label.h"
#include "emp_gui_manager.h"
#include "emp_gui_panel.h"
#include "emp_gui_picture.h"
#include "emp_gui_popup.h"
#include "emp_gui_registrar.h"
#include "emp_gui_skin.h"
#include "emp_gui_slider.h"
#include "emp_gui_spacer.h"
#include "emp_gui_text.h"
#include "emp_gui_toggle.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_astring& get_name(emp::gui::c_skin_data_s& a_rValue)
{ return a_rValue.name; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
line_c::line_c(void):
    items()
{ child_(true); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
line_c::~line_c(void)
{
    emp::cnt::foreach(items, emp::mem::safe_delete_reset_t());
    items.resize(0);

    if (m_pCollapser != nullptr)
    {
        EMP_DEALLOCATE(m_pCollapser);
        m_pCollapser = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t line_c::reflection_count(void) const
{ return reflection != nullptr ? reflection->field_count() : 0; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
line_c& line_c::collapsible_(c_bool a_bValue)
{
    if (a_bValue == false)
    {
        if (m_pCollapser != nullptr)
        {
            remove_control(m_pCollapser);
            EMP_DEALLOCATE(m_pCollapser);
            m_pCollapser = nullptr;
        }
    }
    else if (m_pCollapser == nullptr)
    {
        m_pCollapser = EMP_ALLOCATE(picture_c);
        m_pCollapser->alignment_(alignment_e::MiddleCenter);
        m_pCollapser->on_push.function_(emp::fct::function(this, &line_c::on_collapse));
        m_pCollapser->state().pushable_(true);
        expanded_(false);
        add_control(m_pCollapser);
    }

    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
return_e line_c::on_collapse(cpc_control_i, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    { expanded_(m_bExpanded == false); }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
line_c& line_c::expanded_(c_bool a_bValue)
{
    m_bExpanded = a_bValue;
    if (m_pCollapser != nullptr)
    { m_pCollapser->skin_(skin != nullptr ? skin->collapser : nullptr).model_(
        skin != nullptr ? (m_bExpanded ? skin->gfx_expanded : skin->gfx_collapsed) : nullptr); }
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void line_c::set_selection(c_size a_stItem, c_bool a_bFocus, c_change_e a_eChange)
{
    if (a_bFocus == false)
    { return; }

    if (emp::tt::is_not_max(a_stItem))
    {
        item_i* pItem = items.val(a_stItem);

        pc_control_i pControl = pItem->control().got_focus(focus_e::First, false, false, a_eChange);
        if (pControl != nullptr)
        {
            return EMP_GUI_MANAGER->activate_focus(
                pItem->control().got_focus(focus_e::First, true, true, a_eChange), focus_e::First, true, true, a_eChange);
        }
    }

    if (focus_in_hierarchy(this) == false)
    { EMP_GUI_MANAGER->activate_focus(this, focus_e::First, true, true, a_eChange); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void line_c::add_item(item_i* const a_pItem)
{
    EMP_ASSERT(a_pItem != nullptr);
    items.add(a_pItem);
    add_control(&a_pItem->control());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
line_c& line_c::skin_(cpc_line_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    if (m_pCollapser != nullptr)
    {
        if (a_pValue)
        { m_pCollapser->skin_(a_pValue->collapser).model_(m_bExpanded ? a_pValue->gfx_expanded : a_pValue->gfx_collapsed); }
        else
        { m_pCollapser->skin_(nullptr); }
    }
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove EMP_RETURN control_i& line_c::control(c_size a_stIndex)
{
    pc_control_i pControl = m_Container.control(a_stIndex);
    EMP_ASSERT(pControl);
    return *pControl;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove EMP_RETURN c_control_i& line_c::control(c_size a_stIndex) const
{
    cpc_control_i pControl = m_Container.control(a_stIndex);
    EMP_ASSERT(pControl);
    return *pControl;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void class_c::copy(class_c const& a_rFrom)
{
    destroy();
    over = a_rFrom.over;
    field = a_rFrom.field;
    if (a_rFrom.field.inner != nullptr)
    {
        field.inner = EMP_ALLOCATE(class_c, a_rFrom.field.inner->fields.seed());
        field.inner->copy(*a_rFrom.field.inner);
    }
    for (auto it = a_rFrom.fields.begin(); it.not_end(); ++it)
    {
        auto* const pField = allocate(*it.key());
        auto* const pFrom = *it.value();
        *pField = *pFrom;
        if (pFrom->inner != nullptr)
        {
            pField->inner = EMP_ALLOCATE(class_c, pFrom->inner->fields.seed());
            pField->inner->copy(*pFrom->inner);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void class_c::destroy(void)
{
    for (auto it = fields.begin(); it.not_end(); ++it)
    {
        auto* pField = *it.value();
        if (pField->inner != nullptr)
        { EMP_DEALLOCATE(pField->inner); }
        EMP_DEALLOCATE(pField);
    }
    fields.clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN field_s* class_c::allocate(c_astring& a_sName)
{
    field_s* const pField = EMP_ALLOCATE(field_s);
    fields.set(a_sName, pField);
    return pField;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void designer_c::copy(designer_c const& a_rFrom)
{
    destroy();
    for (auto it = a_rFrom.classes.begin(); it.not_end(); ++it)
    { allocate(*it.key(), a_rFrom.classes.bucket_count(), false)->copy(**it.value()); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void designer_c::destroy(void)
{
    for (auto it = classes.begin(); it.not_end(); ++it)
    { EMP_DEALLOCATE(*it.value()); }
    classes.clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN class_c* designer_c::allocate(c_astring& a_sName, c_size a_stSeed, c_bool a_bOverride)
{
    class_c* const pClass = EMP_ALLOCATE(class_c, a_stSeed);
    pClass->over = a_bOverride;
    classes.set(a_sName, pClass);
    return pClass;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
list_c::list_c(void):
    on_selection_changed(),
    on_selection_double_click(),
    on_item_click(),
    on_item_double_click(),
    m_vpLines(),
    navigables(),
    columns(),
    column(),
    spacer(),
    panel(),
    on_reflect(),
    on_edit(),
    m_pHeaderMeshVertices()
{
    column.state().index_(0);

    navigables.add(&column);

    spacer.state().dock_(dock_e::Top);

    panel.state().index_(1);

    EMP_TT_MAX_VAR(m_ui32EditLine);
    EMP_TT_MAX_VAR(m_ui32EditItem);

    add_control(&column, this);
    add_control(&spacer, this);
    add_control(&panel, this);

    on_reflect = emp::fct::function(this, &list_c::reflect);
    on_edit = emp::fct::function(this, &list_c::on_edit_push);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
list_c::~list_c(void)
{
    emp::cnt::foreach(m_vpLines, emp::mem::safe_delete_reset_t());
    m_vpLines.resize(0);

    // @@0  move in registrar?
    if (window_text != nullptr)
    {
        emp::tt::silent(EMP_GUI_MANAGER->try_remove_window(window_text));
        registrar->remove_window(window_text);
        EMP_DEALLOCATE(window_text);
        // @@0 create text->destroy();
        EMP_DEALLOCATE(text);
        text = nullptr;
        window_text = nullptr;
    }
    if (window_combo != nullptr)
    {
        emp::tt::silent(EMP_GUI_MANAGER->try_remove_window(window_combo));
        registrar->remove_window(window_combo);
        EMP_DEALLOCATE(window_combo);
        // @@0 create combo->destroy();
        EMP_DEALLOCATE(combo);
        combo = nullptr;
        window_combo = nullptr;
    }

    m_pHeaderMeshVertices->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN window_c* new_window(registrar_i* const a_pRegistrar, cpc_list_skin_c a_pSkin);
EMP_RETURN window_c* new_window(registrar_i* const a_pRegistrar, cpc_list_skin_c a_pSkin)
{
    EMP_ASSERT(a_pRegistrar != nullptr);

    window_c* const pWindow = EMP_ALLOCATE(window_c);
    pWindow->create();
    pWindow->skin_(a_pSkin != nullptr ? a_pSkin->window : nullptr);
    pWindow->state().index_(0).autosize.xy(true);
    pWindow->top_(true);
    a_pRegistrar->register_window(pWindow);
    return pWindow;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN window_c* list_c::get_or_alloc_text_window(void)
{
    if (window_text != nullptr)
    { return window_text; }

    text = EMP_ALLOCATE(text_c);

    // @@0 create text->create();
    text->state().index_(0).focusable_(true);
    text->skin_(skin != nullptr ? skin->text : nullptr);
    text->on_cancel.function_(emp::fct::function(this, &list_c::on_text_cancel));
    text->on_valid.function_(emp::fct::function(this, &list_c::on_text_valid));
    text->on_edit.function_(emp::fct::function(this, &list_c::on_text_edit));

    window_text = new_window(registrar, skin);
    window_text->add_control(text);

    return window_text;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
window_c* list_c::get_or_alloc_combo_window(void)
{
    if (window_combo != nullptr)
    { return window_combo; }

    combo = EMP_ALLOCATE(combo_c);

    combo->skin_(skin != nullptr ? skin->combo : nullptr).state().index_(0);
    combo->on_cancel.function_(emp::fct::function(this, &list_c::on_combo_cancel));
    combo->on_valid.function_(emp::fct::function(this, &list_c::on_combo_valid));
    combo->on_edit.function_(emp::fct::function(this, &list_c::on_combo_edit));

    window_combo = new_window(registrar, skin);
    window_combo->add_control(combo);

    return window_combo;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
control_i* list_c::got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
{
    ui32 EMP_TT_MAX_VAR(ui32Selection);
    pc_control_i pFocus = got_focus_controls(navigables, a_eFocus, a_bSelect, a_bPush, ui32Selection, a_eChange);
    if (pFocus != nullptr && emp::tt::is_not_max(ui32Selection))
    {
        if (a_bPush)
        {
            focused();
            set_selection(ui32Selection, m_ui32FocusItem, false, a_eChange);
        }
        return pFocus;
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::focused(void)
{
    c_size stCount = navigables.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        line_c const* const pLine = navigables.val(stIndex);
        if (state_focus(pLine))
        {
            m_ui32FocusLine = emp::ast::cast<ui32>(stIndex);

            c_size stItemCount = pLine->items.count();
            for (size_t stItem = 0; stItem < stItemCount; ++stItem)
            {
                item_i const& rItem = *pLine->items.val(stItem);
                if (state_focus(&rItem.control()))
                {
                    m_ui32FocusItem = emp::ast::cast<ui32>(stItem);
                    break;
                }
            }

            if (pLine != &column)
            { panel.ensure_visible(pLine); }

            break;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list_c::unselect(c_bool a_bTransmit)
{
// @@0 check ??
///*    if (focus_in_hierarchy(&panel.scroller) ||
//        focus_in_hierarchy(&column))
//    {
//        panel.scroller.unselect(true);
//        column.unselect(true);
//    }
//    else*/
//    { emp::tt::silent(tbase1::unselect(a_bTransmit)); }
//return false;
    return tbase1::unselect(a_bTransmit);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool resize_combo(window_c* const a_pWindow, combo_c* const a_pCombo);
EMP_INTERN EMP_RETURN bool resize_combo(window_c* const a_pWindow, combo_c* const a_pCombo)
{
    if (a_pWindow == nullptr || a_pCombo == nullptr ||
        a_pWindow->state().visible == false ||
        a_pWindow->state().enable == false)
    { return true; }

    cpc_control_i pControl = a_pWindow->hook;

    a_pCombo->expand_last_column_(false).state().autosize.xy(true);
    EMP_RET_FALSE_IF_NOT(a_pCombo->resize());
    // @@0  panel.user.scrollbars
    EMP_RET_FALSE_IF_NOT(a_pCombo->resize());

    c_float fItemW = pControl->transformation().width();
    c_float fWidth = emp::tt::maxof(a_pCombo->transformation().width(), fItemW);
    a_pCombo->expand_last_column_(true).state().autosize.x_(false);
    a_pCombo->transformation().width_(fWidth);
    EMP_RET_FALSE_IF_NOT(a_pCombo->resize());

    c_float fComboX = fItemW - fWidth;
    a_pWindow->transformation().position_(fComboX + 1, -1 - pControl->transformation().height());
    EMP_RET_FALSE_IF_NOT(a_pWindow->resize());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool resize_text(window_c* const a_pWindow, text_c* const a_pText);
EMP_INTERN EMP_RETURN bool resize_text(window_c* const a_pWindow, text_c* const a_pText)
{
    if (a_pWindow == nullptr || a_pText == nullptr ||
        a_pWindow->state().visible == false ||
        a_pWindow->state().enable == false)
    { return true; }

    cpc_control_i pControl = a_pWindow->hook;

    a_pText->state().autosize.xy(true);
    EMP_RET_FALSE_IF_NOT(a_pText->resize());
    EMP_RET_FALSE_IF_NOT(a_pText->resize());

    c_float fItemW = pControl->transformation().width();
    c_float fItemH = pControl->transformation().height();
    c_float fWidth = emp::tt::maxof(a_pText->transformation().width(), fItemW);
    a_pText->state().autosize.x_(false);
    a_pText->transformation().width_(fWidth);
    EMP_RET_FALSE_IF_NOT(a_pText->resize());

    c_float fTextH = -fItemH;
    c_float fTextX = fItemW - fWidth;
    a_pWindow->transformation().position_(fTextX, fTextH);
    EMP_RET_FALSE_IF_NOT(a_pWindow->resize());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list_c::resize(void)
{
    for (size_t stPass = 0; stPass < 2; ++stPass)
    {
    column.state().visible_(show_columns);
    spacer.state().visible_(show_columns);

    c_size stColumnCount = column.items.count();
    EMP_ASSERT(stColumnCount > 0);

    c_size stLineCount = m_vpLines.count();

    c_float fDefaultX = skin && skin->label && skin->label->button && skin->label->button->padding ? skin->label->button->padding->left() : 0;

    bool bCollapse = false;
    float fCurrentX = 0;

    float fMaxCollapserW = 0;

    constexpr const ui64 uiShift = 1;

    for (size_t stColumn = 0; stColumn < stColumnCount; ++stColumn)
    {
        size_t stMaxDepth = 0;
        ui64 uiVisible = 0;
        if (stColumn == 0)
        {
            for (size_t stLine = 0; stLine < stLineCount; ++stLine)
            {
                line_c* const pLine = m_vpLines.val(stLine);
                EMP_ASSERT(pLine != nullptr);
                ui64 uiDepth = pLine->depth;
                ui64 uiMask = emp::tt::max<ui64>::value << uiDepth;
                ui64 uiNegMask = ~uiMask;

                c_bool bVisible = (uiVisible & uiNegMask) == uiNegMask;
                if (bVisible)
                { stMaxDepth = emp::tt::maxof(stMaxDepth, pLine->depth); }
                pLine->state().visible_(bVisible);

                if (pLine->state().visible && pLine->collapsible())
                { bCollapse = true; }
                if (pLine->expanded() && bVisible)
                { uiVisible = uiVisible | (uiShift << uiDepth); }
                else
                { uiVisible = uiVisible & ~(uiShift << uiDepth); }
            }
        }

        float fMaxX = 0;

        if (show_columns)
        {
            item_i& rItem = *column.items.val(stColumn);
            control_i& rControl = rItem.control();
            rItem.autosize().xy(true);
            EMP_RET_FALSE_IF_NOT(rControl.resize());
            rItem.autosize().xy(false);
            fMaxX = emp::tt::maxof(fMaxX, rControl.transformation().width());
        }

        if (stColumn == 0)
        {
            for (size_t stLine = 0; stLine < stLineCount; ++stLine)
            {
                line_c* const pLine = m_vpLines.val(stLine);
                EMP_ASSERT(pLine != nullptr);
                if (pLine->state().visible == false)
                { continue; }

                if (pLine->collapsible())
                {
                    picture_c& rCollapser = pLine->collapser();

                    rCollapser.state().visible_(pLine->reflection_count() > 0);
                    if (rCollapser.state().visible)
                    {
                        rCollapser.state().autosize.xy(true);
                        EMP_RET_FALSE_IF_NOT(rCollapser.resize());
                        rCollapser.state().autosize.xy(false);

                        fMaxCollapserW = emp::tt::maxof(fMaxCollapserW, rCollapser.transformation().width());
                    }
                }
            }
        }

        for (size_t stLine = 0; stLine < stLineCount; ++stLine)
        {
            line_c* const pLine = m_vpLines.val(stLine);
            EMP_ASSERT(pLine != nullptr);
            if (pLine->state().visible == false)
            { continue; }

            item_i& rItem = *pLine->items.val(stColumn);
            control_i& rControl = rItem.control();
            rItem.autosize().xy(true);
            EMP_RET_FALSE_IF_NOT(rControl.resize());
            rItem.autosize().xy(false);

            float fControlW = rControl.transformation().width();
            if (stColumn == 0)
            {
                c_size stDepth = pLine->depth;
                float fDepth = emp::ast::cast<float>(stDepth) * fMaxCollapserW;
                fControlW += fDepth;
                if (bCollapse)
                { fControlW += fDefaultX + fMaxCollapserW; }
            }
            fMaxX = emp::tt::maxof(fMaxX, fControlW);
        }

        if (show_columns)
        {
            control_i& rControl = column.items.val(stColumn)->control();
            rControl.transformation().x_(fCurrentX).width_(fMaxX);
        }

        for (size_t stLine = 0; stLine < stLineCount; ++stLine)
        {
            line_c* const pLine = m_vpLines.val(stLine);
            EMP_ASSERT(pLine != nullptr);
            if (pLine->state().visible == false)
            { continue; }

            control_i& rControl = pLine->items.val(stColumn)->control();
            float fControlX = fCurrentX;
            float fControlW = fMaxX;
            if (stColumn == 0)
            {
                c_float fCollapse = bCollapse ? fDefaultX : 0;
                c_size stDepth = pLine->depth;
                float fDepth = emp::ast::cast<float>(stDepth) * fMaxCollapserW + fCollapse;

                if (pLine->collapsible())
                {
                    picture_c& rCollapser = pLine->collapser();
                    if (rCollapser.state().visible)
                    { rCollapser.transformation().position_(fDepth, 0); }
                }

                fControlX += fDepth + fMaxCollapserW;// + fCollapse;
                fControlW -= fDepth + fMaxCollapserW;// + fCollapse;
            }
            rControl.transformation().x_(fControlX).width_(fControlW);
        }

        fCurrentX += fMaxX;
    }

    float fHeaderY = 0;
    if (show_columns)
    {
        float fMaxY = 0;
        for (size_t stColumn = 0; stColumn < stColumnCount; ++stColumn)
        {
            c_control_i& rControl = column.items.val(stColumn)->control();
            fMaxY = emp::tt::maxof(fMaxY, rControl.transformation().height());
        }

        for (size_t stColumn = 0; stColumn < stColumnCount; ++stColumn)
        {
            control_i& rControl = column.items.val(stColumn)->control();
            rControl.transformation().height_(fMaxY);
            EMP_RET_FALSE_IF_NOT(rControl.resize());
        }
        column.transformation().size_(fCurrentX, fMaxY);
        spacer.transformation().width_(fCurrentX);
        fHeaderY += fMaxY;
        fHeaderY += spacer.transformation().height();
    }

    size_t EMP_TT_MAX_VAR(stCurrentDepth);

    float fLineMaxHeight = 0;

    float fCurrentY = 0;
    line_c* pLastLine = nullptr;
    for (size_t stLine = 0; stLine < stLineCount; ++stLine)
    {
        float fMaxY = 0;

        line_c* const pLine = m_vpLines.val(stLine);
        EMP_ASSERT(pLine != nullptr);
        if (pLine->state().visible == false)
        { continue; }

        pLastLine = pLine;
        for (size_t stColumn = 0; stColumn < stColumnCount; ++stColumn)
        {
           c_control_i& rControl = pLine->items.val(stColumn)->control();
           fMaxY = emp::tt::maxof(fMaxY, rControl.transformation().height());
        }

        if (pLine->collapsible())
        {
            picture_c& rCollapser = pLine->collapser();
            if (rCollapser.state().visible)
            { fMaxY = emp::tt::maxof(fMaxY, rCollapser.transformation().height()); }
        }

        pLine->transformation().y_(-fCurrentY).size_(fCurrentX, fMaxY);

        fLineMaxHeight = emp::tt::maxof(fLineMaxHeight, fMaxY);

        for (size_t stColumn = 0; stColumn < stColumnCount; ++stColumn)
        {
            control_i& rControl = pLine->items.val(stColumn)->control();
            rControl.transformation().height_(fMaxY);
            EMP_RET_FALSE_IF_NOT(rControl.resize());
        }

        if (pLine->collapsible())
        {
            picture_c& rCollapser = pLine->collapser();
            if (rCollapser.state().visible)
            { rCollapser.transformation().height_(fMaxY); }
        }

        c_size stDepth = pLine->depth;
        if (emp::tt::is_max(stCurrentDepth))
        { stCurrentDepth = stDepth; }

        pLine->skin_(skin && (skin->lines.count() > 0) ? skin->line(pLine->depth) : nullptr);

        c_bool bBottom = state_focus(pLine) || state_select(pLine);
        c_bool bTop = stCurrentDepth == 0 || stCurrentDepth != stDepth || bBottom;
        pLine->color().foreground.border.border = border_e::All;
        pLine->border_(true, bTop, true, bBottom);
        stCurrentDepth = pLine->depth;

        fCurrentY += fMaxY;
    }

    if (pLastLine != nullptr)
    {
        pLastLine->skin_(skin && (skin->lines.count() > 0) ? skin->line(pLastLine->depth) : nullptr);
        pLastLine->color().foreground.border.border = border_e::All;
        pLastLine->border_(true, pLastLine->depth == 0 || state_focus(pLastLine) || state_select(pLastLine), true, true);
    }

    panel.state().autosize = state().autosize;
    EMP_RET_FALSE_IF_NOT(panel.refresh_scrolling(fCurrentX, fCurrentY));
    EMP_RET_FALSE_IF_NOT(panel.refresh_scrolling(fCurrentX, fCurrentY));
    panel.transformation().y_(-fHeaderY);
    //panel.resize();

    if (state().autosize.x)
    //{ transformation().set_width(fCurrentX); }
    { transformation().width_(panel.transformation().width()); }
    else
    { panel.transformation().width_(transformation().width()); }

    if (state().autosize.y)
    //{ transformation().set_height(fHeaderY + fCurrentY); }
    { transformation().height_(fHeaderY + panel.transformation().height()); }
    else if (autosize_count > 0)
    {
        c_float fLineHeight = (fLineMaxHeight > 0 ?
            fLineMaxHeight : column.transformation().height()) *
                emp::ast::cast<float>(autosize_count);

        transformation().height_(fHeaderY + fLineHeight);
        panel.transformation().height_(fLineHeight);
    }
    else
    { panel.transformation().height_(transformation().height() + panel.transformation().y()); }

    c_float fX = panel.transformation().x();
    column.transformation().x_(fX);
    spacer.transformation().x_(fX);

    c_float fPosDeltaW =
        emp::tt::maxof(0.0f, panel.delta_user_w());
            //panel.delta_docker_w();

    if (show_columns)
    {
        c_float fWidth = column.transformation().width() + fPosDeltaW;
        column.transformation().width_(fWidth);
        EMP_RET_FALSE_IF_NOT(column.resize());
        spacer.transformation().width_(fWidth);
        EMP_RET_FALSE_IF_NOT(spacer.resize());
    }
    for (size_t stLine = 0; stLine < stLineCount; ++stLine)
    {
        line_c* const pLine = m_vpLines.val(stLine);
        EMP_ASSERT(pLine != nullptr);
        if (pLine->state().visible == false)
        { continue; }

        pLine->state().index_(emp::ast::cast<ui32>(stLine + 1));
        c_float fWidth = pLine->transformation().width() + fPosDeltaW;
        pLine->transformation().width_(fWidth);

        if (expand_last_column)
        {
            c_size stItemCount = pLine->items.count();
            if (stItemCount > 0)
            {
                control_i& rControl = pLine->items.val(stItemCount - 1)->control();
                rControl.transformation().width_(fWidth - rControl.transformation().x());
                EMP_RET_FALSE_IF_NOT(rControl.resize());
            }
        }
        EMP_RET_FALSE_IF_NOT(pLine->resize());
    }

/*  @@0 remove  if (emp::str::cmp(name(), "m_pPlayListMusic"))
    {
    EMP_LOG_INFO("COLUMN #0", column.transformation().width());
    if (m_vpLines.count() > 0)
    {
        EMP_LOG_INFO("LINE #0", m_vpLines.val(0)->transformation().width());
    }
    }*/
    EMP_RET_FALSE_IF_NOT(resize_combo(window_combo, combo));
    EMP_RET_FALSE_IF_NOT(resize_text(window_text, text));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list_c::transform(stack_c& a_rStack)
{
    if (state().visible)
    { EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack)); }

    c_float w = transformation().width();
    c_float y0 = -transformation().height();
    c_float y1 = y0 + column.transformation().height();

    m_pHeaderMeshVertices->create(emp::gfx::usage_e::Dynamic);

    if (show_columns) // @@0 remove ?
    {
        c_v4f v4Color = (skin && skin->header_color) ? skin->header_color->rgba : v4f::all(0);

        c_float fDepth = depth(transformation().fore_depth, 0);
    
        m_pHeaderMeshVertices->add({ 0, y0 , fDepth }, v4Color);
        m_pHeaderMeshVertices->add({ w, y0 , fDepth }, v4Color);
        m_pHeaderMeshVertices->add({ 0, y1 , fDepth }, v4Color);
        m_pHeaderMeshVertices->add({ w, y1 , fDepth}, v4Color);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    /* @@0 remove if (skin->show_columns())
    {
        auto& rDevice = EMP_GRAPHICS;
        rDevice.set_vertex_buffer(m_pHeaderMeshVertices);
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(color_attribute_program());
        rDevice.set_uniform(uniform_e::m44Transform, transformation().matrix());
        rDevice.render(emp::gfx::primitive_e::Triangles);
    }*/

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void get_next(emp::cnt::vector<line_c*> const& a_rvpLines, ui32& a_rui32Line, ui32& a_rui32Item,
    c_bool a_bBack, c_bool a_bHorz, c_bool a_bWrap, c_bool a_bColumn);
EMP_INTERN void get_next(emp::cnt::vector<line_c*> const& a_rvpLines, ui32& a_rui32Line, ui32& a_rui32Item,
    c_bool a_bBack, c_bool a_bHorz, c_bool a_bWrap,c_bool a_bColumn)
{
    c_ui32 ui32Line = a_rui32Line;
    c_ui32 ui32LineCount = emp::ast::cast<ui32>(a_rvpLines.count());
    line_c* const pLine = ui32Line < ui32LineCount ? a_rvpLines.val(ui32Line) : nullptr;

    if (pLine == nullptr)
    { return; }

    if (a_bHorz)
    {
        if (pLine->collapsible())
        {
            if (pLine->expanded() == a_bBack)
            {
                pLine->expanded_(a_bBack == false);
                return;
            }
        }

//horizontal case
        c_ui32 ui32Item = a_rui32Item;

        pc_control_i pControl = get_focusable(nullptr, pLine, a_rui32Item, a_bBack, false, a_rui32Item);
        if (a_bWrap)
        {
            if (pControl == nullptr)
            { emp::tt::silent(get_focusable(nullptr, pLine, emp::tt::max<ui32>::value, a_bBack, false, a_rui32Item)); }
            return;
        }
        else
        {
            if (pControl == nullptr ||
                pControl == &pLine->collapser())
            { a_rui32Item = ui32Item; }
            else
            { return; }
        }
    }

//vertical case
    if (a_bBack)
    {
        while (true)
        {
            a_rui32Line = (a_rui32Line > (a_bColumn ? 0u : 1u) ? a_rui32Line : ui32LineCount) - 1;
            if (a_rvpLines.val(a_rui32Line)->state().enable)
            { break; }
        }
    }
    else
    {
        while (true)
        {
            c_ui32 ui32Next = a_rui32Line + 1;
            a_rui32Line = ui32Next >= ui32LineCount ? (a_bColumn ? 0 : 1) : ui32Next;
            if (a_rvpLines.val(a_rui32Line)->state().enable)
            { break; }
        }
    }

// store_indices

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false ||
        state().enable == false)
    { return return_e::Unused; }

    c_return_e eReturn = tbase1::process_keyboard(a_rEvent);
    if (eReturn != return_e::Unused)
    { return eReturn; }

    auto const& rKeyboard = a_rEvent.keyboard;
    c_bool bPressed = rKeyboard.m_bPressed;
    c_bool bRepeat = rKeyboard.m_bRepeat;

    if ((
        emp::gui::focus_in_hierarchy(&panel.scroller) ||
        emp::gui::focus_in_hierarchy(&column)) &&
        rKeyboard.modified() == false)
    {
        emp::app::c_key_e eCode = rKeyboard.m_eKey;

        ui32 ui32Line = m_ui32FocusLine;
        ui32 ui32Item = m_ui32FocusItem;

        c_change_e eChange = change_e::Keyboard;
        switch (eCode)
        {
            case emp::app::key_e::Right:
            {
                if (bPressed)
                {
                    get_next(navigables, ui32Line, ui32Item, false, true, wrap_lines, show_columns);
                    set_selection(ui32Line, ui32Item, true, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                goto list_process_mouse_unused_label;
            }
            case emp::app::key_e::Down:
            {
                if (bPressed)
                {
                    get_next(navigables, ui32Line, ui32Item, false, false, wrap_lines, show_columns);
                    set_selection(ui32Line, ui32Item, true, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                goto list_process_mouse_unused_label;
            }
            case emp::app::key_e::Left:
            {
                if (bPressed)
                {
                    get_next(navigables, ui32Line, ui32Item, true, true, wrap_lines, show_columns);
                    set_selection(ui32Line, ui32Item, true, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                goto list_process_mouse_unused_label;
            }
            case emp::app::key_e::Up:
            {
                if (bPressed)
                {
                    get_next(navigables, ui32Line, ui32Item, true, false, wrap_lines, show_columns);
                    set_selection(ui32Line, ui32Item, true, eChange);
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                goto list_process_mouse_unused_label;
            }
            case emp::app::key_e::Space:
            case emp::app::key_e::Enter:
            {
                if (bRepeat == false)
                {
                    if (bPressed)
                    {
                        if (enter == false)
                        {
                            enter = true;
                            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                        }
                    }
                    else
                    {
                        if (enter == true)
                        {
                            EMP_RET_X_IF_NOT(return_e::Error, set_selection_double_click(m_ui32FocusLine, m_ui32FocusItem, eChange));
                            enter = false;
                        }
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
                else
                { return return_e::Unused; }
            }
            case emp::app::key_e::none:
            case emp::app::key_e::Tabulation:
            case emp::app::key_e::Backspace:
            case emp::app::key_e::Escape:
            case emp::app::key_e::Delete:
            case emp::app::key_e::LeftAlt:
            case emp::app::key_e::RightAlt:
            case emp::app::key_e::LeftShift:
            case emp::app::key_e::RightShift:
            case emp::app::key_e::LeftCtrl:
            case emp::app::key_e::RightCtrl:
            case emp::app::key_e::LeftCommand:
            case emp::app::key_e::RightCommand:
            case emp::app::key_e::Period:
            case emp::app::key_e::SemiColon:
            case emp::app::key_e::Slash:
            case emp::app::key_e::Comma:
            case emp::app::key_e::Num_0:
            case emp::app::key_e::Num_1:
            case emp::app::key_e::Num_2:
            case emp::app::key_e::Num_3:
            case emp::app::key_e::Num_4:
            case emp::app::key_e::Num_5:
            case emp::app::key_e::Num_6:
            case emp::app::key_e::Num_7:
            case emp::app::key_e::Num_8:
            case emp::app::key_e::Num_9:
            case emp::app::key_e::Alpha_A:
            case emp::app::key_e::Alpha_B:
            case emp::app::key_e::Alpha_C:
            case emp::app::key_e::Alpha_D:
            case emp::app::key_e::Alpha_E:
            case emp::app::key_e::Alpha_F:
            case emp::app::key_e::Alpha_G:
            case emp::app::key_e::Alpha_H:
            case emp::app::key_e::Alpha_I:
            case emp::app::key_e::Alpha_J:
            case emp::app::key_e::Alpha_K:
            case emp::app::key_e::Alpha_L:
            case emp::app::key_e::Alpha_M:
            case emp::app::key_e::Alpha_N:
            case emp::app::key_e::Alpha_O:
            case emp::app::key_e::Alpha_P:
            case emp::app::key_e::Alpha_Q:
            case emp::app::key_e::Alpha_R:
            case emp::app::key_e::Alpha_S:
            case emp::app::key_e::Alpha_T:
            case emp::app::key_e::Alpha_U:
            case emp::app::key_e::Alpha_V:
            case emp::app::key_e::Alpha_W:
            case emp::app::key_e::Alpha_X:
            case emp::app::key_e::Alpha_Y:
            case emp::app::key_e::Alpha_Z:
            default:
            {
                goto list_process_mouse_unused_label;
            }
        }
    }

// unused label
list_process_mouse_unused_label:
    enter = false;
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::process_left_click(c_size a_stClick, c_change_e a_eChange)
{
    c_ui32 ui32Selection = m_ui32SelectionLine;

    c_ui32 ui32Count = emp::ast::cast<ui32>(navigables.count());
    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    {
        line_c* const pLine = navigables.val(ui32Index);
        EMP_ASSERT(pLine != nullptr);

        if (pLine->state().visible &&
            pLine->state().enable &&
            pLine->state().hover)
        {
            c_ui32 ui32ItemCount = emp::ast::cast<ui32>(pLine->items.count());
            for (ui32 ui32Item = 0; ui32Item < ui32ItemCount; ++ui32Item)
            {
                c_control_i& rControl = pLine->items.val(ui32Item)->control();

                if (rControl.state().visible &&
                    rControl.state().enable &&
                    rControl.state().hover)
                {
                    if (a_stClick == 1)
                    {
                        if (ui32Selection != ui32Index)
                        {
                            if (on_item_click.is_valid())
                            {
                                return on_item_click(ui32Index, ui32Item, a_eChange);
                            }
                            else
                            {
                                set_selection(ui32Index, ui32Item, true, a_eChange);
                                return EMP_GUI_RETURNE(return_e::Consumed);
                            }
                        }
                    }
                    else if (a_stClick == 2)
                    {
                        if (ui32Selection == ui32Index)
                        {
                            if (on_item_double_click.is_valid())
                            {
                                return on_item_double_click(ui32Index, ui32Item, a_eChange);
                            }
                            else
                            {
                                EMP_RET_X_IF_NOT(return_e::Error, set_selection_double_click(ui32Index, ui32Item, a_eChange));
                                return EMP_GUI_RETURNE(return_e::Consumed);
                            }
                        }
                    }
                }
            }

            return return_e::Unused;//EMP_GUI_RETURNE(return_e::Consumed);
        }
    }
    if (a_stClick == 1)
    {
        set_selection(
            emp::tt::max<ui32>::value,
            emp::tt::max<ui32>::value, true,
            a_eChange);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    auto const& rMouse = a_rEvent.mouse;
    emp::app::c_button_e eButton = rMouse.m_eButton;
    if (a_rEvent.mouse_pressed() || a_rEvent.mouse_moved()) // @@0 remove
    {
        int kk = 0;
        ++kk;
    }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    if (state().enable &&
        state().hover &&
        a_rEvent.keyboard.modified() == false &&
        a_rEvent.mouse_pressed())
    {
        c_change_e eChange = change_e::Mouse;
        if (eButton == emp::app::button_e::WheelUp)
        {
            panel.scroll_up();
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eButton == emp::app::button_e::WheelDown)
        {
            panel.scroll_down();
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eButton == emp::app::button_e::Right)
        {
            c_return_e eReturn = process_left_click(rMouse.m_stTapCount, eChange);
            if (popup != nullptr)
            {
                popup->show(gl_to_gui(rMouse.m_v2fPosition, transformation().matrix), change_e::Mouse);
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
            return eReturn;
        }
        else if (eButton == emp::app::button_e::Left)
        { return process_left_click(rMouse.m_stTapCount, eChange); }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::clear(void)
{
    set_selection(emp::tt::max<ui32>::value, emp::tt::max<ui32>::value, true, change_e::Code);
    if (focus_in_hierarchy(this))
    { EMP_GUI_MANAGER->deactivate_focus(); }
    emp::cnt::foreach(m_vpLines, emp::mem::safe_delete_reset_t());
    empty_lines();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::empty_lines(void)
{
    set_selection(emp::tt::max<ui32>::value, emp::tt::max<ui32>::value, true, change_e::Code);
    panel.scroller.clear_controls();
    m_vpLines.resize(0);
    navigables.resize(0);
    navigables.add(&column);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::set_check(c_ui32 a_ui32Line, c_bool a_bValue)
{
    line_c* const pLine = navigables.val(a_ui32Line);
    pLine->state().check_(a_bValue);
    c_size stCount = pLine->items.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { pLine->items.val(stIndex)->control().state().check_(a_bValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::set_selection(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_bool a_bFocus, c_change_e a_eChange)
{
    EMP_TT_MAX_VAR(m_ui32EditLine);
    EMP_TT_MAX_VAR(m_ui32EditItem);

    m_ui32FocusLine = a_ui32Line;
    m_ui32FocusItem = a_ui32Item;

    if (a_bFocus)
    {
        c_ui32 ui32OldLine = m_ui32SelectionLine;
        c_ui32 ui32OldItem = m_ui32SelectionItem;

        m_ui32SelectionLine = a_ui32Line;
        m_ui32SelectionItem = a_ui32Item;
        if ((
            ui32OldLine != a_ui32Line ||
            ui32OldItem != a_ui32Item) &&
            on_selection_changed.is_valid())
        { on_selection_changed(a_ui32Line, a_ui32Item, a_eChange); }

        if (navigables.is_valid_index(m_ui32SelectionLine))
        {
            line_c* const pLine = navigables.val(m_ui32SelectionLine);
            EMP_ASSERT(pLine != nullptr);
            pLine->set_selection(m_ui32SelectionItem, a_bFocus, a_eChange);
        }
    }

    if (navigables.is_valid_index(m_ui32FocusLine))
    {
        line_c* const pLine = navigables.val(m_ui32FocusLine);
        if (pLine != &column)
        { panel.ensure_visible(pLine); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list_c::set_selection_double_click(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange)
{
    if (emp::tt::is_not_max(a_ui32Line) &&
        emp::tt::is_not_max(a_ui32Item))
    {
        line_c* const pLine = navigables.val(a_ui32Line);
        EMP_ASSERT(pLine != nullptr);

        item_i& rItem = *pLine->items.val(a_ui32Item);
        emp::rfx::type_i* const pReflection = rItem.reflection();
        if (pReflection != nullptr)
        {
            emp::rfx::c_type_e eType = pReflection->id()->m_eType;

            if (rItem.control().state().focusable &&
                (pReflection->settable() || pReflection->bindable()) &&
                eType != emp::rfx::type_e::eBool)
            {
                emp::rfx::enum_i* pEnumerable = nullptr;
                /*if (pReflection->e_type() == emp::rfx::type_e::eBool)
                {
                }
                else*/
                if (eType == emp::rfx::type_e::Enum)
                {
                    pEnumerable = static_cast<emp::rfx::enum_i*>(pReflection); // @@0 enumerable_i
                }
                else
                {
                    class_c const* const pClass = get_class(pReflection);
                    if (pClass != nullptr &&
                        pClass->over)
                    { pEnumerable = pClass->field.enumeration; }
                }

                m_ui32EditLine = a_ui32Line;
                m_ui32EditItem = a_ui32Item;

                c_control_i& rControl = rItem.control();

                if (pEnumerable != nullptr)
                {
                    window_combo = get_or_alloc_combo_window();
                    #if defined EMP_XX_DEBUG // @@0 remove 
                    window_combo->m_stDebugId = 38;
#endif
                    window_combo->hook_(&rControl);

                    combo->set_reflection(pEnumerable);
                    combo->set_on_edit(true, a_eChange);
                    combo->state().visible_(true).enable_(true);

                    EMP_RET_FALSE_IF_NOT(resize_combo(window_combo, combo));

                    EMP_GUI_MANAGER->set_focus(window_combo, nullptr);
                    emp::tt::silent(EMP_GUI_MANAGER->show_window(window_combo, true, true, a_eChange, false));
                }
                else
                {
                    window_text = get_or_alloc_text_window();
                    window_text->hook_(&rControl);

                    text->set_reflection(pReflection);
                    text->set_on_edit(true, a_eChange);
                    text->state().visible_(true).enable_(true);

                    EMP_RET_FALSE_IF_NOT(resize_text(window_text, text));

                    EMP_GUI_MANAGER->set_focus(window_text, nullptr);
                    emp::tt::silent(EMP_GUI_MANAGER->show_window(window_text, true, true, a_eChange, false));
                }

                return true;
            }
        }
    }

    if (on_selection_double_click.is_valid())
    { on_selection_double_click(a_ui32Line, a_ui32Item, a_eChange); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove void list_c::insert_line(c_size a_stIndex, c_astring& a_sValue)
{
    line_c* const pLine = insert_new_line(a_stIndex);

    item_i* const pItem = create_item_t<label_c, label_skin_s, astring>::apply(
        a_sValue, &skin->label(), nullptr);

    pLine->add_item(pItem);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN line_c* list_c::new_line(c_ui32 a_ui32Index)
{
    line_c* const pLine = EMP_ALLOCATE(line_c);
    pLine->skin_((skin != nullptr && skin->lines.count() > 0) ? skin->line(pLine->depth) : nullptr)
        .state().index_(a_ui32Index + 1).focusable_(true);
    return pLine;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::insert_line(c_ui32 a_ui32Index, line_c* const a_pLine)
{
    EMP_ASSERT(a_pLine != nullptr);

    panel.scroller.insert_control(a_ui32Index, a_pLine);

    m_vpLines.insert(a_ui32Index, a_pLine);
    navigables.insert(a_ui32Index + 1, a_pLine);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN line_c* list_c::new_line(c_ui32 a_ui32Index, emp::rfx::field_s const& a_rField, cpc_label_skin_s a_pSkin, c_ui32 a_ui32ItemIndex, c_alignment_e a_eAlignment)
{
    line_c* const pLine = new_line(a_ui32Index);

    item_t<label_c>* const pItem = new_item<label_c>(a_pSkin, nullptr, emp::ast::cast<ui32>(a_ui32ItemIndex));
    pItem->control().alignment_(a_eAlignment);
    pItem->control().set_text(a_rField.m_sLabel);

    pLine->add_item(pItem);
    return pLine;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::insert_line(c_ui32 a_ui32Index, emp::rfx::field_s const& a_rField, cpc_label_skin_s a_pSkin, c_ui32 a_ui32ItemIndex)
{
    line_c* const pLine = new_line(a_ui32Index, a_rField, a_pSkin, a_ui32ItemIndex);
    insert_line(a_ui32Index, pLine);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::add_line(c_u8string& a_sValue)
{
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpLines.count());
    line_c* const pLine = new_line(ui32Index);
    insert_line(ui32Index, pLine);
    add_item(pLine, a_sValue, 0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::add_line(emp::loc::string_s const& a_eValue)
{
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpLines.count());
    line_c* const pLine = new_line(ui32Index);
    insert_line(ui32Index, pLine);

    item_t<label_c>* const pItem = new_item<label_c>(skin != nullptr ? skin->label : nullptr, nullptr, 0);
    pItem->control().set_text(a_eValue);
    pLine->add_item(pItem);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::add_item(line_c* const a_pLine, c_u8string& a_sValue, c_size a_stIndex)
{
    item_t<label_c>* const pItem = new_item<label_c>(skin != nullptr ? skin->label : nullptr, nullptr, emp::ast::cast<ui32>(a_stIndex));
    pItem->control().set_text(a_sValue);
    a_pLine->add_item(pItem);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::add_item(emp::rfx::type_i* const a_pParent, line_c* const a_pLine, emp::rfx::field_s const& a_rField, emp::gui::class_c const* const a_pClass)
{
    c_size stCount = a_pLine->items.count();

    emp::rfx::type_i* const pType = a_rField.m_pType;
    emp::rfx::c_type_e eType = pType->id()->m_eType;

    c_bool bEditable = pType->bindable() || pType->settable();

    emp::str::formater_i* pFormater = nullptr;
    field_s const* pField = nullptr;
    if (designer != nullptr && a_pParent != nullptr)
    {
        class_c const* pClass = a_pClass;
        if (pClass == nullptr)
        { pClass = get_class(a_pParent); }
        if (pClass != nullptr)
        {
            pField = pClass->get(a_rField.m_sName);
            if (pField != nullptr)
            {
                pFormater = pField->formater;
                if (pField->inner != nullptr)
                { pField = &pField->inner->field; }
            }
        }
    }

    if (designer != nullptr)
    {
    }

    if (eType == emp::rfx::type_e::eBool)
    {
        item_t<toggle_c>* const pToggle = new_item<toggle_c>(skin != nullptr ? skin->toggle : nullptr, pType, emp::ast::cast<ui32>(stCount));
        pToggle->control().set_text(pType);
        pToggle->control().state().focusable_(bEditable).index_(emp::ast::cast<ui32>(stCount));
        a_pLine->add_item(pToggle);
        return;
    }
    else if (eType == emp::rfx::type_e::Enum)
    {
        item_t<icon_c>* const pIcon = new_item<icon_c>(skin != nullptr ? skin->icon : nullptr, pType, emp::ast::cast<ui32>(stCount));
        pIcon->control().label.set_text(pType);
        pIcon->control().picture_(skin != nullptr ? skin->gfx_edit : nullptr);
        pIcon->control().splitter.orientation_(dock_e::Right);
        pIcon->control().picture.on_push.function_(on_edit);
        pIcon->control().state().focusable_(bEditable).index_(emp::ast::cast<ui32>(stCount));
        a_pLine->add_item(pIcon);
        return;
    }
    else if (eType == emp::rfx::type_e::Float32 && bEditable)
    {
        item_t<slider_c>* const pSlider = new_item<slider_c>(skin != nullptr ? skin->slider : nullptr, pType, emp::ast::cast<ui32>(stCount));
        pSlider->control().set_text(pType);
        pSlider->control().state().focusable_(bEditable);
        pSlider->control().min_(pField ? pField->min : -1).max_(pField ? pField->max : 1).step_(pField ? pField->step : 0.1f);
        a_pLine->add_item(pSlider);
    }
    else if (eType == emp::rfx::type_e::UInt32 && bEditable)
    {
        item_t<slider_c>* const pSlider = new_item<slider_c>(skin != nullptr ? skin->slider : nullptr, pType, emp::ast::cast<ui32>(stCount));
        pSlider->control().set_text(pType);
        pSlider->control().state().focusable_(bEditable);
        pSlider->control().min_(pField ? pField->min : 0).max_(pField ? pField->max : 1).step_(pField ? pField->step : 1);
        a_pLine->add_item(pSlider);
    }
    else
    {
        class_c const* const pClass = get_class(pType);
        if (pClass != nullptr &&
            pClass->over &&
            pClass->field.on_new_item.is_valid())
        {
            item_i* const pItem = pClass->field.on_new_item(pType, on_edit, skin, stCount);
            a_pLine->add_item(pItem);
            return;
        }

        item_t<label_c>* const pLabelButton = new_item<label_c>(skin != nullptr ? skin->label : nullptr, pType, emp::ast::cast<ui32>(stCount));
        pLabelButton->control().set_text(pType);
        pLabelButton->control().formater_(pFormater).state().focusable_(bEditable);
        a_pLine->add_item(pLabelButton);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list_c::reflect(emp::rfx::type_i* const a_pParent, c_size EMP_SILENT(a_stDepth), emp::rfx::field_s const& a_rField)
{
    add_item(a_pParent, current_line, a_rField, nullptr);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::add_line(emp::rfx::type_i* const a_pReflection)
{
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpLines.count());
    line_c* const pLine = new_line(ui32Index);
    insert_line(ui32Index, pLine);

    pLine->reflection_(a_pReflection);
    current_line = pLine;
    if (a_pReflection != nullptr)
    { EMP_ASSERT_BOOL_CALL(a_pReflection->iterate(0, on_reflect)); }
    current_line = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::add_line(line_c* const a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpLines.count());
    insert_line(ui32Index, a_pValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
list_c& list_c::add_column_(emp::loc::string_s const& a_eLabel)
{
    cpc_label_skin_s pColumnSkin = (skin != nullptr) ? skin->column : nullptr;
    item_t<label_c>* pItem = new_item<label_c>(pColumnSkin, nullptr, emp::ast::cast<ui32>(column.items.count()));
    pItem->control().set_text(a_eLabel);
    pItem->control().state().focusable_(true);
    EMP_ASSERT(pItem != nullptr);
    column.add_item(pItem);
    columns.add(&pItem->control());
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void list_c::remove_line(c_size a_stIndex)
{
    line_c* const pLine = m_vpLines.val(a_stIndex);
    EMP_ASSERT(pLine != nullptr);

    m_vpLines.remove(a_stIndex);
    navigables.remove(a_stIndex + 1);

    panel.scroller.remove_control(pLine);

    EMP_DEALLOCATE(pLine);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_text_cancel(text_c const* const, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    { emp::tt::silent(EMP_GUI_MANAGER->hide_window(window_text)); }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_text_valid(text_c const* const, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    { emp::tt::silent(EMP_GUI_MANAGER->hide_window(window_text)); }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_text_edit(text_c const* const, c_bool EMP_SILENT(a_bValue), c_change_e)
{ return EMP_GUI_RETURNE(return_e::Consumed); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_combo_cancel(combo_c const* const, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(window_combo));
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_combo_valid(combo_c const* const, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(window_combo));
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_combo_edit(
    combo_c const* const EMP_SILENT(a_pSender),
    c_bool EMP_SILENT(a_bValue),
    c_change_e EMP_SILENT(a_eChange))
{ return EMP_GUI_RETURNE(return_e::Consumed); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e list_c::on_edit_push(cpc_control_i EMP_SILENT(a_pSender), c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue == false)
    {
        EMP_RET_X_IF_NOT(return_e::Error, set_selection_double_click(m_ui32FocusLine, m_ui32FocusItem, a_eChange));
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return EMP_GUI_RETURNE(return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN class_c const* list_c::get_class(emp::rfx::type_i const* const a_pReflection)
{
    if (designer == nullptr)
    { return nullptr; }

    emp::rfx::type_s const& rType = *a_pReflection->id();
    c_astring& sName = rType.m_sName;
    emp::rfx::c_type_e eType = rType.m_eType;
    if (eType == emp::rfx::type_e::Pointer)
    {
        astring sType(sName);
        sType.trim_right();
        return designer->get(sType);
    }
    return designer->get(sName);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
list_c& list_c::skin_(cpc_list_skin_c a_pValue)
{
    skin = a_pValue;

    column.skin_(a_pValue ? a_pValue->header : nullptr);
    // @@0 spacer spacer.skin_(a_pValue ? a_pValue->spacer : nullptr);
    tbase1::skin_(a_pValue ? a_pValue->sizer : nullptr);
    panel.skin_(a_pValue ? a_pValue->panel : nullptr);
    c_size stCount = columns.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { columns.val(stIndex)->skin_(a_pValue ? a_pValue->column : nullptr); }
    // @@0 fixme skin reskin items
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

