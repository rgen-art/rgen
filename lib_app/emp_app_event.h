//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_APP_EVENT_H
#define EMP_APP_EVENT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct configuration_s;
struct event_s;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef emp::fct::function_t<bool (*)(configuration_s& a_rConfiguration)> on_create_f;
typedef emp::fct::function_t<bool (*)(configuration_s& a_rConfiguration)> on_parse_f;
typedef emp::fct::function_t<bool (*)(void)> on_reflect_f;
typedef emp::fct::function_t<bool (*)(void)> on_init_f;
typedef emp::fct::function_t<bool (*)(bool)> on_exit_f;
typedef emp::fct::function_t<bool (*)(void)> on_deserialize_f;
typedef emp::fct::function_t<bool (*)(emp::cnt::vector<event_s> const&)> on_loop_f;
typedef emp::fct::function_t<bool (*)(void)> on_unit_test_f;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name state_e
#define enum_values\
    enum_value( Starting )\
    enum_value( Created  )\
    enum_value( Parsed   )\
    enum_value( Loading  )\
    enum_value( Running  )\
    enum_value( Exiting  )\
    enum_value( Failing  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name button_e
#define enum_type
#define enum_values\
    enum_value( Invalid   )\
    enum_value( Left      )\
    enum_value( Right     )\
    enum_value( Center    )\
    enum_value( WheelUp   )\
    enum_value( WheelDown )
#include "emp_tt_enum.def.h"

//enum_value(Pointer)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name event_e
#define enum_type
#define enum_values\
    enum_value( none             )\
    enum_value( KeyboardPressed  )\
    enum_value( KeyboardReleased )\
    enum_value( MousePressed     )\
    enum_value( MouseReleased    )\
    enum_value( MouseDragged     )\
    enum_value( MouseMoved       )\
    enum_value( MouseEnter       )\
    enum_value( MouseLeave       )\
    enum_value( eFocusIn         )\
    enum_value( eFocusOut        )\
    enum_value( ResizeWindow     )\
    enum_value( MoveWindow       )\
    enum_value( MoveResizeWindow )\
    enum_value( FullScreen       )\
    enum_value( Minimize         )\
    enum_value( Maximize         )\
    enum_value( Restore          )\
    enum_value( Active           )\
    enum_value( Inactive         )\
    enum_value( AppCreate        )\
    enum_value( AppParse         )\
    enum_value( AppReflect       )\
    enum_value( AppDeserialize   )\
    enum_value( AppSerialize     )\
    enum_value( AppInit          )\
    enum_value( AppExit          )\
    enum_value( AppKill          )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name key_e
#define enum_values\
    enum_value( none         )\
    enum_value( Tabulation   )\
    enum_value( Space        )\
    enum_value( Enter        )\
    enum_value( Left         )\
    enum_value( Right        )\
    enum_value( Up           )\
    enum_value( Down         )\
    enum_value( Escape       )\
    enum_value( Backspace    )\
    enum_value( Delete       )\
    enum_value( LeftAlt      )\
    enum_value( RightAlt     )\
    enum_value( LeftShift    )\
    enum_value( RightShift   )\
    enum_value( LeftCtrl     )\
    enum_value( RightCtrl    )\
    enum_value( LeftCommand  )\
    enum_value( RightCommand )\
    enum_value( Slash        )\
    enum_value( Period       )\
    enum_value( SemiColon    )\
    enum_value( Comma        )\
    enum_value( Num_0        )\
    enum_value( Num_1        )\
    enum_value( Num_2        )\
    enum_value( Num_3        )\
    enum_value( Num_4        )\
    enum_value( Num_5        )\
    enum_value( Num_6        )\
    enum_value( Num_7        )\
    enum_value( Num_8        )\
    enum_value( Num_9        )\
    enum_value( Alpha_A      )\
    enum_value( Alpha_B      )\
    enum_value( Alpha_C      )\
    enum_value( Alpha_D      )\
    enum_value( Alpha_E      )\
    enum_value( Alpha_F      )\
    enum_value( Alpha_G      )\
    enum_value( Alpha_H      )\
    enum_value( Alpha_I      )\
    enum_value( Alpha_J      )\
    enum_value( Alpha_K      )\
    enum_value( Alpha_L      )\
    enum_value( Alpha_M      )\
    enum_value( Alpha_N      )\
    enum_value( Alpha_O      )\
    enum_value( Alpha_P      )\
    enum_value( Alpha_Q      )\
    enum_value( Alpha_R      )\
    enum_value( Alpha_S      )\
    enum_value( Alpha_T      )\
    enum_value( Alpha_U      )\
    enum_value( Alpha_V      )\
    enum_value( Alpha_W      )\
    enum_value( Alpha_X      )\
    enum_value( Alpha_Y      )\
    enum_value( Alpha_Z      )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(keyboard_event_s)
public:
    key_e m_eKey = key_e::none;
    ui32 m_ui32Code = 0;
    ui64 m_ui64Char = 0;
    bool m_bPressed = false;
    bool m_bAlt = false;
    bool m_bShift = false;
    bool m_bCtrl = false;
    bool m_bRepeat = false;

    EMP_RETURN bool modified(void) const { return m_bAlt || m_bShift || m_bCtrl; }
    EMP_RETURN bool ctrl_only(void) const { return m_bCtrl && (m_bAlt == false) && (m_bShift == false); }
};
EMP_TYPEDEF(keyboard_event_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(mouse_event_s)
public:
    button_e m_eButton = button_e::Invalid;
    ui64 m_uiId = 0;

    size_t m_stToucheCount = 0;
    size_t m_stTapCount = 0;

    v2f m_v2fOrigin = v2f::zero();
    v2f m_v2fPrevious = v2f::zero();
    v2f m_v2fPosition = v2f::zero();

    map_t<ui64, event_s>* m_pmTouches = nullptr;

    EMP_INLINE EMP_RETURN v2f distance(void) const { return v2f::sub(m_v2fPosition, m_v2fOrigin); }
    EMP_INLINE EMP_RETURN v2f velocity(void) const { return v2f::sub(m_v2fPosition, m_v2fPrevious); }
};
EMP_TYPEDEF(mouse_event_s)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(tap_s)
    button_e m_eButton = button_e::Invalid;
    v2f m_v2fPosition{};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::tim;

