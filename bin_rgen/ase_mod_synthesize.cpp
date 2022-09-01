//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_synthesize.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_emphase.h"
#include "ase_cor_game.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_graph.h"
#include "emp_gui_list.h"
#include "emp_gui_popup.h"
#include "emp_gui_skin.h"
#include "emp_gui_splitter.h"
#include "emp_gui_window.h"

#include "emp_snd_soft_synth.h"
#include "emp_snd_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, synthesize_c)
public:
    splitter_c splitter{};

private:
    list_c list;
    popup_c popup;
    graph_c graph;

    emp::cnt::vector<emp::snd::soft_synth_c*> m_vpSynths;
    emp::cnt::vector<emp::rfx::type_i*> m_vpReflections;

public:
    synthesize_c(void);
    ~synthesize_c(void);

public:
    EMP_RETURN bool create(registrar_i* const a_pRegistrar);
    void destroy(void);

public:
    void add_synth(emp::snd::soft_synth_c* const a_pSynth);
    void populate_synthesizer_list(c_ui32 a_ui32Selection, c_change_e a_eChange);

    EMP_RETURN bool set_value(c_size a_stIndex, c_astring& a_rsValue);
    EMP_RETURN bool get_value(c_size a_stIndex, astring& a_rsValue);

public:
    EMP_RETURN bool synthesizer_changed(c_ui32 a_ui32Line, c_ui32 a_ui32Item, c_change_e a_eChange);

public:
    void set_current_frequency(c_float a_fValue);

