//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_LIST_H
#define EMP_GUI_LIST_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_docker.h"
#include "emp_gui_icon.h"
#include "emp_gui_panel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h" // @@0 include

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { template <typename...> class buffer_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class item_i;
template <typename> class item_t;

EMP_FORWARD(class, combo_c)
EMP_FORWARD(class, popup_c)

EMP_FORWARD(struct, line_skin_s)
EMP_FORWARD(class , list_skin_c)
EMP_FORWARD(struct, skin_data_s)
EMP_FORWARD(struct, slider_skin_s)

EMP_FORWARD(class, registrar_i)

EMP_FORWARD(class, text_c)
EMP_FORWARD(struct, text_skin_s)

EMP_FORWARD(struct, toggle_skin_s)

EMP_FORWARD(class, window_c)
EMP_FORWARD(struct, window_skin_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename S>
EMP_RETURN item_t<T>* new_item(S const* const a_pSkin, emp::rfx::pc_type_i a_pReflection, c_ui32 a_ui32Index)
{
    item_t<T>* pItem = EMP_ALLOCATE(item_t<T>);
    pItem->set_reflection(a_pReflection);

    T& rControl = pItem->control();
// @@0 create    rControl.create();
    rControl.skin_(a_pSkin).state().dock_(dock_e::Left).index_(a_ui32Index);
    return pItem;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef return_e(*PFnOnEdit)(c_control_i*, bool, c_change_e);
typedef emp::fct::function_t<PFnOnEdit> fct_on_edit_d;

typedef item_i* (*PFnNewItem)(emp::rfx::pc_type_i a_pValue, fct_on_edit_d const& a_rFctOnEdit, cpc_list_skin_c a_pSkin, c_size a_stCount);
typedef emp::fct::function_t<PFnNewItem> new_item_f;

typedef emp::cnt::map_t<emp::str::astring, new_item_f const*> editor_map_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name visibility_e
#define enum_values\
    enum_value( Reflect )\
    enum_value( Visible )\
    enum_value( Hidden  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class class_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(field_s)
public:
    typedef emp::fct::function_t<bool (*)(void)> on_enability_d;
    typedef emp::fct::function_t<bool (*)(void)> on_visibility_d;

public:
    EMP_FIELD( visibility_e         , visibility    , visibility_e::Reflect );
    EMP_FIELD( bool                 , expanded      , false                 );
    EMP_FIELD( bool                 , scope         , true                  );
    EMP_FIELD( emp::rfx::enum_i*    , enumeration   , nullptr               );
    EMP_FIELD( emp::str::formater_i*, formater      , nullptr               );
    EMP_FIELD( float                , min           , 0.0f                  );
    EMP_FIELD( float                , max           , 0.0f                  );
    EMP_FIELD( float                , step          , 0.0f                  );
    EMP_FIELD( on_enability_d       , on_enability  , {})
    EMP_FIELD( on_visibility_d      , on_visibility , {})
    EMP_FIELD( new_item_f           , on_new_item   , {})
    class_c* inner = nullptr;

public:
    ~field_s(void) = default;

public:
    field_s& range_(c_float a_fMin, c_float a_fMax, c_float a_fStep)
    { min = a_fMin; max = a_fMax; step = a_fStep; return *this; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, class_c)
public:
    EMP_FIELD(bool, over, false);
    field_s field;

private:
    map_t<astring, field_s*> fields;

public:
    class_c(c_size a_stSeed):
        field(),
        fields(a_stSeed)
    {}

    ~class_c(void)
    { destroy(); }

public:
    void copy(class_c const& a_rFrom);
    void destroy(void);

public:
    EMP_RETURN c_field_s* get(c_astring& a_sName) const
    {
        auto const it = fields.get(a_sName);
        if (it.not_end())
        { return *it.value(); }
        return nullptr;
    }

    EMP_RETURN field_s* get(c_astring& a_sName)
    {
        auto const it = fields.get(a_sName);
        if (it.not_end())
        { return *it.value(); }
        return nullptr;
    }

    EMP_RETURN field_s& push(c_astring& a_sName)
    {
        pc_field_s pField = get(a_sName);
        if (pField != nullptr)
        { return *pField; }
        return *allocate(a_sName);
    }

private:
    EMP_RETURN field_s* allocate(c_astring& a_sName);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, designer_c)
private:
    emp::cnt::map_t<astring, class_c*> classes;

public:
    designer_c(c_size a_stSeed):
        classes(a_stSeed)
    {}

    ~designer_c(void)
    { destroy(); }

public:
    void copy(designer_c const& a_rFrom);
    void destroy(void);

public:
    EMP_RETURN class_c const* get(c_astring a_sName) const
    {
        auto const it = classes.get(a_sName);
        if (it.not_end())
        { return *it.value(); }
        return nullptr;
    }

    EMP_RETURN class_c* get(c_astring& a_sName)
    {
        auto const it = classes.get(a_sName);
        if (it.not_end())
        { return *it.value(); }
        return nullptr;
    }

public:
    template <typename T>
    EMP_INLINE EMP_RETURN class_c& push(c_size a_stSeed, c_bool a_bOverride = true)
    { return push(EMP_FACTORY.id<T>()->m_sName, a_stSeed, a_bOverride); }

    template <typename T>
    EMP_RETURN class_c& get(void)
    { return *get(EMP_FACTORY.id<T>()->m_sName); }

    EMP_RETURN class_c& push_anonymous(c_size a_stSeed, c_bool a_bOverride)
    { return push(STR_AFORMAT("#0", classes.count()), a_stSeed, a_bOverride); }

private:
    EMP_RETURN class_c& push(c_astring& a_sName, c_size a_stSeed, c_bool a_bOverride)
    {
        class_c* const pClass = get(a_sName);
        if (pClass != nullptr)
        { return *pClass; }
        return *allocate(a_sName, a_stSeed, a_bOverride);
    }

private:
    EMP_RETURN class_c* allocate(c_astring& a_sName, c_size a_stSeed, c_bool a_bOverride);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, list_skin_c)
public:
    EMP_FIELD(c_window_skin_s*     , window       , nullptr)
    // @@0 spacer EMP_FIELD(c_container_skin_s*   , spacer       , nullptr)
    EMP_FIELD(c_sizer_skin_s*      , sizer        , nullptr)
    EMP_FIELD(c_line_skin_s*       , header       , nullptr)
    EMP_FIELD(c_color_s*           , header_color , nullptr)
    EMP_FIELD(c_label_skin_s*      , column       , nullptr)
    EMP_FIELD(c_label_skin_s*      , label        , nullptr)
    EMP_FIELD(c_icon_skin_s*       , icon         , nullptr)
    EMP_FIELD(c_toggle_skin_s*     , toggle       , nullptr)
    EMP_FIELD(c_panel_skin_s*      , panel        , nullptr)
    EMP_FIELD(c_text_skin_s*       , text         , nullptr)
    EMP_FIELD(c_list_skin_c*       , combo        , nullptr)
    EMP_FIELD(c_slider_skin_s*     , slider       , nullptr)
    EMP_FIELD(emp::gfx::c_model_c* , gfx_edit     , nullptr)

    vector<c_line_skin_s*> lines{};

    list_skin_c(void) {}

    EMP_RETURN c_line_skin_s* line(c_size a_stIndex) const { return lines.val(a_stIndex % lines.count()); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(line_skin_s)
public:
    EMP_FIELD(c_container_skin_s*       , container     , nullptr)
    EMP_FIELD(c_picture_skin_s*         , collapser     , nullptr)
    EMP_FIELD(emp::gfx::model_c const*  , gfx_collapsed , nullptr)
    EMP_FIELD(emp::gfx::model_c const*  , gfx_expanded  , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, line_c, public, container_c)
private:
    friend class list_c;

public:
    EMP_FIELD(size_t, depth, 0);
    EMP_FIELD(emp::rfx::type_i*, reflection, nullptr);

private:
    bool m_bExpanded = false;
    picture_c* m_pCollapser = nullptr;
protected:
    c_line_skin_s* skin = nullptr;

    emp::cnt::vector<item_i*> items;

public:
    line_c(void);
    virtual ~line_c(void);

public:
    //> control_i
    EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange)
    {
        pc_control_i pFocus = tbase1::got_focus(a_eFocus, a_bSelect, a_bPush, a_eChange);
        if (pFocus != nullptr)
        { return pFocus; }
        return (emp::tt::is_not_max(state().index)) ? this : nullptr;
    }
    //< control_i

public:
    void border_(c_bool a_bLeft, c_bool a_bTop, c_bool a_bRight, c_bool a_bBottom)
    {
        auto& rBorder = color().background.border;
        rBorder.border = border_e::none;
        if (a_bLeft)
        { rBorder.add_border(border_e::Left); }
        if (a_bTop)
        { rBorder.add_border(border_e::Top); }
        if (a_bRight)
        { rBorder.add_border(border_e::Right); }
        if (a_bBottom)
        { rBorder.add_border(border_e::Bottom); }
    }

public:
    void add_item(item_i* const a_pValue);

public:
    EMP_RETURN size_t reflection_count(void) const;

    line_c& expanded_(c_bool a_bValue);
    EMP_RETURN bool expanded(void) const { return m_pCollapser == nullptr || m_bExpanded; }

    line_c& collapsible_(c_bool a_bValue);
    EMP_RETURN bool collapsible(void) const { return m_pCollapser != nullptr; }

    EMP_RETURN return_e on_collapse(cpc_control_i a_pButton, c_bool a_bValue, c_change_e a_eChange);

    EMP_RETURN picture_c& collapser(void) { return *m_pCollapser; }
    EMP_RETURN picture_c const& collapser(void) const { return *m_pCollapser; }

private:
    void set_selection(c_size a_stItem, c_bool a_bFocus, c_change_e a_eChange);

public:
    line_c& skin_(cpc_line_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, item_i)
public:
    item_i(void) {}
    virtual ~item_i(void) = 0;

public:
    //>> item_i
    virtual EMP_RETURN control_i& control(void) = 0;
    virtual EMP_RETURN c_control_i& control(void) const = 0;
    virtual EMP_RETURN emp::rfx::type_i* reflection(void) = 0;
    virtual EMP_RETURN autosize_data_s& autosize(void) = 0;
    //<< item_i
};
inline item_i::~item_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, item_t, public, item_i)
private:
    T m_Control;
    emp::rfx::type_i* m_pReflection;

