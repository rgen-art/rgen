//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#if 0
#include "emp_gui_renderer.h"

#include "emp_gui_container.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"
#include <unistd.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"

#include "emp_fs_file.h"

#include "emp_gfx_gl_device.h"

// @@0 CLASH None defined in lib_gfx
#include "emp_ose_compiler.h"
#include "emp_ose_error.h"
#include "emp_ose_program.h"
#include "emp_ose_processor.h"
#include "emp_ose_type.h"

#include "emp_png_util.h"

#include "emp_prf_profiler.h"

#include "emp_snd_mixer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

#define enum_name im_operation_e
#define enum_type
#define enum_values\
    enum_value( none         )\
    enum_value( square       )\
    enum_value( triangle     )\
    enum_value( circle       )\
    enum_value( hsba         )\
    enum_value( hsb          )\
    enum_value( sba          )\
    enum_value( hs           )\
    enum_value( hb           )\
    enum_value( sb           )\
    enum_value( sa           )\
    enum_value( ba           )\
    enum_value( hue          )\
    enum_value( saturation   )\
    enum_value( brightness   )\
    enum_value( alpha        )\
    enum_value( translate    )\
    enum_value( translate_xy )\
    enum_value( translate_x  )\
    enum_value( translate_y  )\
    enum_value( rotate       )\
    enum_value( scale        )\
    enum_value( scale_xy     )\
    enum_value( scale_x      )\
    enum_value( scale_y      )\
    enum_value( flip         )
#include "emp_tt_enum.def.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v4f::tret to_rgb(c_float fH, c_float fS, c_float fV, c_float fA)
{
    //if (fS <= 0.0)
    //{ return emp::mat::v4f::make(fH, fS, fV, fA); }

    c_float hh = (fH >= 360.0f ? 0.0f : fH) / 60.0f;

    c_int i = emp::ast::cast<int>(hh);

    c_float ff = hh - emp::ast::cast<float>(i);

    c_float p = fV * (1.0f - fS);
    c_float q = fV * (1.0f - (fS * ff));
    c_float t = fV * (1.0f - (fS * (1.0f - ff)));

    switch(i)
    {
        case 0: { return { fV, t, p, fA }; }
        case 1: { return { q, fV, p, fA }; }
        case 2: { return { p, fV, t, fA }; }
        case 3: { return { p, q, fV, fA }; }
        case 4: { return { t, p, fV, fA }; }
        case 5: default: { return { fV, p, q, fA }; }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v4f::tret to_rgb(v4f::tparam a_v4Hsva)
{ return to_rgb(a_v4Hsva.x, a_v4Hsva.y, a_v4Hsva.z, a_v4Hsva.w); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void rgen_option_s::load(ose::compiler_c& a_rCompiler, ose::program_c& a_rProgram)
{
#define XTMP_OPT(x_Action)\
    auto def = rgen_option_s();\
    auto& rTable = a_rProgram.table();\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__clear_hue")), clearH, def.clearH));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__clear_saturation")), clearS, def.clearS));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__clear_brightness")), clearB, def.clearB));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__clear_alpha")), clearA, def.clearA));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__min_size")), minSize, def.minSize));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__min_alpha")), minAlpha, def.minAlpha));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__origin_x")), originX, def.originX));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__origin_y")), originY, def.originY));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__scale_x")), scaleX, def.scaleX));\
    emp::tt::silent(a_rCompiler.x_Action##float(a_rProgram, rTable, astring(ALITERAL("__scale_y")), scaleY, def.scaleY));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__seed")), seed, def.seed));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__instructions_per_frame")), instructionsPerFrame, def.instructionsPerFrame));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__primitives_per_image")), primitivesPerImage, def.primitivesPerImage));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__triangles_per_frame")), trianglesPerFrame, def.trianglesPerFrame));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__triangles_per_image")), trianglesPerImage, def.trianglesPerImage));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__bytes_per_frame")), bytesPerFrame, def.bytesPerFrame));\
    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__bytes_per_image")), bytesPerImage, def.bytesPerImage));
    XTMP_OPT(get_)
}
//    emp::tt::silent(a_rCompiler.x_Action##uint(a_rProgram, a_rProgram.table(), astring(ALITERAL("__instructions_per_image")), instructionsPerImage, def.instructionsPerImage));\


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void rgen_option_s::save(ose::compiler_c& a_rCompiler, ose::program_c& a_rProgram) const
{
    XTMP_OPT(set_)
}

