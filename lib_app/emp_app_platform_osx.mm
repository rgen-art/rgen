//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_app_platform_osx.h"

// @@0 todo resource path ios/osx

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_OS_OSX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ast_errno.h"
#include "emp_app_application.h"
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app { class platform_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@interface ns_application_c : NSResponder<NSApplicationDelegate> {}
@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@interface ns_window_c : NSWindow <NSWindowDelegate> {}
@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@interface ns_view_c : NSView {}
@end

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
    ns_application_c* m_pNsApplication = nullptr;
    ns_window_c* m_pNsWindow = nullptr;
    ns_view_c* m_pNsView = nullptr;

    NSOpenGLContext* m_pNSGLContext = nullptr;

    CGLContextObj m_pCGLContext = nullptr;
    CGLPixelFormatObj m_pCGLPixelFormat = nullptr;

    CVDisplayLinkRef m_pCVDisplayLink = nullptr;

    array<key_e, 256, true> m_aKeys;
    astring m_sResourcePath;

    f64 m_dScaleFactor = 1.0;
    f64 m_dScreenPointWidth = 0.0;
    f64 m_dScreenPointHeight = 0.0;

public:
    bool m_bExitFullscreen = false;

private:
    EMP_RETURN bool resource_path(c_astring& a_sValue, c_bool a_bRelative)
    {
#if defined(EMP_XX_ENV_MAKE)
        m_sResourcePath = a_sValue;
#elif defined(EMP_XX_ENV_XCODE)
        if (a_bRelative)
        {
            NSString *nsResourcePath = [[NSBundle mainBundle] resourcePath];

            u8string s8ResourcePath;
            EMP_RET_FALSE_IF_NOT(u8string::dup([nsResourcePath UTF8String], s8ResourcePath));

            astring sResourcePath;
            EMP_RET_FALSE_IF_NOT(astring::from_u8string(sResourcePath, s8ResourcePath), "non ascii path.");

            m_sResourcePath.clear();
            emp::tt::silent(STR_FORMATA(m_sResourcePath, "#0/#1", sResourcePath, a_sValue));
        }
        else
        { m_sResourcePath = a_sValue; }
#else
#error invalid environment
#endif
        return true;
    }

    EMP_RETURN c_astring& resource_path(void) const { return m_sResourcePath; }

    EMP_RETURN bool run(void)
    {
        m_dScaleFactor = [[NSScreen mainScreen] backingScaleFactor];
        m_dScreenPointWidth = [[NSScreen mainScreen] frame].size.width;
        m_dScreenPointHeight = [[NSScreen mainScreen] frame].size.height;

        EMP_APPLICATION.add(event_e::AppCreate);
        EMP_APPLICATION.add(event_e::AppParse);
        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.loop());
        EMP_RET_FALSE_IF(EMP_APPLICATION.state() != state_e::Parsed);

        EMP_APPLICATION.add(event_e::AppReflect);
        EMP_APPLICATION.add(event_e::AppInit);
        EMP_APPLICATION.add(event_e::AppDeserialize);

        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.loop());

        CVDisplayLinkCreateWithActiveCGDisplays(&m_pCVDisplayLink);
        CVDisplayLinkSetOutputCallback(m_pCVDisplayLink, &CVDisplayLinkCallback, m_pCGLContext);
        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(m_pCVDisplayLink, m_pCGLContext, m_pCGLPixelFormat);
        CVDisplayLinkStart(m_pCVDisplayLink);

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp run];

        return true;
    }

    EMP_RETURN bool create(void)
    {
        c_key_e eKey = key_e::none;
        switch (eKey)
        {
            case key_e::none:
            #define XTMP_KEY(x_Value, x_Name)\
            case key_e::x_Name:\
            { m_aKeys.set(x_Value, key_e::x_Name); }
            XTMP_KEY( kVK_Tab            , Tabulation       )
            XTMP_KEY( kVK_Space          , Space            )
            XTMP_KEY( kVK_Return         , Enter            )
            XTMP_KEY( kVK_LeftArrow      , Left             )
            XTMP_KEY( kVK_RightArrow     , Right            )
            XTMP_KEY( kVK_UpArrow        , Up               )
            XTMP_KEY( kVK_DownArrow      , Down             )
            XTMP_KEY( kVK_Escape         , Escape           )
            XTMP_KEY( kVK_Delete         , Backspace        )
            XTMP_KEY( kVK_ForwardDelete  , Delete           )
            XTMP_KEY( kVK_Option         , LeftAlt          )
            XTMP_KEY( kVK_RightOption    , RightAlt         )
            XTMP_KEY( kVK_Shift          , LeftShift        )
            XTMP_KEY( kVK_RightShift     , RightShift       )
            XTMP_KEY( kVK_Control        , LeftCtrl         )
            XTMP_KEY( kVK_RightControl   , RightCtrl        )
            XTMP_KEY( kVK_Command        , LeftCommand      )
            XTMP_KEY( kVK_Command        , RightCommand     ) // @@0 fail

            XTMP_KEY( kVK_ANSI_Slash     , Slash            )
            XTMP_KEY( kVK_ANSI_Period    , Period           )
            XTMP_KEY( kVK_ANSI_Comma     , Comma            )
            XTMP_KEY( kVK_ANSI_M         , Alpha_M          )
            XTMP_KEY( kVK_ANSI_N         , Alpha_N          )
            XTMP_KEY( kVK_ANSI_B         , Alpha_B          )
            XTMP_KEY( kVK_ANSI_V         , Alpha_V          )
            XTMP_KEY( kVK_ANSI_C         , Alpha_C          )
            XTMP_KEY( kVK_ANSI_X         , Alpha_X          )
            XTMP_KEY( kVK_ANSI_Z         , Alpha_Z          )
            XTMP_KEY( kVK_ANSI_Semicolon , SemiColon        )
            XTMP_KEY( kVK_ANSI_L         , Alpha_L          )
            XTMP_KEY( kVK_ANSI_K         , Alpha_K          )
            XTMP_KEY( kVK_ANSI_J         , Alpha_J          )
            XTMP_KEY( kVK_ANSI_H         , Alpha_H          )
            XTMP_KEY( kVK_ANSI_G         , Alpha_G          )
            XTMP_KEY( kVK_ANSI_H         , Alpha_F          )
            XTMP_KEY( kVK_ANSI_D         , Alpha_D          )
            XTMP_KEY( kVK_ANSI_S         , Alpha_S          )
            XTMP_KEY( kVK_ANSI_A         , Alpha_Q          )
            XTMP_KEY( kVK_ANSI_P         , Alpha_P          )
            XTMP_KEY( kVK_ANSI_O         , Alpha_O          )
            XTMP_KEY( kVK_ANSI_I         , Alpha_I          )
            XTMP_KEY( kVK_ANSI_U         , Alpha_U          )
            XTMP_KEY( kVK_ANSI_Y         , Alpha_Y          )
            XTMP_KEY( kVK_ANSI_T         , Alpha_T          )
            XTMP_KEY( kVK_ANSI_R         , Alpha_R          )
            XTMP_KEY( kVK_ANSI_E         , Alpha_E          )
            XTMP_KEY( kVK_ANSI_W         , Alpha_W          )
            XTMP_KEY( kVK_ANSI_Q         , Alpha_A          )
            XTMP_KEY( kVK_ANSI_0         , Num_0            )
            XTMP_KEY( kVK_ANSI_9         , Num_9            )
            XTMP_KEY( kVK_ANSI_8         , Num_8            )
            XTMP_KEY( kVK_ANSI_7         , Num_7            )
            XTMP_KEY( kVK_ANSI_6         , Num_6            )
            XTMP_KEY( kVK_ANSI_5         , Num_5            )
            XTMP_KEY( kVK_ANSI_4         , Num_4            )
            XTMP_KEY( kVK_ANSI_3         , Num_3            )
            XTMP_KEY( kVK_ANSI_2         , Num_2            )
            XTMP_KEY( kVK_ANSI_1         , Num_1            )
            #undef XTMP_KEY
            default:
            { break; }
        };

        // @@0 handle emp::gfx::c_gl_version_e eGlVersion = EMP_GRAPHICS.gl_version();

        int siMajor = 0;
        int siMinor = 0;
        CGLGetVersion(&siMajor, &siMinor);
        EMP_LOG_INFO("cgl version #0, #1", siMajor, siMinor);

        configuration_s const& rConfiguration = EMP_APPLICATION.configuration();

        {
            emp::cnt::vector<CGLPixelFormatAttribute> vAttributes;

            if (rConfiguration.double_buffer)
            { vAttributes.add(kCGLPFADoubleBuffer); }

            //@@0 remove if (rConfiguration.stereo)
            //@@0 remove { vAttributes.add(kCGLPFAStereo); }

            vAttributes.add(kCGLPFAColorSize);
            vAttributes.add(static_cast<CGLPixelFormatAttribute>(rConfiguration.buffer_size));

            vAttributes.add(kCGLPFAAlphaSize);
            vAttributes.add(static_cast<CGLPixelFormatAttribute>(rConfiguration.alpha_bpp));

            vAttributes.add(kCGLPFADepthSize);
            vAttributes.add(static_cast<CGLPixelFormatAttribute>(rConfiguration.depth_bpp));

            vAttributes.add(kCGLPFAStencilSize);
            vAttributes.add(static_cast<CGLPixelFormatAttribute>(rConfiguration.stencil_bpp));

            vAttributes.add(static_cast<CGLPixelFormatAttribute>(0));

            GLint siVirtualScreenCount = 0;

            CGLError cglError = CGLChoosePixelFormat(vAttributes.ptr(0), &m_pCGLPixelFormat, &siVirtualScreenCount);
            EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLChoosePixelFormat Error : #0", astring::dup(CGLErrorString(cglError))); // @@0 alloc

            CGLRetainPixelFormat(m_pCGLPixelFormat);
        }

        {
            CGLError cglError = CGLCreateContext(m_pCGLPixelFormat, nullptr, &m_pCGLContext);
            EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLCreateContext Error : #0", astring::dup(CGLErrorString(cglError))); //@@0 alloc
        }

        return true;
    }

    EMP_RETURN bool parse(void)
    {
        [NSApplication sharedApplication];
        m_pNSGLContext = [[NSOpenGLContext alloc] initWithCGLContextObj:m_pCGLContext];

        configuration_s const& rConfiguration = EMP_APPLICATION.configuration();

        c_bool bFullscreen = rConfiguration.fullscreen;

        c_f64 dWidth = bFullscreen ? screen_point_width() : rConfiguration.width / m_dScaleFactor;
        c_f64 dHeight = bFullscreen ? screen_point_height() : rConfiguration.height / m_dScaleFactor;

        m_pNsView = [[ns_view_c alloc] initWithFrame:NSMakeRect(0, 0, dWidth, dHeight)];
        [m_pNsView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];

        //[[NSNotificationCenter defaultCenter] addObserver:m_pNsView selector:@selector(windowDidResize)
        //    name:NSViewGlobalFrameDidChangeNotification object:m_pNsView];

        NSUInteger windowStyle =
            NSTitledWindowMask |
            NSClosableWindowMask |
            NSResizableWindowMask |
            NSMiniaturizableWindowMask |
            NSTexturedBackgroundWindowMask;

        c_bool bCentered = rConfiguration.centered;
        c_f64 dX = bCentered ? (screen_point_width() - dWidth) * 0.5 : rConfiguration.x;
        c_f64 dY = bCentered ? (screen_point_height() - dHeight) * 0.5 : rConfiguration.y;

        m_pNsWindow = [[ns_window_c alloc] initWithContentRect:NSMakeRect(dX, dY, dWidth, dHeight)
            styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO];

        [m_pNsView setWantsLayer:YES];
        m_pNsView.layer.contentsScale = m_dScaleFactor;
        m_pNsView.layer.backgroundColor = [NSColor blackColor].CGColor;
        [m_pNsView setWantsBestResolutionOpenGLSurface:YES];

        // @@0 NSString* psCaption = [NSString stringWithUTF8: rConfiguration.title];
        // @@0 [pWindow autorelease];
        // @@0 [pWindow setTitle:psCaption];

        [m_pNsWindow setContentView:m_pNsView];
        EMP_RET_FALSE_IF_NOT(current_context());

        [m_pNSGLContext setView:m_pNsView];

        NSTrackingAreaOptions oOptions =
            NSTrackingActiveAlways |
            NSTrackingInVisibleRect |
            NSTrackingMouseEnteredAndExited |
            NSTrackingMouseMoved;

        NSTrackingArea* area = [[NSTrackingArea alloc] initWithRect:[m_pNsView bounds] options:oOptions owner:m_pNsView userInfo:nil];
        [m_pNsView addTrackingArea:area];

        m_pNsApplication = [ns_application_c alloc];
        [(NSApplication*)NSApp setDelegate:m_pNsApplication];

        [m_pNsWindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary|NSWindowCollectionBehaviorFullScreenPrimary];
        [m_pNsWindow setDelegate:m_pNsWindow];

        [NSApp activateIgnoringOtherApps:YES];

        return true;
    }

    EMP_RETURN bool reflect(void) { return true; } // @@0 interface
    EMP_RETURN bool deserialize(void) { return true; } // @@0 interface
    EMP_RETURN bool init(void) { return true; } // @@0 interface

    EMP_RETURN bool poll(void)
    {
        // @@0 device wtf
        EMP_APPLICATION.add(event_e::none);
        return true;
    }

    EMP_RETURN bool present(void)
    {
        CGLError cglError = CGLFlushDrawable(m_pCGLContext);
        EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLFlushDrawable Error : #0", astring::dup(CGLErrorString(cglError))); // @@0 alloc
        return true;
    }

    EMP_RETURN bool exit(c_bool a_bError)
    {
        CVDisplayLinkStop(m_pCVDisplayLink);
        EMP_ASSERT_BOOL_CALL(EMP_APPLICATION.exit(a_bError));
        std::exit(0); // @@0 fucking exit....
        return true;
    }

    EMP_RETURN bool serialize(void) { return true; } // @@0 interface

    EMP_RETURN bool destroy(void)
    {
        if (m_pCVDisplayLink != nullptr)
        {
            CVDisplayLinkRelease(m_pCVDisplayLink);
            m_pCVDisplayLink = nullptr;
        }

        if (m_pCGLPixelFormat != nullptr)
        {
            CGLReleasePixelFormat(m_pCGLPixelFormat);
            m_pCGLPixelFormat = nullptr;
        }

        if (m_pCGLContext != nullptr)
        {
            CGLReleaseContext(m_pCGLContext);
            m_pCGLContext = nullptr;
        }

        //m_pWindow = nullptr;
        // @@0 leak

        return true;
    }

    EMP_RETURN array<key_e, 256, true> const& keys(void) const { return m_aKeys; }

    EMP_RETURN bool show(void) { return false; } // @@0 interface
    EMP_RETURN bool center(void) { [m_pNsWindow center]; return false; }

    EMP_RETURN bool move(c_f64 EMP_SILENT(a_dX), c_f64 EMP_SILENT(a_dY)) { return false; } // @@0 inteface