public:
    item_t(void) :
        m_Control(),
        m_pReflection(nullptr)
    {}

    ~item_t(void)
    {}

public:
    //>> item_i
    EMP_RETURN T const& control(void) const { return m_Control; }
    EMP_RETURN T& control(void) { return m_Control; }

    EMP_RETURN emp::rfx::type_i* reflection(void) { return m_pReflection; }
    EMP_RETURN autosize_data_s& autosize(void) { return m_Control.state().autosize; }
    //<< item_i

public:
    void set_reflection(emp::rfx::pc_type_i a_pValue) { m_pReflection = a_pValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN auto const& get_name(T const& a_rValue)
{ return a_rValue.name(); }

EMP_RETURN c_astring& get_name(c_skin_data_s& a_rValue);

template <typename T>
EMP_NOCOPY_BASE(class, enum_t, public, emp::rfx::enum_i)
private:
    emp::rfx::type_t<T*>* m_pReflection;
    emp::rfx::store_t<T>* m_pStore;

    mutable emp::cnt::vector<emp::loc::string_s*> m_vpStrings;

public:
    enum_t(void):
        m_pReflection(nullptr),
        m_pStore(nullptr),
        m_vpStrings()
    {}

    ~enum_t(void)
    { emp::cnt::foreach(m_vpStrings, emp::mem::safe_delete_reset_t()); }

public:
    // >> type_i
    EMP_RETURN emp::rfx::type_s const* id(void) const { return nullptr; }
    EMP_RETURN bool bind(void) { return false; }
    EMP_RETURN bool read_object(xml::reader_c&, c_size, c_astring&) { return false; }
    EMP_RETURN bool read_attribute(xml::reader_c&) { return false; }
    EMP_RETURN bool eat_object(astring&) { return false; }
    EMP_RETURN bool post_load(void) { return false; }
    EMP_RETURN bool from_string(c_astring&) { return false; }
    EMP_RETURN bool from_string(c_u8string&) { return false; }
    EMP_RETURN bool to_string(str::formater_i* const, u8string&) const { return false; }
    EMP_RETURN size_t field_count(void) const { return 0; }
    EMP_RETURN bool settable(void) const { return false; }
    EMP_RETURN bool bindable(void) const { return false; }
    EMP_RETURN bool iterate(c_size, emp::rfx::reflect_f const&) { return false; }
    EMP_RETURN bool reflect(emp::rfx::factory_c&) { return false; }
    EMP_RETURN bool write(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }
    EMP_RETURN bool write_attribute(xml::writer_c& EMP_SILENT(a_rWriter), c_astring& EMP_SILENT(a_sName)) { return false; }
    EMP_RETURN bool write_object(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }
    EMP_RETURN bool write_element(xml::writer_c& EMP_SILENT(a_rWriter)) { return false; }
    EMP_RETURN bool is_attribute(void) const { return false; }
    EMP_RETURN bool is_writable(void) const { return false; }
    // << type_i

    //>> enum_i
    EMP_RETURN size_t enum_value_count(void) const
    { return EMP_STORE(T)->object_count(); }

    EMP_RETURN emp::loc::string_s const& enum_to_string(c_size a_stValue) const
    {
        if (m_vpStrings.is_valid_index(a_stValue))
        { return *m_vpStrings.val(a_stValue); }

        T* const pValue = EMP_STORE(T)->get_object(a_stValue);
        EMP_ASSERT(pValue != nullptr);

        // @@0 old : emp::loc::string_s* pString = EMP_ALLOCATE(emp::loc::string_s, emp::loc::string_s(pValue->name(), pValue->name()));
        emp::loc::string_s* pString = EMP_ALLOCATE(emp::loc::string_s, emp::loc::string_s(astring(), u8string(get_name(*pValue))));

        if (a_stValue >= m_vpStrings.count())
        { m_vpStrings.resize(a_stValue + 1); }

        m_vpStrings.set(a_stValue, pString);

        return *pString;
    }

    void enum_from_index(c_size a_stValue)
    {
        T* const pValue = EMP_STORE(T)->get_object(a_stValue);
        EMP_ASSERT(pValue != nullptr);

        if (m_pReflection->bindable())
        { m_pReflection->link().value() = pValue; }
        else if (m_pReflection->settable())
        { EMP_ASSERT_BOOL_CALL(m_pReflection->link().setter()(pValue)); }
    }

    EMP_RETURN size_t enum_current_index(void) const
    {
        c_size stCount = enum_value_count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            T const* const pValue = EMP_STORE(T)->get_object(stIndex);
            if (m_pReflection->bindable())
            {
                if (m_pReflection->link().value() == pValue)
                { return stIndex; }
            }
            else
            {
                T* pGetValue = nullptr;
                if (m_pReflection->link().getter()(pGetValue))
                {
                    if (pGetValue == pValue)
                    { return stIndex; }
                }
            }
        }
        return emp::tt::max<size_t>::value;
    }

    //<< enum_i

    EMP_RETURN item_i* new_item(
        emp::rfx::pc_type_i a_pReflection,
        fct_on_edit_d const& a_rFctOn,
        cpc_list_skin_c a_pSkin,
        c_size a_stCount)
    {
        EMP_ASSERT(a_pReflection != nullptr);
        EMP_ASSERT(a_pReflection->id()->m_eType == emp::rfx::type_e::Pointer);
        // @@0 assert name
        //EMP_ASSERT(emp::str::cmp(emp::rfx::type_t<T*>::name(), a_pReflection->s_type()));

        m_pReflection = static_cast<emp::rfx::type_t<T*>*>(a_pReflection);
        m_pStore = EMP_STORE(T);

        item_t<icon_c>* const pIcon = emp::gui::new_item<emp::gui::icon_c>(a_pSkin != nullptr ? a_pSkin->icon : nullptr, a_pReflection, emp::ast::cast<ui32>(a_stCount));
        pIcon->control().label.set_text(a_pReflection);
        pIcon->control().picture_(a_pSkin != nullptr ? a_pSkin->gfx_edit : nullptr);
        pIcon->control().splitter.orientation_(dock_e::Right);
        pIcon->control().picture.on_push.function_(a_rFctOn);
        pIcon->control().state().focusable_(true);
        return pIcon;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, list_c, public, docker_c)
public:
    typedef emp::fct::function_t<bool (*)(c_ui32, c_ui32, c_change_e)> on_selection_double_click_d;
    typedef emp::fct::function_t<bool (*)(c_ui32, c_ui32, c_change_e)> on_selection_changed_d;

    typedef emp::fct::function_t<return_e (*)(c_ui32, c_ui32, c_change_e)> on_item_click_d;
    typedef emp::fct::function_t<return_e (*)(c_ui32, c_ui32, c_change_e)> on_item_double_click_d;

public:
    EMP_FIELD(on_selection_changed_d        , on_selection_changed      );
    EMP_FIELD(on_selection_double_click_d   , on_selection_double_click );
    EMP_FIELD(on_item_click_d               , on_item_click             );
    EMP_FIELD(on_item_double_click_d        , on_item_double_click      );

    EMP_FIELD(registrar_i*                  , registrar         , nullptr);
    EMP_FIELD(size_t                        , autosize_count    , 0      );
    EMP_FIELD(bool                          , expand_last_column, false  );
    EMP_FIELD(popup_c*                      , popup             , nullptr);
    EMP_FIELD(designer_c const*             , designer          , nullptr);

    EMP_FIELD(bool, show_columns , false);
    EMP_FIELD(bool, wrap_lines   , false);