#undef XTMP_OPT
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e im_stack_c::push(c_bool a_bAppend)
{
    if (visible() == false)
    { return emp::ose::return_e::Break; }

    if (a_bAppend)
    {
    }
    else
    {
        m_vv4Colors.add(m_v4CurrentColor);
        m_Matrices.add(m_mCurrentMatrix);
    }

    return emp::ose::return_e::Continue;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e im_stack_c::pop(void)
{
    m_mCurrentMatrix = m_Matrices.current();
    m_Matrices.pop_no_stack();

    m_v4CurrentColor = m_vv4Colors.last_ref();
    m_v4RgbColor = to_rgb(m_v4CurrentColor);
    m_vv4Colors.resize(m_vv4Colors.count() - 1);

    return emp::ose::return_e::Continue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderlet_c::renderlet_c(void):
    m_sMother(),
    m_sFather(),
    m_pProgram(nullptr),
    m_pStack(),
    m_pImStack(),
    m_pBackgroundVertices(),
    m_pvpImMeshes(),
    m_pImMeshIndices(nullptr),
    m_pImMeshVertices(nullptr),
    m_Stamp(),
    m_m44Transform(),
    m_m44PreviousTransform(),
    m_m44RealPrevious(),
    m_fX(),
    m_fY(),
    m_fScaleX(),
    m_fScaleY(),
    m_fPreviousX(),
    m_fPreviousY(),
    m_fPreviousScaleX(),
    m_fPreviousScaleY(),
    m_origin(),
    m_delta(),
    m_fSizeX(1),
    m_fSizeY(1),
    m_bCompleted(false),
    m_bBeauty(false),
    m_bAllocated(false),
    m_bDealloc(false),
    m_iRendered(0),
    m_aTargets(),
    // @@0 remove m_ResolveTarget(),
    m_stCurrentTarget(0),
    m_stAllocatedBytes(0),
    m_stAllocatedCount(0),
    m_stPrimitiveCount(0)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderlet_c::~renderlet_c(void)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderer_c::renderer_c(void) :
    m_pParent(nullptr),
    m_State(),
    m_Stencil(),
    m_Transform(),
    m_Border(),
    m_mIdentity(),
    m_vpImMeshes(),
    m_stImMesh(),
    m_pRenderlet(nullptr),
    EMP_TT_MAX_CTOR(m_stRenderingRenderlet),
    m_fMinScale(),
    m_vui32Pixels(),
    m_pImQuadVertices(),
    m_pCompiler(),
    m_pProcessor(),
    m_RefreshDirectoryTime(emp::tim::time_c::from_seconds(1.0f)),
    m_RefreshSourceTime(),
    m_ThumbTime(emp::tim::time_c::from_seconds(0.0f)),
    m_mInvalidPrograms(8),
    m_muiPixels(128),
    m_Best(),
    //m_sFilename(),
    m_sMother(),
    m_sFather(),
    m_bProfile(false),
    m_bMutate(false),
    m_bSwitch(false),
    m_bLookup(false),
    m_bPng(false),
    m_bMove(false),
    m_aSimulated(),
    m_Rendered(),
    m_stCurrentSort(0),
    m_avBests(),
    m_vpRenderlets(),
    //m_vpFreeRenderlets(),
    //m_vpRunningRenderlets(),
    m_vsFiles(),
    m_vsToSortFiles(),
    m_sPath(),
    //m_sWorkingDir(ALITERAL("demo")),
    m_sWorkingDir(ALITERAL("src")),
    //m_sWorkingDir(ALITERAL("debug")),
    m_sCurrentFilename(),
    m_iNextFile(0),
    m_bReset(false),
    m_bBakeOptions(false),
    m_bResetOptions(false),
    m_bWriteOptions(false),
    m_bRandomSeed(false),
    m_bNoReload(false),
    m_bFirstBake(false),
    m_stNextTransition(),
    m_stCurrentFile(),
    m_eMod(renderer_e::Edition),
    m_bDragLeft(false),
    m_bDragRight(false),
    m_bLeftPressed(false),
    m_bLeftReleased(false),
    m_bLeftDoubleClick(false),
    m_bRightPressed(false),
    m_bRightReleased(false),
    m_bRightDoubleClick(false),
    m_v2DragOrigin(),
    m_v2DragDelta(),
    EMP_TT_MAX_CTOR(m_fTransitionTime),
    m_fTransitionDuration(0.1f),
    m_fTransitionDelta(),
    m_DeltaTime()
#if defined(EMP_XX_PROFILE)
    , m_pProfiler(ALITERAL("run"))
#endif
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderer_c::~renderer_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_LOG(x_Log)\
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO x_Log;)
#define XTMP_STACK(x_Stack, x_Action, x_From)\
{\
    auto* pImStack = &*m_pRenderlet->m_pImStack;\
    XTMP_LOG(("begin #0 #1, #2", ALITERAL(x_Action), ALITERAL(x_From), pImStack->count()))\
    emp::ose::return_e eReturn = pImStack->x_Stack();\
    XTMP_LOG(("end #0 #1, #2", ALITERAL(x_Action), ALITERAL(x_From), pImStack->count()))\
    if (eReturn != emp::ose::return_e::Continue)\
    { return eReturn; }\
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e renderer_c::push(void)
{
    XTMP_STACK(push, "PUSH", "scope")
    return emp::ose::return_e::Continue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e renderer_c::pop(void)
{
    XTMP_STACK(pop, "POP", "scope")
    return emp::ose::return_e::Continue;
}

#undef XTMP_STACK
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::ose::return_e renderer_c::process(emp::ose::program_c const& EMP_SILENT(a_rProgram), emp::ose::stack_c& a_rStack,
    c_ui32 a_ui32User)
{
    auto const& l_add_mesh = [this]()
    {
        auto& rvpImMeshes = *m_pRenderlet->m_pvpImMeshes;
        if (rvpImMeshes.count() == 0 || std::get<0>(*rvpImMeshes.last_val()).count() >= 2048 * 3)
        {
            std::tuple<buffer_t<ui16>, buffer_t<v2f, v4f>>* pImMesh = nullptr;
            if (m_stImMesh >= m_vpImMeshes.count())
            {
                pImMesh = EMP_ALLOCATE((std::tuple<buffer_t<ui16>, buffer_t<v2f, v4f>>));
                m_vpImMeshes.add(pImMesh);

                if (m_stImMesh % 100 == 0)
                {
                    EMP_LOG_INFO("NEW MESH #0", m_stImMesh);
                }
            }
            else
            {
                pImMesh = m_vpImMeshes.val(m_stImMesh);
                ++m_stImMesh;
            }

            rvpImMeshes.add(pImMesh);
            m_pRenderlet->m_pImMeshIndices = &std::get<0>(*pImMesh);
            m_pRenderlet->m_pImMeshVertices = &std::get<1>(*pImMesh);
            m_pRenderlet->m_pImMeshIndices->create(emp::gfx::usage_e::Dynamic);
            m_pRenderlet->m_pImMeshVertices->create(emp::gfx::usage_e::Dynamic);
        }
    };

    c_im_operation_e eImOperation = static_cast<im_operation_e>(a_ui32User); //@@0 enum_cast
    switch (eImOperation)
    {
        case im_operation_e::square:
        {
            XTMP_LOG(("SQUARE"))
            auto* pImStack = &*m_pRenderlet->m_pImStack;
            emp::ose::return_e eReturn = pImStack->append();
            if (eReturn == ose::return_e::Error)
            { return eReturn; }
            if (eReturn == ose::return_e::Continue && m_bProfile == false)
            {
                l_add_mesh();
                m_pRenderlet->m_pImStack->emit_quad(*m_pRenderlet->m_pImMeshIndices, *m_pRenderlet->m_pImMeshVertices);
                m_pRenderlet->m_stPrimitiveCount++;
            }
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::triangle:
        {
            XTMP_LOG(("TRIANGLE"))
            auto* pImStack = &*m_pRenderlet->m_pImStack;
            emp::ose::return_e eReturn = pImStack->append();
            if (eReturn == ose::return_e::Error)
            { return eReturn; }
            if (eReturn == ose::return_e::Continue && m_bProfile == false)
            {
                l_add_mesh();
                m_pRenderlet->m_pImStack->emit_triangle(*m_pRenderlet->m_pImMeshIndices, *m_pRenderlet->m_pImMeshVertices);
                m_pRenderlet->m_stPrimitiveCount++;
            }
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::circle:
        {
            XTMP_LOG(("CIRCLE"))
            auto* pImStack = &*m_pRenderlet->m_pImStack;
            emp::ose::return_e eReturn = pImStack->append();
            if (eReturn == ose::return_e::Error)
            { return eReturn; }
            if (eReturn == ose::return_e::Continue && m_bProfile == false)
            {
                l_add_mesh();
                m_pRenderlet->m_pImStack->emit_circle(*m_pRenderlet->m_pImMeshIndices, *m_pRenderlet->m_pImMeshVertices); 
                m_pRenderlet->m_stPrimitiveCount++;
            }
            return emp::ose::return_e::Continue;
        }

#define XTMP_POP_FLOAT(x_Name)\
        float x_Name = 0;\
        EMP_RET_X_IF_NOT(emp::ose::return_e::Error, a_rStack.pop(x_Name));

        case im_operation_e::hsba:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HSVA #0, #1, #2, #3", fH, fS, fV, fA))
            m_pRenderlet->m_pImStack->update_color(fH, fS, fV, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hsb:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HSV #0, #1, #2", fH, fS, fV))
            m_pRenderlet->m_pImStack->update_color(fH, fS, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::sba:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::SVA #0, #1, #2", fS, fV, fA))
            m_pRenderlet->m_pImStack->update_color(0, fS, fV, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hs:
        {
            XTMP_POP_FLOAT(fS);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HS #0, #1", fH, fS))
            m_pRenderlet->m_pImStack->update_color(fH, fS, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hb:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::HV #0, #1", fH, fV))
            m_pRenderlet->m_pImStack->update_color(fH, 0, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::sb:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::SV #0, #1", fS, fV))
            m_pRenderlet->m_pImStack->update_color(0, fS, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::sa:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::SA #0, #1", fS, fA))
            m_pRenderlet->m_pImStack->update_color(0, fS, 0, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::ba:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_POP_FLOAT(fV);
            XTMP_LOG(("IM_OPERATION_E::VA #0, #1", fV, fA))
            m_pRenderlet->m_pImStack->update_color(0, 0, fV, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::hue:
        {
            XTMP_POP_FLOAT(fH);
            XTMP_LOG(("IM_OPERATION_E::H #0", fH))
            m_pRenderlet->m_pImStack->update_color(fH, 0, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::saturation:
        {
            XTMP_POP_FLOAT(fS);
            XTMP_LOG(("IM_OPERATION_E::S #0", fS))
            m_pRenderlet->m_pImStack->update_color(0, fS, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::brightness:
        {
            XTMP_POP_FLOAT(fV);
            XTMP_LOG(("IM_OPERATION_E::V #0", fV))
            m_pRenderlet->m_pImStack->update_color(0, 0, fV, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::alpha:
        {
            XTMP_POP_FLOAT(fA);
            XTMP_LOG(("IM_OPERATION_E::ALPHA #0", fA))
            m_pRenderlet->m_pImStack->update_color(0, 0, 0, fA);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate:
        {
            XTMP_POP_FLOAT(fXY);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE #0", fXY))
            m_pRenderlet->m_pImStack->translate(fXY, fXY, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate_xy:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE_XY #0 #1", fX, fY))
            m_pRenderlet->m_pImStack->translate(fX, fY, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate_x:
        {
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE_X #0", fX))
            m_pRenderlet->m_pImStack->translate(fX, 0, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::translate_y:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_LOG(("IM_OPERATION_E::TRANSLATE_Y #0", fY))
            m_pRenderlet->m_pImStack->translate(0, fY, 0);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::rotate:
        {
            XTMP_POP_FLOAT(fR);
            XTMP_LOG(("IM_OPERATION_E::ROTATE #0", fR))
            m_pRenderlet->m_pImStack->rotate(fR * emp::mat::to_rad<float>());
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale:
        {
            XTMP_POP_FLOAT(fXY);
            XTMP_LOG(("IM_OPERATION_E::SCALE #0", fXY))
            m_pRenderlet->m_pImStack->scale(fXY, fXY, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale_xy:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::SCALE_XY #0, #1", fX, fY))
            m_pRenderlet->m_pImStack->scale(fX, fY, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale_x:
        {
            XTMP_POP_FLOAT(fX);
            XTMP_LOG(("IM_OPERATION_E::SCALE_X #0", fX))
            m_pRenderlet->m_pImStack->scale(fX, 1, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::scale_y:
        {
            XTMP_POP_FLOAT(fY);
            XTMP_LOG(("IM_OPERATION_E::SCALE_Y #0", fY))
            m_pRenderlet->m_pImStack->scale(1, fY, 1);
            return emp::ose::return_e::Continue;
        }
        case im_operation_e::flip:
        {
            XTMP_POP_FLOAT(fF);
            XTMP_LOG(("IM_OPERATION_E::FLIP #0", fF))
            m_pRenderlet->m_pImStack->flip(fF * emp::mat::to_rad<float>());
            return emp::ose::return_e::Continue;
        }
#undef XTMP_LOG
#undef XTMP_POP_FLOAT
        case im_operation_e::none:
        default:
        {
            EMP_LOG_ERROR("invalid im_operation_e #0", a_ui32User);
            return emp::ose::return_e::Error;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::im_operation_e first_im_operation(void);
EMP_RETURN emp::gui::im_operation_e first_im_operation(void)
{ return emp::gui::im_operation_e::none; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::destroy_buffers(void)
{
    auto const& l_destroy_target = [](emp::gfx::target_s& a_rTarget)
    {
        auto& rDevice = EMP_GRAPHICS;
        if (a_rTarget.m_ui32Texture != 0)
        {
            rDevice.glDeleteTextures_(1, &a_rTarget.m_ui32Texture);
            a_rTarget.m_ui32Texture = 0;
        }

        if (a_rTarget.m_ui32Framebuffer != 0)
        {
            rDevice.glDeleteFramebuffers_(1, &a_rTarget.m_ui32Framebuffer);
            a_rTarget.m_ui32Framebuffer = 0;
        }

        if (a_rTarget.m_ui32Renderbuffer != 0)
        {
            rDevice.glDeleteRenderbuffers_(1, &a_rTarget.m_ui32Renderbuffer);
            a_rTarget.m_ui32Renderbuffer = 0;
        }
    };

    for (size_t stIndex = 0; stIndex < m_vpRenderlets.count(); ++stIndex)
    {
        renderlet_c* const pRenderlet = m_vpRenderlets.val(stIndex);
        for (size_t stTarget = 0; stTarget < pRenderlet->m_aTargets.count(); ++stTarget)
        { l_destroy_target(pRenderlet->m_aTargets.ref(stTarget)); }
        // @@0 remove l_destroy_target(pRenderlet->m_ResolveTarget);
    }

    l_destroy_target(m_PngTarget);
    l_destroy_target(m_MsaaTarget0);
    l_destroy_target(m_MsaaTarget1);

    l_destroy_target(m_ResolveTarget);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::resize_buffers(c_f64 a_dWidth, c_f64 a_dHeight)
{
    GLsizei szWidth = emp::ast::cast<GLsizei>(a_dWidth);
    GLsizei szHeight = emp::ast::cast<GLsizei>(a_dHeight);

    auto& rDevice = EMP_GRAPHICS;

    auto const& l_resize_target = [this, &rDevice] (c_si32 l_si32Width, c_si32 l_si32Height, emp::gfx::target_s& a_rTarget, bool msaa = false) EMP_RETURN -> bool
    {
        emp::gfx::target_s oNewTarget;
        oNewTarget.m_siWidth = l_si32Width;
        oNewTarget.m_siHeight = l_si32Height;
        rDevice.glGenTextures_(1, &oNewTarget.m_ui32Texture);
        rDevice.glGenFramebuffers_(1, &oNewTarget.m_ui32Framebuffer);
        rDevice.glGenRenderbuffers_(1, &oNewTarget.m_ui32Renderbuffer);

        GLenum glTarget = GL_TEXTURE_2D;
        if (msaa)
        {
            glTarget = 0x9100;
        }

        rDevice.glBindTexture_(glTarget, oNewTarget.m_ui32Texture);

        if (!msaa)
        {
        rDevice.glTexParameteri_(glTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
        rDevice.glTexParameteri_(glTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
        rDevice.glTexParameteri_(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        rDevice.glTexParameteri_(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        if (msaa)
        {
            rDevice.glTexImage2DMultisample_(glTarget, 8, GL_RGBA, l_si32Width, l_si32Height, false);

        }
        else
        {
        rDevice.glTexImage2D_(glTarget, 0, GL_RGBA, l_si32Width, l_si32Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        }

        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oNewTarget));

        rDevice.glFramebufferTexture2D_(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glTarget, oNewTarget.m_ui32Texture, 0);

        //rDevice.glBindRenderbuffer_(GL_RENDERBUFFER, oNewTarget.m_ui32Renderbuffer);
        //rDevice.glRenderbufferStorage_(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, l_si32Width, l_si32Height);
        //rDevice.glFramebufferRenderbuffer_(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, oNewTarget.m_ui32Renderbuffer);

        EMP_RET_FALSE_IF_NOT(rDevice.framebuffer_status());
        EMP_RET_FALSE_IF_NOT(rDevice.clear(v4f::zero()));

        c_bool bDestroy = a_rTarget.m_ui32Texture != 0 && a_rTarget.m_ui32Framebuffer != 0 && a_rTarget.m_ui32Renderbuffer != 0;
        EMP_RET_FALSE_IF_NOT(bDestroy || (a_rTarget.m_ui32Texture == 0 && a_rTarget.m_ui32Framebuffer == 0 && a_rTarget.m_ui32Renderbuffer == 0));
        
        if (bDestroy)
        {
            tx_d const& mTransform = m_Transform.matrix();

            rDevice.set_blend(true);
            rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
            rDevice.set_vertex_buffer(&*m_pImQuadVertices);
            rDevice.set_index_buffer(rDevice.quad_index_buffer());
            rDevice.set_program(rDevice.screen_program());
            rDevice.set_uniform(uniform_e::m44Transform, mTransform);
            rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rTarget.m_ui32Texture);
            rDevice.render(emp::gfx::primitive_e::Triangles);

            rDevice.glDeleteTextures_(1, &a_rTarget.m_ui32Texture);
            a_rTarget.m_ui32Texture = 0;

            rDevice.glDeleteFramebuffers_(1, &a_rTarget.m_ui32Framebuffer);
            a_rTarget.m_ui32Framebuffer = 0;

            rDevice.glDeleteRenderbuffers_(1, &a_rTarget.m_ui32Renderbuffer);
            a_rTarget.m_ui32Renderbuffer = 0;
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

    EMP_RET_FALSE_IF_NOT(l_resize_target(2048, 2048, m_PngTarget));
    EMP_RET_FALSE_IF_NOT(l_resize_target(4096, 4096, m_ResolveTarget, false));
    EMP_RET_FALSE_IF_NOT(l_resize_target(4096, 4096, m_MsaaTarget0, true));
    EMP_RET_FALSE_IF_NOT(l_resize_target(4096, 4096, m_MsaaTarget1, true));

    return true;
}

int compare_string(c_astring& from, c_astring& to);
int compare_string(c_astring& from, c_astring& to)
{
    c_astring* pLeft = &from;
    c_astring* pRight = &to;
    int invert = 1;
    if (pRight->len() < pLeft->len())
    {
        pLeft = &to;
        pRight = &from;
        invert = -1;
    }

    for (size_t i = 0; i < pLeft->len(); ++i)
    {
        auto const cLeft = pLeft->get_char(i);
        if (i < pRight->len())
        {
            auto const cRight = pRight->get_char(i);
            if (cLeft == cRight)
            {
                continue;
            }
            else
            {
                return invert * (cRight - cLeft);
            }
        }
        else
        {
            return invert * (0 - cLeft);
        }
    }

    return 0;
}

void sort_files(vector<astring> const& from, vector<astring>& to);
void sort_files(vector<astring> const& from, vector<astring>& to)
{
    to.resize(0);

    for (size_t i = 0; i < from.count(); ++i)
    {
        auto& rFrom = from.ref(i);
        bool added = false;
        for (size_t k = 0; k < to.count(); ++k)
        {
            auto& rTo = to.ref(k);
            if (compare_string(rFrom, rTo) >= 0)
            {
                to.insert(k, rFrom);
                added = true;
                break;
            }
        }

        if (added == false)
        {
            to.add(rFrom);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_DISABLED_OPTIMIZATION)
EMP_RETURN bool renderer_c::create(void)
{
    m_iNextFile = 1;
    m_stCurrentFile = 0;

    m_RandomFloat.reset_seed(std::chrono::system_clock::now().time_since_epoch().count());

    EMP_APPLICATION.add_resize_hook(emp::fct::function(this, &renderer_c::resize_buffers));

    emp::app::application_c const& rApplication = EMP_APPLICATION;

    c_f64 dWidth = rApplication.view_pixel_width();
    c_f64 dHeight = rApplication.view_pixel_height();

    EMP_RET_FALSE_IF_NOT(m_pCompiler->create());

    emp::ose::rti::data_c const* const pRealType = m_pCompiler->table().real_type();

    c_size stFloatCount = 4;
    emp::ose::table_c& rTable = m_pCompiler->table();
    for (size_t stIndex = 0; stIndex < stFloatCount; ++stIndex)
    {
        emp::ose::rti::function_c* pType = rTable.new_type<emp::ose::rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);

        for (size_t stFloat = 0; stFloat <= stIndex; ++stFloat)
        { EMP_RET_FALSE_IF(pType->add_parameter(pRealType) == false); }
        EMP_RET_FALSE_IF(rTable.qualify_type(pType) == false);
        EMP_RET_FALSE_IF(rTable.reg_type<emp::ose::rti::function_c>(pType) == false);
        EMP_LOG_INFO("REGISTERED TYPE #0", pType->name());
    }

#define XTMP_GUI_REG(x_Extension, x_Name, x_Type)\
    case emp::gui::im_operation_e::x_Extension:\
    {\
        if (m_pCompiler->table().reg_extension(enum_to__(im_operation_e::x_Extension), ALITERAL(x_Name), ALITERAL(x_Type)) == nullptr)\
        {\
            EMP_LOG_ERROR("failed to register instruction im_operation_e::#0, #1, #2",\
                ALITERAL(EMP_PP_STRING(x_Extension)), ALITERAL(x_Name), ALITERAL(x_Type));\
            return false;\
        }\
    }

    switch (first_im_operation())
    {
        case im_operation_e::none: {}
        XTMP_GUI_REG(square       , "square"    , "()()")
        XTMP_GUI_REG(triangle     , "triangle"  , "()()")
        XTMP_GUI_REG(circle       , "circle"    , "()()")
        XTMP_GUI_REG(hsba         , "hsba"      , "()(float,float,float,float,)")
        XTMP_GUI_REG(hsb          , "hsb"       , "()(float,float,float,)")
        XTMP_GUI_REG(sba          , "sba"       , "()(float,float,float,)")
        XTMP_GUI_REG(hs           , "hs"        , "()(float,float,)")
        XTMP_GUI_REG(hb           , "hb"        , "()(float,float,)")
        XTMP_GUI_REG(sb           , "sb"        , "()(float,float,)")
        XTMP_GUI_REG(sa           , "sa"        , "()(float,float,)")
        XTMP_GUI_REG(ba           , "ba"        , "()(float,float,)")
        XTMP_GUI_REG(hue          , "h"         , "()(float,)")
        XTMP_GUI_REG(saturation   , "u"         , "()(float,)")
        XTMP_GUI_REG(brightness   , "b"         , "()(float,)")
        XTMP_GUI_REG(alpha        , "a"         , "()(float,)")
        XTMP_GUI_REG(translate    , "t"         , "()(float,)")
        XTMP_GUI_REG(translate_xy , "t"         , "()(float,float,)")
        XTMP_GUI_REG(translate_x  , "x"         , "()(float,)")
        XTMP_GUI_REG(translate_y  , "y"         , "()(float,)")
        XTMP_GUI_REG(rotate       , "r"         , "()(float,)")
        XTMP_GUI_REG(scale        , "s"         , "()(float,)")
        XTMP_GUI_REG(scale_xy     , "s"         , "()(float,float,)")
        XTMP_GUI_REG(scale_x      , "sx"        , "()(float,)")
        XTMP_GUI_REG(scale_y      , "sy"        , "()(float,)")
        XTMP_GUI_REG(flip         , "f"         , "()(float,)")
        default:
        { break; }
    };
#undef XTMP_REG

    //EMP_RET_FALSE_IF(m_pCompiler->end_create() == false)

    m_pProcessor->set_extension(emp::fct::function(this, &renderer_c::process));
    m_pProcessor->set_push(emp::fct::function(this, &renderer_c::push));
    m_pProcessor->set_pop(emp::fct::function(this, &renderer_c::pop));

    m_pImQuadVertices->create(emp::gfx::usage_e::Static);
    m_pImQuadVertices->add({ 0, 0 }, { 0, 0 });
    m_pImQuadVertices->add({ 2, 0 }, { 1, 0 });
    m_pImQuadVertices->add({ 0, 2 }, { 0, 1 });
    m_pImQuadVertices->add({ 2, 2 }, { 1, 1 });

    if (m_eMod == renderer_e::Generate)
    {
        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            auto* pSimulated = m_aSimulated.ptr(stIndex);
            pSimulated->m_pImStack->init_circle(128);
            m_vpRenderlets.add(pSimulated);
        }
    }

    m_Rendered.m_pImStack->init_circle(128);
    m_vpRenderlets.add(&m_Rendered);

    // @@0 remove
    //for (size_t stIndex = 0; stIndex < sc_stRenderletCount; ++stIndex)
    //{
    //    renderlet_c* const pRenderlet = EMP_ALLOCATE(renderlet_c);
    //    m_vpRenderlets.add(pRenderlet);
    //    m_vpFreeRenderlets.add(pRenderlet);
    //    pRenderlet->m_pImStack->init_circle(128);
    //}

    EMP_RET_FALSE_IF_NOT(resize_buffers(dWidth, dHeight));

    EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());

    EMP_TT_MAX_VAR(m_fTransitionTime);

    m_sPath.clear();
    emp::tt::silent(STR_FORMATA(m_sPath, "#0/#1/", EMP_APPLICATION.resource_path(), m_sWorkingDir));

    m_vsToSortFiles.resize(0);
    EMP_RET_FALSE_IF_NOT(emp::fs::list(m_sPath, ALITERAL("*.txt"), m_vsToSortFiles, false));
    sort_files(m_vsToSortFiles, m_vsFiles);
    m_stFileCount = m_vsFiles.count();


    //m_mInvalidPrograms.set(ALITERAL("rgen_0"), 0);
    //m_mInvalidPrograms.set(ALITERAL("rgen_1"), 0);
    return true;
}
EMP_PRAGMA_POP_IGNORE(EMP_W_DISABLED_OPTIMIZATION)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::destroy(void)
{
    m_pParent = nullptr;

    m_State = state_data_s();
    m_Stencil.destroy();
    m_Transform.destroy();
    m_Border.destroy();

    destroy_buffers();

    emp::cnt::foreach(m_vpImMeshes, emp::mem::safe_delete_reset_t());
    m_vpImMeshes.resize(0);
    m_stImMesh = 0;

    emp::cnt::foreach(m_vpRenderlets, emp::mem::safe_delete_reset_t());
    m_vpRenderlets.resize(0);

    // @@0 remove
    //m_vpFreeRenderlets.resize(0);
    //m_vpRunningRenderlets.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void renderer_c::update(emp::tim::clock_c const& a_rClock)
{
    m_DeltaTime = a_rClock.delta();


    emp::tim::time_c::add(m_RefreshSourceTime, m_DeltaTime, m_RefreshSourceTime);
    emp::tim::time_c::add(m_RefreshDirectoryTime, m_DeltaTime, m_RefreshDirectoryTime);
    emp::tim::time_c::add(m_ThumbTime, m_DeltaTime, m_ThumbTime);

    /*if (emp::tim::time_c::more(m_RefreshDirectoryTime, emp::tim::time_c::from_seconds(2.0f)))
    {
        m_vsFiles.resize(0);
        if (emp::fs::list(m_sPath, ALITERAL("*.txt"), m_vsFiles, false))
        {
            sort_files(m_vsToSortFiles, m_vsFiles);
            m_RefreshDirectoryTime = emp::tim::time_c();
        }
    }*/

    if (emp::tim::time_c::more(m_RefreshSourceTime, emp::tim::time_c::from_seconds(0.5f)))
    {
        if (m_Rendered.m_pProgram != nullptr)
        {
            emp::fs::file_c oFile;
            if (oFile.create(STR_AFORMAT("#0/#1", m_sPath, m_Rendered.m_sMother), emp::fs::file_e::Text, emp::fs::create_e::Read))
            {
                astring sBuffer;
                if (oFile.read_all(sBuffer))
                {
                    if (false == emp::str::cmp(sBuffer, m_Rendered.m_pProgram->buffer()))
                    {
                        m_bReset = true;
                    }
                }
            }
        } else {
            m_bReset = true;
        }
    }


    m_fTransitionDelta = emp::tt::minof(1 / 60.0f, emp::tim::time_c::to_seconds(m_DeltaTime));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool print_ose(ose::compiler_c& a_rCompiler, renderlet_c& a_rRenderlet, astring& a_rsOutput);
EMP_RETURN bool print_ose(ose::compiler_c& a_rCompiler, renderlet_c& a_rRenderlet, astring& a_rsOutput)
{
    bool bEndScope = false;
    bool bEndDef = false;
    EMP_ASSERT_BOOL_CALL(a_rRenderlet.m_pProgram->rename());
    if (a_rRenderlet.m_pProgram != nullptr && a_rRenderlet.m_pProgram->is_valid_table())
    {
            /*
            a_rRenderlet.m_viewport = a_rRenderlet.m_stackViewport;
            c_float fx = a_rRenderlet.m_fX;
            c_float fy = a_rRenderlet.m_fY;
            c_float sx = a_rRenderlet.m_fScaleX;
            c_float sy = a_rRenderlet.m_fScaleY;
            a_rRenderlet.m_backupTransform.x = fx;
            a_rRenderlet.m_backupTransform.y = fy;
            a_rRenderlet.m_backupTransform.z = sx;
            a_rRenderlet.m_backupTransform.w = sy;

            c_float dx = (a_rRenderlet.m_origin.x + a_rRenderlet.m_delta.x);
            c_float dy = (a_rRenderlet.m_origin.y + a_rRenderlet.m_delta.y);
            a_rRenderlet.m_pImStack->translate(dx, dy, 0.0);

            auto const& v = a_rRenderlet.m_backupTransform;
            a_rRenderlet.m_pImStack->scale(v.z, v.w, 1.0f);
            a_rRenderlet.m_pImStack->translate(v.x, v.y, 0.0);
            a_rRenderlet.m_pImStack->set_min_scale(minScale * v.z * a_rRenderlet.m_fSizeX);
            a_rRenderlet.m_pImStack->scale(a_rRenderlet.m_fSizeX, a_rRenderlet.m_fSizeY, 1.0);
*/


        a_rRenderlet.m_Options.save(a_rCompiler, *a_rRenderlet.m_pProgram);
    }


    if (emp::ose::print_ose(a_rRenderlet.m_pProgram->ast(), 0, a_rsOutput, bEndScope, bEndDef, false))
    {
        a_rRenderlet.m_pProgram->set_buffer(a_rsOutput);
        return true;
    }

    return false;
}


EMP_RETURN bool renderer_c::transform(tx_stack_d& a_rStack)
{
    if (m_State.visible() == false)
    { return true; }

    auto& rDevice = EMP_GRAPHICS;

    auto const& l_change_dna = [this](astring& a_sFile, c_int a_iDirection, c_bool a_bBreak) -> bool
    {
        a_sFile = astring();
       c_size stFileCount = m_vsFiles.count();

        if (a_iDirection < 0 && (m_stCurrentFile <= 0) && a_bBreak)
        { return false; }

        if (a_iDirection > 0 && (m_stCurrentFile >= stFileCount) && a_bBreak)
        { return false; }

        while (true)
        {
            if (a_iDirection == 0)
            {
                m_stCurrentFile = emp::ast::cast<size_t>(emp::mat::abs(emp::mat::random()));
            }
            else if (a_iDirection < 0)
            {
                if (m_stCurrentFile <= 0)
                { m_stCurrentFile = m_vsFiles.count(); }
                else
                { --m_stCurrentFile; }
            }
            else if (a_iDirection > 0)
            { ++m_stCurrentFile; }

            if (a_bBreak == false || a_iDirection == 0)
            {
                if (m_vsFiles.count() > 0)
                { m_stCurrentFile = m_stCurrentFile % m_vsFiles.count(); }
                else
                { m_stCurrentFile = 0; }
            }

            if (m_vsFiles.is_valid_index(m_stCurrentFile))
            {
                c_astring& sFile = m_vsFiles.ref(m_stCurrentFile);
                if (m_mInvalidPrograms.get(sFile) == m_mInvalidPrograms.end())
                {
                    a_sFile = sFile;
                    EMP_LOG_INFO("Changing DNA #0/#1 : #2", m_stCurrentFile, stFileCount, sFile);
                    return true;

                }
            }
            else
            {
                break;
            }
        }
        return false;
    };

    auto const& l_update_renderlet = [this](renderlet_c& a_rRenderlet)
    {
        emp::ose::program_c const* const pProg = a_rRenderlet.m_pProgram;
        if (pProg != nullptr && a_rRenderlet.m_bCompleted == false &&
            a_rRenderlet.m_pStack->instruction_pointer() != pProg->exit_point() /*&&
            a_rRenderlet.m_stAllocatedBytes < (2048 << 10u) &&
            emp::tim::time_c::to_seconds(a_rRenderlet.m_Stamp) < 180.0f*/)
        {
            m_pRenderlet = &a_rRenderlet;

            c_size stCurrentBytes = EMP_ALLOCATOR.current_bytes();
            c_size stCurrentCount = EMP_ALLOCATOR.current_count();

            emp::tim::time_c::add(a_rRenderlet.m_Stamp, m_DeltaTime, a_rRenderlet.m_Stamp);


            c_size stInstructionCount = a_rRenderlet.m_Options.instructionsPerFrame;
            c_si32 siCount = m_pProcessor->run(*pProg, *a_rRenderlet.m_pStack, stInstructionCount, a_rRenderlet.m_RandomFloat);


            c_size stNextBytes = EMP_ALLOCATOR.current_bytes();
            c_size stNextCount = EMP_ALLOCATOR.current_count();

            a_rRenderlet.m_stAllocatedBytes += stNextBytes > stCurrentBytes ? stNextBytes - stCurrentBytes : 0;
            a_rRenderlet.m_stAllocatedCount += stNextCount > stCurrentCount ? stNextCount - stCurrentCount : 0;
            a_rRenderlet.m_stInstructionCount += stInstructionCount;

            if (a_rRenderlet.m_Options.instructionsPerImage > 0 && a_rRenderlet.m_stInstructionCount >= a_rRenderlet.m_Options.instructionsPerImage)
            {
                EMP_LOG_INFO("REACHED MAX INSTRUCTION COUNT : stopping : '#0' x '#1', #2", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather, a_rRenderlet.m_Options.instructionsPerImage);
                a_rRenderlet.m_bCompleted = true;
            }

            if (siCount < 0)
            {
                EMP_LOG_INFO("Failed A to run program '#0' x '#1'", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather);
            }
            else if (siCount == 0 && a_rRenderlet.m_pImStack->count() != 1)
            {
                EMP_LOG_INFO("Failed B to run program '#0' x '#1', invalid stack #2", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather, a_rRenderlet.m_pImStack->count());
            }
        }
        else if (a_rRenderlet.m_bCompleted == false && pProg != nullptr)
        {
            EMP_LOG_INFO("PROCESSING COMPLETE : stopping : '#0' x '#1'", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather);
            a_rRenderlet.m_bCompleted = true;
        }
    };

    auto const& l_reset_renderlet = [this, &rDevice](renderlet_c& a_rRenderlet, c_astring& a_sMother, c_astring& a_sFather) EMP_UNUSED -> bool
    {
        a_rRenderlet.m_sMother = a_sMother;
        a_rRenderlet.m_sFather = a_sFather;

        a_rRenderlet.m_Stamp = emp::tim::time_c();

        a_rRenderlet.m_pProgram = nullptr;

        a_rRenderlet.m_fX = 0;
        a_rRenderlet.m_fY = 0;
        a_rRenderlet.m_fScaleX = 1;
        a_rRenderlet.m_fScaleY = 1;

        a_rRenderlet.m_fPreviousX = 0;
        a_rRenderlet.m_fPreviousY = 0;
        a_rRenderlet.m_fPreviousScaleX = 1;
        a_rRenderlet.m_fPreviousScaleY = 1;

        a_rRenderlet.m_bCompleted = false;
        a_rRenderlet.m_bBeauty = false;
        a_rRenderlet.m_iRendered = 0;
        a_rRenderlet.m_iFrame = 0;

        tx_d::identity(a_rRenderlet.m_m44Transform);
        tx_d::identity(a_rRenderlet.m_m44PreviousTransform);
        tx_d::identity(a_rRenderlet.m_m44RealPrevious);

        a_rRenderlet.m_pvpImMeshes->resize(0);
        a_rRenderlet.m_pImMeshIndices = nullptr;
        a_rRenderlet.m_pImMeshVertices = nullptr;
        a_rRenderlet.m_pImStack->clear();
        a_rRenderlet.m_pImStack->scale(1, EMP_GUI_SKIN.resolution().current_ratio(), 1);

        //c_float dx = (a_rRenderlet.m_origin.x + a_rRenderlet.m_delta.x);
        //c_float dy = (a_rRenderlet.m_origin.y + a_rRenderlet.m_delta.y);
        //a_rRenderlet.m_pImStack->translate(dx, dy, 0.0);


        //a_rRenderlet.m_pImStack->scale(a_rRenderlet.m_fSizeX, a_rRenderlet.m_fSizeY, 1.0);

        emp::tt::silent(a_rRenderlet.m_pImStack->push());
        a_rRenderlet.m_stAllocatedBytes = 0;
        a_rRenderlet.m_stAllocatedCount = 0;
        a_rRenderlet.m_stPrimitiveCount = 0;
        a_rRenderlet.m_stInstructionCount = 0;

        /*
            a_rRenderlet.m_viewport = a_rRenderlet.m_stackViewport;
            c_float fx = a_rRenderlet.m_fX;
            c_float fy = a_rRenderlet.m_fY;
            c_float sx = a_rRenderlet.m_fScaleX;
            c_float sy = a_rRenderlet.m_fScaleY;
            a_rRenderlet.m_backupTransform.x = fx;
            a_rRenderlet.m_backupTransform.y = fy;
            a_rRenderlet.m_backupTransform.z = sx;
            a_rRenderlet.m_backupTransform.w = sy;

            c_float dx = (a_rRenderlet.m_origin.x + a_rRenderlet.m_delta.x);
            c_float dy = (a_rRenderlet.m_origin.y + a_rRenderlet.m_delta.y);
            a_rRenderlet.m_pImStack->translate(dx, dy, 0.0);

            auto const& v = a_rRenderlet.m_backupTransform;
            a_rRenderlet.m_pImStack->scale(v.z, v.w, 1.0f);
            a_rRenderlet.m_pImStack->translate(v.x, v.y, 0.0);
            a_rRenderlet.m_pImStack->set_min_scale(minScale * v.z * a_rRenderlet.m_fSizeX);
            a_rRenderlet.m_pImStack->scale(a_rRenderlet.m_fSizeX, a_rRenderlet.m_fSizeY, 1.0);
*/





        c_size stTargetCount = a_rRenderlet.m_aTargets.count();
        for (size_t stTarget = 0; stTarget < stTargetCount; ++stTarget)
        {
            emp::gfx::target_s const& rTarget = a_rRenderlet.m_aTargets.ref(stTarget);
            EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(rTarget));
            EMP_RET_FALSE_IF_NOT(rDevice.clear(v4f::zero()));
        }


        return true;
    };

    auto const& l_transform_renderlet = [this](renderlet_c& a_rRenderlet)
    {
        auto* const pVertices = &*a_rRenderlet.m_pBackgroundVertices;
        pVertices->create(emp::gfx::usage_e::Dynamic);

        c_v2f& v2Size = transform().size();
        c_float fRight = v2Size.x;
        c_float fTop = v2Size.y;
        pVertices->add(v2f::zero());
        pVertices->add({ fRight, 0 });
        pVertices->add({ 0, fTop });
        pVertices->add(v2Size);

        tx_d::identity(a_rRenderlet.m_m44PreviousTransform);

        auto const* const pImStack = &*a_rRenderlet.m_pImStack;
        auto v2Min = pImStack->min();
        auto v2Max = pImStack->max();
        auto const& rOptions = a_rRenderlet.m_Options;
        if (rOptions.scaleX > 0 || rOptions.scaleY > 0)
        {
            a_rRenderlet.m_fX = rOptions.originX;
            a_rRenderlet.m_fY = rOptions.originY;
            a_rRenderlet.m_fScaleX = rOptions.scaleX;
            a_rRenderlet.m_fScaleY = rOptions.scaleY;
            tx_d::identity(a_rRenderlet.m_m44RealPrevious);
            tx_d::identity(a_rRenderlet.m_m44Transform);
        }
        else
        {
            if (!bool_equals(v2Min, emp::tt::max<v2f>::value))
            {
                c_float fX = v2Min.x;
                c_float fY = v2Min.y;
                c_float fWidth = (v2Max.x - v2Min.x);
                c_float fHeight = (v2Max.y - v2Min.y);

                c_float fMaxSize = emp::tt::maxof(fWidth,fHeight);

                c_float fNewX = 0.5f * (fMaxSize - fWidth) - fX;
                c_float fNewY = 0.5f * (fMaxSize - fHeight) - fY;

                if (a_rRenderlet.m_iRendered <= 0)
                {
                    a_rRenderlet.m_fPreviousX = a_rRenderlet.m_fX;
                    a_rRenderlet.m_fPreviousY = a_rRenderlet.m_fY;
                    a_rRenderlet.m_fPreviousScaleX = a_rRenderlet.m_fScaleX;
                    a_rRenderlet.m_fPreviousScaleY = a_rRenderlet.m_fScaleY;
                }

                a_rRenderlet.m_fX = fNewX;
                a_rRenderlet.m_fY = fNewY;

                //c_float fNewSizeX = /*bReset ? fNewWidth :*/ emp::tt::minof(fNewWidth, fNewHeight);
                //c_float fNewSizeY = /*bReset ? fNewHeight :*/ fNewSizeX;

                c_float fScaleX = 2.0f / fWidth;
                c_float fScaleY = 2.0f / fHeight;
                c_float fScale = emp::tt::minof(fScaleX, fScaleY);

                a_rRenderlet.m_fScaleX = fScale;
                a_rRenderlet.m_fScaleY = fScale;
            }
            else
            {
                a_rRenderlet.m_fX = 0;
                a_rRenderlet.m_fY = 0;
                a_rRenderlet.m_fScaleX = 1;
                a_rRenderlet.m_fScaleY = 1;

                a_rRenderlet.m_fPreviousX =0 ;
                a_rRenderlet.m_fPreviousY = 0;
                a_rRenderlet.m_fPreviousScaleX = 1;
                a_rRenderlet.m_fPreviousScaleY = 1;
            }

            {
                tx_d scaling;
                tx_d::identity(scaling);
                tx_d::scale(scaling, {  1 / a_rRenderlet.m_fPreviousScaleX,  1 / a_rRenderlet.m_fPreviousScaleY, 1 }, scaling);

                tx_d translate;
                tx_d::identity(translate);
                tx_d::translate(translate, { -a_rRenderlet.m_fPreviousX, -a_rRenderlet.m_fPreviousY, 0 }, translate);

                tx_d::mul(translate, scaling, a_rRenderlet.m_m44RealPrevious);
            }

            bool bChanged = false;
            if (
                    emp::tt::equal_(a_rRenderlet.m_fPreviousScaleX, a_rRenderlet.m_fScaleX) == false ||
                    emp::tt::equal_(a_rRenderlet.m_fPreviousScaleY, a_rRenderlet.m_fScaleY) == false ||
                    emp::tt::equal_(a_rRenderlet.m_fPreviousX, a_rRenderlet.m_fX) == false ||
                    emp::tt::equal_(a_rRenderlet.m_fPreviousY, a_rRenderlet.m_fY) == false)
            {
                a_rRenderlet.m_m44PreviousTransform = a_rRenderlet.m_m44RealPrevious;
                bChanged = true;
            }

            {
                tx_d::type scaling;
                tx_d::identity(scaling);
                tx_d::scale(scaling, { a_rRenderlet.m_fScaleX, a_rRenderlet.m_fScaleY, 1.0f }, scaling);

                tx_d translate;
                tx_d::identity(translate);
                tx_d::translate(translate, { a_rRenderlet.m_fX, a_rRenderlet.m_fY, 0 }, translate);

                tx_d::mul(scaling, translate, a_rRenderlet.m_m44Transform);
                if (bChanged)
                {
                    tx_d::mul(a_rRenderlet.m_m44Transform, a_rRenderlet.m_m44PreviousTransform, a_rRenderlet.m_m44PreviousTransform);
                }
            }
        }
    };

    auto const& l_render_renderlet = [&rDevice, this](renderlet_c& a_rRenderlet) EMP_UNUSED -> bool
    {
        rDevice.set_depth_mask(false);
        rDevice.set_depth_test(false);
        rDevice.set_stencil_test(false);
        rDevice.set_dither(false);
        rDevice.set_polygon_smooth(false);

        auto mTransform = m_Transform.matrix();

        auto const& rOptions = a_rRenderlet.m_Options;
        auto const vClearColor = to_rgb(rOptions.clearH, rOptions.clearS, rOptions.clearB, rOptions.clearA);
        c_size stMeshCount = a_rRenderlet.m_pvpImMeshes->count();


        size_t stPrevioustTexture = a_rRenderlet.m_stCurrentTarget;
        if (stPrevioustTexture <= 0)
        { stPrevioustTexture = a_rRenderlet.m_aTargets.count() - 1; }
        else
        { stPrevioustTexture = (stPrevioustTexture - 1) % a_rRenderlet.m_aTargets.count(); }

        a_rRenderlet.m_iFrame++;

        if (a_rRenderlet.m_iRendered > 0)
        {
            if (a_rRenderlet.m_stPrimitiveCount <= 0)
            {
            }
            else
            {
                if (a_rRenderlet.m_Options.scaleX <= 0 || a_rRenderlet.m_Options.scaleY <= 0)
                {
                    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
                    EMP_RET_FALSE_IF_NOT(rDevice.clear(vClearColor));
                    tx_d mCurrent;
                    tx_d::mul(a_rRenderlet.m_m44RealPrevious, a_rRenderlet.m_m44Transform, mCurrent);
                    tx_d::mul(mTransform, mCurrent, mCurrent);
                    rDevice.set_blend(true);
                    rDevice.glBlendFunc_(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    rDevice.set_vertex_buffer(&*m_pImQuadVertices);
                    rDevice.set_index_buffer(rDevice.quad_index_buffer());
                    rDevice.set_program(rDevice.screen_program());
                    rDevice.set_uniform(uniform_e::m44Transform, mCurrent);
                    rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rRenderlet.m_aTargets.ref(stPrevioustTexture).m_ui32Texture);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    rDevice.render(emp::gfx::primitive_e::Triangles);
                }
            }
        }
        else
        {
            if (stMeshCount <= 0 && a_rRenderlet.m_stPrimitiveCount <= 0)
            {
                EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
                EMP_RET_FALSE_IF_NOT(rDevice.clear(vClearColor));
            }
            else if (stMeshCount > 0)
            {
                if (a_rRenderlet.m_Options.scaleX > 0 && a_rRenderlet.m_Options.scaleY > 0)
                {
                    //auto const& rTarget0 = a_rRenderlet.m_aTargets.ref(0);
                    auto const& rTarget0 = m_MsaaTarget0;

                    // current meshes
                    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(rTarget0));

                    if (a_rRenderlet.m_iFrame == 1)
                    {
                        EMP_RET_FALSE_IF_NOT(rDevice.clear(vClearColor));
                    }

                    tx_d mCurrent;
                    tx_d::mul(mTransform, a_rRenderlet.m_m44Transform, mCurrent);
                    rDevice.set_blend(true);
                    rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                    rDevice.set_program(rDevice.ose_program());
                    rDevice.set_uniform(uniform_e::m44Transform, mCurrent);
                    for (size_t stMesh = 0; stMesh < stMeshCount; ++stMesh)
                    {
                        auto& rMesh = *a_rRenderlet.m_pvpImMeshes->val(stMesh);
                        auto& rVertices = std::get<1>(rMesh);
                        if (rVertices.count() > 0)
                        {
                            auto& rIndices = std::get<0>(rMesh);
                            if (rIndices.count() > 0)
                            {
                                rDevice.set_vertex_buffer(&rVertices);
                                rDevice.set_index_buffer(&rIndices);
                                rDevice.render(emp::gfx::primitive_e::Triangles);
                            }
                        }
                    }
                    a_rRenderlet.m_pvpImMeshes->resize(0);


                    // compositing
                    /*EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(rTarget1));
                    EMP_RET_FALSE_IF_NOT(rDevice.clear(vClearColor));
                    rDevice.set_blend(true);
                    rDevice.glBlendFunc_(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    rDevice.set_vertex_buffer(&*m_pImQuadVertices);
                    rDevice.set_index_buffer(rDevice.quad_index_buffer());
                    rDevice.set_program(rDevice.screen_program());
                    rDevice.set_uniform(uniform_e::m44Transform, mTransform);
                    rDevice.set_uniform(uniform_e::t2Texture_0, 0, rTarget0.m_ui32Texture);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    rDevice.render(emp::gfx::primitive_e::Triangles);*/



                    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.glBindFramebuffer__(GL_DRAW_FRAMEBUFFER, m_ResolveTarget.m_ui32Framebuffer));
                    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.glBindFramebuffer__(GL_READ_FRAMEBUFFER, rTarget0.m_ui32Framebuffer));
                    EMP_RET_FALSE_IF_NOT(rDevice.clear(vClearColor));
                    //EMP_GRAPHICS.glDrawBuffer_(GL_BACK);
                    EMP_GRAPHICS.glBlitFramebuffer_(0, 0,  rTarget0.m_siWidth,  rTarget0.m_siHeight, 0, 0, m_ResolveTarget.m_siWidth, m_ResolveTarget.m_siHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
                }
                else
                {
                    c_size stCurrentTarget = a_rRenderlet.m_stCurrentTarget;
                    a_rRenderlet.m_stCurrentTarget = (a_rRenderlet.m_stCurrentTarget + 1) % (a_rRenderlet.m_aTargets.count());

                    // previous texture
                    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(a_rRenderlet.m_aTargets.ref(stCurrentTarget)));
                    EMP_RET_FALSE_IF_NOT(rDevice.clear(v4f::zero()));
                    tx_d mPrevious;
                    tx_d::mul(mTransform, a_rRenderlet.m_m44PreviousTransform, mPrevious);
                    rDevice.set_blend(true);
                    rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
                    rDevice.set_vertex_buffer(&*m_pImQuadVertices);
                    rDevice.set_index_buffer(rDevice.quad_index_buffer());
                    rDevice.set_program(rDevice.screen_program());
                    rDevice.set_uniform(uniform_e::m44Transform, mPrevious);
                    rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rRenderlet.m_aTargets.ref(stPrevioustTexture).m_ui32Texture);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    rDevice.render(emp::gfx::primitive_e::Triangles);


                    // current meshes
                    tx_d mCurrent;
                    tx_d::mul(mTransform, a_rRenderlet.m_m44Transform, mCurrent);
                    rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                    rDevice.set_program(rDevice.ose_program());
                    rDevice.set_uniform(uniform_e::m44Transform, mCurrent);
                    for (size_t stMesh = 0; stMesh < stMeshCount; ++stMesh)
                    {
                        auto& rMesh = *a_rRenderlet.m_pvpImMeshes->val(stMesh);
                        auto& rVertices = std::get<1>(rMesh);
                        if (rVertices.count() > 0)
                        {
                            auto& rIndices = std::get<0>(rMesh);
                            if (rIndices.count() > 0)
                            {
                                rDevice.set_vertex_buffer(&rVertices);
                                rDevice.set_index_buffer(&rIndices);
                                rDevice.render(emp::gfx::primitive_e::Triangles);
                            }
                        }
                    }
                    a_rRenderlet.m_pvpImMeshes->resize(0);

                    // compositing
                    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
                    EMP_RET_FALSE_IF_NOT(rDevice.clear(vClearColor));
                    rDevice.set_blend(true);
                    rDevice.glBlendFunc_(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    rDevice.set_vertex_buffer(&*m_pImQuadVertices);
                    rDevice.set_index_buffer(rDevice.quad_index_buffer());
                    rDevice.set_program(rDevice.screen_program());
                    rDevice.set_uniform(uniform_e::m44Transform, mTransform);
                    rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rRenderlet.m_aTargets.ref(stCurrentTarget).m_ui32Texture);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    rDevice.render(emp::gfx::primitive_e::Triangles);
                }
            }
        }

        if (a_rRenderlet.m_bCompleted && stMeshCount <= 0)
        {
            if (a_rRenderlet.m_iRendered == 0)
            {
                EMP_LOG_INFO("RENDERING COMPLETE #0", a_rRenderlet.m_sMother);
                a_rRenderlet.m_fPreviousX = a_rRenderlet.m_fX;
                a_rRenderlet.m_fPreviousY = a_rRenderlet.m_fY;
                a_rRenderlet.m_fPreviousScaleX = a_rRenderlet.m_fScaleX;
                a_rRenderlet.m_fPreviousScaleY = a_rRenderlet.m_fScaleY;
            }
            a_rRenderlet.m_iRendered++;
        }
        return true;
    };

    auto const& l_load_renderlet = [this, l_reset_renderlet](renderlet_c& a_rRenderlet, c_astring& a_sMother, c_astring& a_sFather, c_bool a_bMutate)
    {
        m_RefreshSourceTime = emp::tim::time_c();

        bool bFromCache = false;
        l_reset_renderlet(a_rRenderlet, a_sMother, a_sFather);

        emp::ose::program_c* pFather = nullptr;
        auto oFinally = emp::tt::finally([&pFather]() { if (pFather != nullptr) { EMP_DEALLOCATE(pFather); pFather = nullptr; } });
        if (a_sFather.len() > 0)
        {
            pFather = m_pCompiler->from_file(STR_AFORMAT("#0/#1", m_sPath, a_sFather), a_bMutate, bFromCache, nullptr, a_rRenderlet.m_RandomFloat);
            if (pFather == nullptr)
            {
                //EMP_LOG_ERROR("Failed to build father '#0'", a_sFather);
               // m_mInvalidPrograms.set(a_sFather, true);
            }
        }

        emp::ose::program_c* pMother = m_pCompiler->from_file(STR_AFORMAT("#0/#1", m_sPath, a_sMother), a_bMutate, bFromCache, pFather, a_rRenderlet.m_RandomFloat);
        if (pMother == nullptr)
        {
            EMP_LOG_INFO("Failed to build mother '#0'", a_sMother);
            //m_mInvalidPrograms.set(a_sMother, true);
        }
        else
        {
            a_rRenderlet.m_pStack->copy(pMother);
            EMP_LOG_INFO("starting : '#0' x '#1'", a_sMother, a_sFather);
        }

        //if (a_rRenderlet.m_pProgram != nullptr)
        //{ EMP_DEALLOCATE(a_rRenderlet.m_pProgram); }

        a_rRenderlet.m_pProgram = pMother;
        a_rRenderlet.m_bDealloc = true;

        if (a_rRenderlet.m_pProgram != nullptr && a_rRenderlet.m_pProgram->is_valid_table())
        { a_rRenderlet.m_Options.load(*m_pCompiler, *a_rRenderlet.m_pProgram); }

        auto const& rOptions = a_rRenderlet.m_Options;
        c_float scaleX = rOptions.scaleX <= 0.0f ? 1.0f : rOptions.scaleX;
        c_float scaleY = rOptions.scaleY <= 0.0f ? 1.0f : rOptions.scaleY;
        a_rRenderlet.m_pImStack->scale(scaleX, scaleY, 1.0f);
        a_rRenderlet.m_pImStack->translate(rOptions.originX, rOptions.originY, 0.0);
        a_rRenderlet.m_pImStack->set_min_size(rOptions.minSize * scaleX);
        a_rRenderlet.m_pImStack->set_min_alpha(rOptions.minAlpha);
        a_rRenderlet.m_RandomFloat.reset_seed(rOptions.seed);
    };

    auto const& l_get_png_name = [this](c_astring& a_sMother, c_ui32 a_uiLevel, c_bool a_bRetIfExists) -> astring
    {
        astring sPngPath;
        astring sPngFilename;
        astring sPngExtension;
        emp::str::extract_PFE(a_sMother, sPngPath, sPngFilename, sPngExtension);

        astring sPath;
        size_t stIndex = 0;

        while (true)
        {
            sPath.clear();
            emp::tt::silent(STR_FORMATA(sPath, "#0/#1_s#2_v#3.png", m_sPath, sPngFilename, a_uiLevel, stIndex));
            bool bExists = false;
            if (emp::fs::path_exists(sPath, bExists) == false)
            { sPath = astring(); }
            if (bExists == false)
            { break; }
            else if (a_bRetIfExists)
            {
                sPath = astring();
                break;
            }
            ++stIndex;
        }

        return sPath;
    };

    auto const& l_save_png = [this, l_get_png_name](renderlet_c& a_rRenderlet) -> bool
    {
        auto& rTarget = m_ResolveTarget;
        if (rTarget.m_siWidth > 0 && rTarget.m_siHeight > 0)
        {
            c_ui32 ui32Width = emp::ast::cast<ui32>(rTarget.m_siWidth);
            //c_ui32 ui32Height = emp::ast::cast<ui32>(rTarget.m_siHeight);

            ui32 uiLevel = ui32Width;

            while (uiLevel > 128)
            {
                {
                    astring sPath = l_get_png_name(a_rRenderlet.m_sMother, uiLevel, false);
                    if (sPath.len() > 0)
                    {
                        EMP_LOG_INFO("Saving #0", sPath);
                        EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.read_pixels(rTarget, m_vui32Pixels, false));
                        if (emp::png::save(sPath, m_vui32Pixels, uiLevel, uiLevel, false) == false) // @@0 save in devdir
                        {
                            EMP_LOG_ERROR("Failed to capture screenshot");
                            return false;
                        }
                    }
                }


/*          c_size stPixelCount = emp::ast::cast<size_t>(rTarget.m_siWidth * rTarget.m_siHeight);
            m_vui32Pixels.resize(stPixelCount);

            EMP_GRAPHICS.glBindTexture_(GL_TEXTURE_2D, rTarget.m_ui32Texture);
            EMP_GRAPHICS.glGetTexImage_(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, m_vui32Pixels.first_ptr());
*/

               /* {
                    uiLevel = emp::ast::cast<ui32>(m_PngTarget.m_siWidth);
                    astring sPath = l_get_png_name(a_rRenderlet.m_sMother, uiLevel, false);
                    if (sPath.len() > 0)
                    {
                        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.bind_framebuffer(m_PngTarget));

                        EMP_GRAPHICS.set_blend(false);
                        EMP_GRAPHICS.glBlendFunc_(GL_ONE, GL_ZERO);
                        EMP_GRAPHICS.set_vertex_buffer(&*m_pImQuadVertices);
                        EMP_GRAPHICS.set_index_buffer(EMP_GRAPHICS.quad_index_buffer());
                        EMP_GRAPHICS.set_program(EMP_GRAPHICS.screen_program());
                        EMP_GRAPHICS.set_uniform(uniform_e::m44Transform, transform().matrix());
                        EMP_GRAPHICS.set_uniform(uniform_e::t2Texture_0, 0, m_Rendered.m_aTargets.last_ref().m_ui32Texture);
                        EMP_GRAPHICS.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        EMP_GRAPHICS.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        EMP_GRAPHICS.render(emp::gfx::primitive_e::Triangles);

                        EMP_LOG_INFO("Saving #0", sPath);
                        EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.read_pixels(m_PngTarget, m_vui32Pixels));
                        if (emp::png::save(sPath, m_vui32Pixels, uiLevel, uiLevel, false) == false) // @@0 save in devdir
                        {
                            EMP_LOG_ERROR("Failed to capture screenshot");
                            return false;
                        }
                    }
                }*/

                uiLevel /= 2;
                break;
            }
        }



        return true;
    };

    auto const& l_is_beauty = [this, &rDevice](renderlet_c& a_rRenderlet, best_s& a_rBest) EMP_RETURN -> bool
    {
        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.read_pixels(a_rRenderlet.m_aTargets.last_ref(), m_vui32Pixels, false));
        m_muiPixels.clear();

        c_size stPixelCount = m_vui32Pixels.count();

        float fAlpha = 0;
        float EMP_TT_MAX_VAR(fMinAlpha);
        float EMP_TT_MIN_VAR(fMaxAlpha);
        for (size_t stPixel = 0; stPixel < stPixelCount; ++stPixel)
        {
            c_ui32 ui32Pixel = m_vui32Pixels.val(stPixel);
            /*c_float r = static_cast<float>((ui32Pixel >> 0) & 0xFF);
            c_float g = static_cast<float>((ui32Pixel >> 8) & 0xFF);
            c_float b = static_cast<float>((ui32Pixel >> 16) & 0xFF);*/
            c_float a = static_cast<float>((ui32Pixel >> 24) & 0xFF) / 255.0f;
            fAlpha += a;
            fMinAlpha = emp::tt::minof(a, fMinAlpha);
            fMaxAlpha = emp::tt::maxof(a, fMaxAlpha);

            c_size stCount = m_muiPixels.has(ui32Pixel) ? *m_muiPixels.get(ui32Pixel).value() : 1u;
            m_muiPixels.set(ui32Pixel, stCount);
        }

        fAlpha /= emp::ast::cast<float>(stPixelCount);

        a_rBest.m_pProgram = a_rRenderlet.m_pProgram;
        a_rBest.m_sMother = a_rRenderlet.m_sMother;
        a_rBest.m_sFather = a_rRenderlet.m_sFather;
        a_rBest.m_fAlpha = fAlpha;
        a_rBest.m_stCount = m_muiPixels.count();
        a_rBest.m_Stamp = a_rRenderlet.m_Stamp;
        a_rRenderlet.m_pProgram = nullptr;
        return true;
    };

    auto const l_write_renderlet = [this](renderlet_c& a_rRenderlet, c_astring const& a_sDirectory) -> bool
    {
        if (a_rRenderlet.m_pProgram != nullptr && // @@0 some bugs here, program is nullptr when it should'nt
            a_rRenderlet.m_pProgram->ast() != nullptr)
        {
            astring sSource;
            EMP_RET_FALSE_IF_NOT(print_ose(*m_pCompiler, a_rRenderlet, sSource));

            astring sPath(STR_AFORMAT("#0/#1/#2", m_sPath, a_sDirectory, a_rRenderlet.m_sMother));
            EMP_LOG_INFO("SAVING FILE #0\n", sPath);
            emp::fs::file_c oFile;
            if (oFile.create(sPath, emp::fs::file_e::Text, emp::fs::create_e::Trunc))
            {
                EMP_RET_FALSE_IF_NOT(oFile.write(sSource));
                return true;
            }
        }

        return false;
    };

    auto const& l_move_renderlet = [&a_rstFileCount = m_stFileCount, &a_vsFiles = m_vsFiles, l_write_renderlet, &a_pCompiler = m_pCompiler, &a_sPath = m_sPath, l_get_png_name, l_save_png](renderlet_c& a_rRenderlet, c_astring& a_sDirectory)
    {
        bool bSaved = false;
        astring sOldPath(STR_AFORMAT("#0/#1", a_sPath, a_rRenderlet.m_sMother));
        astring sNewPath(STR_AFORMAT("#0/#1/#2", a_sPath, a_sDirectory, a_rRenderlet.m_sMother));

        if (a_rRenderlet.m_pProgram != nullptr &&
            a_rRenderlet.m_pProgram->ast() != nullptr)
        {
            if (l_write_renderlet(a_rRenderlet, a_sDirectory))
            {
                if (emp::fs::remove(sOldPath) == false)
                {
                    EMP_LOG_INFO("FAILED TO REMOVE FILE #0\n", sOldPath);
                }
                else
                {
                    EMP_LOG_INFO("REMOVED FILE #0\n", sOldPath);
                    bSaved = true;
                }
            }
        }
        else
        {
            if (emp::fs::rename(sOldPath, sNewPath) == false)
            {
                EMP_LOG_INFO("FAILED TO MOVE FILE #0 -> #1\n", sOldPath, sNewPath);
            }
            else
            {
                EMP_LOG_INFO("MOVED FILE #0\n", sOldPath);
                bSaved = true;
            }
        }

        if (bSaved && (a_rRenderlet.m_pProgram == nullptr || l_save_png(a_rRenderlet)))
        {
            if (a_rstFileCount > 0)
            {
                --a_rstFileCount;
            }
            astring sPatternPath;
            astring sPatternFilename;
            astring sPatternExtension;
            emp::str::extract_PFE(a_rRenderlet.m_sMother, sPatternPath, sPatternFilename, sPatternExtension);

            emp::cnt::vector<astring> vPngToRename;
            astring sPattern = STR_AFORMAT("#0_s*.png", sPatternFilename);
            //EMP_LOG_INFO("PATTERN #0", sPattern);
            if (emp::fs::list(a_sPath, sPattern, vPngToRename, false))
            {
                for (size_t stIndex = 0; stIndex < vPngToRename.count(); ++stIndex)
                {
                    astring sOldPng = STR_AFORMAT("#0/#1", a_sPath, vPngToRename.ref(stIndex));
                    astring sNewPng = STR_AFORMAT("#0/#1/#2", a_sPath, a_sDirectory, vPngToRename.ref(stIndex));
                    if (emp::fs::rename(sOldPng, sNewPng) == false)
                    {
                        EMP_LOG_INFO("FAILED TO RENAME PNG #0 -> #1\n", sOldPng, sNewPng);
                    }
                    else
                    {
                        EMP_LOG_INFO("RENAMED PNG #0 -> #1\n", sOldPng, sNewPng);
                    }
                }
            }
        }

        EMP_LOG_INFO("--------------------->>>>> LEFT : #0/#1\n", a_rstFileCount, a_vsFiles.count());
    };
    /*auto const& l_sort_bests = [](vector<best_s>* const a_pvFrom, vector<best_s>* const a_pvSorted, c_bool a_bTime)
    {
        a_pvSorted->resize(0);
        for (size_t stFrom = 0; stFrom < a_pvFrom->count(); ++stFrom)
        {
            best_s& rFrom = a_pvFrom->ref(stFrom);
            bool bAdded = false;
            for (size_t stSort = 0; stSort < a_pvSorted->count(); ++stSort)
            {
                best_s& rSort = a_pvSorted->ref(stSort);
                if (a_bTime) // @@0 outside loop
                {
                    if (emp::tim::time_c::less(rFrom.m_Stamp, rSort.m_Stamp))
                    {
                        a_pvSorted->insert(stSort, rFrom);
                        bAdded = true;
                        break;
                    }
                }
                else
                {
                    if (rFrom.m_stCount < rSort.m_stCount)
                    {
                        a_pvSorted->insert(stSort, rFrom);
                        bAdded = true;
                        break;
                    }
                }
            }
            if (bAdded == false)
            {
                a_pvSorted->add(rFrom);
            }
        }
    };*/

    /*auto const& l_get_bests = [this](vector<best_s>* const a_pvpBests, auto const& a_lIsBeauty)
    {
        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            auto& rSimulated = m_aSimulated.ref(stIndex);
            if ((rSimulated.m_bCompleted) && rSimulated.m_pProgram != nullptr)
            {
                a_lIsBeauty(rSimulated, a_pvpBests->push(), true);
                rSimulated.m_pProgram = nullptr;
                //a_lLoadRenderlet(rSimulated, m_sMother, m_sFather, m_bMutate, a_lResetRenderlet);
            }
        }
    };*/

    if (m_eMod == renderer_e::Generate)
    {
        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            auto& rRenderlet = m_aSimulated.ref(stIndex);
            if (rRenderlet.m_pProgram != nullptr)
            {
                l_update_renderlet(rRenderlet/*, l_change_dna*/);
            }
        }
    }


    auto* const pToAdd = m_avBests.ptr(0);
    //auto* const pToSort = m_avBests.ptr(1);

        //l_get_bests(pToAdd, true, l_is_beauty/*, l_load_renderlet, l_reset_renderlet*/);
        //if ((pToAdd->count() > 0) == false)
        //{ l_get_bests(pToAdd, false, l_is_beauty/*, l_load_renderlet, l_reset_renderlet*/); }

    if (m_eMod == renderer_e::Generate)
    {
        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            auto& rSimulated = m_aSimulated.ref(stIndex);
            if ((rSimulated.m_bCompleted/* || a_bCompleted == false*/) && rSimulated.m_pProgram != nullptr)
            {
                EMP_ASSERT_BOOL_CALL(l_is_beauty(rSimulated, pToAdd->push()));
                rSimulated.m_pProgram = nullptr;
            }

            if ((rSimulated.m_pProgram == nullptr) && (pToAdd->count() < 32))
            {
                EMP_ASSERT_BOOL_CALL(l_change_dna(rSimulated.m_sMother, 0, false));
                EMP_ASSERT_BOOL_CALL(l_change_dna(rSimulated.m_sFather, 0, false));
                l_load_renderlet(rSimulated, rSimulated.m_sMother, rSimulated.m_sFather, true);
            }
        }
    }



    int iSetNext = 0;
    bool bSetCurate = false;

    if (m_iNextFile != 0)
    {
        m_fTransitionTime = 0.0f;
    }


    if (m_eMod == renderer_e::Edition)
    {
        if (m_iNextFile != 0)
        {
            //l_write_renderlet(m_Rendered, astring(ALITERAL("")));
            if (m_Rendered.m_pProgram != nullptr)
            { EMP_DEALLOCATE(m_Rendered.m_pProgram); }
            if (false == l_change_dna(m_Rendered.m_sMother, m_iNextFile, false))
            { EMP_LOG_INFO("Failed to pick next file...."); }

            m_Rendered.m_sFather = astring();
            l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate);
            l_write_renderlet(m_Rendered, astring(ALITERAL("")));

            if (m_Rendered.m_pProgram != nullptr)
            { EMP_DEALLOCATE(m_Rendered.m_pProgram); }
            m_Rendered.m_sFather = astring();
            l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate);
            m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
        }
        else if (m_bReset)
        {
            auto& rOptions = m_Rendered.m_Options;
            if (m_bResetOptions)
            {
                if (m_bRandomSeed)
                {
                    rOptions.seed = m_RandomDevice();
                    rOptions.clearH = 300;
                    rOptions.clearS = 1;
                    rOptions.clearB = 1;
                    rOptions.clearA = 1;
                }
                rOptions.scaleX = 0.0f;
                rOptions.scaleY = 0.0f;
                rOptions.originX = 0.0f;
                rOptions.originY = 0.0f;
                m_bFirstBake = false;
            }

            if (m_bBakeOptions)
            {

                if (rOptions.scaleX <= 0 || rOptions.scaleY <= 0 || m_bFirstBake == false)
                {
                    rOptions.originX = m_Rendered.m_fX;
                    rOptions.originY = m_Rendered.m_fY;
                    rOptions.scaleX = m_Rendered.m_fScaleX;
                    rOptions.scaleY = m_Rendered.m_fScaleY;
                    m_bFirstBake = true;
                }

                c_float scaleX = rOptions.scaleX <= 0.0f ? 1.0f : rOptions.scaleX;
                c_float scaleY = rOptions.scaleY <= 0.0f ? 1.0f : rOptions.scaleY;

                EMP_LOG_INFO("ORIGIN #0, #1, #2", m_Rendered.m_origin.x, m_Rendered.m_delta.y, rOptions.originX);
                rOptions.originX += (m_Rendered.m_origin.x + m_Rendered.m_delta.x) * (2.0f / scaleX);
                rOptions.originY += (m_Rendered.m_origin.y + m_Rendered.m_delta.y) * (2.0f / scaleY);
                rOptions.scaleX *= m_Rendered.m_fSizeX;
                rOptions.scaleY *= m_Rendered.m_fSizeY;

                m_Rendered.m_origin = v2f::zero();//m_v2DragPosition;
                m_Rendered.m_delta = v2f::zero();
                m_Rendered.m_fSizeX = 1.0f;
                m_Rendered.m_fSizeY = 1.0f;
            }

            if (m_bWriteOptions || m_bResetOptions || m_bBakeOptions)
            {
                l_write_renderlet(m_Rendered, astring(ALITERAL("")));
            }


            if (m_bNoReload == false)
            {
                if (m_Rendered.m_pProgram != nullptr)
                { EMP_DEALLOCATE(m_Rendered.m_pProgram); }
                m_Rendered.m_sFather = astring();
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate);
                m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
            }
        }
        else if (m_bMove)
        {
            l_move_renderlet(m_Rendered, m_sMoveFolder);
            iSetNext = 1;
        }
    }
    else if (m_eMod == renderer_e::Thumbail)
    {
        if (m_iNextFile != 0)
        {
            if (m_Rendered.m_pProgram != nullptr)
            { EMP_DEALLOCATE(m_Rendered.m_pProgram); }

            //l_reset_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, true);
            while (true)
            {
                if (false == l_change_dna(m_Rendered.m_sMother, m_iNextFile, true))
                {
                    EMP_LOG_INFO("Failed to pick next file....");
                    return false;
                }

                astring sPngName = l_get_png_name(m_Rendered.m_sMother, 2048, true);
                if (sPngName.len() > 0)
                { break; }
                EMP_LOG_INFO("Skipping thumbnail generation #0", m_Rendered.m_sMother);
            }

            m_Rendered.m_sFather = astring();
            l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate);
        }
        m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
    }
    else
    {
        if (m_iNextFile != 0)
        {
            if (m_bSwitch)
            {
                //l_sort_bests(pToAdd, pToSort, true);
                //l_sort_bests(pToSort, pToAdd, false);

                if (m_Rendered.m_pProgram != nullptr)
                { EMP_DEALLOCATE(m_Rendered.m_pProgram); }

                if (pToAdd->count() > 0)
                {
                    best_s& rBest = pToAdd->last_ref();
                    l_reset_renderlet(m_Rendered, rBest.m_sMother, rBest.m_sFather);
                    m_Rendered.m_pProgram = rBest.m_pProgram;
                    rBest = best_s();
                    pToAdd->remove_last();
                }
                else
                {
                    EMP_ASSERT_BOOL_CALL(l_change_dna(m_Rendered.m_sMother, 0, false));
                    EMP_ASSERT_BOOL_CALL(l_change_dna(m_Rendered.m_sFather, 0, false));
                    l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate);
                }

                size_t stIndex = 0;
                while (stIndex < pToAdd->count())
                {
                    best_s& rBest = pToAdd->ref(stIndex);
                    if (rBest.m_stCount <= 1)
                    {
                        if (rBest.m_pProgram != nullptr)
                        { EMP_DEALLOCATE(rBest.m_pProgram); }
                        rBest = best_s();
                        pToAdd->remove(stIndex);
                    }
                    else
                    {
                        ++stIndex;
                    }
                }
            }
            else
            {
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate);
            }
            m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
        }
    }



    if (m_Rendered.m_pProgram != nullptr)
    { l_update_renderlet(m_Rendered); }

    if (m_bLookup)
    {
        best_s oBest;
        EMP_ASSERT_BOOL_CALL(l_is_beauty(m_Rendered, oBest));
        m_bLookup = false;
    }

    m_Transform.push(a_rStack, m_State.enable());
    //m_fSize = 1;

    if (m_eMod == renderer_e::Generate)
    {
        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            if (m_aSimulated.ref(stIndex).m_pProgram != nullptr)
            {
                l_transform_renderlet(m_aSimulated.ref(stIndex));
            }
        }
    }

    if (m_Rendered.m_pProgram != nullptr)
    {
        l_transform_renderlet(m_Rendered);
    }

    m_Transform.pop(a_rStack);

    //m_Stencil.update(m_Transform.size());

    if (m_eMod == renderer_e::Generate)
    {
        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            if (m_aSimulated.ref(stIndex).m_pProgram != nullptr)
            {
                l_render_renderlet(m_aSimulated.ref(stIndex));
            }
        }
    }

    if (m_Rendered.m_pProgram != nullptr)
    {
        l_render_renderlet(m_Rendered);
    }

    /*if (m_fTransitionTime <= m_fTransitionDuration)
    {
        {
            m_stCurrentTarget = (m_stCurrentTarget + 1) % m_aTTargets.count();
            emp::gfx::target_s const& rTarget = m_aTTargets.ref(m_stCurrentTarget);
            EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(rTarget));
        }

        EMP_RET_FALSE_IF_NOT(rDevice.clear({1, 0, 0, 1 }));

        rDevice.set_blend(false);
        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
        rDevice.set_vertex_buffer(&*m_pImQuadVertices);
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(rDevice.transition_programs().val(0));//m_stNextTransition));
        rDevice.set_uniform(uniform_e::m44Transform, transform().matrix());
        rDevice.set_uniform(uniform_e::t2Texture_0, 0, m_aTTargets.ref((m_stCurrentTarget + 1) % m_aTTargets.count()).m_ui32Texture);
        rDevice.set_uniform(uniform_e::t2Texture_1, 1, m_Rendered.m_aTargets.last_ref().m_ui32Texture);
        rDevice.set_uniform(uniform_e::fProgress, (m_fTransitionTime / m_fTransitionDuration));
        rDevice.render(emp::gfx::primitive_e::Triangles);

        if (m_fTransitionTime >= m_fTransitionDuration)
        {
            EMP_TT_MAX_VAR(m_fTransitionTime);
            m_stNextTransition = (m_stNextTransition + 1) % rDevice.transition_programs().count();
            if (m_Rendered.m_bCompleted)
            {
                l_reset_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather);
            }
        }
        else
        {
            m_fTransitionTime = emp::tt::minof(m_fTransitionTime + m_fTransitionDelta, m_fTransitionDuration);
        }
    }
    else */if (m_Rendered.m_pProgram != nullptr)
    {
        /*emp::gfx::target_s const& rTarget = m_ResolveTarget;
        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(rTarget));

        rDevice.set_blend(false);
        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
        rDevice.set_vertex_buffer(&*m_pImQuadVertices);
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(rDevice.screen_program());
        rDevice.set_uniform(uniform_e::m44Transform, transform().matrix());
        rDevice.set_uniform(uniform_e::t2Texture_0, 0, m_Rendered.m_aTargets.last_ref().m_ui32Texture);
        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        rDevice.render(emp::gfx::primitive_e::Triangles);*/
        if ((m_Rendered.m_bCompleted && m_eMod == renderer_e::Thumbail) || m_bPng)
        {

            m_bPng = false;
            EMP_RET_FALSE_IF_NOT(l_save_png(m_Rendered));

            if (m_eMod == renderer_e::Thumbail)
            {
                iSetNext = 1;
                bSetCurate = true;
            }
        }
    }
    else if (m_eMod == renderer_e::Thumbail)
    {
        iSetNext = 1;
        bSetCurate = true;
    }

    //for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
    //{
    //    renderlet_c& rRenderlet = m_aSimulated.ref(stIndex);
    //    if (rRenderlet.m_bCompleted || rRenderlet.m_pProgram == nullptr)
    //    {
    //        l_change_dna(rRenderlet.m_sMother);
    //        l_change_dna(rRenderlet.m_sFather);
    //        l_load_renderlet(rRenderlet, rRenderlet.m_sMother, rRenderlet.m_sFather, m_bMutate, l_reset_renderlet);
    //    }
    //}

    m_iNextFile = iSetNext;
    m_bReset = false;
    m_bResetOptions = false;
    m_bWriteOptions = false;
    m_bBakeOptions = false;
    m_stImMesh = 0;
    m_bMove = false;
    m_bNoReload = false;
    m_bRandomSeed = false;
    m_sMoveFolder = astring();


    m_bSwitch = false;
    m_bMutate = false;


    EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());
    return true;
}

////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
void renderer_c::render(void) const
{
    if (m_State.visible() == false)
    { return; }

    auto& rDevice = EMP_GRAPHICS;
    rDevice.set_blend(false);
    rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
    rDevice.set_vertex_buffer(&*m_pImQuadVertices);
    rDevice.set_index_buffer(rDevice.quad_index_buffer());
    rDevice.set_program(rDevice.screen_program());
    rDevice.set_uniform(uniform_e::m44Transform, transform().matrix());
    rDevice.set_uniform(uniform_e::t2Texture_0, 0, m_ResolveTarget.m_ui32Texture);
    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    rDevice.render(emp::gfx::primitive_e::Triangles);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e renderer_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (m_State.visible() == false || m_State.enable() == false)
    { return return_e::Unused; }

    auto const& rKeyboard = a_rEvent.m_Keyboard;
    c_bool bPressed = rKeyboard.m_bPressed;
    emp::app::c_key_e eKey = rKeyboard.m_eKey;


    if (m_eMod == renderer_e::Edition)
    {
        if (rKeyboard.modified() == true && bPressed && rKeyboard.m_bRepeat == false)
        {
            if (rKeyboard.ctrl_only() && eKey == emp::app::key_e::Alpha_R)
            {
                m_bReset = true;
                m_bResetOptions = true;
                m_bWriteOptions = true;
                m_bRandomSeed = false;
                return return_e::Consumed;
            }
            else if (rKeyboard.ctrl_only() && eKey == emp::app::key_e::SemiColon)
            {
                m_bReset = true;
                m_bResetOptions = true;
                m_bWriteOptions = true;
                m_bRandomSeed = true;
                return return_e::Consumed;
            }
        }
        else if (rKeyboard.modified() == false && bPressed && rKeyboard.m_bRepeat == false)
        {
            if (eKey == emp::app::key_e::Left)
            {
                m_iNextFile = -1;
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Right)
            {
                m_iNextFile = 1;
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Alpha_E)
            {
                m_bReset = true;
                m_bWriteOptions = true;
                //m_bNoReload = true;
                auto& rOptions = m_Rendered.m_Options;
                rOptions.clearH = m_RandomFloat.value() * 360.0f;
                rOptions.clearS = m_RandomFloat.value();
                rOptions.clearB = m_RandomFloat.value();
                rOptions.clearA = 1.0;//m_Rendered.m_RandomFloat();

                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Alpha_R)
            {
                m_bReset = true;
                m_bBakeOptions = true;
                m_bWriteOptions = true;
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Alpha_P)
            {
                m_bPng = true;
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_0)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("0"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_1)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("1"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_2)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("2"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_3)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("3"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_4)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("4"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_5)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("5"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_6)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("6"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_7)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("7"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_8)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("8"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Num_9)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("9"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Alpha_S)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("special"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Alpha_C)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("copy"));
                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Backspace)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("trash"));
                return return_e::Consumed;
            }
        }
    }
    else if (m_eMod == renderer_e::Thumbail)
    {
    }
    else
    {
        if (rKeyboard.modified() == false && bPressed)
        {
            if (eKey == emp::app::key_e::Enter)
            {
                if (m_Rendered.m_pProgram != nullptr && // @@0 some bugs here, program is nullptr when it should'nt
                    m_Rendered.m_pProgram->ast() != nullptr)
                {
                    astring sSource;
                    EMP_ASSERT_BOOL_CALL(print_ose(*m_pCompiler, m_Rendered, sSource));

                    /*size_t stIndex = 0;
                    astring sPath;
                    astring sFilename;
                    while (true)
                    {
                        bool bExists = false;
                        sFilename = STR_AFORMAT("rgen_#0.good", stIndex);
                        sPath = STR_AFORMAT("#0/#1", m_sPath, sFilename);
                        if (emp::fs::path_exists(sPath, bExists))
                        {
                            if (bExists)
                            { ++stIndex; }
                            else
                            { break; }
                        }
                        else
                        {
                            EMP_TT_MAX_VAR(stIndex);
                            break;
                        }
                    }*/

                    //if (emp::tt::is_not_max(stIndex))
                    bool bSaved = false;
                    {
                        emp::fs::file_c oFile;


                        astring sPath(STR_AFORMAT("#0good/#1", m_sPath, m_Rendered.m_sMother));
                        EMP_LOG_INFO("SAVING FILE #0\n", sPath);
                        if (oFile.create(sPath, emp::fs::file_e::Text, emp::fs::create_e::New)) // @@0 save in devdir
                        {
                            EMP_ASSERT_BOOL_CALL(oFile.write(sSource));
                            bSaved = true;
                        }
                    }

                    if (bSaved)
                    {
                        astring sRemoveMother(STR_AFORMAT("#0#1", m_sPath, m_Rendered.m_sMother));
                        EMP_LOG_INFO("REMOVING FILE #0\n", sRemoveMother);
                        if (emp::fs::remove(sRemoveMother) == false)
                        {
                            EMP_LOG_ERROR("FAILED TO REMOVE FILE #0\n", sRemoveMother);
                        }
                    }
                }

                m_iNextFile = 1;

                return return_e::Consumed;
            }
            else if (eKey == emp::app::key_e::Space)
            {
                if (m_Rendered.m_pProgram != nullptr && // @@0 some bugs here, program is nullptr when it should'nt
                    m_Rendered.m_pProgram->ast() != nullptr)
                {
                    astring sSource;
                    EMP_ASSERT_BOOL_CALL(print_ose(*m_pCompiler, m_Rendered, sSource));

                    astring sRemoveMother(STR_AFORMAT("#0#1", m_sPath, m_Rendered.m_sMother));
                    EMP_LOG_INFO("REMOVING FILE #0\n", sRemoveMother);
                    if (emp::fs::remove(sRemoveMother) == false)
                    {
                        EMP_LOG_ERROR("FAILED TO REMOVE FILE #0\n", sRemoveMother);
                    }
                }

                m_iNextFile = 1;

                return return_e::Consumed;
            }
            /*else if (eKey == emp::app::key_e::Alpha_S)
            {
                m_bSwitch = true;
                m_iNextFile = 1;
            }*/
            else if (eKey == emp::app::key_e::Alpha_D)
            {
                m_bMutate = true;
                m_bSwitch = true;
                m_iNextFile = 1;
            }
            else if (eKey == emp::app::key_e::Alpha_F)
            {
                m_bMutate = true;
                m_iNextFile = 1;
            }
            else if (eKey == emp::app::key_e::Alpha_G)
            {
                m_bLookup = true;
            }
        }
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e renderer_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    emp::gui::process_mouse_on_control(*this, a_rEvent);

    if (state().visible() == false)
    { return emp::gui::return_e::Unused; }

    auto const& rMouse = a_rEvent.m_Mouse;
    emp::app::c_button_e eButton = rMouse.m_eButton;

    c_bool bContains = state().hover();
    c_bool bPressed = rMouse.m_bPressed;

    c_int siCount = rMouse.m_siCount;

    c_v2f v2RelPosition = rMouse.m_v2fPosition;

    m_bLeftPressed = false;
    m_bLeftReleased = false;

    if (eButton == emp::app::button_e::Left)
    {
        if (siCount == 1)
        {
            if (bContains)
            {
                if (bPressed)
                {
                    if (m_bDragLeft == false)
                    {
                        m_bDragLeft = true;
                        m_v2DragPosition = v2RelPosition;
                        m_v2DragOrigin = v2RelPosition;
                        m_v2DragDelta = emp::mat::v2f::zero();
                        m_bLeftPressed = true;
                    }

                    return emp::gui::return_e::Consumed;
                }
                else
                {
                    if (m_bDragLeft)
                    {
                        m_bDragLeft = false;

                        m_v2DragPosition = v2RelPosition;
                        m_v2DragOrigin = v2f::add(m_v2DragOrigin, m_v2DragDelta);
                        m_v2DragDelta = emp::mat::v2f::zero();

                        //m_Rendered.m_origin = v2f::add(m_Rendered.m_origin, m_Rendered.m_delta);
                        m_Rendered.m_delta = v2f::zero();

                        m_bLeftReleased = true;
                        return emp::gui::return_e::Consumed;
                    }
                }
            }
            else
            {
                if (m_bDragLeft)
                {
                    m_bDragLeft = false;
                    m_v2DragDelta = v2f::zero();
                    m_Rendered.m_delta = v2f::zero();
                    return emp::gui::return_e::Consumed;
                }
            }
        }
    }
    else if (eButton == emp::app::button_e::Right)
    {
        if (siCount == 1)
        {
            if (bContains)
            {
                if (bPressed)
                {
                    if (m_bDragRight == false)
                    {
                        m_bDragRight = true;
                        m_v2DragPosition = v2RelPosition;
                        m_v2DragOrigin = v2RelPosition;
                        m_v2DragDelta = v2f::zero();
                        m_bRightPressed = true;
                    }

                    return emp::gui::return_e::Consumed;
                }
                else
                {
                    if (m_bDragRight)
                    {
                        m_bDragRight = false;
                        m_v2DragPosition = v2RelPosition;
                        m_v2DragOrigin = emp::mat::v2f::add(m_v2DragOrigin, m_v2DragDelta);
                        m_v2DragDelta = v2f::zero();
                        m_bRightReleased = true;
                        return emp::gui::return_e::Consumed;
                    }
                }
            }
            else
            {
                if (m_bDragRight)
                {
                    m_bDragRight = false;
                    m_v2DragDelta = v2f::zero();
                    return emp::gui::return_e::Consumed;
                }
            }
        }
    }
    else if (eButton == emp::app::button_e::none)
    {
        if (m_bDragLeft ||
            m_bDragRight)
        {
            m_v2DragDelta = v2f::sub(v2RelPosition, m_v2DragOrigin);
            m_Rendered.m_delta = v2f::sub(v2RelPosition, m_v2DragPosition);
            m_v2DragPosition = v2RelPosition;
            m_bReset = true;
            m_bBakeOptions = true;
            return emp::gui::return_e::Consumed;
        }
    }
    else if (eButton == emp::app::button_e::WheelUp)
    {
        m_Rendered.m_fSizeX = m_Rendered.m_fSizeX + 0.01f;
        m_Rendered.m_fSizeY = m_Rendered.m_fSizeY + 0.01f;
        m_Rendered.m_scaleOrigin = v2RelPosition;
        m_bReset = true;
        m_bBakeOptions = true;
        return return_e::Consumed;
    }
    else if (eButton == emp::app::button_e::WheelDown)
    {
        m_Rendered.m_fSizeX = emp::tt::maxof<float>(m_Rendered.m_fSizeX - 0.01f, 0.01f);
        m_Rendered.m_fSizeY = emp::tt::maxof<float>(m_Rendered.m_fSizeY - 0.01f, 0.01f);
        m_bReset = true;
        m_bBakeOptions = true;
        return return_e::Consumed;
    }


    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif