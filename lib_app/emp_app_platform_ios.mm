//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_app_platform_ios.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_OS_IOS

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <QuartzCore/QuartzCore.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app { class platform_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@interface ui_view_c : UIView {}
//-(void)loop;
@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@interface ui_application_c : UIResponder <UIApplicationDelegate> {}
@property (strong, nonatomic) UIWindow *window;
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

    platform_c(void):
        m_mpTouches(16)
    {}

private:
    ui_application_c* m_pUiApplication;
    ui_view_c* m_pUiView;

    CAEAGLLayer *m_pEAGLLayer;
    EAGLContext *m_pEAGLContext;

    uint m_uiFramebuffer;
    uint m_uiRenderbuffer;

    map_t<UITouch*, ui64> m_mpTouches;

    array<key_e, 256, true> m_aKeys;
    astring m_sResourcePath;

    EMP_RETURN bool resource_path(c_astring& a_sValue, c_bool a_bRelative EMP_UNUSED)
    {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSArray *paths = NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES);

        NSString* nsDocumentPath = [paths objectAtIndex:0];
        NSString* nsResourcePath = [nsDocumentPath stringByAppendingPathComponent:@"data/"];
        BOOL success = [fileManager fileExistsAtPath:nsResourcePath];
        if (success)
        {
            NSLog(@"Success!");
            /*{
            NSError* error = nil;
            [fileManager removeItemAtPath:nsResourcePath error:&error];
            if (error)
            {
                NSLog(@"Could not remove old files. Error:%@", [error localizedDescription]);
            }
            }

            if ([fileManager fileExistsAtPath:nsResourcePath])
            {
                NSLog(@"Should not exists");
            }*/
        }
        else
        {
            //simplified method with more common and helpful method
            NSString* nsBundlePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"data/"];

            //fixed a deprecated method
            //fileManager [createDirectoryAtPath:nsResourcePath withIntermediateDirectories:NO attributes:nil error:nil];

            {
                NSError *error = nil;
                [fileManager copyItemAtPath:nsBundlePath toPath:nsResourcePath error:&error];
                if (error)
                {
                    NSLog(@"Failed to copy:%@", [error localizedDescription]);
                    //check if destinationFolder exists
                    if ([fileManager fileExistsAtPath:nsResourcePath])
                    {
                        //remove file path and NSLog error if it exists.
                        NSError* error1 = nil;
                        [fileManager removeItemAtPath:nsResourcePath error:&error1];
                        if (error1)
                        {
                            NSLog(@"Could not remove old files. Error:%@", [error1 localizedDescription]);
                        }
                    }

                    return false;
                }
            }
        }

        //NSString *nsResourcePath = [[NSBundle mainBundle] resourcePath];
        //NSString* nsResourcePath = documentDBFolderPath;

        u8string s8ResourcePath;
        EMP_RET_FALSE_IF_NOT(u8string::dup([nsDocumentPath UTF8String], s8ResourcePath));

        astring sResourcePath;
        EMP_RET_FALSE_IF_NOT(astring::from_u8string(sResourcePath, s8ResourcePath));

        m_sResourcePath.clear();
        emp::tt::silent(STR_FORMATA(m_sResourcePath, "#0/#1", sResourcePath, a_sValue));

        return true;
    }

    EMP_RETURN c_astring& resource_path(void) const { return m_sResourcePath; }

    EMP_RETURN bool run(void)
    {
        [UIApplication sharedApplication].statusBarHidden = YES;
        UIApplicationMain(0, nil, nil, NSStringFromClass([ui_application_c class]));
        return true;
    }

    EMP_RETURN bool create(void) { return true; } // @@0 interface

    EMP_RETURN bool parse(void)
    {
        emp::gfx::c_gl_version_e eGlVersion = EMP_GRAPHICS.gl_version();
        EAGLRenderingAPI eaglRenderingApi;
        if (eGlVersion == emp::gfx::gl_version_e::ES30)
        { eaglRenderingApi = kEAGLRenderingAPIOpenGLES3; }
        else if (eGlVersion == emp::gfx::gl_version_e::ES20)
        { eaglRenderingApi = kEAGLRenderingAPIOpenGLES2; }
        else
        {
            EMP_LOG_ERROR("unhandled gles version");
            return false;
        }

        m_pEAGLContext = [[EAGLContext alloc] initWithAPI:eaglRenderingApi];
        EMP_RET_FALSE_IF(m_pEAGLContext == nullptr);

        emp::gfx::gl_device_c& rDevice = EMP_GRAPHICS;

        configuration_s const& rConfiguration = EMP_APPLICATION.configuration();
        c_f64 dWidth = screen_pixel_width(); // @@0 handle configuration
        EMP_RET_FALSE_IF(dWidth <= 0 || dWidth > 4096, "invalid width #0", dWidth);
        c_si32 siWidth = emp::ast::cast<si32>(dWidth);

        c_f64 dHeight = screen_pixel_width(); // @@0 handle configuration
        EMP_RET_FALSE_IF(dHeight <= 0 || dHeight > 4096, "invalid height #0", dHeight);
        c_si32 siHeight = emp::ast::cast<si32>(dHeight);

        c_ui32 ui32DepthFormat = rConfiguration.depth_bpp;
        GLenum eDepthFormat = GL_NONE;
        if (ui32DepthFormat == 32)
        {
            eDepthFormat = GL_DEPTH_COMPONENT32F;
        }
        else if (ui32DepthFormat == 24)
        {
            eDepthFormat = GL_DEPTH_COMPONENT24;
        }
        else if (ui32DepthFormat == 16)
        {
            eDepthFormat = GL_DEPTH_COMPONENT16;
        }
        else if (ui32DepthFormat == 0)
        {
            eDepthFormat = GL_NONE;
        }
        else
        {
            EMP_LOG_ERROR("invalid depth format #0", ui32DepthFormat);
            return false;
        }

        c_ui32 ui32StencilFormat = rConfiguration.stencil_bpp;
        GLenum eStencilFormat = GL_NONE;
        if (ui32StencilFormat == 8)
        {
            eStencilFormat = GL_STENCIL_INDEX8;
        }
        else if (ui32StencilFormat == 0)
        {
        }
        else
        {
            EMP_LOG_ERROR("invalid stencil format #0",ui32StencilFormat);
            return false;
        }

        EMP_RET_FALSE_IF_NOT(current_context());

        m_uiFramebuffer = 0;
        EMP_RET_FALSE_IF_NOT(rDevice.glGenFramebuffers__(1, &m_uiFramebuffer));
        EMP_RET_FALSE_IF_NOT(m_uiFramebuffer);
        EMP_RET_FALSE_IF_NOT(rDevice.glBindFramebuffer__(GL_FRAMEBUFFER, m_uiFramebuffer));

        // @@0 handle color formats
        /*GLKViewDrawableColorFormat eColorFormat = pView.drawableColorFormat;
         if (eColorFormat == GLKViewDrawableColorFormatRGB565)
         {}*/
        {
            m_uiRenderbuffer = 0;
            EMP_RET_FALSE_IF_NOT(rDevice.glGenRenderbuffers__(1, &m_uiRenderbuffer));
            EMP_RET_FALSE_IF_NOT(m_uiRenderbuffer);
            EMP_RET_FALSE_IF_NOT(rDevice.glBindRenderbuffer__(GL_RENDERBUFFER, m_uiRenderbuffer));
            EMP_RET_FALSE_IF_NOT([m_pEAGLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(m_pEAGLLayer)]);
            EMP_RET_FALSE_IF_NOT(rDevice.glFramebufferRenderbuffer__(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_uiRenderbuffer));
        }

        int siNewWidth = 0;
        EMP_RET_FALSE_IF_NOT(rDevice.glGetRenderbufferParameteriv__(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &siNewWidth));

        int siNewHeight = 0;
        EMP_RET_FALSE_IF_NOT(rDevice.glGetRenderbufferParameteriv__(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &siNewHeight));

        // @@0 handle depth/stencil packing

        if (eDepthFormat != GL_NONE)
        {
            uint uiDepthBuffer = 0;
            EMP_RET_FALSE_IF_NOT(rDevice.glGenRenderbuffers__(1, &uiDepthBuffer));
            EMP_RET_FALSE_IF_NOT(uiDepthBuffer, "failed to generate depth buffer");
            EMP_RET_FALSE_IF_NOT(rDevice.glBindRenderbuffer__(GL_RENDERBUFFER, uiDepthBuffer));
            EMP_RET_FALSE_IF_NOT(rDevice.glRenderbufferStorage__(GL_RENDERBUFFER, eDepthFormat, siWidth, siHeight));
            EMP_RET_FALSE_IF_NOT(rDevice.glFramebufferRenderbuffer__(
                GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, uiDepthBuffer));
        }

        if (eStencilFormat != GL_NONE)
        {
            uint uiStencilBuffer = 0;
            EMP_RET_FALSE_IF_NOT(rDevice.glGenRenderbuffers__(1, &uiStencilBuffer));
            EMP_RET_FALSE_IF_NOT(uiStencilBuffer, "failed to generate stencil buffer");
            EMP_RET_FALSE_IF_NOT(rDevice.glBindRenderbuffer__(GL_RENDERBUFFER, uiStencilBuffer));
            EMP_RET_FALSE_IF_NOT(rDevice.glRenderbufferStorage__(GL_RENDERBUFFER, eStencilFormat, siWidth, siHeight));
            EMP_RET_FALSE_IF_NOT(rDevice.glFramebufferRenderbuffer__(
                GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, uiStencilBuffer));
        }

        return true;
    }

    EMP_RETURN bool reflect(void) { return true; } // @@0 interface
    EMP_RETURN bool deserialize(void) { return true; } // @@0 interface
    EMP_RETURN bool init(void) { return true; }

    EMP_RETURN bool poll(void) { EMP_APPLICATION.add(event_e::none); return true; } // @@0 wtf

    EMP_RETURN bool present(void)
    {
        //EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.discard_framebuffer());
        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.glBindRenderbuffer__(GL_RENDERBUFFER, renderbuffer()));
        return [m_pEAGLContext presentRenderbuffer:GL_RENDERBUFFER];
    }

    EMP_RETURN bool exit(c_bool a_bError EMP_UNUSED) { return true; } // @@0 interface
    EMP_RETURN bool serialize(void) { return false; } // @@0 interface

