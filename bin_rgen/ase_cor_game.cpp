//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_cor_game.h"

#include "ase_cor_emphase.h"

#include "ase_mod_emphase.h"
#include "ase_mod_selection.h"
#include "ase_mod_evolution.h"

#include "ase_mod_improvize.h"
#include "ase_mod_music.h"
#include "ase_mod_mode_i.h"

#include "ase_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"

#include "emp_ose_renderer.h"
#include "emp_ose_compiler.h"
#include "emp_ose_processor.h"

#include "emp_gui_dialog.h"
#include "emp_gui_manager.h"
#include "emp_gui_menu.h"
#include "emp_gui_property.h"
#include "emp_gui_registrar.h"
#include "emp_gui_skin.h"
#include "emp_gui_splitter.h"
#include "emp_gui_toolbar.h"
#include "emp_gui_window.h"

#include "emp_snd_track.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_reflection.rfx.h"
#include "emp_app_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::tt;
using namespace emp::fct;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
game_c* game_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
game_c::game_c(void):
m_vpMods(),
m_vpDebugMods(),
m_pvpMods(),
m_pDlgExit(),
m_pCtrlExitLabel(),
m_pRegistrar(),
m_pMenu(),
m_pProperties(),
m_pDesigner(5u),
m_pOptions(),
m_pTrack(),
m_pEmphase(),
m_pSkin(),
m_pMatrix(),
m_pRenderer(),
m_pEnumLocales(),
m_pEnumUsers(),
m_pEnumMappings(),
m_pEnumLayouts(),
m_pEnumScales(),
m_pEnumSkins()
/*,
m_mpMods(8)*/
{

/*    ase::mod::text_c oText;
    ase::mod::selection_c oSelection;
    ase::mod::evolution_c oEvolution;
    ase::mod::emphase_c oEmphase;
    ase::mod::improvize_c oImprovize;
    ase::mod::music_c oMusic;
    ase::mod::compose_c oCompose;
    ase::mod::synthesize_c oSynthesize;
    ase::mod::debug_c oDebug;*/

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
game_c::~game_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool game_c::post_load(void)
{
    using namespace emp::loc;
    using namespace emp::tt;
    using namespace emp::gui;

    m_pvpMods = &m_vpMods;

    m_pRegistrar->start();

    m_pSkin->create();
    m_pMatrix->destroy();

    set_user(m_pUser);
    EMP_RET_FALSE_IF(m_pUser == nullptr);
    EMP_RET_FALSE_IF(m_pUser->init_mod == nullptr);
    set_next_mod(m_pUser->init_mod);

    EMP_RET_FALSE_IF_NOT(m_pRenderer->create());

    m_pReflection = ase::mod::new_reflection<option_shell_c>(m_vpReflections);
    m_pReflection->link().set_value(m_pOptions);

    {
        class_c& rClass = m_pDesigner->push<user_c>(2u, true);
        rClass.field
            .expanded_(false)
            .enumeration_(m_pEnumUsers)
            .on_new_item_(function(&*m_pEnumUsers, &enum_t<user_c>::new_item));

        rClass.push(ALITERAL("scaling" )).visibility_(visibility_e::Hidden).range_(0.1f, 4.0f, 0.1f);
        rClass.push(ALITERAL("name"    )).visibility_(visibility_e::Hidden);
        rClass.push(ALITERAL("init_mod")).visibility_(visibility_e::Hidden);
        rClass.push(ALITERAL("options" )).expanded_(true).scope_(false);
    }

    m_pDesigner->push<ase::cor::pool_list_c>(2u, true).field.scope_(false);

    {
        class_c& rClass = m_pDesigner->push<option_s>(3u, true);
        rClass.field.scope_(false).expanded_(true);
        rClass.push(ALITERAL("skin"     )).visibility_(visibility_e::Visible);
        rClass.push(ALITERAL("locale"   )).visibility_(visibility_e::Visible);
        rClass.push(ALITERAL("scale"    )).visibility_(visibility_e::Visible);
        rClass.push(ALITERAL("evolution")).on_visibility_(function(&*m_pOptions, &option_shell_c::evolution_visible));
        rClass.push(ALITERAL("selection"  )).on_visibility_(function(&*m_pOptions, &option_shell_c::selection_visible));
        rClass.push(ALITERAL("emphase"  )).on_visibility_(function(&*m_pOptions, &option_shell_c::emphase_visible));
    }

    {

        class_c& rClass = m_pDesigner->push<emp::ose::mutation_s>(4u, true);
        rClass.field.scope_(false).expanded_(true);

        auto const l_design_node_mutation = [&rClass](c_astring& a_sName, emp::gui::field_s::on_enability_d const& a_rOnEnability,
            c_visibility_e a_eVisibility)
        {
            auto& rField = rClass.push(a_sName);
            rField.inner = EMP_ALLOCATE(class_c, 4u);

            rField.visibility = a_eVisibility;
            rField.inner->field.visibility_(a_eVisibility);
            rField.inner->push(ALITERAL("count" )).range_(1, 64, 1).on_enability_(a_rOnEnability);
            rField.inner->push(ALITERAL("base"  )).range_(0, 64, 1).on_enability_(a_rOnEnability);
            rField.inner->push(ALITERAL("scale" )).range_(0, 8, 0.01f).on_enability_(a_rOnEnability);
            rField.inner->push(ALITERAL("offset")).range_(0, 8, 0.01f).on_enability_(a_rOnEnability);
        };

        l_design_node_mutation(ALITERAL("literal") , function(&*m_pOptions, &option_shell_c::literal_mutation_enabled) , visibility_e::Reflect);
        l_design_node_mutation(ALITERAL("variant") , function(&*m_pOptions, &option_shell_c::variant_mutation_enabled) , visibility_e::Reflect);
        l_design_node_mutation(ALITERAL("call")    , function(&*m_pOptions, &option_shell_c::call_mutation_enabled)    , visibility_e::Reflect);
        l_design_node_mutation(ALITERAL("data")    , function(&*m_pOptions, &option_shell_c::data_mutation_enabled)    , visibility_e::Hidden);
        l_design_node_mutation(ALITERAL("trailing"), function(&*m_pOptions, &option_shell_c::trailing_mutation_enabled), visibility_e::Reflect);
    }

    {
        class_c& rClass = m_pDesigner->push<emp::ose::crossover_s>(4u, true);
        auto const oEnability = function(&*m_pOptions, &option_shell_c::crossover_enabled);
        rClass.push(ALITERAL("xfather" )).on_enability_(oEnability);
        rClass.push(ALITERAL("count"   )).on_enability_(oEnability).range_(1, 64, 1);
        rClass.push(ALITERAL("base"    )).on_enability_(oEnability).range_(0, 64, 1);
        rClass.push(ALITERAL("offset"  )).on_enability_(oEnability).range_(0, 64, 1);
    }

    {
        class_c& rClass = m_pDesigner->push<score_option_s>(4u, true);
        rClass.field.expanded_(false);
        auto const oEnability = function(&*m_pOptions, &option_shell_c::score_visible);
        rClass.push(ALITERAL("letters"   )).on_enability_(oEnability);
        rClass.push(ALITERAL("durations" )).on_enability_(oEnability);
        rClass.push(ALITERAL("scale"     )).on_enability_(oEnability).range_(0.5f, 2.0f, 0.01f);
        rClass.push(ALITERAL("time_scale")).on_enability_(oEnability).range_(0.1f, 1.0f, 0.01f);
    }

    {
        class_c& rClass = m_pDesigner->push<keyboard_option_s>(4u, true);
        rClass.field.expanded_(false);
        auto const oEnability = function(&*m_pOptions, &option_shell_c::keyboard_visible);
        rClass.push(ALITERAL("letters")).on_enability_(oEnability);
        rClass.push(ALITERAL("scale"  )).on_enability_(oEnability).range_(0.5f, 2.0f, 0.01f);
    }

    {
        class_c& rClass = m_pDesigner->push<selection_option_s>(4u, true);
        rClass.field.expanded_(false);
        rClass.push(ALITERAL("gallery_width"  )).range_(1, 16, 1);
    }

    {
        class_c& rClass = m_pDesigner->push<evolution_option_s>(4u, true);
        rClass.field.expanded_(false);
        rClass.push(ALITERAL("left"    )).visibility_(emp::gui::visibility_e::Hidden);
        rClass.push(ALITERAL("max"     )).visibility_(emp::gui::visibility_e::Hidden);
        rClass.push(ALITERAL("consumed")).visibility_(emp::gui::visibility_e::Hidden);
    }



    m_pDesigner->push<mapping_c>         (1u, true).field.enumeration_(m_pEnumMappings)
        .on_new_item_(function(&*m_pEnumMappings, &enum_t<mapping_c>::new_item));

    m_pDesigner->push<skin_data_s>       (1u, true).field.enumeration_(m_pEnumSkins)
        .on_new_item_(function(&*m_pEnumSkins   , &enum_t<skin_data_s>::new_item));

    m_pDesigner->push<emp::snd::layout_i>(1u, true).field.enumeration_(m_pEnumLayouts)
        .on_new_item_(function(&*m_pEnumLayouts , &enum_t<emp::snd::layout_i>::new_item));

    m_pDesigner->push<emp::snd::scale_i> (1u, true).field.enumeration_(m_pEnumScales)
        .on_new_item_(function(&*m_pEnumScales  , &enum_t<emp::snd::scale_i>::new_item));

    m_pDesigner->push<emp::loc::locale_c>(1u, true).field.enumeration_(m_pEnumLocales)
        .on_new_item_(function(&*m_pEnumLocales , &enum_t<emp::loc::locale_c>::new_item));

    auto& rSkin = EMP_GUI_SKIN;

    // gui
    m_pProperties->skin_(EMP_GUI_SKIN_(list, "lst."))
        .expanded_(true)
        .designer_(m_pDesigner)
        .registrar_(&*m_pRegistrar)
        .state().navigable_(true).index_(1).autosize.xy(true, true);
    EMP_RET_FALSE_IF_NOT(m_pProperties->set_object(m_pReflection));
    EMP_RET_FALSE_IF_NOT(m_pProperties->resize());
    //m_pProperties->set_selection(1, 1, true, change_e::Code);

    m_pMenu->create();
    m_pMenu->skin_(rSkin.menu.get(ALITERAL("men."))).user_(m_pProperties).toolbar.on_escape_(function(this, &game_c::on_escape_menu));


    auto const& rFunction = function(this, &game_c::on_button);

    {
        c_size stCount = EMP_STORE(ase::mod::mode_a)->object_count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            ase::mod::mode_a* const pMod = EMP_STORE(ase::mod::mode_a)->get_object(stIndex);
            EMP_ASSERT(pMod != nullptr);
            EMP_RET_FALSE_IF_NOT(pMod->create());
        }
    }

    {
        c_size stCount = m_pvpMods->count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            ase::mod::mode_i* const pMod = m_pvpMods->val(stIndex);
            EMP_RET_FALSE_IF(pMod == nullptr);
            m_pMenu->toolbar.add_item(pMod->name(), rFunction, nullptr);
        }
    }

    //EMP_RET_FALSE_IF_NOT(m_pModEvolution->create());
    //EMP_RET_FALSE_IF_NOT(m_pModImprovize->create());
    m_pModMusic = m_pModImprovize->mod_music();

   // @@0 create  m_pCtrlExitLabel->create();
    m_pCtrlExitLabel->skin_(EMP_GUI_SKIN_(label, "lbl.text_padded"));
    m_pCtrlExitLabel->state().autosize.y_(true);
    m_pCtrlExitLabel->set_text(emp::loc::string_s(ALITERAL("ase_mod_select_dlg_exit")));

    m_pDlgExit->transformation().width_(0.6f * rSkin.resolution.scale());
    m_pDlgExit->skin_(EMP_GUI_SKIN_(dialog, "dlg."))
        .type_(dialog_e::OkCancel)
        .user_control_(m_pCtrlExitLabel)
        .on_cancel_(function(this, &game_c::on_dlg_exit_cancel))
        .on_ok_(function(this, &game_c::on_dlg_exit_ok))
        .state().index_(0).autosize.xy(false, true);

    m_pRegistrar->register_window(m_pDlgExit);

    EMP_GUI_MANAGER->on_scale = emp::fct::function(this, &game_c::scale);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::scale(c_bool a_bUp)
{
//{ EMP_GUI_SKIN.resolution.scale_(EMP_GUI_SKIN.resolution.scale() + 0.1f); }
//{ EMP_GUI_SKIN.resolution.scale_(EMP_GUI_SKIN.resolution.scale() - 0.1f); }

    user().options.scale += a_bUp ? 0.1f : -0.1f;
    EMP_GUI_SKIN.resolution.scale_(user().options.scale);
    user().options.scale = EMP_GUI_SKIN.resolution.scale();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e game_c::on_dlg_exit_cancel(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    { emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgExit)); }
    return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e game_c::on_dlg_exit_ok(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pDlgExit));
        EMP_APPLICATION.send_exit();
        //set_next_mod(m_pExitMod);
    }
    return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
