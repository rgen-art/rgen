//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_rgen.h"

#include "ase_cor_game.h"
#include "ase_cor_selection.h"
#include "ase_cor_reflection.rfx.h"

#include "emp_ast_errno.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

#include "emp_fct_function.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_label.h"
#include "emp_gui_slider.h"
#include "emp_gui_window.h"

#include "emp_rfx_factory.h"

#include "emp_ose_processor.h"
#include "emp_ose_program.h"
#include "emp_ose_renderer.h"

#include "emp_fs_file.h"

#include "emp_mat_random.h"

#include "emp_png_util.h"


#include "emp_hh_stdio.h"
#if defined EMP_XX_OS_WIN
#include "emp_hh_windows.h"
#include <sys/stat.h>
#include <share.h>

#include <io.h>
#endif



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {
/*
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
    enum_value( translate_x  )\
    enum_value( translate_y  )\
    enum_value( scale_x      )\
    enum_value( scale_y      )\


*/

#define enum_name im_operation_e
#define enum_type
#define enum_values\
    enum_value( none         )\
    enum_value( square       )\
    enum_value( triangle     )\
    enum_value( circle       )\
    enum_value( hsba         )\
    enum_value( translate    )\
    enum_value( translate_xy )\
    enum_value( rotate       )\
    enum_value( scale        )\
    enum_value( scale_xy     )\
    enum_value( flip         )
#include "emp_tt_enum.def.h"

#define enum_name renderer_e
#define enum_type
#define enum_values\
    enum_value( Edition  )\
    enum_value( Thumbnail)\
    enum_value( Duplicate)\
    enum_value( Genome)\
    enum_value( Rename)\
    enum_value( FUCK)
#include "emp_tt_enum.def.h"

    // enum_value( Curate )\
    // enum_value( Generate )\
    // enum_value( Thumbail )\
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN gui::im_operation_e first_im_operation(void);
EMP_RETURN gui::im_operation_e first_im_operation(void)
{ return gui::im_operation_e::none; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v4f::tret to_rgb(c_float fH, c_float fS, c_float fV, c_float fA);
EMP_RETURN v4f::tret to_rgb(c_float fH, c_float fS, c_float fV, c_float fA)
{
    //if (fS <= 0.0)
    //{ return mat::v4f::make(fH, fS, fV, fA); }

    c_float hh = (fH >= 360.0f ? 0.0f : fH) / 60.0f;

    c_int i = ast::cast<int>(hh);

    c_float ff = hh - ast::cast<float>(i);

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
EMP_RETURN v4f::tret to_rgb(v4f::tparam a_v4Hsva);
EMP_RETURN v4f::tret to_rgb(v4f::tparam a_v4Hsva)
{ return to_rgb(a_v4Hsva.x, a_v4Hsva.y, a_v4Hsva.z, a_v4Hsva.w); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
EMP_NOCOPY(class, im_stack_c)
private:
    cnt::vector<v4f> m_vv4Colors;
//    cnt::vector<ui32> m_vuiColors;
    v4f m_v4CurrentColor;
    v4f m_v4RgbColor;

    stack_t<m44f> m_Matrices;
//    cnt::vector<ui32> m_vuiMatrices;
    m44f m_mCurrentMatrix;

    v2f m_v2Min;
    v2f m_v2Max;
    float m_fMinSize;
    float m_fMinAlpha;
    cnt::vector<v4f> m_v4Circle;

public:
    im_stack_c(void):
    m_vv4Colors(),
    //m_vuiColors(),
    m_v4CurrentColor(),
    m_v4RgbColor(),
    m_Matrices(),
    //m_vuiMatrices(),
    m_mCurrentMatrix(),
    EMP_TT_MAX_CTOR(m_v2Min),
    EMP_TT_MIN_CTOR(m_v2Max),
    m_fMinSize(1.0f),
    m_fMinAlpha(1.0f),
    m_v4Circle()
    {}

    void clear(void)
    {
        m_vv4Colors.resize(0);
        m_v4CurrentColor = { 0, 0, 0, 1 };
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
        m_vv4Colors.add(m_v4CurrentColor);

        m_Matrices.destroy();
        m44f::identity(m_mCurrentMatrix);
        m_Matrices.add(m_mCurrentMatrix);

        m_fMinSize = 1.0f;
        m_fMinAlpha = 1.0f;

        EMP_TT_MAX_VAR(m_v2Min);
        EMP_TT_MIN_VAR(m_v2Max);
    }

    EMP_RETURN size_t count(void) const { return m_Matrices.count(); }

    EMP_RETURN c_v4f& current_color(void) const
    { return m_v4RgbColor; }

    EMP_RETURN m44f const& current_matrix(void) const
    { return m_mCurrentMatrix; }

    v2f max(void) const { return m_v2Max;}//v2f::sub(m_v2Max, m_v2Min); }

    v2f min(void) const { return m_v2Min; }

    void set_min_size(c_float a_fValue)
    { m_fMinSize = a_fValue; }

    void set_min_alpha(c_float a_fValue)
    { m_fMinAlpha = a_fValue; }

    EMP_RETURN bool visible(void) const
    {
        m44f const& rCurrent = current_matrix();
        c_float fMinSizeX = m_fMinSize;
        //c_float fRatio = EMP_GUI_SKIN.resolution().current_ratio();
        //c_float fMinY = fMinX * fRatio;
        //c_float fMaxX = 1000000;
        //c_float fMaxY = fMaxX * fRatio;



        c_v3f vs = m44f::transform_vector({ 1.0, 1.0, 0.0}, rCurrent); // @@0 v2f
        //c_v3f v1 = m44f::transform_vector({ 0.0, 1.0, 0.0}, rCurrent); // @@0 v2f


        //c_v4f v0 = m44f::mul_v4(rCurrent, {0, 0, 0, 1});
        //c_v4f v1 = m44f::mul_v4(rCurrent, {1, 1, 0, 1});


        //c_float fSizeX = mat::abs(v1.x - v0.x);
        //c_float fSizeY = mat::abs(v1.y - v0.y);


        c_float length = mat::sqr(vs.x * vs.x + vs.y * vs.y + vs.z * vs.z);

        //c_float fSizeAX = rCurrent.f00 * rCurrent.f00 + rCurrent.f01 * rCurrent.f01 + rCurrent.f02 * rCurrent.f02;
        //c_float fSizeAY = rCurrent.f10 * rCurrent.f10 + rCurrent.f11 * rCurrent.f11 + rCurrent.f12 * rCurrent.f12;

        if (length < fMinSizeX)
            return false;


        c_float fMinAlpha = m_fMinAlpha;
        auto& color = current_color();
        if (fMinAlpha < 0.0)
        {
            if (color.w < mat::abs(fMinAlpha))
            { return false; }
        }
        else if (fMinAlpha > 0.0)
        {
        }
        return true;
    }

    void set_color(c_float fh, c_float fs, c_float fv, c_float fa)
    {
        c_float fNextH = fh;
        c_float fNextS = fs;
        c_float fNextV = fv;
        c_float fNextA = (m_fMinAlpha > 0.0) ? tt::maxof(m_fMinAlpha, fa) : fa;
        c_float fFinalH = fNextH > 360.0f ? 0.0f : fNextH < 0.0f ? 360.0f : fNextH;
        m_v4CurrentColor = v4f::max(v4f::zero(), v4f::min({ 360, 1, 1, 1 }, { fFinalH, fNextS, fNextV, fNextA }));
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
    }

    void update_color(c_float fh, c_float fs, c_float fv, c_float fa)
    {
        set_color(m_v4CurrentColor.x + fh, m_v4CurrentColor.y + fs, m_v4CurrentColor.z + fv, m_v4CurrentColor.w + fa);
    }

    void translate(c_float a_fX, c_float a_fY, c_float a_fZ)
    {
        m44f m;
        m44f::identity(m);
        m44f::translate(m, { a_fX, a_fY, a_fZ }, m);
        m44f::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    void scale(c_float a_fX, c_float a_fY, c_float a_fZ)
    {
        m44f m;
        m44f::identity(m);
        m44f::scale(m, { a_fX, a_fY, a_fZ }, m);
        m44f::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    void rotate(float const a_fZ)
    {
        m44f m;
        m44f::identity(m);
        m44f::rotation_z(-a_fZ, m);
        m44f::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    void flip(float const a_fX)
    {
        m44f m;
        m44f::identity(m);
        m44f::flip(-a_fX, m);
        m44f::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    EMP_RETURN ose::return_e push(void)
    {
        if (visible() == false)
        { return ose::return_e::Break; }

        m_vv4Colors.add(m_v4CurrentColor);
        m_Matrices.add(m_mCurrentMatrix);

        return ose::return_e::Continue;
    }

    EMP_RETURN ose::return_e pop(void)
    {
        m_mCurrentMatrix = m_Matrices.current();
        m_Matrices.pop();

        m_v4CurrentColor = m_vv4Colors.last_ref();
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
        m_vv4Colors.resize(m_vv4Colors.count() - 1);

        return ose::return_e::Continue;
    }

    EMP_RETURN size_t byte_count(void) const
    {
        size_t stSize = 0;
        stSize += m_Matrices.byte_count();
        stSize += m_vv4Colors.size() * sizeof(ui32);
        return stSize;
    }

    void emit_triangle(gfx::buffer_t<ui16>& a_rIndices, gfx::buffer_t<v3f, v4f>& a_rVertices) // @@0 tristrip
    {
        c_v4f& c = current_color();
        m44f const& m = current_matrix();

        c_v4f v0 = m44f::mul_v4(m, { 0, 0.5f, 0, 1 }); // @@0 v2f
        c_v4f v1 = m44f::mul_v4(m, { -0.5f, -0.5f, 0, 1 }); // @@0 v2f
        c_v4f v2 = m44f::mul_v4(m, { 0.5f, -0.5f, 0, 1 }); // @@0 v2f

        m_v2Min = v2f::min(v4f::xy(v0), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v1), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v2), m_v2Min);

        m_v2Max = v2f::max(v4f::xy(v0), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v1), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v2), m_v2Max);

        // @@0 refact
        gfx::emit_triangle(a_rVertices.count(), a_rIndices);
        a_rVertices.add(v4f::xyz(v0), c);
        a_rVertices.add(v4f::xyz(v1), c);
        a_rVertices.add(v4f::xyz(v2), c);
    }

    void init_circle(c_size a_stCount)
    {
        for (size_t stIndex = 0; stIndex < a_stCount; ++stIndex)
        {
            float const fIndex = ast::cast<float>(stIndex) *
            2 * mat::pi<float>() / ast::cast<float>(a_stCount);
            m_v4Circle.add({ 0.5f * mat::cos(fIndex), 0.5f * mat::sin(fIndex), 0, 1 });
        }
    }

    void emit_circle(gfx::buffer_t<ui16>& a_rIndices, gfx::buffer_t<v3f, v4f>& a_rVertices) // @@0 tristrip
    {
        c_v4f& c = current_color();
        m44f::type const& m = current_matrix();

        c_ui16 ui16Count = ast::cast<ui16>(m_v4Circle.count());
        c_ui16 ui16Base = ast::cast<ui16>(a_rVertices.count());
        c_v4f v4Zero = m44f::mul_v4(m, { 0, 0, 0, 1 }); // @@0 v2f

        a_rVertices.add(v4f::xyz(v4Zero), c);

        //size_t i = 0;
        //EMP_LOG_INFO("v#0 : #1 : #2", i, mat::v4f::get_x(v4Start), mat::v4f::get_y(v4Start)); ++i;

auto const& lEmitVertex = [this, &a_rVertices, &c, &m](c_ui16 a_ui16Index)
{
    c_v4f v = m44f::mul_v4(m, m_v4Circle.ref(a_ui16Index));
    a_rVertices.add(v4f::xyz(v), c);
    m_v2Min = v2f::min(v4f::xy(v), m_v2Min); // @@0 device move in mesh
    m_v2Max = v2f::max(v4f::xy(v), m_v2Max); // @@0 device move in mesh
};

        for (ui16 ui16Index = 0; ui16Index < ui16Count; ++ui16Index)
        {
            lEmitVertex(ui16Index);
            a_rIndices.add(ast::cast<ui16>(ui16Base + 0));
            a_rIndices.add(ast::cast<ui16>(ui16Base + ui16Index + 1));
            a_rIndices.add(ast::cast<ui16>(ui16Base + ui16Index + 2));

            //EMP_LOG_INFO("v#0 : #1 : #2", i, mat::v4f::get_x(v4Zero), mat::v4f::get_y(v4Zero)); ++i;
            //EMP_LOG_INFO("v#0 : #1 : #2", i, mat::v4f::get_x(v1), mat::v4f::get_y(v1)); ++i;
            //EMP_LOG_INFO("v#0 : #1 : #2", i, mat::v4f::get_x(v1), mat::v4f::get_y(v1)); ++i;
        }

        //EMP_LOG_INFO("v#0 : #1 : #2", i, mat::v4f::get_x(v4Zero), mat::v4f::get_y(v4Zero)); ++i;
        //EMP_LOG_INFO("v#0 : #1 : #2", i, mat::v4f::get_x(v4Start), mat::v4f::get_y(v4Start)); ++i;

        lEmitVertex(0);
    }

    void emit_quad(gfx::buffer_t<ui16>& a_rIndices, gfx::buffer_t<v3f, v4f>& a_rVertices) // @@0 tristrip
    {
        c_v4f& c = current_color();
        m44f const& m = current_matrix();

        c_float fHX = 0.5f;
        c_float fHY = fHX;// * EMP_GAME().gui_skin().resolution().current_ratio();
        c_v4f v0 = m44f::mul_v4(m, { -fHX, -fHY, 0, 1 }); // @@0 v2f
        c_v4f v1 = m44f::mul_v4(m, { fHX, -fHY, 0, 1 });
        c_v4f v2 = m44f::mul_v4(m, { -fHX, fHY, 0, 1 });
        c_v4f v3 = m44f::mul_v4(m, { fHX, fHY, 0, 1 });

        m_v2Min = v2f::min(v4f::xy(v0), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v1), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v2), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v3), m_v2Min);

        m_v2Max = v2f::max(v4f::xy(v0), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v1), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v2), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v3), m_v2Max);

        gfx::emit_quad(a_rVertices.count(), a_rIndices);
        a_rVertices.add(v4f::xyz(v0), c);
        a_rVertices.add(v4f::xyz(v1), c);
        a_rVertices.add(v4f::xyz(v2), c);
        a_rVertices.add(v4f::xyz(v3), c);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct rgen_option_s
{
public:
    rgen_option_s()
    {
        reset(0);
    }

    float clearH;
    float clearS;
    float clearB;
    float clearA;

    float minSize;
    float minAlpha;

    float originX;
    float originY;

    float scaleX;
    float scaleY;

    uint32_t seed;

    uint32_t instructionsPerFrame;
    //uint32_t instructionsPerImage;

    uint32_t trianglesPerFrame;
    uint32_t trianglesPerImage;

    uint32_t bytesPerFrame;
    uint32_t bytesPerImage;

    uint32_t primitivesPerImage;


    void reset(uint32_t a_Seed)
    {
        clearH = 300;
        clearS = 1;
        clearB = 1;
        clearA = 1;

        minSize = 0.01f;
        minAlpha = 0.2f;

        originX = 0;
        originY = 0;

        scaleX = 0;
        scaleY = 0;

        seed = a_Seed;

        instructionsPerFrame = 65536;
        //instructionsPerImage = 0;

        trianglesPerFrame = 1024;
        trianglesPerImage = 0;

        bytesPerFrame = 1024;
        bytesPerImage = 0;

        primitivesPerImage = 0;
    }

    EMP_RETURN bool load(ose::compiler_c& a_rCompiler, ose::program_c& a_rProgram)
    {
#define XTMP_OPT(x_Name, x_Var, x_Default)\
{\
    ose::error_e const eResult = a_rCompiler.push_data(a_rProgram, astring(ALITERAL(x_Name)));\
    EMP_RET_FALSE_IF(eResult == ose::error_e::CompilerError);\
    if (eResult == ose::error_e::none)\
    { EMP_RET_FALSE_IF_NOT(a_rCompiler.m_pStack->val(0, x_Var)); }\
    else\
    { x_Var = x_Default; }\
}

    auto def = rgen_option_s();
    XTMP_OPT("__clear_hue float", clearH, def.clearH);
    XTMP_OPT("__clear_saturation float", clearS, def.clearS);
    XTMP_OPT("__clear_brightness float", clearB, def.clearB);
    XTMP_OPT("__clear_alpha float", clearA, def.clearA);
    XTMP_OPT("__min_size float", minSize, def.minSize);
    XTMP_OPT("__min_alpha float", minAlpha, def.minAlpha);
    XTMP_OPT("__origin_x float", originX, def.originX);
    XTMP_OPT("__origin_y float", originY, def.originY);
    XTMP_OPT("__scale_x float", scaleX, def.scaleX);
    XTMP_OPT("__scale_y float", scaleY, def.scaleY);
    XTMP_OPT("__seed uint", seed, def.seed);
    XTMP_OPT("__instructions_per_frame uint", instructionsPerFrame, def.instructionsPerFrame);
    // XTMP_OPT("__instructions_per_image uint", instructionsPerImage, def.instructionsPerImage);
    XTMP_OPT("__triangles_per_frame uint", trianglesPerFrame, def.trianglesPerFrame);
    XTMP_OPT("__triangles_per_image uint", trianglesPerImage, def.trianglesPerImage);
    XTMP_OPT("__bytes_per_frame uint", bytesPerFrame, def.bytesPerFrame);
    XTMP_OPT("__bytes_per_image uint", bytesPerImage, def.bytesPerImage);
    XTMP_OPT("__primitives_per_image uint", primitivesPerImage, def.primitivesPerImage);
        return true;
    }

    EMP_RETURN bool save(ose::compiler_c& a_rCompiler, ose::program_c& a_rProgram) const
    {
        // auto const l_add_data = [](ose::compiler_c& a_rCompiler, ose::program_c& a_rProgram, c_astring& a_rsName, auto a_tValue) EMP_RETURN -> bool
        // {
        //     auto& rTable = a_rProgram.m_Table;
        //     auto const* const pType = rTable.get_data_type<decltype(a_tValue)>();
        //     EMP_RET_FALSE_IF(pType == nullptr);
        //     auto* const pLexeme = a_rProgram.m_Table.get_lexeme(a_rsName, pType);
        //     if (pLexeme == nullptr)
        //     {
        //         /*c_size stDeclare = a_rCompiler.alloc_node();
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stDeclare));
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.insert_child_node(stAst, stDeclare, 0));
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.fill_node(stDeclare, a_rCompiler.m_pAssign, 0, 0)); // @@1 invalid line/column number*/

        //         c_size stName = a_rCompiler.alloc_node(&a_rProgram);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stName));

        //         a_rCompiler.m_vNodes.ref(stName).m_eType = ose::node_e::DefinitionU;
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.insert_child_node(a_rProgram.m_stAst, stName, 0));
        //         ose::data_c* const pData =rTable.reg_data(a_rsName, pType);
        //         EMP_RET_FALSE_IF(pData == nullptr);
        //         pData->m_bLocal = false;
        //         a_rCompiler.m_vNodes.ref(stName).m_sName = a_rsName;
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.fill_node(stName, pData, { 0, 0})); // @@1 invalid line/column number

        //         c_size stLeftBrace = a_rCompiler.alloc_node(&a_rProgram);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stLeftBrace));
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.add_child_node(stName, stLeftBrace));
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.fill_node(stLeftBrace, a_rCompiler.m_pLeftBrace, {0, 0})); // @@1 invalid line/column number

        //         c_size stLiteral = a_rCompiler.alloc_node(&a_rProgram);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stLiteral));
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.add_child_node(stLeftBrace, stLiteral));

        //         auto* pLiteral = ose::to_literal<decltype(a_tValue)>(rTable.get_lexeme(a_rsName, pType));
        //         if (pLiteral == nullptr)
        //         { pLiteral = rTable.reg_literal<decltype(a_tValue)>(a_tValue); }

        //         EMP_RET_FALSE_IF(pLiteral == nullptr);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.fill_node(stLiteral, pLiteral, {0, 0})); // @@1 invalid line/column number
        //     }
        //     else
        //     {
        //         c_size stName = a_rProgram.find_node(&a_rCompiler, pLexeme);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stName));
        //         ose::node_c& rName = a_rCompiler.m_vNodes.ref(stName);
        //         EMP_RET_FALSE_IF(rName.m_pvstLeaves == nullptr || emp::tt::is_max(rName.m_stLeaves));
        //         EMP_RET_FALSE_IF(rName.m_pvstLeaves->count() != 1);

        //         c_size stLeftBrace = rName.m_pvstLeaves->val(0);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stLeftBrace));
        //         ose::node_c& rLeftBrace = a_rCompiler.m_vNodes.ref(stLeftBrace);
        //         EMP_RET_FALSE_IF(rLeftBrace.m_pvstLeaves == nullptr || emp::tt::is_max(rLeftBrace.m_stLeaves));
        //         EMP_RET_FALSE_IF(rLeftBrace.m_pvstLeaves->count() != 1);

        //         size_t stLiteral = rLeftBrace.m_pvstLeaves->val(0);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stLiteral));
        //         ose::node_c* pLiteral = a_rCompiler.m_vNodes.ptr(stLiteral);
        //         EMP_RET_FALSE_IF(pLiteral->m_pLexeme == nullptr);

        //         if (pLiteral->m_pLexeme->m_eType == ose::lexeme_e::Literal)
        //         {
        //             EMP_RET_FALSE_IF(pLiteral->m_pvstLeaves != nullptr || emp::tt::is_not_max(pLiteral->m_stLeaves));
        //         }
        //         else
        //         {
        //             EMP_RET_FALSE_IF(pLiteral->m_pvstLeaves == nullptr || emp::tt::is_max(pLiteral->m_stLeaves));
        //             stLiteral = a_rCompiler.alloc_node(&a_rProgram);
        //             EMP_RET_FALSE_IF_NOT(a_rCompiler.m_vNodes.is_valid_index(stLiteral));
        //             pLiteral = a_rCompiler.m_vNodes.ptr(stLiteral);
        //             EMP_RET_FALSE_IF_NOT(a_rCompiler.set_child_node(stLeftBrace, stLiteral, 0));
        //         }

        //         EMP_RET_FALSE_IF(pLiteral == nullptr);

        //         auto* const pLiteralLexeme = rTable.reg_literal<decltype(a_tValue)>(a_tValue);
        //         EMP_RET_FALSE_IF_NOT(a_rCompiler.fill_node(stLiteral, pLiteralLexeme, {0, 0}));

        //         pLiteral->m_sName = pLiteralLexeme->m_sName;
        //         pLiteral->m_pLexeme = pLiteralLexeme;

        //         //a_rCompiler.fill_node(stNewNode);
        //         //a_rCompiler.m_vNodes.ref(stNewiNode)->m_pLexeme = pLexeme;
        //     }

        //     return true;
        // };


    auto def = rgen_option_s();

    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__clear_hue")), clearH, def.clearH));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__clear_saturation")), clearS, def.clearS));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__clear_brightness")), clearB, def.clearB));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__clear_alpha")), clearA, def.clearA));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__min_size")), minSize, def.minSize));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__min_alpha")), minAlpha, def.minAlpha));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__origin_x")), originX, def.originX));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__origin_y")), originY, def.originY));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__scale_x")), scaleX, def.scaleX));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_float(a_rProgram, astring(ALITERAL("__scale_y")), scaleY, def.scaleY));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__seed")), seed, def.seed));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__instructions_per_frame")), instructionsPerFrame, def.instructionsPerFrame));
    // EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__instructions_per_image")), instructionsPerImage, def.instructionsPerImage));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__triangles_per_frame")), trianglesPerFrame, def.trianglesPerFrame));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__triangles_per_image")), trianglesPerImage, def.trianglesPerImage));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__bytes_per_frame")), bytesPerFrame, def.bytesPerFrame));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__bytes_per_image")), bytesPerImage, def.bytesPerImage));
    EMP_RET_FALSE_IF_NOT(a_rCompiler.set_uint(a_rProgram, astring(ALITERAL("__primitives_per_image")), primitivesPerImage, def.primitivesPerImage));