// EMP_PRAGMA_PUSH_IGNORE(EMP_W_COPY_CTOR_IMPLICITLY_DEFINED_DELETED)
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_MOVE_CTOR_IMPLICITLY_DEFINED_DELETED)
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED)
// EMP_PRAGMA_PUSH_IGNORE(EMP_W_MOVE_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED)
EMP_NOCOPY(struct, tap_event_s)
public: 
    emp::tim::time_c m_Time{};
    button_e m_eButton = button_e::Invalid;
    size_t m_stReleaseCount = 0;
    size_t m_stTapCount = 0;
    //bool m_bCancelled = false;
    //bool m_bReleased = false;
    bool m_bDiscard = false;
    vector<bool> m_vbTapped{};
    vector<v2f> m_vTouches{};
    vector<tap_s> m_vNext{};
    map_t<ui64, event_s> m_mTouches{16};
    size_t m_stMaxToucheCount = 0;

    tap_event_s() {}

    void start(time_c const& a_rTime)
    { m_Time = a_rTime; }

    void update(time_c const& a_rTime)
    {
        if (time_c::equal(time_c(), m_Time) == false &&
            time_c::more(time_c::sub(a_rTime, m_Time), time_c::from_seconds(0.2f)))
        {
            cancel();
            //EMP_LOG_INFO("UPDATE CANCEL------------");
            //m_bReleased = true;
        }
    }

    void cancel(void)
    {
        //m_bCancelled = true;

        m_Time = time_c();
        m_vbTapped.resize(0);
        m_vTouches.resize(0);

        m_stTapCount = 0;
        //EMP_LOG_INFO("RESTARTING TAP_COUNT");
    }

    void release(void)
    {
        //m_bCancelled = false;
        //m_bReleased = true;
        m_bDiscard = false;

        m_stReleaseCount = 0;
    }
};
// EMP_PRAGMA_POP_IGNORE(EMP_W_MOVE_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED)
// EMP_PRAGMA_POP_IGNORE(EMP_W_ASSIGNMENT_OPERATOR_IMPLICITLY_DEFINED_DELETED)
// EMP_PRAGMA_POP_IGNORE(EMP_W_MOVE_CTOR_IMPLICITLY_DEFINED_DELETED)
// EMP_PRAGMA_POP_IGNORE(EMP_W_COPY_CTOR_IMPLICITLY_DEFINED_DELETED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(window_event_s)
public:
    f64 m_dPointX = 0;
    f64 m_dPointY = 0;
    f64 m_dPointWidth = 0;
    f64 m_dPointHeight = 0;
    f64 m_dScale = 1;
    bool m_bFullScreen = false;
    bool m_bBegin = false;

