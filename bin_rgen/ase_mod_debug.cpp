//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_debug.h"

#include "ase_cor_game.h"

#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_reflection.rfx.h"

#include "emp_gui_button.h"
#include "emp_gui_glyph.h"
#include "emp_gui_property.h"
#include "emp_gui_nurbs.h"
#include "emp_gui_manager.h"
#include "emp_gui_mdi.h"
#include "emp_gui_outline.h"
#include "emp_gui_resolution.h"
#include "emp_gui_skin.h"
#include "emp_gui_splitter.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_reflection.rfx.h"
#include "ase_mod_reflection.rfx.h"

#include "emp_gfx_reflection.rfx.h"
#include "emp_loc_reflection.rfx.h"
#include "emp_mat_reflection.rfx.h"
#include "emp_snd_reflection.rfx.h"
#include "emp_ttf_reflection.rfx.h"

#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::rfx;
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, viewer_i)
private:
public:
    viewer_i(void) {}
    virtual ~viewer_i(void) = 0;

public:
    //>> viewer_i
    virtual EMP_RETURN emp::gui::control_i* control(void) = 0;
    //<<, viewer_i
};

inline viewer_i::~viewer_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,viewer_c, public, viewer_i)
private:
    emp::rfx::type_i* m_pReflection;
    emp::gui::property_c m_Control;
    emp::gui::button_c m_Button;

public:
    viewer_c(void):
        m_pReflection(),
        m_Control(),
        m_Button()
    { }

    ~viewer_c(void)
    { destroy(); }

public:
    EMP_RETURN bool create(emp::gui::registrar_i* const a_pRegistrar)
    {
        emp::gui::skin_c& rSkin = EMP_GUI_SKIN;

        m_Control
            .skin_(EMP_GUI_SKIN_(list, "background"))
            .registrar_(a_pRegistrar)
            .state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(m_Control.resize());

        m_Button
            .skin_(rSkin.button.get(ALITERAL("background.link")))
            .control_(&m_Control)
            .state().navigable_(true).autosize.xy(true);
        return true;
    }

    void destroy(void)
    {
        // @@0 create m_Control.destroy();
    }

public:
    //>> viewer_i
    EMP_RETURN emp::gui::control_i* control(void) { return &m_Button; }
    //<< viewer_i

public:
    EMP_RETURN bool set_reflection(emp::rfx::type_i* const a_pValue)
    {
        m_pReflection = a_pValue;
        EMP_RET_FALSE_IF_NOT(m_Control.set_object(m_pReflection));
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, viewer_t, public, viewer_i) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_NOCOPY_BASE(class, viewer_t<emp::mat::nurbs_c*>, public, viewer_i)
private:
    emp::mat::nurbs_c* m_pObject;
    emp::gui::button_c m_Button;
    emp::gui::resolution_c m_Resolution;
    emp::gui::nurbs_c m_Nurbs;

public:
    viewer_t(void):
        m_pObject(),
        m_Button(),
        m_Resolution(),
        m_Nurbs()
    {
    }

    ~viewer_t(void)
    { destroy(); }

public:
    EMP_RETURN bool create(void)
    {
        emp::gui::skin_c& rSkin = EMP_GUI_SKIN;

        m_Nurbs.skin_(rSkin.nurbs.get(ALITERAL("default"))).transformation().size_(0.3f, 0.3f);

        m_Button.skin_(rSkin.button.get(ALITERAL("background.link"))).control_(&m_Nurbs)
            .state().focusable_(true).navigable_(true);

        m_Resolution.set_control(&m_Button);
        m_Resolution.on_resolution_changed = emp::fct::function(this, &viewer_t::resolution_changed);
        return true;
    }

public:
    void resolution_changed(emp::gui::resolution_c const* const a_pSender)
    {
        EMP_ASSERT(a_pSender != nullptr);
        m_Nurbs.frame.set_origin(v2f::mul(m_Nurbs.frame.origin(), a_pSender->size_ratio()));
    }

    void destroy(void)
    {
        m_Nurbs.destroy();
        m_pObject = nullptr;
    }

public:
    //>> viewer_i
    EMP_RETURN emp::gui::control_i* control(void)
    { return &m_Resolution; }
    //<< viewer_i

public:
    EMP_RETURN bool set_reflection(emp::rfx::type_t<emp::mat::nurbs_c*>* const a_pValue)
    {
        m_pObject = a_pValue->link().value();
        m_Nurbs.nurbs_(m_pObject);
        EMP_RET_FALSE_IF_NOT(m_Nurbs.resize());
        EMP_RET_FALSE_IF_NOT(m_Nurbs.auto_resize());
        m_Nurbs.frame.center_origin();
        EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(m_pObject));
        m_Button.state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(m_Button.resize());
        m_Button.state().autosize.xy(false);
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_NOCOPY_BASE(class,viewer_t<emp::gfx::icon_c*>, public, viewer_i)
private:
    emp::gfx::icon_c* m_pObject;
    emp::gui::button_c m_Button;
    emp::gui::resolution_c m_Resolution;
    emp::gui::glyph_c m_Glyph;