#undef XTMP_OPT
#undef XTMP_OPTIONS

        return true;

    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
struct best_s
{
    ose::program_c* m_pProgram = nullptr;
    astring m_sMother = astring();
    astring m_sFather = astring();
    float m_fAlpha = 0.0;
    size_t m_stCount = 0;
    tim::time_c m_Stamp = tim::time_c();
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_CLASS(renderlet_c)

public:
    renderlet_c(void) {}
    ~renderlet_c(void){}

public:
    astring m_sMother;
    astring m_sFather;

    ose::program_c* m_pProgram = nullptr;
    size_t EMP_TT_MAX_VAR(m_stProgram);
    EMP_POINTER(ose::stack_c) m_pStack;
    EMP_POINTER(gui::im_stack_c) m_pImStack;
    EMP_POINTER((vector<std::tuple<gfx::buffer_t<ui16>, gfx::buffer_t<v3f, v4f>>*>)) m_pvpImMeshes;
    gfx::buffer_t<ui16>* m_pImMeshIndices = nullptr;
    gfx::buffer_t<v3f, v4f>* m_pImMeshVertices = nullptr;

    tim::time_c m_Stamp;
    m44f m_m44Transform;
    m44f m_m44PreviousTransform;
    m44f m_m44RealPrevious;
    float m_fX;
    float m_fY;
    float m_fScaleX;
    float m_fScaleY;
    float m_fPreviousX;
    float m_fPreviousY;
    float m_fPreviousScaleX;
    float m_fPreviousScaleY;

    rgen_option_s m_Options;

    v2f m_origin;
    v2f m_delta;
    float m_fSizeX = 0;
    float m_fSizeY = 0;

    bool m_bCompleted = false;
    bool m_bAllocated = false;

    int m_iRendered = 0;
    int m_iFrame = 0;
    constexpr static const ui32 sc_ui32TargetCount = 3;
    static_assert(renderlet_c::sc_ui32TargetCount >= 2);

    array<gfx::target_s, sc_ui32TargetCount, true> m_aTargets;
    size_t m_stCurrentTarget = 0;
    size_t m_stAllocatedBytes = 0;
    size_t m_stAllocatedCount = 0;
    ui32 m_uiInstructionsProcessed = 0;
    ui32 m_uiPrimitivesProcessed = 0;

    EMP_RETURN bool print_ose(ose::compiler_c& a_rCompiler, astring& a_rsOutput, c_bool a_bPrune)
    {
        if (m_pProgram != nullptr &&
            a_rCompiler.m_vpPrograms.is_valid_index(m_stProgram))
        {
            EMP_RET_FALSE_IF_NOT(a_rCompiler.rename(m_stProgram, a_bPrune));
            EMP_RET_FALSE_IF_NOT(m_Options.save(a_rCompiler, *m_pProgram));
            if (a_rCompiler.print_ose(m_pProgram->m_stAst, a_rsOutput))
            {
                m_pProgram->set_buffer(a_rsOutput);
                return true;
            }
        }

        return false;
    }


    EMP_RETURN bool print_asm(ose::compiler_c& a_rCompiler, astring& a_rsOutput, c_bool a_bFull, vector<astring>& a_rvsInstructions)
    {
        if (m_pProgram != nullptr &&
            a_rCompiler.m_vpPrograms.is_valid_index(m_stProgram))
        {
            if (emp::ose::print_asm(*m_pProgram, a_rsOutput, a_bFull, a_rvsInstructions))
            {
                return true;
            }
        }

        return false;
    }
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_NOCOPY_BASE(class, renderer_c, public, docker_c)
public:
    control_i* m_pParent = nullptr;
    vector<std::tuple<gfx::buffer_t<ui16>, gfx::buffer_t<v3f, v4f>>*> m_vpImMeshes;
    size_t m_stImMesh;
    renderlet_c* m_pRenderlet = nullptr;
    vector<ui32> m_vui32Pixels;

    vector<ui8> m_vui8Scratch;

    EMP_POINTER(ose::compiler_c) m_pCompiler;
    EMP_POINTER(ose::processor_c) m_pProcessor;

    tim::time_c m_RefreshDirectoryTime;
    tim::time_c m_RefreshSourceTime;

    //gfx::target_s m_PngTarget;
    gfx::target_s m_MsaaTarget0;
    //gfx::target_s m_MsaaTarget1;
    gfx::target_s m_ResolveTarget;

    bool m_bPng = false;
    bool m_bMove = false;
    bool m_bReset = false;
    bool m_bBakeOptions = false;
    bool m_bFirstBake = false;
    bool m_bResetOptions = false;
    bool m_bWriteOptions = false;
    bool m_bNoReload = false;
    bool m_bRandomSeed = false;
    bool m_bSaveInstructionsProcessed = false;

    astring m_sMoveFolder;
    renderlet_c m_Rendered;

    size_t m_stFileCount;
    vector<astring> m_vsFiles;
    vector<astring> m_vsToSortFiles;
    astring m_sPath;
    //astring m_sWorkingDir{ALITERAL("v2/mint/rgens/")};
    astring m_sWorkingDir{ALITERAL("")};
    int m_iNextFile;
    size_t m_stCurrentFile;
    std::random_device m_RandomDevice;
    mat::random_float_s m_RandomFloat;

    //renderer_e m_eMod = renderer_e::Genome;
    //renderer_e m_eMod = renderer_e::Rename;
    //renderer_e m_eMod = renderer_e::Duplicate;
    renderer_e m_eMod = renderer_e::Edition;
    //renderer_e m_eMod = renderer_e::Thumbnail;

    bool m_bDragLeft;

