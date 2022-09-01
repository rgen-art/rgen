//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gfx_gl_device.h"

#include "emp_gfx_model.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ast_cast.h"

#include "emp_fs_file.h"

#include "emp_png_util.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(sizeof(GLfloat) == sizeof(float), "");
static_assert(sizeof(GLuint) == sizeof(uint), "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
gl_device_c* gl_device_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
gl_device_c::gl_device_c(void):
#define EMP_GL_CALL(x_Ret, x_Name, ...)
#define EMP_GL_CALL_EX(x_PFN, x_Ret, x_Name, ...) EMP_GL_FCT(x_Name)(nullptr),
    EMP_GL_CALLS()
#undef EMP_GL_CALL
#undef EMP_GL_CALL_EX
m_Framebuffer()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
gl_device_c::~gl_device_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_OS_WIN
template <typename T>
EMP_RETURN T cast_proc(PROC a_pProc)
{ EMP_PRAGMA_IGNORE_W_UNSAFE_REINTERPRET_CAST(return reinterpret_cast<T>(a_pProc);) }
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::init(void)
{
    destroy();
#if defined EMP_GLES
    m_eGlVersion = gl_version_e::ES30;
    m_eGlslVersion = glsl_version_e::SL300;
#elif defined EMP_GL
    m_eGlVersion = gl_version_e::DS21;
    m_eGlslVersion = glsl_version_e::SL120;
#else
#error "Unknown gl system"
#endif

    c_gl_version_e eGlVersion = m_eGlVersion;
    c_glsl_version_e eGlslVersion = m_eGlslVersion;

    astring sMacro;
    astring sVertex;
    astring sPixel;

#if defined EMP_GLES
    if (eGlVersion == gl_version_e::ES30 &&
        eGlslVersion == glsl_version_e::SL300)
    {
        sMacro.append_literal(ALITERAL("#version 300 es\n"));
        sMacro.append_literal(ALITERAL("#define attribute in\n"));
        sMacro.append_literal(ALITERAL("#define texture2D texture\n"));
        sVertex.append_literal(ALITERAL("#define varying out\n"));
        sPixel.append_literal(ALITERAL("#define varying in\n"));
        sPixel.append_literal(ALITERAL("out highp vec4 o_v4Color;\n"));
    }
    else if (
        eGlVersion == gl_version_e::ES20 &&
        eGlslVersion == glsl_version_e::SL200)
    {
        sMacro.append_literal(ALITERAL("#version 100"));
        sMacro.append_literal(ALITERAL("#define o_v4Color gl_FragColor"));
    }
    else
    {
        EMP_LOG_ERROR("unknown gl/glsl version combination");
        return false;
    }

#elif defined EMP_GL
    if (eGlVersion == gl_version_e::DS21 &&
        eGlslVersion == glsl_version_e::SL120)
    {
        sMacro.append_literal(ALITERAL("#version 150\n"));
        sMacro.append_literal(ALITERAL("#define attribute in\n"));
        sMacro.append_literal(ALITERAL("#define texture2D texture\n"));
        sVertex.append_literal(ALITERAL("#define varying out\n"));
        sPixel.append_literal(ALITERAL("#define varying in\n"));
        sPixel.append_literal(ALITERAL("out highp vec4 o_v4Color;\n"));
    }
    else
    {
        EMP_LOG_ERROR("unknown gl/glsl version combination\n");
        return false;
    }
    sMacro.append_literal(ALITERAL("#define lowp\n"));
    sMacro.append_literal(ALITERAL("#define mediump\n"));
    sMacro.append_literal(ALITERAL("#define highp\n"));

#else
#error "Unknown gl system"
#endif

    sVertex.append_literal(ALITERAL("#define o_v4Position gl_Position\n"));

    {
        m_sVertexMacro.append_string(sMacro);
        m_sVertexMacro.append_string(sVertex);

        m_szVertexMacro = m_sVertexMacro.cstr();
        c_size stLen = m_sVertexMacro.len();
        EMP_RET_FALSE_IF(stLen > emp::tt::max<int>::value);
        m_siVertexMacro = emp::ast::cast<int>(stLen);
    }

    {
        m_sPixelMacro.append_string(sMacro);
        m_sPixelMacro.append_string(sPixel);

        m_szPixelMacro = m_sPixelMacro.cstr();
        c_size stLen = m_sPixelMacro.len();
        EMP_RET_FALSE_IF(stLen > emp::tt::max<int>::value);
        m_siPixelMacro = emp::ast::cast<int>(stLen);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::create(void)
{
#ifdef EMP_XX_OS_WIN
#define EMP_GL_CALL_EX(x_PFN, x_Ret, x_Name, ...)\
{\
    EMP_GL_FCT(x_Name) = cast_proc<x_PFN>(wglGetProcAddress(EMP_PP_STRING(x_Name)));\
    EMP_RET_FALSE_IF_NOT(EMP_GL_HAS(x_Name)(), "GetLastError #0", emp::win::GetLastErrorString());\
}
#define EMP_GL_CALL(x_Ret, x_Name, ...)\
{\
    EMP_RET_FALSE_IF_NOT(EMP_GL_HAS(x_Name)());\
}
//    EMP_GL_FCT(x_Name) = reinterpret_cast<EMP_GL_PFN(x_Name)>(wglGetProcAddress(EMP_PP_STRING(x_Name)));
#elif defined EMP_XX_OS_LIN
#define EMP_GL_CALL_EX(x_PFN, x_Ret, x_Name, ...)\
{\
    EMP_GL_FCT(x_Name) = reinterpret_cast<x_PFN>(glXGetProcAddress(reinterpret_cast<GLubyte const*>(EMP_PP_STRING(x_Name))));\
    EMP_RET_FALSE_IF_NOT(EMP_GL_HAS(x_Name)());\
}
#define EMP_GL_CALL(x_Ret, x_Name, ...)\
{\
    EMP_RET_FALSE_IF_NOT(EMP_GL_HAS(x_Name)());\
}
#elif defined EMP_XX_OS_OSX
#define EMP_GL_CALL_EX(x_PFN, x_Ret, x_Name, ...)\
{\
    void* pfn =dlsym(RTLD_DEFAULT, EMP_PP_STRING(x_Name));\
    reinterpret_cast<void*&>(EMP_GL_FCT(x_Name)) = pfn;\
    EMP_RET_FALSE_IF_NOT(EMP_GL_HAS(x_Name)());\
}
#define EMP_GL_CALL(x_Ret, x_Name, ...)\
{\
    EMP_RET_FALSE_IF_NOT(EMP_GL_HAS(x_Name)());\
}
#elif defined EMP_XX_OS_IOS
#define EMP_GL_CALL_EX(x_PFN, x_Ret, x_Name, ...) { EMP_GL_FCT(x_Name) = &x_Name; }
#define EMP_GL_CALL(x_Ret, x_Name, ...)
#endif


EMP_GL_CALLS()
#undef EMP_GL_CALL
#undef EMP_GL_CALL_EX


#define XTMP_UNI(x_Uniform, x_Name)\
    case uniform_e::x_Uniform:\
    { m_aUniforms.ref(enum_to__(uniform_e::x_Uniform)) = gl_uniform_s::make(ALITERAL(x_Name), uniform_e::x_Uniform); }
    c_uniform_e eUniform = uniform_e::none;
    m_aUniforms.resize();
    switch (eUniform)
    {
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
        case uniform_e::none:
        XTMP_UNI(m44View      , "u_m44View")
        XTMP_UNI(m44World     , "u_m44World")
        XTMP_UNI(m44Projection, "u_m44Projection")
        XTMP_UNI(m44Transform , "u_m44Transform")
        XTMP_UNI(m22Transform , "u_m22Transform")
        XTMP_UNI(v4Color      , "u_v4Color")
        XTMP_UNI(t2Texture_0  , "u_t2Texture_0")
        XTMP_UNI(t2Texture_1  , "u_t2Texture_1")
        XTMP_UNI(fTime        , "u_fTime")
        XTMP_UNI(fDuration    , "u_fDuration")
        XTMP_UNI(fProgress    , "u_fProgress")
        default:
        {}
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
    }
#undef XTMP_UNI

#define XTMP_ATR(x_Type, x_Count, x_Stride, x_Attribute, x_Name)\
    case attribute_e::x_Attribute:\
    {\
        m_aAttributes.ref(enum_to__(attribute_e::x_Attribute)) =\
            gl_attribute_s::make(ALITERAL(x_Name), attribute_e::x_Attribute, emp::tt::max<size_t>::value, x_Type, x_Count, x_Stride);\
    }
    c_attribute_e eAttribute = attribute_e::none;
    m_aAttributes.resize();
    switch (eAttribute)
    {
EMP_PRAGMA_PUSH_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
        case attribute_e::none:
        XTMP_ATR(GL_FLOAT, 4, 4, v4Position, "a_v4Position")
        XTMP_ATR(GL_FLOAT, 3, 3, v3Position, "a_v3Position")
        XTMP_ATR(GL_FLOAT, 2, 2, v2Position, "a_v2Position")
        XTMP_ATR(GL_FLOAT, 4, 4, v4Texcoord, "a_v4Texcoord")
        XTMP_ATR(GL_FLOAT, 3, 3, v3Texcoord, "a_v3Texcoord")
        XTMP_ATR(GL_FLOAT, 2, 2, v2Texcoord, "a_v2Texcoord")
        XTMP_ATR(GL_FLOAT, 4, 4, v4Color   , "a_v4Color")
        default:
        {}
EMP_PRAGMA_POP_IGNORE(EMP_W_IMPLICIT_FALLTHROUGH)
    }
#undef XTMP_ATR

    m_pQuadIndexBuffer->create(emp::gfx::usage_e::Static);
    emp::gfx::emit_quad(0, *m_pQuadIndexBuffer);
    // @@0 remove EMP_RET_FALSE_IF_NOT(update_buffer(GL_ELEMENT_ARRAY_BUFFER, m_pQuadIndexBuffer, m_IndexCycle, true));

    m_pLineIndexBuffer->create(emp::gfx::usage_e::Static);
    emp::gfx::emit_line(0, *m_pLineIndexBuffer);
    // @@0 remove EMP_RET_FALSE_IF_NOT(update_buffer(GL_ELEMENT_ARRAY_BUFFER, m_pLineIndexBuffer, m_IndexCycle, true));

    //
    c_size stCount = 16;
    m_vBorderIndexBuffers.resize(stCount);
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        auto& rIndices = m_vBorderIndexBuffers.ref(stIndex);
        rIndices.create(usage_e::Static);
        if (stIndex & (0x1 << 0))
        {
            rIndices.add(0);
            rIndices.add(2);
        }

        if (stIndex & (0x1 << 1))
        {
            rIndices.add(1);
            rIndices.add(3);
        }

        if (stIndex & (0x1 << 2))
        {
            rIndices.add(2);
            rIndices.add(3);
        }

        if (stIndex & (0x1 << 3))
        {
            rIndices.add(0);
            rIndices.add(1);
        }

        //EMP_RET_FALSE_IF_NOT(update_buffer(GL_ELEMENT_ARRAY_BUFFER, &rIndices, m_IndexCycle, true));
    }

    m_pQuadVertices->create(emp::gfx::usage_e::Static);
    m_pQuadVertices->add({ 0, 0, 0 }, { 0, 0, 0, 0});
    m_pQuadVertices->add({ 2, 0, 0 }, { 1, 0, 0, 0});
    m_pQuadVertices->add({ 0, 2, 0 }, { 0, 1, 0, 0});
    m_pQuadVertices->add({ 2, 2, 0 }, { 1, 1, 0, 0 });

    m_pGlQuadVertices->create(emp::gfx::usage_e::Static);
    m_pGlQuadVertices->add({ -1, -1, 0 }, { 0, 0, 0, 0 });
    m_pGlQuadVertices->add({  1, -1, 0 }, { 1, 0, 0, 0 });
    m_pGlQuadVertices->add({ -1,  1, 0 }, { 0, 1, 0, 0 });
    m_pGlQuadVertices->add({  1,  1, 0 }, { 1, 1, 0, 0 });

    m_vTargets.resize(128);
    m_bValid = true;
    glGenVertexArrays_(1, &m_VAO); // @@0 delete
    emp::tt::silent(success());

    glBindVertexArray_(m_VAO);
    emp::tt::silent(success());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::create_resources(c_astring& a_sDirectory)
{
    using namespace emp::cnt;

    EMP_RET_FALSE_IF_NOT(create_program(a_sDirectory, ALITERAL("_vs_p3_t4.glsl"), ALITERAL("_ps_color_uniform.glsl"), m_stColorUniformProgram,
        shell<c_uniform_e>({ uniform_e::m44Transform, uniform_e::v4Color }),
        shell<c_attribute_e>({ attribute_e::v3Position, attribute_e::v4Texcoord })));

    EMP_RET_FALSE_IF_NOT(create_program(a_sDirectory, ALITERAL("_vs_p3_t4.glsl"), ALITERAL("_ps_color_attribute.glsl"), m_stColorAttributeProgram,
        shell<c_uniform_e>({ uniform_e::m44Transform/*, uniform_e::v4Color*/ }),
        shell<c_attribute_e>({ attribute_e::v3Position, attribute_e::v4Texcoord })));

    EMP_RET_FALSE_IF_NOT(create_program(a_sDirectory, ALITERAL("_vs_p3_t4.glsl"), ALITERAL("_ps_path.glsl"), m_stPathProgram,
        shell<c_uniform_e>({ uniform_e::m44Transform, uniform_e::v4Color }),
        shell<c_attribute_e>({ attribute_e::v3Position, attribute_e::v4Texcoord })));

    EMP_RET_FALSE_IF_NOT(create_program(a_sDirectory, ALITERAL("_vs_p3_t4.glsl"), ALITERAL("_ps_texture.glsl"), m_stTextureProgram,
        shell<c_uniform_e>({ uniform_e::m44Transform, uniform_e::t2Texture_0, uniform_e::v4Color }),
        shell<c_attribute_e>({ attribute_e::v3Position, attribute_e::v4Texcoord })));

    m_vstTransitionPrograms.resize(0);

    for (size_t stIndex = 0; stIndex < 1; ++stIndex)
    {
        astring sPixelShader = STR_AFORMAT("ps_transition_#0.glsl", stIndex);
        size_t& rstProgram = m_vstTransitionPrograms.push();
        EMP_RET_FALSE_IF_NOT(create_program(a_sDirectory, ALITERAL("_vs_p3_t4.glsl"), sPixelShader, rstProgram,
            shell<c_uniform_e>({ uniform_e::m44Transform, uniform_e::t2Texture_0, uniform_e::t2Texture_1, /*uniform_e::fTime, uniform_e::fDuration,*/ uniform_e::fProgress }),
            shell<c_attribute_e>({ attribute_e::v3Position, attribute_e::v4Texcoord })));
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::create_program(c_astring& a_sDirectory, c_astring& a_sVertex, c_astring& a_sPixel,
    size_t& a_rstProgram, shell<c_uniform_e> const& a_vUniforms, shell<c_attribute_e> const& a_vAttributes)
{
    EMP_TT_MAX_VAR(a_rstProgram);
    c_astring sProgram = STR_AFORMAT("#0.#1", a_sVertex, a_sPixel);

    {
        auto const& oIt = m_mstPrograms.get(sProgram);
        if (oIt != m_mstPrograms.end())
        {
            a_rstProgram = *oIt.value();
            return true;
        }
    }
    c_uint uiGlProgram = glCreateProgram_();
    EMP_RET_FALSE_IF(failure() || uiGlProgram == 0, "failed to create program : #0/#1 : #2", a_sVertex,a_sPixel, uiGlProgram);
    m_vuiPrograms.add(uiGlProgram);

    size_t EMP_TT_MAX_VAR(stVertex);
    EMP_RET_FALSE_IF_NOT(create_shader(a_sDirectory, a_sVertex, shader_e::Vertex, stVertex));

    size_t EMP_TT_MAX_VAR(stPixel);
    EMP_RET_FALSE_IF_NOT(create_shader(a_sDirectory, a_sPixel, shader_e::Pixel, stPixel));

    c_uint uiGlVertex = m_vuiShaders.val(stVertex);
    c_uint uiGlPixel = m_vuiShaders.val(stPixel);

    EMP_RET_FALSE_IF_NOT(glAttachShader__(uiGlProgram, uiGlVertex), "failed to attach vertex shader '#0'", a_sVertex);
    EMP_RET_FALSE_IF_NOT(glAttachShader__(uiGlProgram, uiGlPixel), "failed to attach pixel shader '#0'", a_sPixel);

    program_s* const pProgram = EMP_ALLOCATE(program_s);
    pProgram->vertex = stVertex;
    pProgram->pixel = stPixel;
    c_size stProgram = m_vpPrograms.count();
    a_rstProgram = stProgram;
    m_mstPrograms.set(sProgram, stProgram);
    m_vpPrograms.add(pProgram);

    pProgram->attributes.resize(enum_count(attribute_e));
    c_size stAttributeCount = a_vAttributes.count();
    for (uint uiIndex = 0; uiIndex < stAttributeCount; ++uiIndex)
    {
        c_size stAttribute = enum_to(a_vAttributes.val(uiIndex));
        gl_attribute_s const& rAttribute = m_aAttributes.ref(stAttribute);
        astring sName(rAttribute.name);
        EMP_RET_FALSE_IF(sName.is_empty(), "empty attribute name");
        EMP_RET_FALSE_IF_NOT(glBindAttribLocation__(uiGlProgram, uiIndex, sName.cstr()), "failed to bind attribute : #0", sName);
        gl_attribute_s oAttribute = rAttribute;
        oAttribute.location = uiIndex;
        pProgram->attributes.set(stAttribute, oAttribute);
    }

    glLinkProgram_(uiGlProgram);
    EMP_RET_FALSE_IF(failure() || link_failure(uiGlProgram), "failed to link program #0, #1", a_sVertex, a_sPixel);

    glUseProgram_(uiGlProgram);
    pProgram->uniforms.resize(enum_count(uniform_e));
    c_size stUniformCount = a_vUniforms.count();
    for (uint uiIndex = 0; uiIndex < stUniformCount; ++uiIndex)
    {
        c_size stUniform = enum_to(a_vUniforms.val(uiIndex));
        gl_uniform_s const& rUniform = m_aUniforms.ref(stUniform);
        astring sName(rUniform.name);
        EMP_RET_FALSE_IF(sName.is_empty(), "empty uniform name");

        GLint const siLocation = glGetUniformLocation_(uiGlProgram, rUniform.name.cstr());
        EMP_RET_FALSE_IF(failure() || siLocation < 0, "failed to get uniform location : #0 : #1", sName, siLocation);
        gl_uniform_s oUniform = rUniform;
        oUniform.location = siLocation;
        pProgram->uniforms.set(stUniform, oUniform);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::create_shader(c_astring& a_sDirectory, c_astring& a_sFilename, c_shader_e a_eShader, size_t& a_rstShader)
{
    EMP_TT_MAX_VAR(a_rstShader);

    {
        auto const& oIt = m_mstShaders.get(a_sFilename);
        if (oIt != m_mstShaders.end())
        {
            a_rstShader = *oIt.value();
            return true;
        }
    }

    EMP_LOG_INFO("compiling '#0'", a_sFilename);

    c_astring sSource = STR_AFORMAT("#0glsl/#1", a_sDirectory, a_sFilename);

    emp::fs::file_c oFile;
    EMP_RET_FALSE_IF_NOT(oFile.create(sSource, emp::fs::file_e::Binary, fs::create_e::Read), "failed to open shader file : #0", sSource);

    uint uiGlShader = 0;
    uint uiGlType = 0;
    switch (a_eShader)
    {
        case shader_e::Vertex: { uiGlType = GL_VERTEX_SHADER; break; }
        case shader_e::Pixel: { uiGlType = GL_FRAGMENT_SHADER; break; }
        case shader_e::none: default: { return false; }
    }

    a_rstShader = m_vuiShaders.count();

    EMP_RET_FALSE_IF_NOT(create_shader(oFile, uiGlType, uiGlShader), "failed to create shader from filename : #0", sSource);

    m_mstShaders.set(a_sFilename, a_rstShader);

    return uiGlShader != 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::create_shader(emp::fs::file_c const& a_File, c_uint a_uiType, uint& a_ruiShader)
{
    a_ruiShader = 0;
    astring sSource;
    EMP_RET_FALSE_IF_NOT(a_File.read_all(sSource), "failed to read file content");
    return create_shader(sSource, a_uiType, a_ruiShader);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::create_shader(c_astring& a_sSource, c_uint a_uiType, uint& a_ruiShader)
{
    a_ruiShader = 0;

    EMP_LOG_INFO("building shader : #0", a_sSource);

    EMP_RET_FALSE_IF(a_sSource.is_empty());
    EMP_RET_FALSE_IF_NOT(GL_VERTEX_SHADER == a_uiType || GL_FRAGMENT_SHADER == a_uiType);

    c_uint uiGlShader = glCreateShader_(a_uiType);
    EMP_RET_FALSE_IF(failure() || uiGlShader == 0, "failed to create shader '#0'", a_sSource);
    m_vuiShaders.add(uiGlShader);

    int siSourceLength = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_sSource.len(), siSourceLength));

    astring sSource(a_sSource);
    int asiLength[2] = { a_uiType == GL_VERTEX_SHADER ? m_siVertexMacro : m_siPixelMacro, siSourceLength };
    c_char* aszSources[2] = { a_uiType == GL_VERTEX_SHADER ? m_szVertexMacro : m_szPixelMacro, sSource.cstr() };

    EMP_XX_IF_DEBUG(
                    EMP_LOG_INFO("0 : '#0'", astring::ref(aszSources[0]));
                    EMP_LOG_INFO("1 : '#0'", astring::ref(aszSources[1]));
    )
    EMP_RET_FALSE_IF_NOT(glShaderSource__(uiGlShader, 2, aszSources, asiLength));
    EMP_RET_FALSE_IF_NOT(glCompileShader__(uiGlShader));
    EMP_RET_FALSE_IF(compile_failure(uiGlShader));

    a_ruiShader = uiGlShader;
    return a_ruiShader != 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::destroy(void)
{
    m_bValid = false;
    /*
    //@@0 c_size stCount = m_vBuffers.count();
    //for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    //{ DeleteBuffer(1, &m_vBuffers.ref(stIndex).m_ui32Name); }
    //m_IndexCycle.destroy();
    //m_VertexCycle.destroy();
    */

    m_v4fClearColor = v4f::all(0);
    m_bColorMask = true;
    m_bDepthMask = true;
    m_bDepthTest = false;
    m_bStencilTest = false;
    m_bBlend = false;
    m_eStencilFunc = GL_ALWAYS;
    m_siStencilFuncReference = 0;
    m_uiStencilFuncMask = emp::tt::max<GLuint>::value;
    m_eStencilOpSf = GL_KEEP;
    m_eStencilOpSp = GL_KEEP;
    m_uiStencilMask = emp::tt::max<GLuint>::value;
    m_si8Stencil = 0;
    m_eSrcBlend = GL_ZERO;
    m_eDstBlend = GL_ZERO;
    m_bDither = true;
    m_bPolygonSmooth = false;
    m_eFrontFace = GL_CCW;
    m_eCullFace = GL_BACK;
    m_eGlVersion = gl_version_e::none;
    m_eGlslVersion = glsl_version_e::none;
    m_sVertexMacro = astring();
    m_szVertexMacro = nullptr;
    m_siVertexMacro = 0;
    m_sPixelMacro = astring();
    m_szPixelMacro = nullptr;
    m_siPixelMacro = 0;

// @@0 remove    
//#ifdef EMP_XX_OS_WIN
//#define enum_id EMP_GFX_FUNCTION_E_NSNAME()::Value
//#define enum_values EMP_GFX_FUNCTION_E_VALUES()
//#define enum_value_ex_impl(x_Name, x_Ret, x_Params, x_Function, x_Args) EMP_PP_CAT(m_Fct_, x_Function)  = nullptr;
//#include "emp_enu_enum_ex.def"
//#endif

    {
        c_size stCount = m_vpPrograms.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            program_s* const pProgram = m_vpPrograms.val(stIndex);
            c_uint uiGlProgram = m_vuiPrograms.val(stIndex);

            c_uint uiGlVertex = m_vuiShaders.val(pProgram->vertex);
            c_uint uiGlPixel =  m_vuiShaders.val(pProgram->pixel);

            glDetachShader_(uiGlProgram, uiGlVertex);
            glDetachShader_(uiGlProgram, uiGlPixel);

            glDeleteProgram_(uiGlProgram);
            EMP_DEALLOCATE(pProgram);
        }
        m_vpPrograms.resize(0);
        m_mstPrograms.clear();
        m_vuiPrograms.resize(0);
    }

    {
        c_size stCount = m_vuiShaders.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_uint uiGlShader = m_vuiShaders.val(stIndex);
            glDeleteShader_(uiGlShader);
        }
        m_mstShaders.clear();
        m_vuiPrograms.resize(0);
    }

    emp::cnt::foreach(m_vpDeclarations, emp::mem::safe_delete_reset_t());
    m_vpDeclarations.clear();

    for (auto it = m_mpLineIndexBuffers.begin(); it.not_end(); ++it)
    { EMP_DEALLOCATE(*it.value()); }
    m_mpLineIndexBuffers.clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_clear_color(v4f::tparam a_v4fValue)
{
    //if (v4f::bool_and(v4f::equals(clear_color(), a_v4fValue)) == false)
    {
        m_v4fClearColor = a_v4fValue;
        glClearColor_(a_v4fValue.x, a_v4fValue.y, a_v4fValue.z, a_v4fValue.w);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::clear_target(void)
{
    EMP_RET_FALSE_IF(m_si8Stencil != 0, "invalid stencil value #0", m_si8Stencil);

    // @@0 remove EMP_RET_FALSE_IF_NOT(TryDisable(GL_SCISSOR_TEST))
    EMP_RET_FALSE_IF_NOT(glDepthMask__(GL_TRUE)); // @@0 remove

    set_stencil_mask(emp::tt::max<GLuint>::value);
    set_color_mask(true);

    return glClear__(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::clear_target(c_v4f& a_rv4Color)
{
    set_clear_color(a_rv4Color);
    return clear_target();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_color_mask(c_bool a_bValue)
{
    //if (a_bValue != color_mask())
    {
        GLboolean const bValue = a_bValue ? 1u : 0u;
        glColorMask_(bValue, bValue, bValue, bValue);
        m_bColorMask = a_bValue;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_depth_mask(c_bool a_bValue EMP_UNUSED)
//{ return; }
{

    //if (a_bValue != depth_mask())
    {
        glDepthMask_(a_bValue ? 1u : 0u);
        m_bDepthMask = a_bValue;
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_depth_test(c_bool a_bValue EMP_UNUSED)
//{ return;  }
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(glDepthFunc__(GL_LESS));
    }
    set_flag(m_bDepthTest, a_bValue, GL_DEPTH_TEST);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_stencil_test(c_bool a_bValue)
{ set_flag(m_bStencilTest, a_bValue, GL_STENCIL_TEST); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_stencil_func(GLenum const a_eFunc, GLint const a_siReference, GLuint const a_uiMask)
{
    /*if (a_eFunc != stencil_func() ||
            a_siReference != stencil_func_reference() ||
            a_uiMask != stencil_func_mask())*/
    {
        glStencilFunc_(a_eFunc, a_siReference, a_uiMask);

        m_eStencilFunc = a_eFunc;
        m_siStencilFuncReference = a_siReference;
        m_uiStencilFuncMask = a_uiMask;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_stencil_op(GLenum const a_eSf, GLenum const a_eSp)
{
    //if (a_eSf != stencil_op_sf() || a_eSp != stencil_op_sp())
    {
        glStencilOp_(a_eSf, a_eSp, a_eSp);

        m_eStencilOpSf = a_eSf;
        m_eStencilOpSp = a_eSp;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_stencil_mask(GLuint const a_ui8Mask)
{
    //if (a_ui8Mask != stencil_mask())
    {
        glStencilMask_(a_ui8Mask);

        m_uiStencilMask = a_ui8Mask;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::increment_stencil(void)
{
    EMP_ASSERT_BOOL_CALL(emp::mat::add(stencil(), 1, m_si8Stencil));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::decrement_stencil(void)
{
    EMP_ASSERT_BOOL_CALL(emp::mat::sub(stencil(), 1, m_si8Stencil));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_flag(bool& a_rbCurrent, c_bool a_bValue, GLenum const& a_eFlag) const
{
    //if (a_rbCurrent != a_bValue)
    {
        if (a_bValue)
        { glEnable_(a_eFlag); }
        else
        { glDisable_(a_eFlag); }
        a_rbCurrent = a_bValue;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_blend(c_bool a_bValue)
{ set_flag(m_bBlend, a_bValue, GL_BLEND); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_scissor(c_bool a_bValue)
{ set_flag(m_bScissor, a_bValue, GL_SCISSOR_TEST); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_blend_func(GLenum const a_eSrc, GLenum const a_eDst)
{
    //if (m_eSrcBlend != a_eSrc || m_eDstBlend != a_eDst)
    {
        glBlendFunc_(a_eSrc, a_eDst);
        m_eSrcBlend = a_eSrc;
        m_eDstBlend = a_eDst;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_dither(c_bool a_bValue)
{ set_flag(m_bDither, a_bValue, GL_DITHER); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_polygon_smooth(c_bool EMP_XX_IF_NOT_IOS(a_bValue))
{
    EMP_XX_IF_NOT_IOS(set_flag(m_bPolygonSmooth, a_bValue, GL_POLYGON_SMOOTH);)
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_front_face(GLenum const a_eValue)
{
    //if (m_eFrontFace != a_eValue)
    {
        glFrontFace_(a_eValue);
        m_eFrontFace = a_eValue;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_cull_face(GLenum const a_eValue)
{
    //if (m_eCullFace != a_eValue)
    {
        glCullFace_(a_eValue);
        m_eCullFace = a_eValue;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::viewport(c_f64 a_dWidth, c_f64 a_dHeight)
{ return viewport(0, 0, a_dWidth, a_dHeight); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::viewport(c_v4f a_v4Value)
{ return viewport(a_v4Value.x, a_v4Value.y, a_v4Value.z, a_v4Value.w); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::viewport(c_f64 a_dX, c_f64 a_dY, c_f64 a_dW, c_f64 a_dH)
{
    m_v4Viewport = {emp::ast::cast<float>(a_dX), emp::ast::cast<float>(a_dY), emp::ast::cast<float>(a_dW), emp::ast::cast<float>(a_dH) };

    return glViewport__(
        emp::tt::maxof(0, emp::ast::cast<si32>(a_dX)), emp::tt::maxof(0, emp::ast::cast<si32>(a_dY)),
        emp::tt::maxof(0, emp::ast::cast<si32>(a_dW)), emp::tt::maxof(0, emp::ast::cast<si32>(a_dH)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::scissor(c_f64 a_dX, c_f64 a_dY, c_f64 a_dW, c_f64 a_dH, bool& a_rbEmpty)
{
    a_rbEmpty = false; // @@0 unused...
    m_v4Scissor = {emp::ast::cast<float>(a_dX), emp::ast::cast<float>(a_dY), emp::ast::cast<float>(a_dW), emp::ast::cast<float>(a_dH) };

    si32 siX = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dX, siX));
    siX = emp::tt::maxof(0, siX);

    si32 siY = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dY, siY));
    siY = emp::tt::maxof(0, siY);

    si32 siW = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dW, siW));
    siW = emp::tt::maxof(0, siW);

    si32 siH = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dH, siH));
    siH = emp::tt::maxof(0, siH);

    c_f64 dResX = a_dX - siX;
    c_f64 dResY = a_dY - siY;

    c_f64 dResW = a_dW - siW;
    c_f64 dResH = a_dH - siH;

    c_f64 dOffsetX = 2; // @@0 scissor offset
    c_f64 dOffsetY = 2; // @@0 scissor offset

    si32 siResW = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dResX + dResW + dOffsetX, siResW));

    si32 siResH = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(dResY + dResH + dOffsetY, siResH));

    c_si32 siFinalW = emp::tt::maxof(0, siW + siResW);
    c_si32 siFinalH = emp::tt::maxof(0, siH + siResH);
    a_rbEmpty = siFinalW <= 0 || siFinalH <= 0;

    v4<si32> v4si32Scissor = { siX, siY, siFinalW, siFinalH };
    if (v4si32Scissor != m_v4si32Scissor)
    {
        m_v4si32Scissor = v4si32Scissor;

        return glScissor__(siX, siY, siFinalW, siFinalH);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::lock_texture(c_astring& a_sPath, ui32& a_rui32Texture, c_ui32 a_ui32Color, c_bool a_bDelete)
{
    EMP_LOG_DEBUG("LOCK TEXURE #0", a_sPath);
    m_vui8ScratchPad.resize(0);
    m_vpui8ScratchPad.resize(0);
    ui32 ui32Width = 0;
    ui32 ui32Height = 0;
    ui32 ui32Depth = 0;
    if (emp::png::load(a_sPath, m_vui8ScratchPad, m_vpui8ScratchPad, ui32Width, ui32Height, ui32Depth) == false)
    {
        if (a_bDelete)
        {
            bool bExists = false;
            EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(a_sPath, bExists));
            if (bExists)
            {
                EMP_RET_FALSE_IF_NOT(emp::fs::delete_file(a_sPath));
            }
        }
        return false;
    }

    if ((m_vui8ScratchPad.count() > 0) == false)
    {
        ui32Width = 8;
        ui32Height = 8;
        ui32Depth = 4;
        c_size stCount = 8 * 8;
        m_vui8ScratchPad.resize(stCount * 4);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stOffset = stIndex * 4;
            m_vui8ScratchPad.set(stOffset + 0, static_cast<ui8>((a_ui32Color >> 24) & 0xFF));
            m_vui8ScratchPad.set(stOffset + 1, static_cast<ui8>((a_ui32Color >> 16) & 0xFF));
            m_vui8ScratchPad.set(stOffset + 2, static_cast<ui8>((a_ui32Color >> 8) & 0xFF));
            m_vui8ScratchPad.set(stOffset + 3, static_cast<ui8>((a_ui32Color >> 0) & 0xFF));
        }
    }

    GLint siInternalFormat = 0;
    GLenum eFormat = 0;

    if (ui32Depth == 3)
    {
        siInternalFormat = GL_RGB;
        eFormat = GL_RGB;
    }
    else if (ui32Depth == 4)
    {
        siInternalFormat = GL_RGBA;
        eFormat = GL_RGBA;
    }
    else
    { return false; }

    EMP_RET_FALSE_IF_NOT(glGenTextures__(1, &a_rui32Texture));
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
    EMP_RET_FALSE_IF_NOT(glBindTexture__(GL_TEXTURE_2D, a_rui32Texture));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    si32 siW = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast<ui32>(ui32Width, siW));

    si32 siH = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast<ui32>(ui32Height, siH));

    EMP_RET_FALSE_IF_NOT(glTexImage2D__(GL_TEXTURE_2D, 0, siInternalFormat, siW, siH, 0, eFormat, GL_UNSIGNED_BYTE, m_vui8ScratchPad.ptr(0)));

#if defined(EMP_XX_PROFILE)
    m_mTextures.set(a_rui32Texture, { a_rui32Texture, ui32Depth, siW, siH });
#endif

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::unlock_texture(c_ui32 a_ui32Texture EMP_UNUSED)
{
    EMP_LOG_DEBUG("UNLOCK TEXTURE #0", a_ui32Texture);
    EMP_RET_FALSE_IF_NOT(a_ui32Texture > 0);
    EMP_RET_FALSE_IF_NOT(glDeleteTextures__(1, &a_ui32Texture));

#if defined(EMP_XX_PROFILE)
    auto it = m_mTextures.get(a_ui32Texture);
    EMP_RET_FALSE_IF(it.end());
    m_mTextures.remove(it);
#endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::lock_framebuffer(c_ui32 a_ui32Width, c_ui32 a_ui32Height, c_bool a_bResizable, ui32& a_rui32Index)
{
    EMP_RET_FALSE_IF_NOT(emp::tt::is_max(a_rui32Index));

    ui32 EMP_TT_MAX_VAR(ui32Count);
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(m_vTargets.count(), ui32Count));

    EMP_RET_FALSE_IF_NOT(a_ui32Width <= 8192 && a_ui32Width > 0);
    EMP_RET_FALSE_IF_NOT(a_ui32Height <= 8192 && a_ui32Height > 0);

    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    {
        target_s& rTarget = m_vTargets.ref(ui32Index);
        if (rTarget.m_bLocked == false)
        {
            if (emp::tt::is_max(rTarget.m_ui32Index))
            {
                rTarget.m_ui32Index = ui32Index;
                rTarget.m_ui32Width = a_ui32Width;
                rTarget.m_ui32Height = a_ui32Height;
                rTarget.m_bResizable = a_bResizable;
                EMP_RET_FALSE_IF_NOT(allocate_framebuffer(rTarget));
                EMP_LOG_INFO("NEW TARGET #0 x #1, COUNT #2", a_ui32Width, a_ui32Height, ui32Count);
                rTarget.m_bLocked = true;
                a_rui32Index = ui32Index;
                return true;
            }
            else if (rTarget.m_ui32Width == a_ui32Width &&
                    rTarget.m_ui32Height == a_ui32Height)
            {
                rTarget.m_bResizable = a_bResizable;
                rTarget.m_bLocked = true;
                a_rui32Index = ui32Index;
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::unlock_framebuffer(c_ui32 a_ui32Index)
{
    EMP_RET_FALSE_IF(emp::tt::is_max(a_ui32Index));
    EMP_RET_FALSE_IF_NOT(m_vTargets.is_valid_index(a_ui32Index));
    target_s& rTarget = m_vTargets.ref(a_ui32Index);
    EMP_RET_FALSE_IF_NOT(rTarget.m_bLocked);
    rTarget.m_bLocked = false;
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_GLES
EMP_RETURN bool gl_device_c::discard_framebuffers(void)
{
    c_size stCount = m_vTargets.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        target_s const& rTarget = m_vTargets.ref(stIndex);
        EMP_RET_FALSE_IF_NOT(bind_framebuffer(rTarget));
        GLenum const invalid_ap[3] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT};
        EMP_RET_FALSE_IF_NOT(glInvalidateFramebuffer__(GL_FRAMEBUFFER, 3, invalid_ap));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::discard_framebuffer()
{
    GLenum const invalid_ap[3] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT};
    EMP_RET_FALSE_IF_NOT(glInvalidateFramebuffer__(GL_FRAMEBUFFER, 3, invalid_ap));
    return true;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::allocate_framebuffer(target_s& a_rTarget)
{
    int siW = 0;
    int siH = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_rTarget.m_ui32Width, siW));
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_rTarget.m_ui32Height, siH));

    EMP_RET_FALSE_IF(siW > 8192 || siW <= 0);
    EMP_RET_FALSE_IF(siH > 8192 || siH <= 0);

    EMP_RET_FALSE_IF_NOT(glGenTextures__(1, &a_rTarget.m_ui32Texture));
    EMP_RET_FALSE_IF_NOT(glGenFramebuffers__(1, &a_rTarget.m_ui32Framebuffer));
    EMP_RET_FALSE_IF_NOT(glGenRenderbuffers__(1, &a_rTarget.m_ui32Renderbuffer));
    EMP_RET_FALSE_IF_NOT(glBindTexture__(GL_TEXTURE_2D, a_rTarget.m_ui32Texture));

    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    EMP_RET_FALSE_IF_NOT(glTexParameteri__(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    EMP_RET_FALSE_IF_NOT(glTexImage2D__(GL_TEXTURE_2D, 0, GL_RGBA, siW, siH, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

    EMP_RET_FALSE_IF_NOT(bind_framebuffer(a_rTarget));

    EMP_RET_FALSE_IF_NOT(glFramebufferTexture2D__(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, a_rTarget.m_ui32Texture, 0));

    EMP_RET_FALSE_IF_NOT(glBindRenderbuffer__(GL_RENDERBUFFER, a_rTarget.m_ui32Renderbuffer));
    EMP_RET_FALSE_IF_NOT(glRenderbufferStorage__(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, siW, siH));

    EMP_RET_FALSE_IF_NOT(glFramebufferRenderbuffer__(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, a_rTarget.m_ui32Renderbuffer));

    EMP_RET_FALSE_IF_NOT(framebuffer_status());
    EMP_RET_FALSE_IF_NOT(clear_target(v4f::all(0)));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::resize(c_f64 a_dWidth, c_f64 a_dHeight)
{
    ui32 uiW = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dWidth, uiW));

    ui32 uiH = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dHeight, uiH));

    EMP_RET_FALSE_IF_NOT(uiW <= 8192 && uiW > 0);
    EMP_RET_FALSE_IF_NOT(uiH <= 8192 && uiH > 0);

    emp::mat::m44f v44Identity;
    emp::mat::m44f::identity(v44Identity);

    m_vui32Targets.resize(0);
    for (size_t stIndex = 0; stIndex < m_vTargets.count(); ++stIndex)
    {
        target_s& rTarget = m_vTargets.ref(stIndex);
        if (rTarget.m_bLocked && rTarget.m_bResizable && (rTarget.m_ui32Width != uiW || rTarget.m_ui32Height != uiH))
        { m_vui32Targets.add(rTarget.m_ui32Index); }
    }

    for (size_t stIndex = 0; stIndex < m_vui32Targets.count(); ++stIndex)
    {
        c_ui32 ui32Old = m_vui32Targets.val(stIndex);
        target_s& rTarget = m_vTargets.ref(ui32Old);
        EMP_ASSERT(rTarget.m_bLocked);

        EMP_RET_FALSE_IF(emp::tt::is_max(rTarget.m_ui32Texture));
        EMP_RET_FALSE_IF(emp::tt::is_max(rTarget.m_ui32Framebuffer));
        EMP_RET_FALSE_IF(emp::tt::is_max(rTarget.m_ui32Renderbuffer));

        ui32 EMP_TT_MAX_VAR(ui32Target);
        EMP_RET_FALSE_IF_NOT(lock_framebuffer(uiW, uiH, true, ui32Target));

        set_blend(true);
        glBlendFunc_(GL_ONE, GL_ZERO);
        set_vertex_buffer(&*m_pGlQuadVertices);
        set_index_buffer(&*m_pQuadIndexBuffer);
        set_program(texture_program());
        set_uniform(uniform_e::m44Transform, v44Identity);
        set_uniform(uniform_e::t2Texture_0, 0, rTarget.m_ui32Texture);
        set_uniform(uniform_e::v4Color, v4f({1, 1, 1, 1}));
        draw(emp::gfx::primitive_e::Triangles);

        EMP_RET_FALSE_IF_NOT(glDeleteTextures__(1, &rTarget.m_ui32Texture));
        EMP_RET_FALSE_IF_NOT(glDeleteFramebuffers__(1, &rTarget.m_ui32Framebuffer));
        EMP_RET_FALSE_IF_NOT(glDeleteRenderbuffers__(1, &rTarget.m_ui32Renderbuffer));

        EMP_RET_FALSE_IF_NOT(framebuffer(ui32Target, rTarget));
        EMP_RET_FALSE_IF_NOT(unlock_framebuffer(ui32Old));
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool gl_device_c::resize_framebuffer(target_s& a_rTarget)
{
    auto const siW = a_rTarget.m_siWidth;
    auto const siH = a_rTarget.m_siHeight;

    EMP_RET_FALSE_IF(siW > 4096 || siW <= 0);
    EMP_RET_FALSE_IF(siH > 4096 || siH <= 0);
    EMP_RET_FALSE_IF_NOT(a_rTarget.m_bLocked);

    c_bool bDestroy = a_rTarget.m_ui32Texture != 0 && a_rTarget.m_ui32Framebuffer != 0 && a_rTarget.m_ui32Renderbuffer != 0;
    EMP_RET_FALSE_IF_NOT(bDestroy || (a_rTarget.m_ui32Texture == 0 && a_rTarget.m_ui32Framebuffer == 0 && a_rTarget.m_ui32Renderbuffer == 0));
        
        if (bDestroy)
        {

        }

        a_rTarget = oNewTarget;
        return true;
    };

    for (size_t stIndex = 0; stIndex < m_vpRenderlets.count(); ++stIndex)
    {
        renderlet_c* const pRenderlet = m_vpRenderlets.val(stIndex);
        c_bool bSimulated = pRenderlet != &m_Rendered;
        auto const szW = bSimulated ? 64 : szWidth;
        auto const szH = bSimulated ? 64 : szHeight;
        if ((bSimulated == false) || pRenderlet->m_bAllocated == false)
        {
            for (size_t stTarget = 0; stTarget < pRenderlet->m_aTargets.count(); ++stTarget)
            { EMP_RET_FALSE_IF_NOT(l_resize_target(szW, szH, pRenderlet->m_aTargets.ref(stTarget))); }
        }
        pRenderlet->m_bAllocated = true;
    }

    for (size_t stIndex = 0; stIndex < m_aTTargets.count(); ++stIndex)
    { EMP_RET_FALSE_IF_NOT(l_resize_target(szWidth, szHeight, m_aTTargets.ref(stIndex))); }

    return true;

}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::bind_framebuffer(c_ui32 a_ui32Framebuffer, c_f64 a_dW, c_f64 a_dH)
{
    m_Framebuffer.m_ui32Index = a_ui32Framebuffer;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dW, m_Framebuffer.m_siW));
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_dH, m_Framebuffer.m_siH));
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.glBindFramebuffer__(GL_FRAMEBUFFER, a_ui32Framebuffer));
    EMP_RET_FALSE_IF_NOT(viewport(a_dW, a_dH));
    EMP_GRAPHICS.set_scissor(true);
    bool bEmpty = false;
    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.scissor(0, 0, a_dW, a_dH, bEmpty));
    EMP_RET_FALSE_IF(bEmpty);
    return true;	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::bind_framebuffer(target_s const& a_rTarget)
{
    EMP_RET_FALSE_IF_NOT(bind_framebuffer(a_rTarget.m_ui32Framebuffer, a_rTarget.m_ui32Width, a_rTarget.m_ui32Height));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::framebuffer_status(void) const
{
    GLenum const eReturn = glCheckFramebufferStatus_(GL_FRAMEBUFFER);
    if (failure())
    { return false; }

    switch (eReturn)
    {
        case GL_FRAMEBUFFER_COMPLETE:
        { return true; }

#define XTMP_GL_ERROR_CASE(x_Error, x_Name)\
            case x_Error:\
            {\
                EMP_LOG_ERROR("Gl Error : #0, #1, #2", x_Error, ALITERAL(EMP_PP_STRING(x_Error)), ALITERAL(x_Name));\
                 return false;\
            }

            XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, "GL_FRAMEBUFFER_INCOMPLETE_ATTACMENT")
#if defined(EMP_XX_OS_OSX)
            // @@0 remove XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS")
#elif defined(EMP_XX_OS_LIN)
            // @@0 remove XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS")
#elif defined(EMP_XX_OS_WIN)
            // @@0 remove XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS")
#elif defined(EMP_XX_OS_IOS)
            XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS")
#endif

            XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT")
            XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_UNSUPPORTED, "GL_FRAMEBUFFER_UNSUPPORTED")
            XTMP_GL_ERROR_CASE(GL_FRAMEBUFFER_UNDEFINED, "GL_FRAMEBUFFER_UNDEFINED")
#undef XTMP_GL_ERROR_CASE
        default:
                {
                    EMP_LOG_ERROR("Gl Error : #0", eReturn);
                    EMP_ASSERT_UNREACHABLE();
                    return false;
                }
    }

// @@0 unreachable    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN int get_location(program_s const& a_rProgram, c_uniform_e a_eIndex);
EMP_RETURN int get_location(program_s const& a_rProgram, c_uniform_e a_eIndex)
{
    gl_uniform_s const& rUniform = a_rProgram.uniforms.val(enum_to(a_eIndex));
    c_int siLocation = rUniform.location;
    EMP_RET_X_IF(-1, siLocation < 0, "Invalid uniform #0 location #1", rUniform.name, siLocation); // @@0 ref not working
    return siLocation;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_uniform(c_uniform_e a_eIndex, m44f::tparam a_m44Value, c_bool a_bTranspose) const
{
    c_int siLocation = get_location(*m_pCurrentProgram, a_eIndex);
    EMP_RET_IF(siLocation < 0);

    glUniformMatrix4fv_(siLocation, 1, a_bTranspose ? 1u : 0u, &a_m44Value.f00);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_uniform(c_uniform_e a_eIndex, v4f::tparam a_v4Value) const
{
    c_int siLocation = get_location(*m_pCurrentProgram, a_eIndex);
    EMP_RET_IF(siLocation < 0);

    glUniform4fv_(siLocation, 1, &a_v4Value.x);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_uniform(c_uniform_e a_eIndex, c_float a_fValue) const
{
    c_int siLocation = get_location(*m_pCurrentProgram, a_eIndex);
    EMP_RET_IF(siLocation < 0);

    glUniform1fv_(siLocation, 1, &a_fValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_uniform(c_uniform_e a_eIndex, c_int a_siSlot, c_uint a_uiTexture) const
{
    c_int siLocation = get_location(*m_pCurrentProgram, a_eIndex);
    EMP_RET_IF(siLocation < 0);

    EMP_RET_IF_NOT(a_uiTexture > 0, "Invalid texture name #0", a_uiTexture);

    if (a_siSlot == 0)
    {
        glActiveTexture_(GL_TEXTURE0);
    }
    else if (a_siSlot == 1)
    {
        glActiveTexture_(GL_TEXTURE1);
    }
    else
    {
        EMP_ASSERT_UNREACHABLE();
        return;
    }

    glBindTexture_(GL_TEXTURE_2D, a_uiTexture);
    glUniform1i_(siLocation, a_siSlot);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::set_program(c_size a_stProgram)
{
    m_pCurrentProgram = m_vpPrograms.val(a_stProgram);
    glUseProgram_(m_vuiPrograms.val(a_stProgram));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN buffer_t<ui8>* gl_device_c::line_index_buffer(c_size a_stCount)
{
    if ((a_stCount > 0) == false)
    { return nullptr; }

    auto const it = m_mpLineIndexBuffers.get(a_stCount);
    if (it.not_end())
    { return *it.value(); }

    auto* const pLineIndexBuffer = EMP_ALLOCATE(buffer_t<ui8>);
    m_mpLineIndexBuffers.set(a_stCount, pLineIndexBuffer);
    c_size stCount = a_stCount - 1;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { emp::gfx::emit_line(stIndex, *pLineIndexBuffer); }
    return pLineIndexBuffer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN buffer_s* gl_device_c::lock_buffer(cycle_d& a_rCycle, c_usage_e a_eUsage, c_ui32 a_ui32Target, c_size a_stSize)
{
    ui32 ui32Usage = GL_DONT_CARE;
    size_t stOffset = 0;
    switch (a_eUsage)
    {
        case usage_e::Static:
        {
            ui32Usage = GL_STATIC_DRAW;
            stOffset = 2;
            break;
        }
        case usage_e::Dynamic:
        {
            ui32Usage = GL_STREAM_DRAW;
            break;
        }
        case usage_e::none:
        default:
        {
            EMP_ASSERT_UNREACHABLE();
            return nullptr;
        }
    };

    auto& rPool = a_rCycle.ref(0 + stOffset);

    size_t stSize = 1;
    size_t stSlot = 0;
    while (stSize < a_stSize) // @@0 legit ?
    {
        stSize <<= 1;
        ++stSlot;
    }

    vector<size_t>& rvBuffers = rPool.ref(stSlot);
    if (rvBuffers.count() > 0)
    {
        c_size stBuffer = rvBuffers.last_val();
        rvBuffers.remove_last();
        buffer_s* pBuffer = m_vBuffers.ptr(stBuffer);
        EMP_ASSERT(pBuffer->m_ui32Usage == ui32Usage);
        if (pBuffer->m_ui32Usage == GL_STATIC_DRAW)
        { pBuffer->m_stAllocated = 1; }
        return pBuffer;
    }

    c_size stBufferCount = m_vBuffers.count(); // @@info stored before push.
    buffer_s& rBuffer = m_vBuffers.push();
    rBuffer.m_ui32Name = new_name();
    rBuffer.m_ui32Target = a_ui32Target;
    rBuffer.m_ui32Usage = ui32Usage;
    rBuffer.m_siptrSize = emp::ast::cast<GLsizeiptr>(stSize);
    rBuffer.m_stSlot = stSlot;
    rBuffer.m_stIndex = stBufferCount;
    // @@0 frame rBuffer.m_stFrame = m_stFrame;
    rBuffer.m_stAllocated = 0;
    rBuffer.m_pCycle = &a_rCycle;
    return &rBuffer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::unlock_buffer(c_size a_stBuffer)
{
    buffer_s* pBuffer = emp::tt::is_max(a_stBuffer) ? nullptr : m_vBuffers.ptr(a_stBuffer);
    if (pBuffer != nullptr)
    { unlock_buffer(pBuffer); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::unlock_buffer(buffer_s* const a_pBuffer)
{
    if (a_pBuffer->m_stAllocated > 0)
    {
        glBindBuffer_(a_pBuffer->m_ui32Target, a_pBuffer->m_ui32Name);
        glBufferData_(a_pBuffer->m_ui32Target, a_pBuffer->m_siptrSize, nullptr, a_pBuffer->m_ui32Usage);
    }

    size_t stOffset = 0;
    switch (a_pBuffer->m_ui32Usage)
    {
        case GL_STATIC_DRAW:
        {
            stOffset = 2;
            break;
        }
        case GL_STREAM_DRAW:
        { break; }
        default:
        {
            EMP_ASSERT_UNREACHABLE();
            return;
        }
    };

    auto& rPool = a_pBuffer->m_pCycle->ref(1 + stOffset);
    rPool.ref(a_pBuffer->m_stSlot).add(a_pBuffer->m_stIndex);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::end_of_frame(void)
{
    auto const& lEndOfFrame = [](cycle_d& a_rCycle, c_size a_stOffset)
    {
        auto& rTo = a_rCycle.ref(0 + a_stOffset);
        auto& rFrom = a_rCycle.ref(1 + a_stOffset);
        c_size stSlotCount = rFrom.count();
        for (size_t stSlot = 0; stSlot < stSlotCount; ++stSlot)
        {
            auto& rToBuffers = rTo.ref(stSlot);
            auto& rFromBuffers = rFrom.ref(stSlot);
            c_size stBufferCount = rFromBuffers.count();
            for (size_t stBuffer = 0; stBuffer < stBufferCount; ++stBuffer)
            { rToBuffers.add(rFromBuffers.ref(stBuffer)); }
            rFromBuffers.clear();
        }
    };

    lEndOfFrame(m_IndexCycle, 0);
    lEndOfFrame(m_VertexCycle, 0);
    lEndOfFrame(m_IndexCycle, 2);
    lEndOfFrame(m_VertexCycle, 2);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::update_buffer(c_ui32 a_ui32Target, buffer_i* const a_pBuffer, cycle_d& a_rCycle, c_bool EMP_SILENT(a_bStatic))
{
    c_size stBuffer = a_pBuffer->name();
    buffer_s* pBuffer = emp::tt::is_max(stBuffer) ? nullptr : m_vBuffers.ptr(stBuffer);

    if (a_pBuffer->changed() || pBuffer == nullptr)
    {
        if (pBuffer != nullptr)
        { unlock_buffer(pBuffer); }

        c_size stSize = a_pBuffer->size();
        if ((stSize > 0) == false)
        { return false; }

        pBuffer = lock_buffer(a_rCycle, a_pBuffer->usage(), a_ui32Target, stSize);
        a_pBuffer->set_name(pBuffer->m_stIndex);

        glBindBuffer_(a_ui32Target, pBuffer->m_ui32Name);

        char const* const pData = a_pBuffer->data();
        c_size stAllocated = pBuffer->m_stAllocated;
        if (stAllocated > 0)
        {
            EMP_XX_IF_ASSERT(c_ui32 ui32Usage = pBuffer->m_ui32Usage;)

            EMP_ASSERT((ui32Usage == GL_STREAM_DRAW) || (ui32Usage == GL_STATIC_DRAW && stAllocated == 1));
            EMP_XX_IF_ASSERT(
                GLint siSize = 0;
                glGetBufferParameteriv_(a_ui32Target, GL_BUFFER_SIZE, &siSize);
                EMP_ASSERT(siSize == pBuffer->m_siptrSize);)
            glBufferSubData_(a_ui32Target, 0, pBuffer->m_siptrSize, pData);
        }
        else
        {
            glBufferData_(a_ui32Target, pBuffer->m_siptrSize, pData, pBuffer->m_ui32Usage);
            EMP_XX_IF_ASSERT(
                GLint siSize = 0;
                glGetBufferParameteriv_(a_ui32Target, GL_BUFFER_SIZE, &siSize);
                EMP_ASSERT(siSize == pBuffer->m_siptrSize);)
        }

        ++pBuffer->m_stAllocated;
        a_pBuffer->set_processed();
    }
    else
    { glBindBuffer_(a_ui32Target, pBuffer->m_ui32Name); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void gl_device_c::draw(c_primitive_e a_ePrimitive)
{
    if (update_buffer(GL_ELEMENT_ARRAY_BUFFER, m_pCurrentIndexBuffer, m_IndexCycle, false) == false ||
        update_buffer(GL_ARRAY_BUFFER, m_pCurrentVertexBuffer, m_VertexCycle, false) == false)
    {
        return;
    }

    uintptr_t uiOffset = 0;
    declaration_c const* pDeclaration = m_pCurrentVertexBuffer->declaration();
    GLsizei iStride = emp::ast::cast<GLsizei>(pDeclaration->m_stStride);
    EMP_ASSERT(iStride > 0);
    glBindVertexArray_(m_VAO);
    emp::tt::silent(success());

    auto const& vAttributes = pDeclaration->m_vAttributes;
    c_ui32 ui32Count = emp::ast::cast<ui32>(vAttributes.count());
    for (ui32 ui32Index = 0; ui32Index < ui32Count; ++ui32Index)
    {
        glEnableVertexAttribArray_(ui32Index);
        attribute_s const& rAttribute = vAttributes.ref(ui32Index);

        GLenum eType = GL_NONE;
        switch (rAttribute.m_eType)
        {
            case type_e::F32:
            {
                eType = GL_FLOAT;
                break;
            }
            case type_e::UI8:
            case type_e::UI16:
            case type_e::UI32:
            case type_e::VTX:
            case type_e::none:
            default:
            {
                EMP_ASSERT(false);
                return;
            }
        };

        glVertexAttribPointer_(ui32Index, emp::ast::cast<GLint>(rAttribute.m_stCount), eType, GL_TRUE, iStride, reinterpret_cast<void*>(uiOffset));

        uiOffset += rAttribute.m_stCount * rAttribute.m_stSize;
    }

    GLsizei iCount = emp::ast::cast<GLsizei>(m_pCurrentIndexBuffer->count());

    GLenum eMode = GL_NONE; // @@1 array
    switch (a_ePrimitive)
    {
        case primitive_e::Lines:
        {
            EMP_ASSERT((iCount % 2) == 0);
            eMode = GL_LINES;
            break;
        }
        case primitive_e::Triangles:
        {
            EMP_ASSERT((iCount % 3) == 0);
            eMode = GL_TRIANGLES;
            break;
        }
        case primitive_e::none:
        default:
        {
            EMP_ASSERT(false);
            return;
        }
    };

    GLenum eIndex = GL_NONE; // @@1 array
    switch (m_pCurrentIndexBuffer->type())
    {
        case type_e::UI8:
        {
            eIndex = GL_UNSIGNED_BYTE;
            break;
        }
        case type_e::UI16:
        {
            eIndex = GL_UNSIGNED_SHORT;
            break;
        }
        case type_e::UI32:
        {
            eIndex = GL_UNSIGNED_INT;
            break;
        }
        case type_e::F32:
        case type_e::VTX:
        case type_e::none:
        default:
        {
            EMP_ASSERT(false);
            return;
        }
    };

    glDrawElements_(eMode, iCount, eIndex, 0);
    //@@1 perf DrawRangeElements(eMode, a_uiMin, a_uiMax, iCount, eType, 0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::read_pixels(emp::gfx::target_s const& a_rTarget, vector<ui32>& a_rvui32Pixels, c_bool a_bAlpha)
{
    int siWidth = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_rTarget.m_ui32Width, siWidth));

    int siHeight = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(a_rTarget.m_ui32Height, siHeight));

    EMP_RET_FALSE_IF_NOT(siWidth > 0 && siHeight > 0);

    c_size stPixelCount = emp::ast::cast<size_t>(siWidth * siHeight);

    a_rvui32Pixels.resize(stPixelCount);

//#if defined EMP_GLES
    EMP_RET_FALSE_IF_NOT(bind_framebuffer(a_rTarget));
    glReadPixels(0, 0, siWidth, siHeight, a_bAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, a_rvui32Pixels.first_ptr());

//#elif defined EMP_GL
//     glBindTexture_(GL_TEXTURE_2D, a_rTarget.m_ui32Texture);
//     glGetTexImage_(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, a_rvui32Pixels.first_ptr());
// #endif
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::success(void) const
{
    bool bNoError = true;
    while (true)
    {
        GLenum const eError = glGetError();
        if (eError == GL_NO_ERROR)
        { break; }
        switch (eError)
        {
            case GL_NO_ERROR:
            {
                break;
            }
            //@@3 duplicate string/enum
#define XTMP_GL_ERROR_CASE(x_Error, x_szError)\
case x_Error:\
{\
    EMP_LOG_ERROR("Gl Error : #0, #1, #2", x_Error, ALITERAL(EMP_PP_STRING(x_Error)), ALITERAL(x_szError));\
    bNoError = false;\
    continue;\
}

                XTMP_GL_ERROR_CASE(GL_INVALID_ENUM, "GL_INVALID_ENUM")
                XTMP_GL_ERROR_CASE(GL_INVALID_VALUE, "GL_INVALID_VALUE")
                XTMP_GL_ERROR_CASE(GL_INVALID_OPERATION, "GL_INVALID_OPERATION")
                XTMP_GL_ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAME_BUFFER_OPERATION")
                XTMP_GL_ERROR_CASE(GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY")
                EMP_XX_IF_NOT_IOS(XTMP_GL_ERROR_CASE(GL_STACK_OVERFLOW, "GL_STACK_OVERFLOW"))
                EMP_XX_IF_NOT_IOS(XTMP_GL_ERROR_CASE(GL_STACK_UNDERFLOW, "GL_STACK_UNDERFLOW"))

#undef XTMP_GL_ERROR_CASE

            default:
            {
                EMP_LOG_ERROR("Gl Error : #0", eError);
                EMP_ASSERT_UNREACHABLE();
                return false;
            }
        }
    }

    if (bNoError)
    {
        return bNoError;
    }

    EMP_ASSERT(false);
    return bNoError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::failure(void) const
{ return success() == false; }
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool t_bShader>
void gl_device_c::get_iv(GLuint shader, GLenum pname, GLint* params) const
{ glGetShaderiv_(shader, pname, params); }

template <bool t_bShader>
void gl_device_c::get_info_log(GLuint shader, GLsizei max, GLsizei* length, GLchar* log) const
{ glGetShaderInfoLog_(shader, max, length, log); }

template <>
void gl_device_c::get_iv<false>(GLuint shader, GLenum pname, GLint* params) const
{ glGetProgramiv_(shader, pname, params); }

template <>
void gl_device_c::get_info_log<false>(GLuint shader, GLsizei max, GLsizei* length, GLchar* log) const
{ glGetProgramInfoLog_(shader, max, length, log); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <bool t_bShader>
EMP_RETURN bool gl_device_c::glsl_success(c_uint a_uiShader, c_uint a_uiType) const
{
    EMP_ASSERT(a_uiShader > 0);

    EMP_ASSERT( a_uiType == GL_COMPILE_STATUS ||
               a_uiType == GL_LINK_STATUS);

    int siStatusOk = 0;
    get_iv<t_bShader>(a_uiShader, a_uiType, &siStatusOk);
    EMP_RET_FALSE_IF(failure(), "a_rGetiv STATUS failed");

    if (siStatusOk == false)
    {
        int siSize = 0;
        get_iv<t_bShader>(a_uiShader, GL_INFO_LOG_LENGTH, &siSize);
        EMP_RET_FALSE_IF(failure(),  "a_rGetiv GL_INFO_LOG_LENGTH failed");

        size_t stSize = 0;
        EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(siSize, stSize));
        EMP_RET_FALSE_IF_NOT(stSize > 0);

        astring sLog;
        sLog.resize(stSize);
        get_info_log<t_bShader>(a_uiShader, siSize, NULL, sLog.get_first_char_ptr());
        EMP_RET_FALSE_IF_NOT(success(), "a_rGetInfoLog failed"); // @@0 ?? duplicate
        sLog.set_char(stSize - 1, 0);
        sLog.resize(stSize);

        EMP_LOG_ERROR("error(s) while compiling shader\n#0", sLog);
        EMP_XX_HOOK()
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::compile_success(c_uint a_uiShader) const
{ return glsl_success<true>(a_uiShader, GL_COMPILE_STATUS); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::compile_failure(c_uint a_uiShader) const
{ return compile_success(a_uiShader) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::link_success(c_uint a_uiShader) const
{ return glsl_success<false>( a_uiShader, GL_LINK_STATUS); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool gl_device_c::link_failure(c_uint a_uiShader)const
{ return link_success(a_uiShader) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(EMP_XX_PROFILE)
void gl_device_c::log_info(void) const
{
    {
        //buffers
        size_t stSize = 0;
        c_size stCount = m_vBuffers.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { stSize += emp::ast::cast<size_t>(m_vBuffers.ref(stIndex).m_siptrSize); }

        EMP_LOG_INFO("gfx : buffers, count : #0, size : #1", stCount, emp::mem::size_s(stSize));
    }

    {
        //targets
        size_t stSize = 0;
        c_size stCount = m_vTargets.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            target_s const& rTarget = m_vTargets.ref(stIndex);
            c_size stRes = emp::ast::cast<size_t>(rTarget.m_siWidth * rTarget.m_siHeight) * 4;
            stSize += stRes * 4;
            stSize += stRes * 3;
        }

        EMP_LOG_INFO("gfx : targets : count : #0, size : #1", stCount, emp::mem::size_s(stSize));
    }


    {
        //textures
        size_t stSize = 0;
        for (auto it = m_mTextures.begin(); it.not_end(); ++it)
        {
            texture_s const& rTexture = *it.value();
            stSize += emp::ast::cast<size_t>(4 * rTexture.depth * emp::ast::cast<size_t>(rTexture.width * rTexture.height));
        }

        EMP_LOG_INFO("gfx : textures : count : #0, size : #1", m_mTextures.count(), emp::mem::size_s(stSize));
    }
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

