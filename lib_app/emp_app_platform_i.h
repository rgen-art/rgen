//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_APP_PLATFORM_I_H
#define EMP_APP_PLATFORM_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class application_c;

EMP_FORWARD(enum, ui32, key_e)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, platform_i)
private:
    friend class application_c;

public:
    platform_i(void) {}
    inline virtual ~platform_i(void) {}

private:
    virtual EMP_RETURN bool resource_path(c_astring& a_sValue, c_bool a_bRelative) = 0;
    virtual EMP_RETURN c_astring& resource_path(void) const = 0;

    virtual EMP_RETURN bool run(void) = 0;
    virtual EMP_RETURN bool create(void) = 0;
    virtual EMP_RETURN bool parse(void) = 0;
    virtual EMP_RETURN bool reflect(void) = 0;
    virtual EMP_RETURN bool deserialize(void) = 0;
    virtual EMP_RETURN bool init(void) = 0;
    virtual EMP_RETURN bool poll(void) = 0;
    virtual EMP_RETURN bool present(void) = 0;

    virtual EMP_RETURN bool exit(c_bool a_bError) = 0;
    virtual EMP_RETURN bool serialize(void) = 0;

public:
    virtual EMP_RETURN bool destroy(void) = 0;

private:
    virtual EMP_RETURN array<key_e, 256, true> const& keys(void) const = 0;

    virtual EMP_RETURN bool show(void) = 0;
    virtual EMP_RETURN bool center(void) = 0;
    virtual EMP_RETURN bool move(c_f64 a_dX, c_f64 a_dY) = 0;
    virtual EMP_RETURN bool resize(c_f64 a_dW, c_f64 a_dH) = 0;
    virtual EMP_RETURN bool move_resize(c_f64 a_dX, c_f64 a_dY, c_f64 a_dW, c_f64 a_dH) = 0;
    virtual EMP_RETURN bool fullscreen(c_bool a_bValue) = 0;

    virtual EMP_RETURN uint framebuffer(void) const = 0;
    virtual EMP_RETURN uint renderbuffer(void) const = 0;

public:
    virtual EMP_RETURN bool current_context(void) = 0;
    virtual EMP_RETURN bool reset_context(void) = 0;

    virtual EMP_RETURN bool set_swap_interval(c_int a_siValue) const = 0;
    virtual EMP_RETURN bool get_swap_interval(int& a_rsiValue) const = 0;

    virtual void sync(void) = 0;

public:
    virtual void output(cpc_char a_szOutput) const = 0;

public:
    virtual EMP_RETURN f64 screen_point_width(void) const = 0;
    virtual EMP_RETURN f64 screen_point_height(void) const = 0;

    virtual EMP_RETURN f64 window_point_width(void) const = 0;
    virtual EMP_RETURN f64 window_point_height(void) const = 0;

    virtual EMP_RETURN f64 view_point_width(void) const = 0;
    virtual EMP_RETURN f64 view_point_height(void) const = 0;

    virtual EMP_RETURN double scale_factor(void) const = 0;

    EMP_RETURN f64 screen_pixel_width(void) const { return screen_point_width() * scale_factor(); }
    EMP_RETURN f64 screen_pixel_height(void) const { return screen_point_height() * scale_factor(); }

    EMP_RETURN f64 window_pixel_width(void) const { return window_point_width() * scale_factor(); }
    EMP_RETURN f64 window_pixel_height(void) const { return window_point_height() * scale_factor(); }

    EMP_RETURN f64 view_pixel_width(void) const { return view_point_width() * scale_factor(); }
    EMP_RETURN f64 view_pixel_height(void) const { return view_point_height() * scale_factor(); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