protected:
    c_list_skin_c* skin = nullptr;

private:
    text_c* text = nullptr;
    combo_c* combo = nullptr;

    line_c* current_line = nullptr;

    window_c* window_text = nullptr;
    window_c* window_combo = nullptr;

    bool enter = false;

    ui32 EMP_TT_MAX_VAR(m_ui32EditLine);
    ui32 EMP_TT_MAX_VAR(m_ui32EditItem);

    ui32 EMP_TT_MAX_VAR(m_ui32SelectionLine);
    ui32 EMP_TT_MAX_VAR(m_ui32SelectionItem);

    ui32 EMP_TT_MAX_VAR(m_ui32FocusLine);
    ui32 EMP_TT_MAX_VAR(m_ui32FocusItem);

    vector<line_c*> m_vpLines;
    vector<line_c*> navigables;
    vector<label_c*> columns;

    line_c column;
    spacer_c spacer;
    panel_c panel;

    emp::rfx::reflect_f on_reflect;
    fct_on_edit_d on_edit;

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pHeaderMeshVertices;

public:
    list_c(void);
    virtual ~list_c(void);

public:
    //> control_i
    EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange);
    void focused(void);
    EMP_RETURN bool unselect(c_bool a_bTransmit);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //< control_i

    //> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //< handler_i

    list_c& skin_(cpc_list_skin_c a_pValue);

