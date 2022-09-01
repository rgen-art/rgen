//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_COR_GAME_H
#define ASE_COR_GAME_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_game_i.h"

#include "emp_cnt_vector.h"

#include "emp_gfx_model.h"

#include "emp_mat_vector.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define ASE_GAME ase::cor::game_c::instance()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {
    struct event_s;
} }

namespace emp { namespace gui {
    class designer_c;
    class dialog_c;
    template <typename> class enum_t;
    class label_c;
    class menu_c;
    class property_c;
    class registrar_c;
    struct skin_data_s;
    class splitter_c;
    class window_c;
    EMP_FORWARD(enum, ui32, change_e)
    EMP_FORWARD(enum, ui32, return_e)
} }

namespace emp { namespace ose {
    class compiler_c;
    class processor_c;
    class renderer_c;
} }

namespace emp { namespace rfx {
    class type_i;
} }

namespace emp { namespace snd {
    class layout_i;
    class scale_i;
    class track_c;
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase {
    namespace gui {
        class skin_c;
    }
    namespace mod {
        class emphase_c;
        class selection_c;
        class evolution_c;
        class improvize_c;
        class mode_i;
        class music_c;
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace cor {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class emphase_c;
class option_shell_c;
class mapping_c;
class matrix_c;
class user_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, game_c, public, emp::app::game_i)
private:
    static game_c* s_pInstance;
    EMP_POINTER(emp::mat::random_c) m_pRandom{};

    // serialized.
    user_c* m_pUser = nullptr;
    ase::mod::selection_c* m_pModSelection = nullptr;
    ase::mod::evolution_c* m_pModEvolution = nullptr;
    ase::mod::emphase_c* m_pModEmphase = nullptr;
    ase::mod::improvize_c* m_pModImprovize = nullptr;
    ase::mod::music_c* m_pModMusic = nullptr;

    emp::cnt::vector<ase::mod::mode_i*> m_vpMods;
    emp::cnt::vector<ase::mod::mode_i*> m_vpDebugMods;
    emp::cnt::vector<ase::mod::mode_i*>* m_pvpMods;

    // runtime.
    ase::mod::mode_i* m_pPreviousMod = nullptr;
    ase::mod::mode_i* m_pCurrentMod = nullptr;
    ase::mod::mode_i* m_pNextMod = nullptr;


    EMP_POINTER(emp::gui::dialog_c) m_pDlgExit;
    EMP_POINTER(emp::gui::label_c) m_pCtrlExitLabel;

    EMP_POINTER(emp::gui::registrar_c)  m_pRegistrar;
    EMP_POINTER(emp::gui::menu_c) m_pMenu;
    EMP_POINTER(emp::gui::property_c) m_pProperties;
    EMP_POINTER(emp::gui::designer_c) m_pDesigner;
    EMP_POINTER(ase::cor::option_shell_c) m_pOptions;
    emp::rfx::type_t<ase::cor::option_shell_c>* m_pReflection = nullptr;

    EMP_POINTER(emp::snd::track_c) m_pTrack;
    EMP_POINTER(emphase_c) m_pEmphase;
    EMP_POINTER(ase::gui::skin_c) m_pSkin;
    EMP_POINTER(matrix_c) m_pMatrix;

    EMP_POINTER(emp::ose::renderer_c) m_pRenderer;

    emp::cnt::vector<emp::rfx::type_i*> m_vpReflections{};

    EMP_POINTER(emp::gui::enum_t<emp::loc::locale_c>) m_pEnumLocales;
    EMP_POINTER(emp::gui::enum_t<ase::cor::user_c>) m_pEnumUsers;
    EMP_POINTER(emp::gui::enum_t<ase::cor::mapping_c>) m_pEnumMappings;
    EMP_POINTER(emp::gui::enum_t<emp::snd::layout_i>) m_pEnumLayouts;
    EMP_POINTER(emp::gui::enum_t<emp::snd::scale_i>) m_pEnumScales;
    EMP_POINTER(emp::gui::enum_t<emp::gui::skin_data_s>) m_pEnumSkins;

    typedef emp::fct::function_t<void (*)(user_c* const a_pPrevious, user_c* const a_pCurrent)> on_user_changed;
    vector<on_user_changed> m_vFctUserChanged{};

    //map_t<size_t, ase::mod::mode_i*> m_mpMods;
    size_t EMP_TT_MAX_VAR(m_stCurrentMod);

public:
    game_c(void);
    ~game_c(void);

public:
    EMP_RETURN bool post_load(void);
    void destroy(void);

public:
    static void set_instance(game_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_RETURN game_c& instance(void) { return *s_pInstance; }

public:
    void clear(void);

public:
    EMP_RETURN emp::rfx::type_t<option_shell_c> const& rfx_options(void) const { return *m_pReflection; }
    EMP_RETURN emp::rfx::type_t<option_shell_c>& rfx_options(void) { return *m_pReflection; }

    EMP_RETURN emp::gui::designer_c const& dsn_options(void) const { return *m_pDesigner; }

public:
    //>> game_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    void render(void) {}
    EMP_RETURN emp::gui::handler_i* handler(void) const;
    //<< game_i

    void set_next_mod(ase::mod::mode_i* const a_pValue);

public:
    emp::mat::random_c& random(void) { return *m_pRandom; }

public:
    void set_layout(emp::snd::layout_i* const a_pValue);
    void set_scale(emp::snd::scale_i* const a_pValue);

public:
    EMP_RETURN bool is_current_mod(ase::mod::mode_i const& a_rMod)
    { return m_pCurrentMod == &a_rMod; }

public:
    EMP_RETURN emp::ose::renderer_c const& ose_renderer(void) const { return *m_pRenderer; }
    EMP_RETURN emp::ose::renderer_c& ose_renderer(void) { return *m_pRenderer; }

public:
    EMP_RETURN ase::mod::selection_c const& mod_selection(void) const { return * m_pModSelection; }
    EMP_RETURN ase::mod::selection_c& mod_selection(void) { return *m_pModSelection; }

public:
    EMP_RETURN ase::mod::evolution_c const& mod_evolution(void) const { return *m_pModEvolution; }
    EMP_RETURN ase::mod::evolution_c& mod_evolution(void) { return *m_pModEvolution; }

public:
    EMP_RETURN ase::mod::emphase_c const& mod_emphase(void) const { return *m_pModEmphase; }
    EMP_RETURN ase::mod::emphase_c& mod_emphase(void) { return *m_pModEmphase; }

public:
    EMP_RETURN ase::mod::improvize_c const& mod_improvize(void) const { return *m_pModImprovize; }
    EMP_RETURN ase::mod::improvize_c& mod_improvize(void) { return *m_pModImprovize; }
public:
    EMP_RETURN ase::mod::music_c const& mod_music(void) const { return *m_pModMusic; }
    EMP_RETURN ase::mod::music_c& mod_music(void) { return *m_pModMusic; }

public:
    EMP_RETURN emphase_c const& emphase(void) const { return *m_pEmphase; }
    EMP_RETURN emphase_c& emphase(void) { return *m_pEmphase; }

public:
    EMP_RETURN emp::gui::menu_c& menu(void) { return *m_pMenu; }

public:
    void set_user(user_c* const a_pValue);
    EMP_RETURN user_c const& user(void) const { return *m_pUser; }
    EMP_RETURN user_c& user(void) { return *m_pUser; }

public:
    EMP_RETURN emp::snd::track_c* track(void) { return m_pTrack; }
    EMP_RETURN emp::snd::track_c const* track(void) const { return m_pTrack; }

public:
    EMP_RETURN matrix_c* matrix(void) { return m_pMatrix; }
    EMP_RETURN matrix_c const* matrix(void) const { return m_pMatrix; }

public:
    EMP_RETURN ase::gui::skin_c const& skin(void) const { return *m_pSkin; }

private:
    EMP_RETURN ase::mod::mode_i const* previous_mod(void) const { return m_pPreviousMod; }

public:
    EMP_RETURN emp::gui::return_e process_mouse(emp::app::event_s const& a_rEvent);
    EMP_RETURN emp::gui::return_e process_keyboard(emp::app::event_s const& a_rEvent);

public:
    EMP_RETURN emp::gui::return_e on_dlg_exit_cancel(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange));
    EMP_RETURN emp::gui::return_e on_dlg_exit_ok(c_bool a_bValue, emp::gui::c_change_e EMP_SILENT(a_eChange));

public:
    EMP_RETURN emp::gui::return_e on_button(c_size a_stIndex, c_bool a_bValue, emp::gui::c_change_e a_eChange);

public:
    EMP_RETURN emp::gui::return_e on_escape_menu(c_bool a_bValue, emp::gui::c_change_e a_eChange);

public:
    void add_on_user_changed(on_user_changed const& a_rFunction) { m_vFctUserChanged.add(a_rFunction); }

private:
    void scale(c_bool a_bUp);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