public:
    EMP_RETURN bool resize(c_f64 EMP_SILENT(a_dWidth), c_f64 EMP_SILENT(a_dHeight)) { return false; }

    EMP_RETURN bool resize(void) { return resize(window_point_width(), window_point_height()); }

    EMP_RETURN bool move_resize(c_f64 a_dX EMP_UNUSED, c_f64 a_dY EMP_UNUSED, c_f64 a_dW EMP_UNUSED, c_f64 a_dH EMP_UNUSED)
    {
        /*dispatch_async(dispatch_get_main_queue(),
        ^{
            [m_pNsWindow setFrame:NSMakeRect(a_dX, a_dY, a_dW, a_dH) display:YES animate:YES];
        });*/
        return true;
    }

    EMP_RETURN bool fullscreen(c_bool EMP_SILENT(a_bValue))
    {
        dispatch_async(dispatch_get_main_queue(),
        ^{
            [m_pNsWindow toggleFullScreen:m_pNsWindow];
        });
        return true;
    }

private:
    EMP_RETURN uint framebuffer(void) const { return 0; }
    EMP_RETURN uint renderbuffer(void) const { return 0; }

    EMP_RETURN bool current_context(void) { return set_current(m_pCGLContext); }

public:
    EMP_RETURN bool reset_context(void) { return set_current(nullptr); }

