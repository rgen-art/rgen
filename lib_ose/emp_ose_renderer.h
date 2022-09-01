//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_RENDERER_H
#define EMP_OSE_RENDERER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_error.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_model.h"

#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class compiler_c;
struct crossover_s;
struct mutation_s;
class pool_i;
class processor_c;
class program_c;
class stack_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//#define EMP_OSE_AUTOSCALE
//#define EMP_OSE_AUTOCOLOR

#if defined EMP_OSE_AUTOCOLOR
static constexpr const float s_fDefaultAlpha = 1.0f;
#else
static constexpr const float s_fDefaultAlpha = 0.0;
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, im_stack_c)
private:
    emp::cnt::vector<v4f> m_vv4Colors{};
    emp::cnt::vector<ui32> m_vuiColors{};
    v4f m_v4CurrentColor{};
    v4f m_v4RgbColor{};

    stack_t<m44f> m_Matrices{};
    vector<ui32> m_vuiMatrices{};
    m44f m_mCurrentMatrix{};

    v2f EMP_TT_MAX_VAR(m_v2Min);
    v2f EMP_TT_MAX_VAR(m_v2Max);
    public:
    float m_fMinScale = 0.1f;
private:
    array<vector<v4f>, 8, true> m_apv4Circles{};

public:
    static constexpr const float s_cfScale = 0.5f;
    static constexpr const float s_cfUnit = 0.1f;

public:
    im_stack_c(void)
    {
        /*for (size_t stIndex = 0; stIndex < m_apv4Circles.count(); ++stIndex)
        {
            EMP_DEALLOCATE(m_apv4Circles.ref(stIndex));
            m_apv4Circles.set(stIndex, nullptr);
        }*/
    }

public:
    void create(void)
    {
        c_float fScale = s_cfScale;
        //c_float fScale = 0.5f;
        c_size stCircleCount = m_apv4Circles.count();
        for (size_t stCircle = 0; stCircle < stCircleCount; ++stCircle)
        {
            //m_apv4Circles.set(stCircle, EMP_ALLOCATE(vector<v4f>));
            c_size stVertexCount = 1 << (4 + stCircle);
            for (size_t stIndex = 0; stIndex < stVertexCount; ++stIndex)
            {
                c_float fIndex = emp::ast::cast<float>(stIndex) *
                2 * emp::mat::pi<float>() / emp::ast::cast<float>(stVertexCount);
                m_apv4Circles.ref(stCircle).add({ fScale * emp::mat::cos(fIndex), fScale * emp::mat::sin(fIndex), 0, 1 });
            }
        }
    }

    void clear(void)
    {
        m_vv4Colors.resize(0);
        m_vuiColors.resize(0);
        m_v4CurrentColor = { 0, 0, 0, 1 };
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
        m_vv4Colors.add(m_v4CurrentColor);
        m_vuiColors.add(0);

        m_Matrices.destroy();
        m_vuiMatrices.resize(0);
        m44f::identity(m_mCurrentMatrix);
        m_Matrices.add(m_mCurrentMatrix);
        m_vuiMatrices.add(0);

        EMP_TT_MAX_VAR(m_v2Min);
        EMP_TT_MIN_VAR(m_v2Max);
    }

public:
//    void frame(void) { m_stTriangleCount = 0; }

    EMP_RETURN size_t count(void) const { return m_Matrices.count(); }

public:
    EMP_RETURN c_v4f& current_color(void) const { return m_v4RgbColor; }
    EMP_RETURN m44f const& current_matrix(void) const { return m_mCurrentMatrix; }

public:
    v2f size(void) const { return v2f::sub(m_v2Max, m_v2Min); }
    v2f min(void) const { return m_v2Min; }

private:
    EMP_RETURN v4f::tret to_rgb(c_float fH, c_float fS, c_float fV, c_float fA) const
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

    EMP_RETURN v4f::tret to_rgb(v4f::tparam a_v4Hsva) const
    { return to_rgb(a_v4Hsva.x, a_v4Hsva.y, a_v4Hsva.z, a_v4Hsva.w); }

private:
    EMP_RETURN bool visible(void) const;

