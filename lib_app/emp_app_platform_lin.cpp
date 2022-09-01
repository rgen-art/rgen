//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_app_platform_lin.h"

#include "emp_app_application.h"
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_OS_LIN

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/sync.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <gl.h>
#include <glx.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <X11/Xlib.h>
#include <X11/extensions/sync.h>
#include <X11/keysym.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, platform_c, public, platform_i)
private:
    static platform_c* s_pInstance;
public:
    #define EMP_PLATFORM platform_c::instance()
    static EMP_RETURN platform_c* new_instance(void);
    static EMP_RETURN platform_c& instance(void) { return *s_pInstance; }

    platform_c(void) {}

private:
    XIM m_pXim = 0;
    XIC m_pXic = 0;

    GLXFBConfig* m_pConfigurations = nullptr;
    GLXContext m_pContext = nullptr;
    Window m_pWindow = 0;
    Display* m_pDisplay = nullptr;
    long m_lEventMask = 0;
    Atom m_oWmDeleteWindow = 0;
    Atom m_oWmSyncRequest = 0;
    Atom m_oWmSyncRequestCounter = 0;
    Atom m_oWmProtocols = 0;
    XSyncCounter m_oXSyncCounter = 0;
    XSyncValue m_oXSyncValue = XSyncValue();
    XVisualInfo* m_pVisualInfo = nullptr;
    array<key_e, 256, true> m_aKeys = array<key_e, 256, true>();
    astring m_sResourcePath = astring();