    v2f m_v2DragPosition;
    v2f m_v2DragOrigin;
    v2f m_v2DragDelta;
    tim::time_c m_DeltaTime;

public:
    renderer_c(void);
    ~renderer_c(void);

public:
    EMP_RETURN bool create(void)
    {
        m_iNextFile = 1;
        m_stCurrentFile = 0;

        //EMP_APPLICATION.add_resize_hook(fct::function(this, &renderer_c::resize_buffers));

        EMP_RET_FALSE_IF_NOT(m_pCompiler->create());

        ose::table_c& rTable = m_pCompiler->m_Table;
        ose::rti::data_c const* const pRealType = rTable.real_type();

        c_size stFloatCount = 4;
        for (size_t stIndex = 0; stIndex < stFloatCount; ++stIndex)
        {
            ose::rti::function_c* pType = rTable.new_type<ose::rti::function_c>();
            EMP_RET_FALSE_IF(pType == nullptr);

            for (size_t stFloat = 0; stFloat <= stIndex; ++stFloat)
            { EMP_RET_FALSE_IF(pType->add_parameter(pRealType) == false); }
            EMP_RET_FALSE_IF(rTable.qualify_type(pType) == false);
            EMP_RET_FALSE_IF(rTable.reg_type<ose::rti::function_c>(pType) == false);
            EMP_LOG_INFO("REGISTERED TYPE #0", pType->m_sName);
        }

    #define XTMP_GUI_REG(x_Extension, x_Name, x_Type)\
        case gui::im_operation_e::x_Extension:\
        {\
            if (rTable.reg_extension(enum_to__(im_operation_e::x_Extension), ALITERAL(x_Name), ALITERAL(x_Type), false) == nullptr)\
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
            //XTMP_GUI_REG(hsb          , "hsb"       , "()(float,float,float,)") // DONE
            //XTMP_GUI_REG(sba          , "sba"       , "()(float,float,float,)") // DONE
            //XTMP_GUI_REG(hs           , "hs"        , "()(float,float,)") // DONE
            //XTMP_GUI_REG(hb           , "hb"        , "()(float,float,)") // DONE
            //XTMP_GUI_REG(sb           , "sb"        , "()(float,float,)") // DONE
            //XTMP_GUI_REG(sa           , "sa"        , "()(float,float,)") // DONE
            //XTMP_GUI_REG(ba           , "ba"        , "()(float,float,)") // REMOVE
            //XTMP_GUI_REG(hue          , "h"         , "()(float,)") // REMOVE
            //XTMP_GUI_REG(saturation   , "u"         , "()(float,)") // REMOVE
            //XTMP_GUI_REG(brightness   , "b"         , "()(float,)") // REMOVE
            //XTMP_GUI_REG(alpha        , "a"         , "()(float,)") // REMOVE
            XTMP_GUI_REG(translate    , "t"         , "()(float,)")
            XTMP_GUI_REG(translate_xy , "t"         , "()(float,float,)") // REMOVE
            //XTMP_GUI_REG(translate_x  , "x"         , "()(float,)") // REMOVE
            //XTMP_GUI_REG(translate_y  , "y"         , "()(float,)") // REMOVE
            XTMP_GUI_REG(rotate       , "r"         , "()(float,)")
            XTMP_GUI_REG(scale        , "s"         , "()(float,)")
            XTMP_GUI_REG(scale_xy     , "s"         , "()(float,float,)") // REMOVE
            //XTMP_GUI_REG(scale_x      , "sx"        , "()(float,)") // REMOVE
            //XTMP_GUI_REG(scale_y      , "sy"        , "()(float,)") // REMOVE
            XTMP_GUI_REG(flip         , "f"         , "()(float,)")
            default:
            { break; }
        };
    #undef XTMP_REG

        m_pProcessor->set_extension(fct::function(this, &renderer_c::process));
        m_pProcessor->set_push(fct::function(this, &renderer_c::push));
        m_pProcessor->set_pop(fct::function(this, &renderer_c::pop));

        m_Rendered.m_pImStack->init_circle(128);

        EMP_RET_FALSE_IF_NOT(alloc_targets());
        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());

        m_sPath.clear();
        tt::silent(STR_FORMATA(m_sPath, "#0src/#1/", EMP_APPLICATION.resource_path(), m_sWorkingDir));

        m_vsToSortFiles.resize(0);
        EMP_RET_FALSE_IF_NOT(fs::list(m_sPath, ALITERAL("*.txt"), m_vsFiles, false));
        //sort_files(m_vsToSortFiles, m_vsFiles);
        m_stFileCount = m_vsFiles.count();

        //m_mInvalidPrograms.set(ALITERAL("rgen_0"), 0);
        //m_mInvalidPrograms.set(ALITERAL("rgen_1"), 0);

        return true;
    }

    void destroy(void)
    {
        m_pParent = nullptr;

        destroy_buffers();

        cnt::foreach(m_vpImMeshes, mem::safe_delete_reset_t());
        m_vpImMeshes.resize(0);
        m_stImMesh = 0;
    }


    void destroy_buffers(void)
    {
auto const& l_destroy_target = [](gfx::target_s& a_rTarget)
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

        for (size_t stTarget = 0; stTarget < m_Rendered.m_aTargets.count(); ++stTarget)
        { l_destroy_target(m_Rendered.m_aTargets.ref(stTarget)); }

        //l_destroy_target(m_PngTarget);
        l_destroy_target(m_MsaaTarget0);
        //l_destroy_target(m_MsaaTarget1);

        l_destroy_target(m_ResolveTarget);
    }

    EMP_RETURN bool alloc_targets()
    {
        auto& rDevice = EMP_GRAPHICS;

auto const& l_alloc_target = [&rDevice] (c_ui32 l_ui32Width, c_ui32 l_ui32Height, gfx::target_s& a_rTarget, bool msaa = false) EMP_RETURN -> bool
{
    gfx::target_s oNewTarget;
    oNewTarget.m_ui32Width = l_ui32Width;
    oNewTarget.m_ui32Height = l_ui32Height;
    rDevice.glGenTextures_(1, &oNewTarget.m_ui32Texture);
    rDevice.glGenFramebuffers_(1, &oNewTarget.m_ui32Framebuffer);
    //rDevice.glGenRenderbuffers_(1, &oNewTarget.m_ui32Renderbuffer);

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

    c_si32 si32Width = static_cast<si32>(l_ui32Width);
    c_si32 si32Height = static_cast<si32>(l_ui32Height);
    if (msaa)
    {
        rDevice.glTexImage2DMultisample_(glTarget, 8, GL_RGBA, si32Width, si32Height, false);
    }
    else
    {
        rDevice.glTexImage2D_(glTarget, 0, GL_RGBA, si32Width, si32Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    }

    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oNewTarget));

    rDevice.glFramebufferTexture2D_(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glTarget, oNewTarget.m_ui32Texture, 0);

    //rDevice.glBindRenderbuffer_(GL_RENDERBUFFER, oNewTarget.m_ui32Renderbuffer);
    //rDevice.glRenderbufferStorage_(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, l_si32Width, l_si32Height);
    //rDevice.glFramebufferRenderbuffer_(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, oNewTarget.m_ui32Renderbuffer);

    EMP_RET_FALSE_IF_NOT(rDevice.framebuffer_status());
    EMP_RET_FALSE_IF_NOT(rDevice.clear_target(v4f::zero()));

    a_rTarget = oNewTarget;
    return true;
};


        //c_ui32 uiRender = 4096;
        //c_ui32 uiPng = uiRender / 2;

        c_ui32 uiRender = 4096 * 2;
        //c_ui32 uiPng = uiRender / 2;

        for (size_t stTarget = 0; stTarget < m_Rendered.m_aTargets.count(); ++stTarget)
        { EMP_RET_FALSE_IF_NOT(l_alloc_target(1024, 1024, m_Rendered.m_aTargets.ref(stTarget))); }

        //EMP_RET_FALSE_IF_NOT(l_alloc_target(uiPng, uiPng, m_PngTarget));
        EMP_RET_FALSE_IF_NOT(l_alloc_target(uiRender, uiRender, m_ResolveTarget, false));
        EMP_RET_FALSE_IF_NOT(l_alloc_target(uiRender, uiRender, m_MsaaTarget0, true));
        //EMP_RET_FALSE_IF_NOT(l_alloc_target(uiRender, uiRender, m_MsaaTarget1, true));

        return true;
    }

#define XTMP_LOG(x_Log) EMP_OSE_IF_DEBUG(EMP_LOG_INFO x_Log;)
#define XTMP_STACK(x_Stack, x_Action, x_From)\
{\
    auto* pImStack = &*m_pRenderlet->m_pImStack;\
    XTMP_LOG(("begin #0 #1, #2", ALITERAL(x_Action), ALITERAL(x_From), pImStack->count()))\
    ose::return_e eReturn = pImStack->x_Stack();\
    XTMP_LOG(("end #0 #1, #2", ALITERAL(x_Action), ALITERAL(x_From), pImStack->count()))\
    if (eReturn != ose::return_e::Continue)\
    { return eReturn; }\
}

    EMP_RETURN ose::return_e push(void)
    {
        XTMP_STACK(push, "PUSH", "scope")
        return ose::return_e::Continue;
    }

    EMP_RETURN ose::return_e pop(void)
    {
        XTMP_STACK(pop, "POP", "scope")
        return ose::return_e::Continue;
    }