private:
    EMP_RETURN bool set_swap_interval(c_int a_siValue) const { return set_parameter(m_pCGLContext, kCGLCPSwapInterval, a_siValue); }
    EMP_RETURN bool get_swap_interval(int& a_rsiValue) const { return get_parameter(m_pCGLContext, kCGLCPSwapInterval, a_rsiValue); }

    void sync(void) {} // @@0 interface

    double m_dWindowPointWidth = 0;
    double m_dWindowPointHeight = 0;
    double m_dViewPointWidth = 0;
    double m_dViewPointHeight = 0;

    EMP_RETURN f64 screen_point_width(void) const { return m_dScreenPointWidth; }
    EMP_RETURN f64 screen_point_height(void) const { return m_dScreenPointHeight; }
    EMP_RETURN f64 window_point_width(void) const { return m_dWindowPointWidth; }
    EMP_RETURN f64 window_point_height(void) const { return m_dWindowPointHeight; }
    EMP_RETURN f64 view_point_width(void) const { return m_dViewPointWidth; }
    EMP_RETURN f64 view_point_height(void) const { return m_dViewPointHeight; }

public:
    EMP_RETURN double scale_factor(void) const { return m_dScaleFactor; }

public:
    void launched(void)
    {
        [NSApp activateIgnoringOtherApps:YES];
        //[[NSApplication sharedApplication] setPresentationOptions:NSApplicationPresentationFullScreen];

        [m_pNsWindow makeKeyAndOrderFront:m_pNsWindow];
        [m_pNsWindow makeMainWindow];
        [m_pNsWindow makeFirstResponder:m_pNsView];
        if (EMP_APPLICATION.configuration().fullscreen)
        { [m_pNsWindow toggleFullScreen:m_pNsWindow]; }
    }