emp::gui::return_e game_c::on_button(c_size a_stIndex, c_bool a_bValue, emp::gui::c_change_e a_eChange)
{
    if (a_bValue)
    {
        /*auto const it = m_mpMods.get(a_stIndex);

        if (it != m_mpMods.end())
        {
            m_pNextMod = *it.value();
            if (m_stCurrentMod == a_stIndex && m_pNextMod != nullptr && m_pNextMod->next_mod() != nullptr)
            { m_pNextMod = m_pNextMod->next_mod(); }
            m_mpMods.set(a_stIndex, m_pNextMod);
        }
        else*/
        {
            m_pNextMod = m_pvpMods->val(a_stIndex);
            if (m_stCurrentMod == a_stIndex && m_pNextMod != nullptr && m_pNextMod->next_mod() != nullptr)
            { m_pNextMod = m_pNextMod->next_mod(); }
            //m_mpMods.set(a_stIndex, m_pNextMod);
        }
        m_stCurrentMod = a_stIndex;

        if (a_eChange == emp::gui::change_e::Mouse)
        {
            if (m_pMenu->state().visible)
            { emp::tt::silent(EMP_GUI_MANAGER->hide_window(&*m_pMenu)); }
        }
        return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e game_c::on_escape_menu(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange))
{
    if (m_pMenu->state().visible && a_bValue == true)
    {
        emp::tt::silent(EMP_GUI_MANAGER->hide_window(&*m_pMenu));
        return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
    }
    return EMP_GUI_RETURNE(emp::gui::return_e::Consumed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::destroy(void)
{
    m_pRegistrar->stop();
    m_pRegistrar->destroy();

    m_pPreviousMod = nullptr;
    m_pCurrentMod = nullptr;
    m_pNextMod = nullptr;

    m_pTrack->destroy_track();
    m_pEmphase->destroy();
    // @@0 m_pSkin->destroy();
    m_pMatrix->destroy();

    m_pRenderer->destroy();

    // @@0 create m_pProperties->destroy();

    foreach(m_vpReflections, emp::mem::safe_delete_reset_t());
    m_vpReflections.resize(0);

    // @@0 create m_pCtrlExitLabel->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::clear(void)
{
    m_pEmphase->clear();
    m_pTrack->clear_track();
    m_pMatrix->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool game_c::update(emp::tim::clock_c const& a_rClock)
{
    EMP_RET_FALSE_IF_NOT(m_pRenderer->update(a_rClock.delta()));
    EMP_GUI_SKIN.resolution.scale_(user().options.scale);

    if (m_pCurrentMod != m_pNextMod &&
        m_pNextMod != nullptr)
    {
        if (m_pNextMod->registrar().state() != emp::gui::state_e::Started)
        {
            EMP_LOG_INFO("Starting mod #0", m_pNextMod->name().key);
            EMP_RET_FALSE_IF_NOT(m_pNextMod->start(a_rClock));
            EMP_GUI_MANAGER->next_transition();
        }

        if (m_pCurrentMod != nullptr &&
            m_pCurrentMod->registrar().state() != emp::gui::state_e::Stopped &&
            m_pCurrentMod->registrar().state() != emp::gui::state_e::Stopping)
        {

            EMP_LOG_INFO("Stopping mod #0", m_pCurrentMod->name().key);
            m_pCurrentMod->stop();
        }

        if (m_pCurrentMod == nullptr ||
            (m_pCurrentMod->registrar().state() == emp::gui::state_e::Stopped ||
             m_pCurrentMod->registrar().state() == emp::gui::state_e::none))
        {
            EMP_RET_FALSE_IF(m_pNextMod == nullptr);

            clear();

            if (m_pCurrentMod != nullptr)
            { EMP_LOG_INFO("Stopped mod #0", m_pNextMod->name().key); }
            //EMP_RET_FALSE_IF_NOT(m_pNextMod->start(a_rClock));
            m_pPreviousMod = m_pCurrentMod;
            m_pCurrentMod = m_pNextMod;
            m_pNextMod = nullptr;

            //EMP_RET_FALSE_IF_NOT(m_pProperties->set_object(m_pReflection));
        }
    }

    EMP_RET_FALSE_IF(m_pCurrentMod == nullptr);

    if (m_pCurrentMod->registrar().state() != emp::gui::state_e::none)
    {
        EMP_RET_FALSE_IF_NOT(m_pCurrentMod->update(a_rClock));
    }

    if (m_pNextMod != nullptr && m_pNextMod->registrar().state() != emp::gui::state_e::none)
    {
        EMP_RET_FALSE_IF_NOT(m_pNextMod->update(a_rClock));
    }

    auto& rSkin = EMP_GUI_SKIN;
    if (m_pUser->options.skin != rSkin.data)
    { EMP_RET_FALSE_IF_NOT(rSkin.create(m_pUser->options.skin)); }

    EMP_RET_FALSE_IF_NOT(EMP_LOCALIZER.create(m_pUser->options.locale));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::handler_i* game_c::handler(void) const
{ return m_pCurrentMod; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e game_c::process_mouse(emp::app::event_s const& a_rEvent)
{
    emp::app::c_event_e eType = a_rEvent.type;
    if (eType == emp::app::event_e::MouseLeave)
    {
        if ((EMP_GUI_MANAGER->modal() == false && m_pMenu->is_hidden()) || m_pMenu->is_hiding())
        {
/*            emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pMenu, true, true, emp::gui::change_e::Mouse, false));
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);*/
            return emp::gui::return_e::Unused;
        }
    }

    /*else if (eType == emp::app::event_e::MouseEnter)
    {
        if (m_pMenu->state().visible())
        {
            emp::tt::silent(EMP_GAME().gui_manager().hide_window(&*m_pMenu));
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    else if (eType == emp::app::event_e::MousePressed)
    {
        auto const& rMouse = a_rEvent.mouse;
        if (rMouse.m_pmTouches->count() == 2)
        {
            if (EMP_GUI_MANAGER->modal() == false)
            {
                emp::tt::silent(EMP_GUI_MANAGER->show_window(&ASE_GAME.menu(), true, true, emp::gui::change_e::Mouse));
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
        }
    }
    else
    {
        auto const& rMouse = a_rEvent.mouse;
        if (rMouse.m_eButton == emp::app::button_e::Left &&
            rMouse.m_bPressed == false &&
            rMouse.m_siCount == 2)
        {
            if (EMP_GUI_MANAGER->modal() == false)
            {
                emp::tt::silent(EMP_GUI_MANAGER->show_window(&*m_pMenu, true, true, emp::gui::change_e::Mouse));
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
        }
    }*/
    auto const& rMouse = a_rEvent.mouse;
    if (a_rEvent.mouse_pressed() &&
        a_rEvent.mouse.m_eButton == emp::app::button_e::Left)
    {
        c_size stCount = rMouse.m_stToucheCount;
        if (stCount == 2)
        {
            //if (EMP_GUI_MANAGER->modal() == false)
            if (m_pMenu->is_visible() == false)
            {
                /*emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pMenu, true, true, emp::gui::change_e::Mouse, false));
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);*/
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Unused);
            }
        }
        else if (stCount == 3)
        {
            if (m_pMenu->is_visible())
            {
                if (m_pDlgExit->is_visible() == false)
                {
                    emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pDlgExit, true, true, emp::gui::change_e::Mouse, true));
                    EMP_RET_X_IF_NOT(emp::gui::return_e::Error, m_pDlgExit->resize());
                    m_pDlgExit->center();

                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
            }
            else
            if (m_pCurrentMod != nullptr)
            {
                auto* const pNextMod = m_pCurrentMod->next_mod();
                if (pNextMod != nullptr)
                {
                    set_next_mod(pNextMod);
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
            }
        }
    }
    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e game_c::process_keyboard(emp::app::event_s const& a_rEvent)
{
    auto const& rKeyboard = a_rEvent.keyboard;

    if (m_pTrack != nullptr &&
        m_pMatrix != nullptr)
    {
        if (m_pEmphase->translate(rKeyboard, m_pCurrentMod->time(), *m_pTrack, *m_pMatrix, *m_pUser->options.emphase.keyboard.mapping, true))
        {
            //if (a_rEvent.keyboard().pressed() && m_pOnTouch->is_valid())
            //{ (*m_pOnTouch)(); }
            return emp::gui::return_e::Unused;
        }
    }

    c_bool bPressed = rKeyboard.m_bPressed;
    emp::app::c_key_e eKey = rKeyboard.m_eKey;

    if (bPressed == true &&
        rKeyboard.modified() == false &&
        rKeyboard.m_bRepeat == false &&
        eKey == emp::app::key_e::Escape)
    {
        if (m_pCurrentMod->registrar().is_modal_visible() == false)
        {
            if (m_pMenu->state().visible)
            {
                emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pMenu));
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
            else if (m_pCurrentMod->prev_mod() == nullptr)
            {
                if (m_pDlgExit->state().visible == false || m_pDlgExit->is_hiding())
                {
                    emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pDlgExit, true, true, emp::gui::change_e::Mouse, true));
                    EMP_RET_X_IF_NOT(emp::gui::return_e::Error, m_pDlgExit->resize());
                    m_pDlgExit->center();
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
            }
            else if (m_pCurrentMod->fire_prev())
            { return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed); }
        }
    }
    else
    {
        if (eKey == emp::app::key_e::LeftAlt ||
            eKey == emp::app::key_e::RightAlt)
        {
            if (bPressed == false)
            {
                if (m_pMenu->state().visible)
                {
                    emp::tt::silent(EMP_GUI_MANAGER->hide_window(m_pMenu));
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
            }
            else if (
                rKeyboard.m_bAlt &&
                rKeyboard.m_bShift == false &&
                rKeyboard.m_bCtrl == false)
            {
                if (EMP_GUI_MANAGER->modal() == false ||
                    m_pMenu->is_hiding() || m_pMenu->is_hidden())
                {
                    /*emp::tt::silent(EMP_GUI_MANAGER->show_window(m_pMenu, true, true,emp::gui::change_e::Keyboard, false));
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);*/
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Unused);
                }
            }
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::set_next_mod(ase::mod::mode_i* const a_pValue)
{
    //EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(&user()));
    //EMP_ASSERT_BOOL_CALL(

    if (emp::tt::is_max(m_stCurrentMod))
    {
        c_size stCount = m_pvpMods->count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            if (m_pvpMods->val(stIndex) == a_pValue)
            { m_stCurrentMod = stIndex; }
        }
    }

    m_pNextMod = a_pValue;
    /*if (emp::tt::is_not_max(m_stCurrentMod))
    {
        //m_mpMods.set(m_stCurrentMod, a_pValue);
    }*/
    
    c_size stCount = m_pvpMods->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (m_pvpMods->val(stIndex) == a_pValue)
        { emp::tt::silent(m_pMenu->toolbar.set_selection(stIndex, emp::gui::change_e::Code)); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::set_layout(emp::snd::layout_i* const a_pValue)
{
    emp::snd::track_c* const pTrack = m_pTrack;
    if (pTrack == nullptr || (pTrack->layout() != a_pValue))
    {
        if ((&*m_pEmphase) != nullptr)
        { m_pEmphase->clear(); }
    }
    if (pTrack != nullptr)
    {
        pTrack->set_layout(a_pValue ? a_pValue :
        (m_pUser ? m_pUser->options.emphase.keyboard.layout : nullptr));
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::set_scale(emp::snd::scale_i* const a_pValue)
{
    emp::snd::track_c* const pTrack = m_pTrack;
    if (pTrack == nullptr || (pTrack->scale() != a_pValue))
    {
        if ((&*m_pEmphase) != nullptr)
        { m_pEmphase->clear(); }
    }
    if (pTrack != nullptr)
    { pTrack->set_scale(a_pValue ? a_pValue : (m_pUser ? m_pUser->options.emphase.keyboard.m_pScale : nullptr)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void game_c::set_user(user_c* const a_pValue)
{
    user_c* const pPrevious = m_pUser;
    m_pUser = a_pValue;
    if (a_pValue != nullptr)
    {
        EMP_GUI_SKIN.resolution.scale_(a_pValue->options.scale);
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(this));
        EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(a_pValue));
    }

    c_size stCount = m_vFctUserChanged.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        auto const& rFunction = m_vFctUserChanged.ref(stIndex);
        if (rFunction.is_valid())
        { rFunction(pPrevious, m_pUser); }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