#undef XTMP_STACK

    EMP_RETURN ose::return_e process(ose::program_c const& EMP_SILENT(a_rProgram), ose::stack_c& a_rStack,
        c_ui32 a_ui32User)
    {
        auto const& l_add_mesh = [this]()
        {
            auto& rvpImMeshes = *m_pRenderlet->m_pvpImMeshes;
            if (rvpImMeshes.count() == 0 || std::get<0>(*rvpImMeshes.last_val()).count() >= 2048 * 3)
            {
                std::tuple<buffer_t<ui16>, buffer_t<v3f, v4f>>* pImMesh = nullptr;
                if (m_stImMesh >= m_vpImMeshes.count())
                {
                    pImMesh = EMP_ALLOCATE((std::tuple<buffer_t<ui16>, buffer_t<v3f, v4f>>));
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
                m_pRenderlet->m_pImMeshIndices->create(gfx::usage_e::Dynamic);
                m_pRenderlet->m_pImMeshVertices->create(gfx::usage_e::Dynamic);
            }
        };

        c_im_operation_e eImOperation = static_cast<im_operation_e>(a_ui32User); //@@0 enum_cast
        switch (eImOperation)
        {
            case im_operation_e::square:
            {
                XTMP_LOG(("SQUARE"))
                if (m_pRenderlet->m_pImStack->visible())//&& m_bProfile == false)
                {
                    if ((m_pRenderlet->m_Options.primitivesPerImage == 0) || (m_pRenderlet->m_uiPrimitivesProcessed < m_pRenderlet->m_Options.primitivesPerImage))
                    {
                        l_add_mesh();
                        m_pRenderlet->m_pImStack->emit_quad(*m_pRenderlet->m_pImMeshIndices, *m_pRenderlet->m_pImMeshVertices);
                    }
                    m_pRenderlet->m_uiPrimitivesProcessed++;
                }
                return ose::return_e::Continue;
            }
            case im_operation_e::triangle:
            {
                XTMP_LOG(("TRIANGLE"))
                if (m_pRenderlet->m_pImStack->visible())// && m_bProfile == false)
                {
                    if ((m_pRenderlet->m_Options.primitivesPerImage == 0) || (m_pRenderlet->m_uiPrimitivesProcessed < m_pRenderlet->m_Options.primitivesPerImage))
                    {
                        l_add_mesh();
                        m_pRenderlet->m_pImStack->emit_triangle(*m_pRenderlet->m_pImMeshIndices, *m_pRenderlet->m_pImMeshVertices);
                    }
                    m_pRenderlet->m_uiPrimitivesProcessed++;
                }
                return ose::return_e::Continue;
            }
            case im_operation_e::circle:
            {
                XTMP_LOG(("CIRCLE"))
                if (m_pRenderlet->m_pImStack->visible())// && m_bProfile == false)
                {
                    if ((m_pRenderlet->m_Options.primitivesPerImage == 0) || (m_pRenderlet->m_uiPrimitivesProcessed < m_pRenderlet->m_Options.primitivesPerImage))
                    {
                        l_add_mesh();
                        m_pRenderlet->m_pImStack->emit_circle(*m_pRenderlet->m_pImMeshIndices, *m_pRenderlet->m_pImMeshVertices);
                    }
                    m_pRenderlet->m_uiPrimitivesProcessed++;
                }
                return ose::return_e::Continue;
            }

    #define XTMP_POP_FLOAT(x_Name)\
            float x_Name = 0;\
            EMP_RET_X_IF_NOT(ose::return_e::Error, a_rStack.pop(x_Name));

            case im_operation_e::hsba:
            {
                XTMP_POP_FLOAT(fA);
                XTMP_POP_FLOAT(fV);
                XTMP_POP_FLOAT(fS);
                XTMP_POP_FLOAT(fH);
                XTMP_LOG(("IM_OPERATION_E::HSVA #0, #1, #2, #3", fH, fS, fV, fA))
                m_pRenderlet->m_pImStack->update_color(fH, fS, fV, fA);
                return ose::return_e::Continue;
            }
            /*case im_operation_e::hsb:
            {
                XTMP_POP_FLOAT(fV);
                XTMP_POP_FLOAT(fS);
                XTMP_POP_FLOAT(fH);
                XTMP_LOG(("IM_OPERATION_E::HSV #0, #1, #2", fH, fS, fV))
                m_pRenderlet->m_pImStack->update_color(fH, fS, fV, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::sba:
            {
                XTMP_POP_FLOAT(fA);
                XTMP_POP_FLOAT(fV);
                XTMP_POP_FLOAT(fS);
                XTMP_LOG(("IM_OPERATION_E::SVA #0, #1, #2", fS, fV, fA))
                m_pRenderlet->m_pImStack->update_color(0, fS, fV, fA);
                return ose::return_e::Continue;
            }
            case im_operation_e::hs:
            {
                XTMP_POP_FLOAT(fS);
                XTMP_POP_FLOAT(fH);
                XTMP_LOG(("IM_OPERATION_E::HS #0, #1", fH, fS))
                m_pRenderlet->m_pImStack->update_color(fH, fS, 0, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::hb:
            {
                XTMP_POP_FLOAT(fV);
                XTMP_POP_FLOAT(fH);
                XTMP_LOG(("IM_OPERATION_E::HV #0, #1", fH, fV))
                m_pRenderlet->m_pImStack->update_color(fH, 0, fV, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::sb:
            {
                XTMP_POP_FLOAT(fV);
                XTMP_POP_FLOAT(fS);
                XTMP_LOG(("IM_OPERATION_E::SV #0, #1", fS, fV))
                m_pRenderlet->m_pImStack->update_color(0, fS, fV, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::sa:
            {
                XTMP_POP_FLOAT(fA);
                XTMP_POP_FLOAT(fS);
                XTMP_LOG(("IM_OPERATION_E::SA #0, #1", fS, fA))
                m_pRenderlet->m_pImStack->update_color(0, fS, 0, fA);
                return ose::return_e::Continue;
            }
            case im_operation_e::ba:
            {
                XTMP_POP_FLOAT(fA);
                XTMP_POP_FLOAT(fV);
                XTMP_LOG(("IM_OPERATION_E::VA #0, #1", fV, fA))
                m_pRenderlet->m_pImStack->update_color(0, 0, fV, fA);
                return ose::return_e::Continue;
            }
            case im_operation_e::hue:
            {
                XTMP_POP_FLOAT(fH);
                XTMP_LOG(("IM_OPERATION_E::H #0", fH))
                m_pRenderlet->m_pImStack->update_color(fH, 0, 0, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::saturation:
            {
                XTMP_POP_FLOAT(fS);
                XTMP_LOG(("IM_OPERATION_E::S #0", fS))
                m_pRenderlet->m_pImStack->update_color(0, fS, 0, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::brightness:
            {
                XTMP_POP_FLOAT(fV);
                XTMP_LOG(("IM_OPERATION_E::V #0", fV))
                m_pRenderlet->m_pImStack->update_color(0, 0, fV, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::alpha:
            {
                XTMP_POP_FLOAT(fA);
                XTMP_LOG(("IM_OPERATION_E::ALPHA #0", fA))
                m_pRenderlet->m_pImStack->update_color(0, 0, 0, fA);
                return ose::return_e::Continue;
            }*/
            case im_operation_e::translate:
            {
                XTMP_POP_FLOAT(fXY);
                XTMP_LOG(("IM_OPERATION_E::TRANSLATE #0", fXY))
                m_pRenderlet->m_pImStack->translate(fXY, fXY, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::translate_xy:
            {
                XTMP_POP_FLOAT(fY);
                XTMP_POP_FLOAT(fX);
                XTMP_LOG(("IM_OPERATION_E::TRANSLATE_XY #0 #1", fX, fY))
                m_pRenderlet->m_pImStack->translate(fX, fY, 0);
                return ose::return_e::Continue;
            }
            /*case im_operation_e::translate_x:
            {
                XTMP_POP_FLOAT(fX);
                XTMP_LOG(("IM_OPERATION_E::TRANSLATE_X #0", fX))
                m_pRenderlet->m_pImStack->translate(fX, 0, 0);
                return ose::return_e::Continue;
            }
            case im_operation_e::translate_y:
            {
                XTMP_POP_FLOAT(fY);
                XTMP_LOG(("IM_OPERATION_E::TRANSLATE_Y #0", fY))
                m_pRenderlet->m_pImStack->translate(0, fY, 0);
                return ose::return_e::Continue;
            }*/
            case im_operation_e::rotate:
            {
                XTMP_POP_FLOAT(fR);
                XTMP_LOG(("IM_OPERATION_E::ROTATE #0", fR))
                m_pRenderlet->m_pImStack->rotate(fR * mat::to_rad<float>());
                return ose::return_e::Continue;
            }
            case im_operation_e::scale:
            {
                XTMP_POP_FLOAT(fXY);
                XTMP_LOG(("IM_OPERATION_E::SCALE #0", fXY))
                m_pRenderlet->m_pImStack->scale(fXY, fXY, 1);
                return ose::return_e::Continue;
            }
            case im_operation_e::scale_xy:
            {
                XTMP_POP_FLOAT(fY);
                XTMP_POP_FLOAT(fX);
                XTMP_LOG(("IM_OPERATION_E::SCALE_XY #0, #1", fX, fY))
                m_pRenderlet->m_pImStack->scale(fX, fY, 1);
                return ose::return_e::Continue;
            }
            /*case im_operation_e::scale_x:
            {
                XTMP_POP_FLOAT(fX);
                XTMP_LOG(("IM_OPERATION_E::SCALE_X #0", fX))
                m_pRenderlet->m_pImStack->scale(fX, 1, 1);
                return ose::return_e::Continue;
            }
            case im_operation_e::scale_y:
            {
                XTMP_POP_FLOAT(fY);
                XTMP_LOG(("IM_OPERATION_E::SCALE_Y #0", fY))
                m_pRenderlet->m_pImStack->scale(1, fY, 1);
                return ose::return_e::Continue;
            }*/
            case im_operation_e::flip:
            {
                XTMP_POP_FLOAT(fF);
                XTMP_LOG(("IM_OPERATION_E::FLIP #0", fF))
                m_pRenderlet->m_pImStack->flip(fF * mat::to_rad<float>());
                return ose::return_e::Continue;
            }
    #undef XTMP_LOG
    #undef XTMP_POP_FLOAT
            case im_operation_e::none:
            default:
            {
                EMP_LOG_ERROR("invalid im_operation_e #0", a_ui32User);
                return ose::return_e::Error;
            }
        }
    }



public:
    //>> control_i
    EMP_RETURN bool update(tim::clock_c const& a_rClock)
    {
        m_DeltaTime = a_rClock.delta();

        tim::time_c::add(m_RefreshSourceTime, m_DeltaTime, m_RefreshSourceTime);
        tim::time_c::add(m_RefreshDirectoryTime, m_DeltaTime, m_RefreshDirectoryTime);
        //tim::time_c::add(m_ThumbTime, m_DeltaTime, m_ThumbTime);

        /*if (tim::time_c::more(m_RefreshDirectoryTime, tim::time_c::from_seconds(2.0f)))
        {
            m_vsFiles.resize(0);
            if (fs::list(m_sPath, ALITERAL("*.txt"), m_vsFiles, false))
            {
                sort_files(m_vsToSortFiles, m_vsFiles);
                m_RefreshDirectoryTime = tim::time_c();
            }
        }*/

        if (tim::time_c::more(m_RefreshSourceTime, tim::time_c::from_seconds(0.5f)))
        {
            if (m_Rendered.m_pProgram != nullptr)
            {
                fs::file_c oFile;
                if (oFile.create(STR_AFORMAT("#0/#1", m_sPath, m_Rendered.m_sMother), fs::file_e::Binary, fs::create_e::Read))
                {
                    astring sBuffer;
                    if (oFile.read_all(sBuffer))
                    {
                        if (false == str::cmp(sBuffer, m_Rendered.m_pProgram->buffer()))
                        {
                            m_bReset = true;
                        }
                    }
                }
            } else {
                m_bReset = true;
            }
        }

        return tbase1::update(a_rClock);
    }

    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(gui::stack_c& a_rStack)
    {
        if (state().visible == false)
        { return true; }

        EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

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
                    m_stCurrentFile = ast::cast<size_t>(mat::abs(m_RandomDevice()));
                }
                else if (a_iDirection < 0)
                {
                    if (m_stCurrentFile <= 0)
                    { m_stCurrentFile = m_vsFiles.count() - 1; }
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
                    // if (m_mInvalidPrograms.get(sFile) == m_mInvalidPrograms.end())
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
            ose::program_c* const pProg = a_rRenderlet.m_pProgram;
            if (pProg != nullptr && a_rRenderlet.m_bCompleted == false &&
                a_rRenderlet.m_pStack->instruction_pointer() != pProg->exit_point() /*&&
                a_rRenderlet.m_stAllocatedBytes < (2048 << 10u) &&
                tim::time_c::to_seconds(a_rRenderlet.m_Stamp) < 180.0f*/)
            {
                m_pRenderlet = &a_rRenderlet;

                c_size stCurrentBytes = EMP_ALLOCATOR.current_bytes();
                c_size stCurrentCount = EMP_ALLOCATOR.current_count();

                tim::time_c::add(a_rRenderlet.m_Stamp, m_DeltaTime, a_rRenderlet.m_Stamp);

                c_ui32 uiInstructionsPerFrame = a_rRenderlet.m_Options.instructionsPerFrame;
                c_ui32 uiInstructionsPerImage = 0;//a_rRenderlet.m_Options.instructionsPerImage;
                c_si32 siCount = m_pProcessor->run(*pProg, *a_rRenderlet.m_pStack, uiInstructionsPerFrame, uiInstructionsPerImage , a_rRenderlet.m_uiInstructionsProcessed);

                c_size stNextBytes = EMP_ALLOCATOR.current_bytes();
                c_size stNextCount = EMP_ALLOCATOR.current_count();

                a_rRenderlet.m_stAllocatedBytes += stNextBytes > stCurrentBytes ? stNextBytes - stCurrentBytes : 0;
                a_rRenderlet.m_stAllocatedCount += stNextCount > stCurrentCount ? stNextCount - stCurrentCount : 0;

                if (a_rRenderlet.m_Options.primitivesPerImage > 0 && a_rRenderlet.m_uiPrimitivesProcessed >= a_rRenderlet.m_Options.primitivesPerImage)
                {
                    EMP_LOG_INFO("REACHED MAX PRIMITIVE COUNT : stopping : '#0' x '#1', #2", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather, a_rRenderlet.m_Options.primitivesPerImage);
                    a_rRenderlet.m_bCompleted = true;
                }


                // if (a_rRenderlet.m_Options.instructionsPerImage > 0 && a_rRenderlet.m_uiInstructionsProcessed >= a_rRenderlet.m_Options.instructionsPerImage)
                // {
                //     EMP_LOG_INFO("REACHED MAX INSTRUCTION COUNT : stopping : '#0' x '#1', #2", a_rRenderlet.m_sMother, a_rRenderlet.m_sFather, a_rRenderlet.m_Options.instructionsPerImage);
                //     a_rRenderlet.m_bCompleted = true;
                // }

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

        auto const& l_reset_renderlet = [&rDevice](renderlet_c& a_rRenderlet, c_astring& a_sMother, c_astring& a_sFather) EMP_UNUSED -> bool
        {
            a_rRenderlet.m_sMother = a_sMother;
            a_rRenderlet.m_sFather = a_sFather;

            a_rRenderlet.m_Stamp = tim::time_c();

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
            a_rRenderlet.m_iRendered = 0;
            a_rRenderlet.m_iFrame = 0;

            m44f::identity(a_rRenderlet.m_m44Transform);
            m44f::identity(a_rRenderlet.m_m44PreviousTransform);
            m44f::identity(a_rRenderlet.m_m44RealPrevious);

            a_rRenderlet.m_pvpImMeshes->resize(0);
            a_rRenderlet.m_pImMeshIndices = nullptr;
            a_rRenderlet.m_pImMeshVertices = nullptr;
            a_rRenderlet.m_pImStack->clear();
            a_rRenderlet.m_pImStack->scale(1, EMP_GUI_SKIN.resolution.current(), 1);

            tt::silent(a_rRenderlet.m_pImStack->push());
            a_rRenderlet.m_stAllocatedBytes = 0;
            a_rRenderlet.m_stAllocatedCount = 0;
            a_rRenderlet.m_uiInstructionsProcessed = 0;
            a_rRenderlet.m_uiPrimitivesProcessed = 0;

            c_size stTargetCount = a_rRenderlet.m_aTargets.count();
            for (size_t stTarget = 0; stTarget < stTargetCount; ++stTarget)
            {
                gfx::target_s const& rTarget = a_rRenderlet.m_aTargets.ref(stTarget);
                EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(rTarget));
                EMP_RET_FALSE_IF_NOT(rDevice.clear_target(v4f::zero()));
            }

            return true;
        };

        auto const& l_transform_renderlet = [](renderlet_c& a_rRenderlet)
        {
            m44f::identity(a_rRenderlet.m_m44PreviousTransform);

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
                m44f::identity(a_rRenderlet.m_m44RealPrevious);
                m44f::identity(a_rRenderlet.m_m44Transform);
            }
            else
            {
                if (!bool_equal(v2Min, tt::max<v2f>::value))
                {
                    c_float fX = v2Min.x;
                    c_float fY = v2Min.y;
                    c_float fWidth = (v2Max.x - v2Min.x);
                    c_float fHeight = (v2Max.y - v2Min.y);

                    c_float fMaxSize = tt::maxof(fWidth,fHeight);

                    c_float fNewX = 0.5f * (fMaxSize - fWidth) - fX - fMaxSize / 2.0f;
                    c_float fNewY = 0.5f * (fMaxSize - fHeight) - fY - fMaxSize / 2.0f;

                    if (a_rRenderlet.m_iRendered <= 0)
                    {
                        a_rRenderlet.m_fPreviousX = a_rRenderlet.m_fX;
                        a_rRenderlet.m_fPreviousY = a_rRenderlet.m_fY;
                        a_rRenderlet.m_fPreviousScaleX = a_rRenderlet.m_fScaleX;
                        a_rRenderlet.m_fPreviousScaleY = a_rRenderlet.m_fScaleY;
                    }

                    a_rRenderlet.m_fX = fNewX;
                    a_rRenderlet.m_fY = fNewY;

                    c_float fScaleX = 2.0f / fWidth;
                    c_float fScaleY = 2.0f / fHeight;
                    c_float fScale = tt::minof(fScaleX, fScaleY);

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
                    m44f scaling;
                    m44f::identity(scaling);
                    m44f::scale(scaling, {  1 / a_rRenderlet.m_fPreviousScaleX,  1 / a_rRenderlet.m_fPreviousScaleY, 1 }, scaling);

                    m44f translate;
                    m44f::identity(translate);
                    m44f::translate(translate, { -a_rRenderlet.m_fPreviousX, -a_rRenderlet.m_fPreviousY, 0 }, translate);

                    m44f::mul(translate, scaling, a_rRenderlet.m_m44RealPrevious);
                }

                bool bChanged = false;
                if (
                        tt::equal_(a_rRenderlet.m_fPreviousScaleX, a_rRenderlet.m_fScaleX) == false ||
                        tt::equal_(a_rRenderlet.m_fPreviousScaleY, a_rRenderlet.m_fScaleY) == false ||
                        tt::equal_(a_rRenderlet.m_fPreviousX, a_rRenderlet.m_fX) == false ||
                        tt::equal_(a_rRenderlet.m_fPreviousY, a_rRenderlet.m_fY) == false)
                {
                    a_rRenderlet.m_m44PreviousTransform = a_rRenderlet.m_m44RealPrevious;
                    bChanged = true;
                }

                {
                    m44f::type scaling;
                    m44f::identity(scaling);
                    m44f::scale(scaling, { a_rRenderlet.m_fScaleX, a_rRenderlet.m_fScaleY, 1.0f }, scaling);

                    m44f translate;
                    m44f::identity(translate);
                    m44f::translate(translate, { a_rRenderlet.m_fX, a_rRenderlet.m_fY, 0 }, translate);

                    m44f::mul(scaling, translate, a_rRenderlet.m_m44Transform);
                    if (bChanged)
                    {
                        m44f::mul(a_rRenderlet.m_m44Transform, a_rRenderlet.m_m44PreviousTransform, a_rRenderlet.m_m44PreviousTransform);
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

            auto const& rOptions = a_rRenderlet.m_Options;
            v4f const vClearColor = to_rgb(rOptions.clearH, rOptions.clearS, rOptions.clearB, rOptions.clearA);
            c_size stMeshCount = a_rRenderlet.m_pvpImMeshes->count();


            size_t stPrevioustTexture = a_rRenderlet.m_stCurrentTarget;
            if (stPrevioustTexture <= 0)
            { stPrevioustTexture = a_rRenderlet.m_aTargets.count() - 1; }
            else
            { stPrevioustTexture = (stPrevioustTexture - 1) % a_rRenderlet.m_aTargets.count(); }

            a_rRenderlet.m_iFrame++;

            if (a_rRenderlet.m_iRendered > 0)
            {
                if (a_rRenderlet.m_uiPrimitivesProcessed <= 0)
                {
                }
                else
                {
                    if (a_rRenderlet.m_Options.scaleX <= 0 || a_rRenderlet.m_Options.scaleY <= 0)
                    {
                        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
                        EMP_RET_FALSE_IF_NOT(rDevice.clear_target(vClearColor));
                        m44f mCurrent;
                        m44f::mul(a_rRenderlet.m_m44RealPrevious, a_rRenderlet.m_m44Transform, mCurrent);
                        rDevice.set_blend(true);
                        rDevice.glBlendFunc_(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
                        rDevice.set_index_buffer(rDevice.quad_index_buffer());
                        rDevice.set_program(rDevice.texture_program());
                        rDevice.set_uniform(uniform_e::m44Transform, mCurrent);
                        rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rRenderlet.m_aTargets.ref(stPrevioustTexture).m_ui32Texture);
                        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        rDevice.draw(gfx::primitive_e::Triangles);
                    }
                }
            }
            else
            {
                if (stMeshCount <= 0 && a_rRenderlet.m_uiPrimitivesProcessed <= 0)
                {
                    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
                    EMP_RET_FALSE_IF_NOT(rDevice.clear_target(vClearColor));
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
                            EMP_RET_FALSE_IF_NOT(rDevice.clear_target(vClearColor));
                        }

                        m44f mCurrent = m44f::ident();
//                        m44f::translate(mCurrent, {-1.0, -1.0, 0}, mCurrent);
                        m44f::mul(mCurrent, a_rRenderlet.m_m44Transform, mCurrent);
                        rDevice.set_blend(true);
                        rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                        rDevice.set_program(rDevice.color_attribute_program());
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
                                    rDevice.draw(gfx::primitive_e::Triangles);
                                }
                            }
                        }
                        a_rRenderlet.m_pvpImMeshes->resize(0);

                        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.glBindFramebuffer__(GL_DRAW_FRAMEBUFFER, m_ResolveTarget.m_ui32Framebuffer));
                        EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.glBindFramebuffer__(GL_READ_FRAMEBUFFER, rTarget0.m_ui32Framebuffer));
                        EMP_RET_FALSE_IF_NOT(rDevice.clear_target(vClearColor));
                        EMP_GRAPHICS.glBlitFramebuffer_(0, 0,
                            static_cast<int>(rTarget0.m_ui32Width),  static_cast<int>(rTarget0.m_ui32Height),
                            0, 0, static_cast<int>(m_ResolveTarget.m_ui32Width), static_cast<int>(m_ResolveTarget.m_ui32Height), GL_COLOR_BUFFER_BIT, GL_LINEAR);
                    }
                    else
                    {
                        c_size stCurrentTarget = a_rRenderlet.m_stCurrentTarget;
                        a_rRenderlet.m_stCurrentTarget = (a_rRenderlet.m_stCurrentTarget + 1) % (a_rRenderlet.m_aTargets.count());

                        // previous texture
                        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(a_rRenderlet.m_aTargets.ref(stCurrentTarget)));
                        EMP_RET_FALSE_IF_NOT(rDevice.clear_target(v4f::zero()));
                        rDevice.set_blend(true);
                        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
                        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
                        rDevice.set_index_buffer(rDevice.quad_index_buffer());
                        rDevice.set_program(rDevice.texture_program());
                        m44f mPrevious = m44f::ident();
//                        m44f::translate(mPrevious, {-1.0, -1.0, 0}, mPrevious);
                        m44f::mul(mPrevious, a_rRenderlet.m_m44PreviousTransform, mPrevious);
                        rDevice.set_uniform(uniform_e::m44Transform, mPrevious);
                        rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rRenderlet.m_aTargets.ref(stPrevioustTexture).m_ui32Texture);
                        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        rDevice.draw(gfx::primitive_e::Triangles);

                        // current meshes
                        m44f mCurrent = m44f::ident();
//                        m44f::translate(mCurrent, {-1.0, -1.0, 0}, mCurrent);
                        m44f::mul(mCurrent, a_rRenderlet.m_m44Transform, mCurrent);
                        rDevice.glBlendFuncSeparate_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                        rDevice.set_program(rDevice.color_attribute_program());
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
                                    rDevice.draw(gfx::primitive_e::Triangles);
                                }
                            }
                        }
                        a_rRenderlet.m_pvpImMeshes->resize(0);

                        // compositing
                        EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
                        EMP_RET_FALSE_IF_NOT(rDevice.clear_target(vClearColor));
                        rDevice.set_blend(true);
                        rDevice.glBlendFunc_(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
                        rDevice.set_index_buffer(rDevice.quad_index_buffer());
                        rDevice.set_program(rDevice.texture_program());
                        rDevice.set_uniform(uniform_e::m44Transform, m44f::ident());
                        rDevice.set_uniform(uniform_e::t2Texture_0, 0, a_rRenderlet.m_aTargets.ref(stCurrentTarget).m_ui32Texture);
                        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        rDevice.draw(gfx::primitive_e::Triangles);
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

        auto const& l_load_renderlet = [this, l_reset_renderlet](renderlet_c& a_rRenderlet, c_astring& a_sMother, c_astring& a_sFather, c_bool EMP_SILENT(a_bMutate), c_bool a_bSkipGlobals, c_size a_stGen)
        {
            m_RefreshSourceTime = tim::time_c();

            l_reset_renderlet(a_rRenderlet, a_sMother, a_sFather);

            bool bDealloc = true;
            size_t EMP_TT_MAX_VAR(stFather);
            auto oFinallyFather = tt::finally([&a_rRenderlet, this, stFather, &bDealloc]()
            {
                if (m_pCompiler->m_vpPrograms.is_valid_index(stFather))
                {
                    bDealloc = m_pCompiler->dealloc_program(stFather);
                    EMP_LOG_ERROR("Failed to deallocate program");
                }
                a_rRenderlet.m_pProgram = nullptr;
                EMP_TT_MAX_VAR(a_rRenderlet.m_stProgram);
            });

            if (a_sFather.len() > 0)
            {
                ose::mutation_s oMutation;
                size_t stMutation;
                ose::crossover_s oCrossover;
                size_t stCrossover;

                stFather = m_pCompiler->from_file(STR_AFORMAT("#0/#1", m_sPath, a_sFather), tt::max<size_t>::value, oMutation, stMutation, oCrossover, stCrossover, a_bSkipGlobals, a_stGen);
                if (m_pCompiler->m_vpPrograms.is_valid_index(stFather))
                {
                    //EMP_LOG_ERROR("Failed to build father '#0'", a_sFather);
                    //m_mInvalidPrograms.set(a_sFather, true);
                }
            }

            ose::mutation_s oMutation;
            size_t stMutation;
            ose::crossover_s oCrossover;
            size_t stCrossover;

            c_size stMother = m_pCompiler->from_file(STR_AFORMAT("#0/#1", m_sPath, a_sMother), stFather, oMutation, stMutation, oCrossover, stCrossover, a_bSkipGlobals, a_stGen);
            ose::program_c* pMother = nullptr;
            if (m_pCompiler->m_vpPrograms.is_valid_index(stMother))
            {
                pMother = m_pCompiler->m_vpPrograms.val(stMother);
                a_rRenderlet.m_pStack->copy(pMother);
                EMP_LOG_INFO("starting : '#0' x '#1'", a_sMother, a_sFather);
            }
            else
            {
                EMP_LOG_INFO("Failed to build mother '#0'", a_sMother);
                //m_mInvalidPrograms.set(a_sMother, true);
            }

            a_rRenderlet.m_pProgram = pMother;
            a_rRenderlet.m_pProgram = pMother;
            a_rRenderlet.m_stProgram = stMother;

            auto const& rOptions = a_rRenderlet.m_Options;
            if (a_rRenderlet.m_pProgram != nullptr && a_bSkipGlobals == false)
            {
                EMP_ASSERT_BOOL_CALL(a_rRenderlet.m_Options.load(*m_pCompiler, *a_rRenderlet.m_pProgram)); //@@0 return instead of assert
                a_rRenderlet.m_pProgram->m_pRandom->reset_seed(rOptions.seed);
            }

            c_float scaleX = rOptions.scaleX <= 0.0f ? 1.0f : rOptions.scaleX;
            c_float scaleY = rOptions.scaleY <= 0.0f ? 1.0f : rOptions.scaleY;
            a_rRenderlet.m_pImStack->scale(scaleX, scaleY, 1.0f);
            a_rRenderlet.m_pImStack->translate(rOptions.originX, rOptions.originY, 0.0);
            //a_rRenderlet.m_pImStack->translate(-1.0f, -1.0f, 0.0f);
            a_rRenderlet.m_pImStack->set_min_size(rOptions.minSize * scaleX);
            a_rRenderlet.m_pImStack->set_min_alpha(rOptions.minAlpha);

            oFinallyFather.release();
        };

        auto const& l_get_next_name = [this](c_astring& a_sMother, c_ui32 a_uiLevel, c_bool a_bRetIfExists, c_astring a_sExtension) -> astring
        {
            astring sPngPath;
            astring sPngFilename;
            astring sPngExtension;
            str::extract_PFE(a_sMother, sPngPath, sPngFilename, sPngExtension);

            astring sPath;
            size_t stIndex = 0;

            while (true)
            {
                sPath.clear();
                tt::silent(STR_FORMATA(sPath, "#0/#1_s#2_v#3.#4", m_sPath, sPngFilename, a_uiLevel, stIndex, a_sExtension));
                bool bExists = false;
                if (fs::path_exists(sPath, bExists) == false)
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

        auto const& l_save_png = [this, l_get_next_name](renderlet_c& a_rRenderlet) -> bool
        {
            auto& rTarget = m_ResolveTarget;
            if (rTarget.m_ui32Width > 0 && rTarget.m_ui32Height > 0)
            {
                c_ui32 ui32Width = rTarget.m_ui32Width;
                ui32 uiLevel = ui32Width;

                c_bool bAlpha = false;

                while (uiLevel > 128)
                {
                    EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.read_pixels(rTarget, m_vui32Pixels, bAlpha));

// save png
#if 1
                    {
                        astring sPath = l_get_next_name(a_rRenderlet.m_sMother, uiLevel, false, ALITERAL("png"));
                        if (sPath.len() > 0)
                        {
                            EMP_LOG_INFO("Saving #0", sPath);
                            if (png::save(sPath, m_vui32Pixels, m_vui8Scratch, uiLevel, uiLevel, false, fs::create_e::New, bAlpha) == false) // @@0 save in devdir
                            {
                                EMP_LOG_ERROR("Failed to save #0", sPath);
                                return false;
                            }
                        }
                    }
#endif

// save jpeg
#if 1
                    {
                        astring sPath = l_get_next_name(a_rRenderlet.m_sMother, uiLevel, false, ALITERAL("jpeg"));
                        if (sPath.len() > 0)
                        {
                            EMP_LOG_INFO("Saving #0", sPath);
                            if (jpg::save(sPath, m_vui32Pixels, m_vui8Scratch, uiLevel, uiLevel, false, fs::create_e::New, bAlpha, 96) == false) // @@0 save in devdir
                            {
                                EMP_LOG_ERROR("Failed to save #0", sPath);
                                return false;
                            }
                        }
                    }
#endif
                    uiLevel /= 2;
                    break;
                }
            }

            return true;
        };

        auto const l_write_renderlet = [this](renderlet_c& a_rRenderlet, c_astring const& a_sDirectory, c_bool a_bPrune) -> bool
        {
            if (a_rRenderlet.m_pProgram != nullptr && // @@0 some bugs here, program is nullptr when it should'nt
                tt::is_not_max(a_rRenderlet.m_pProgram->m_stAst))
            {
                astring sSource;
                EMP_RET_FALSE_IF_NOT(a_rRenderlet.print_ose(*m_pCompiler, sSource, a_bPrune));

                astring sPath(STR_AFORMAT("#0/#1/#2", m_sPath, a_sDirectory, a_rRenderlet.m_sMother));
                EMP_LOG_INFO("SAVING FILE #0", sPath);
                fs::file_c oFile;
                if (oFile.create(sPath, fs::file_e::Binary, fs::create_e::Trunc))
                {
                    EMP_RET_FALSE_IF_NOT(oFile.write(sSource));
                    return true;
                }
            }

            return false;
        };

        auto const& l_copy_renderlet = [&a_sPath = m_sPath](renderlet_c& a_rRenderlet, c_astring& a_sDirectory) -> bool
        {
            bool bSaved = false;
            astring sOldPath(STR_AFORMAT("#0/#1", a_sPath, a_rRenderlet.m_sMother));
            astring sNewPath(STR_AFORMAT("#0/#1/#2", a_sPath, a_sDirectory, a_rRenderlet.m_sMother));

            if (!::CopyFileA(astring(sOldPath).cstr(), astring(sNewPath).cstr(), false))
            {
                if (emp::ast::get_errno() != 0)
                {
                    EMP_LOG_INFO("FAILED TO COPY FILE #0 -> #1", sOldPath, sNewPath);
                }
            }
            else
            {
                EMP_LOG_INFO("MOVED COPY #0", sOldPath);
                bSaved = true;
            }

            if (bSaved)
            {
                astring sPatternPath;
                astring sPatternFilename;
                astring sPatternExtension;
                str::extract_PFE(a_rRenderlet.m_sMother, sPatternPath, sPatternFilename, sPatternExtension);

                cnt::vector<astring> vImgToRename;

                auto const l_move_images = [](c_astring& a_sPath, c_astring& l_sDirectory, c_astring& a_sFilename, c_astring& a_sExtension, vector<astring>& a_vToRename)
                {
                    a_vToRename.resize(0);
                    astring sPattern = STR_AFORMAT("#0_s*.#1", a_sFilename, a_sExtension);
                    EMP_LOG_INFO("PATTERN #0", sPattern);
                    if (fs::list(a_sPath, sPattern, a_vToRename, false))
                    {
                        for (size_t stIndex = 0; stIndex < a_vToRename.count(); ++stIndex)
                        {
                            astring sOld = STR_AFORMAT("#0/#1", a_sPath, a_vToRename.ref(stIndex));
                            astring sNew = STR_AFORMAT("#0/#1/#2", a_sPath, l_sDirectory, a_vToRename.ref(stIndex));

                            if (!CopyFileA(astring(sOld).cstr(), astring(sNew).cstr(), false))
                            {
                                if (emp::ast::get_errno() != 0)
                                {
                                    EMP_LOG_INFO("FAILED TO COPY #0 -> #1", sOld, sNew);
                                }
                            }
                            else
                            {
                                EMP_LOG_INFO("COPIED #0 -> #1", sOld, sNew);
                            }
                        }
                    }
                };

                l_move_images(a_sPath, a_sDirectory, sPatternFilename, ALITERAL("png"), vImgToRename);
                l_move_images(a_sPath, a_sDirectory, sPatternFilename, ALITERAL("jpeg"), vImgToRename);
            }

            return true;
        };

        auto const& l_move_renderlet = [&a_rstFileCount = m_stFileCount, &a_vsFiles = m_vsFiles, l_write_renderlet, &a_sPath = m_sPath, l_save_png](ose::compiler_c* a_pCompiler, renderlet_c& a_rRenderlet, c_astring& a_sDirectory) -> bool
        {
            bool bSaved = false;
            astring sOldPath(STR_AFORMAT("#0/#1", a_sPath, a_rRenderlet.m_sMother));
            astring sNewPath(STR_AFORMAT("#0/#1/#2", a_sPath, a_sDirectory, a_rRenderlet.m_sMother));

            if (a_rRenderlet.m_pProgram != nullptr &&
                tt::is_not_max(a_rRenderlet.m_pProgram->m_stAst))
            {
                if (l_write_renderlet(a_rRenderlet, a_sDirectory, true))
                {
                    if (fs::remove(sOldPath) == false)
                    {
                        EMP_LOG_INFO("FAILED TO REMOVE FILE #0", sOldPath);
                    }
                    else
                    {
                        EMP_LOG_INFO("REMOVED FILE #0", sOldPath);
                        bSaved = true;
                    }
                }
            }
            else
            {
                if (fs::rename(sOldPath, sNewPath) == false)
                {
                    EMP_LOG_INFO("FAILED TO MOVE FILE #0 -> #1", sOldPath, sNewPath);
                }
                else
                {
                    EMP_LOG_INFO("MOVED FILE #0", sOldPath);
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
                str::extract_PFE(a_rRenderlet.m_sMother, sPatternPath, sPatternFilename, sPatternExtension);

                cnt::vector<astring> vImgToRename;

                auto const l_move_images = [](c_astring& a_sPath, c_astring& l_sDirectory, c_astring& a_sFilename, c_astring& a_sExtension, vector<astring>& a_vToRename)
                {
                    a_vToRename.resize(0);
                    astring sPattern = STR_AFORMAT("#0_s*.#1", a_sFilename, a_sExtension);
                    EMP_LOG_INFO("PATTERN #0", sPattern);
                    if (fs::list(a_sPath, sPattern, a_vToRename, false))
                    {
                        for (size_t stIndex = 0; stIndex < a_vToRename.count(); ++stIndex)
                        {
                            astring sOld = STR_AFORMAT("#0/#1", a_sPath, a_vToRename.ref(stIndex));
                            astring sNew = STR_AFORMAT("#0/#1/#2", a_sPath, l_sDirectory, a_vToRename.ref(stIndex));
                            if (fs::rename(sOld, sNew) == false)
                            {
                                EMP_LOG_INFO("FAILED TO RENAME #0 -> #1", sOld, sNew);
                            }
                            else
                            {
                                EMP_LOG_INFO("RENAMED #0 -> #1", sOld, sNew);
                            }
                        }
                    }
                };

                l_move_images(a_sPath, a_sDirectory, sPatternFilename, ALITERAL("png"), vImgToRename);
                l_move_images(a_sPath, a_sDirectory, sPatternFilename, ALITERAL("jpeg"), vImgToRename);

            }

            if (a_pCompiler->m_vpPrograms.is_valid_index(a_rRenderlet.m_stProgram))
            {
                c_size stToDelete = a_rRenderlet.m_stProgram;
                a_rRenderlet.m_pProgram = nullptr;
                EMP_TT_MAX_VAR(a_rRenderlet.m_stProgram);
                EMP_RET_FALSE_IF_NOT(a_pCompiler->dealloc_program(stToDelete));
            }

            EMP_LOG_INFO("--------------------->>>>> LEFT : #0/#1", a_rstFileCount, a_vsFiles.count());
            return true;
        };

        int iSetNext = 0;


        auto const l_compute_genome = [l_load_renderlet](ose::compiler_c* const l_pCompiler, renderlet_c& l_rRenderlet, vector<astring> const& l_rvsFiles, vector<vector<astring>*>& l_vpvsGenomes, vector<vector<astring>*>& l_vpvsInstructions, size_t& stPoolGenCount) EMP_RETURN -> bool
        {
            c_size stCount = l_rvsFiles.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                l_rRenderlet.m_sFather = astring();
                l_rRenderlet.m_sMother = l_rvsFiles.ref(stIndex);

                vector<astring>* pvsGenomes = EMP_ALLOCATE(vector<astring>);
                vector<astring>* pvsInstructions = EMP_ALLOCATE(vector<astring>);

                size_t stGen = 0;
                while (true)
                {
                    l_load_renderlet(l_rRenderlet, l_rRenderlet.m_sMother, l_rRenderlet.m_sFather, false, true, stGen);
                    if (l_pCompiler->m_vpPrograms.is_valid_index(l_rRenderlet.m_stProgram))
                    {
                        astring sAsm;
                        if (l_rRenderlet.print_asm(*l_pCompiler, sAsm, false, *pvsInstructions) == false)
                        { EMP_LOG_INFO("Failed to print gen #0", l_rRenderlet.m_sMother); }

                        // EMP_LOG_INFO("GEN #0", stGen);
                        // EMP_LOG_INFO("#0", sAsm);
                        pvsGenomes->add(sAsm);
                        ++stGen;
                        ++stPoolGenCount;

                        c_size stToDelete = l_rRenderlet.m_stProgram;
                        l_rRenderlet.m_pProgram = nullptr;
                        EMP_TT_MAX_VAR(l_rRenderlet.m_stProgram);
                        EMP_RET_FALSE_IF_NOT(l_pCompiler->dealloc_program(stToDelete));
                    }
                    else
                    {
                        break;
                    }
                }

                l_vpvsGenomes.add(pvsGenomes);
                l_vpvsInstructions.add(pvsInstructions);
            }
            return true;
        };

        auto const l_release_genome = [](vector<vector<astring>*>& l_vpvsGenomes, vector<vector<astring>*>& l_vpvsInstructions)
        {
            c_size stCount = l_vpvsGenomes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                EMP_DEALLOCATE(l_vpvsGenomes.val(stIndex));
                EMP_DEALLOCATE(l_vpvsInstructions.val(stIndex));
            }
        };

        if (m_eMod == renderer_e::Genome)
        {
            vector<vector<astring>*> vpvsGenomes;
            vector<vector<astring>*> vpvsInstructions;
            
            size_t stPoolGenCount = 0;
            EMP_RET_FALSE_IF_NOT(l_compute_genome(m_pCompiler, m_Rendered, m_vsFiles, vpvsGenomes, vpvsInstructions, stPoolGenCount));

            c_size stCount = m_vsFiles.count();
            EMP_RET_FALSE_IF(stCount != vpvsGenomes.count());

            vector<astring> vsSortedGens;
            vector<size_t> vsSortedUsedGens;


            vector<size_t> vGenCount;
            {
                vector<astring> vsAllInst;
                vector<size_t> vsAllInstCount;
                EMP_LOG_INFO("INSTRUCTIONS");
                size_t stPoolInstructionCount = 0;
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    vector<astring>* pvsInstr = vpvsInstructions.val(stIndex);
                    for (size_t stInstr = 0; stInstr < pvsInstr->count(); ++stInstr)
                    {
                        bool bFound = false;
                        for (size_t stAll = 0; stAll < vsAllInst.count(); ++stAll)
                        {
                            if (str::cmp(vsAllInst.ref(stAll), pvsInstr->ref(stInstr)))
                            {
                                vsAllInstCount.ref(stAll)++;
                                ++stPoolInstructionCount;
                                bFound = true;
                                break;
                            }
                        }

                        if (bFound == false)
                        {
                            vsAllInst.add(pvsInstr->ref(stInstr));
                            vsAllInstCount.add(1);
                            ++stPoolInstructionCount;
                        }
                    }
                }

                EMP_LOG_INFO("INSTRUCTION MAP");
                for (size_t stIndex = 0; stIndex < vsAllInst.count(); ++stIndex)
                {
                    EMP_LOG_INFO("#0 used #1", vsAllInst.ref(stIndex), vsAllInstCount.ref(stIndex));
                }

                vector<astring> vsAllGens;
                vector<size_t> vsAllUsedGens;
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    vector<astring>* pvsGenome = vpvsGenomes.val(stIndex);
                    EMP_LOG_INFO("SORTING GEN #0/#1, COUNT #2, #3", stIndex, stCount, pvsGenome->count(), m_vsFiles.ref(stIndex));

                    if (pvsGenome->count() >= vGenCount.count())
                    {
                        vGenCount.resize(pvsGenome->count() + 1);

                    }

                    vGenCount.ref(pvsGenome->count())++;

                    for (size_t stCurrentGen = 0; stCurrentGen < pvsGenome->count(); ++stCurrentGen)
                    {
                        bool bGeneFound = false;
                        c_astring& sCurrentGen = pvsGenome->ref(stCurrentGen);
                        for (size_t stGene = 0; stGene < vsAllGens.count(); ++stGene)
                        {
                            if (emp::str::cmp(sCurrentGen, vsAllGens.ref(stGene)))
                            {
                                vsAllUsedGens.ref(stGene)++;
                                bGeneFound = true;
                                break;
                            }
                        }
                        if (bGeneFound == false)
                        {
                            vsAllGens.add(sCurrentGen);
                            vsAllUsedGens.add(1);
                        }
                    }
                }


                EMP_LOG_INFO("GENE PER FUNCTIONS");
                for (size_t stIndex = 0; stIndex < vGenCount.count(); ++stIndex)
                {
                    if (vGenCount.val(stIndex) > 0)
                    {
                        EMP_LOG_INFO("#0 rgens have #1 genes", vGenCount.val(stIndex), stIndex);
                    }
                }

                vector<size_t> vstSorted;
                for (size_t stIndex = 0; stIndex < vsAllGens.count(); ++stIndex)
                {
                    bool bAdded = false;
                    for (size_t stSort = 0; stSort < vstSorted.count(); ++stSort)
                    {
                        if (vsAllUsedGens.val(stIndex) > vsAllUsedGens.val(vstSorted.val(stSort)))
                        {
                            vstSorted.insert(stSort, stIndex);
                            bAdded = true;
                            break;
                        }
                    }

                    if (bAdded == false)
                    {
                        vstSorted.add(stIndex);
                    }
                }

                for (size_t stIndex = 0; stIndex < vstSorted.count(); ++stIndex)
                {
                    vsSortedGens.add(astring());
                    vsSortedUsedGens.add(0);
                }

                // for (size_t stIndex = 0; stIndex < vstSorted.count(); ++stIndex)
                // {
                //     c_size stSorted = vstSorted.val(stIndex);
                //     vsSortedGens.set(stSorted, vsAllGens.ref(stIndex));
                //     vsSortedUsedGens.set(stSorted, vsAllUsedGens.ref(stIndex));
                // }

                for (size_t stIndex = 0; stIndex < vstSorted.count(); ++stIndex)
                {
                    c_size stSorted = vstSorted.val(stIndex);
                    vsSortedGens.set(stIndex, vsAllGens.ref(stSorted));
                    vsSortedUsedGens.set(stIndex, vsAllUsedGens.ref(stSorted));
                }

                vector<astring> vsSizedGens;
                for (size_t stIndex = 0; stIndex < vsAllGens.count(); ++stIndex)
                {
                    bool bAdded = false;
                    for (size_t stSort = 0; stSort < vsSizedGens.count(); ++stSort)
                    {
                        if (vsAllGens.ref(stIndex).len() < vsSizedGens.ref(stSort).len())
                        {
                            vsSizedGens.insert(stSort, vsAllGens.ref(stIndex));
                            bAdded = true;
                            break;
                        }
                    }

                    if (bAdded == false)
                    {
                        vsSizedGens.add(vsAllGens.ref(stIndex));
                    }
                }

                // for (size_t stIndex = 0; stIndex < vsSizedGens.count(); ++stIndex)
                // {
                //     EMP_LOG_INFO("// #0 ------------------------------------------------", vsSizedGens.ref(stIndex).len());
                //     EMP_LOG_INFO("#0", vsSizedGens.ref(stIndex));
                // }



                EMP_LOG_INFO("//------------------------------------------------------------------------------");
                EMP_LOG_INFO("//------------------------------------------------------------------------------");
                EMP_LOG_INFO("#0 rgens", m_vsFiles.count());
                EMP_LOG_INFO("#0 functions (with duplicates)", stPoolGenCount);
                EMP_LOG_INFO("#0 distincts functions", vsAllGens.count());
                EMP_LOG_INFO("#0 instructions (with duplicates)", stPoolInstructionCount);
                EMP_LOG_INFO("#0 distincts instructions", vsAllInst.count());
                EMP_LOG_INFO("#0 average function count per rgen", (float)stPoolGenCount / (float)m_vsFiles.count());
                EMP_LOG_INFO("#0 average instruction count per rgen", (float)stPoolInstructionCount / (float) m_vsFiles.count());
                EMP_LOG_INFO("#0 average trait count per rgens", (float)(stPoolGenCount + stPoolInstructionCount) / (float) m_vsFiles.count());
                EMP_LOG_INFO("gene map:");


                EMP_RET_FALSE_IF_NOT(vstSorted.count() == vsAllGens.count());
            }


            for (size_t stIndex = 0; stIndex < vsSortedGens.count(); ++stIndex)
            {
                size_t stUsedCount = vsSortedUsedGens.val(stIndex);
                EMP_LOG_INFO("function #0 used in #1 rgens", stIndex, stUsedCount);
                // if (stUsedCount > 50)
                // {
                //     EMP_LOG_INFO("#0", vsSortedGens.ref(stIndex));
                // }
                //EMP_LOG_INFO("#0", vsAllGens.ref(stIndex));
            }


            EMP_LOG_INFO("//------------------------------------------------------");

            size_t stTmpCount = 0;
            size_t stAllCount = 0;

            EMP_LOG_INFO("VS_SORTED_GENS_COUNT #0", vsSortedGens.count());
            for (size_t stIndex = 0; stIndex < vsSortedGens.count(); ++stIndex)
            {
                //EMP_LOG_INFO("INDEX #0/#1", stIndex, vsAllGens.count());
                ++stTmpCount;
                //c_size stGen = vstSorted.val(stIndex);
                c_size stUsedCount = vsSortedUsedGens.val(stIndex);

                c_size stNext = stIndex + 1;
                //size_t stFinalUsed = 0;
                if (stNext >= vsSortedGens.count())
                {
                    //stFinalUsed = stUsedCount;
                    EMP_LOG_INFO("#0 functions used #1", stTmpCount, stUsedCount);
                    stAllCount += stTmpCount;
                    stTmpCount = 0;
                    continue;
                }
                else
                {
                    c_size stNextUsedCount = vsSortedUsedGens.val(stNext);
                    if (stUsedCount != stNextUsedCount)
                    {
                        //stFinalUsed = stUsedCount;
                        EMP_LOG_INFO("#0 functions used #1", stTmpCount, stUsedCount);
                        stAllCount += stTmpCount;
                        stTmpCount = 0;
                        continue;
                    }
                }
            }

            EMP_LOG_INFO("ST_ALL_COUNT #0", stAllCount);



            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                astring sPngPath;
                astring sPngFilename;
                astring sPngExtension;
                str::extract_PFE(m_vsFiles.ref(stIndex), sPngPath, sPngFilename, sPngExtension);

                astring sTraitFilename = STR_AFORMAT("#0/#1_traits.txt", m_sPath, sPngFilename);
                        // EMP_LOG_INFO("// ------------------------------------");
                        // EMP_LOG_INFO("// TRAITS : #0", sTraitFilename);

                {
                    fs::file_c oFile;
                    if (oFile.create(sTraitFilename, fs::file_e::Text, fs::create_e::Trunc))
                    {
                        vector<size_t> vstCurrentGens;
                        size_t stDupCount = 0;
                        vector<astring>* pvsGenes = vpvsGenomes.val(stIndex);
                        for (size_t stGene = 0; stGene < pvsGenes->count(); ++stGene)
                        {
                            c_astring& sGene = pvsGenes->ref(stGene);
                            //bool bFound = false;
                            for (size_t stAllGene = 0; stAllGene < vsSortedGens.count(); ++stAllGene)
                            {
                                if (str::cmp(sGene, vsSortedGens.ref(stAllGene)))
                                {
                                    bool bDup = false;
                                    for (size_t stDup = 0; stDup < vstCurrentGens.count(); ++stDup)
                                    {
                                        if (stAllGene == vstCurrentGens.val(stDup))
                                        {
                                            bDup = true;
                                            ++stDupCount;
                                            break;
                                        }
                                    }

                                    if (bDup == false)
                                    {
                                        vstCurrentGens.add(stAllGene);
                                    }
                                    //bFound = true;
                                    break;   
                                }
                            }
                        }

                        EMP_RET_FALSE_IF(pvsGenes->count() != (vstCurrentGens.count() + stDupCount));
                        
                        vector<size_t> vstCurrentSorted;
                        for (size_t stToSort = 0; stToSort < vstCurrentGens.count(); ++stToSort)
                        {
                            bool bAdded = false;
                            size_t stGene = vstCurrentGens.val(stToSort);
                            for (size_t stSort = 0; stSort < vstCurrentSorted.count(); ++stSort)
                            {
                                if (vsSortedUsedGens.val(stGene) > vsSortedUsedGens.val(vstCurrentSorted.val(stSort)))
                                {
                                    vstCurrentSorted.insert(stSort, stGene);
                                    bAdded = true;
                                    break;
                                }
                            }

                            if (bAdded == false)
                            {
                                vstCurrentSorted.add(stGene);
                            }
                        }


                        {
                            for (size_t stLookup = 0; stLookup < pvsGenes->count(); ++stLookup)
                            {
                                bool bFound = false;
                                for (size_t stSorted = 0; stSorted < vstCurrentSorted.count(); ++stSorted)
                                {
                                    c_size stCurrent = vstCurrentSorted.val(stSorted);
                                    if (str::cmp(pvsGenes->ref(stLookup), vsSortedGens.ref(stCurrent)))
                                    {
                                        bFound = true;
                                        break;
                                    }
                                }
                                EMP_RET_FALSE_IF_NOT(bFound);
                            }
                        }

                        size_t stFunctionCount = 0;
                        size_t stIntrinsicCount = 0;

                        astring sGenes;
                        size_t EMP_TT_MAX_VAR(stPrevGene);
                        for (size_t stGene = 0; stGene < vstCurrentSorted.count(); ++stGene)
                        {
                            c_size stCurrent = vstCurrentSorted.val(stGene);

                            bool bFound = false;
                            for (size_t stLookup = 0; stLookup < pvsGenes->count(); ++stLookup)
                            {
                                if (str::cmp(pvsGenes->ref(stLookup), vsSortedGens.ref(stCurrent)))
                                {
                                    bFound = true;
                                    break;
                                }
                            }

                            EMP_RET_FALSE_IF_NOT(bFound);
                            EMP_RET_FALSE_IF(stCurrent == stPrevGene);
                            stPrevGene = stCurrent;

                            char sPrintGen[5];
                            sprintf(sPrintGen, "%04d", stCurrent);
                            sGenes.append_cstr(sPrintGen);
                            sGenes.append_cstr(" ");
                            ++stFunctionCount;
                        }

                        vector<astring> vsSortedInstructions;
                        sort_files(*(vpvsInstructions.val(stIndex)), vsSortedInstructions);
                        astring sInstructions;
                        for (size_t stInstr = 0; stInstr < vsSortedInstructions.count(); ++stInstr)
                        {
                            sInstructions.append_string(vsSortedInstructions.ref(stInstr));
                            sInstructions.append_string(astring(ALITERAL(" ")));
                            ++stIntrinsicCount;
                        }
                        char sPrintRGEN[5];
                        sprintf(sPrintRGEN, "%04d", stIndex);

                        astring sTraits;
                        sTraits.append_string(STR_AFORMAT("nft drop: rgen #0/2047\n\n", stIndex));
                        sTraits.append_string(astring(ALITERAL("#random #meta-#generative #art\n\n")));

                        sTraits.append_string(astring(ALITERAL("\n\n\n")));
                        sTraits.append_cstr(sPrintRGEN);
                        sTraits.append_string(astring(ALITERAL("\n\n\n")));


                        sTraits.append_string(astring(ALITERAL("https:://rgen.art/s0/")));
                        sTraits.append_cstr(sPrintRGEN);
                        sTraits.append_string(astring(ALITERAL("\n\n\n")));

                        sTraits.append_string(STR_AFORMAT("properties : #0\n\n", stIntrinsicCount + stFunctionCount));

                        sTraits.append_string(STR_AFORMAT("#0 FUNC: #1\n\n\n", stFunctionCount, sGenes));

                        sTraits.append_string(STR_AFORMAT("#0 INTR: #1\n\n\n", stIntrinsicCount, sInstructions));

                        sTraits.append_string(astring(ALITERAL("MAP:\n")));
                        for (size_t stGeneIndex = 0; stGeneIndex < vstCurrentSorted.count(); ++stGeneIndex)
                        {
                            c_size stCurrent = vstCurrentSorted.val(stGeneIndex);

                            sTraits.append_string(astring(ALITERAL("//--------------------------------------------------\n")));
                            sTraits.append_string(STR_AFORMAT("//GENE #0\n", stCurrent));
                            sTraits.append_string(STR_AFORMAT("#0\n", vsSortedGens.ref(stCurrent)));
                        }

                        EMP_RET_FALSE_IF_NOT(oFile.write(sTraits));
                    }
                }
            }

            l_release_genome(vpvsGenomes, vpvsInstructions);
            int *fuck = nullptr;
            *fuck = 0;
        }
        else if (m_eMod == renderer_e::Rename)
        {
            c_size stCount = m_vsFiles.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                m_Rendered.m_sFather = astring();
                m_Rendered.m_sMother = m_vsFiles.ref(stIndex);
                EMP_LOG_INFO("RENAMING #0", m_Rendered.m_sMother);
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, false, emp::tt::max<size_t>::value);
                EMP_RET_FALSE_IF_NOT(l_write_renderlet(m_Rendered, ALITERAL(""), true));
                //EMP_LOG_INFO("RENAMED #0", m_Rendered.m_sMother);
                c_size stToDelete = m_Rendered.m_stProgram;
                m_Rendered.m_pProgram = nullptr;
                EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                EMP_RET_FALSE_IF_NOT(m_pCompiler->dealloc_program(stToDelete));
            }
        }
        else if (m_eMod == renderer_e::Duplicate)
        {
            c_size stCount = m_vsFiles.count();
            vector<astring> vsAsms;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                m_Rendered.m_sFather = astring();
                m_Rendered.m_sMother = m_vsFiles.ref(stIndex);
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, true, emp::tt::max<size_t>::value);
                astring sAsm;
                vector<astring> vsInstructions;
                if (m_Rendered.print_asm(*m_pCompiler, sAsm, false, vsInstructions) == false)
                { EMP_LOG_INFO("Failed to print asm #0", m_Rendered.m_sMother); }
                //EMP_LOG_INFO("---------------------------------------------------");
                //EMP_LOG_INFO("#0", sAsm);
                vsAsms.add(sAsm);
            }



            vector<vector<astring>*> vpLines;
            vector<vector<astring>*> vpInstructions;