public:
    void minimize(void) { EMP_APPLICATION.add(event_e::Minimize); }
    void restore(void) { EMP_APPLICATION.add(event_e::Restore); }

public:
    void output(cpc_char a_szOutput) const
    {
        NSString* sOutput = [NSString stringWithUTF8String:a_szOutput];
        NSLog(@"%@", sOutput);
    }

private:
    EMP_INLINE EMP_RETURN bool current_context(CGLContextObj a_pContext)
    {
        CGLError cglError = CGLSetCurrentContext(a_pContext);
        EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLSetCurrentContext Error : #0", astring::dup(CGLErrorString(cglError))); //@@0 alloc
        return true;
    }

public:
    void update_context(void)
    {
        CGLLockContext(m_pCGLContext);
        [m_pNSGLContext update];
        CGLUnlockContext(m_pCGLContext);
    }

private:
    EMP_INLINE EMP_RETURN bool set_current(CGLContextObj a_pContext)
    {
        CGLError cglError = CGLSetCurrentContext(a_pContext);
        EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLSetCurrentContext Error : #0", astring::dup(CGLErrorString(cglError))); //@@0 alloc
        return true;
    }

    EMP_INTERN EMP_RETURN bool get_parameter(CGLContextObj a_pContext, CGLContextParameter const a_eName, int& a_rsiValue)
    {
        CGLError cglError = CGLGetParameter(a_pContext, a_eName, &a_rsiValue);
        EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLGetParameter Error : #0", astring::dup(CGLErrorString(cglError))); //@@0 alloc
        return true;
    }

    EMP_INTERN EMP_RETURN bool set_parameter(CGLContextObj a_pContext, CGLContextParameter const a_eName, c_int a_siValue)
    {
        CGLError cglError = CGLSetParameter(a_pContext, a_eName, &a_siValue);
        EMP_RET_FALSE_IF(cglError != kCGLNoError, "CGLSetParameter Error : #0", astring::dup(CGLErrorString(cglError))); // @@0 alloc
        return true;
    }

public:
    void send_move_resize_event(c_f64 a_dX, c_f64 a_dY, f64& a_rdWidth, f64& a_rdHeight, emp::app::c_event_e a_eEvent)
    {
        c_f64 dScale = scale_factor();
        [[m_pNsView layer] setContentsScale:dScale];

        c_f64 dBarSize = m_pNsView.frame.size.height - m_pNsWindow.contentView.frame.size.height;

        c_f64 dConfigW = EMP_APPLICATION.configuration().width / dScale;
        c_f64 dConfigH = EMP_APPLICATION.configuration().height / dScale;

        c_f64 dWidth = m_bExitFullscreen ? dConfigW : a_rdWidth;
        c_f64 dHeight = m_bExitFullscreen ? dConfigH - dBarSize : a_rdHeight;

        if (m_bExitFullscreen)
        {
            a_rdWidth = dConfigW;
            a_rdHeight = dConfigH;
        }

        event_s& rEvent = EMP_APPLICATION.push(a_eEvent);
        window_event_s& rWindow = rEvent.window;
        rWindow.m_dPointX = a_dX;
        rWindow.m_dPointY = a_dY;
        rWindow.m_dPointWidth = dWidth;
        rWindow.m_dPointHeight = dHeight;
        rWindow.m_dScale = dScale;
        rWindow.m_bBegin = true;

        m_dWindowPointWidth = [m_pNsWindow frame].size.width;
        m_dWindowPointHeight = [m_pNsWindow frame].size.height;
        m_dViewPointWidth = [m_pNsView frame].size.width;
        m_dViewPointHeight = [m_pNsView frame].size.height;
    }

    void send_resize_event(void)
    {
        f64 dW = [m_pNsWindow frame].size.width;
        f64 dH = [m_pNsWindow frame].size.height;
        send_move_resize_event(0, 0, dW, dH, event_e::ResizeWindow);
    }

public:
    void process_key(NSEvent const* const a_pEvent)
    {
        auto const uiModifier = [a_pEvent modifierFlags];
        c_bool bPressed =
            (uiModifier & NSAlternateKeyMask) != 0 ||
            (uiModifier & NSShiftKeyMask)     != 0 ||
            (uiModifier & NSControlKeyMask)   != 0;

        c_ui32 ui32Code = [a_pEvent keyCode];
        c_bool bModified =
            (ui32Code == kVK_Option ||
             ui32Code == kVK_Shift ||
             ui32Code == kVK_Control);

        process_key(a_pEvent, nullptr, bPressed, bModified);
    }

    void process_key(NSEvent const* const a_pEvent, cpc_char a_szChars, c_bool a_bPressed, c_bool a_bModified)
    {
        c_ui32 ui32Code = [a_pEvent keyCode];

        if (a_bPressed && EMP_APPLICATION.event_keys().ref(ui32Code).keyboard.m_bPressed)
        { process_key(a_pEvent, a_szChars, false, a_bModified); }

        event_s& rEvent = EMP_APPLICATION.push();
        rEvent.type = a_bPressed ? event_e::KeyboardPressed: event_e::KeyboardReleased;

        keyboard_event_s& rKeyboard = rEvent.keyboard;
        rKeyboard.m_bPressed = a_bPressed;
        rKeyboard.m_ui32Code = ui32Code;
        rKeyboard.m_eKey = m_aKeys.val(ui32Code);
        EMP_APPLICATION.keyboard_repeat(rEvent);

        if (a_bPressed && a_bModified == false)
        {
            if (a_szChars != nullptr)
            {
                u8string su8Char;
                if (u8string::dup(a_szChars, su8Char))
                {
                    if (su8Char.len() > 0)
                    { rKeyboard.m_ui64Char = su8Char.get_first_char(); }
                }
            }
        }

        if (a_bModified == false)
        { [m_pNsView interpretKeyEvents:[NSArray arrayWithObject:a_pEvent]]; }
    }

private:
    EMP_RETURN button_e button(NSEventType const a_siButton)
    {
        switch (a_siButton)
        {
            case NSLeftMouseDown:
            case NSLeftMouseUp: { return button_e::Left; }

            case NSRightMouseDown:
            case NSRightMouseUp: { return button_e::Right; }

            case NSOtherMouseDown:
            case NSOtherMouseUp: { return button_e::Center; }

            case NSMouseMoved:
            case NSLeftMouseDragged:
            case NSRightMouseDragged:
            case NSMouseEntered:
            case NSMouseExited:
            case NSKeyUp:
            case NSKeyDown:
            case NSFlagsChanged:
            case NSAppKitDefined:
            case NSSystemDefined:
            case NSApplicationDefined:
            case NSPeriodic:
            case NSCursorUpdate:
            case NSEventTypeRotate:
            case NSEventTypeBeginGesture:
            case NSEventTypeEndGesture:
            case NSScrollWheel:
            case NSTabletPoint:
            case NSTabletProximity:
            case NSOtherMouseDragged:
            case NSEventTypeGesture:
            case NSEventTypeMagnify:
            case NSEventTypeSwipe:
            case NSEventTypeSmartMagnify:
            case NSEventTypeQuickLook:
            case NSEventTypePressure:

            default: { EMP_ASSERT_UNREACHABLE(); }
        };
        return button_e::Invalid;
    }

    EMP_RETURN int xcoord(c_double a_dX) { return emp::ast::cast<int>(a_dX); }
    EMP_RETURN int ycoord(c_double a_dY) { return emp::ast::cast<int>(window_point_height() - a_dY); }