public:
    EMP_RETURN f64 pixel_x(void) const { return m_dPointX * m_dScale; }
    EMP_RETURN f64 pixel_y(void) const { return m_dPointY * m_dScale; }
    EMP_RETURN f64 pixel_width(void) const { return m_dPointWidth * m_dScale; }
    EMP_RETURN f64 pixel_height(void) const { return m_dPointHeight * m_dScale; }
};
EMP_TYPEDEF(window_event_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(event_s)
    event_e type = event_e::none;
    emp::tim::time_c time = emp::tim::time_c();
    mouse_event_s mouse = mouse_event_s();
    keyboard_event_s keyboard = keyboard_event_s();
    window_event_s window = window_event_s();

    EMP_INLINE EMP_RETURN bool mouse_pressed(void) const { return type == event_e::MousePressed; }
    EMP_INLINE EMP_RETURN bool mouse_released(void) const { return type == event_e::MouseReleased; }
    EMP_INLINE EMP_RETURN bool mouse_moved(void) const { return type == event_e::MouseMoved; }
    EMP_INLINE EMP_RETURN bool mouse_dragged(void) const { return type == event_e::MouseDragged; }

    EMP_INLINE EMP_RETURN bool mouse_event(void) const
    {
        switch (type)
        {
            case event_e::MouseEnter:
            case event_e::MouseLeave:
            case event_e::MouseMoved:
            case event_e::MouseDragged:
            case event_e::MousePressed:
            case event_e::MouseReleased:
            { return true; }
            case event_e::none:
            case event_e::KeyboardPressed:
            case event_e::KeyboardReleased:
            case event_e::eFocusIn:
            case event_e::eFocusOut:
            case event_e::ResizeWindow:
            case event_e::MoveWindow:
            case event_e::MoveResizeWindow:
            case event_e::FullScreen:
            case event_e::Minimize:
            case event_e::Maximize:
            case event_e::Restore:
            case event_e::Active:
            case event_e::Inactive:
            case event_e::AppCreate:
            case event_e::AppParse:
            case event_e::AppReflect:
            case event_e::AppDeserialize:
            case event_e::AppSerialize:
            case event_e::AppInit:
            case event_e::AppExit:
            case event_e::AppKill:
            default:
            { return false; }
        };
    }

    EMP_INLINE EMP_RETURN bool keyboard_event(void) const
    {
        switch (type)
        {
            case event_e::KeyboardPressed:
            case event_e::KeyboardReleased:
            { return true; }
            case event_e::MouseEnter:
            case event_e::MouseLeave:
            case event_e::MouseMoved:
            case event_e::MouseDragged:
            case event_e::MousePressed:
            case event_e::MouseReleased:
            case event_e::none:
            case event_e::eFocusIn:
            case event_e::eFocusOut:
            case event_e::ResizeWindow:
            case event_e::MoveWindow:
            case event_e::MoveResizeWindow:
            case event_e::FullScreen:
            case event_e::Minimize:
            case event_e::Maximize:
            case event_e::Restore:
            case event_e::Active:
            case event_e::Inactive:
            case event_e::AppCreate:
            case event_e::AppParse:
            case event_e::AppReflect:
            case event_e::AppDeserialize:
            case event_e::AppSerialize:
            case event_e::AppInit:
            case event_e::AppExit:
            case event_e::AppKill:
            default:
            { return false; }
        };
    }
};
EMP_TYPEDEF(event_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