//#define EMP_COMPARE_LINE
//#define EMP_COMPARE_FILE
#define EMP_COMPARE_GENE
#ifdef EMP_COMPARE_LINE
            auto const& l_split_lines = [](c_astring& l_sFrom, vector<astring>& l_vsLines)
            {
               // EMP_LOG_INFO("SPLIT_LINES");
                c_size stCount = l_sFrom.len();
                size_t stLast = 0;
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    if (l_sFrom.get_char(stIndex) == '\n')
                    {
                        astring sLine = l_sFrom;
                        c_size stTrimCount = stIndex - stLast;
                        if (stTrimCount > 0)
                        {
                            if (stLast > 0)
                            {
                                sLine.trim_count(stLast, stTrimCount);
                            }
                            else if ((stCount - stLast) > 0)
                            {
                                sLine.trim_right(stCount - stLast);
                            }
                            if (sLine.len() > 0)
                            {
                                l_vsLines.add(sLine);
                               // EMP_LOG_INFO("LINE: '#0'", sLine);
                            }
                            stLast = stIndex + 1;
                        }
                    }
                }
            };
                    auto const& l_is_dup = [](c_astring&, c_astring&, vector<astring>& l_rvsLeft, vector<astring>& l_rvsRight) EMP_RETURN -> bool
        {

            vector<astring>* pLeft = &l_rvsLeft;
            vector<astring>* pRight = &l_rvsRight;
            if (l_rvsLeft.count() <= l_rvsRight.count())
            {
                pLeft = &l_rvsLeft;
                pRight = &l_rvsRight;
            }

            c_size stLeftCount = pLeft->count();
            // c_size stRightCount = pRight->count();

            size_t stSame = 0;
//                size_t stBackup = 0;
            for (size_t stLeft = 0; stLeft < pLeft->count();)
            {
                bool bSame = false;
                for (size_t stRight = 0; stRight < pRight->count();)
                {
                    if (str::cmp(pLeft->ref(stLeft), pRight->ref(stRight)))
                    {
                        pLeft->remove(stLeft);
                        pRight->remove(stRight);
                        ++stSame;
                        bSame = true;
                        break;
                    }
                    ++stRight;
                }
                if (bSame == false)
                {
                    ++stLeft;
                }
            }

            float fSame = stSame > 0 ? (float)100.0f * (float)stSame / (float)stLeftCount : 0;
            // float fSameRight = stSame > 0 ? (float)100.0f * (float)stSame / (float)stRightCount : 0;
//                if (fSame > 0.0f)
//              { EMP_LOG_INFO("SAME #0", fSame); }

            //c_size stTmp = (size_t)((float)stRightCount * 0.9f);
            return fSame >= 98.0f;// && (stTmp <= stLeftCount);// && fSameRight >= 80.0f;
        };

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            vector<astring>* pLines = EMP_ALLOCATE(vector<astring>);
            l_split_lines(vsAsms.ref(stIndex), *pLines);
            vpLines.add(pLines);
        }
