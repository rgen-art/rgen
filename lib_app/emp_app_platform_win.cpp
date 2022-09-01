//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_app_platform_win.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_OS_WIN

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"
#include "emp_app_platform_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_fs_file.h"
#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "Windowsx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, platform_c, public, platform_i)
private:
    static platform_c* s_pInstance;
public:
    #define EMP_PLATFORM emp::app::platform_c::instance()
    static EMP_RETURN platform_c* new_instance(void);
    static EMP_RETURN platform_c& instance(void) { return *s_pInstance; }

    platform_c(void) {}

private:
    HINSTANCE m_pInstance = 0;
    HWND m_pWindow = 0;
    HGLRC m_pContext = 0;
    HDC m_pHdc = 0;
    bool m_bCreated = false;
    bool m_bEnter = false;
    bool m_bNoResize = false;
    // @@0 remove vector<uint> m_vRenderbuffers;
    // @@0 remove vector<uint> m_vFramebuffers;
    array<key_e, 256, true> m_aKeys = array<key_e, 256, true>();
    astring m_sResourcePath = astring();

    EMP_RETURN bool resource_path(c_astring& a_sValue, c_bool EMP_SILENT(a_bRelative))
    {
        c_bool bAbsolute = a_sValue.len() > 0 && a_sValue.get_first_char() == '/';
        if (bAbsolute)
        {
            m_sResourcePath = a_sValue;
            m_sResourcePath.trim_left();
        }
        else
        {
            char szResourcePath[MAX_PATH]; // @@0 remove tmp copy
            EMP_RET_FALSE_IF(GetCurrentDirectoryA(MAX_PATH, szResourcePath) == 0, "GetLastError #0", emp::win::GetLastErrorString()); // @@0 unicode

            m_sResourcePath.append_cstr(szResourcePath);
            m_sResourcePath.append_char('\\');
            m_sResourcePath.append_string(a_sValue);
        }
        return true;
    }

    EMP_RETURN c_astring& resource_path(void) const { return m_sResourcePath; }

    EMP_RETURN bool run(void)
    {
        EMP_APPLICATION.add(event_e::AppCreate);
        EMP_APPLICATION.add(event_e::AppParse);

        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.loop());

        EMP_APPLICATION.add(event_e::AppReflect);
        EMP_APPLICATION.add(event_e::AppInit);

        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.loop());

        EMP_APPLICATION.add(event_e::AppDeserialize);

        while (true)
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
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
            case emp::app::key_e::none:
            #define XTMP_KEY(x_Value, x_Name)\
            case emp::app::key_e::x_Name:\
            { m_aKeys.set(x_Value, emp::app::key_e::x_Name); }
            XTMP_KEY( VK_TAB     , Tabulation   )
            XTMP_KEY( VK_SPACE   , Space        )
            XTMP_KEY( VK_RETURN  , Enter        )
            XTMP_KEY( VK_LEFT    , Left         )
            XTMP_KEY( VK_RIGHT   , Right        )
            XTMP_KEY( VK_UP      , Up           )
            XTMP_KEY( VK_DOWN    , Down         )
            XTMP_KEY( VK_ESCAPE  , Escape       )
            XTMP_KEY( VK_BACK    , Backspace    )
            XTMP_KEY( VK_DELETE  , Delete       ) // @@0 fail
            XTMP_KEY( VK_MENU    , LeftAlt      )
            XTMP_KEY( VK_MENU    , RightAlt     )
            XTMP_KEY( VK_SHIFT   , LeftShift    )
            XTMP_KEY( VK_SHIFT   , RightShift   )
            XTMP_KEY( VK_CONTROL , LeftCtrl     )
            XTMP_KEY( VK_CONTROL , RightCtrl    ) // @@0 fail
            XTMP_KEY( VK_LWIN    , LeftCommand  )
            XTMP_KEY( VK_RWIN    , RightCommand )
            XTMP_KEY( 90  , Slash        )
            XTMP_KEY( 86  , Period       )
            XTMP_KEY( 87  , Comma        )
            XTMP_KEY( 77  , Alpha_M      )
            XTMP_KEY( 66  , Alpha_N      )
            XTMP_KEY( 88  , Alpha_B      )
            XTMP_KEY( 75  , Alpha_V      )
            XTMP_KEY( 74  , Alpha_C      )
            XTMP_KEY( 81  , Alpha_X      )
            XTMP_KEY( 186 , Alpha_Z      )
            XTMP_KEY( 83  , SemiColon    )
            XTMP_KEY( 78  , Alpha_L      )
            XTMP_KEY( 84  , Alpha_K      )
            XTMP_KEY( 72  , Alpha_J      )
            XTMP_KEY( 68  , Alpha_H      )
            XTMP_KEY( 73  , Alpha_G      )
            XTMP_KEY( 85  , Alpha_F      )
            XTMP_KEY( 69  , Alpha_D      )
            XTMP_KEY( 79  , Alpha_S      )
            XTMP_KEY( 65  , Alpha_A      )
            XTMP_KEY( 76  , Alpha_P      )
            XTMP_KEY( 82  , Alpha_O      )
            XTMP_KEY( 67  , Alpha_I      )
            XTMP_KEY( 71  , Alpha_U      )
            XTMP_KEY( 70  , Alpha_Y      )
            XTMP_KEY( 89  , Alpha_T      )
            XTMP_KEY( 80  , Alpha_R      )
            XTMP_KEY( 190 , Alpha_E      )
            XTMP_KEY( 188 , Alpha_W      )
            XTMP_KEY( 222 , Alpha_Q      )
            XTMP_KEY( 48  , Num_0        )
            XTMP_KEY( 57  , Num_9        )
            XTMP_KEY( 56  , Num_8        )
            XTMP_KEY( 55  , Num_7        )
            XTMP_KEY( 54  , Num_6        )
            XTMP_KEY( 53  , Num_5        )
            XTMP_KEY( 52  , Num_4        )
            XTMP_KEY( 51  , Num_3        )
            XTMP_KEY( 50  , Num_2        )
            XTMP_KEY( 49  , Num_1        )
            #undef XTMP_KEY
            default:
            { break; }
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
        };
        return true;
    }
    EMP_RETURN bool parse(void)
    {
        m_pInstance = GetModuleHandle(nullptr);

        auto const& rConfiguration = EMP_APPLICATION.configuration();

        WNDCLASS oWndClass;
        memset(&oWndClass, 0, sizeof(oWndClass));
        oWndClass.lpfnWndProc = &platform_c::wnd_proc;
        oWndClass.hInstance = m_pInstance;
        oWndClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
        oWndClass.lpszClassName = L"rgen";
        oWndClass.style = CS_OWNDC;

        EMP_RET_FALSE_IF_NOT(RegisterClass(&oWndClass), "GetLastError #0", emp::win::GetLastErrorString());

        RECT oRectangle;
        si32 si32ClientWidth = 0;
        si32 si32ClientHeight = 0;
        si32 si32FrameWidth = 0;
        si32 si32FrameHeight = 0;
        DWORD dwStyle = 0;

        EMP_RET_FALSE_IF_NOT(compute_window_rect(oRectangle, false, emp::ast::cast<si32>(rConfiguration.width), emp::ast::cast<si32>(rConfiguration.height), si32ClientWidth, si32ClientHeight, si32FrameWidth, si32FrameHeight, dwStyle));

        // @@0  : unicode
        m_pWindow = CreateWindowW(
            oWndClass.lpszClassName, // @@0 rConfiguration.title,
            oWndClass.lpszClassName, // @@0 rConfiguration.title,
            dwStyle,
            oRectangle.left, oRectangle.top, si32FrameWidth, si32FrameHeight,
            0, 0, m_pInstance, this);

        EMP_RET_FALSE_IF(m_pWindow == nullptr, "GetLastError #0", emp::win::GetLastErrorString());

        MSG oMessage;
        while (GetMessage(&oMessage, NULL, 0, 0) > 0 && m_bCreated == false)
        {
            DispatchMessage(&oMessage);
        }

        PIXELFORMATDESCRIPTOR oPixelFormat;

        oPixelFormat.nSize = emp::ast::cast<WORD>(sizeof(PIXELFORMATDESCRIPTOR));
        oPixelFormat.nVersion = 1;
        oPixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        oPixelFormat.iPixelType = PFD_TYPE_RGBA;
        oPixelFormat.cColorBits = emp::ast::cast<BYTE>(rConfiguration.buffer_size);
        oPixelFormat.cRedBits = emp::ast::cast<BYTE>(rConfiguration.red_bpp);
        oPixelFormat.cRedShift = 0;
        oPixelFormat.cGreenBits = emp::ast::cast<BYTE>(rConfiguration.green_bpp);
        oPixelFormat.cGreenShift = 0;
        oPixelFormat.cBlueBits = emp::ast::cast<BYTE>(rConfiguration.blue_bpp);
        oPixelFormat.cBlueShift = 0;
        oPixelFormat.cAlphaBits = emp::ast::cast<BYTE>(rConfiguration.alpha_bpp);
        oPixelFormat.cAlphaShift = 0;
        oPixelFormat.cAccumBits = 0;
        oPixelFormat.cAccumRedBits = 0;
        oPixelFormat.cAccumGreenBits = 0;
        oPixelFormat.cAccumBlueBits = 0;
        oPixelFormat.cAccumAlphaBits = 0;
        oPixelFormat.cDepthBits = emp::ast::cast<BYTE>(rConfiguration.depth_bpp);
        oPixelFormat.cStencilBits = emp::ast::cast<BYTE>(rConfiguration.stencil_bpp);
        oPixelFormat.cAuxBuffers = 0;
        oPixelFormat.iLayerType = PFD_MAIN_PLANE;
        oPixelFormat.bReserved = 0;
        oPixelFormat.dwLayerMask = 0;
        oPixelFormat.dwVisibleMask = 0;
        oPixelFormat.dwDamageMask = 0;

        m_pHdc = GetDC(m_pWindow);

        // @@0  : remove -lgdi32
        c_int siPixelFormat = ChoosePixelFormat(m_pHdc, &oPixelFormat);

        SetPixelFormat(m_pHdc, siPixelFormat, &oPixelFormat);

        m_pContext = wglCreateContext(m_pHdc); 

        EMP_RET_FALSE_IF_NOT(current_context());
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
         (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

        if (m_pContext != nullptr)
        {
            wglDeleteContext(m_pContext);
        }

        const int attribs [] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                                WGL_CONTEXT_MINOR_VERSION_ARB, 2,
                                0 };

        m_pContext = wglCreateContextAttribsARB(m_pHdc, 0, attribs);
        EMP_RET_FALSE_IF_NOT(show());

        return true;
    }

    EMP_RETURN bool reflect(void) { return true; } // @@0 interface;
    EMP_RETURN bool deserialize(void) { return true; } // @@0 interface;
    EMP_RETURN bool init(void) { return true; } // @@0 interface
    EMP_RETURN bool poll(void)
    {
        MSG oMessage;
        while (PeekMessage(&oMessage, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&oMessage);
            DispatchMessage(&oMessage);
        }

        if (oMessage.message == WM_QUIT)
        {
            EMP_APPLICATION.send_exit();
            // @@0 remove EMP_APPLICATION.add(event_e::AppExit);
        }

        return true;
    }

    EMP_RETURN bool present(void)
    {
        return SwapBuffers(m_pHdc) == TRUE;
    }

    EMP_RETURN bool exit(c_bool EMP_SILENT(a_bError)) { return true; } // @@0 interface
    EMP_RETURN bool serialize(void) { return true; } // @@0 interface
    EMP_RETURN bool destroy(void) 
    {
        m_bCreated = false;

        if (m_pHdc != nullptr)
        {
            ReleaseDC(m_pWindow, m_pHdc);
        }
        m_pHdc = nullptr;

        if (m_pContext != nullptr)
        {
            wglDeleteContext(m_pContext);
        }
        m_pContext = nullptr;
        return true;
    }

    EMP_RETURN array<key_e, 256, true> const& keys(void) const { return m_aKeys; }

    EMP_RETURN bool show(void)
    {
        ShowWindow(m_pWindow, SW_SHOW);
        return true;
    }

    EMP_RETURN bool center(void) { return false; } // @@0 interface

    EMP_RETURN bool move_resize(c_f64 a_dX, c_f64 a_dY, c_f64 a_dW, c_f64 a_dH)
    { return move_resize(a_dX, a_dY, a_dW, a_dH, 0); }

    EMP_RETURN bool move_resize(c_f64 EMP_SILENT(a_dX), c_f64 EMP_SILENT(a_dY), c_f64 a_dW, c_f64 a_dH, c_uint a_uiFlags) // @@0 factorize
    {
        //si32 siX = 0;
        //EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dX, siX));

        //si32 siY = 0;
        //EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dY, siY));

        si32 si32TargetW = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dW, si32TargetW));

        si32 si32TargetH = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dH, si32TargetH));

        RECT oRect;
        si32 si32ClientWidth = 0;
        si32 si32ClientHeight = 0;
        si32 si32FrameWidth = 0;
        si32 si32FrameHeight = 0;
        DWORD dwStyle = 0;

        EMP_RET_FALSE_IF_NOT(compute_window_rect(oRect, false, si32TargetW, si32TargetH, si32ClientWidth, si32ClientHeight, si32FrameWidth, si32FrameHeight, dwStyle));
        EMP_RET_FALSE_IF_NOT(SetWindowPos(m_pWindow, 0, oRect.left, oRect.top, si32FrameWidth, si32FrameHeight, a_uiFlags), "GetLastError #0", emp::win::GetLastErrorString());
        return true;
    }

