//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_function.h"

#include "emp_gui_manager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

#include "emp_gfx_gl_device.h"
#include "emp_gfx_model.h"

#include "emp_mat_intersect.h"

#include "emp_mem_pointer.h"

#include "emp_snd_soft_synth.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::tt;
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
function_c::function_c(void):
    m_pStack(),
    m_pImLineVertices(),
    m_pImLineIndices(),
    m_vpVarX(),
    m_vpVarIndex()
{
    frame.on_render = emp::fct::function(this, &function_c::render_function);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
function_c::~function_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool function_c::transform(stack_c& a_rStack)
{
    using tool_d = emp::mat::simd_v4<precision_d>;
    using vector_d = typename tool_d::type;

    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    if (function == nullptr)
    { return true; }

    m_vpVarIndex.resize(0);
    m_vpVarX.resize(0);
    emp::tt::silent(emp::snd::get_unbound_variables(&m_vpVarX, function, astring()));

    c_astring sIndexVariable(ALITERAL("frequency"));
    c_astring sXVariable(ALITERAL("time"));
    c_size stVariableCount = m_vpVarX.count();
    for (size_t stIndex = 0; stIndex < stVariableCount; ++stIndex)
    {
        emp::mat::input_t<precision_d> const* const pInput = m_vpVarX.val(stIndex);
        if (emp::str::cmp(pInput->name(), sXVariable) == false &&
            emp::str::cmp(pInput->name(), sIndexVariable) == false)
        { return true; }
    }

    m_vpVarX.resize(0);
    emp::tt::silent(emp::snd::get_unbound_variables(&m_vpVarX, function, sXVariable));

    m_vpVarIndex.resize(0);
    emp::tt::silent(emp::snd::get_unbound_variables(&m_vpVarIndex, function, sIndexVariable));

    function->link();
    m_pStack->create(64, function->size() + function->result() * emp::mat::precision_c::array_stride);
    m_pStack->set_compute(false);

    c_v2f v2Min = frame.min_visible();
    c_v2f v2Max = frame.max_visible();

    c_float fPlainMinX = v2Min.x;
    c_float fPlainMaxX = v2Max.y;

    c_bool bLogX = frame.data.xlog;
    c_bool bLogY = frame.data.ylog;

    c_float fMinX = bLogX ? emp::mat::log10(fPlainMinX) : fPlainMinX;
    c_float fMaxX = bLogX ? emp::mat::log10(fPlainMaxX) : fPlainMaxX;

    c_float fSizeX = fMaxX - fMinX;

    c_size stTapCount = 100;

    v2f v2LastClient = v2f::zero();

    bool bValid = false;

    c_size stReturnCount = function->result();

    c_size stXVarCount = m_vpVarX.count();
    c_size stIndexVarCount = m_vpVarIndex.count();

    c_float fStep = fSizeX / emp::ast::cast<float>(stTapCount);

    c_size stDataStride = emp::mat::precision_c::data_stride;
    c_size stArrayStride = emp::mat::precision_c::array_stride;
    c_size stVectorStride = emp::mat::precision_c::vector_stride;

    for (size_t stIndex = 0; stIndex < stTapCount; stIndex+=stDataStride)
    {
        emp::cnt::array<precision_d, stDataStride, true> afInput;
        emp::cnt::array<precision_d, stDataStride, true> afOutput;

        for (size_t stStride = 0; stStride < stArrayStride; ++stStride)
        {
            c_size stOffset = stIndex + stStride * stVectorStride;
            vector_d const vPlainNextSample =
                tool_d::add(
                    tool_d::set(fMinX),
                    tool_d::mul(
                        tool_d::set(fStep),
                        tool_d::set(
                            emp::ast::cast<float>(stOffset + 0),
                            emp::ast::cast<float>(stOffset + 1),
                            emp::ast::cast<float>(stOffset + 2),
                            emp::ast::cast<float>(stOffset + 3))));

            vector_d const vNextSample =
                tool_d::bit_select(
                    tool_d::bool_mask(bLogX),
                    tool_d::pow(tool_d::set(10), vPlainNextSample),
                    vPlainNextSample);

            vector_d const vInput = vNextSample;
            afInput.set(stStride * stVectorStride + 0, tool_d::get_x(vInput));
            afInput.set(stStride * stVectorStride + 1, tool_d::get_y(vInput));
            afInput.set(stStride * stVectorStride + 2, tool_d::get_z(vInput));
            afInput.set(stStride * stVectorStride + 3, tool_d::get_w(vInput));

            for (size_t stVar = 0; stVar < stXVarCount; ++stVar)
            { m_vpVarX.val(stVar)->set_value(*cash, stStride, vNextSample);}

            for (size_t stVar = 0; stVar < stIndexVarCount; ++stVar)
            {
                m_vpVarIndex.val(stVar)->set_value(
                    *cash,stStride,tool_d::set(current_frequency, current_frequency, current_frequency, current_frequency));
            }
        }

        m_pImLineVertices->create(emp::gfx::usage_e::Dynamic);
        m_pImLineIndices->create(emp::gfx::usage_e::Dynamic);

        cash->clean();
        m_pStack->begin_frame(function->result());
        c_bool bReturn = function->evaluate(*m_pStack);

        c_float fDepth = depth(transformation().fore_depth, 0);

        if (bReturn == false || ((stReturnCount > 0) == false))
        { bValid = false; }
        else
        {
            for (size_t stStride = 0; stStride < stArrayStride;++stStride)
            {
                vector_d const vOutput = m_pStack->get_input(EMP_EVL_STRIDE(stStride, 0, stReturnCount));
                afOutput.set(stStride * stVectorStride +0, tool_d::get_x(vOutput));
                afOutput.set(stStride * stVectorStride +1, tool_d::get_y(vOutput));
                afOutput.set(stStride * stVectorStride +2, tool_d::get_z(vOutput));
                afOutput.set(stStride * stVectorStride +3, tool_d::get_w(vOutput));
            }
            for (size_t stOutput = 0; stOutput < stDataStride; ++stOutput)
            {
                c_float fInput = afInput.val(stOutput);
                c_float fX = bLogX ? emp::tt::maxof(0.00001f, fInput) : fInput;

                c_float fOutput = afOutput.val(stOutput);
                c_float fY = bLogY ? emp::tt::maxof(0.00001f, fOutput) : fOutput;

                c_v2f v2NextClient = frame.frame_to_client({ emp::ast::cast<float>(fX), -emp::ast::cast<float>(fY) });

                if (bValid)
                {
                    if (emp::mat::line_intersect_rectangle(v2LastClient, v2NextClient, v2f::zero(), frame.transformation().size) == false)
                    {
                        emp::gfx::emit_line(m_pImLineVertices->count(), *m_pImLineIndices);
                        m_pImLineVertices->add(v3f::all(v2LastClient, fDepth));
                        m_pImLineVertices->add(v3f::all(v2NextClient, fDepth));
                    }
                }

                v2LastClient = v2NextClient;
                bValid = true;
            }
        }

        if (bReturn)
        { m_pStack->end_frame(); }

        for (size_t stVar = 0; stVar < stXVarCount; ++stVar)
        { m_vpVarX.val(stVar)->reset(); }

        for (size_t stVar = 0; stVar < stIndexVarCount; ++stVar)
        { m_vpVarIndex.val(stVar)->reset(); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void function_c::auto_resize(void)
{
    /*c_v2f v2Size = v3f::xy(m_pGlyph->model()->model()->size());

    c_v2f v2Scale = v2f::make(m_Coordinate.scaled_unit_size_x(), m_Coordinate.scaled_unit_size_y());

    cpc_frame_skin_s pFrameSkin = m_pSkin->coordinate()->frame();
    c_v2f v2Unit = v2f::make(pFrameSkin->unit_x(), pFrameSkin->unit_y());
    c_v2f v2Ratio = v2f::div(v2Scale, v2Unit);

    m_Coordinate.auto_resize(v2f::add(v2f::mul(v2Size, v2Ratio), v2Scale));

    c_v2f v2HalfScale = v2f::mul(v2Scale, v2f::half());

    c_v2f v2ModelCenter = v3f::xy(m_pGlyph->model()->center());

    m_Coordinate.center_origin();
    m_Coordinate.set_origin(v2f::sub(m_Coordinate.origin(), v2f::mul(v2ModelCenter, v2Ratio)));*/
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void function_c::render_function(frame_c const& a_rSender)
{
    auto& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_test(true); // @@0 depth
    rDevice.set_depth_mask(true);

    rDevice.set_vertex_buffer(&*m_pImLineVertices);
    rDevice.set_index_buffer(&*m_pImLineIndices);
    rDevice.set_program(rDevice.color_uniform_program());
    rDevice.set_uniform(uniform_e::m44Transform, a_rSender.transformation().matrix);
    rDevice.set_uniform(uniform_e::v4Color, skin->line->rgba);
    rDevice.draw(emp::gfx::primitive_e::Lines);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
function_c& function_c::skin_(cpc_function_skin_s a_pValue)
{
    skin = a_pValue;
    tbase1::skin_(a_pValue ? a_pValue->coordinate : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