private:
    EMP_RETURN bool resource_path(c_astring& a_sValue)
    {
        m_sResourcePath = a_sValue;
        return true;
    }

    EMP_RETURN c_astring& resource_path(void) const { return m_sResourcePath; }

    EMP_RETURN bool run(void)
    {
        EMP_APPLICATION.add(emp::app::event_e::AppCreate);
        EMP_APPLICATION.add(emp::app::event_e::AppParse);
        // @@0 remove EMP_RET_FALSE_IF_NOT(loop());
        // @@0 remove EMP_RET_FALSE_IF(state() != emp::app::state_e::Parsed);
        EMP_APPLICATION.add(emp::app::event_e::AppReflect);
        EMP_APPLICATION.add(emp::app::event_e::AppInit);
        EMP_APPLICATION.add(emp::app::event_e::AppDeserialize);

        while(true)
        {
            EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.loop());
        }

        return true;
    }

    EMP_RETURN bool create(void)
    {
        c_key_e eKey = key_e::none;
        switch (eKey)
        {
            case emp::app::key_e::none:
            #define XTMP_KEY(x_Value, x_Name)\
            case emp::app::key_e::x_Name:\
            { m_aKeys.set(x_Value, emp::app::key_e::x_Name); }
            XTMP_KEY( 23  , Tabulation   )
            XTMP_KEY( 65  , Space        )
            XTMP_KEY( 36  , Enter        )
            XTMP_KEY( 113 , Left         )
            XTMP_KEY( 114 , Right        )
            XTMP_KEY( 111 , Up           )
            XTMP_KEY( 116 , Down         )
            XTMP_KEY( 9   , Escape       )
            XTMP_KEY( 22  , Backspace    )
            XTMP_KEY( 0   , Delete       ) // @@0 fail
            XTMP_KEY( 64  , LeftAlt      )
            XTMP_KEY( 108 , RightAlt     )
            XTMP_KEY( 66  , LeftShift    )
            XTMP_KEY( 62  , RightShift   )
            XTMP_KEY( 37  , LeftCtrl     )
            XTMP_KEY( 37  , RightCtrl    ) // @@0 fail
            XTMP_KEY( 133 , LeftCommand  )
            XTMP_KEY( 134 , RightCommand )
            XTMP_KEY( 61  , Slash        )
            XTMP_KEY( 60  , Period       )
            XTMP_KEY( 59  , Comma        )
            XTMP_KEY( 58  , Alpha_M      )
            XTMP_KEY( 57  , Alpha_N      )
            XTMP_KEY( 56  , Alpha_B      )
            XTMP_KEY( 55  , Alpha_V      )
            XTMP_KEY( 54  , Alpha_C      )
            XTMP_KEY( 53  , Alpha_X      )
            XTMP_KEY( 52  , Alpha_Z      )
            XTMP_KEY( 47  , SemiColon    )
            XTMP_KEY( 46  , Alpha_L      )
            XTMP_KEY( 45  , Alpha_K      )
            XTMP_KEY( 44  , Alpha_J      )
            XTMP_KEY( 43  , Alpha_H      )
            XTMP_KEY( 42  , Alpha_G      )
            XTMP_KEY( 41  , Alpha_F      )
            XTMP_KEY( 40  , Alpha_D      )
            XTMP_KEY( 39  , Alpha_S      )
            XTMP_KEY( 38  , Alpha_A      )
            XTMP_KEY( 33  , Alpha_P      )
            XTMP_KEY( 32  , Alpha_O      )
            XTMP_KEY( 31  , Alpha_I      )
            XTMP_KEY( 30  , Alpha_U      )
            XTMP_KEY( 29  , Alpha_Y      )
            XTMP_KEY( 28  , Alpha_T      )
            XTMP_KEY( 27  , Alpha_R      )
            XTMP_KEY( 26  , Alpha_E      )
            XTMP_KEY( 25  , Alpha_W      )
            XTMP_KEY( 24  , Alpha_Q      )
            XTMP_KEY( 19  , Num_0        )
            XTMP_KEY( 18  , Num_9        )
            XTMP_KEY( 17  , Num_8        )
            XTMP_KEY( 16  , Num_7        )
            XTMP_KEY( 15  , Num_6        )
            XTMP_KEY( 14  , Num_5        )
            XTMP_KEY( 13  , Num_4        )
            XTMP_KEY( 12  , Num_3        )
            XTMP_KEY( 11  , Num_2        )
            XTMP_KEY( 10  , Num_1        )
            #undef XTMP_KEY
            default:
            { break; }
        };

        EMP_LOG_INFO("Opening x11 display");
        auto const& pDisplay = XOpenDisplay(nullptr);
        m_pDisplay = pDisplay;
        EMP_RET_FALSE_IF(pDisplay == nullptr);

        int siGlxMajor = 0;
        int siGlxMinor = 0;
        EMP_RET_FALSE_IF_NOT(glXQueryVersion(pDisplay, &siGlxMajor, &siGlxMinor));

        EMP_LOG_INFO("Glx Version : #0.#1", siGlxMajor, siGlxMinor);
        EMP_RET_FALSE_IF((1 > siGlxMajor) || ((1 == siGlxMajor) && (3 > siGlxMinor)),
            "Invalid glx version : #0.#1, required >= 1.3", siGlxMajor, siGlxMinor);

        configuration_s const& rConfiguration = EMP_APPLICATION.configuration();

        emp::cnt::vector<int> vAttributes;

        vAttributes.add(GLX_BUFFER_SIZE);
        int siBufferSize = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.buffer_size, siBufferSize));
        vAttributes.add(siBufferSize);

        vAttributes.add(GLX_LEVEL);
        vAttributes.add(0);

        vAttributes.add(GLX_DOUBLEBUFFER);
        vAttributes.add(rConfiguration.double_buffer);

        vAttributes.add(GLX_STEREO);
        vAttributes.add(rConfiguration.stereo);

        vAttributes.add(GLX_AUX_BUFFERS);
        vAttributes.add(0);

        {
            vAttributes.add(GLX_RED_SIZE);
            int siBpp = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.red_bpp, siBpp));
            vAttributes.add(siBpp);
        }

        {
            vAttributes.add(GLX_GREEN_SIZE);
            int siBpp = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.green_bpp, siBpp));
            vAttributes.add(siBpp);
        }

        {
            vAttributes.add(GLX_BLUE_SIZE);
            int siBpp = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.blue_bpp, siBpp));
            vAttributes.add(siBpp);
        }

        {
            vAttributes.add(GLX_ALPHA_SIZE);
            int siBpp = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.alpha_bpp, siBpp));
            vAttributes.add(siBpp);
        }

        {
            vAttributes.add(GLX_DEPTH_SIZE);
            int siBpp = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.depth_bpp, siBpp));
            vAttributes.add(siBpp);
        }

        {
            vAttributes.add(GLX_STENCIL_SIZE);
            int siBpp = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.stencil_bpp, siBpp));
            vAttributes.add(siBpp);
        }

        vAttributes.add(GLX_ACCUM_RED_SIZE);
        vAttributes.add(0);

        vAttributes.add(GLX_ACCUM_GREEN_SIZE);
        vAttributes.add(0);

        vAttributes.add(GLX_ACCUM_BLUE_SIZE);
        vAttributes.add(0);

        vAttributes.add(GLX_ACCUM_ALPHA_SIZE);
        vAttributes.add(0);

        vAttributes.add(GLX_RENDER_TYPE);
        vAttributes.add(GLX_RGBA_BIT);

        vAttributes.add(GLX_DRAWABLE_TYPE);
        vAttributes.add(GLX_WINDOW_BIT);

        vAttributes.add(GLX_X_RENDERABLE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));
        //vAttributes.add(True);

        vAttributes.add(GLX_X_VISUAL_TYPE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));
        //vAttributes.add(GLX_TRUE_COLOR);

        vAttributes.add(GLX_CONFIG_CAVEAT);
        vAttributes.add(GLX_NONE);

        vAttributes.add(GLX_TRANSPARENT_TYPE);
        vAttributes.add(GLX_NONE);

        //vAttributes.add(GLX_SAMPLE_BUFFERS);
        //vAttributes.add(1);

        //vAttributes.add(GLX_SAMPLES);
        //vAttributes.add(4);

        vAttributes.add(GLX_TRANSPARENT_INDEX_VALUE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));

        vAttributes.add(GLX_TRANSPARENT_RED_VALUE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));

        vAttributes.add(GLX_TRANSPARENT_GREEN_VALUE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));

        vAttributes.add(GLX_TRANSPARENT_BLUE_VALUE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));

        vAttributes.add(GLX_TRANSPARENT_ALPHA_VALUE);
        vAttributes.add(static_cast<int>(GLX_DONT_CARE));

        EMP_XX_IF_LOG(dump_configuration(vAttributes);)

        int siCount = 0;

        auto const& pScreen = screen();

        auto const& pConfigurations = glXChooseFBConfig(pDisplay, pScreen, vAttributes.ptr(0), &siCount);
        m_pConfigurations = pConfigurations;
        EMP_XX_IF_LOG(dump_configurations(vAttributes, pDisplay, pScreen);)
        EMP_RET_FALSE_IF(pConfigurations == nullptr || siCount <= 0);

        auto const& pContext = glXCreateNewContext(pDisplay, pConfigurations[0], GLX_RGBA_TYPE, 0, True);
        m_pContext = pContext;
        EMP_RET_FALSE_IF(pContext == nullptr);

        EMP_XX_IF_LOG(c_bool bDRC = glXIsDirect(pDisplay, pContext);)
        EMP_LOG_INFO("Direct rendering context : #0", bDRC);

        m_pVisualInfo = glXGetVisualFromFBConfig(pDisplay, pConfigurations[0]);
        EMP_RET_FALSE_IF(m_pVisualInfo == nullptr);

       return true;

        /*
           if ((siRenderType & GLX_RGBA_BIT) == 0)
           if ((siDrawType&GLX_WINDOW_BIT)==0&&(GLX_PBUFFER_BIT & siDrawType) == 0)
           if (siXColorType != GLX_TRUE_COLOR && siXColorType!=GLX_DIRECT_COLOR &&
           siXColorType != GLX_PSEUDO_COLOR && siXColorType != GLX_STATIC_COLOR)
           if (siXCaveat != GLX_NONE)
           if (siTransparentType != GLX_NONE)
           if (siTransparentIndexValue != 0) {
           if (siTransparentRedValue != 0) {
           if (siTransparentGreenValue != 0) {
           if (siTransparentBlueValue != 0) {
           if (siTransparentAlphaValue != 0) {
         */
    }

    EMP_RETURN bool parse(void)
    {
        configuration_s const& rConfig = EMP_APPLICATION.configuration();

        f64 dX = rConfig.x;
        f64 dY = rConfig.y;
        f64 dW = rConfig.width;
        f64 dH = rConfig.height;
        if (rConfig.fullscreen)
        {
            dX = 0;
            dY = 0;
            dW = screen_point_width();
            dH = screen_point_height();
        }

        int siX = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dX, siX));

        int siY = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dY, siY));

        ui32 uiWidth = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dW, uiWidth));

        ui32 uiHeight = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dH, uiHeight));

        EMP_LOG_INFO("Creating window : position (#0, #1) : size (#2, #3)", siX, siY, uiWidth, uiHeight);

        auto const& pDisplay = m_pDisplay;
        auto const& pRoot = root();
        auto* const pVisualInfo = m_pVisualInfo;

        XSetWindowAttributes oSWA;
        oSWA.colormap = XCreateColormap(pDisplay, pRoot, pVisualInfo->visual, AllocNone);
        oSWA.background_pixmap = None;
        oSWA.border_pixel = 0;
        oSWA.event_mask = StructureNotifyMask | SubstructureNotifyMask;

        auto const& pWindow = XCreateWindow(
                pDisplay,
                pRoot,
                siX, siY,
                uiWidth, uiHeight,
                1,
                pVisualInfo->depth,
                InputOutput,
                pVisualInfo->visual,
                CWBorderPixel | CWColormap | CWEventMask,
                &oSWA);
        m_pWindow = pWindow;
        EMP_RET_FALSE_IF(pWindow == 0);

        astring sTitle(rConfig.title);
        XStoreName(pDisplay, pWindow, sTitle.cstr());

        XFree(pVisualInfo);

        select_event(ExposureMask, true);
        select_event(StructureNotifyMask, true);
        select_event(SubstructureNotifyMask, true);
        select_event(FocusChangeMask, true);
        select_event(KeyPressMask, true);
        select_event(KeyReleaseMask, true);
        select_event(ButtonPressMask, true);
        select_event(ButtonReleaseMask, true);
        select_event(PointerMotionMask, true);
        select_event(EnterWindowMask, true);
        select_event(LeaveWindowMask, true);

        m_oWmProtocols = XInternAtom(pDisplay, "WM_PROTOCOLS", False);
        EMP_RET_FALSE_IF(m_oWmProtocols == None);
        EMP_RET_FALSE_IF(XSetWMProtocols(pDisplay, m_pWindow, &m_oWmProtocols, 1) == 0);

        m_oWmDeleteWindow = XInternAtom(pDisplay, "WM_DELETE_WINDOW", False);
        EMP_RET_FALSE_IF(m_oWmDeleteWindow == None);
        EMP_RET_FALSE_IF(XSetWMProtocols(pDisplay, m_pWindow, &m_oWmDeleteWindow, 1) == 0);

        //m_oWmSyncRequest = XInternAtom(pDisplay, "_NET_WM_SYNC_REQUEST", False);
        //EMP_RET_FALSE_IF(m_oWmSyncRequest == None);

        //m_oWmSyncRequestCounter = XInternAtom(pDisplay, "_NET_WM_SYNC_REQUEST_COUNTER", False);
        //EMP_RET_FALSE_IF(m_oWmSyncRequestCounter == None);

        //EMP_RET_FALSE_IF(XInternAtom(pDisplay, "_NET_SUPPORTED", False) == None);

        //emp::cnt::vector<Atom> vAtoms;


        //vAtoms.add(m_oWmDeleteWindow);
        //vAtoms.add(m_oWmSyncRequest);


        //XSyncIntToValue(&m_oXSyncValue, 0);
        //m_oXSyncCounter = XSyncCreateCounter(pDisplay, m_oXSyncValue);

       // XChangeProperty(pDisplay, m_pWindow, m_oWmSyncRequestCounter, XA_CARDINAL, 32, PropModeReplace,
       //     (unsigned char*)&m_oXSyncCounter, 1);



        //Atom oAllowed = XInternAtom(pDisplay, "_NET_WM_ALLOWED_ACTIONS", False);
        /*Atom oAllowed = XInternAtom(pDisplay, "_NET_SUPPORTED", False);
        EMP_RET_FALSE_IF(oAllowed == None);

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MOVE", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_RESIZE", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MINIMIZE", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_HORZ", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_HORZ", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_HORZ", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_VERT", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_FULLSCREEN", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_CHANGE_DESKTOP", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }

        {
            Atom oAt = XInternAtom(pDisplay, "_NET_WM_ACTION_MAXIMIZE_CLOSE", False);
            EMP_RET_FALSE_IF(oAt == None);
            XChangeProperty(pDisplay, m_pWindow, oAllowed, XA_ATOM, 32, PropModeReplace, (unsigned char*)&oAt, 1);
        }*/





        m_pXim = XOpenIM(m_pDisplay, nullptr, nullptr, nullptr);
        EMP_RET_FALSE_IF(m_pXim == nullptr);

        m_pXic = XCreateIC(m_pXim, XNInputStyle, XIMPreeditNothing|XIMStatusNothing,
            XNClientWindow, pWindow, XNFocusWindow, pWindow, NULL);

        EMP_RET_FALSE_IF(m_pXic == nullptr);

        EMP_RET_FALSE_IF_NOT(show());
        return true;
     }

    EMP_RETURN bool reflect(void) { return true; } // @@0 interface
    EMP_RETURN bool deserialize(void) { return true; } // @@0 interface
    EMP_RETURN bool init(void) { return true; } // @@0 interface
    EMP_RETURN bool poll(void)
    {
        if (EMP_APPLICATION.state() != emp::app::state_e::Running)
        { return true; }

        auto const& pDisplay = m_pDisplay;
        auto const& pWindow = m_pWindow;
        while (true)
        {
            if (XEventsQueued(pDisplay, QueuedAlready) <= 0)
            {
                //EMP_LOG_INFO("noevent");
                return true;
            }

    //        EMP_LOG_INFO("EVENT");
            XEvent oXEvent;
            XNextEvent(pDisplay, &oXEvent);
            if (oXEvent.xclient.window != pWindow)
            {
     //           EMP_LOG_INFO("NO WINDOW");
                continue;
            }

            switch (oXEvent.type)
            {
                case ClientMessage:
                {
                    if (oXEvent.xclient.message_type == m_oWmProtocols &&
                        oXEvent.xclient.format == 32)
                    {
                        Atom const oAtom = emp::ast::cast<Atom>(oXEvent.xclient.data.l[0]);
                        if (oAtom == m_oWmDeleteWindow)
                        {
                            EMP_LOG_INFO("OSNTUHESONTHUESNTUHOSNETUHSONTUHSENOTHUSENTOH");
                            EMP_APPLICATION.send_exit();
                            // @@0 remove EMP_APPLICATION.add(event_e::AppExit);
                            XUnmapWindow(pDisplay, pWindow);
                            XDestroyWindow(pDisplay, pWindow);
                            return true;
                        }
                        /*else if (oAtom == m_oWmSyncRequest)
                        {
                            XSyncIntsToValue(&m_oXSyncValue, emp::ast::cast<uint>(oXEvent.xclient.data.l[2]),
                                emp::ast::cast<int>(oXEvent.xclient.data.l[3]));
                        }*/
                    }
                    break;
                }
                case ConfigureRequest:
                {
                    EMP_LOG_DEBUG("CONFIGURE_REQUEST");
                    break;
                }
                case ConfigureNotify:
                {
                    XConfigureEvent const oXCEvent = oXEvent.xconfigure;
                    event_s& rEvent = EMP_APPLICATION.push(event_e::MoveResizeWindow);

                    window_event_s& rWindow = rEvent.m_Window;
                    rWindow.m_dX = oXCEvent.x;
                    rWindow.m_dY = oXCEvent.y;
                    rWindow.m_dPointWidth = oXCEvent.width;
                    rWindow.m_dPointHeight = oXCEvent.height;
                    rWindow.m_dScale = 1;
                    rWindow.m_bBegin = true;
                    break;
                }
                case DestroyNotify:
                {
                    break;
                }
                case Expose:
                {
                    break;
                }
                case KeyPress:
                {
                    process_key(oXEvent.xkey, true);
                    break;
                }
                case KeyRelease:
                {
                    process_key(oXEvent.xkey, false);
                    break;
                }
                case ButtonPress:
                {
                    auto const& rButton = oXEvent.xbutton;
                    EMP_APPLICATION.mouse_pressed(get_button(rButton.button), rButton.x, rButton.y);
                    break;
                }
                case ButtonRelease:
                {
                    auto const& rButton = oXEvent.xbutton;
                    EMP_APPLICATION.mouse_released(get_button(rButton.button), rButton.x, rButton.y);
                    break;
                }
                case EnterNotify:
                {
                    EMP_APPLICATION.mouse_enter();
                    break;
                }
                case LeaveNotify:
                {
                    EMP_APPLICATION.mouse_leave();
                    break;
                }
                case MotionNotify:
                {
                    auto const& rMotion = oXEvent.xmotion;
                    EMP_APPLICATION.mouse_move(rMotion.x, rMotion.y);
                    break;
                }
                case FocusIn:
                {
                    EMP_APPLICATION.add(event_e::eFocusIn);
                    break;
                }
                case FocusOut:
                {
                    EMP_APPLICATION.add(event_e::eFocusOut);
                    break;
                }
                case MapNotify:
                {
                    EMP_LOG_DEBUG("MapNotify");
                    //EMP_APPLICATION.add(event_e::Active);
                    EMP_APPLICATION.add(event_e::Restore);
                    break;
                }
                case UnmapNotify:
                {
                    EMP_LOG_DEBUG("UnmapNotify");
                    //EMP_APPLICATION.add(event_e::Inactive);
                    EMP_APPLICATION.add(event_e::Minimize);
                    break;
                }
                default:
                { break; }
            };
        }
        return true;
    }

    EMP_RETURN bool present(void)
    {
        //glFinish();
        // @@0  : error check
        glXSwapBuffers(m_pDisplay, m_pWindow);
        return true;
    }

    EMP_RETURN bool exit(c_bool a_bError EMP_UNUSED) { return true; } // @@0 interface
    EMP_RETURN bool serialize(void) { return true; } // @@0 interface
    EMP_RETURN bool destroy(void)
    {
        auto const& pDisplay = m_pDisplay;

        if (m_pXic != nullptr)
        {
            XDestroyIC(m_pXic);
            m_pXic = nullptr;
        }

        if (m_pXim != nullptr)
        {
            XCloseIM(m_pXim);
            m_pXim = nullptr;
        }

        if (m_pWindow != 0)
        {
            XLowerWindow(pDisplay, m_pWindow);
            XUnmapWindow(pDisplay, m_pWindow);
            XDestroyWindow(pDisplay, m_pWindow);
            m_pWindow = 0;
        }

        if (m_pConfigurations != nullptr)
        { XFree(m_pConfigurations); }
        m_pConfigurations = nullptr;

        if (m_pContext != nullptr)
        {
            glXDestroyContext(pDisplay, m_pContext);
            m_pContext = nullptr;
        }

        if (pDisplay != nullptr)
        { XCloseDisplay(pDisplay); }
        m_pDisplay = nullptr;

        return true;
    }


    EMP_RETURN array<key_e, 256, true> const& keys(void) const { return m_aKeys; }

    EMP_RETURN bool show(void)
    {
        auto const& pDisplay = m_pDisplay;
        auto const& pWindow = m_pWindow;

        /*XSetWindowAttributes oAttributes;
        oAttributes.override_redirect = True;
        XChangeWindowAttributes(pDisplay, pWindow, CWOverrideRedirect, &oAttributes);*/

        if (EMP_APPLICATION.configuration().fullscreen)
        {
            Atom const oAtomFullScreen = XInternAtom(pDisplay, "_NET_WM_STATE_FULLSCREEN", True);
            Atom const oAtomState = XInternAtom(pDisplay, "_NET_WM_STATE", True);

            XChangeProperty(pDisplay, pWindow, oAtomState, XA_ATOM, 32, PropModeReplace, (unsigned char const*)&oAtomFullScreen, 1);
        }

        /*int siNonComposited = 1;
        XChangeProperty(pDisplay, pWindow,
        XInternAtom(pDisplay, "_HILDON_NON_COMPOSITED_WINDOW", True), XA_INTEGER, 32, PropModeReplace, (unsigned char*)&siNonComposited, 1);*/

        XMapWindow(pDisplay, pWindow);
        XRaiseWindow(pDisplay, pWindow);
        //XSetInputFocus(pDisplay, pWindow, RevertToParent, CurrentTime);

        //if (EMP_APPLICATION.configuration().fullscreen)
        //{ EMP_RET_FALSE_IF_NOT(fullscreen(true)); }
        //else
        { XSync(m_pDisplay, False); }
        return true;
    }

    EMP_RETURN bool center(void) { return false; }

    EMP_RETURN bool move_resize(c_f64 a_dX, c_f64 a_dY, c_f64 a_dW, c_f64 a_dH)
    {
        si32 siX = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dX, siX));

        si32 siY = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dY, siY));

        ui32 uiW = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dW, uiW));

        ui32 uiH = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dH, uiH));

        XMoveResizeWindow(m_pDisplay, m_pWindow, siX, siY, uiW, uiH);
        return true;
    }

    EMP_RETURN bool move(c_f64 a_dX, c_f64 a_dY)
    {
        si32 siX = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dX, siX));

        si32 siY = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dY, siY));

        XMoveWindow(m_pDisplay, m_pWindow, siX, siY);
        return true;
    }

    EMP_RETURN bool resize(c_f64 a_dW, c_f64 a_dH)
    {
        uint uiWidth = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dW, uiWidth));

        uint uiHeight = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dH, uiHeight));

        XResizeWindow(m_pDisplay, m_pWindow, uiWidth, uiHeight);
        return true;
    }

    EMP_RETURN bool fullscreen(c_bool a_bValue)
    {
        Atom const oAtomFullScreen = XInternAtom(m_pDisplay, "_NET_WM_STATE_FULLSCREEN", True);
        Atom const oAtomState = XInternAtom(m_pDisplay, "_NET_WM_STATE", True);

        XEvent oXEvent;
        emp::mem::zero(&oXEvent, sizeof(oXEvent));

        oXEvent.type                 = ClientMessage;
        oXEvent.xclient.window       = m_pWindow;
        oXEvent.xclient.message_type = oAtomState;
        oXEvent.xclient.format       = 32;
        oXEvent.xclient.data.l[0]    = a_bValue ? 1l : 0l;
        oXEvent.xclient.data.l[1]    = emp::ast::cast<long int>(oAtomFullScreen);
        oXEvent.xclient.data.l[2]    = 0l;

        XSendEvent(m_pDisplay, root(),
            False,
            SubstructureNotifyMask | SubstructureRedirectMask,
            &oXEvent);

        XSync(m_pDisplay, False);

        EMP_RET_FALSE_IF_NOT(poll());
        return true;
    }


    EMP_RETURN uint framebuffer(void) const { return 0; }
    EMP_RETURN uint renderbuffer(void) const { return 0; }

    EMP_RETURN bool current_context(void) { return set_current(m_pContext); }
    EMP_RETURN bool reset_context(void) { return set_current(nullptr); }

    EMP_RETURN bool set_swap_interval(c_int a_siValue EMP_UNUSED) const
    {
        return false;
        /* @@0 device
        if (make_current() == false)
        {
            EMP_LOG_ERROR("failed to make current context");
            return false;
        }

        if (glXIsDirect(m_pDisplay, m_pContext) == false)
        {
            EMP_LOG_ERROR("not a valid direct context");
            return false;
        }
        if (m_FctSetSwapInterval.is_valid() == false)
        { return false; }

        c_int i = m_FctSetSwapInterval(a_siValue);
        if (i == GLX_BAD_VALUE)
        {
            EMP_LOG_ERROR("Failed to set swap interval : GLX_BAD_VALUE");
            return false;
        }
        else if (i == GLX_BAD_CONTEXT)
        {
            EMP_LOG_ERROR("Failed to set swap interval : GLX_BAD_CONTEXT");
            return false;
        }
        return i == 0;*/
    }

    EMP_RETURN bool get_swap_interval(int& a_rsiValue EMP_UNUSED) const
    {
        return false;
        /* @@0 device
        if (m_FctGetSwapInterval.is_valid() == false)
        { return false; }

        a_rsiValue = m_FctGetSwapInterval();
        return true;*/
    }


    void sync(void)
    {
        //XSync(m_pDisplay, False);
        //XSyncSetCounter(m_pDisplay, m_oXSyncCounter, m_oXSyncValue);
    }

public:
    EMP_RETURN f64 screen_point_width(void) const { return DisplayWidth(m_pDisplay, screen()); }
    EMP_RETURN f64 screen_point_height(void) const { return DisplayHeight(m_pDisplay, screen()); }

    EMP_RETURN f64 window_point_width(void) const
    {
        XWindowAttributes oAttributes;
        XGetWindowAttributes(m_pDisplay, m_pWindow, &oAttributes);
        return oAttributes.width;
    }

    EMP_RETURN f64 window_point_height(void) const
    {
        XWindowAttributes oAttributes;
        XGetWindowAttributes(m_pDisplay, m_pWindow, &oAttributes);
        return oAttributes.height;
    }


    EMP_RETURN f64 view_point_width(void) const { return window_point_width(); }
    EMP_RETURN f64 view_point_height(void) const { return window_point_height(); }

    EMP_RETURN double scale_factor(void) const { return 1; }

private:
    void select_event(long const a_lEvent, c_bool a_bValue)
    {
        EMP_LOG_DEBUG("Getting window attributes");

        auto const& pDisplay = m_pDisplay;

        XWindowAttributes oCurrentAttributes;
        XGetWindowAttributes(pDisplay, m_pWindow, &oCurrentAttributes);

        // @@0 log
        //print_events(oCurrentAttributes);

        long const lCurrentMask = oCurrentAttributes.your_event_mask;
        long const lNextMask = a_bValue ?
            lCurrentMask | a_lEvent :
            lCurrentMask & (~a_lEvent);

        EMP_LOG_DEBUG("Computing mask : previous #0, next #1", INT_FMT_0H(lCurrentMask), INT_FMT_0H(lNextMask));
        EMP_LOG_DEBUG("Changing window attributes");

        long const lAttributeMask = CWEventMask;
        XSetWindowAttributes oAttributes;

        m_lEventMask = lNextMask;
        oAttributes.event_mask = lNextMask;
        XChangeWindowAttributes(pDisplay, m_pWindow, lAttributeMask, &oAttributes);
    }

    EMP_RETURN bool set_current(GLXContext a_pContext)
    {
        // @@0  : error check
        glXMakeCurrent(m_pDisplay, m_pWindow, a_pContext);
        return true;
    }

    EMP_RETURN int screen(void) const { return DefaultScreen(m_pDisplay); }
    EMP_RETURN Window root(void) const { return XRootWindow(m_pDisplay, screen()); }

    void process_key(XKeyEvent& a_rXEvent, c_bool a_bPressed)
    {
        event_s& rEvent = EMP_APPLICATION.push(emp::tim::time_c(a_rXEvent.time));
        rEvent.m_eType = a_bPressed ? event_e::eKeyPress : event_e::eKeyRelease;

        keyboard_event_s& rKeyboard = rEvent.keyboard;
        rKeyboard.m_bPressed = a_bPressed;
        rKeyboard.m_ui32Code = a_rXEvent.keycode;
        rKeyboard.m_eKey = m_aKeys.val(a_rXEvent.keycode);
        EMP_APPLICATION.keyboard_repeat(rEvent);

        if (a_bPressed)
        {
            // @@0 :buffersize
            KeySym oKeySym;
            Status oStatus;
            array<char, 64, true> aBuffer;
            Xutf8LookupString(m_pXic,&a_rXEvent, aBuffer.first_ptr(), emp::ast::cast<int>(aBuffer.size()), &oKeySym, &oStatus);

            if (oStatus == XLookupChars ||
                oStatus == XLookupBoth)
            {
                u8string su8Buffer;
                c_bool bRet = u8string::ref(reinterpret_cast<c_ui8*>(aBuffer.first_ptr()), su8Buffer);
                EMP_ASSERT(bRet);
                if (bRet)
                {
                    rKeyboard.m_ui32Char = su8Buffer.get_first_char();
                }
            }
        }

        // @@0 old if (XKeysymToKeycode(rDisplay, x_XKey) == ui32Code)
        // @@0 old { rKeyboard.set_touch(x_EmpKey); }
    }

    EMP_RETURN button_e get_button(c_uint a_siButton)
    {
        switch(a_siButton)
        {
            case Button1: { return button_e::Left; }
            case Button2: { return button_e::Center; }
            case Button3: { return button_e::Right; }
            case Button4: { return button_e::WheelUp; }
            case Button5: { return button_e::WheelDown; }
            default: { EMP_ASSERT_UNREACHABLE(); }
        };
        return button_e::none;
    }