public:
    viewer_t(void):
        m_pObject(),
        m_Button(),
        m_Resolution(),
        m_Glyph()
    {
    }

    ~viewer_t(void)
    { destroy(); }

public:
    EMP_RETURN bool create(void)
    {
        emp::gui::skin_c& rSkin = EMP_GUI_SKIN;

        m_Glyph.skin_(rSkin.glyph.get(ALITERAL("default"))).transformation().size_(0.3f, 0.3f);

        m_Resolution.set_control(&m_Glyph);
        m_Resolution.on_resolution_changed = emp::fct::function(this, &viewer_t::resolution_changed);

        m_Button.skin_(rSkin.button.get(ALITERAL("background.link"))).control_(&m_Resolution)
            .state().focusable_(true).navigable_(true);
        return true;
    }

public:
    void resolution_changed(emp::gui::resolution_c const* const a_pSender)
    {
        EMP_ASSERT(a_pSender != nullptr);
        m_Glyph.frame.set_origin(v2f::mul(m_Glyph.frame.origin(), a_pSender->size_ratio()));
    }

    void destroy(void)
    {
        m_Glyph.destroy();
        m_pObject = nullptr;
    }

public:
    //>> viewer_i
    EMP_RETURN emp::gui::control_i* control(void) { return &m_Button; }
    //<< viewer_i

public:
    EMP_RETURN bool set_reflection(emp::rfx::type_t<emp::gfx::icon_c*>* const a_pValue)
    {
        m_pObject = a_pValue->link().value();
        m_Glyph.gfx_icon_(m_pObject);
        EMP_RET_FALSE_IF_NOT(m_Glyph.resize());
        EMP_RET_FALSE_IF_NOT(m_Glyph.auto_resize());
        EMP_RET_FALSE_IF_NOT(m_Glyph.resize());
        EMP_RET_FALSE_IF_NOT(m_Glyph.auto_resize());

        c_size stCount = m_pObject->glyphs().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            emp::mat::glyph_s* const pGlyph = m_pObject->glyphs().val(stIndex);
            EMP_ASSERT(pGlyph != nullptr);
            EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(pGlyph));
        }
        EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(m_pObject->model()));
        m_Button.state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(m_Button.resize());
        m_Button.state().autosize.xy(false);
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_NOCOPY_BASE(class,viewer_t<emp::mat::outline_s*>, public, viewer_i)
private:
    emp::mat::outline_s* m_pObject;
    emp::gui::button_c m_Button;
    emp::gui::resolution_c m_Resolution;
    emp::gui::outline_c m_Outline;

public:
    viewer_t(void):
        m_pObject(),
        m_Button(),
        m_Resolution(),
        m_Outline()
    { }

    ~viewer_t(void)
    { destroy(); }

public:
    EMP_RETURN bool create(void)
    {
        emp::gui::skin_c& rSkin = EMP_GUI_SKIN;

        m_Outline.skin_(rSkin.outline.get(ALITERAL("default"))).transformation().size_(0.3f, 0.3f);

        m_Resolution.set_control(&m_Outline);
        m_Resolution.on_resolution_changed = emp::fct::function(this, &viewer_t::resolution_changed);

        m_Button.skin_(rSkin.button.get(ALITERAL("background.link"))).control_(&m_Resolution)
            .state().focusable_(true).navigable_(true);
        return true;
    }

public:
    void resolution_changed(emp::gui::resolution_c const* const a_pSender)
    {
        EMP_ASSERT(a_pSender != nullptr);
        m_Outline.frame.set_origin(v2f::mul(m_Outline.frame.origin(), a_pSender->size_ratio()));
    }

public:
    void destroy(void)
    {
        m_Outline.destroy();
        m_pObject = nullptr;
    }

public:
    //>> viewer_i
    EMP_RETURN emp::gui::control_i* control(void) { return &m_Button; }
    //<< viewer_i