public:
    EMP_RETURN bool destroy(void)
    {
        if (m_pEAGLContext != nullptr)
        {
            //CGLReleaseContext(m_pContext); // @@0 leak
            m_pEAGLContext = nullptr;
        }

        //[m_pEAGLContext release];
        m_pEAGLContext = nil;

        return true;
    }

private:
    EMP_RETURN array<key_e, 256, true> const& keys(void) const { return m_aKeys; }

    EMP_RETURN bool show(void) { return true; } // @@0 interface
    EMP_RETURN bool center(void) { return true; } // @@0 interface
    EMP_RETURN bool move_resize(c_f64 EMP_SILENT(a_dX), c_f64 EMP_SILENT(a_dY), c_f64 EMP_SILENT(a_dW), c_f64 EMP_SILENT(a_dH)) { return true; }
    EMP_RETURN bool move(c_f64 EMP_SILENT(a_dX), c_f64 EMP_SILENT(a_dY)) { return true; } // @@0 interface
    EMP_RETURN bool resize(c_f64 EMP_SILENT(a_dW), c_f64 EMP_SILENT(a_dH)) { return true; } // @@0 interface
    EMP_RETURN bool fullscreen(c_bool EMP_SILENT(a_bValue)) { return true; } // @@0 interface

    EMP_RETURN uint framebuffer(void) const { return m_uiFramebuffer; }
    EMP_RETURN uint renderbuffer(void) const { return m_uiRenderbuffer; }

    EMP_RETURN bool current_context(void) { [EAGLContext setCurrentContext:m_pEAGLContext]; return true; }
    EMP_RETURN bool reset_context(void) { [EAGLContext setCurrentContext:nil]; return true; }

    EMP_RETURN bool set_swap_interval(c_int a_siValue EMP_UNUSED) const { return false; }
    EMP_RETURN bool get_swap_interval(int& a_rsiValue EMP_UNUSED) const { return false; }

    void sync(void) {}

    EMP_RETURN f64 screen_point_width(void) const { return [UIScreen mainScreen].bounds.size.width; }
    EMP_RETURN f64 screen_point_height(void) const { return [UIScreen mainScreen].bounds.size.height; }
    EMP_RETURN f64 window_point_width(void) const { return m_pUiApplication.window.frame.size.width; }
    EMP_RETURN f64 window_point_height(void) const { return m_pUiApplication.window.frame.size.height; }
    EMP_RETURN f64 view_point_width(void) const { return m_pUiView.frame.size.width; }
    EMP_RETURN f64 view_point_height(void) const { return m_pUiView.frame.size.height; }

    EMP_RETURN double scale_factor(void) const { return [m_pUiView contentScaleFactor]; }