#define XTMP_FOREACH_ATTRIBUTES()\
    XTMP_ATTRIBUTE(GLX_FBCONFIG_ID, "GLX_FB_CONFIG_ID")\
    XTMP_ATTRIBUTE(GLX_BUFFER_SIZE, "GLX_BUFFER_SIZE")\
    XTMP_ATTRIBUTE(GLX_LEVEL, "GLX_LEVEL")\
    XTMP_ATTRIBUTE(GLX_DOUBLEBUFFER, "GLX_DOUBLEBUFFER")\
    XTMP_ATTRIBUTE(GLX_STEREO, "GLX_STEREO")\
    XTMP_ATTRIBUTE(GLX_AUX_BUFFERS, "GLX_AUX_BUFFERS")\
    XTMP_ATTRIBUTE(GLX_RED_SIZE, "GLX_RED_SIZE")\
    XTMP_ATTRIBUTE(GLX_GREEN_SIZE, "GLX_GREEN_SIZE")\
    XTMP_ATTRIBUTE(GLX_BLUE_SIZE, "GLX_BLUE_SIZE")\
    XTMP_ATTRIBUTE(GLX_ALPHA_SIZE, "GLX_ALPHA_SIZE")\
    XTMP_ATTRIBUTE(GLX_DEPTH_SIZE, "GLX_DEPTH_SIZE")\
    XTMP_ATTRIBUTE(GLX_STENCIL_SIZE, "GLX_STENCIL_SIZE")\
    XTMP_ATTRIBUTE(GLX_ACCUM_RED_SIZE, "GLX_ACCUM_RED_SIZE")\
    XTMP_ATTRIBUTE(GLX_ACCUM_GREEN_SIZE, "GLX_ACCUM_GREEN_SIZE")\
    XTMP_ATTRIBUTE(GLX_ACCUM_BLUE_SIZE, "GLX_ACCUM_BLUE_SIZE")\
    XTMP_ATTRIBUTE(GLX_ACCUM_ALPHA_SIZE, "GLX_ACCUM_ALPHA_SIZE")\
    XTMP_ATTRIBUTE(GLX_RENDER_TYPE, "GLX_RENDER_TYPE")\
    XTMP_ATTRIBUTE(GLX_DRAWABLE_TYPE, "GLX_DRAWABLE_TYPE")\
    XTMP_ATTRIBUTE(GLX_X_RENDERABLE, "GLX_X_RENDERABLE")\
    XTMP_ATTRIBUTE(GLX_VISUAL_ID, "GLX_VISUAL_ID")\
    XTMP_ATTRIBUTE(GLX_X_VISUAL_TYPE, "GLX_X_VISUAL_TYPE")\
    XTMP_ATTRIBUTE(GLX_CONFIG_CAVEAT, "GLX_CONFIG_CAVEAT")\
    XTMP_ATTRIBUTE(GLX_TRANSPARENT_TYPE, "GLX_TRANSPARENT_TYPE")\
    XTMP_ATTRIBUTE(GLX_TRANSPARENT_INDEX_VALUE, "GLX_TRANSPARENT_INDEX_VALUE")\
    XTMP_ATTRIBUTE(GLX_TRANSPARENT_RED_VALUE, "GLX_TRANSPARENT_RED_VALUE")\
    XTMP_ATTRIBUTE(GLX_TRANSPARENT_GREEN_VALUE, "GLX_TRANSPARENT_GREEN_VALUE")\
    XTMP_ATTRIBUTE(GLX_TRANSPARENT_BLUE_VALUE, "GLX_TRANSPARENT_BLUE_VALUE")\
    XTMP_ATTRIBUTE(GLX_TRANSPARENT_ALPHA_VALUE, "GLX_TRANSPARENT_ALPHA_VALUE")\
    XTMP_ATTRIBUTE(GLX_MAX_PBUFFER_WIDTH, "GLX_MAX_PBUFFER_WIDTH")\
    XTMP_ATTRIBUTE(GLX_MAX_PBUFFER_HEIGHT, "GLX_MAX_PBUFFER_HEIGHT")\
    XTMP_ATTRIBUTE(GLX_MAX_PBUFFER_PIXELS, "GLX_MAX_PBUFFER_PIXELS")

