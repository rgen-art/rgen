//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_APP_APPLICATION_H
#define EMP_APP_APPLICATION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_configuration.h"
#include "emp_app_event.h"
#include "emp_app_platform_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_prf_profiler.h"

#include "emp_tim_time.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_thread.h"
#include "emp_hh_mutex.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
    class manager_c;
    class skin_c;
    class stack_c;
} }

namespace emp { namespace rfx { class factory_c; } }
namespace emp { namespace snd { class alc_context_c; } }
namespace emp { namespace snd { class al_listener_c; } }
namespace emp { namespace snd { class mixer_c; } }
namespace emp { namespace snd { class player_c; } }

namespace emp { namespace mat {
    template <typename> struct m44;
    template <typename> class stack_t;
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class argument_i;
class game_i;
class keyboard_c;
class mouse_c;
class mutex_c;
class platform_i;
class platform_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, application_c)
public:
    friend platform_i;
    friend platform_c;

private:
    static application_c* s_pInstance;
public:
    #define EMP_APPLICATION emp::app::application_c::instance()
    #define EMP_APPLICATION_LOCK std::lock_guard<std::recursive_mutex> oLock(EMP_APPLICATION.application_mutex());

    static void set_instance(application_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_RETURN application_c& instance(void) { return *s_pInstance; }

private:
    platform_i* m_pPlatform;
    game_i* m_pGame;

    //@@0 remove emp::tim::time_c m_TimePressed;
    //@@0 remove button_e m_eButtonPressed;

    array<key_e, 256, true> m_aEnumKeys;
    array<event_s, 256, true> m_aEventKeys;

    bool m_bAlt;
    bool m_bShift;
    bool m_bCtrl;

    emp::gui::manager_c* m_pGuiManager;
    emp::gui::skin_c* m_pGuiSkin;
    EMP_POINTER(emp::snd::alc_context_c) m_pSndContext;
    EMP_POINTER(emp::snd::al_listener_c) m_pSndListener;
    EMP_POINTER(emp::snd::mixer_c) m_pSndMixer;
    EMP_POINTER(emp::snd::player_c) m_pSndPlayer;

    EMP_XX_IF_PROFILE(
        emp::prf::profiler_c m_LoadingProfiler;
        EMP_POINTER(emp::prf::profiler_c) m_pSwap;
        EMP_POINTER(emp::prf::time_profiler_c) m_pFrame;
        EMP_POINTER(emp::prf::time_profiler_c) m_pResize;
        EMP_POINTER(emp::prf::time_profiler_c) m_pRender;
        EMP_POINTER(emp::prf::time_profiler_c) m_pUpdate;
    )

    vector<emp::fct::function_t<bool(*)(c_f64, c_f64)>> m_vResizeHooks;

    vector<event_s> m_vEvents0;
    vector<event_s> m_vEvents1;
    vector<event_s>* m_pvEvents;
    vector<event_s> m_vMouseQueue{};

    bool m_bFocused;
    bool m_bResize;
    bool m_bMinimized;
    bool m_bPreviousMinimized;

    EMP_POINTER(emp::gui::stack_c) m_pStack;

    float m_fConsoleTime;
    bool m_bDumpProfile;
    emp::tim::time_c m_FrameDrop;
    astring m_sTime;

    emp::cnt::vector<argument_i*> m_vpArguments;
    configuration_s m_Configuration;

    std::recursive_mutex m_oStateMutex;
    std::thread m_oLoadingThread;

    std::recursive_mutex* m_pApplicationMutex;

    state_e m_eState;

    f64 m_dPointWidth;
    f64 m_dPointHeight;
    f64 m_dPixelWidth;
    f64 m_dPixelHeight;

    ui64 m_ui64TouchId = 0;
    enum_array_of(tap_event_s, button_e) m_avTouches{};

protected:
    astring m_sResourcePath;

public:
    application_c(void);
    ~application_c(void);

public:
    EMP_RETURN ui64 new_touch_id(void)
    {
        c_ui64 ui64Id = m_ui64TouchId;
        if (emp::tt::is_max(m_ui64TouchId))
        { m_ui64TouchId = 0; }
        else
        { ++m_ui64TouchId; }
        return ui64Id;
    }

public:
    platform_i const& platform(void) { return *m_pPlatform; }

public:
    EMP_RETURN bool run(configuration_s const& a_rConfiguration, platform_i* const a_pPlatform, std::recursive_mutex& a_rMutex);

    void add_argument(argument_i* const a_pValue) { m_vpArguments.add(a_pValue); }

    void send_exit(void) { add(event_e::AppExit); }
    void send_kill(void) { add(event_e::AppKill); }

    EMP_RETURN game_i& game(void) { return *m_pGame; }
    void game(game_i* const a_pValue) { m_pGame = a_pValue; }

    EMP_RETURN emp::gui::manager_c* gui_manager(void) { return m_pGuiManager; }
    void gui_manager(emp::gui::manager_c* const a_pValue) { m_pGuiManager = a_pValue; }

    EMP_RETURN emp::gui::skin_c& gui_skin(void) { return *m_pGuiSkin; }
    void gui_skin(emp::gui::skin_c* const a_pValue);

    EMP_RETURN emp::snd::player_c& snd_player(void) { return *m_pSndPlayer; }

    void add_resize_hook(emp::fct::function_t<bool(*)(c_f64, c_f64)> const& a_rHook) { m_vResizeHooks.add(a_rHook); }

    EMP_RETURN f64 view_pixel_width(void) const;
    EMP_RETURN f64 view_pixel_height(void) const;
    EMP_RETURN bool bind_platform(void);
    EMP_RETURN bool clear_platform(void);
    EMP_RETURN c_astring& resource_path(void) const;
    EMP_RETURN bool bind_push(void);
    EMP_RETURN bool bind_pop(void);

    EMP_RETURN std::recursive_mutex& application_mutex(void) { return *m_pApplicationMutex; }

    EMP_RETURN bool loop(void);

public:
    void output(cpc_char a_szOutput) const;

private:

    void add(c_event_e a_eValue) { emp::tt::silent(push(a_eValue)); }
    EMP_RETURN event_s& push(void) { return push(emp::tim::time_c()); }

    EMP_RETURN array<event_s, 256, true> const& event_keys(void) const { return m_aEventKeys; }


public:
    EMP_RETURN bool fullscreen(c_bool a_bValue);

private:
    EMP_RETURN state_e state(void);
public:
    EMP_RETURN configuration_s const& configuration(void) const { return m_Configuration; }

public:
    EMP_RETURN bool exit(c_bool a_bError);

private:
    EMP_RETURN bool fullscreen(void) const { return m_Configuration.fullscreen; }

    void state(c_state_e a_eValue);

    EMP_RETURN bool loop(emp::cnt::vector<event_s>& a_rvEvents);
    EMP_RETURN bool create(void);
    EMP_RETURN bool create(cpc_char a_szDataDirectory);

public:
    void purge_wheel(void);

public:
    EMP_RETURN bool destroy(void);

private:
    EMP_RETURN bool parse(void);
    EMP_RETURN bool parse(c_int a_siArgc, c_char* a_aszArgv[], c_shell<argument_i *const>& a_vpArguments, cpc_char a_szUsage);

    EMP_RETURN bool reflect(void);
    EMP_RETURN bool async_deserialize(void);
    void sync_deserialize(void);
    EMP_RETURN bool deserialize(void);
    EMP_RETURN bool serialize(void);
    EMP_RETURN bool init(configuration_s const& a_rConfiguration);
    EMP_RETURN bool init(void);

    //void show(void);

    EMP_RETURN event_s& push(emp::tim::time_c const& a_rTime)
    {
        event_s& rEvent = m_pvEvents->push();
        rEvent = event_s();
        rEvent.time = a_rTime;
        return rEvent;
    }

    EMP_RETURN event_s& push(c_event_e a_eValue)
    {
        event_s& rEvent = push(emp::tim::time_c());
        rEvent.type = a_eValue;
        return rEvent;
    }


    //void add(emp::tim::time_c const& a_rTime, c_event_e a_eValue)
    //{ push(a_rTime).type = a_eValue; }

    EMP_RETURN event_s* get_last_event(c_event_e a_eEvent);

    EMP_RETURN emp::cnt::vector<event_s>& events(void) { return *m_pvEvents; }
    EMP_RETURN emp::cnt::vector<event_s> const& events(void) const { return *m_pvEvents; }

    EMP_RETURN bool begin_resize(c_f64 a_dWidth, c_f64 a_dHeight);
    EMP_RETURN bool end_resize(c_f64 a_dWidth, c_f64 a_dHeight);

    //void center(void);
    //void move(c_f64 a_dX, c_f64 a_dY);
    //EMP_RETURN bool resize(c_f64 a_dW, c_f64 a_dH);
    EMP_RETURN v2f::tret coordinates(c_f64 a_dX, c_f64 a_dY) const;

    //void set_focus(c_bool a_bValue) { m_bFocused = a_bValue; }
    //EMP_RETURN bool focus(void) const { return m_bFocused; }

    /*void set_default_width(c_f64 a_dValue) { m_Configuration.width = a_dValue; }
    EMP_RETURN f64 default_width(void) const { return m_Configuration.width; }

    void set_default_height(c_f64 a_dValue) { m_Configuration.height = a_dValue; }
    EMP_RETURN f64 default_height(void) const { return m_Configuration.height; }

    void set_default_x(c_f64 a_dValue) { m_Configuration.x = a_dValue; }
    EMP_RETURN f64 default_x(void) const { return m_Configuration.x; }

    void set_default_y(c_f64 a_dValue) { m_Configuration.y = a_dValue; }
    EMP_RETURN f64 default_y(void) const { return m_Configuration.y; }*/

    void get_tap_count(tap_event_s& a_rButton, emp::tim::time_c const& a_rTime, c_bool a_bPressed);


    EMP_RETURN array<key_e, 256, true>  const& enum_keys(void) const { return m_aEnumKeys; }
    void clear_keyboard(void);
    void clear_mouse(void);
    void fill_event(event_s& a_rEvent);
    void mouse_pressed(c_ui64 a_uiTouch, c_button_e a_eButton, c_f64 a_dX, c_f64 a_dY);
    void mouse_released(c_ui64 a_uiTouch, c_button_e a_eButton, c_f64 a_dX, c_f64 a_dY);
    void mouse_move(c_ui64 a_uiTouch, c_button_e a_eButton, c_f64 a_dX, c_f64 a_dY);
    void mouse_enter(void) { add(event_e::MouseEnter); }
    void mouse_leave(void) { add(event_e::MouseLeave); }
    void keyboard_repeat(event_s& a_rEvent);


private:
    EMP_RETURN bool process_app_event(c_event_s& a_rEvent);
    EMP_RETURN bool process_window_event(event_s& a_rEvent);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

