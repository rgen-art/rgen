//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_app_application.h"

#include "emp_app_application.h"
#include "emp_app_argument.h"
#include "emp_app_event.h"
#include "emp_app_game_i.h"
#include "emp_app_platform.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_gl_device.h"

#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

#include "emp_loc_localizer.h"
#include "emp_loc_reflection.rfx.h"

#include "emp_prf_profiler.h"

#include "emp_snd_alc_context.h"
#include "emp_snd_al_device.h"
#include "emp_snd_mixer.h"
#include "emp_snd_player.h"
#include "emp_snd_quality.h"

#include "emp_tim_time.h"

#if defined(EMP_XX_VALGRIND)
#include "valgrind/callgrind.h"
#endif

#include "emp_xml_reader.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, mutex_c)
private:
    std::mutex m_oMutex;
public:
    mutex_c(void):
        m_oMutex()
    {}

public:
    EMP_RETURN std::mutex const& value(void) const { return m_oMutex; }
    EMP_RETURN std::mutex& value(void) { return m_oMutex; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
application_c* application_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
application_c::application_c(void):
    m_pPlatform(nullptr),
    m_pGame(),
    // @@0 remove m_TimePressed(),
    // @@0 remove m_eButtonPressed(),
    m_aEnumKeys(),
    m_aEventKeys(),
    m_bAlt(false),
    m_bShift(false),
    m_bCtrl(false),
    m_pGuiManager(),
    m_pGuiSkin(),
    m_pSndContext(),
    m_pSndListener(),
    m_pSndMixer(),
    m_pSndPlayer(),
#if defined(EMP_XX_PROFILE)
    m_LoadingProfiler(ALITERAL("loading")),
    m_pSwap(ALITERAL("swap")),
    m_pFrame(ALITERAL("frame")),
    m_pResize(ALITERAL("resize")),
    m_pRender(ALITERAL("render")),
    m_pUpdate(ALITERAL("update")),
#endif
    m_vResizeHooks(),
    m_vEvents0(),
    m_vEvents1(),
    m_pvEvents(&m_vEvents0),
    m_bFocused(),
    m_bResize(false),
    m_bMinimized(false),
    m_bPreviousMinimized(false),
    m_pStack(),
    m_fConsoleTime(0),
    m_bDumpProfile(false),
    m_FrameDrop(emp::tim::time_c::from_seconds(0.034f)),
    m_sTime(),
    m_vpArguments(),
    m_Configuration(),
    m_oStateMutex(),
    m_oLoadingThread(),
    m_pApplicationMutex(nullptr),
    m_eState(state_e::Starting),
    m_dPointWidth(0),
    m_dPointHeight(0),
    m_dPixelWidth(0),
    m_dPixelHeight(0),
    m_sResourcePath()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
application_c::~application_c(void)
{ EMP_ASSERT_BOOL_CALL(destroy()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::run(configuration_s const& a_rConfiguration, platform_i* const a_pPlatform, std::recursive_mutex& a_rMutex)
{
    m_pApplicationMutex = &a_rMutex;
    m_pPlatform = a_pPlatform;
    EMP_RET_FALSE_IF(m_pPlatform == nullptr);
    EMP_RET_FALSE_IF_NOT(EMP_CLOCK.create());

    EMP_XX_IF_PROFILE(m_LoadingProfiler.begin_sample());
    state(state_e::Starting);
    m_Configuration = a_rConfiguration;
    return m_pPlatform->run();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::create(void)
{
    EMP_RET_FALSE_IF_NOT(m_pPlatform->create());
    EMP_RET_FALSE_IF_NOT(m_pSndContext->create());
    EMP_RET_FALSE_IF_NOT(m_pSndListener->create(v3f::zero(), { 0, 0, -1 }, { 0, 1, 0 }));
    EMP_RET_FALSE_IF_NOT(m_pSndMixer->create(emp::snd::quality_c(44100, 16, 2, 1), 50, 0.0005f));

    m_pSndPlayer->create();

    emp::tim::time_c oSeed;
    EMP_RET_FALSE_IF_NOT(EMP_CLOCK.system_current(oSeed));

    emp::mat::simd_v4<float>::denormal_off();

    if (m_Configuration.on_create.is_valid())
    { EMP_RET_FALSE_IF_NOT(m_Configuration.on_create(m_Configuration)); }

    state(state_e::Created);

    {
        c_size stCount = enum_count(button_e);
        m_avTouches.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_avTouches.ref(stIndex).m_eButton = enum_from(button_e, stIndex); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::destroy(void)
{
    m_pPlatform = nullptr;

    m_pSndPlayer->destroy();
    m_pSndMixer->destroy();
    m_pSndListener->destroy();
   // m_pSndContext->destroy();

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::parse(void)
{
    EMP_RET_FALSE_IF_NOT(argument_i::parse(
        m_Configuration.argc, m_Configuration.argv,emp::cnt::shell<argument_i*const>(m_vpArguments),
        m_Configuration.usage));

    if (m_Configuration.on_parse.is_valid())
    { EMP_RET_FALSE_IF_NOT(m_Configuration.on_parse(m_Configuration)); }

    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.init());

#if defined EMP_XX_OS_WIN
#else
    EMP_RET_FALSE_IF_NOT(m_pPlatform->current_context());
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.create());
#endif

    c_f64 dScreenWidth = m_pPlatform->screen_pixel_width();
    c_f64 dScreenHeight = m_pPlatform->screen_pixel_height();

    if (m_Configuration.centered)
    {
        m_Configuration.x = (dScreenWidth - m_Configuration.width) / 2.0;
        m_Configuration.y = (dScreenHeight - m_Configuration.height) / 2.0;
    }

    EMP_RET_FALSE_IF_NOT(m_pPlatform->parse());

    astring sPath = m_Configuration.data;
    c_bool bRelative = sPath.len() > 0 && sPath.get_first_char() != ':';
    if (bRelative == false)
    { sPath.trim_left(); }
    EMP_RET_FALSE_IF_NOT(m_pPlatform->resource_path(sPath, bRelative));
    EMP_LOG_INFO("resource path : '#0'", resource_path());

    EMP_FACTORY.create(STR_AFORMAT("#0rfx/", resource_path()));

    EMP_RET_FALSE_IF_NOT(m_pPlatform->current_context());

#if defined EMP_XX_OS_WIN
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.create());
#endif

    auto& rGraphics = EMP_GRAPHICS;
    rGraphics.set_clear_color(v4f::all(0));
    // @@0 remove rGraphics.set_clear_color({1, 0, 1, 1 });
    // @@0 rGraphics.set_blend(true);
    rGraphics.set_blend(false);
    rGraphics.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    rGraphics.set_front_face(GL_CCW);
    rGraphics.set_cull_face(GL_BACK);
    rGraphics.glDisable_(GL_CULL_FACE);
    rGraphics.set_depth_test(false);
    rGraphics.set_dither(false);
    rGraphics.set_polygon_smooth(false);

    EMP_RET_FALSE_IF_NOT(m_pPlatform->current_context());

    {
        int siSwapInterval = 0;
        if (m_pPlatform->get_swap_interval(siSwapInterval) == false)
        { EMP_LOG_INFO("failed to get swap interval"); }
        else
        { EMP_LOG_INFO("swap interval : #0", siSwapInterval); }
    }

    c_int siSetSwapInterval = m_Configuration.vsync ? 1 : 0;
    if (m_pPlatform->set_swap_interval(siSetSwapInterval) == false)
    { EMP_LOG_INFO("failed to set swap interval #0", siSetSwapInterval); }

    {
        int siSwapInterval = 0;
        if (m_pPlatform->get_swap_interval(siSwapInterval) == false)
        { EMP_LOG_INFO("failed to get swap interval"); }
        else
        { EMP_LOG_INFO("swap interval : #0", siSwapInterval); }
    }

    EMP_RET_FALSE_IF_NOT(m_pPlatform->current_context());
    EMP_RET_FALSE_IF_NOT(bind_platform());
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.framebuffer_status());
    EMP_RET_FALSE_IF_NOT(clear_platform());

    m_dPointWidth = m_pPlatform->view_point_width();
    m_dPointHeight = m_pPlatform->view_point_height();

    m_dPixelWidth = m_pPlatform->view_pixel_width();
    m_dPixelHeight = m_pPlatform->view_pixel_height();

    state(state_e::Parsed);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::reflect(void)
{
    EMP_RET_FALSE_IF_NOT(m_pPlatform->reflect());

    if (m_Configuration.on_reflect.is_valid())
    { EMP_RET_FALSE_IF_NOT(m_Configuration.on_reflect()); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::init(void)
{
    EMP_RET_FALSE_IF_NOT(EMP_XML_PARSER.create());
    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.instanciate(true));

    EMP_RET_FALSE_IF_NOT(m_pPlatform->init());

    if (m_Configuration.on_init.is_valid())
    { EMP_RET_FALSE_IF_NOT(m_Configuration.on_init()); }

#if defined(EMP_XX_OS_OSX) || defined(EMP_XX_OS_LIN) || defined(EMP_XX_OS_WIN) // @@0 fullscreen
    // @@0 remove m_pGuiManager->on_alt_enter() = emp::fct::function(&*this, &application_a::toggle_fullscreen);
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN state_e application_c::state(void)
{
    std::lock_guard<std::recursive_mutex> oLock(m_oStateMutex);
    return m_eState;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::state(c_state_e a_eValue)
{
    std::lock_guard<std::recursive_mutex> oLock(m_oStateMutex);
    m_eState = a_eValue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::async_deserialize(void)
{
    m_oLoadingThread = std::thread(emp::fct::function(this, &application_c::sync_deserialize));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::sync_deserialize(void)
{
    state(state_e::Loading);
    if (m_pPlatform->current_context() == false)
    {
        state(state_e::Failing);
        return;
    }

    if (deserialize() == false)
    {
        state(state_e::Failing);
        return;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::deserialize(void)
{
    EMP_RET_FALSE_IF_NOT(m_pPlatform->deserialize());

    EMP_RET_FALSE_IF_NOT(m_pPlatform->current_context());
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.create_resources(resource_path()));

    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.read());
    EMP_RET_FALSE_IF_NOT(EMP_FACTORY.post_load());

    if (m_Configuration.on_deserialize.is_valid())
    { EMP_RET_FALSE_IF_NOT(m_Configuration.on_deserialize()); }

    state(state_e::Running);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::serialize(void)
{ return m_pPlatform->serialize(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::exit(c_bool a_bError)
{
    state(state_e::Exiting);
    if (m_Configuration.on_exit.is_valid())
    { EMP_RET_FALSE_IF_NOT(m_Configuration.on_exit(a_bError)); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void application_c::show(void) // @@0 remove
{
    m_pPlatform->show();

    if (m_Configuration.centered)
    { center(); }
    else
    { move(m_Configuration.x, m_Configuration.y); }
    EMP_ASSERT_BOOL_CALL(fullscreen(m_Configuration.fullscreen));
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::loop(void)
{
    std::lock_guard<std::recursive_mutex> oLock(*m_pApplicationMutex);

    emp::cnt::vector<event_s>* const pvEvents = m_pvEvents;
    m_pvEvents = (m_pvEvents == &m_vEvents0 ? &m_vEvents1 : &m_vEvents0);

    c_bool bResult = loop(*pvEvents);
    if (bResult == false)
    {
        state(state_e::Failing);
        EMP_LOG_ERROR("failing...");
        emp::tt::silent(m_pPlatform->exit(true));
    }

    pvEvents->resize(0);

    return bResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN f64 application_c::view_pixel_width(void) const
{ return m_pPlatform->view_pixel_width(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN f64 application_c::view_pixel_height(void) const
{ return m_pPlatform->view_pixel_height(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_astring& application_c::resource_path(void) const
{ return m_pPlatform->resource_path(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::bind_platform(void)
{ return EMP_GRAPHICS.bind_framebuffer(m_pPlatform->framebuffer(), view_pixel_width(), view_pixel_height()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::clear_platform(void)
{
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.clear_target());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::output(cpc_char a_szOutput) const
{ m_pPlatform->output(a_szOutput); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void application_c::center(void)
{
    c_f64 dScreenWidth = m_pPlatform->screen_point_width();
    c_f64 dScreenHeight = m_pPlatform->screen_point_height();

    c_f64 dWindowWidth = m_pPlatform->window_point_width();
    c_f64 dWindowHeight = m_pPlatform->window_point_height();

    c_f64 dX = (dScreenWidth - dWindowWidth) / 2;
    c_f64 dY = (dScreenHeight - dWindowHeight) / 2;

    move(dX, dY);
    m_pPlatform->center();
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void application_c::move(c_f64 a_dX, c_f64 a_dY)
{
    m_Configuration.m_dX = a_dX;
    m_Configuration.m_dY = a_dY;
    m_pPlatform->move(a_dX, a_dY);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool application_c::resize(c_f64 a_dW, c_f64 a_dH)
{
    set_default_width(a_dW);
    set_default_height(a_dH);
    EMP_RET_FALSE_IF_NOT(m_pPlatform->resize(a_dW, a_dH));
    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret application_c::coordinates(c_f64 a_dX, c_f64 a_dY) const
{
    c_f64 x = a_dX / m_pPlatform->view_point_width();
    c_f64 y = 1 -  a_dY / m_pPlatform->view_point_height();
    return v2f::add(v2f::all(-1), v2f::mul(v2f::all(2), { emp::ast::cast<float>(x), emp::ast::cast<float>(y) }));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::loop(vector<event_s>& a_rvEvents)
{
    EMP_RET_FALSE_IF_NOT(EMP_CLOCK.update(), "failed to update time");

    c_state_e eState = state();
    if (eState == state_e::Running || eState == state_e::Loading)
    {
        EMP_RET_FALSE_IF_NOT(m_pPlatform->current_context());
        EMP_RET_FALSE_IF_NOT(bind_platform());
        EMP_RET_FALSE_IF_NOT(clear_platform());
    }

    EMP_RET_FALSE_IF_NOT(m_pPlatform->poll(), "failed to poll events");

    {
        c_size stCount = a_rvEvents.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            event_s& rEvent = a_rvEvents.ref(stIndex);
            rEvent.time = EMP_CLOCK.application_current();
            EMP_RET_FALSE_IF_NOT(process_app_event(rEvent), "failed to process event");
        }
    }

    switch (state())
    {
        case state_e::Starting:{ return true; }
        case state_e::Created: { return true; }
        case state_e::Parsed:  { return true; }
        case state_e::Failing: { EMP_RET_FALSE; }
        case state_e::Exiting: { return true; }
        case state_e::Loading:
        {
            //EMP_RET_FALSE_IF_NOT(bind_platform());
            //EMP_RET_FALSE_IF_NOT(clear_platform());
            EMP_RET_FALSE_IF_NOT(m_pPlatform->present());
            return true;
        }
        case state_e::Running:
        {
            //EMP_RET_FALSE_IF_NOT(bind_platform());
            //EMP_RET_FALSE_IF_NOT(clear_platform());
#if defined EMP_UNIT_TEST
            if (m_Configuration.on_unit_test.is_valid())
            { EMP_RET_FALSE_IF_NOT(m_Configuration.on_unit_test()); }

#endif
            c_bool bPreviousMinimized = m_bPreviousMinimized;
            m_bPreviousMinimized = m_bMinimized;

            if (m_bMinimized == false ||
                bPreviousMinimized == false)
            {
                if (m_pPlatform->view_pixel_width() > 0 &&
                    m_pPlatform->view_pixel_height() > 0)
                {
                    auto const& oCurrent = EMP_CLOCK.application_current();
#if defined(EMP_XX_VALGRIND)
                    CALLGRIND_START_INSTRUMENTATION;
#endif

                    m_sTime.clear();
#if defined(EMP_XX_PROFILE)
                    m_pSwap->end_sample();
                    c_float fSwap = emp::tim::time_c::to_seconds(m_pSwap->time().watch().current());
                    if (m_bDumpProfile)
                    {
                        m_pSwap->output();
                        m_bDumpProfile = false;
                    }
                    m_pSwap->begin_sample();

                    m_pFrame->begin_sample();
#endif


                    //set_skin(m_pGuiSkin);


                    auto const l_flush_tap = [this](c_button_e a_eButton, tap_event_s& a_rButton, emp::tim::time_c const& a_rTime, c_bool a_bPressed)
                    {
                        auto& rvTouches = a_rButton.m_vNext;
                        rvTouches.resize(0);
                        c_size stCount = m_vMouseQueue.count();
                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        {
                            event_s const& rEvent = m_vMouseQueue.ref(stIndex);
                            if (rEvent.mouse.m_eButton == a_eButton &&
                                rEvent.mouse_pressed())
                            {
                                tap_s& rTap = rvTouches.push();
                                rTap.m_eButton = rEvent.mouse.m_eButton;
                                rTap.m_v2fPosition = rEvent.mouse.m_v2fPosition;
                            }
                        }

                        if (rvTouches.count() > 0)
                        {
                            get_tap_count(a_rButton, a_rTime, a_bPressed);
                        }
                    };

                    bool bFlushed = false;
                    bool bFlushOthers = true;

                    c_size stButtonCount = m_avTouches.count();
                    for (size_t stButton = 0; stButton < stButtonCount; ++stButton)
                    {
                        auto& rButton = m_avTouches.ref(stButton);
                        c_button_e eButton = rButton.m_eButton;
                        using namespace emp::tim;
                        for (size_t stIndex = 0; stIndex < m_vMouseQueue.count(); ++stIndex)
                        {
                            event_s const& rEvent = m_vMouseQueue.ref(stIndex);
                            if (rEvent.mouse_pressed()) //@@0 remove
                            {
                                int k = 0;
                                ++k;
                            }

                            if (rEvent.mouse.m_eButton == eButton)
                            {
                                if (rEvent.mouse_pressed())
                                {
                                    bFlushOthers = false;
                                    time_c const oDelta = time_c::sub(oCurrent, rEvent.time);
                                    if (time_c::more(oDelta, time_c::from_seconds(0.2f)) && rButton.m_stTapCount > 0)
                                    {
                                        bFlushed = true;
                                        rButton.m_bDiscard = true;
                                        l_flush_tap(eButton, rButton, oCurrent, true);
                                        break;
                                    }
                                }
                                else if (rEvent.mouse_released())
                                {
                                    l_flush_tap(eButton, rButton, oCurrent, false);
                                    bFlushed = true;
                                    break;
                                }
                                else if (rEvent.mouse_dragged())
                                {
                                    bFlushed = true;
                                }
                                else if (rEvent.mouse_moved())
                                {
                                    bFlushed = true;
                                }
                            }
                        }
                        rButton.update(oCurrent);
                    }

                    if (bFlushed || bFlushOthers)
                    {
                        c_size stCount = m_vMouseQueue.count();
                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        { a_rvEvents.add(m_vMouseQueue.ref(stIndex)); }
                        m_vMouseQueue.resize(0);
                    }

                    c_size stCount = a_rvEvents.count();
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        event_s& rEvent = a_rvEvents.ref(stIndex);
                        if (rEvent.mouse_pressed())// @@0 remove
                        {
                            int kk = 0;
                            ++kk;
                        }
                        EMP_RET_FALSE_IF_NOT(process_window_event(rEvent));
                        if (rEvent.mouse_event())
                        {
                            switch (m_pGame->process_mouse(rEvent))
                            {
                                case emp::gui::return_e::Unused:
                                {
                                    switch (EMP_GUI_MANAGER->process_mouse(rEvent))
                                    {
                                        case emp::gui::return_e::Unused:
                                        {
                                            EMP_RET_FALSE_IF(m_pGame->handler() != nullptr && m_pGame->handler()->process_mouse(rEvent) == emp::gui::return_e::Error);
                                            break;
                                        }
                                        case emp::gui::return_e::Consumed:
                                        { continue; }
                                        case emp::gui::return_e::Error:
                                        default:
                                        { EMP_RET_X(false); }
                                    };
                                    break;
                                }
                                case emp::gui::return_e::Consumed:
                                { continue; }
                                case emp::gui::return_e::Error:
                                default:
                                { EMP_RET_X(false); }
                            };
                        }
                        else if (rEvent.keyboard_event())
                        {
                            switch (m_pGame->process_keyboard(rEvent))
                            {
                                case emp::gui::return_e::Unused:
                                {
                                    switch (EMP_GUI_MANAGER->process_keyboard(rEvent))
                                    {
                                        case emp::gui::return_e::Unused:
                                        {
                                            EMP_RET_FALSE_IF(m_pGame->handler() != nullptr && m_pGame->handler()->process_keyboard(rEvent) == emp::gui::return_e::Error);
                                            break;
                                        }
                                        case emp::gui::return_e::Consumed:
                                        { continue; }
                                        case emp::gui::return_e::Error:
                                        default:
                                        { EMP_RET_X(false); }
                                    };
                                    break;
                                }
                                case emp::gui::return_e::Consumed:
                                { continue; }
                                case emp::gui::return_e::Error:
                                default:
                                { EMP_RET_X(false); }
                            };
                        }
                    }

                    EMP_RET_FALSE_IF_NOT(m_pGame->update(EMP_CLOCK), "failed to update game");

                    EMP_XX_IF_PROFILE(m_pUpdate->begin_sample();)
                    EMP_RET_FALSE_IF_NOT(m_pGuiManager->update(EMP_CLOCK));
                    m_pSndPlayer->update();
                    m_pSndMixer->update();
                    EMP_XX_IF_PROFILE(m_pUpdate->end_sample();)

                    EMP_XX_IF_PROFILE(m_pResize->begin_sample();)
                    EMP_RET_FALSE_IF_NOT(m_pGuiManager->resize());
                    EMP_XX_IF_PROFILE(m_pResize->end_sample();)

                    EMP_RET_FALSE_IF_NOT(m_pGuiManager->transform(*m_pStack));
                    EMP_ASSERT(m_pStack->matrices.count() == 0);
                    EMP_ASSERT(m_pStack->depths.count() == 0);
                    EMP_ASSERT(m_pStack->enabled.count() == 0);

                    EMP_XX_IF_PROFILE(m_pRender->begin_sample();)
                    EMP_RET_FALSE_IF_NOT(m_pGuiManager->render());
                    EMP_XX_IF_PROFILE(m_pRender->end_sample();)

                    m_pGuiManager->clean();

                    m_pGame->render();

                    m_pSndPlayer->render(*m_pSndMixer);

                    m_pSndContext->process();

                    if (m_fConsoleTime > 2 && m_Configuration.fps)
                    { m_bDumpProfile = true; }
                    while (m_fConsoleTime > 2)
                    { m_fConsoleTime -= 2; }
                    EMP_XX_IF_PROFILE(m_fConsoleTime += fSwap;)

#if defined(EMP_XX_PROFILE)
                    if (m_bDumpProfile)
                    {
                        EMP_LOG_INFO("-------------------------");
                        EMP_LOG_INFO("cpu : #0fps, #1", emp::ast::cast<ui32>(1.0f/fSwap), emp::str::from(m_sTime, m_pFrame->watch().current()));

                        EMP_GRAPHICS.log_info();

                        EMP_LOG_INFO("mem : ovh : #0 - #1, net : #2 - #3, alloc : #4 - #5, dealloc : #6 - #7",
                        emp::mem::size_s(EMP_ALLOCATOR.raw().net_bytes().m_stCount - EMP_ALLOCATOR.req().net_bytes().m_stCount),
                        emp::mem::size_s(EMP_ALLOCATOR.raw().m_AllocBytes.m_stCount - EMP_ALLOCATOR.req().m_AllocBytes.m_stCount),
                        EMP_ALLOCATOR.raw().net_bytes(), EMP_ALLOCATOR.raw().net_count(),
                        EMP_ALLOCATOR.raw().m_AllocBytes, EMP_ALLOCATOR.raw().m_stAllocCount,
                        EMP_ALLOCATOR.raw().m_DeallocBytes, EMP_ALLOCATOR.raw().m_stDeallocCount);

                        m_pResize->output();
                        m_pRender->output();
                        m_pUpdate->output();
                    }
                    m_pFrame->end_sample();
                    if (emp::tim::time_c::more(m_pFrame->watch().delta(), m_FrameDrop))
                    {
                        EMP_LOG_WARNING("------------ Frame Drop -----------");
                        m_pFrame->output();
                    }
                    else if (m_bDumpProfile)
                    { m_pFrame->output(); }
#endif

#if defined(EMP_XX_VALGRIND)
                    CALLGRIND_STOP_INSTRUMENTATION;
#endif
                }

                if (m_bResize)
                {
                    m_pPlatform->sync();
                    // @@0 remove EMP_RET_FALSE_IF_NOT(swap(), "failed to swap")
                    m_bResize = false;
                }
                else
                {
                    // @@0 remove EMP_RET_FALSE_IF_NOT(swap(), "failed to swap")
                }
            }

            EMP_GRAPHICS.end_of_frame();
            EMP_RET_FALSE_IF_NOT(m_pPlatform->present());
            return true;
        }
        default:
        {
            EMP_RET_X(false);
        }
    }
    // @@0 unreachable return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN event_s* application_c::get_last_event(c_event_e a_eEvent)
{
    c_size stCount = m_pvEvents->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        size_t stOffset = stCount - stIndex - 1;
        event_s* pEvent = m_pvEvents->ptr(stOffset);
        if (pEvent->type == a_eEvent)
        { return pEvent; }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::purge_wheel(void)
{
    m_vMouseQueue.resize(0);
    m_vEvents0.resize(0);
    m_vEvents1.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::process_window_event(event_s& a_rEvent)
{
    c_event_e eType = a_rEvent.type;

    auto const l_move_window = [this](c_window_event_s const& a_rWindow)
    {
        if (m_bMinimized == false &&
            m_Configuration.fullscreen == false &&
            m_Configuration.maximized == false)
        {
            m_Configuration.x = a_rWindow.pixel_x();
            m_Configuration.y = a_rWindow.pixel_y();
            m_Configuration.centered = false;
        }
    };

    auto const l_resize_window = [this](c_window_event_s const& a_rWindow) EMP_RETURN -> bool
    {
        if (m_bMinimized == false)
        {
            c_f64 dWidth = a_rWindow.pixel_width();
            c_f64 dHeight = a_rWindow.pixel_height();

            if (emp::mat::not_equal_(dWidth, m_dPixelWidth) ||
                emp::mat::not_equal_(dHeight, m_dPixelHeight))
            {
                if (a_rWindow.m_bBegin)
                {
                    event_s& rEvent = push(event_e::ResizeWindow);
                    rEvent.window = a_rWindow;
                    rEvent.window.m_bBegin = false;

                    if (fullscreen() == false && m_Configuration.maximized == false)
                    {
                        m_Configuration.width = dWidth;
                        m_Configuration.height = dHeight;
                    }

                    EMP_RET_FALSE_IF_NOT(begin_resize(dWidth, dHeight));
                }
                else
                {
                    EMP_RET_FALSE_IF_NOT(end_resize(dWidth, dHeight));
                    m_dPixelWidth = dWidth;
                    m_dPixelHeight = dHeight;
                    m_dPointWidth = a_rWindow.m_dPointWidth;
                    m_dPointHeight = a_rWindow.m_dPointHeight;
                }
            }

            m_bResize = true;
        }
        return true;
    };

    auto const l_update_touche_count = [&a_rEvent](tap_event_s& a_rButton)
    {
        a_rEvent.mouse.m_stToucheCount = a_rButton.m_stMaxToucheCount;
        for (auto it = a_rButton.m_mTouches.begin(); it.not_end(); ++it)
        { it.value()->mouse.m_stToucheCount = a_rButton.m_stMaxToucheCount; }
    };

    switch (eType)
    {
        case event_e::MoveWindow:
        {
            l_move_window(a_rEvent.window);
            break;
        }
        case event_e::ResizeWindow:
        {
            EMP_RET_FALSE_IF_NOT(l_resize_window(a_rEvent.window));
            break;
        }
        case event_e::MoveResizeWindow:
        {
            l_move_window(a_rEvent.window);
            EMP_RET_FALSE_IF_NOT(l_resize_window(a_rEvent.window));
            break;
        }
        case event_e::KeyboardPressed:
        {
            auto const& rKeyboard = a_rEvent.keyboard;
            if (rKeyboard.m_eKey == key_e::Enter &&
                rKeyboard.m_bAlt && rKeyboard.m_bShift == false && rKeyboard.m_bCtrl == false)
            { EMP_RET_FALSE_IF_NOT(fullscreen(fullscreen() == false)); }

            EMP_LOG_LOUSY("event_e::eKeyPress | code #0 | char #1 | touch #2 | pressed #3 | repeat #4",
                rKeyboard.m_ui32Code, rKeyboard.m_ui64Char, static_cast<ui32>(rKeyboard.m_eKey),
                rKeyboard.m_bPressed ? ALITERAL("true") : ALITERAL("false"),
                rKeyboard.m_bRepeat ? ALITERAL("true") : ALITERAL("false"));

            break;
        }
        case event_e::KeyboardReleased:
        {
#ifdef EMP_XX_LOG_LOUSY
            EMP_XX_IF_LOG(auto const& rKeyboard = a_rEvent.keyboard;)
            EMP_LOG_LOUSY("event_e::eKeyRelease | code #0 | char #1 | touch #2 | pressed #3 | repeat #4",
                rKeyboard.m_ui32Code, rKeyboard.m_ui64Char, static_cast<ui32>(rKeyboard.m_eKey),
                rKeyboard.m_bPressed ? ALITERAL("true") : ALITERAL("false"),
                rKeyboard.m_bRepeat ? ALITERAL("true") : ALITERAL("false"));
#endif
            break;
        }
        case event_e::MousePressed:
        {
            EMP_LOG_LOUSY("event_e::MousePressed #0", a_rEvent.mouse.m_uiId);
            l_update_touche_count(m_avTouches.ref(enum_to(a_rEvent.mouse.m_eButton)));
            break;
        }
        case event_e::MouseReleased:
        {
            c_ui64 uiId = a_rEvent.mouse.m_uiId;
            EMP_LOG_LOUSY("event_e::MouseReleased #0", uiId);
            auto& rButton = m_avTouches.ref(enum_to(a_rEvent.mouse.m_eButton));
            auto it = rButton.m_mTouches.get(uiId);
            a_rEvent.mouse.m_v2fOrigin = it.value()->mouse.m_v2fOrigin;
            a_rEvent.mouse.m_stTapCount = it.value()->mouse.m_stTapCount;
            EMP_RET_FALSE_IF(it.end());
            rButton.m_mTouches.remove(it);

            l_update_touche_count(rButton);

            if ((rButton.m_mTouches.count() > 0) == false)
            {
                rButton.m_stMaxToucheCount = 0;
            }
            break;
        }
        case event_e::MouseMoved:
        {
            auto& rButton = m_avTouches.ref(enum_to(a_rEvent.mouse.m_eButton));
            l_update_touche_count(rButton);

            c_ui64 uiId = a_rEvent.mouse.m_uiId;
            auto const it = rButton.m_mTouches.get(uiId);
            c_bool bDragged = it.not_end();
            if (bDragged) // && @@0 remove  v2f::length(a_rEvent.mouse.delta())) > 0.02f)
            {
                a_rEvent.type = event_e::MouseDragged;
                a_rEvent.mouse.m_eButton = it.value()->mouse.m_eButton;
                a_rEvent.mouse.m_stTapCount = it.value()->mouse.m_stTapCount;
                // @@0 remove EMP_LOG_INFO("DRAGGED #0", uiId);
            }
            else
            {
                a_rEvent.mouse.m_eButton = button_e::Invalid;
                // @@0 remove EMP_LOG_INFO("NOT DRAGGED #0", uiId);
            }
            break;
        }
        case event_e::MouseDragged:
        {
            auto& rButton = m_avTouches.ref(enum_to(a_rEvent.mouse.m_eButton));
            l_update_touche_count(rButton);

            c_ui64 uiId = a_rEvent.mouse.m_uiId;
            //EMP_LOG_DEBUG("event_e::MouseDragged #0", uiId);
            auto const it = rButton.m_mTouches.get(uiId);
            EMP_RET_FALSE_IF(it.end());
            it.value()->mouse.m_v2fPrevious = it.value()->mouse.m_v2fPosition;
            a_rEvent.mouse.m_v2fPrevious = it.value()->mouse.m_v2fPosition;
            it.value()->mouse.m_v2fPosition = a_rEvent.mouse.m_v2fPosition;
            //EMP_LOG_INFO("VX #0, VY #1", it.value()->mouse.velocity().x, it.value()->mouse.velocity().y);
            a_rEvent.mouse.m_v2fOrigin = it.value()->mouse.m_v2fOrigin;
            break;
        }
        case event_e::MouseEnter: { break; }
        case event_e::MouseLeave: { break; }
        case event_e::eFocusIn:
        {
            clear_keyboard();
            m_bFocused = true;
            break;
        }
        case event_e::eFocusOut:
        {
            clear_keyboard();
            m_bFocused = false;
            break;
        }
        case event_e::Minimize:
        {
            EMP_LOG_LOUSY("EMINIMIZE");
            m_bMinimized = true;
            break;
        }
        case event_e::Maximize:
        {
            EMP_LOG_LOUSY("EMAXIMIZE");
            m_Configuration.maximized = true;
            break;
        }
        case event_e::Restore:
        {
            EMP_LOG_LOUSY("ERESTORE");
            m_Configuration.maximized = false;
            m_bMinimized = false;
            break;
        }
        case event_e::Active:
        {
            EMP_LOG_LOUSY("EACTIVE");
            //m_bMinimized = false;
            break;
        }
        case event_e::Inactive:
        {
            EMP_LOG_LOUSY("EINACTIVE");
            break;
        }
        case event_e::FullScreen:
        {
            m_Configuration.fullscreen = a_rEvent.window.m_bFullScreen;
            break;
        }
        case event_e::AppCreate:
        case event_e::AppParse:
        case event_e::AppReflect:
        case event_e::AppDeserialize:
        case event_e::AppSerialize:
        case event_e::AppInit:
        case event_e::AppExit:
        case event_e::AppKill:
        { break; }
        case event_e::none:
        default:
        { break; }
    };

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::process_app_event(c_event_s& a_rEvent)
{
    c_event_e eType = a_rEvent.type;

    switch (eType)
    {
        case event_e::MoveWindow:
        case event_e::ResizeWindow:
        case event_e::MoveResizeWindow:
        case event_e::KeyboardPressed:
        case event_e::KeyboardReleased:
        case event_e::MousePressed:
        case event_e::MouseReleased:
        case event_e::MouseDragged:
        case event_e::MouseMoved:
        case event_e::MouseEnter:
        case event_e::MouseLeave:
        case event_e::eFocusIn:
        case event_e::eFocusOut:
        case event_e::Minimize:
        case event_e::Maximize:
        case event_e::Restore:
        case event_e::Active:
        case event_e::Inactive:
        case event_e::FullScreen:
        { break; }
        case event_e::AppCreate:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            EMP_RET_FALSE_IF_NOT(create(), "failed to create");
            break;
        }
        case event_e::AppParse:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            EMP_RET_FALSE_IF_NOT(parse(), "failed to parse");
            break;
        }
        case event_e::AppReflect:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            EMP_RET_FALSE_IF_NOT(reflect(), "failed to reflect");
            break;
        }
        case event_e::AppDeserialize:
        {
            {
                EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
                EMP_RET_FALSE_IF_NOT(deserialize(), "failed to deserialize");
            }
            EMP_XX_IF_PROFILE(m_LoadingProfiler.end_sample();)
            EMP_XX_IF_PROFILE(m_LoadingProfiler.output();)
            break;
        }
        case event_e::AppSerialize:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            EMP_RET_FALSE_IF_NOT(serialize(), "failed to serialize");
            break;
        }
        case event_e::AppInit:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            EMP_RET_FALSE_IF_NOT(init(), "failed to init");
            break;
        }
        case event_e::AppExit:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            EMP_RET_FALSE_IF_NOT(m_pPlatform->exit(false), "failed to exit");
            return false;
        }
        case event_e::AppKill:
        {
            EMP_XX_IF_PROFILE(emp::prf::profiler_c oProfiler(enum_to_astring(eType), true);)
            //EMP_RET_FALSE_IF_NOT(m_pPlatform->exit(true), "failed to kill");
            return false;
        }
        case event_e::none:
        default:
        { break; }
    };

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::fullscreen(c_bool a_bValue)
{
#if defined EMP_XX_OS_OSX
#else
    c_bool bTurnOff = m_Configuration.fullscreen && a_bValue == false;
#endif
    m_Configuration.fullscreen = a_bValue;
    // @@0 remove c_si32 si32Width = a_bValue ? screen_width_in_points() : m_Configuration.width;
    // @@0 remove c_si32 si32Height = a_bValue ? screen_height_in_points() : m_Configuration.height;

    EMP_RET_FALSE_IF_NOT(m_pPlatform->fullscreen(a_bValue));
    #if defined EMP_XX_OS_OSX // @@0 remove for other platform too.
    #else
    if (bTurnOff)
    {
        EMP_RET_FALSE_IF_NOT(m_pPlatform->move_resize(m_Configuration.x, m_Configuration.y,
            m_Configuration.width, m_Configuration.height));
    }
    #endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::begin_resize(c_f64 a_dWidth, c_f64 a_dHeight)
{
    gui_skin(m_pGuiSkin);

    // @@0 remove c_si32 si32Width = emp::ast::cast<si32>(scale_factor() * a_si32Width);
    // @@0 remove c_si32 si32Height = emp::ast::cast<si32>(scale_factor() * a_si32Height);

    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.viewport(a_dWidth, a_dHeight));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool application_c::end_resize(c_f64 a_dWidth, c_f64 a_dHeight)
{
    // @@0 remove c_si32 si32Width = emp::ast::cast<si32>(scale_factor() * a_si32Width);
    // @@0 remove c_si32 si32Height = emp::ast::cast<si32>(scale_factor() * a_si32Height);

    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.resize(a_dWidth, a_dHeight));
    c_size stCount = m_vResizeHooks.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(m_vResizeHooks.ref(stIndex)(a_dWidth, a_dHeight)); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 remove
void application_c::gui_skin(emp::gui::skin_c* const a_pValue)
{
    m_pGuiSkin = a_pValue;
    if (m_pGuiSkin != nullptr)
    { m_pGuiSkin->resolution.size_(m_pPlatform->view_pixel_width(), m_pPlatform->view_pixel_height()); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::clear_keyboard(void)
{
    m_bAlt = false;
    m_bShift = false;
    m_bCtrl = false;
    c_size stCount = m_aEventKeys.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_aEventKeys.ref(stIndex) = event_s(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::clear_mouse(void)
{
    // @@0 remove m_eButtonPressed = button_e::Invalid;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::keyboard_repeat(event_s& a_rEvent)
{
    keyboard_event_s& rKeyboard = a_rEvent.keyboard;
    c_key_e eKey = rKeyboard.m_eKey;
    c_bool bPressed = rKeyboard.m_bPressed;

    if (eKey == key_e::LeftAlt || eKey == key_e::RightAlt)
    { m_bAlt = bPressed; }
    else if (eKey == key_e::LeftShift || eKey == key_e::RightShift)
    { m_bShift = bPressed; }
    else if (eKey == key_e::LeftCtrl || eKey == key_e::RightCtrl)
    { m_bCtrl = bPressed; }

    rKeyboard.m_bAlt = m_bAlt;
    rKeyboard.m_bShift = m_bShift;
    rKeyboard.m_bCtrl = m_bCtrl;

    c_ui32 ui32Code = rKeyboard.m_ui32Code;

    c_keyboard_event_s& rPreviousKey = m_aEventKeys.ref(ui32Code).keyboard;
    if (rPreviousKey.m_bPressed == bPressed)
    {
        // @@0 log
        //EMP_LOG_WARNING("Discarding key #0 event on touch #1", bPressed ? "press" : "release", ui32Code);
    }
    else
    {
        if (bPressed)
        {
            c_size stCount = events().count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_size stOffset = stCount - stIndex - 1;
                event_s& rPrevious = events().ref(stOffset);

                if (rPrevious.type == event_e::KeyboardReleased &&
                    rPrevious.keyboard.m_ui32Code == ui32Code &&
                    rPrevious.keyboard.m_bPressed == false)
                {
                    if (emp::tim::time_c::equal(a_rEvent.time, rPrevious.time))
                    {
                        rPrevious.keyboard.m_bRepeat = true;
                        a_rEvent.keyboard.m_bRepeat = true;
                    }
                    break;
                }

            }
        }
        else
        {
            //rPrevious.keyboard().set_keychar(a_rEvent.keyboard().keychar());
            a_rEvent.keyboard.m_ui64Char = rPreviousKey.m_ui64Char;
        }
        m_aEventKeys.set(ui32Code, a_rEvent);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::get_tap_count(tap_event_s& a_rButton, emp::tim::time_c const& EMP_SILENT(a_rTime), c_bool EMP_SILENT(a_bPressed))
{
    auto const l_process_queue = [this, &a_rButton](void)
    {
        //size_t stRemoved = a_rButton.m_stTapCount;
        ++a_rButton.m_stTapCount;
        //EMP_LOG_INFO("_________TAP_COUNT #0", a_rButton.m_stTapCount);
        if (a_rButton.m_stTapCount > 1) // @@0 remove
        {
            int kk = 0;
            ++kk;
        }
        size_t stIndex = 0;
        while (stIndex < m_vMouseQueue.count())
        {
            event_s& rEvent = m_vMouseQueue.ref(stIndex);
            /*if (stRemoved > 0 && a_bPressed)
            {
                if (rEvent.mouse_pressed())
                { m_vMouseQueue.remove(stIndex); }
                else if (rEvent.mouse_released())
                {
                    m_vMouseQueue.remove(stIndex);
                    --stRemoved;
                    continue;
                }
            }
            else */if (
                rEvent.mouse_pressed() ||
                rEvent.mouse_released())
            { rEvent.mouse.m_stTapCount = a_rButton.m_stTapCount; }
            ++stIndex;
        }

        for (auto it = a_rButton.m_mTouches.begin(); it.not_end(); ++it)
        { it.value()->mouse.m_stTapCount = a_rButton.m_stTapCount; }
    };

    vector<tap_s>& rvCurrent = a_rButton.m_vNext;
    c_size stCurrentCount = rvCurrent.count();

    vector<v2f>& rvLast = a_rButton.m_vTouches;

    c_size stLastCount = rvLast.count();
    if (stLastCount > 0) // @@0 remove
    {
        int kk = 0;
        kk++;
    }
    else if (stCurrentCount > 0)
    {
        a_rButton.m_vbTapped.resize(stCurrentCount);
        for (size_t stIndex = 0; stIndex < stCurrentCount; ++stIndex)
        {
            rvLast.add(rvCurrent.ref(stIndex).m_v2fPosition);
            a_rButton.m_vbTapped.set(stIndex, stIndex);
        }
    }

    size_t stTouchCount = 0;
    if (stLastCount == stCurrentCount)
    {
        a_rButton.m_vbTapped.resize(stLastCount);
        for (size_t stLast = 0; stLast < stLastCount; ++stLast)
        { EMP_TT_MAX_VAR(a_rButton.m_vbTapped.ref(stLast)); }

        for (size_t stLast = 0; stLast < stLastCount; ++stLast)
        {
            c_v2f v2fLast = rvLast.ref(stLast);
            for (size_t stCur = 0; stCur < stCurrentCount; ++stCur)
            {
                tap_s const& rCur = rvCurrent.ref(stCur);
                if (rCur.m_eButton == a_rButton.m_eButton &&
                    emp::tt::is_max(a_rButton.m_vbTapped.val(stLast)) &&
                    v2f::length(v2f::sub(v2fLast, rCur.m_v2fPosition)) < 0.1f)
                {
                    a_rButton.m_vbTapped.set(stLast, stCur);
                    ++stTouchCount;
                    break;
                }
            }
        }
    }

    if (stTouchCount == stLastCount)
    { l_process_queue(); }
    else
    { a_rButton.cancel(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove EMP_INTERN EMP_RETURN bool get_double_click(c_button_e a_eLastButton, emp::tim::time_c const& a_rLastTime,
    c_button_e a_eCurrentButton, emp::tim::time_c const& a_rCurrentTime);
EMP_INTERN EMP_RETURN bool get_double_click(c_button_e a_eLastButton, emp::tim::time_c const& a_rLastTime,
    c_button_e a_eCurrentButton, emp::tim::time_c const& a_rCurrentTime)
{
    if (a_eLastButton == a_eCurrentButton)
    {
        emp::tim::time_c oDelta;
        emp::tim::time_c::sub(a_rCurrentTime, a_rLastTime, oDelta);
        if (oDelta.usec() < 250000)
        { return true; }
    }
    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::fill_event(event_s& a_rEvent)
{
    keyboard_event_s& rKeyboard = a_rEvent.keyboard;
    rKeyboard.m_bShift = m_bShift;
    rKeyboard.m_bAlt = m_bAlt;
    rKeyboard.m_bCtrl = m_bCtrl;
    a_rEvent.mouse.m_pmTouches = &m_avTouches.ref(enum_to(a_rEvent.mouse.m_eButton)).m_mTouches;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::mouse_pressed(c_ui64 a_uiTouch, c_button_e a_eButton, c_f64 a_dX, c_f64 a_dY)
{
    tap_event_s& rButton = m_avTouches.ref(enum_to(a_eButton));
    ++rButton.m_stReleaseCount;
    if (rButton.m_bDiscard)
    { return; }

    using namespace emp::tim;

    time_c const oTime = EMP_CLOCK.application_current();
    c_size stCount = m_vMouseQueue.count();

    if ((stCount > 0) == false || time_c::less(time_c::sub(oTime, m_vMouseQueue.first_ref().time), time_c::from_seconds(0.2f)))
    {
        /*if (stCount > 0)
        { EMP_LOG_INFO("Pushing #0", emp::tim::time_c::to_seconds(time_c::sub(oTime, m_vMouseQueue.first_ref().time).m_ui64usec)); }
        else
        { EMP_LOG_INFO("Pushing first"); }*/

        event_s& rEvent = m_vMouseQueue.push();
        rEvent.time = oTime;
        rEvent.type = event_e::MousePressed;

        c_v2f v2fPosition = coordinates(a_dX, a_dY);

        mouse_event_s oMouse;
        oMouse.m_eButton = a_eButton;
        oMouse.m_v2fPosition = v2fPosition;
        oMouse.m_v2fOrigin = v2fPosition;
        oMouse.m_v2fPrevious = v2fPosition;
        oMouse.m_uiId = a_uiTouch;
        rEvent.mouse = oMouse;


        fill_event(rEvent);

        c_ui64 uiId = rEvent.mouse.m_uiId;

        EMP_RET_IF_NOT(rButton.m_mTouches.get(uiId).end());
        rButton.m_mTouches.set(uiId, rEvent);
        rButton.start(rEvent.time);
        ++rButton.m_stMaxToucheCount;
    }
    else
    {
        EMP_LOG_LOUSY("Dropping #0", emp::tim::time_c::to_seconds(time_c::sub(oTime, m_vMouseQueue.first_ref().time).m_ui64usec));
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::mouse_released(c_ui64 a_uiTouch, c_button_e a_eButton, c_f64 a_dX, c_f64 a_dY)
{
    auto& rButton = m_avTouches.ref(enum_to(a_eButton));
    size_t& rstReleaseCount = rButton.m_stReleaseCount;

    //if (rButton.m_bDiscard == false)
    {
        if (rButton.m_mTouches.get(a_uiTouch).not_end())
        {
            rButton.m_bDiscard = true;

            if (rstReleaseCount > 0)
            {
                event_s& rEvent = m_vMouseQueue.push();
                rEvent.time = EMP_CLOCK.application_current();
                rEvent.type = event_e::MouseReleased;

                c_v2f v2fPosition = coordinates(a_dX, a_dY);

                mouse_event_s oMouse;
                oMouse.m_eButton = a_eButton;
                oMouse.m_v2fPosition = v2fPosition;
                oMouse.m_uiId = a_uiTouch;
                auto const it = rButton.m_mTouches.get(a_uiTouch);
                oMouse.m_v2fPrevious = it.value()->mouse.m_v2fPosition;

                rEvent.mouse = oMouse;

                fill_event(rEvent);
            }
            else
            { EMP_LOG_LOUSY("mouse_a::process_button_released : discarding event"); }
        }
    }

    if (rstReleaseCount > 0)
    {
        --rstReleaseCount;
        if (rstReleaseCount == 0)
        {
            rButton.release();
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void application_c::mouse_move(c_ui64 a_uiTouch, c_button_e a_eButton, c_f64 a_dX, c_f64 a_dY)
{
    auto const l_mouse_move = [this, a_dX, a_dY](tap_event_s& a_rButton, c_ui64 a_ui64Id)
    {
        //if (a_rButton.m_bDiscard)
        //{ return; }

        event_s& rEvent = m_vMouseQueue.push();
        rEvent.time = EMP_CLOCK.application_current();
        rEvent.type = event_e::MouseMoved;

        c_v2f v2fPosition = coordinates(a_dX, a_dY);

        mouse_event_s oMouse;
        oMouse.m_eButton = a_rButton.m_eButton;
        oMouse.m_v2fPosition = v2fPosition;
        oMouse.m_uiId = a_ui64Id;
        rEvent.mouse = oMouse;
        
        fill_event(rEvent);
    };

    bool bAdded = false;
    if (a_eButton == button_e::Invalid)
    {
        c_size stCount = m_avTouches.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            tap_event_s& rButton = m_avTouches.ref(stIndex);
            if (rButton.m_eButton != button_e::Invalid)
            {
                for (auto it = rButton.m_mTouches.begin(); it.not_end(); ++it)
                {
                    auto const eType = it.value()->type;
                    if (eType == event_e::MousePressed ||
                        eType == event_e::MouseMoved ||
                        eType == event_e::MouseDragged)
                    {
                        l_mouse_move(m_avTouches.ref(stIndex), it.value()->mouse.m_uiId);
                        bAdded = true;
                        //break;
                    }
                }
            }
        }
    }
    if (bAdded == false)
    { l_mouse_move(m_avTouches.ref(enum_to(a_eButton)), a_uiTouch); }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