#if defined EMP_XX_LOG
    void dump_configuration(emp::cnt::vector<int> const& a_rvAttributes)
    {
        c_size stCount = a_rvAttributes.count();
        if ((stCount % 2) != 0)
        {
            EMP_LOG_ERROR("invalid attribute count");
            return;
        }

        EMP_LOG_INFO("//-----------------------------------");
        EMP_LOG_INFO("// Requested GLXFBConfig : ");
        EMP_LOG_INFO("//-----------------------------------");
        c_size stElements = stCount / 2;
        for (size_t stIndex = 0; stIndex < stElements; ++stIndex)
        {
            c_int siName = a_rvAttributes.val(stIndex * 2);
            c_int siValue = a_rvAttributes.val(stIndex * 2 + 1);

            switch (siName)
            {

    #define XTMP_ATTRIBUTE(x_Index, x_Name)\
                case x_Index:\
                {\
                    EMP_LOG_INFO("#0 : #1", ALITERAL(x_Name), siValue);\
                    break;\
                }
                XTMP_FOREACH_ATTRIBUTES()
    #undef XTMP_ATTRIBUTE

                default:
                {
                    EMP_LOG_ERROR("invalid attribute id : #0", siName);
                    break;
                }
            }
        }
    }
    #endif

#ifdef EMP_XX_LOG
    void dump_configurations(emp::cnt::vector<int> const& a_rvAttributes, Display* const a_pDisplay, c_int a_siScreen)
    {
        int siCount = 0;
        GLXFBConfig const* const pConfigs = glXGetFBConfigs(a_pDisplay, a_siScreen, &siCount);
        if (pConfigs == nullptr || siCount <= 0)
        {
            EMP_LOG_ERROR("failed to retreive valid configurations");
            return;
        }

        for (int siIndex = 0; siIndex < siCount; ++siIndex)
        {
        EMP_LOG_INFO("//-----------------------------------");
        EMP_LOG_INFO("// GLXFBConfig #0 : ", siIndex);
        EMP_LOG_INFO("//-----------------------------------");
    #define XTMP_ATTRIBUTE(x_Index, x_Name)\
        {\
            int siValue = 0;\
            c_int siReturn = glXGetFBConfigAttrib(a_pDisplay, pConfigs[siIndex], x_Index, &siValue);\
            auto const& rName = ALITERAL(x_Name);\
            if (siReturn == GLX_NO_EXTENSION)\
            { EMP_LOG_ERROR("#0 : GLX_NO_EXTENSION", rName); }\
            else if (siReturn == GLX_BAD_ATTRIBUTE)\
            { EMP_LOG_ERROR("#0 : GLX_BAD_ATTRIBUTE", rName); }\
            else if (siReturn == 0)\
            { EMP_LOG_INFO("#0 : #1", rName, siValue); }\
            else\
            { EMP_LOG_ERROR("#0 : failed to get attribute value", rName); }\
        }
        XTMP_FOREACH_ATTRIBUTES()
        }

    #undef XTMP_ATTRIBUTE

        for (int siIndex = 0; siIndex < siCount; ++siIndex)
        {
        EMP_LOG_INFO("//-----------------------------------");
        EMP_LOG_INFO("// GLXFBConfig Differences #0 : ", siIndex);
        EMP_LOG_INFO("//-----------------------------------");
    #define XTMP_ATTRIBUTE(x_Index, x_Name)\
        {\
            int siValue = 0;\
            auto const& rName = ALITERAL(x_Name);\
            c_int siReturn = glXGetFBConfigAttrib(a_pDisplay, pConfigs[siIndex], x_Index, &siValue);\
            if (siReturn == GLX_NO_EXTENSION)\
            { EMP_LOG_ERROR("#0 : GLX_NO_EXTENSION", rName); }\
            else if (siReturn == GLX_BAD_ATTRIBUTE)\
            { EMP_LOG_ERROR("#0 : GLX_BAD_ATTRIBUTE", rName); }\
            else if (siReturn == 0)\
            {\
                for (size_t stIndex = 0; stIndex < a_rvAttributes.count() / 2; ++stIndex)\
                {\
                    if (a_rvAttributes.val(stIndex * 2) == x_Index)\
                    {\
                        c_int siRequestedValue = a_rvAttributes.val(stIndex * 2 + 1);\
                        if (siRequestedValue != siValue)\
                        {  EMP_LOG_INFO("#0 : requested #1 : got #2", rName, siRequestedValue, siValue); }\
                    }\
                }\
            }\
            else\
            { EMP_LOG_ERROR("#0 : failed to get attribute value", rName); }\
        }
        XTMP_FOREACH_ATTRIBUTES()
        }

    #undef XTMP_ATTRIBUTE
    }
    #undef XTMP_FOREACH_ATTRIBUTES