private:
    EMP_RETURN return_e process_left_click(c_size a_stClick, c_change_e a_eChange);

public:
    void clear(void);

    void empty_lines(void);

    EMP_RETURN emp::cnt::vector<line_c*> const& lines(void) const { return m_vpLines; }

public:
    EMP_RETURN ui32 selection_line(void) const { return m_ui32SelectionLine; }
    EMP_RETURN ui32 selection_item(void) const { return m_ui32SelectionItem; }

    void set_check(c_ui32 a_ui32Line, c_bool a_bValue);
    void set_selection(c_ui32 a_stLine, c_ui32 a_stItem, c_bool a_bFocus, c_change_e a_eChange);
    EMP_RETURN bool set_selection_double_click(c_ui32 a_stLine, c_ui32 a_stItem, c_change_e a_eChange);

public:
    EMP_RETURN line_c* new_line(c_ui32 a_ui32Index);
    void insert_line(c_ui32 a_ui32Index, line_c* const a_pLine);

    EMP_RETURN line_c* new_line(c_ui32 a_ui32Index, emp::rfx::field_s const& a_rField, cpc_label_skin_s a_pSkin, c_ui32 a_stItemIndex, c_alignment_e a_eAlignment = alignment_e::MiddleCenter);

    void insert_line(c_ui32 a_ui32Index, emp::rfx::field_s const& a_rField, cpc_label_skin_s a_pSkin, c_ui32 a_ui32ItemIndex);

    void add_line(c_u8string& a_sValue);
    void add_line(emp::loc::string_s const& a_eValue);
    void add_line(emp::rfx::type_i* const a_eValue);
    void add_line(line_c* const a_pValue);

    void remove_line(c_size a_stIndex);

    list_c& add_column_(emp::loc::string_s const& a_eLabel);

    void add_item(emp::rfx::type_i* const a_pParent, line_c* const a_pLine, emp::rfx::field_s const& a_rField, emp::gui::class_c const* const a_pClass);
    void add_item(line_c* const a_pLine, c_u8string& a_sValue, c_size a_stIndex);

public:
    EMP_RETURN bool reflect(emp::rfx::type_i* const a_pParent, c_size a_stDepth, emp::rfx::field_s const& a_rField);

public:
    EMP_RETURN return_e on_edit_push(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);

private:
    EMP_RETURN line_c  * insert_new_line(c_ui32 a_ui32Index);
    EMP_RETURN window_c* get_or_alloc_text_window(void);
    EMP_RETURN window_c* get_or_alloc_combo_window(void);
    EMP_RETURN combo_c * get_or_alloc_combo(void);

private:
    EMP_RETURN return_e on_text_cancel(text_c const* const a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_text_valid( text_c const* const a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_text_edit(  text_c const* const a_pSender, c_bool a_bValue, c_change_e a_eChange);

    EMP_RETURN return_e on_combo_cancel(combo_c const* const a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_combo_valid( combo_c const* const a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_combo_edit(  combo_c const* const a_pSender, c_bool a_bValue, c_change_e a_eChange);

public:
    EMP_RETURN class_c const* get_class(emp::rfx::type_i const* const a_pReflection);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