#endif


#ifdef EMP_COMPARE_FILE
        auto const& l_is_dup = [](c_astring& l_sLeft, c_astring& l_sRight, vector<astring>&, vector<astring>&) EMP_RETURN -> bool
        {
            return str::cmp(l_sLeft, l_sRight);
        };
#endif


#ifdef EMP_COMPARE_GENE
        size_t stPoolGenCount = 0;
        EMP_RET_FALSE_IF_NOT(l_compute_genome(m_pCompiler, m_Rendered, m_vsFiles, vpLines, vpInstructions, stPoolGenCount));
        auto const& l_is_dup = [](c_astring& , c_astring& , vector<astring>& l_rvsLeft, vector<astring>& l_rvsRight) EMP_RETURN -> bool
        {
            c_size stLeftCount = l_rvsLeft.count();
            c_size stRightCount = l_rvsRight.count();

            size_t stMatchCount = 0;
            for (size_t stLeft = 0; stLeft < stLeftCount; ++stLeft)
            {
                for (size_t stRight = 0; stRight < stRightCount; ++stRight)
                {
                    if (str::cmp(l_rvsLeft.val(stLeft), l_rvsRight.val(stRight)))
                    {
                        ++stMatchCount;
                        break;
                    }
                }
            }

            return (stMatchCount >= stLeftCount);// && (stLeftCount == stRightCount);//(mat::maxof(stLeftCount, 1) - 1);
        };