#endif
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
platform_c* platform_c::s_pInstance = nullptr;
EMP_RETURN platform_c* platform_c::new_instance(void)
{
    platform_c::s_pInstance = EMP_ALLOCATE(platform_c);
    return platform_c::s_pInstance;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN platform_i* new_platform(void)
{ return platform_c::new_instance(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 log
/*EMP_INTERN void print_events(XWindowAttributes const& rConfiguration);
EMP_INTERN void print_events(XWindowAttributes const& a_rAttributes)
{
    EMP_LOG_DEBUG("All event mask : 0x#0", INT_FMT_0H(a_rAttributes.all_event_masks));
    EMP_LOG_DEBUG("My event mask : 0x#0", INT_FMT_0H(a_rAttributes.your_event_mask));
    EMP_LOG_DEBUG("Do not propagate event mask : 0x#0", INT_FMT_0H(a_rAttributes.do_not_propagate_mask));
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool get_GLXFBAttribute(
  Display* const a_pDisplay, GLXFBConfig const& a_rConfig,
  c_int a_siIndex, int& a_rsiValue);
  EMP_INTERN EMP_RETURN bool get_GLXFBAttribute(
  Display* const a_pDisplay, GLXFBConfig const& a_rConfig,
  c_int a_siIndex, int& a_rsiValue)
  {
  c_int siError = glXGetFBConfigAttrib(
  a_pDisplay, a_rConfig, a_siIndex, &a_rsiValue);

  if (siError == GLX_NO_EXTENSION ||
  siError == GLX_BAD_ATTRIBUTE)
  {
  EMP_LOG_ERROR("Failed to enumerate frame buffer attributes");
  return false;
  }

  return true;
  }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool is_valid_ex(cpc_char a_szExtensions, cpc_char a_szName);
EMP_INTERN EMP_RETURN bool is_valid_ex(cpc_char a_szExtensions, cpc_char a_szName)
{
    EMP_ASSERT(a_szExtensions);
    EMP_ASSERT(a_szName);

    c_size stLen = strlen(a_szName);
    cpc_char szEnd = a_szExtensions + strlen(a_szExtensions);

    c_char* pExtensions = a_szExtensions;

    while (pExtensions < szEnd)
    {
        c_size n = strcspn(pExtensions, " ");

        if ((stLen == n) && (strncmp(a_szName, pExtensions, n) == 0))
        { return true; }

        pExtensions += (n + 1);
    };

    EMP_LOG_ERROR("Extension : #0 not found in #1", a_szName, a_szExtensions);
    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool platform_c::is_glx_extension(cpc_char a_szName) const
{
    cpc_char szExtensions = emp::ast::cast<cpc_char>(glXQueryExtensionsString(m_pDisplay, screen()));
    EMP_RET_FALSE_IF_NOT(szExtensions == nullptr)
    return is_valid_ex(szExtensions, a_szName);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool platform_c::is_glx_client_extension(cpc_char a_szName) const
{
    cpc_char szExtensions = emp::ast::cast<char const*>(glXGetClientString(m_pDisplay, GLX_EXTENSIONS));
    EMP_RET_FALSE_IF(szExtensions == nullptr)
    return is_valid_ex(szExtensions, a_szName);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool platform_c::is_gl_extension(
        char const* const a_szName) const
{
    char const* szExtensions = reinterpret_cast<char const*>(
            glGetString(GL_EXTENSIONS));
    if (szExtensions == nullptr)
    {
        EMP_LOG_ERROR("Failed to glGetString(GL_EXTENSIONS)");
        return false;
    }
    return is_valid_ex(szExtensions, a_szName);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool platform_c::check_extensions(void)
{
    if (is_glx_client_extension("GLX_MESA_swap_control"))
    {
        {
            char const* const pSwapMesa = "glXSwapIntervalMESA";
            PFnSetSwapControl pfnSwapControl =
                reinterpret_cast<PFnSetSwapControl>(glXGetProcAddress(
                    reinterpret_cast<GLubyte const* const>(pSwapMesa)));
            if (pfnSwapControl == nullptr)
            {
                EMP_LOG_ERROR("Failed to glXGetProcAddress(#0);", pSwapMesa);
                return false;
            }
            m_FctSetSwapInterval = emp::fct::function(pfnSwapControl);
        }

        {
            char const* const pSwapMesa = "glXGetSwapIntervalMESA";
            PFnGetSwapControl pfnSwapControl =
                reinterpret_cast<PFnGetSwapControl>(glXGetProcAddress(
                    reinterpret_cast<GLubyte const* const>(pSwapMesa)));
            if (pfnSwapControl == nullptr)
            {
                EMP_LOG_ERROR("Failed to glXGetProcAddress(#0);", pSwapMesa);
                return false;
            }

            m_FctGetSwapInterval = emp::fct::function(pfnSwapControl);
        }
    }
    else if (is_glx_client_extension("GLX_SGI_swap_control"))
    {
        char const* const pSwapSgi = "glXSwapIntervalSGI";
        PFnSetSwapControl pfnSwapControl =
            reinterpret_cast<PFnSetSwapControl>(glXGetProcAddress(
                        reinterpret_cast<GLubyte const* const>(pSwapSgi)));
        if (pfnSwapControl == nullptr)
        {
            EMP_LOG_ERROR("Failed to glXGetProcAddress(#0);", pSwapSgi);
            return false;
        }

        m_FctSetSwapInterval = emp::fct::function(pfnSwapControl);
    }
    else
    {
        EMP_LOG_ERROR("Failed to load vsync extension");
        return false;
    }
    //swap_interval(1);
    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