public:
    EMP_RETURN bool set_reflection(emp::rfx::type_t<emp::mat::outline_s*>* const a_pValue)
    {
        m_pObject = a_pValue->link().value();
        m_Outline.outline_(m_pObject);
        EMP_RET_FALSE_IF_NOT(m_Outline.resize());
        EMP_RET_FALSE_IF_NOT(m_Outline.auto_resize());
        EMP_RET_FALSE_IF_NOT(m_Outline.resize());
        EMP_RET_FALSE_IF_NOT(m_Outline.auto_resize());
        EMP_RET_FALSE_IF_NOT(EMP_FACTORY.write_changes(m_pObject));
        m_Button.state().autosize.xy(true);
        EMP_RET_FALSE_IF_NOT(m_Button.resize());
        m_Button.state().autosize.xy(false);
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
debug_c::debug_c(void) :
m_pDebugSplitter(),
m_pDebugPanel(),
m_pDebugProperty(),
m_vpReflections(),
m_vpViewers()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
debug_c::~debug_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN emp::rfx::type_t<T>* new_reflection(emp::cnt::vector<emp::rfx::type_i*>& a_rvpReflections)
{
    emp::rfx::type_t<T>* const pReflection = EMP_ALLOCATE(emp::rfx::type_t<T>);
    EMP_ASSERT_BOOL_CALL(EMP_FACTORY.reflect_object(*pReflection));

    a_rvpReflections.add(pReflection);

    return pReflection;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
void add_reflection(emp::gui::property_c* const a_pView, emp::cnt::vector<emp::rfx::type_i*>& a_rvpReflections,
    emp::loc::string_s const& EMP_SILENT(a_sName));*/
template <typename T>
void add_reflection(emp::gui::property_c* const EMP_SILENT(a_pView), emp::cnt::vector<emp::rfx::type_i*>& EMP_SILENT(a_rvpReflections),
    emp::loc::string_s const& EMP_SILENT(a_sName))
{
    /*
    // @@0 device
    emp::rfx::type_t<emp::cnt::vector<T*>>* const pReflection = new_reflection<emp::cnt::vector<T*>>(a_rvpReflections);
    emp::rfx::store_t<T>* pStore = EMP_STORE(T);
    EMP_ASSERT(pStore);
    pReflection->link().set_value(&pStore->objects());
    a_pView->add_object(emp::rfx::field_s(pReflection, a_sName.key(), a_sName, a_sName));
    */
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool debug_c::public_create(void)
{
    m_pDebugProperty->skin_(EMP_GUI_SKIN_(list, "lst."))
        .registrar_(&registrar())
        .on_selection_double_click_(emp::fct::function(this, &debug_c::object_changed))
        .state().navigable_(true).index_(0).autosize.x_(true);

    EMP_RET_FALSE_IF_NOT(vector_changed());
    EMP_RET_FALSE_IF_NOT(m_pDebugProperty->populate());

    m_pDebugPanel->skin_(EMP_GUI_SKIN_(panel, "pnl."));
    //m_pDebugPanel->scroller().set_full(true);

    m_pDebugSplitter->skin_(EMP_GUI_SKIN_(splitter, ""))
        .master_(m_pDebugProperty).slave_(m_pDebugPanel).orientation_(emp::gui::dock_e::Left)
        .state().dock_(emp::gui::dock_e::Fill);

    window().add_control(m_pDebugSplitter);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void debug_c::public_destroy(void)
{
    // @@0 create m_pDebugProperty->destroy();

    emp::cnt::foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    emp::cnt::foreach(m_vpViewers, emp::mem::safe_delete_reset_t());
    m_vpViewers.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool debug_c::public_start(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void debug_c::public_stop(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool debug_c::public_update(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool debug_c::condition_satisfied(void) const
{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INTERN EMP_RETURN ase::gui::viewer_t<T>* get_viewer_t(emp::cnt::vector<ase::gui::viewer_i*>& a_rvpViewers,
    emp::rfx::type_i* const a_pReflection);
template <typename T>
EMP_INTERN EMP_RETURN ase::gui::viewer_t<T>* get_viewer_t(emp::cnt::vector<ase::gui::viewer_i*>& a_rvpViewers,
    emp::rfx::type_i* const a_pReflection)
{
    ase::gui::viewer_t<T>* const pViewer = EMP_ALLOCATE(ase::gui::viewer_t<T>);
    auto oFinally(emp::tt::finally([pViewer] { EMP_DEALLOCATE(pViewer); }));

    EMP_RET_NULL_IF_NOT(pViewer->create());
    EMP_RET_NULL_IF_NOT(pViewer->set_reflection(EMP_FACTORY.cast<T>(a_pReflection)));
    a_rvpViewers.add(pViewer);

    oFinally.release();
    return pViewer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN ase::gui::viewer_i* get_viewer_c(emp::cnt::vector<ase::gui::viewer_i*>& a_rvpViewers,
    emp::rfx::type_i* const a_pReflection, emp::gui::registrar_i* const a_pRegistrar);
EMP_INTERN EMP_RETURN ase::gui::viewer_i* get_viewer_c(emp::cnt::vector<ase::gui::viewer_i*>& a_rvpViewers,
    emp::rfx::type_i* const a_pReflection, emp::gui::registrar_i* const a_pRegistrar)
{
    ase::gui::viewer_c* const pViewer = EMP_ALLOCATE(ase::gui::viewer_c);
    auto oFinally(emp::tt::finally([pViewer] { EMP_DEALLOCATE(pViewer); }));

    EMP_RET_NULL_IF_NOT(pViewer->create(a_pRegistrar));
    EMP_RET_NULL_IF_NOT(pViewer->set_reflection(a_pReflection));
    a_rvpViewers.add(pViewer);

    oFinally.release();
    return pViewer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool debug_c::vector_changed(void)
{
    EMP_RET_FALSE_IF_NOT(m_pDebugProperty->set_object(nullptr));

    add_reflection<emp::snd::sound_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("sounds"), U8LITERAL("sounds")));
    add_reflection<emp::snd::sample_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("samples"), U8LITERAL("samples")));
    add_reflection<emp::snd::envelope_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("envelopes"), U8LITERAL("envelopes")));
    add_reflection<emp::snd::scale_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("scales"), U8LITERAL("scales")));
    add_reflection<emp::snd::quality_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("qualities"), U8LITERAL("qualities")));
    add_reflection<emp::mat::nurbs_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("nurbs"), U8LITERAL("nurbs")));
    add_reflection<ase::cor::mapping_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("mappings"), U8LITERAL("mappings")));
    add_reflection<emp::snd::layout_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("layouts"), U8LITERAL("layouts")));
    add_reflection<emp::loc::locale_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("locales"), U8LITERAL("locales")));
    add_reflection<emp::gfx::icon_c>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("glyphs"), U8LITERAL("glyphs")));
    add_reflection<emp::mat::outline_s>(m_pDebugProperty, m_vpReflections, emp::loc::string_s(ALITERAL("outlines"), U8LITERAL("outlines")));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool debug_c::object_changed(c_ui32 a_ui32Line, c_ui32 EMP_SILENT(a_ui32Item), emp::gui::c_change_e a_eChange)
{
    if (emp::tt::is_max(a_ui32Line) ||
        a_ui32Line == 0)
    { return true; }

    emp::gui::line_c const* const pLine = m_pDebugProperty->lines().val(a_ui32Line - 1);
    emp::rfx::type_i* const pReflection = pLine->reflection;

    emp::gui::control_i* pControl = nullptr;

    emp::rfx::factory_c& rFactory = EMP_FACTORY;
    if (pReflection->id() == rFactory.id<emp::mat::nurbs_c*>())
    {
        ase::gui::viewer_t<emp::mat::nurbs_c*>* const pViewer = get_viewer_t<emp::mat::nurbs_c*>(m_vpViewers, pReflection);
        pControl = pViewer->control();
    }
    else if (pReflection->id() == rFactory.id<emp::gfx::icon_c*>())
    {
        ase::gui::viewer_i* const pViewer = get_viewer_t<emp::gfx::icon_c*>(m_vpViewers, pReflection);
        pControl = pViewer->control();
    }
    else if (pReflection->id() == rFactory.id<emp::mat::outline_s*>())
    {
        ase::gui::viewer_t<emp::mat::outline_s*>* const pViewer = get_viewer_t<emp::mat::outline_s*>(m_vpViewers, pReflection);
        pControl = pViewer->control();
    }
    else
    {
        emp::rfx::c_type_e eType = pReflection->id()->m_eType;
        c_bool bCollapsible =
            eType == emp::rfx::type_e::Class ||
            eType == emp::rfx::type_e::Vector ||
            eType == emp::rfx::type_e::Pointer;

        if (bCollapsible)
        {
            ase::gui::viewer_i* const pViewer = get_viewer_c(m_vpViewers, pReflection, &registrar());
            pControl = pViewer->control();
        }
        else
        {
            //EMP_LOG_INFO("NO PROPERTIES");
        }
    }

    if (pControl != nullptr)
    {
        //auto const* const pSpacing = EMP_GUI_SKIN.spacing.get(ALITERAL("default"));
        pControl->transformation().position_(EMP_GUI_SKIN.spacing_x(), EMP_GUI_SKIN.spacing_y());
        c_size stCount = m_pDebugPanel->scroller.control_count();
        pControl->state().index_(emp::ast::cast<ui32>(stCount));
        m_pDebugPanel->scroller.add_control(pControl);
        EMP_GUI_MANAGER->activate_focus(pControl, emp::gui::focus_e::First, true, true, a_eChange);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template EMP_RETURN emp::rfx::type_t<ase::cor::option_shell_c>* new_reflection(
    emp::cnt::vector<emp::rfx::type_i*>& a_rvpReflections);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