public:
    void update_color(c_float fh, c_float fs, c_float fv, c_float fa)
    {
        c_float fNextH = m_v4CurrentColor.x + fh;
        c_float fNextS = m_v4CurrentColor.y + fs;
        c_float fNextV = m_v4CurrentColor.z + fv;
        c_float fNextA = m_v4CurrentColor.w + fa;
        c_float fFinalH = fNextH > 360.0f ? 0.0f : fNextH < 0.0f ? 360.0f : fNextH;
        m_v4CurrentColor = v4f::max(v4f::all(0), v4f::min({ 360, 1, 1, 1 }, { fFinalH, fNextS, fNextV, fNextA }));
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
    }

public:
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

    EMP_RETURN emp::ose::return_e push(void) { return push(false); }
    EMP_RETURN emp::ose::return_e append(void) { return push(true); }

private:
    EMP_RETURN emp::ose::return_e push(c_bool a_bAppend)
    {
        if (visible() == false)
        { return emp::ose::return_e::Break; }

        //if (a_bAppend == false)
        //{ return emp::ose::return_e::Continue; }

        if (m_vv4Colors.count() == 0 ||
            bool_equal(m_v4CurrentColor, m_vv4Colors.last_ref()) == false)
        {
            if (a_bAppend == false)
            {
                m_vv4Colors.add(m_v4CurrentColor);
                m_vuiColors.add(0);
            }
        }
        else if (a_bAppend == false)
        { m_vuiColors.last_ref()++; }

        if (m_Matrices.count() == 0 ||
            m44f::equals(m_mCurrentMatrix, m_Matrices.current()) == false)
        {
            if (a_bAppend == false)
            {
                m_Matrices.add(m_mCurrentMatrix);
                m_vuiMatrices.add(0);
            }
        }
        else if (a_bAppend == false)
        { m_vuiMatrices.last_ref()++; }

        return emp::ose::return_e::Continue;
    }

public:
    EMP_RETURN emp::ose::return_e pop(void)
    {
        {
            c_size stStackCount = m_vuiMatrices.count();
            if ((stStackCount > 0) == false)
            { return emp::ose::return_e::Error; }

            if ((m_Matrices.count() > 0) == false)
            { return emp::ose::return_e::Error; }

            m_mCurrentMatrix = m_Matrices.current();

            c_ui32 ui32LastStack = m_vuiMatrices.last_val();
            if (ui32LastStack > 0)
            { m_vuiMatrices.last_ref() = ui32LastStack - 1; }
            //if (ui32LastStack == 1)
            else
            {
                m_Matrices.pop();
                m_vuiMatrices.resize(stStackCount - 1);
            }
        }

        {
            c_size stStackCount = m_vuiColors.count();
            if ((stStackCount > 0) == false)
            { return emp::ose::return_e::Error; }

            c_size stCount = m_vv4Colors.count();
            if ((stCount > 0) == false)
            { return emp::ose::return_e::Error; }

            m_v4CurrentColor = m_vv4Colors.last_ref();
            m_v4RgbColor = to_rgb(m_v4CurrentColor);

            c_ui32 ui32LastStack = m_vuiColors.last_val();
            if (ui32LastStack > 0)
            { m_vuiColors.last_ref() = ui32LastStack - 1; }
            //if (ui32LastStack == 1)
            else
            {
                m_vv4Colors.resize(stCount - 1);
                m_vuiColors.resize(stStackCount - 1);
            }
        }

        return emp::ose::return_e::Continue;
    }

public:
    EMP_RETURN size_t byte_count(void) const
    {
        size_t stSize = 0;
        stSize += m_Matrices.byte_count();
        stSize += m_vuiMatrices.size() * sizeof(ui32);
        stSize += m_vv4Colors.size() * sizeof(v4f);
        stSize += m_vv4Colors.size() * sizeof(ui32);
        return stSize;
    }

