//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GFX_GL_DEVICE_H
#define EMP_GFX_GL_DEVICE_H

#define GL_SILENCE_DEPRECATION
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_device.h"
#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_map.h"
#include "emp_cnt_array.h"

#include "emp_mat_matrix.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_OS_WIN
#define EMP_GL
#define EMP_GFX_GLAPIENTRY true
#ifndef EMP_XX_COMPILER_MSC
#define UNICODE
#endif
#include "emp_hh_windows.h"
#include <wingdi.h>
#undef max
#undef min
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#elif defined EMP_XX_OS_LIN
#define EMP_GL
#define EMP_GFX_GLAPIENTRY false
#define GL_GLEXT_PROTOTYPES // before including glext.h
#define GLX_GLXEXT_PROTOTYPES // to get glXSwapIntervalSGI
#include <gl.h>
#include <glext.h>
#include <glx.h>
#include <glxext.h>

#elif defined EMP_XX_OS_OSX
#define EMP_GL
#define EMP_GFX_GLAPIENTRY false
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <OpenGL/gltypes.h>

#elif defined EMP_XX_OS_IOS
#define EMP_GLES
#define EMP_GFX_GLAPIENTRY false
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/glext.h>

#elif defined EMP_XX_OS_OSX
#define EMP_GL
#define EMP_GFX_GLAPIENTRY false
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else

#error "Unknown os"

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_GL_PARAMS_0(...)
#define EMP_GL_PARAMS_1(x_P0, x_A0) x_P0 x_A0
#define EMP_GL_PARAMS_2(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_1(__VA_ARGS__))
#define EMP_GL_PARAMS_3(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_2(__VA_ARGS__))
#define EMP_GL_PARAMS_4(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_3(__VA_ARGS__))
#define EMP_GL_PARAMS_5(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_4(__VA_ARGS__))
#define EMP_GL_PARAMS_6(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_5(__VA_ARGS__))
#define EMP_GL_PARAMS_7(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_6(__VA_ARGS__))
#define EMP_GL_PARAMS_8(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_7(__VA_ARGS__))
#define EMP_GL_PARAMS_9(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_8(__VA_ARGS__))
#define EMP_GL_PARAMS_10(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_9(__VA_ARGS__))
#define EMP_GL_PARAMS_11(x_P0, x_A0, ...) x_P0 x_A0, EMP_PP_FORWARD(EMP_GL_PARAMS_10(__VA_ARGS__))
#define EMP_GL_PARAMS(x_Count, ...) EMP_PP_FORWARD(EMP_PP_CAT(EMP_GL_PARAMS_, x_Count)(__VA_ARGS__))