public:
    EMP_RETURN return_e on_new_synth(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_remove_synth(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_rename_synth(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
synthesize_c::synthesize_c():
    list(),
    popup(),
    graph(),
    m_vpSynths(),
    m_vpReflections()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
synthesize_c::~synthesize_c(void)
{ destroy();}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool synthesize_c::create(registrar_i* const a_pRegistrar)
{
    destroy();

    popup.create(a_pRegistrar, &list)
        .skin_(EMP_GUI_SKIN_(toolbar, "tbr."))
        .add_item(ALITERAL("ase::gui::synthesize_c::on_new_synth"), fct::function(this, &synthesize_c::on_new_synth))
        .add_item(ALITERAL("ase::gui::synthesize_c::on_rename_synth"), fct::function(this, &synthesize_c::on_rename_synth))
        .add_item(ALITERAL("ase::gui::synthesize_c::on_remove_synth"), fct::function(this, &synthesize_c::on_remove_synth));

    list.skin_(EMP_GUI_SKIN_(list, "lst."))
        .popup_(&popup)
        .registrar_(a_pRegistrar)
        .add_column_(emp::loc::string_s(ALITERAL(".synthesizers."), U8LITERAL(".synthesizers.")))
        .on_selection_changed_(fct::function(this, &synthesize_c::synthesizer_changed))
        .state().navigable_(true).index_(0).autosize.x_(true);
    EMP_RET_FALSE_IF_NOT(list.resize());

    graph.create(a_pRegistrar);
    graph.set_skin(EMP_GUI_SKIN_(graph, "grp."));
    graph.state().index_(1);

    splitter.skin_(EMP_GUI_SKIN_(splitter, ""))
        .master_(&list).slave_(&graph).orientation_(dock_e::Left)
        .state().index_(0).dock_(dock_e::Fill);

    splitter.redock();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void synthesize_c::destroy(void)
{
    // @@0 create list->destroy();
    popup.destroy();
    graph.destroy();

    m_vpSynths.resize(0);
    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool synthesize_c::set_value(c_size a_stIndex, c_astring& a_rsValue)
{
    snd::soft_synth_c* const pSynth = m_vpSynths.val(a_stIndex);
    pSynth->set_name(a_rsValue);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool synthesize_c::get_value(c_size a_stIndex, astring& a_rsValue)
{
    emp::snd::soft_synth_c const* const pSynth = m_vpSynths.val(a_stIndex);
    a_rsValue = pSynth->name();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void synthesize_c::add_synth(emp::snd::soft_synth_c* const a_pSynth)
{
    c_ui32 ui32Index = emp::ast::cast<ui32>(m_vpSynths.count());
    m_vpSynths.add(a_pSynth);

    rfx::type_t<astring>* const pType = EMP_ALLOCATE(rfx::type_t<astring>,
        bind(fct::function(this, &synthesize_c::get_value), ui32Index),
        bind(fct::function(this, &synthesize_c::set_value), ui32Index));

    m_vpReflections.add(pType);

    line_c* const pLine = list.new_line(ui32Index);
    list.add_item(nullptr, pLine, emp::rfx::field_s(pType), nullptr);
    list.insert_line(ui32Index, pLine);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void synthesize_c::populate_synthesizer_list(c_ui32 a_ui32Selection, c_change_e a_eChange)
{
    m_vpSynths.resize(0);
    list.clear();
    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    auto* const pStore = EMP_STORE(snd::soft_synth_c);
    c_ui32 ui32Count = emp::ast::cast<ui32>(pStore->object_count());
    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    { add_synth(pStore->get_object(ui32Index)); }

    if (a_ui32Selection < ui32Count)
    { list.set_selection(a_ui32Selection + 1, 0, true, a_eChange); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool synthesize_c::synthesizer_changed(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), c_change_e EMP_SILENT(a_eChange))
{
    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    c_ui32 ui32Index = a_ui32Line - 1;

    snd::soft_synth_c* const pSynth = m_vpSynths.val(ui32Index);

    graph.create(graph.registrar());
    graph.set_skin(EMP_GUI_SKIN.graph.get(ALITERAL("grp.")));
    graph.state().index_(1);
    splitter.slave_(&graph).redock();
    graph.set_synth(pSynth);
    EMP_RET_FALSE_IF_NOT(graph.resize());
    EMP_RET_FALSE_IF_NOT(splitter.resize());
    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(pSynth));
    ASE_GAME.set_layout(pSynth);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void synthesize_c::set_current_frequency(float const a_fValue)
{ graph.set_current_frequency(a_fValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e synthesize_c::on_new_synth(c_size, c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue)
    {
        popup.hide();

        auto& rFactory = EMP_FACTORY;
        snd::soft_synth_c* const pSynth = rFactory.store<snd::soft_synth_c>()->new_object_pattern(ALITERAL("emp_snd_soft_synth"));
        snd::quality_c* pQuality = rFactory.store<snd::quality_c>()->get_object(ALITERAL("emp_snd_quality_00.xml"));

        pSynth->set_name(ALITERAL("__no_name__"));
        pSynth->set_quality(pQuality);
        EMP_ASSERT_BOOL_CALL(pSynth->post_load());
        add_synth(pSynth);
        list.set_selection(emp::ast::cast<ui32>(m_vpSynths.count()), 0, true, a_eChange);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e synthesize_c::on_remove_synth(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        popup.hide();
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e synthesize_c::on_rename_synth(c_size, c_bool a_bValue, c_change_e a_eChange)
{
    if (a_bValue)
    {
        popup.hide();
        emp::tt::silent(list.on_edit_push(nullptr, false, a_eChange));
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
synthesize_c::synthesize_c() :
m_pCtrlSynthesize()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
synthesize_c::~synthesize_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool synthesize_c::public_create(void)
{
    EMP_RET_FALSE_IF_NOT(m_pCtrlSynthesize->create(&registrar()));
    m_pCtrlSynthesize->populate_synthesizer_list(0, emp::gui::change_e::Code);

    window().add_control(&m_pCtrlSynthesize->splitter);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void synthesize_c::public_destroy(void)
{ m_pCtrlSynthesize->destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool synthesize_c::public_update(void)
{
    m_pCtrlSynthesize->set_current_frequency(ASE_GAME.emphase().current_frequency());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