public:

    EMP_RETURN float compute_size(void) const
    {
        c_float fWidth = m_v2Max.x - m_v2Min.x;
        c_float fHeight = m_v2Max.y - m_v2Min.y;

        c_float fNewWidth = 2 / fWidth;
        c_float fNewHeight = 2 / fHeight;
        c_float fSize = emp::tt::minof(fNewWidth, fNewHeight);
        return fSize;
    }

    EMP_RETURN size_t compute_target_triangle_count(c_f64 a_dArea)
    {
        c_float fAX = 0.1f;
        c_float fAY = 2048;
        c_float fBX = 20;
        c_float fBY = 8;

        c_float fA = (fBY - fAY) / (fBX - fAX);

        c_float fB = fAY - fA * fAX;

        float fArea = 0;
        if (emp::ast::try_cast(emp::tt::minof<double>(a_dArea, emp::tt::max<float>::value), fArea) == false)
        { return 0; }

        c_float fTarget = fA * fArea + fB;

        if (emp::tt::is_not_valid(fTarget))
        { return 0; }

        size_t stTarget = 0;
        if (emp::ast::try_cast(emp::tt::maxof<float>(0, fTarget), stTarget) == false)
        { return 0; }

        return stTarget;
    }

    EMP_RETURN bool emit_triangle(emp::gfx::buffer_t<ui16>& a_rIndices, emp::gfx::buffer_t<v3f, v4f>& a_rVertices, size_t& a_rstTriangleCount, f64& a_rdArea, size_t& a_rstMaxTriangles) // @@0 tristrip
    {
        c_v4f& c = current_color();
        m44f const& m = current_matrix();

        c_float fWidth = 0.86602540378f * s_cfScale;
        c_float fHeight0 = 1.0f * s_cfScale;
        c_float fHeight1 = -0.5f * s_cfScale;

        /*c_float fWidth = 0.5f;
        c_float fHeight0 = 0.5f;
        c_float fHeight1 = 0.5f;*/

        c_v4f v0 = m44f::mul_v4(m, {  0     , fHeight0, 0, 1 }); // @@0 v2f
        c_v4f v1 = m44f::mul_v4(m, { -fWidth, fHeight1, 0, 1 }); // @@0 v2f
        c_v4f v2 = m44f::mul_v4(m, {  fWidth, fHeight1, 0, 1 }); // @@0 v2f

        m_v2Min = v2f::min(v4f::xy(v0), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v1), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v2), m_v2Min);

        m_v2Max = v2f::max(v4f::xy(v0), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v1), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v2), m_v2Max);

        c_v2f v20 = v4f::xy(v0);
        c_v2f v21 = v4f::xy(v1);
        c_v2f v22 = v4f::xy(v2);

        emp::gfx::emit_triangle(a_rVertices.count(), a_rIndices);
        a_rVertices.add(v4f::xyz(v0), c);
        a_rVertices.add(v4f::xyz(v1), c);
        a_rVertices.add(v4f::xyz(v2), c);

        c_float fSize = compute_size();

        a_rdArea += ((v2f::length(v2f::sub(v21, v20)) + v2f::length(v2f::sub(v22, v20)) + v2f::length(v2f::sub(v22, v21))) / 2.0) * (fSize * fSize);
        a_rstMaxTriangles = emp::tt::maxof<size_t>(1, emp::tt::minof<size_t>(8192, compute_target_triangle_count(a_rdArea)));
        a_rstTriangleCount++;

        return a_rstTriangleCount < a_rstMaxTriangles;
    }

    EMP_RETURN bool emit_quad(emp::gfx::buffer_t<ui16>& a_rIndices, emp::gfx::buffer_t<v3f, v4f>& a_rVertices, size_t& a_rstTriangleCount, f64& a_rdArea, size_t& a_rstMaxTriangles) // @@0 tristrip
    {
        c_v4f& c = current_color();
        m44f const& m = current_matrix();

        c_float fHX = 0.70710678118f * s_cfScale;
        //c_float fHX = 0.5f;

        c_float fHY = fHX;// * EMP_GAME().gui_skin().resolution().current_ratio();
        c_v4f v0 = m44f::mul_v4(m, { -fHX, -fHY, 0, 1 }); // @@0 v2f
        c_v4f v1 = m44f::mul_v4(m, {  fHX, -fHY, 0, 1 });
        c_v4f v2 = m44f::mul_v4(m, { -fHX,  fHY, 0, 1 });
        c_v4f v3 = m44f::mul_v4(m, {  fHX,  fHY, 0, 1 });

        m_v2Min = v2f::min(v4f::xy(v0), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v1), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v2), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v3), m_v2Min);

        m_v2Max = v2f::max(v4f::xy(v0), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v1), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v2), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v3), m_v2Max);

        emp::gfx::emit_quad(a_rVertices.count(), a_rIndices);
        a_rVertices.add(v4f::xyz(v0), c);
        a_rVertices.add(v4f::xyz(v1), c);
        a_rVertices.add(v4f::xyz(v2), c);
        a_rVertices.add(v4f::xyz(v3), c);

        c_float fSize = compute_size();
        a_rdArea += (emp::mat::abs(v1.x - v0.x) * emp::mat::abs(v2.y - v0.y)) * (fSize * fSize);
        a_rstMaxTriangles = emp::tt::maxof<size_t>(1, emp::tt::minof<size_t>(8192, compute_target_triangle_count(a_rdArea)));
        a_rstTriangleCount += 2;

        return a_rstTriangleCount < a_rstMaxTriangles;
    }

    EMP_RETURN bool emit_circle(emp::gfx::buffer_t<ui16>& a_rIndices, emp::gfx::buffer_t<v3f, v4f>& a_rVertices, size_t& a_rstTriangleCount, f64& a_rdArea, size_t& a_rstMaxTriangles) // @@0 tristrip
    {
        c_v4f& c = current_color();
        m44f const& m = current_matrix();

        /*c_float fCurrentSize = [this]() EMP_RETURN -> float
        {
            c_float fSize = compute_size();
            return fSize > 0 ? fSize * 0.5f : 1;
        }();*/

        //c_size stCircleCount = m_apv4Circles.count() - 1;
        c_size stCurrentCircle = 4;//emp::mat::minof(stCircleCount, emp::ast::cast<size_t>(fCurrentSize * stCircleCount));

        c_ui16 ui16Count = emp::ast::cast<ui16>(m_apv4Circles.ref(stCurrentCircle).count());
        c_ui16 ui16Base = emp::ast::cast<ui16>(a_rVertices.count());
        c_v4f v4Zero = m44f::mul_v4(m, { 0, 0, 0, 1 }); // @@0 v2f

        a_rVertices.add(v4f::xyz(v4Zero), c);

        v2f v2Min = emp::tt::max<v2f>::value;
        v2f v2Max = emp::tt::min<v2f>::value;

        auto const& lEmitVertex = [this, &v2Min, &v2Max, &a_rVertices, &c, &m](c_ui16 a_ui16Index)
        {
            v4f v = m44f::mul_v4(m, m_apv4Circles.ref(stCurrentCircle).ref(a_ui16Index));
            a_rVertices.add(v4f::xyz(v), c);
            v2Min = v2f::min(v4f::xy(v), v2Min); // @@0 device move in mesh
            v2Max = v2f::max(v4f::xy(v), v2Max); // @@0 device move in mesh
        };

        for (ui16 ui16Index = 0; ui16Index < ui16Count; ++ui16Index)
        {
            lEmitVertex(ui16Index);
            a_rIndices.add(emp::ast::cast<ui16>(ui16Base + 0));
            a_rIndices.add(emp::ast::cast<ui16>(ui16Base + ui16Index + 1));
            a_rIndices.add(emp::ast::cast<ui16>(ui16Base + ui16Index + 2));
        }

        m_v2Min = v2f::min(v2Min, m_v2Min);
        m_v2Max = v2f::max(v2Max, m_v2Max);

        lEmitVertex(0);

        c_float fSize = compute_size();
        a_rdArea += (emp::mat::abs(v2Max.x - v2Min.x) * emp::mat::abs(v2Max.y - v2Min.y)) * (fSize * fSize);
        a_rstMaxTriangles = emp::tt::maxof<size_t>(1, emp::tt::minof<size_t>(8192, compute_target_triangle_count(a_rdArea)));
        a_rstTriangleCount += ui16Count;

        return a_rstTriangleCount < a_rstMaxTriangles;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
struct best_s
{
    emp::ose::program_c* m_pProgram = nullptr;
    size_t EMP_TT_MAX_VAR(m_stProgram);
    astring m_sMother = astring();
    astring m_sFather = astring();
    float m_fRed = 0.0f;
    float m_fGreen = 0.0f;
    float m_fBlue = 0.0f;
    float m_fAlpha = 0.0;
    float m_fVarRed = 0.0f;
    float m_fVarGreen = 0.0f;
    float m_fVarBlue = 0.0f;
    float m_fVarAlpha = 0.0f;
    size_t m_stCount = 0;
    bool m_bBeauty = false;
    size_t m_stTriangleCount = 0;
    size_t m_stBeautyCount = 0;
    size_t m_stAllCount = 0;
    emp::tim::time_c m_Stamp = emp::tim::time_c();
    size_t m_stCrossCount = 0;
    size_t m_stMutationCount = 0;
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_CLASS(renderlet_c)
public:
    astring m_sMother{};
    astring m_sFather{};

    emp::ose::program_c* m_pProgram = nullptr;
    size_t EMP_TT_MAX_VAR(m_stProgram);

    EMP_POINTER(emp::ose::stack_c) m_pStack{};
    EMP_POINTER(emp::ose::im_stack_c) m_pImStack{};
    EMP_POINTER((vector<std::tuple<emp::gfx::buffer_t<ui16>, emp::gfx::buffer_t<v3f, v4f>>*>)) m_pvpImMeshes{};

    emp::tim::time_c m_Stamp{};
    m44f m_m44Transform{};
    m44f m_m44PreviousTransform{};

    ui32 m_ui32Steps = 0;
    ui32 m_ui32Processed = 0;

    float m_fCompleted = 0;
    float m_fUnit = im_stack_c::s_cfUnit;

    v3f m_v3Scale = {0, 0, 0};
    v3f m_v3Rotate = {0, 0, 0};
    v3f m_v3Translate = {0, 0, 0};
    v3f m_v3Size = {0, 0, 0};

    v3f m_v3PreviousScale = {0, 0, 0};
    v3f m_v3PreviousRotate = {0, 0, 0};
    v3f m_v3PreviousTranslate = {0, 0, 0};

    v4f m_v4BackgroundColor = {0, 0, 0, s_fDefaultAlpha};
    f64 m_dArea = 0;
    f64 m_dAllArea = 0;

    bool m_bCompleted = false;
    bool m_bAutoScale = true;
    bool m_bFailed = false;
    
    ui32 EMP_TT_MAX_VAR(m_uiTarget0);
    ui32 EMP_TT_MAX_VAR(m_uiTarget1);

    size_t m_stTriangleAllCount = 0;
    size_t m_stTriangleCount = 0;
    size_t m_stBeautyCount = 0;
    size_t m_stAllCount = 0;

    size_t m_stCurrentTarget = 0;
    size_t m_stAllocatedBytes = 0;
    size_t m_stAllocatedCount = 0;

    size_t m_stCrossCount = 0;
    size_t m_stMutationCount = 0;

    best_s m_Best{};

public:
    renderlet_c(void);
    ~renderlet_c(void);

    EMP_RETURN bool reset(c_astring& a_rsMother, c_astring& a_rsFather, c_float a_fRatio);
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_NOCOPY(class, renderer_c)
public:
    size_t m_stImMesh = 0;
    typedef std::tuple<emp::gfx::buffer_t<ui16>, emp::gfx::buffer_t<v3f, v4f>> mesh_d;
    vector<mesh_d*> m_vpImMeshes{};

    EMP_POINTER(compiler_c) m_pCompiler{};
    EMP_POINTER(processor_c) m_pProcessor{};
    EMP_POINTER(emp::mat::random_c) m_pRandom{};

    vector<mesh_d*>* m_pvpImMeshes = nullptr;
    emp::gfx::buffer_t<ui16>* m_pImMeshIndices = nullptr;
    emp::gfx::buffer_t<v3f, v4f>* m_pImMeshVertices = nullptr;

    renderlet_c* m_pRenderlet = nullptr;
    im_stack_c* m_pImStack = nullptr;
    stack_c* m_pStack = nullptr;

    vector<ui32> m_vui32Pixels{};
    vector<ui8> m_vui8Data{};

    map_t<ui32, size_t> m_muiPixels;

    bool m_bColor = false;

    astring m_sPath{};

    emp::tim::time_c m_RefreshDirectoryTime;
    map_t<astring, ui32> m_mInvalidPrograms;
    map_t<astring, ui32> m_mChangePrograms;

    emp::tim::time_c m_DeltaTime{};

    size_t m_stTriangleCount = 0;
    size_t m_stMaxTriangles = 0;
    double m_dArea = 0.0;

    vector<astring> const* m_pvsOse = nullptr;

    ui32 m_ui32MaxWidth = 8192;
    ui32 m_ui32MaxHeight = 8192;

    v4f m_v4ClearColor = {0, 0, 0, s_fDefaultAlpha};

private:
    ui32 m_ui32RenderTriangles;

public:
    EMP_RETURN ui32 render_triangle_count(void) const { return m_ui32RenderTriangles; }

public:
    constexpr static const ui32 sc_ui32SimuBytes = 1024 * 1024;
    constexpr static const ui32 sc_ui32SimuInstructions = 4096;
    constexpr static c_float sc_fSimuTime = 5.0f;
    constexpr static const ui32 sc_ui32SimuCount = 8;
    constexpr static const ui32 sc_ui32SimuWidth = 32;
    constexpr static const ui32 sc_ui32SimuHeight = 32;

    constexpr static const ui32 sc_ui32RenderBytes = 2024 * 1024 * 1024;
    constexpr static const ui32 sc_ui32RenderInstructions = 4096 * 1024;
    constexpr static c_float sc_fRenderTime = 120.0;
    constexpr static const ui32 sc_ui32BestCount = 32;

public:
    renderer_c(void);

public:
    EMP_RETURN bool create(void);
    EMP_RETURN return_e push(void);
    EMP_RETURN return_e pop(void);
    EMP_RETURN return_e process(program_c const& a_rProgram, stack_c& a_rStack,
    c_ui32 a_ui32User);

public:
#if defined EMP_UNIT_TEST
    EMP_RETURN bool unit_test(c_astring& a_sResourcePath, c_float a_fResolution, c_bool a_bKill, c_bool a_bLog, c_astring& a_sFile, c_si32 a_si32Skip);
#endif

public:
    void reset(void)
    {
        m_stTriangleCount = 0;
        m_stMaxTriangles = 4096;
    }
    
    EMP_RETURN bool update(emp::tim::time_c const& a_rDelta);

public:
    EMP_RETURN bool get_dnapath(astring& a_rsPath, astring& a_rsFilename);
    void get_filepath(c_astring& a_sFile, astring& a_rsFilepath);

public:
    void run(renderlet_c& a_rRenderlet, c_size a_stCount, c_float a_fSeconds, c_size a_stBytes);

    EMP_RETURN bool load(renderlet_c& a_rRenderlet, c_float a_fRatio, c_bool a_bInvalid);
    EMP_RETURN bool load(renderlet_c& a_rRenderlet, c_float a_fRatio, mutation_s const& a_rMutation, crossover_s const& a_rCross, c_bool a_bInvalid);

    EMP_RETURN bool load_valid(renderlet_c& a_rRenderlet, c_float a_fRatio);

    EMP_RETURN bool save(renderlet_c& a_rRenderlet, c_astring& a_sName, astring& a_rsBuffer, c_bool a_bLog, c_bool a_bOverwrite, c_bool a_bPatch);

    void clear_invalids(void) { m_mInvalidPrograms.clear(); }

    void destroy(void);

    void change(astring& a_sFile);

    void transform(renderlet_c& a_rRenderlet, c_v2f& a_v2Size);

    EMP_RETURN bool renderU(renderlet_c& a_rRenderlet, m44f const& a_rTransform, c_ui32 a_ui32W, c_ui32 a_ui32H);

    EMP_RETURN bool save(c_ui32 a_ui32Target, c_astring& a_sName);

private:
    void print_ose(renderlet_c& a_rRenderlet);

public:
    EMP_RETURN bool print_ose(renderlet_c& a_rRenderlet, astring& a_rsOutput);

public:
    EMP_RETURN bool is_beauty(renderlet_c& a_rRenderlet, best_s& a_rBest, c_bool a_bLog);

public:
    mesh_d* get_mesh(void)
    {
        mesh_d* pImMesh = nullptr;
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
        return pImMesh;
    }
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