#define EMP_GL_ARGS_0(...)
#define EMP_GL_ARGS_1(x_P0, x_A0) x_A0
#define EMP_GL_ARGS_2(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_1(__VA_ARGS__))
#define EMP_GL_ARGS_3(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_2(__VA_ARGS__))
#define EMP_GL_ARGS_4(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_3(__VA_ARGS__))
#define EMP_GL_ARGS_5(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_4(__VA_ARGS__))
#define EMP_GL_ARGS_6(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_5(__VA_ARGS__))
#define EMP_GL_ARGS_7(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_6(__VA_ARGS__))
#define EMP_GL_ARGS_8(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_7(__VA_ARGS__))
#define EMP_GL_ARGS_9(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_8(__VA_ARGS__))
#define EMP_GL_ARGS_10(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_9(__VA_ARGS__))
#define EMP_GL_ARGS_11(x_P0, x_A0, ...) x_A0, EMP_PP_FORWARD(EMP_GL_ARGS_10(__VA_ARGS__))
#define EMP_GL_ARGS(x_Count, ...) EMP_PP_FORWARD(EMP_PP_CAT(EMP_GL_ARGS_, x_Count)(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_GL_CALL_EX(PFNGLDRAWBUFFERPROC, void, glDrawBuffer, GLenum, buf)
#define EMP_GL_CALLS_COMMON()\
EMP_GL_CALL_EX(PFNGLBLITFRAMEBUFFERPROC,\
void,glBlitFramebuffer,GLint,srcX0,GLint,srcY0,GLint,srcX1,GLint,srcY1,GLint,dstX0,GLint,dstY0,GLint,dstX1,GLint,dstY1,GLbitfield,mask,GLenum,filter)\
EMP_GL_CALL_EX(PFNGLGENVERTEXARRAYSPROC   , void , glGenVertexArrays     , GLsizei,n, GLuint*,arrays)\
EMP_GL_CALL_EX(PFNGLBINDVERTEXARRAYPROC   , void , glBindVertexArray, GLuint,array)\
EMP_GL_CALL_EX(PFNGLACTIVETEXTUREPROC              , void   , glActiveTexture              , GLenum,texture)\
EMP_GL_CALL_EX(PFNGLATTACHSHADERPROC               , void   , glAttachShader               , GLuint,program, GLuint,shader)\
EMP_GL_CALL_EX(PFNGLBINDATTRIBLOCATIONPROC         , void   , glBindAttribLocation         , GLuint,program, GLuint,index, GLchar const*,name)\
EMP_GL_CALL_EX(PFNGLBINDBUFFERPROC                 , void   , glBindBuffer                 , GLenum,target, GLuint,buffer)\
EMP_GL_CALL_EX(PFNGLBINDFRAMEBUFFERPROC            , void   , glBindFramebuffer            , GLenum,target, GLuint,framebuffer)\
EMP_GL_CALL_EX(PFNGLBINDRENDERBUFFERPROC           , void   , glBindRenderbuffer           , GLenum,target, GLuint,renderbuffer)\
EMP_GL_CALL(                                         void   , glBindTexture                , GLenum,target, GLuint,texture)\
EMP_GL_CALL(                                         void   , glBlendFunc                  , GLenum,src, GLenum,dst)\
EMP_GL_CALL_EX(PFNGLBLENDFUNCSEPARATEPROC          , void   , glBlendFuncSeparate          , GLenum,scrc, GLenum,dstc, GLenum,srca, GLenum,dsta)\
EMP_GL_CALL_EX(PFNGLBUFFERDATAPROC                 , void   , glBufferData                 , GLenum,target, GLsizeiptr,size, GLvoid const*,data, GLenum,usage)\
EMP_GL_CALL_EX(PFNGLBUFFERSUBDATAPROC              , void   , glBufferSubData              , GLenum,target, GLintptr,offset, GLsizeiptr,size, GLvoid const*,data)\
EMP_GL_CALL_EX(PFNGLCHECKFRAMEBUFFERSTATUSPROC     , GLenum , glCheckFramebufferStatus     , GLenum,target)\
EMP_GL_CALL(                                         void   , glClear                      , GLbitfield,mask)\
EMP_GL_CALL(                                         void   , glClearColor                 , GLclampf,red, GLclampf,blue, GLclampf,green, GLclampf,alpha)\
EMP_GL_CALL(                                         void   , glColorMask                  , GLboolean,red, GLboolean,green, GLboolean,blue, GLboolean,alpha)\
EMP_GL_CALL_EX(PFNGLCOMPILESHADERPROC              , void   , glCompileShader              , GLuint,shader)\
EMP_GL_CALL_EX(PFNGLCREATEPROGRAMPROC              , GLuint , glCreateProgram              , void)\
EMP_GL_CALL_EX(PFNGLCREATESHADERPROC               , GLuint , glCreateShader               , GLenum,type)\
EMP_GL_CALL(                                         void   , glCullFace                   , GLenum,mode)\
EMP_GL_CALL_EX(PFNGLDELETEBUFFERSPROC              , void   , glDeleteBuffers              , GLsizei,n, GLuint const*,buffers)\
EMP_GL_CALL_EX(PFNGLDELETEFRAMEBUFFERSPROC         , void   , glDeleteFramebuffers         , GLsizei,n, GLuint const*,framebuffers)\
EMP_GL_CALL_EX(PFNGLDELETEPROGRAMPROC              , void   , glDeleteProgram              , GLuint,program)\
EMP_GL_CALL_EX(PFNGLDELETERENDERBUFFERSPROC        , void   , glDeleteRenderbuffers        , GLsizei,n, GLuint const*,renderbuffers)\
EMP_GL_CALL_EX(PFNGLDELETESHADERPROC               , void   , glDeleteShader               , GLuint,shader)\
EMP_GL_CALL(                                         void   , glDeleteTextures             , GLsizei,n, GLuint const*,textures)\
EMP_GL_CALL(                                         void   , glDepthMask                  , GLboolean,flag)\
EMP_GL_CALL(                                         void   , glDepthFunc                  , GLenum,func)\
EMP_GL_CALL_EX(PFNGLDETACHSHADERPROC               , void   , glDetachShader               , GLuint,program, GLuint,shader)\
EMP_GL_CALL(                                         void   , glDisable                    , GLenum,cap)\
EMP_GL_CALL(                                         void   , glDrawElements               , GLenum,mode, GLsizei,count, GLenum,type, GLvoid const*,indices)\
EMP_GL_CALL_EX(PFNGLDRAWRANGEELEMENTSPROC          , void   , glDrawRangeElements          , GLenum,mode, GLuint,start, GLuint,end, GLsizei,count, GLenum,type, GLvoid const*,indices)\
EMP_GL_CALL(                                         void   , glEnable                     , GLenum,cap)\
EMP_GL_CALL_EX(PFNGLENABLEVERTEXATTRIBARRAYPROC    , void   , glEnableVertexAttribArray    , GLuint,index)\
EMP_GL_CALL_EX(PFNGLFRAMEBUFFERRENDERBUFFERPROC    , void   , glFramebufferRenderbuffer    , GLenum,target, GLenum,attachment, GLenum,renderbufferTarget, GLuint,renderbuffer)\
EMP_GL_CALL_EX(PFNGLFRAMEBUFFERTEXTURE2DPROC       , void   , glFramebufferTexture2D       , GLenum,target, GLenum,attachment, GLenum,textarget, GLuint,texture, GLint,level)\
EMP_GL_CALL(                                         void   , glFrontFace                  , GLenum,mode)\
EMP_GL_CALL_EX(PFNGLGENBUFFERSPROC                 , void   , glGenBuffers                 , GLsizei,n, GLuint*,buffers)\
EMP_GL_CALL_EX(PFNGLGETBUFFERPARAMETERIVPROC       , void   , glGetBufferParameteriv       , GLenum,target, GLenum,value, GLint*,data)\
EMP_GL_CALL_EX(PFNGLGENFRAMEBUFFERSPROC            , void   , glGenFramebuffers            , GLsizei,n, GLuint*,framebuffers)\
EMP_GL_CALL_EX(PFNGLGETPROGRAMIVPROC               , void   , glGetProgramiv               , GLuint,program, GLenum,pname, GLint*,params)\
EMP_GL_CALL_EX(PFNGLGETPROGRAMINFOLOGPROC          , void   , glGetProgramInfoLog          , GLuint,program, GLsizei,max, GLsizei*,length, GLchar*,log)\
EMP_GL_CALL_EX(PFNGLGETSHADERIVPROC                , void   , glGetShaderiv                , GLuint,shader, GLenum,pname, GLint*,params)\
EMP_GL_CALL_EX(PFNGLGETSHADERINFOLOGPROC           , void   , glGetShaderInfoLog           , GLuint,shader, GLsizei,max, GLsizei*,length, GLchar*,log)\
EMP_GL_CALL_EX(PFNGLGETRENDERBUFFERPARAMETERIVPROC , void   , glGetRenderbufferParameteriv , GLenum,target, GLenum,pname, GLint*,params)\
EMP_GL_CALL_EX(PFNGLGENRENDERBUFFERSPROC           , void   , glGenRenderbuffers           , GLsizei,n, GLuint*,renderbuffers)\
EMP_GL_CALL(                                         void   , glGenTextures                , GLsizei,n, GLuint*,textures)\
EMP_GL_CALL_EX(PFNGLGETUNIFORMLOCATIONPROC         , GLint  , glGetUniformLocation         , GLuint,program, GLchar const*,name)\
EMP_GL_CALL_EX(PFNGLLINKPROGRAMPROC                , void   , glLinkProgram                , GLuint,program)\
EMP_GL_CALL_EX(PFNGLRENDERBUFFERSTORAGEPROC        , void   , glRenderbufferStorage        , GLenum,target, GLenum,internalFormat, GLsizei,width, GLsizei,height)\
EMP_GL_CALL(                                         void   , glScissor                    , GLint,x, GLint,y, GLsizei,width, GLsizei,height)\
EMP_GL_CALL_EX(PFNGLSHADERSOURCEPROC               , void   , glShaderSource               , GLuint,shader, GLsizei,count, GLchar const* const*,string, GLint const*,length)\
EMP_GL_CALL(                                         void   , glStencilFunc                , GLenum,func, GLint,ref, GLuint,mask)\
EMP_GL_CALL(                                         void   , glStencilMask                , GLuint,mask)\
EMP_GL_CALL(                                         void   , glStencilOp                  , GLenum,sfail, GLenum,dpfail, GLenum,dppass)\
EMP_GL_CALL(                                         void   , glTexImage2D                 , GLenum,target, GLint,level, GLint,internalFormat, GLsizei,width,\
                                                     GLsizei,height, GLint,border, GLenum,format, GLenum,type, const GLvoid*,data)\
EMP_GL_CALL_EX(PFNGLTEXIMAGE2DMULTISAMPLEPROC      ,void   , glTexImage2DMultisample       , GLenum,target,  GLsizei,samples, GLenum,internalformat, GLsizei,width, GLsizei,height, GLboolean,fixedsamplelocations)\
EMP_GL_CALL(                                         void   , glTexParameteri              , GLenum,target, GLenum,pname, GLint,param)\
EMP_GL_CALL_EX(PFNGLUNIFORM1IPROC                  , void   , glUniform1i                  , GLint,location, GLint,value)\
EMP_GL_CALL_EX(PFNGLUNIFORM1FVPROC                 , void   , glUniform1fv                 , GLint,location, GLsizei,count, GLfloat const*,value)\
EMP_GL_CALL_EX(PFNGLUNIFORM4FVPROC                 , void   , glUniform4fv                 , GLint,location, GLsizei,count, GLfloat const*,value)\
EMP_GL_CALL_EX(PFNGLUNIFORMMATRIX4FVPROC           , void   , glUniformMatrix4fv           , GLint,location, GLsizei,count, GLboolean,transpose, GLfloat const*,value)\
EMP_GL_CALL_EX(PFNGLUSEPROGRAMPROC                 , void   , glUseProgram                 , GLuint,program)\
EMP_GL_CALL_EX(PFNGLVERTEXATTRIBPOINTERPROC        , void   , glVertexAttribPointer        , GLuint,index, GLint,size, GLenum,type, GLboolean,normalized, GLsizei,stride, GLvoid const*,pointer)\
EMP_GL_CALL(                                         void   , glViewport                   , GLint,x, GLint,y, GLsizei,width, GLsizei,height)

#if defined EMP_GLES
#define EMP_GL_CALLS_MOBILE()\
EMP_GL_CALL(void , glReadPixels           , GLint,x, GLint,y, GLsizei,width, GLsizei,height, GLenum,format, GLenum,type, GLvoid*,pixels)\
EMP_GL_CALL(void , glInvalidateFramebuffer, GLenum,target, GLsizei,numAttachments, const GLenum*,attachments)
#elif defined EMP_GL
#define EMP_GL_CALLS_DESKTOP()\
EMP_GL_CALL(void , glGetTexImage , GLenum,target, GLint,level, GLenum,format, GLenum,type, void*,pixels)
#else
#error unknown gl version
#endif

#ifdef EMP_XX_OS_WIN
#define EMP_GL_CALLS()\
EMP_GL_CALLS_COMMON()\
EMP_GL_CALLS_DESKTOP()\
EMP_GL_CALL_EX(PFNWGLSWAPINTERVALEXTPROC           , int  , wglSwapIntervalEXT    , int,interval)\
EMP_GL_CALL_EX(PFNWGLGETSWAPINTERVALEXTPROC        , int  , wglGetSwapIntervalEXT , void)

#elif defined EMP_XX_OS_LIN
#define EMP_GL_CALLS()\
EMP_GL_CALLS_COMMON()\
EMP_GL_CALLS_DESKTOP()\
EMP_GL_CALL(int, glXSwapIntervalMESA   , unsigned int,interval)\
EMP_GL_CALL(int, glXSwapIntervalSGI    , int,interval)\
EMP_GL_CALL(int, glXGetSwapIntervalMESA, void)
// @@0 not foundEMP_GL_CALL(int, glXGetSwapIntervalSGI , void)

#elif defined EMP_XX_OS_OSX
#define EMP_GL_CALLS()\
EMP_GL_CALLS_COMMON()\
EMP_GL_CALLS_DESKTOP()

#elif defined EMP_XX_OS_IOS
#define EMP_GL_CALLS()\
EMP_GL_CALLS_COMMON()\
EMP_GL_CALLS_MOBILE()