#endif


            size_t stDuplicateFolder = 0;
            astring sDuplicateFolder;
            EMP_RET_FALSE_IF(vsAsms.count() != stCount);
            vector<bool> vbDups;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { vbDups.add(false); }


            vector<vector<astring>*> vpDuplicates;

            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_astring& sAsm = vsAsms.ref(stIndex);
                EMP_LOG_INFO("LOOKING DUPLICATE #0/#1", stIndex, stCount);
                if (sAsm.len() > 0)
                {
                    vector<astring>* vsDuplicates = EMP_ALLOCATE(vector<astring>);
                    vsDuplicates->add(m_vsFiles.ref(stIndex));
                    for (size_t stNext = 0; stNext < stCount; ++stNext)
                    {
                        if (stIndex != stNext && l_is_dup(sAsm, vsAsms.ref(stNext), *(vpLines.val(stIndex)), *(vpLines.val(stNext))) && (vbDups.val(stNext) == false))
                        {
                            vsDuplicates->add(m_vsFiles.ref(stNext));
                        }
                    }

                    if (vsDuplicates->count() > 1)
                    {
                        vector<astring>* vpLeft = vsDuplicates;

                        bool bDirFound = false;
                        for (size_t stDup = 0; stDup < vpDuplicates.count(); ++stDup)
                        {
                            vector<astring>* vpRight = vpDuplicates.val(stDup);
                            if ((vpLeft->count() == vpRight->count()) && (vpLeft->count() > 0))
                            {
                                bool bSame = true;
                                for (size_t stLeft = 0; stLeft < vpLeft->count(); ++stLeft)
                                {
                                    bool bLeftFound = false;
                                    for (size_t stRight = 0; stRight < vpRight->count(); ++stRight)
                                    {
                                        if (str::cmp(vpLeft->val(stLeft), vpRight->val(stRight)))
                                        {
                                            bLeftFound = true;
                                            break;
                                        }
                                    }

                                    if (bLeftFound == false)
                                    {
                                        bSame = false;
                                        break;
                                    }
                                }

                                if (bSame)
                                {
                                    EMP_DEALLOCATE(vsDuplicates);
                                    bDirFound = true;
                                    break;
                                }
                            }
                        }

                        if (bDirFound == false)
                        {
                            sDuplicateFolder = STR_AFORMAT("../duplicate/#0", stDuplicateFolder);
                            astring sCreateDir = STR_AFORMAT("#0/#1/", m_sPath, sDuplicateFolder);
                            ::mkdir(sCreateDir.cstr());
                            stDuplicateFolder++;
                            vpDuplicates.add(vsDuplicates);


                            for (size_t stDuplicate = 0; stDuplicate < vsDuplicates->count(); ++stDuplicate)
                            {
                                c_astring& sIndex = vsDuplicates->ref(stDuplicate);
                                renderlet_c oDummy;
                                oDummy.m_sMother = sIndex;
                                EMP_RET_FALSE_IF_NOT(l_copy_renderlet(oDummy, sDuplicateFolder));
                            }
                        }
                    }
                    else
                    {
                        EMP_DEALLOCATE(vsDuplicates);
                    }
                }
            }

            for (size_t stIndex = 0; stIndex < vpDuplicates.count(); ++stIndex)
            {
                EMP_DEALLOCATE(vpDuplicates.ref(stIndex));
            }
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                EMP_DEALLOCATE(vpLines.ref(stIndex));
            }

            int* kill = nullptr;
            *kill = 0;
        }
        else if (m_eMod == renderer_e::Thumbnail)
        {
            if (m_pCompiler->m_vpPrograms.is_valid_index(m_Rendered.m_stProgram))
            {
                if (m_Rendered.m_iRendered > 0)
                {
                    EMP_RET_FALSE_IF_NOT(m_pCompiler->reprune(m_Rendered.m_stProgram));
                    l_write_renderlet(m_Rendered, astring(ALITERAL("")), false);
                    l_save_png(m_Rendered);
                   // EMP_RET_FALSE_IF_NOT(l_move_renderlet(m_pCompiler, m_Rendered, astring(ALITERAL("../done"))));
                    m_iNextFile = 1;
                    if (m_stCurrentFile == 0)
                    {
                        int* fuck = nullptr;
                        *fuck = 0;
                    }
                }
            }
            else
            {
                m_iNextFile = 1;
            }
            if (m_iNextFile != 0)
            {
                if (m_pCompiler->m_vpPrograms.is_valid_index(m_Rendered.m_stProgram))
                {
                    c_size stToDelete = m_Rendered.m_stProgram;
                    m_Rendered.m_pProgram = nullptr;
                    EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                    EMP_RET_FALSE_IF_NOT(m_pCompiler->dealloc_program(stToDelete));
                }

                // while (true)
                // {
                    if (false == l_change_dna(m_Rendered.m_sMother, m_iNextFile, false))
                    { EMP_LOG_INFO("Failed to pick next file...."); }

                //     astring sPngDir;
                //     astring sPngFilename;
                //     astring sPngExtension;
                //     str::extract_PFE(m_Rendered.m_sMother, sPngDir, sPngFilename, sPngExtension);


                //     astring sPngPath = STR_AFORMAT("#0/#1_s#2_v#3.#4", m_sPath, sPngFilename, m_ResolveTarget.m_ui32Width, 0, astring(ALITERAL("png")));
                //     EMP_LOG_INFO("PNG_PATH #0", sPngPath);
                //     bool bExists = false;
                //     EMP_RET_FALSE_IF_NOT(fs::path_exists(sPngPath, bExists));
                //     if (bExists == false)
                //     { break; }
                // }

                m_Rendered.m_sFather = astring();
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, false, emp::tt::max<size_t>::value);

                EMP_RET_FALSE_IF_NOT(m_pCompiler->prune(m_Rendered.m_stProgram));
                m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
            }
            else if (m_bReset)
            {
                if (m_bNoReload == false)
                {
                    if (m_pCompiler->m_vpPrograms.is_valid_index(m_Rendered.m_stProgram))
                    {
                        c_size stToDelete = m_Rendered.m_stProgram;
                        m_Rendered.m_pProgram = nullptr;
                        EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                        EMP_RET_FALSE_IF_NOT(m_pCompiler->dealloc_program(stToDelete));
                    }

                    m_Rendered.m_sFather = astring();
                    l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, false, emp::tt::max<size_t>::value);
                    m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
                }
            }
        }
        else if (m_eMod == renderer_e::Edition)
        {
            if (m_iNextFile != 0)
            {
                if (m_pCompiler->m_vpPrograms.is_valid_index(m_Rendered.m_stProgram))
                {
                    c_size stToDelete = m_Rendered.m_stProgram;
                    m_Rendered.m_pProgram = nullptr;
                    EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                    EMP_RET_FALSE_IF_NOT(m_pCompiler->dealloc_program(stToDelete));
                }

                if (false == l_change_dna(m_Rendered.m_sMother, m_iNextFile, false))
                { EMP_LOG_INFO("Failed to pick next file...."); }

                m_Rendered.m_sFather = astring();
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, false, emp::tt::max<size_t>::value);
                l_write_renderlet(m_Rendered, astring(ALITERAL("")), true);


                if (m_pCompiler->m_vpPrograms.is_valid_index(m_Rendered.m_stProgram))
                {
                    c_size stToDelete = m_Rendered.m_stProgram;
                    m_Rendered.m_pProgram = nullptr;
                    EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                    EMP_RET_FALSE_IF_NOT(m_pCompiler->dealloc_program(stToDelete));
                }

                m_Rendered.m_sFather = astring();
                l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, false, emp::tt::max<size_t>::value);
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

                        // rOptions.instructionsPerImage = 0;
                        rOptions.primitivesPerImage = 0;
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

                    if (m_bSaveInstructionsProcessed)
                    {
                        // rOptions.instructionsPerImage = m_pRenderlet->m_uiInstructionsProcessed;
                        rOptions.primitivesPerImage = m_pRenderlet->m_uiPrimitivesProcessed;
                    }

                    c_float scaleX = rOptions.scaleX <= 0.0f ? 1.0f : rOptions.scaleX;
                    c_float scaleY = rOptions.scaleY <= 0.0f ? 1.0f : rOptions.scaleY;

                    rOptions.originX += (m_Rendered.m_origin.x + m_Rendered.m_delta.x) / scaleX;// * (2.0f / scaleX);
                    rOptions.originY += (m_Rendered.m_origin.y + m_Rendered.m_delta.y) / scaleY;// * (2.0f / scaleY);
                    rOptions.scaleX *= m_Rendered.m_fSizeX;
                    rOptions.scaleY *= m_Rendered.m_fSizeY;

                    m_Rendered.m_origin = v2f::zero();//m_v2DragPosition;
                    m_Rendered.m_delta = v2f::zero();
                    m_Rendered.m_fSizeX = 1.0f;
                    m_Rendered.m_fSizeY = 1.0f;
                }

                if (m_bWriteOptions || m_bResetOptions || m_bBakeOptions)
                {
                    l_write_renderlet(m_Rendered, astring(ALITERAL("")), true);
                }

                if (m_bNoReload == false)
                {
                    if (m_pCompiler->m_vpPrograms.is_valid_index(m_Rendered.m_stProgram))
                    {
                        c_size stToDelete = m_Rendered.m_stProgram;
                        m_Rendered.m_pProgram = nullptr;
                        EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                        EMP_RET_FALSE_IF_NOT(m_pCompiler->dealloc_program(stToDelete));
                    }

                    m_Rendered.m_sFather = astring();
                    l_load_renderlet(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, false, false, emp::tt::max<size_t>::value);
                    m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
                }
            }
            else if (m_bMove)
            {
                EMP_RET_FALSE_IF_NOT(l_move_renderlet(m_pCompiler, m_Rendered, m_sMoveFolder));
                iSetNext = 1;
            }
        }

        if (m_Rendered.m_pProgram != nullptr)
        {
            l_update_renderlet(m_Rendered);
            l_transform_renderlet(m_Rendered);
            l_render_renderlet(m_Rendered);
            if (m_bPng)
            {

                m_bPng = false;
                EMP_RET_FALSE_IF_NOT(l_save_png(m_Rendered));
            }
        }

        m_iNextFile = iSetNext;
        m_bReset = false;
        m_bResetOptions = false;
        m_bWriteOptions = false;
        m_bBakeOptions = false;
        m_stImMesh = 0;
        m_bMove = false;
        m_bNoReload = false;
        m_bRandomSeed = false;
        m_bSaveInstructionsProcessed = false;
        m_sMoveFolder = astring();

        EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform());
        return true;
    }
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN gui::return_e process_keyboard(app::c_event_s& a_rEvent);
    EMP_RETURN gui::return_e process_mouse(app::c_event_s& a_rEvent);
    //<< handler_i
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderer_c::renderer_c(void)
#if defined(EMP_XX_PROFILE)
 :
    m_pProfiler(ALITERAL("run"))