public:
    void mouse_pressed(NSEvent const* const a_pEvent)
    {
        NSPoint oPosition = [a_pEvent locationInWindow];
        auto const eButton = button([a_pEvent type]);
        EMP_APPLICATION.mouse_pressed(enum_to(eButton), eButton, xcoord(oPosition.x), ycoord(oPosition.y));
    }

    void mouse_released(NSEvent const* const a_pEvent)
    {
        NSPoint oPosition = [a_pEvent locationInWindow];
        auto const eButton = button([a_pEvent type]);
        EMP_APPLICATION.mouse_released(enum_to(eButton), eButton, xcoord(oPosition.x), ycoord(oPosition.y));
    }

    void mouse_move(NSEvent const* const a_pEvent)
    {
        NSPoint oPosition = [a_pEvent locationInWindow];
        EMP_APPLICATION.mouse_move(emp::tt::max<ui64>::value, button_e::Invalid, xcoord(oPosition.x), ycoord(oPosition.y));
    }

    void mouse_wheel(NSEvent const* const a_pEvent)
    {
        c_double dDelta = [a_pEvent deltaY];
        if (emp::mat::abs(dDelta) > 0)
        {
            c_button_e eButton = dDelta >= 0 ? button_e::WheelUp : button_e::WheelDown;

            NSPoint oPosition = [a_pEvent locationInWindow];
            c_int siX = xcoord(oPosition.x);
            c_int siY = ycoord(oPosition.y);

            c_ui64 uiId = EMP_APPLICATION.new_touch_id();

            EMP_APPLICATION.mouse_pressed(uiId, eButton, siX, siY);
            EMP_APPLICATION.mouse_released(uiId, eButton, siX, siY);
        }
    }

    void mouse_enter(NSEvent const* const EMP_SILENT(a_pEvent)) const
    {
        EMP_APPLICATION.mouse_enter();
    }
    void mouse_leave(NSEvent const* const EMP_SILENT(a_pEvent)) const { EMP_APPLICATION.mouse_leave(); }

    static CVReturn CVDisplayLinkCallback(
        CVDisplayLinkRef a_pCVDisplayLink,
        const CVTimeStamp* a_pNow EMP_UNUSED,
        const CVTimeStamp* a_pOutputTime EMP_UNUSED,
        CVOptionFlags a_oFlagsIn EMP_UNUSED,
        CVOptionFlags* a_oFlagsOut EMP_UNUSED,
        void* a_pCGLContext)
    {
        EMP_APPLICATION_LOCK
        {
            CGLContextObj const pCGLContext = static_cast<CGLContextObj>(a_pCGLContext);
            CGLLockContext(pCGLContext);



            const rlim_t kStackSize = 1L * 1024L * 1024L;   // min stack size = 64 Mb
            struct rlimit rl;
            int result;

            result = getrlimit(RLIMIT_STACK, &rl);
            if (result == 0)
            {
                if (rl.rlim_cur < kStackSize)
                {
                    rl.rlim_cur = kStackSize;
                    result = setrlimit(RLIMIT_STACK, &rl);
                    if (result != 0)
                    {
                        cpc_char pErrno = emp::ast::errno_to_cstr();
                        if (pErrno)
                        {
                        return kCVReturnError;
                    }
                    }
                }
            }


            bool bReturn = EMP_APPLICATION.loop();
            CGLUnlockContext(pCGLContext);

            if (bReturn == false)
            {
                CVDisplayLinkStop(a_pCVDisplayLink);
                std::exit(0);
                //[NSApp terminate:nil];
                return kCVReturnError;
            }
            return kCVReturnSuccess;
        }
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
@implementation ns_application_c
-(void) applicationWillTerminate:(NSNotification* EMP_UNUSED)notification
{
    //EMP_ASSERT_BOOL_CALL(EMP_APPLICATION.exit(true));
}

-(void) applicationWillFinishLaunching :(NSNotification* EMP_UNUSED) notification { }
-(void) applicationDidFinishLaunching  :(NSNotification* EMP_UNUSED) notification { EMP_APPLICATION_LOCK EMP_PLATFORM.launched(); }
-(void) applicationDidBecomeActive     :(NSNotification * EMP_UNUSED)notification { }
-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication* EMP_UNUSED) sender { return YES; }
-(BOOL) mouseDownCanMoveWindow  { return NO; }
@end
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@implementation ns_window_c
-(BOOL) acceptsFirstResponder   { return YES; }
-(BOOL) canBecomeMainWindow     { return YES; }
-(BOOL) canBecomeKeyWindow      { return YES; }
-(BOOL) mouseDownCanMoveWindow  { return NO; }
-(BOOL) windowShouldClose :(NSApplication* EMP_UNUSED) sender
{
    EMP_APPLICATION_LOCK EMP_APPLICATION.send_kill();

    //EMP_APPLICATION_LOCK EMP_APPLICATION.send_exit();
    return false;
}