#endif

#define EMP_GL_HAS(x_Name) EMP_PP_CAT(has_, x_Name)
#define EMP_GL_FCT(x_Name) EMP_PP_CAT(m_pFct_, x_Name)
#define EMP_GL_PROC(x_Name) EMP_PP_CAT(x_Name, _)
#define EMP_GL_TRY(x_Name) EMP_PP_CAT(x_Name, __)
#define EMP_GL_PFN(x_Name) EMP_PP_CAT(pfn_, x_Name)

#define EMP_GL_CALL_IMPL_EX(x_Ext, x_NotVoid, x_Count, x_Name, x_Ret, ...)\
EMP_PP_IF(x_Ext,\
private:\
    typedef x_Ret(EMP_PP_IF(EMP_GFX_GLAPIENTRY, APIENTRY) *EMP_GL_PFN(x_Name))(EMP_PP_FORWARD(EMP_GL_PARAMS(x_Count, __VA_ARGS__)));\
    EMP_GL_PFN(x_Name) EMP_GL_FCT(x_Name);)\
public:\
    EMP_RETURN bool EMP_GL_HAS(x_Name)(void) const { return EMP_PP_IF_ELSE(x_Ext, EMP_GL_FCT(x_Name) != nullptr, true); }\
    EMP_PP_IF(x_NotVoid, EMP_RETURN) x_Ret EMP_GL_PROC(x_Name) (EMP_PP_FORWARD(EMP_GL_PARAMS(x_Count, __VA_ARGS__))) const\
    {\
        EMP_PP_IF(x_NotVoid, x_Ret const oReturn =) EMP_PP_IF_ELSE(x_Ext, EMP_GL_FCT(x_Name), x_Name)(EMP_GL_ARGS(x_Count, __VA_ARGS__));\
        EMP_ASSERT(success());\
        EMP_PP_IF(x_NotVoid, return oReturn;)\
    }\
    EMP_RETURN bool EMP_GL_TRY(x_Name) (EMP_PP_IF(x_NotVoid, x_Ret& a_rReturn)\
        EMP_PP_COMMA_IF(EMP_PP_AND(EMP_PP_BOOL(x_Count), x_NotVoid))\
            EMP_PP_FORWARD(EMP_GL_PARAMS(x_Count, __VA_ARGS__))) const\
    {\
        EMP_PP_IF(x_NotVoid, a_rReturn =) EMP_PP_IF_ELSE(x_Ext, EMP_GL_FCT(x_Name), x_Name)(EMP_GL_ARGS(x_Count, __VA_ARGS__));\
        return success();\
    }

#define EMP_GL_PARAM_COUNT_1  0
#define EMP_GL_PARAM_COUNT_2  0
#define EMP_GL_PARAM_COUNT_3  1
#define EMP_GL_PARAM_COUNT_5  2
#define EMP_GL_PARAM_COUNT_7  3
#define EMP_GL_PARAM_COUNT_9  4
#define EMP_GL_PARAM_COUNT_11 5
#define EMP_GL_PARAM_COUNT_13 6
#define EMP_GL_PARAM_COUNT_15 7
#define EMP_GL_PARAM_COUNT_17 8
#define EMP_GL_PARAM_COUNT_19 9
#define EMP_GL_PARAM_COUNT_21 10
#define EMP_GL_PARAM_COUNT_23 11

#define EMP_GL_CALL(x_Ret, x_Name, ...)\
    EMP_PP_FORWARD(EMP_GL_CALL_IMPL_EX(false, EMP_PP_NOT(EMP_PP_IS_VOID(x_Ret)), EMP_PP_FORWARD(EMP_PP_CAT(EMP_GL_PARAM_COUNT_, EMP_VAARGS_COUNT(x_Ret, __VA_ARGS__))), x_Name, x_Ret, __VA_ARGS__))