public:
    EMP_RETURN bool move(c_f64 a_dX, c_f64 a_dY) // @@0 factorize
    { return move_resize(a_dX, a_dY, 0, 0, SWP_NOSIZE); }

    EMP_RETURN bool resize(c_f64 a_dW, c_f64 a_dH) // @@0 interface // @@0 factorize
    { return move_resize(0, 0, a_dW, a_dH, SWP_NOMOVE); }

    EMP_RETURN bool fullscreen(c_bool EMP_SILENT(a_bValue)) // @@0 unused 
    {
        RECT oRect;
        si32 si32ClientWidth = 0;
        si32 si32ClientHeight = 0;
        si32 si32FrameWidth = 0;
        si32 si32FrameHeight = 0;
        DWORD dwStyle = 0;
        auto const& rConfig = EMP_APPLICATION.configuration();
        EMP_RET_FALSE_IF_NOT(compute_window_rect(oRect, rConfig.fullscreen == false && rConfig.maximized, emp::ast::cast<si32>(rConfig.width), emp::ast::cast<si32>(rConfig.height), si32ClientWidth, si32ClientHeight, si32FrameWidth, si32FrameHeight, dwStyle));

        LONG_PTR lStyle = GetWindowLongPtr(m_pWindow, GWL_STYLE);
        EMP_RET_FALSE_IF(lStyle == 0, "GetLastError #0", emp::win::GetLastErrorString());

        m_bNoResize = true; // @@0 fuck
        EMP_RET_FALSE_IF(SetWindowLongPtr(m_pWindow, GWL_STYLE, dwStyle) == 0,
            "GetLastError #0", emp::win::GetLastErrorString());
        m_bNoResize = false; // @@0 fuck

        EMP_RET_FALSE_IF_NOT(SetWindowPos(m_pWindow, 0, oRect.left, oRect.top, si32FrameWidth, si32FrameHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW),
            "GetLastError #0", emp::win::GetLastErrorString());

        //if (a_bValue)
        //{
        //    DEVMODE dmScreenSettings;                   // Device Mode
        //    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));       // Makes Sure Memory's Cleared
        //    dmScreenSettings.dmSize = sizeof(dmScreenSettings);       // Size Of The Devmode Structure
        //    dmScreenSettings.dmPelsWidth = si32ClientWidth;            // Selected Screen Width
        //    dmScreenSettings.dmPelsHeight = si32ClientHeight;           // Selected Screen Height
        //    dmScreenSettings.dmBitsPerPel = 32;             // Selected Bits Per Pixel
        //    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        //    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        //    {
        //        // @@0 error
        //    }
        //}
        //else
        //{
        //    ChangeDisplaySettings(NULL, 0); // @@0 error
        //}
        return true;
    }

    EMP_RETURN uint framebuffer(void) const { return 0; }
    EMP_RETURN uint renderbuffer(void) const { return 0; }