- (void)windowWillMove:(NSNotification* EMP_UNUSED)notification
{
}

- (void)windowDidMove:(NSNotification* EMP_UNUSED)notification
{
    EMP_APPLICATION_LOCK
    f64 dW = 0;
    f64 dH = 0;
    EMP_PLATFORM.send_move_resize_event([self frame].origin.x, [self frame].origin.y, dW, dH, emp::app::event_e::MoveWindow);
}

-(NSSize) windowWillResize        :(NSWindow* EMP_UNUSED) sender toSize:(NSSize) size
{
    EMP_APPLICATION_LOCK
    EMP_PLATFORM.send_move_resize_event(0, 0, size.width, size.height, emp::app::event_e::ResizeWindow);
    return size;
}
-(void) windowWillMiniaturize     :(NSNotification* EMP_UNUSED) notification { EMP_APPLICATION_LOCK EMP_PLATFORM.minimize(); }
-(void) windowDidDeminiaturize    :(NSNotification* EMP_UNUSED) notification { EMP_APPLICATION_LOCK EMP_PLATFORM.restore(); }
-(void) windowDidResize           :(NSNotification* EMP_UNUSED) notification { EMP_APPLICATION_LOCK EMP_PLATFORM.update_context(); }
-(void) windowWillStartLiveResize :(NSNotification* EMP_UNUSED) notification { }
-(void) windowDidEndLiveResize    :(NSNotification* EMP_UNUSED) notification { }
-(void) windowWillEnterFullScreen :(NSNotification* EMP_UNUSED) notification { EMP_APPLICATION_LOCK EMP_ASSERT_BOOL_CALL(EMP_APPLICATION.fullscreen(true)); }
-(void) windowDidEnterFullScreen  :(NSNotification* EMP_UNUSED) notification { }
-(void) windowWillExitFullScreen  :(NSNotification* EMP_UNUSED) notification { EMP_APPLICATION_LOCK EMP_PLATFORM.m_bExitFullscreen = true; }
-(void) windowDidExitFullScreen   :(NSNotification* EMP_UNUSED) notification
 {
    EMP_APPLICATION_LOCK EMP_PLATFORM.m_bExitFullscreen = false;
    c_f64 dScale = EMP_PLATFORM.scale_factor();
    auto const& rConfig = EMP_APPLICATION.configuration();
    c_f64 dX = rConfig.x / dScale;
    c_f64 dY = rConfig.y / dScale;
    f64 dW = rConfig.width / dScale;
    f64 dH = rConfig.height / dScale;

    [self setFrame:NSMakeRect(dX, dY, dW, dH) display:YES animate:YES];
    EMP_PLATFORM.send_move_resize_event(dX, dY, dW, dH, emp::app::event_e::MoveResizeWindow);
 }
-(void) windowDidBecomeKey        :(NSNotification* EMP_UNUSED) notification { }
-(void) windowDidResignKey        :(NSNotification* EMP_UNUSED) notification { }
@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@implementation ns_view_c
-(BOOL) mouseDownCanMoveWindow  { return NO; }

-(void) viewDidChangeBackingProperties { EMP_APPLICATION_LOCK [super viewDidChangeBackingProperties]; EMP_PLATFORM.send_resize_event(); }

-(void) flagsChanged      :(NSEvent*)event  { EMP_APPLICATION_LOCK EMP_PLATFORM.process_key(event); }
-(void) keyDown           :(NSEvent*)event  { EMP_APPLICATION_LOCK EMP_PLATFORM.process_key(event, [[event characters] UTF8String], true, false); }
-(void) keyUp             :(NSEvent*)event  { EMP_APPLICATION_LOCK EMP_PLATFORM.process_key(event, [[event characters] UTF8String], false, false); }
-(void) rightMouseDown    :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_pressed(event); }
-(void) mouseDown         :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_pressed(event); }
-(void) rightMouseUp      :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_released(event); }
-(void) mouseUp           :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_released(event); }
-(void) rightMouseDragged :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_move(event); }
-(void) mouseDragged      :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_move(event); }
-(void) mouseMoved        :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_move(event); }
-(void) scrollWheel       :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_wheel(event); }
-(void) mouseEntered      :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_enter(event); }
-(void) mouseExited       :(NSEvent*) event { EMP_APPLICATION_LOCK EMP_PLATFORM.mouse_leave(event); }
@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