#define EMP_GL_CALL_EX(x_PFN, x_Ret, x_Name, ...)\
    EMP_PP_FORWARD(EMP_GL_CALL_IMPL_EX(true, EMP_PP_NOT(EMP_PP_IS_VOID(x_Ret)), EMP_PP_FORWARD(EMP_PP_CAT(EMP_GL_PARAM_COUNT_, EMP_VAARGS_COUNT(x_Ret, __VA_ARGS__))), x_Name, x_Ret, __VA_ARGS__))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fs { class file_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name gl_version_e
#if defined EMP_GLES
#define enum_values\
    enum_value( none )\
    enum_value( ES30 )\
    enum_value( ES20 )\
    enum_value( ES10 )
#elif defined EMP_GL
#define enum_values\
    enum_value( none )\
    enum_value( DS21 )
#else
#error "Unknown gl system"
#endif
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name glsl_version_e
#if defined EMP_GLES
#define enum_values\
    enum_value( none  )\
    enum_value( SL300 )\
    enum_value( SL200 )\
    enum_value( SL100 )
#elif defined EMP_GL
#define enum_values\
    enum_value( none  )\
    enum_value( SL120 )
#else
#error "Unknown gl system"
#endif
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::mat;
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_GFX_SLOT_COUNT 32
#define EMP_GFX_FRAME_COUNT 4

typedef array<vector<size_t>, EMP_GFX_SLOT_COUNT, true> pool_d;
typedef array<pool_d, EMP_GFX_FRAME_COUNT, true> cycle_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct texture_s
{
    ui32 name = 0;
    ui32 depth = 0;
    si32 width = 0;
    si32 height = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct buffer_s
{
    ui32 m_ui32Name = 0;
    ui32 m_ui32Target = 0;
    ui32 m_ui32Usage = 0;
    GLsizeiptr m_siptrSize = 0;
    size_t m_stSlot = 0;
    size_t m_stIndex = 0;
    size_t m_stAllocated = 0;
    cycle_d* m_pCycle = nullptr;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct gl_framebuffer_s
{
    ui32 m_ui32Index = 0;
    si32 m_siW = 0;
    si32 m_siH = 0;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
template <typename T, typename E>
struct gl_value_t
{
    tt::aliteral name;
    E key;
    T location;
    GLenum type = 0; // @@0 ?? gl_value_t
    si32 count = 0;
    si32 stride = 0;

    static EMP_RETURN gl_value_t const make(tt::c_aliteral& a_sName, E const a_eKey, T a_tLocation,
        GLenum a_eType, c_si32 a_siCount, c_si32 a_siStride)
    {
        gl_value_t oValue;
        oValue.name = a_sName;
        oValue.key = a_eKey;
        oValue.location = a_tLocation;
        oValue.type = a_eType;
        oValue.count = a_siCount;
        oValue.stride = a_siStride;
        return oValue;
    }

    static EMP_RETURN gl_value_t const make(tt::c_aliteral& a_sName, E const a_eKey, T a_tLocation)
    { return make(a_sName, a_eKey, a_tLocation, -1, -1, -1); }

    static EMP_RETURN gl_value_t const make(tt::c_aliteral& a_sName, E const a_eKey)
    { return make(a_sName, a_eKey, -1, 0, -1, -1); }
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef gl_value_t<size_t, attribute_e> gl_attribute_s;
typedef gl_value_t<int, uniform_e> gl_uniform_s;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
struct program_s
{
    size_t EMP_TT_MAX_VAR(vertex);
    size_t EMP_TT_MAX_VAR(pixel);

    enum_array_of(gl_attribute_s, attribute_e) attributes;
    enum_array_of(gl_uniform_s, uniform_e) uniforms;
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, gl_device_c)
private:
    unsigned int m_VAO;
    static gl_device_c* s_pInstance;
public:
    #define EMP_GRAPHICS emp::gfx::gl_device_c::instance()
    static void set_instance(gl_device_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN gl_device_c& instance(void) { return *s_pInstance; }

public:
    EMP_GL_CALLS()
#undef EMP_GL_CALL
#undef EMP_GL_CALL_EX
#undef EMP_GL_CALL_IMPL

    emp::cnt::vector<float> m_vfScratchPad{};
    emp::cnt::vector<emp::cnt::shell<v3f>> m_vhv3fScratchPad{};
    emp::cnt::vector<ui8> m_vui8ScratchPad{};
    vector<ui8*> m_vpui8ScratchPad{};

private:
    static constexpr const int c_stSlotCount = EMP_GFX_SLOT_COUNT;
    static constexpr const int c_stFrameCount = EMP_GFX_FRAME_COUNT;

    emp::cnt::vector<buffer_s> m_vBuffers{};
    emp::cnt::vector<target_s> m_vTargets{};
    emp::cnt::vector<ui32> m_vui32Targets{};


    cycle_d m_IndexCycle{};
    cycle_d m_VertexCycle{};
    // @@0 frame size_t m_stFrame;

    enum_array_of(gl_attribute_s, attribute_e) m_aAttributes{true};
    enum_array_of(gl_uniform_s, uniform_e) m_aUniforms{true};

    vector<program_s*> m_vpPrograms{};
    map_t<astring, size_t> m_mstPrograms{16};
    vector<uint> m_vuiPrograms{};

    map_t<astring, size_t> m_mstShaders{32};
    vector<uint> m_vuiShaders{};

    v4f m_v4fClearColor{};
    bool m_bColorMask = true;
    bool m_bDepthMask = true;
    bool m_bDepthTest = false;
    bool m_bStencilTest = false;
    bool m_bBlend = false;
    bool m_bScissor = false;
    bool m_bDither = true;
    bool m_bPolygonSmooth = false;
    bool m_bValid = false;

    GLenum m_eStencilFunc = GL_ALWAYS;
    GLint m_siStencilFuncReference = 0;
    GLuint EMP_TT_MAX_VAR(m_uiStencilFuncMask);

    GLenum m_eStencilOpSf = GL_KEEP;
    GLenum m_eStencilOpSp = GL_KEEP;

    GLuint EMP_TT_MAX_VAR(m_uiStencilMask);

    int m_si8Stencil = 0;

    GLenum m_eSrcBlend = GL_ZERO;
    GLenum m_eDstBlend = GL_ZERO;

    GLenum m_eFrontFace = GL_CCW;
    GLenum m_eCullFace = GL_NONE;

    gl_version_e m_eGlVersion = gl_version_e::none;
    glsl_version_e m_eGlslVersion = glsl_version_e::none;

    emp::str::astring m_sVertexMacro{};
    c_char* m_szVertexMacro = nullptr;
    int m_siVertexMacro = 0;

    emp::str::astring m_sPixelMacro{};
    c_char* m_szPixelMacro = nullptr;
    int m_siPixelMacro = 0;

    size_t EMP_TT_MAX_VAR(m_stColorUniformProgram);
    size_t EMP_TT_MAX_VAR(m_stColorAttributeProgram);
    size_t EMP_TT_MAX_VAR(m_stPathProgram);
    size_t EMP_TT_MAX_VAR(m_stTextureProgram);

    vector<size_t> m_vstTransitionPrograms{};

    program_s* m_pCurrentProgram = nullptr;
    buffer_i* m_pCurrentVertexBuffer = nullptr;
    buffer_i* m_pCurrentIndexBuffer = nullptr;

    EMP_POINTER(buffer_t<ui8>) m_pQuadIndexBuffer{};
    EMP_POINTER(buffer_t<ui8>) m_pLineIndexBuffer{};

    map_t<size_t, buffer_t<ui8>*> m_mpLineIndexBuffers{8};

    EMP_POINTER(buffer_t<v3f, v4f>) m_pGlQuadVertices{};
    EMP_POINTER(buffer_t<v3f, v4f>) m_pQuadVertices{};

    EMP_POINTER(buffer_t<v3f>) m_pv3fBuffer{};

    array<buffer_t<ui8>, 16, true> m_vBorderIndexBuffers{};
    vector<declaration_c*> m_vpDeclarations{};

    v4f m_v4Viewport = {0, 0, 0, 0};
    v4f m_v4Scissor = {0, 0, 0, 0};
    v4<si32> m_v4si32Scissor = {0, 0, 0, 0};

    gl_framebuffer_s m_Framebuffer;

#if defined(EMP_XX_PROFILE)
    map_t<ui32, texture_s> m_mTextures(32);
#endif

public:
    void unlock_buffer(c_size a_stBuffer);

private:
    void unlock_buffer(buffer_s* const a_pBuffer);
    EMP_RETURN buffer_s* lock_buffer(cycle_d& a_rCycle, c_usage_e a_eUsage, c_ui32 a_ui32Target, c_size a_stSize);

    EMP_RETURN ui32 new_name(void)
    {
        ui32 EMP_TT_MAX_VAR(ui32Name);
        glGenBuffers_(1, &ui32Name); // @@1 cached GenBuffer
        return ui32Name;
    }

public:
    void end_of_frame(void);

public:
    gl_device_c(void);
    ~gl_device_c(void);

public:
    EMP_RETURN bool valid(void) const { return m_bValid; }
public:
    EMP_RETURN bool init(void);
    EMP_RETURN bool create(void);
    EMP_RETURN bool create_resources(c_astring& a_sDirectory);
    void destroy(void);

    EMP_RETURN bool clear_target(void);
    EMP_RETURN bool clear_target(c_v4f& a_rv4Color);

public:
    EMP_RETURN gl_version_e gl_version(void) const { return m_eGlVersion; }
    EMP_RETURN glsl_version_e glsl_version(void) const { return m_eGlslVersion; }

public:
    EMP_RETURN buffer_t<v3f>* v3f_buffer(void) const { return m_pv3fBuffer; }
    EMP_RETURN buffer_t<v3f, v4f>* gl_quad_vertex_buffer(void) const { return m_pGlQuadVertices; }
    EMP_RETURN buffer_t<v3f, v4f>* quad_vertex_buffer(void) const { return m_pQuadVertices; }
    EMP_RETURN buffer_t<ui8>* quad_index_buffer(void) const { return m_pQuadIndexBuffer; }
    EMP_RETURN buffer_t<ui8>* border_index_buffer(c_size a_stIndex) { return m_vBorderIndexBuffers.ptr(a_stIndex); }
    EMP_RETURN buffer_t<ui8>* line_index_buffer(c_size a_stCount);

public:
    EMP_RETURN size_t color_uniform_program(void) const { return m_stColorUniformProgram; }
    EMP_RETURN size_t color_attribute_program(void) const { return m_stColorAttributeProgram; }
    EMP_RETURN size_t path_program(void) const { return m_stPathProgram; }
    EMP_RETURN size_t texture_program(void) const { return m_stTextureProgram; }
    
    EMP_RETURN size_t transition_programs(c_size a_stIndex) const
    {
        if (m_vstTransitionPrograms.is_valid_index(a_stIndex))
        { return m_vstTransitionPrograms.val(a_stIndex); }
        return emp::tt::max<size_t>::value;
    }

    EMP_RETURN size_t transition_programs_count(void) const
    { return m_vstTransitionPrograms.count(); }

public:
    void set_clear_color(v4f::tparam a_v4fColor);
    EMP_RETURN v4f::tret clear_color(void) const { return m_v4fClearColor; }

    void set_color_mask(c_bool a_bValue);
    EMP_RETURN bool color_mask(void) const { return m_bColorMask; }

    void set_depth_mask(c_bool a_bValue);
    EMP_RETURN bool depth_mask(void) const { return m_bDepthMask; }

    void set_depth_test(c_bool a_bValue);
    EMP_RETURN bool depth_test(void) const { return m_bDepthTest; }

    void set_stencil_test(c_bool a_bValue);
    EMP_RETURN bool stencil_test(void) const { return m_bStencilTest; }

    void set_stencil_func(GLenum const a_eFunc, GLint const a_siReference, GLuint const a_uiMask);
    EMP_RETURN GLenum stencil_func(void) const { return m_eStencilFunc; }
    EMP_RETURN GLint stencil_func_reference(void) const { return m_siStencilFuncReference; }
    EMP_RETURN GLuint stencil_func_mask(void) const { return m_uiStencilFuncMask; }

    void set_stencil_op(GLenum const a_eSf, GLenum const a_eSp);
    EMP_RETURN GLenum stencil_op_sf(void) const { return m_eStencilOpSf; }
    EMP_RETURN GLenum stencil_op_sp(void) const { return m_eStencilOpSp; }

    void set_stencil_mask(GLuint const a_ui8Mask);
    EMP_RETURN GLuint stencil_mask(void) const { return m_uiStencilMask; }

    void set_stencil(c_int a_siValue) { m_si8Stencil = a_siValue; }
    EMP_RETURN int stencil(void) const { return m_si8Stencil; }

    void increment_stencil(void);
    void decrement_stencil(void);

    void set_blend(c_bool a_bValue);
    void set_blend_func(GLenum const a_eScr, GLenum const a_eDst);

    void set_dither(c_bool a_bValue);
    void set_polygon_smooth(c_bool a_bValue);

    void set_scissor(c_bool a_bValue);

    void set_front_face(GLenum const a_eValue);
    void set_cull_face(GLenum const a_eValue);

    EMP_RETURN v4f viewport(void) const { return m_v4Viewport; }
    EMP_RETURN bool viewport(c_v4f a_v4Value);
    EMP_RETURN bool viewport(c_f64 a_fW, c_f64 a_fH);
    EMP_RETURN bool viewport(c_f64 a_fX, c_f64 a_fY, c_f64 a_fW, c_f64 a_fH);

    EMP_RETURN v4f scissor(void) const { return m_v4Scissor; }
    EMP_RETURN bool scissor(c_v4f& a_v4Value, bool& a_rbEmpty) { return scissor(a_v4Value.x, a_v4Value.y, a_v4Value.z, a_v4Value.w, a_rbEmpty); }
    EMP_RETURN bool scissor(c_f64 a_fX, c_f64 a_fY, c_f64 a_fW, c_f64 a_fH, bool& a_rbEmpty);

    EMP_RETURN bool framebuffer(c_ui32 a_ui32Index, target_s& a_rTarget)
    {
        EMP_RET_FALSE_IF(emp::tt::is_max(a_ui32Index));
        EMP_RET_FALSE_IF_NOT(m_vTargets.is_valid_index(a_ui32Index));
        a_rTarget = m_vTargets.ref(a_ui32Index);
        EMP_RET_FALSE_IF_NOT(a_rTarget.m_ui32Width > 0 && a_rTarget.m_ui32Height > 0);
        return true;
    }

    EMP_RETURN bool lock_texture(c_astring& a_sFilename, ui32& a_rui32Texture, c_ui32 a_ui32Color, c_bool a_bDelete);
    EMP_RETURN bool unlock_texture(c_ui32 a_ui32Texture);

    EMP_RETURN size_t framebuffer_count(void) const { return m_vTargets.count(); }
    EMP_RETURN bool lock_framebuffer(c_ui32 a_ui32Width, c_ui32 a_ui32Height, c_bool a_bResizable, ui32& a_rui32Index);
    EMP_RETURN bool unlock_framebuffer(c_ui32 a_ui32Index);
    EMP_RETURN bool allocate_framebuffer(target_s& a_rTarget);

#ifdef EMP_GLES
    EMP_RETURN bool discard_framebuffer();
    EMP_RETURN bool discard_framebuffers(void);
#endif

    EMP_RETURN bool bind_framebuffer(c_ui32 a_ui32Framebuffer, c_f64 a_fW, c_f64 a_fH);
    EMP_RETURN bool bind_framebuffer(target_s const& a_rTarget);

    EMP_RETURN gl_framebuffer_s const& current_framebuffer(void) const { return m_Framebuffer; }

    EMP_RETURN bool framebuffer_status(void) const;

    EMP_RETURN bool resize(c_f64 a_dWidth, c_f64 a_dHeight);


    void set_uniform(uniform_e a_eIndex, m44f::tparam a_m44Value, c_bool a_bTranspose = false) const;
    void set_uniform(uniform_e a_eIndex, v4f::tparam a_v4Value) const;
    void set_uniform(uniform_e a_eIndex, c_float a_fValue) const;
    void set_uniform(uniform_e a_eIndex, c_int a_siSlot, c_uint a_uiTexture) const;
    void set_program(c_size a_stProgram);

    template <typename...T>
    void set_vertex_buffer(buffer_t<T...>* const a_pBuffer)
    {
        m_pCurrentVertexBuffer = a_pBuffer;
        EMP_ASSERT(m_pCurrentVertexBuffer == nullptr || m_pCurrentVertexBuffer->declaration() != nullptr);
    }

    void set_vertex_buffer(buffer_i* const a_pBuffer)
    {
        m_pCurrentVertexBuffer = a_pBuffer;
        EMP_ASSERT(m_pCurrentVertexBuffer == nullptr || m_pCurrentVertexBuffer->declaration() != nullptr);
    }

    template <typename T>
    void set_index_buffer(buffer_t<T>* const a_pBuffer)
    {
        m_pCurrentIndexBuffer = a_pBuffer;
        EMP_ASSERT(m_pCurrentIndexBuffer == nullptr || m_pCurrentIndexBuffer->declaration() != nullptr);
    }

    void set_index_buffer(buffer_i* const a_pBuffer)
    {
        m_pCurrentIndexBuffer = a_pBuffer;
        EMP_ASSERT(m_pCurrentIndexBuffer == nullptr || m_pCurrentIndexBuffer->declaration() != nullptr);
    }

    void draw(c_primitive_e a_ePrimitive);
    EMP_RETURN bool read_pixels(emp::gfx::target_s const& a_rTarget, vector<ui32>& a_rvui32Pixels, c_bool a_bAlpha);


public:
    template <typename... T>
    EMP_RETURN declaration_c const* get_declaration(c_size a_stIndex) const
    {
        if (m_vpDeclarations.is_valid_index(a_stIndex))
        { return m_vpDeclarations.val(a_stIndex); }
        return nullptr;
    }

    template <typename... T>
    EMP_RETURN size_t reg_declaration(void);

    EMP_RETURN size_t reg_declaration_impl(shell<attribute_s const> const& a_vAttributes)
    {
        c_size stAttributeCount = a_vAttributes.count();
        c_size stReturnFailure = emp::tt::max<emp::tt::try_remove_const<decltype(stAttributeCount)>>::value;
        EMP_RET_X_IF(stReturnFailure, stAttributeCount == 0);
        c_size stCount = m_vpDeclarations.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            declaration_c const* pDeclaration = m_vpDeclarations.val(stIndex);
            auto const& vAttributes = pDeclaration->m_vAttributes;
            if (stAttributeCount == vAttributes.count())
            {
                bool bFound = true;
                for (size_t stAttribute = 0; stAttribute < stAttributeCount; ++stAttribute)
                {
                    if (a_vAttributes.ref(stAttribute) != vAttributes.ref(stAttribute))
                    {
                        bFound = false;
                        break;
                    }
                }

                if (bFound)
                { return stIndex; }
            }
        }

        c_size stIndex = m_vpDeclarations.count();
        auto* const pDeclaration = EMP_ALLOCATE(declaration_c);
        m_vpDeclarations.add(pDeclaration);

        size_t stStride = 0;
        for (size_t stAttribute = 0; stAttribute < stAttributeCount; ++stAttribute)
        {
            attribute_s const& rAttribute = a_vAttributes.ref(stAttribute);
            pDeclaration->m_vAttributes.add(rAttribute);
            stStride += rAttribute.m_stCount * rAttribute.m_stSize;
        }

        pDeclaration->m_stStride = stStride;

        return stIndex;
    }

private:
    EMP_RETURN bool update_buffer(c_ui32 a_ui32Target, buffer_i* const a_pBuffer, cycle_d& a_rCycle, c_bool a_bStatic);

private:
    EMP_RETURN bool create_program(c_astring& a_sDirectory, c_astring& a_sVertex, c_astring& a_sPixel, size_t& a_rstProgram,
        c_shell<c_uniform_e>& a_vUniforms, c_shell<c_attribute_e>& a_vAttributes);

    EMP_RETURN bool create_shader(c_astring& a_sDirectory, c_astring& a_sFilename, shader_e a_eShader, size_t& a_rstShader);
    EMP_RETURN bool create_shader(emp::fs::file_c const& a_File, c_uint a_uiType, uint& a_ruiShader);
    EMP_RETURN bool create_shader(c_astring& a_sSource, c_uint a_uiType, uint& a_ruiShader);

    // mesh
private:
    void add_attribute(c_uint a_uiType, c_size a_stCount);
    void update(c_shell<c_float>& a_rvVertices, c_uint a_uiVertexUsage, c_bool a_bVertexChanged,
            c_shell<c_uint>& a_rvIndices, c_uint a_uiIndexUsage, c_bool a_bIndexChanged);
    void update_vertices(c_shell<c_float>& a_rvVertices, c_uint a_uiUsage, c_bool a_bChanged);
    void update_indices(c_shell<c_uint>& a_rvIndices, c_uint a_uiUsage, c_bool a_bChanged);

private:
    // error handling
    EMP_RETURN bool failure(void) const;
    EMP_RETURN bool success(void) const;

    EMP_RETURN bool compile_failure(c_uint a_uiShader) const;
    EMP_RETURN bool compile_success(c_uint a_uiShader) const;

    EMP_RETURN bool link_failure(c_uint a_uiShader) const;
    EMP_RETURN bool link_success(c_uint a_uiShader) const;

    template <bool t_bShader>
    void get_iv(GLuint shader, GLenum pname, GLint* params) const;

    template <bool t_bShader>
    void get_info_log(GLuint shader, GLsizei max, GLsizei* length, GLchar* log) const;

    template <bool t_bShader>
    EMP_RETURN bool glsl_success(c_uint a_uiShader, c_uint a_uiType) const;

    void set_flag(bool& a_rbCurrent, c_bool a_bValue, GLenum const& a_eFlag) const;

#if defined(EMP_XX_PROFILE)
public:
    void log_info(void) const;
#endif
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN gl_device_c& instance(void);
void set_instance(gl_device_c* const a_pValue);

template <typename... T>
EMP_INLINE EMP_RETURN declaration_c const* buffer_t<T...>::declaration(void) const
{ return EMP_GRAPHICS.get_declaration(s_stDeclaration); }

template <typename... T>
EMP_INLINE EMP_RETURN bool buffer_t<T...>::reg_declaration(void)
{
    if (emp::tt::is_max(s_stDeclaration))
    { s_stDeclaration = EMP_GRAPHICS.reg_declaration<T...>(); }
    return emp::tt::is_not_max(s_stDeclaration);
}

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<ui8>(void)
{ return reg_declaration_impl(shell<attribute_s const>({ { type_e::UI8, 1, sizeof(ui8) } })); }

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<ui16>(void)
{ return reg_declaration_impl(shell<attribute_s const>({ { type_e::UI16, 1, sizeof(ui16)} })); }

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v2f>(void)
{ return reg_declaration_impl(shell<attribute_s const>({ { type_e::F32, 2, sizeof(float) } })); }

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v3f>(void)
{ return reg_declaration_impl(shell<attribute_s const>({ { type_e::F32, 3, sizeof(float) } })); }

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v2f, v2f>(void)
{
    return reg_declaration_impl(shell<attribute_s const>({
        { type_e::F32, 2, sizeof(float) },
        { type_e::F32, 2, sizeof(float) } }));
}

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v3f, v2f>(void)
{
    return reg_declaration_impl(shell<attribute_s const>({
        { type_e::F32, 3, sizeof(float) },
        { type_e::F32, 2, sizeof(float) } }));
}

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v3f, v4f>(void)
{
    return reg_declaration_impl(shell<attribute_s const>({
        { type_e::F32, 3, sizeof(float) },
        { type_e::F32, 4, sizeof(float) } }));
}

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v2f, v4f>(void)
{
    return reg_declaration_impl(shell<attribute_s const>({
        { type_e::F32, 2, sizeof(float) },
        { type_e::F32, 4, sizeof(float) } }));
}

template <>
EMP_INLINE EMP_RETURN size_t gl_device_c::reg_declaration<v4f, v4f>(void)
{
    return reg_declaration_impl(shell<attribute_s const>({
        { type_e::F32, 4, sizeof(float) },
        { type_e::F32, 4, sizeof(float) } }));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