public:
    void multi(UIView* view, BOOL a_bValue)
    {
        [view setUserInteractionEnabled:TRUE];
        [view setMultipleTouchEnabled:TRUE];
        [view setExclusiveTouch:a_bValue];
        for (UIGestureRecognizer* r in view.gestureRecognizers)
        {
            r.enabled = FALSE;
        }

        for (UIView* v in view.subviews)
        {
            multi(v, a_bValue);
        }
    }

    void loaded(ui_application_c* a_pUiApplication, SEL sel EMP_UNUSED)
    {
        m_pUiApplication = a_pUiApplication;
    }
    void launched(ui_application_c* a_pUiApplication, SEL sel)
    {
        m_pUiApplication = a_pUiApplication;

        CGRect screenBounds = [UIScreen mainScreen].bounds;
        m_pUiView = [[ui_view_c alloc] initWithFrame:screenBounds];

        [m_pUiApplication.window addSubview:m_pUiView];
        c_f64 dScale = [[UIScreen mainScreen] scale];
        c_f32 fScale = emp::ast::cast<float>(dScale);
        m_pUiApplication.window.contentScaleFactor = fScale;

        m_pUiView.contentScaleFactor = fScale;

        //multi(m_pUiApplication.window);
/*        [m_pUiView setUserInteractionEnabled:YES];
        [m_pUiView setMultipleTouchEnabled:YES];
        [m_pUiView setExclusiveTouch:YES];

        [m_pUiApplication.window setUserInteractionEnabled:YES];
        [m_pUiApplication.window setMultipleTouchEnabled:YES];
        [m_pUiApplication.window setExclusiveTouch:YES];
*/
        m_pEAGLLayer = (CAEAGLLayer*)m_pUiView.layer;
        m_pEAGLLayer.contentsScale = fScale;
        m_pEAGLLayer.opaque = TRUE;

        multi(m_pUiApplication.window, FALSE);
        multi(m_pUiView, TRUE);

        EMP_APPLICATION.add(emp::app::event_e::AppCreate);
        EMP_APPLICATION.add(emp::app::event_e::AppParse);
        EMP_APPLICATION.add(emp::app::event_e::AppReflect);
        EMP_APPLICATION.add(emp::app::event_e::AppInit);
        EMP_APPLICATION.add(emp::app::event_e::AppDeserialize);

        CADisplayLink* pDisplayLink = [CADisplayLink displayLinkWithTarget:m_pUiApplication selector:sel];
        [pDisplayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    }

public:
    void output(cpc_char a_szOutput) const
    {
        NSString* sOutput = [NSString stringWithUTF8String:a_szOutput];
        NSLog(@"%@", sOutput);
    }

private:
    EMP_RETURN bool set_context(EAGLContext* a_pValue) { return [EAGLContext setCurrentContext:a_pValue]; }

public:
    void new_touch(UITouch* a_pTouch)
    {
        EMP_ASSERT(m_mpTouches.get(a_pTouch) == m_mpTouches.end());

        c_ui64 uiTouch = EMP_APPLICATION.new_touch_id();

        m_mpTouches.set(a_pTouch, uiTouch);

        CGPoint oLocation = [a_pTouch locationInView:m_pUiView];
        //NSUInteger uiTaps = [pTouch tapCount];
        //NSLog(@"Tap BEGIN at %f, %f Tap count: %lu", oLocation.x, oLocation.y, emp::ast::cast<unsigned long>(uiTaps));
        //NSLog(@"PRESSED COUNT %lu", (unsigned long)[a_pTouches count]);
        EMP_APPLICATION.mouse_pressed(uiTouch, emp::app::button_e::Left, oLocation.x, oLocation.y);
    }

    void mouse_pressed(NSSet* const a_pTouches, UIEvent* const EMP_SILENT(a_pEvent))
    {
        for (UITouch* pTouch in a_pTouches.allObjects)
        {
            new_touch(pTouch);
        }
    }

    void mouse_released(NSSet* const a_pTouches, UIEvent* const EMP_SILENT(a_pEvent))
    {
        for (UITouch* pTouch in a_pTouches.allObjects)
        {
            auto const it = m_mpTouches.get(pTouch);
            EMP_ASSERT(it != m_mpTouches.end());
            c_ui64 uiTouch = *it.value();
            m_mpTouches.remove(it);

            CGPoint oLocation = [ pTouch locationInView:m_pUiView];

            //NSLog(@"Tap ENDED at %f, %f", oLocation.x, oLocation.y);
            //NSLog(@"RELEASED COUNT %lu", (unsigned long)[a_pTouches count]);

            EMP_APPLICATION.mouse_released(uiTouch, emp::app::button_e::Left, oLocation.x, oLocation.y);
        }
    }

    void mouse_moved(NSSet* const a_pTouches, UIEvent* const EMP_SILENT(a_pEvent))
    {
        for (UITouch* pTouch in a_pTouches.allObjects)
        {
            if (m_mpTouches.get(pTouch) == m_mpTouches.end())
            { new_touch(pTouch); }

            auto const it = m_mpTouches.get(pTouch);
            EMP_ASSERT(it != m_mpTouches.end());

            c_ui64 uiTouch = *it.value();

            //CGPoint oOldLocation = [ pTouch previousLocationInView:m_pUiView];
            CGPoint oLocation = [ pTouch locationInView:m_pUiView];

            //NSLog(@"Finger MOVED from %f, %f to %f, %f", oOldLocation.x, oOldLocation.y, oLocation.x, oLocation.y);
            //NSLog(@"MOVED COUNT %lu", (unsigned long)[a_pTouches count]);

            EMP_APPLICATION.mouse_move(uiTouch, emp::app::button_e::Left, oLocation.x, oLocation.y);
        }
    }

    void loop(void)
    {
        if (EMP_APPLICATION.loop() == false)
        {
            EMP_LOG_ERROR("failed to loop");
            EMP_LOG_ERROR("exiting...");
            // @@0 error
            int* error = nullptr;
            *error = 0;
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
@implementation ui_application_c
@synthesize window;
- (BOOL)application:(UIApplication* EMP_UNUSED)application didFinishLaunchingWithOptions:(NSDictionary * EMP_UNUSED)launchOptions
{
    EMP_APPLICATION_LOCK EMP_PLATFORM.launched(self, @selector(loop));
    return YES;
}

- (void) loop { EMP_APPLICATION_LOCK EMP_PLATFORM.loop(); }

- (void)applicationWillResignActive:(UIApplication * EMP_UNUSED)application { }
- (void)applicationDidEnterBackground:(UIApplication * EMP_UNUSED)application { }
- (void)applicationWillEnterForeground:(UIApplication * EMP_UNUSED)application { }
- (void)applicationDidBecomeActive:(UIApplication * EMP_UNUSED)application { }
- (void)applicationWillTerminate:(UIApplication * EMP_UNUSED)application { }

- (BOOL)canBecomeFirstResponder { return YES; }
- (void)viewDidAppear:(BOOL EMP_UNUSED)animated { [super becomeFirstResponder]; }
- (void)viewDidLoad
{
    EMP_APPLICATION_LOCK EMP_PLATFORM.loaded(self, @selector(loop));
}

- (void)touchesBegan:(NSSet* EMP_UNUSED)touches withEvent:(UIEvent*)event
{
    EMP_APPLICATION_LOCK
    EMP_XX_IF_DEBUG(
    EMP_XX_IF_LOG(
    for (UITouch* pTouch in touches)
    {
        EMP_LOG_DEBUG("BEGAN #0", (ui64)pTouch);
    }))
    //[ super touchesBegan:touches withEvent:event];
    EMP_PLATFORM.mouse_pressed(touches, event);
}

- (void)touchesEnded:(NSSet* EMP_UNUSED)touches withEvent:(UIEvent*)event
{
    EMP_APPLICATION_LOCK
    EMP_XX_IF_DEBUG(
    EMP_XX_IF_LOG(
    for (UITouch* pTouch in touches)
    {
        EMP_LOG_DEBUG("ENDED #0", (ui64)pTouch);
    }))
    //[ super touchesEnded: touches withEvent: event ];
    EMP_PLATFORM.mouse_released(touches, event);
}

- (void)touchesMoved:(NSSet* EMP_UNUSED)touches withEvent:(UIEvent*)event
{
    EMP_APPLICATION_LOCK
    EMP_XX_IF_DEBUG(
    EMP_XX_IF_LOG(
    for (UITouch* pTouch in touches)
    {
        EMP_LOG_DEBUG("MOVED #0", (ui64)pTouch);
    }))
    //[ super touchesMoved: touches withEvent: event ];
    EMP_PLATFORM.mouse_moved(touches, event);
}

- (void)touchesCancelled:(NSSet * EMP_UNUSED)touches withEvent:(UIEvent * EMP_UNUSED)event
{
    EMP_APPLICATION_LOCK
    EMP_XX_IF_DEBUG(
    EMP_XX_IF_LOG(
    for (UITouch* pTouch in touches)
    {
        EMP_LOG_DEBUG("CANCELLED #0", (ui64)pTouch);
    }))
    //[super touchesEnded: touches withEvent:event];
    EMP_PLATFORM.mouse_released(touches, event);
}
@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
@implementation ui_view_c
//- (BOOL)canBecomeFirstResponder { return YES; }
//- (void)viewDidAppear:(BOOL EMP_UNUSED)animated { [super becomeFirstResponder]; }
+ (Class)layerClass { return [CAEAGLLayer class]; }
- (void)dealloc { EMP_APPLICATION_LOCK /*[super dealloc];*/ EMP_ASSERT_BOOL_CALL(EMP_PLATFORM.destroy()); }

@end

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