public:
    void output(cpc_char a_szOutput) const
    {
        puts(a_szOutput);
    }

    EMP_RETURN bool current_context(void)
    {
        return set_current(m_pContext);
    }

    EMP_RETURN bool reset_context(void)
    {
        return set_current(nullptr);
    }


    EMP_RETURN bool set_swap_interval(c_int EMP_SILENT(a_siValue)) const
    {
        /* @@0 device
        if (EMP_GRAPHICS.set_swap_interval(a_siValue) == false)
        {
        EMP_LOG_INFO("Failed to set swap interval");
        DWORD const dwError = GetLastError();
        if (dwError != 0)
        {
        astring sError;
        GetLastErrorString(dwError, sError);
        EMP_LOG_ERROR("GetLastErrorString : #0", sError);
        return false;
        }

        return false;
        }*/

        return true;
    }

    EMP_RETURN bool get_swap_interval(int& EMP_SILENT(a_rsiValue)) const
    {
        /* @@0 device
        a_rsiValue = EMP_GRAPHICS.get_swap_interval();
        DWORD const dwError = GetLastError();
        if (dwError != 0)
        {
        astring sError;
        GetLastErrorString(dwError, sError);
        EMP_LOG_ERROR("GetLastErrorString : #0", sError);
        return false;
        }*/

        return true;
    }

    void sync(void) {} // @@0 interface

    EMP_RETURN f64 screen_point_width(void) const
    {
        HWND oHwnd = GetDesktopWindow();
        RECT oRect;
        GetWindowRect(oHwnd, &oRect);
        c_int siWidth = oRect.right - oRect.left;
        return siWidth;
    }

    EMP_RETURN f64 screen_point_height(void) const
    {
        HWND oHwnd = GetDesktopWindow();
        RECT oRect;
        GetWindowRect(oHwnd, &oRect);
        c_int siHeight = oRect.bottom - oRect.top;
        return siHeight;
    }

    EMP_RETURN f64 window_point_width(void) const
    {
        RECT oRect;
        GetWindowRect(m_pWindow, &oRect);
        c_int siWidth = oRect.right - oRect.left;
        return siWidth;
    }

    EMP_RETURN f64 window_point_height(void) const
    {
        RECT oRect;
        GetWindowRect(m_pWindow, &oRect);
        c_int siHeight = oRect.bottom - oRect.top;
        return siHeight;
    }

    EMP_RETURN f64 view_point_width(void) const
    {
        RECT oRect;
        GetClientRect(m_pWindow, &oRect);
        c_int siWidth = oRect.right - oRect.left;
        return siWidth;
    }

    EMP_RETURN f64 view_point_height(void) const
    {
        RECT oRect;
        GetClientRect(m_pWindow, &oRect);
        c_int siHeight = oRect.bottom - oRect.top;
        return siHeight;
    }

    EMP_RETURN double scale_factor(void) const { return 1; }

    static LRESULT CALLBACK wnd_proc(HWND hWnd, uint message, WPARAM wParam, LPARAM lParam)
    {
#define XTMP_THIS reinterpret_cast<platform_c*>(GetWindowLongPtr(hWnd, GWLP_USERDATA))

        switch (message)
        {
        case WM_CREATE:
        {
            EMP_LOG_LOUSY("WM_CREATE");
            CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            platform_c* pThis = reinterpret_cast<platform_c*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            pThis->m_bCreated = true;
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_QUIT:
        {
            // @@0 remove EMP_APPLICATION.add(event_e::AppExit);
            EMP_APPLICATION.send_exit();
            break;
        }
        case WM_CHAR:
        {
            XTMP_THIS->process_wchar(wParam, lParam);
            break;
        }
        case WM_COMMAND:
        {
            EMP_LOG_LOUSY("WM_COMMAND");
            break;
        }
        case WM_SYSCOMMAND:
        {
            EMP_LOG_LOUSY("WM_SYSCOMMAND");
#define XTMP_CASE(x_Case)\
            case x_Case:\
            {\
                EMP_LOG_LOUSY(EMP_PP_STRING(x_Case));\
                return 0;\
            }
            switch (wParam)
            {
                //XTMP_CASE(SC_CLOSE)
                //XTMP_CASE(SC_CONTEXTHELP)
                //XTMP_CASE(SC_DEFAULT)
                //XTMP_CASE(SC_HOTKEY)
                //XTMP_CASE(SC_HSCROLL)
                XTMP_CASE(SC_KEYMENU)
                    //XTMP_CASE(SC_MAXIMIZE)
                    //XTMP_CASE(SC_MINIMIZE)
                    //XTMP_CASE(SC_MONITORPOWER)
                    //XTMP_CASE(SC_MOUSEMENU)
                    //XTMP_CASE(SC_MOVE)
                    //XTMP_CASE(SC_NEXTWINDOW)
                    //XTMP_CASE(SC_PREVWINDOW)
            case SC_RESTORE:
                {
                    EMP_LOG_LOUSY("SC_RESTORE");
                    EMP_APPLICATION.add(event_e::Restore);
                    break;
                }
                //XTMP_CASE(SC_SCREENSAVE)
                //XTMP_CASE(SC_SIZE)
                //XTMP_CASE(SC_TASKLIST)
                //XTMP_CASE(SC_VSCROLL)
            case SC_MINIMIZE:
            {
                EMP_LOG_LOUSY("SC_MINIMIZE");
                EMP_APPLICATION.add(event_e::Minimize);
                break;
            }
            case SC_MAXIMIZE:
            {
                EMP_LOG_LOUSY("SC_MAXIMIZE");
                EMP_APPLICATION.add(event_e::Maximize);
                break;
            }
            default:
            { break; }
            };
#undef XTMP_CASE
            break;
        }
        case WM_ACTIVATE:
        {
            WORD wActive = LOWORD(wParam);
            switch (wActive)
            {
            case WA_ACTIVE:
            {
                EMP_LOG_LOUSY("WA_ACTIVE");
                EMP_APPLICATION.add(event_e::Active);
                break;
            }
            case WA_CLICKACTIVE:
            {
                EMP_LOG_LOUSY("WA_CLICKACTIVE");
                break;
            }
            case WA_INACTIVE:
            {
                EMP_LOG_LOUSY("WA_INACTIVE");
                EMP_APPLICATION.add(event_e::Inactive);
                break;
            }
            default:
            {
                EMP_ASSERT_UNREACHABLE();
                break;
            }
            };
            break;
        }
        case WM_SIZE:
        {
            if (XTMP_THIS->m_bNoResize == false)
            {
                event_s& rEvent = EMP_APPLICATION.push(event_e::ResizeWindow);
                window_event_s& rWindow = rEvent.window;
                rWindow.m_dPointX = 0;
                rWindow.m_dPointY = 0;
                rWindow.m_dPointWidth = LOWORD(lParam);
                rWindow.m_dPointHeight = HIWORD(lParam);
                rWindow.m_dScale = 1;
                rWindow.m_bBegin = true;
            }
            break;
        }
        case WM_MOVE:
        {
            event_s& rEvent = EMP_APPLICATION.push(event_e::MoveWindow);
            window_event_s& rWindow = rEvent.window;
            rWindow.m_dPointX = LOWORD(lParam);
            rWindow.m_dPointY = HIWORD(lParam);
            rWindow.m_dPointWidth = 0;
            rWindow.m_dPointHeight = 0;
            rWindow.m_dScale = 1;
            rWindow.m_bBegin = false;
            break;
        }
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            XTMP_THIS->process_key(wParam, lParam, true);
            break;
        }
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            XTMP_THIS->process_key(wParam, lParam, false);
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            XTMP_THIS->process_button(button_e::Left, true, lParam);
            break;
        }
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        {
            XTMP_THIS->process_button(button_e::Right, true, lParam);
            break;
        }
        case WM_LBUTTONUP:
        {
            XTMP_THIS->process_button(button_e::Left, false, lParam);
            break;
        }
        case WM_RBUTTONUP:
        {
            XTMP_THIS->process_button(button_e::Right, false, lParam);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            POINT p;
            p.x = GET_X_LPARAM(lParam);
            p.y = GET_Y_LPARAM(lParam);
            if (ScreenToClient(hWnd, &p))
            {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                c_button_e eButton = delta < 0 ? button_e::WheelDown : button_e::WheelUp;

                c_ui64 uiId = EMP_APPLICATION.new_touch_id();

                EMP_APPLICATION.mouse_pressed(uiId, eButton, p.x, p.y);
                EMP_APPLICATION.mouse_released(uiId, eButton, p.x, p.y);
            }
            break;
        }
        case WM_MOUSEMOVE:
        {
            auto const& pThis = XTMP_THIS;
            if (pThis->m_bEnter == false)
            {
                TRACKMOUSEEVENT oTrack;

                oTrack.cbSize = sizeof(TRACKMOUSEEVENT);
                oTrack.dwFlags = TME_CANCEL;
                oTrack.hwndTrack = hWnd;
                oTrack.dwHoverTime = 0;
                TrackMouseEvent(&oTrack);

                oTrack.dwFlags = TME_LEAVE;
                TrackMouseEvent(&oTrack);

                EMP_APPLICATION.mouse_enter();
                pThis->m_bEnter = true;
            }

            pThis->process_move(lParam);
            break;
        }
        case WM_MOUSELEAVE:
        {
            auto const& pThis = XTMP_THIS;
            pThis->m_bEnter = false;

            TRACKMOUSEEVENT oTrack;

            oTrack.cbSize = sizeof(TRACKMOUSEEVENT);
            oTrack.dwFlags = TME_CANCEL;
            oTrack.hwndTrack = hWnd;
            oTrack.dwHoverTime = 0;
            TrackMouseEvent(&oTrack);

            oTrack.dwFlags = TME_HOVER;
            TrackMouseEvent(&oTrack);

            EMP_APPLICATION.mouse_leave();
            break;
        }
        case WM_SETFOCUS:
        {
            EMP_APPLICATION.add(event_e::eFocusIn);
            break;
        }
        case WM_KILLFOCUS:
        {
            EMP_APPLICATION.add(event_e::eFocusOut);
            break;
        }
        default:
        {
            break;
        }
        }