#endif
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
renderer_c::~renderer_c(void)
{}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::resize(void)
{
    EMP_RET_FALSE_IF_NOT(tbase1::resize());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool renderer_c::render(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    auto& rDevice = EMP_GRAPHICS;

//     EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(m_ResolveTarget));
//     EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 1, 0, 1}));


    // gl_framebuffer_s oFramebuffer = rDevice.current_framebuffer();
    // EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oFramebuffer.m_ui32Index, oFramebuffer.m_siW, oFramebuffer.m_siH));
    // EMP_RET_FALSE_IF_NOT(rDevice.clear_target({1, 0, 0, 1}));


    rDevice.set_blend(false);
    rDevice.set_depth_test(false);
    rDevice.set_depth_mask(false);
    rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
    rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
    rDevice.set_index_buffer(rDevice.quad_index_buffer());
    rDevice.set_program(rDevice.texture_program());
    rDevice.set_uniform(uniform_e::m44Transform, m44f::ident());
    rDevice.set_uniform(uniform_e::t2Texture_0, 0, m_ResolveTarget.m_ui32Texture);
    rDevice.set_uniform(uniform_e::v4Color, v4f({1, 1, 1, 1}));
    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rDevice.glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    rDevice.draw(gfx::primitive_e::Triangles);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN gui::return_e renderer_c::process_mouse(app::c_event_s& a_rEvent)
{
    gui::process_mouse_on_control(*this, a_rEvent);

    if (state().visible == false)
    { return gui::return_e::Unused; }

    gui::process_mouse_on_control(*this, a_rEvent);

    auto const& rMouse = a_rEvent.mouse;
    c_v2f v2RelPosition = rMouse.m_v2fPosition;

    // m_bLeftPressed = false;
    // m_bLeftReleased = false;

    if (state().hover)
    {
        if (rMouse.m_stToucheCount == 1)
        {
            if (rMouse.m_eButton == app::button_e::Left)
            {
                if (a_rEvent.mouse_pressed())
                {
                    if (m_bDragLeft == false)
                    {
                        m_bDragLeft = true;
                        m_v2DragPosition = v2RelPosition;
                        m_v2DragOrigin = v2RelPosition;
                        m_v2DragDelta = mat::v2f::zero();
                    }

                    return gui::return_e::Consumed;
                }
                else if (a_rEvent.mouse_released())
                {
                    if (m_bDragLeft)
                    {
                        m_bDragLeft = false;
                        m_v2DragPosition = v2RelPosition;
                        m_v2DragOrigin = v2f::add(m_v2DragOrigin, m_v2DragDelta);
                        m_v2DragDelta = mat::v2f::zero();

                        m_Rendered.m_delta = v2f::zero();
                    }
                    return gui::return_e::Consumed;
                }
                else if (a_rEvent.mouse_dragged())
                {
                    if (m_bDragLeft)
                    {
                        m_v2DragDelta = v2f::sub(v2RelPosition, m_v2DragOrigin);
                        m_Rendered.m_delta = v2f::sub(v2RelPosition, m_v2DragPosition);
                        m_v2DragPosition = v2RelPosition;
                        m_bReset = true;
                        m_bBakeOptions = true;
                    }
                    return gui::return_e::Consumed;
                }
            }
            else if (rMouse.m_eButton == app::button_e::WheelUp)
            {
                m_Rendered.m_fSizeX = m_Rendered.m_fSizeX + 0.1f;
                m_Rendered.m_fSizeY = m_Rendered.m_fSizeY + 0.1f;
                // m_Rendered.m_scaleOrigin = v2RelPosition;
                m_bReset = true;
                m_bBakeOptions = true;
                return return_e::Consumed;
            }
            else if (rMouse.m_eButton == app::button_e::WheelDown)
            {
                m_Rendered.m_fSizeX = tt::maxof<float>(m_Rendered.m_fSizeX - 0.1f, 0.01f);
                m_Rendered.m_fSizeY = tt::maxof<float>(m_Rendered.m_fSizeY - 0.1f, 0.01f);
                m_bReset = true;
                m_bBakeOptions = true;
                return return_e::Consumed;
            }
            else if (state().hover && a_rEvent.mouse_moved())
            {
            }
        }
    }

    // if (eButton == app::button_e::Left)
    // {
    //     if (siCount == 1)
    //     {
    //         if (bContains)
    //         {
    //             if (bPressed)
    //             {
    //             }
    //             else
    //             {
    //             }
    //         }
    //         else
    //         {
    //             // if (m_bDragLeft)
    //             // {
    //             //     m_bDragLeft = false;
    //             //     m_v2DragDelta = v2f::zero();
    //             //     m_Rendered.m_delta = v2f::zero();
    //             //     return gui::return_e::Consumed;
    //             // }
    //         }
    //     }
    // }

    // else if (a_rEvent.mouse_moved() || a_rEvent.mouse_dragged())
    // {
    // }

    return gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN gui::return_e renderer_c::process_keyboard(app::c_event_s& EMP_SILENT(a_rEvent))
{
    if (state().visible == false || state().enable == false)
    { return gui::return_e::Unused; }

    auto const& rKeyboard = a_rEvent.keyboard;
    c_bool bPressed = rKeyboard.m_bPressed;
    app::c_key_e eKey = rKeyboard.m_eKey;

    if (m_eMod == renderer_e::Edition)
    {
        if (rKeyboard.modified() == true && bPressed && rKeyboard.m_bRepeat == false)
        {
            if (rKeyboard.ctrl_only() && eKey == app::key_e::Alpha_R)
            {
                m_bReset = true;
                m_bResetOptions = true;
                m_bWriteOptions = true;
                m_bRandomSeed = false;
                return return_e::Consumed;
            }
            else if (rKeyboard.ctrl_only() && eKey == app::key_e::SemiColon)
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
            if (eKey == app::key_e::Left)
            {
                m_iNextFile = -1;
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Right)
            {
                m_iNextFile = 1;
                return return_e::Consumed;
            }
            else if (eKey ==app::key_e::Alpha_E)
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
            else if (eKey == app::key_e::Alpha_R)
            {
                m_bReset = true;
                m_bBakeOptions = true;
                m_bWriteOptions = true;
                m_bSaveInstructionsProcessed = true;
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Alpha_P)
            {
                m_bPng = true;
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_0)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../0"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_1)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../1"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_2)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../2"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_3)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../3"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_4)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../4"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_5)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../5"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_6)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../6"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_7)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../7"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_8)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../8"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Num_9)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../9"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Alpha_S)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../special"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Alpha_C)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../copy"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Alpha_B)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../bug"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Backspace)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../trash"));
                return return_e::Consumed;
            }
            else if (eKey == app::key_e::Alpha_U)
            {
                m_bMove = true;
                m_sMoveFolder = astring(ALITERAL("../unknown"));
                return return_e::Consumed;
            }
        }
    }

    return gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
rgen_c::rgen_c(void):
renderer()
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rgen_c::~rgen_c(void)
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool rgen_c::public_create(void)
{
    EMP_RET_FALSE_IF_NOT(renderer->create());

    renderer->transformation().position_(0).size_(2.0f, 2.0f);
    renderer->state().autosize.xy(false, false);
    renderer->state().dock_(emp::gui::dock_e::Fill);

    window().add_control(renderer);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void rgen_c::public_destroy(void)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool rgen_c::public_start(void)
{
    set_pause(false);
    // renderer->reset(true);
    // ASE_GAME.menu().docker.clear_controls();
    // ASE_GAME.menu().docker.add_control(&renderer->docker);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void rgen_c::public_stop(void)
{
    ASE_GAME.menu().docker.clear_controls();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool rgen_c::public_update(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

