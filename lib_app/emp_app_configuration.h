//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_APP_CONFIGURATION_H
#define EMP_APP_CONFIGURATION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace app {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
struct configuration_s
{
    int argc = 0;
    cpc_char* argv = nullptr;
    astring usage;
    astring data;
    astring title;
    f64 x = 0;
    f64 y = 0;
    f64 width = 0;
    f64 height = 0;
    bool vsync = false;
    bool fullscreen = false;
    bool centered = false;
    bool maximized = false;
    bool fps = false;
    bool stereo = false;
    bool double_buffer = false;
    bool silent = false;
    ui32 buffer_size = 0;
    ui32 red_bpp = 0;
    ui32 green_bpp = 0;
    ui32 blue_bpp = 0;
    ui32 alpha_bpp = 0;
    ui32 depth_bpp = 0;
    ui32 stencil_bpp = 0;
    ui32 sample_buffers = 0;
    ui32 samples = 0;
    on_create_f on_create;
    on_parse_f on_parse;
    on_reflect_f on_reflect;
    on_init_f on_init;
    on_deserialize_f on_deserialize;
    on_exit_f on_exit;
    on_unit_test_f on_unit_test;

    ~configuration_s(void) {}
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