#undef XTMP_THIS

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    EMP_RETURN bool set_current(HGLRC EMP_SILENT(a_pContext)) const // @@0 bug unused context
    {
        EMP_RET_FALSE_IF_NOT(wglMakeCurrent(m_pHdc, m_pContext), "GetLastError #0", emp::win::GetLastErrorString());
        return true;
    }

    EMP_RETURN DWORD compute_style(void)
    {
        auto const& rConfiguration = EMP_APPLICATION.configuration();
        DWORD dwStyle = WS_VISIBLE;
        if (rConfiguration.maximized)
        {
            dwStyle |= WS_MAXIMIZE;
        }
        if (rConfiguration.fullscreen)
        {
            dwStyle |= WS_POPUP;
        }
        else
        {
            dwStyle |= WS_OVERLAPPEDWINDOW;
        }
        return dwStyle;
    }

    EMP_RETURN bool compute_client_size(c_si32 a_si32FrameWidth, c_si32 a_si32FrameHeight, si32& a_rsi32ClientWidth, si32& a_rsi32ClientHeight)
    {
        //LONG lStyle = GetWindowLong(m_pWindow, GWL_STYLE);
        //EMP_RET_FALSE_IF(lStyle == 0, "GetLastError #0", emp::win::GetLastErrorString());

        RECT oRect;
        oRect.left = 0;
        oRect.top = 0;
        oRect.right = 0;
        oRect.bottom = 0;
        EMP_RET_FALSE_IF_NOT(AdjustWindowRect(&oRect, compute_style(), false), "GetLastError #0", emp::win::GetLastErrorString());

        a_rsi32ClientWidth = a_si32FrameWidth - oRect.right - oRect.left;
        a_rsi32ClientHeight = a_si32FrameHeight - oRect.bottom - oRect.top;
        return true;
    }

    EMP_RETURN bool compute_window_rect(RECT& a_rRect, c_bool a_bMaximized, c_si32 a_si32W, c_si32 a_si32H, si32& a_rsi32ClientWidth, si32& a_rsi32ClientHeight, si32& a_rsi32FrameWidth, si32& a_rsi32FrameHeight, DWORD& a_rdwStyle)
    {
        configuration_s const& rConfiguration = EMP_APPLICATION.configuration();

        c_si32 si32ScreenWidth = emp::ast::cast<si32>(screen_point_width());
        c_si32 si32ScreenHeight = emp::ast::cast<si32>(screen_point_height());

        a_rdwStyle = compute_style();

        if (rConfiguration.fullscreen)
        {
            a_rsi32ClientWidth = si32ScreenWidth;
            a_rsi32ClientHeight = si32ScreenHeight;
            a_rsi32FrameWidth = si32ScreenWidth;
            a_rsi32FrameHeight = si32ScreenHeight;

            a_rRect.left = 0;
            a_rRect.top = 0;

            a_rRect.right = si32ScreenWidth;
            a_rRect.bottom = si32ScreenHeight;
        }
        else if (a_bMaximized)
        {
            EMP_RET_FALSE_IF_NOT(compute_client_size(si32ScreenWidth, si32ScreenHeight, a_rsi32ClientWidth, a_rsi32ClientHeight));

            a_rRect.left = 0;
            a_rRect.right = a_rsi32ClientWidth;
            a_rRect.top = 0;
            a_rRect.bottom = a_rsi32ClientHeight;

            EMP_RET_FALSE_IF_NOT(AdjustWindowRect(&a_rRect, a_rdwStyle, false), "GetLastError #0", emp::win::GetLastErrorString());

            a_rsi32FrameWidth = a_rRect.right - a_rRect.left;
            a_rsi32FrameHeight = a_rRect.bottom - a_rRect.top;
        }
        else
        {
            a_rsi32ClientWidth = a_si32W;
            a_rsi32ClientHeight = a_si32H;

            a_rRect.left = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.x, a_rRect.left));
            a_rRect.right = a_rRect.left + a_rsi32ClientWidth;

            a_rRect.top = 0;
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(rConfiguration.y, a_rRect.top));
            a_rRect.bottom = a_rRect.top + a_rsi32ClientHeight;

            EMP_RET_FALSE_IF_NOT(AdjustWindowRect(&a_rRect, a_rdwStyle, false), "GetLastError #0", emp::win::GetLastErrorString());

            a_rsi32FrameWidth = a_rRect.right - a_rRect.left;
            a_rsi32FrameHeight = a_rRect.bottom - a_rRect.top;

            c_bool bCentered = rConfiguration.centered;
            c_float fX = (bCentered ? emp::ast::cast<float>(si32ScreenWidth - a_rsi32FrameWidth) * 0.5f : emp::ast::cast<float>(a_rRect.left));
            c_float fY = (bCentered ? emp::ast::cast<float>(si32ScreenHeight - a_rsi32FrameHeight) * 0.5f : emp::ast::cast<float>(a_rRect.top));


            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(fX, a_rRect.left)); // @@0 title bar
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(fX + emp::ast::cast<float>(a_rsi32FrameWidth), a_rRect.right));
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(fY, a_rRect.top));
            EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(fY + emp::ast::cast<float>(a_rsi32FrameHeight), a_rRect.bottom));
        }

        EMP_RET_FALSE_IF_NOT(a_rsi32ClientWidth > 0 && a_rsi32ClientHeight > 0 && a_rsi32FrameWidth > 0 && a_rsi32FrameHeight > 0);
        return true;
    }

    void process_button(c_button_e a_eButton, c_bool a_bPressed, LPARAM lParam)
    {
        c_int siX = GET_X_LPARAM(lParam);
        c_int siY = GET_Y_LPARAM(lParam);
        if (a_bPressed)
        {
            EMP_APPLICATION.mouse_pressed(enum_to(a_eButton), a_eButton, siX, siY);
        }
        else
        {
            EMP_APPLICATION.mouse_released(enum_to(a_eButton), a_eButton, siX, siY);
        }
    }

    void process_move(LPARAM lParam)
    {
        c_int siX = GET_X_LPARAM(lParam);
        c_int siY = GET_Y_LPARAM(lParam);
        EMP_APPLICATION.mouse_move(emp::tt::max<ui64>::value, button_e::Invalid, siX, siY);
    }

    void process_wchar(WPARAM wParam, LPARAM EMP_SILENT(lParam))
    {
        emp::app::event_s* const pEvent = EMP_APPLICATION.get_last_event(event_e::KeyboardPressed);
        EMP_ASSERT(pEvent != nullptr);

        //c_astring swChar;
        //swChar.append_char(wParam); //@@ string wchar

        pEvent->keyboard.m_ui32Code = emp::ast::cast<ui32>(wParam);

        // @@0  the entire event ?
        EMP_APPLICATION.m_aEventKeys.ref(pEvent->keyboard.m_ui32Code).keyboard.m_ui32Code = emp::ast::cast<ui32>(wParam);
    }

    void process_key(WPARAM wParam, LPARAM lParam, c_bool a_bPressed)
    {
        c_ui32 ui32Code = emp::ast::cast<ui32>(wParam);
        if (a_bPressed && EMP_APPLICATION.m_aEventKeys.ref(ui32Code).keyboard.m_bPressed)
        {
            process_key(wParam, lParam, false);
        }

        event_s& rEvent = EMP_APPLICATION.push(a_bPressed ? event_e::KeyboardPressed : event_e::KeyboardReleased);

        keyboard_event_s& rKeyboard = rEvent.keyboard;
        rKeyboard.m_bPressed = a_bPressed;
        rKeyboard.m_ui32Code = emp::ast::cast<ui32>(wParam);
        rKeyboard.m_eKey = m_aKeys.val(ui32Code);
        EMP_APPLICATION.keyboard_repeat(rEvent);
        // @@0  needed ? rKeyboard.set_repeat((((lParam >> 30) & 1) == true) && a_bPressed);
    }
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
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

