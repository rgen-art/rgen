//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_property.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_manager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
property_c::property_c(void):
    fields(),
    m_vpLines(),
    stack(),
    line_0(100),
    line_1(100),
    expanded_lines(100),
    on_reflect()
{
    add_column_(emp::loc::string_s(ALITERAL("emp_gui_name")));
    add_column_(emp::loc::string_s(ALITERAL("emp_gui_value")));

    on_reflect = emp::fct::function(this, &property_c::reflect);

    used = &line_0;
    unused = &line_1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
property_c::~property_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool property_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(populate());
    return tbase1::resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool property_c::populate(void)
{
    auto* pCurrent = used;
    used = unused;
    unused = pCurrent;
    m_vpLines.resize(0);

    reflection = 0;
    if (root != nullptr)
    { EMP_ASSERT_BOOL_CALL(root->iterate(0, on_reflect)); }

    c_size stRootCount = fields.count();
    for (size_t stIndex = 0; stIndex < stRootCount; ++stIndex)
    {
        emp::rfx::field_s const& rField = fields.ref(stIndex);
        emp::rfx::type_i* const pRoot = rField.m_pType;
        EMP_ASSERT(pRoot != nullptr);

        class_c const* const pClass = get_class(pRoot);
        if (pClass != nullptr)
        { stack.add(pClass); }
        EMP_RET_FALSE_IF_NOT(reflect(nullptr, 1, rField));
        if (pClass != nullptr)
        { stack.remove_last(); }
    }

    auto it = unused->begin();
    c_bool bUnused = it.not_end();
    for (; it.not_end();)
    {
        line_c* const pLine = *it.value();
        if (pLine != nullptr)
        {
            expanded_lines.set(pLine->reflection, pLine->expanded());
            if (focus_in_hierarchy(pLine))
            { EMP_GUI_MANAGER->deactivate_focus(); }
            EMP_DEALLOCATE(pLine);
        }
        unused->remove(it);
        it = unused->begin();
    }
    EMP_ASSERT(unused->is_empty()); // @@0 asserting...

    if (changed || bUnused)
    {
        //EMP_LOG_INFO("CHANGED");
        empty_lines();
        c_size stUsedCount = m_vpLines.count();
        for (size_t stIndex = 0; stIndex < stUsedCount; ++stIndex)
        { add_line(m_vpLines.ref(stIndex)); }
    }

    changed = false;
    focused();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool property_c::reflect(emp::rfx::type_i* const a_pParent, c_size a_stDepth, emp::rfx::field_s const& a_rField)
{
    c_ui32 ui32Reflection = emp::ast::cast<ui32>(reflection);
    ++reflection;

    bool bExpanded = false;
    //bool bExpanded = m_bExpanded;
    bool bScope = true;
    visibility_e eVisibility = visibility_e::Reflect;

    emp::fct::function_t<bool (*)(void)> const* pFctVisible = nullptr;
    emp::fct::function_t<bool (*)(void)> const* pFctEnable = nullptr;

    emp::rfx::type_i* const pType = a_rField.m_pType;

    class_c const* pClass = get_class(pType);
    if (pClass != nullptr &&
        pClass->over)
    {
        //bExpanded = bExpanded && pClass->m_Field.m_bExpanded;
        eVisibility = pClass->field.visibility;
        pFctVisible = &pClass->field.on_visibility;
        pFctEnable = &pClass->field.on_enability;
        bScope = pClass->field.scope;
    }

    if (stack.count())
    {
        class_c const* const pParentClass = stack.last_val();
        if (pParentClass != nullptr)
        {
            astring const& sField = a_rField.m_sName;
            field_s const* const pField = pParentClass->get(sField);
            if (pField != nullptr)
            {
                //bExpanded = bExpanded && pField->m_bExpanded;
                eVisibility = pField->visibility;
                pFctVisible = &pField->on_visibility;
                pFctEnable = &pField->on_enability;
                bScope = pField->scope;
                if (pClass == nullptr)
                { pClass = pField->inner; }
            }
        }
    }

    if (eVisibility == visibility_e::Hidden ||
    (pFctVisible != nullptr && pFctVisible->is_valid() && ((*pFctVisible)() == false)))
    { return true; }

    emp::rfx::c_type_e eType = pType->id()->m_eType;
    c_bool bCollapsible =
        eVisibility == visibility_e::Reflect &&
        (eType == emp::rfx::type_e::Class ||
         eType == emp::rfx::type_e::Vector ||
         eType == emp::rfx::type_e::Pointer);

    line_c* pLine = nullptr;
    if (bScope)
    {
        auto const it = unused->get(pType);
        if (it.not_end())
        {
            pLine = *it.value();

            used->set(pType, pLine);
            unused->remove(it);

            m_vpLines.add(pLine);

            bExpanded = pLine->expanded();
        }
        else
        {
            pLine = new_line(ui32Reflection, a_rField, skin != nullptr ? skin->label : nullptr, emp::tt::max<ui32>::value, alignment_e::MiddleLeft);
            // @@0 ui32 max (casted)

            pLine->reflection_(pType).depth_(a_stDepth - 1).collapsible_(bCollapsible);
            add_item(a_pParent, pLine, a_rField, stack.count() > 0 ? stack.last_val() : nullptr);

            if (bCollapsible)
            {
                auto const itExpanded = expanded_lines.get(pType);
                if (itExpanded.not_end())
                { bExpanded = *itExpanded.value(); }
           }

            pLine->expanded_(bExpanded);
            used->set(pType, pLine);

            m_vpLines.add(pLine);

            changed = true;
        }
    }
    else
    {
        auto const it = unused->get(pType);
        if (it.not_end())
        { unused->remove(it); }
    }


    if (pLine != nullptr)
    {
        bool bEnable = true;
        if (pFctEnable != nullptr &&
            pFctEnable->is_valid())
        { bEnable = (*pFctEnable)(); }
        pLine->state().enable_(bEnable);
        //c_size stCount = pLine->item_count();
        /*for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            control_i& rControl = pLine->item(stIndex).control();
            rControl.state().set_enable(bEnable);
        }*/
    }
    if (bCollapsible && ((pLine == nullptr/* && bExpanded*/) || (pLine != nullptr && pLine->expanded()))) //-V728
    {
        stack.add(pClass);
        EMP_RET_FALSE_IF_NOT(pType->iterate(a_stDepth - (bScope ? 0 : 1), on_reflect));
        stack.remove_last();
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool property_c::set_object(emp::rfx::type_i* const a_pValue)
{
    root = a_pValue;
    fields.resize(0);

    used = &line_0;
    unused = &line_1;

    line_0.clear();
    line_1.clear();

    m_vpLines.resize(0);

    clear();

    return populate();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void property_c::add_object(emp::rfx::field_s const& a_rField)
{ fields.add(a_rField); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
property_c& property_c::skin_(cpc_list_skin_c a_pValue)
{
    list_c::skin_(a_pValue);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

