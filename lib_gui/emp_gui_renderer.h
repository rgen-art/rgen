//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_RENDERER_H
#define EMP_GUI_RENDERER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_control_i.h"

#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_model.h"

#include "emp_mem_pointer.h"

#include "emp_ose_error.h"

#include "emp_tim_time.h"

#include "emp_mat_random.h"

#include "emp_tt_limit.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { class gl_mesh_c; } }

namespace emp { namespace ose { class compiler_c; } }
namespace emp { namespace ose { class processor_c; } }
namespace emp { namespace ose { class program_c; } }
namespace emp { namespace ose { class stack_c; } }

namespace emp { namespace prf { class time_profiler_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

#define enum_name renderer_e
#define enum_type
#define enum_values\
    enum_value( Curate )\
    enum_value( Generate )\
    enum_value( Thumbail )\
    enum_value( Edition  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v4f::tret to_rgb(c_float fH, c_float fS, c_float fV, c_float fA);
EMP_RETURN v4f::tret to_rgb(v4f::tparam a_v4Hsva);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, im_stack_c)
private:
    emp::cnt::vector<v4f> m_vv4Colors;
//    emp::cnt::vector<ui32> m_vuiColors;
    v4f m_v4CurrentColor;
    v4f m_v4RgbColor;

    tx_stack_d m_Matrices;
//    emp::cnt::vector<ui32> m_vuiMatrices;
    tx_d m_mCurrentMatrix;

    v2f m_v2Min;
    v2f m_v2Max;
    float m_fMinSize;
    float m_fMinAlpha;
    emp::cnt::vector<v4f> m_v4Circle;

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


public:
    void clear(void)
    {
        m_vv4Colors.resize(0);
        m_v4CurrentColor = { 0, 0, 0, 1 };
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
        m_vv4Colors.add(m_v4CurrentColor);

        m_Matrices.destroy();
        tx_d::identity(m_mCurrentMatrix);
        m_Matrices.add(m_mCurrentMatrix);

        m_fMinSize = 1.0f;
        m_fMinAlpha = 1.0f;

        EMP_TT_MAX_VAR(m_v2Min);
        EMP_TT_MIN_VAR(m_v2Max);
    }

public:
    EMP_RETURN size_t count(void) const { return m_Matrices.count(); }

public:
    EMP_RETURN c_v4f& current_color(void) const
    { return m_v4RgbColor; }

    EMP_RETURN tx_d const& current_matrix(void) const
    { return m_mCurrentMatrix; }

public:
    v2f max(void) const { return m_v2Max;}//v2f::sub(m_v2Max, m_v2Min); }

    v2f min(void) const { return m_v2Min; }

private:

public:
    void set_min_size(c_float a_fValue)
    { m_fMinSize = a_fValue; }

    void set_min_alpha(c_float a_fValue)
    { m_fMinAlpha = a_fValue; }

public:
    EMP_RETURN bool visible(void) const
    {
        tx_d const& rCurrent = current_matrix();
        c_float fMinSizeX = m_fMinSize;
        //c_float fRatio = EMP_GUI_SKIN.resolution().current_ratio();
        //c_float fMinY = fMinX * fRatio;
        //c_float fMaxX = 1000000;
        //c_float fMaxY = fMaxX * fRatio;



        c_v3f vs = tx_d::transform_vector({ 1.0, 1.0, 0.0}, rCurrent); // @@0 v2f
        //c_v3f v1 = tx_d::transform_vector({ 0.0, 1.0, 0.0}, rCurrent); // @@0 v2f


        //c_v4f v0 = tx_d::mul_v4(rCurrent, {0, 0, 0, 1});
        //c_v4f v1 = tx_d::mul_v4(rCurrent, {1, 1, 0, 1});


        //c_float fSizeX = emp::mat::abs(v1.x - v0.x);
        //c_float fSizeY = emp::mat::abs(v1.y - v0.y);


        c_float length = emp::mat::sqr(vs.x * vs.x + vs.y * vs.y + vs.z * vs.z);

        //c_float fSizeAX = rCurrent.f00 * rCurrent.f00 + rCurrent.f01 * rCurrent.f01 + rCurrent.f02 * rCurrent.f02;
        //c_float fSizeAY = rCurrent.f10 * rCurrent.f10 + rCurrent.f11 * rCurrent.f11 + rCurrent.f12 * rCurrent.f12;

        if (length < fMinSizeX)
            return false;


        c_float fMinAlpha = m_fMinAlpha;
        auto& color = current_color();
        if (fMinAlpha < 0.0)
        {
            if (color.w < emp::mat::abs(fMinAlpha))
            { return false; }
        }
        else if (fMinAlpha > 0.0)
        {
        }
        return true;
    }

public:
    void set_color(c_float fh, c_float fs, c_float fv, c_float fa)
    {
        c_float fNextH = fh;
        c_float fNextS = fs;
        c_float fNextV = fv;
        c_float fNextA = (m_fMinAlpha > 0.0) ? emp::tt::maxof(m_fMinAlpha, fa) : fa;
        c_float fFinalH = fNextH > 360.0f ? 0.0f : fNextH < 0.0f ? 360.0f : fNextH;
        m_v4CurrentColor = v4f::max(v4f::zero(), v4f::min({ 360, 1, 1, 1 }, { fFinalH, fNextS, fNextV, fNextA }));
        m_v4RgbColor = to_rgb(m_v4CurrentColor);
    }

    void update_color(c_float fh, c_float fs, c_float fv, c_float fa)
    {
        set_color(m_v4CurrentColor.x + fh, m_v4CurrentColor.y + fs, m_v4CurrentColor.z + fv, m_v4CurrentColor.w + fa);
    }

public:
    void translate(c_float a_fX, c_float a_fY, c_float a_fZ)
    {
        tx_d m;
        tx_d::identity(m);
        tx_d::translate(m, { a_fX, a_fY, a_fZ }, m);
        tx_d::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    void scale(c_float a_fX, c_float a_fY, c_float a_fZ)
    {
        tx_d m;
        tx_d::identity(m);
        tx_d::scale(m, { a_fX, a_fY, a_fZ }, m);
        tx_d::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    void rotate(float const a_fZ)
    {
        tx_d m;
        tx_d::identity(m);
        tx_d::rotation_z(-a_fZ, m);
        tx_d::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    void flip(float const a_fX)
    {
        tx_d m;
        tx_d::identity(m);
        tx_d::flip(-a_fX, m);
        tx_d::mul(m_mCurrentMatrix, m, m_mCurrentMatrix);
    }

    EMP_RETURN emp::ose::return_e push(void) { return push(false); }
    EMP_RETURN emp::ose::return_e append(void) { return push(true); }

private:
    EMP_RETURN emp::ose::return_e push(c_bool a_bAppend);
public:
    EMP_RETURN emp::ose::return_e pop(void);

public:
    EMP_RETURN size_t byte_count(void) const
    {
        size_t stSize = 0;
        stSize += m_Matrices.byte_count();
        stSize += m_vv4Colors.size() * sizeof(ui32);
        return stSize;
    }

public:
    void emit_triangle(emp::gfx::buffer_t<ui16>& a_rIndices, emp::gfx::buffer_t<v2f, v4f>& a_rVertices) // @@0 tristrip
    {
        c_v4f& c = current_color();
        tx_d const& m = current_matrix();

        c_v4f v0 = tx_d::mul_v4(m, { 0, 0.5f, 0, 1 }); // @@0 v2f
        c_v4f v1 = tx_d::mul_v4(m, { -0.5f, -0.5f, 0, 1 }); // @@0 v2f
        c_v4f v2 = tx_d::mul_v4(m, { 0.5f, -0.5f, 0, 1 }); // @@0 v2f

        m_v2Min = v2f::min(v4f::xy(v0), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v1), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v2), m_v2Min);

        m_v2Max = v2f::max(v4f::xy(v0), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v1), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v2), m_v2Max);

        // @@0 refact
        emp::gfx::emit_triangle(a_rVertices.count(), a_rIndices);
        a_rVertices.add(v4f::xy(v0), c);
        a_rVertices.add(v4f::xy(v1), c);
        a_rVertices.add(v4f::xy(v2), c);
    }

    void init_circle(c_size a_stCount)
    {
        for (size_t stIndex = 0; stIndex < a_stCount; ++stIndex)
        {
            float const fIndex = emp::ast::cast<float>(stIndex) *
            2 * emp::mat::pi<float>() / emp::ast::cast<float>(a_stCount);
            m_v4Circle.add({ 0.5f * emp::mat::cos(fIndex), 0.5f * emp::mat::sin(fIndex), 0, 1 });
        }
    }

    void emit_circle(emp::gfx::buffer_t<ui16>& a_rIndices, emp::gfx::buffer_t<v2f, v4f>& a_rVertices) // @@0 tristrip
    {
        c_v4f& c = current_color();
        tx_d::type const& m = current_matrix();

        c_ui16 ui16Count = emp::ast::cast<ui16>(m_v4Circle.count());
        c_ui16 ui16Base = emp::ast::cast<ui16>(a_rVertices.count());
        c_v4f v4Zero = tx_d::mul_v4(m, { 0, 0, 0, 1 }); // @@0 v2f

        a_rVertices.add(v4f::xy(v4Zero), c);

        //size_t i = 0;
        //EMP_LOG_INFO("v#0 : #1 : #2", i, emp::mat::v4f::get_x(v4Start), emp::mat::v4f::get_y(v4Start)); ++i;

auto const& lEmitVertex = [this, &a_rIndices, &a_rVertices, &c, &m](c_ui16 a_ui16Index)
{
    v4f v = tx_d::mul_v4(m, m_v4Circle.ref(a_ui16Index));
    a_rVertices.add(v4f::xy(v), c);
    m_v2Min = v2f::min(v4f::xy(v), m_v2Min); // @@0 device move in mesh
    m_v2Max = v2f::max(v4f::xy(v), m_v2Max); // @@0 device move in mesh
};

        for (ui16 ui16Index = 0; ui16Index < ui16Count; ++ui16Index)
        {
            lEmitVertex(ui16Index);
            a_rIndices.add(emp::ast::cast<ui16>(ui16Base + 0));
            a_rIndices.add(emp::ast::cast<ui16>(ui16Base + ui16Index + 1));
            a_rIndices.add(emp::ast::cast<ui16>(ui16Base + ui16Index + 2));

            //EMP_LOG_INFO("v#0 : #1 : #2", i, emp::mat::v4f::get_x(v4Zero), emp::mat::v4f::get_y(v4Zero)); ++i;
            //EMP_LOG_INFO("v#0 : #1 : #2", i, emp::mat::v4f::get_x(v1), emp::mat::v4f::get_y(v1)); ++i;
            //EMP_LOG_INFO("v#0 : #1 : #2", i, emp::mat::v4f::get_x(v1), emp::mat::v4f::get_y(v1)); ++i;
        }

        //EMP_LOG_INFO("v#0 : #1 : #2", i, emp::mat::v4f::get_x(v4Zero), emp::mat::v4f::get_y(v4Zero)); ++i;
        //EMP_LOG_INFO("v#0 : #1 : #2", i, emp::mat::v4f::get_x(v4Start), emp::mat::v4f::get_y(v4Start)); ++i;

        lEmitVertex(0);
    }

    void emit_quad(emp::gfx::buffer_t<ui16>& a_rIndices, emp::gfx::buffer_t<v2f, v4f>& a_rVertices) // @@0 tristrip
    {
        c_v4f& c = current_color();
        tx_d const& m = current_matrix();

        c_float fHX = 0.5f;
        c_float fHY = fHX;// * EMP_GAME().gui_skin().resolution().current_ratio();
        c_v4f v0 = tx_d::mul_v4(m, { -fHX, -fHY, 0, 1 }); // @@0 v2f
        c_v4f v1 = tx_d::mul_v4(m, { fHX, -fHY, 0, 1 });
        c_v4f v2 = tx_d::mul_v4(m, { -fHX, fHY, 0, 1 });
        c_v4f v3 = tx_d::mul_v4(m, { fHX, fHY, 0, 1 });

        m_v2Min = v2f::min(v4f::xy(v0), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v1), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v2), m_v2Min);
        m_v2Min = v2f::min(v4f::xy(v3), m_v2Min);

        m_v2Max = v2f::max(v4f::xy(v0), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v1), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v2), m_v2Max);
        m_v2Max = v2f::max(v4f::xy(v3), m_v2Max);

        emp::gfx::emit_quad(a_rVertices.count(), a_rIndices);
        a_rVertices.add(v4f::xy(v0), c);
        a_rVertices.add(v4f::xy(v1), c);
        a_rVertices.add(v4f::xy(v2), c);
        a_rVertices.add(v4f::xy(v3), c);
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
    uint32_t instructionsPerImage;

    uint32_t trianglesPerFrame;
    uint32_t trianglesPerImage;

    uint32_t bytesPerFrame;
    uint32_t bytesPerImage;


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
        instructionsPerImage = 0;

        trianglesPerFrame = 1024;
        trianglesPerImage = 0;

        bytesPerFrame = 1024;
        bytesPerImage = 0;
    }

    void load(ose::compiler_c& a_rCompiler, ose::program_c& a_pProgram);
    void save(ose::compiler_c& a_rCompiler, ose::program_c& a_rProgram) const;
};

EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
struct best_s
{
    emp::ose::program_c* m_pProgram = nullptr;
    astring m_sMother = astring();
    astring m_sFather = astring();
    float m_fAlpha = 0.0;
    size_t m_stCount = 0;
    emp::tim::time_c m_Stamp = emp::tim::time_c();
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_CLASS(renderlet_c)
public:


public:
    astring m_sMother;
    astring m_sFather;

    emp::ose::program_c* m_pProgram;
    EMP_POINTER(emp::ose::stack_c) m_pStack;
    EMP_POINTER(emp::gui::im_stack_c) m_pImStack;
    EMP_POINTER(emp::gfx::buffer_t<v2f>) m_pBackgroundVertices;
    EMP_POINTER((vector<std::tuple<emp::gfx::buffer_t<ui16>, emp::gfx::buffer_t<v2f, v4f>>*>)) m_pvpImMeshes;
    emp::gfx::buffer_t<ui16>* m_pImMeshIndices;
    emp::gfx::buffer_t<v2f, v4f>* m_pImMeshVertices;

    emp::tim::time_c m_Stamp;
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

    bool m_bChanging;

    rgen_option_s m_Options;

    emp::mat::random_float_s m_RandomFloat;

    v2f m_origin;
    v2f m_delta;
    v2f m_scaleOrigin;
    float m_fSizeX;
    float m_fSizeY;

    bool m_bCompleted;
    bool m_bBeauty;
    bool m_bAllocated;
    bool m_bDealloc;


    int m_iRendered;
    int m_iFrame;
    constexpr static const ui32 sc_ui32TargetCount = 3;
    EMP_STATIC_ASSERT(renderlet_c::sc_ui32TargetCount >= 2);

    array<emp::gfx::target_s, sc_ui32TargetCount, true> m_aTargets;
    size_t m_stCurrentTarget;
    size_t m_stAllocatedBytes;
    size_t m_stAllocatedCount;
    size_t m_stPrimitiveCount;
    size_t m_stInstructionCount;


public:
    renderlet_c(void);
    ~renderlet_c(void);
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_NOCOPY_BASE(class,renderer_c, public, control_i)
private:


    control_i* m_pParent;
    state_data_s m_State;
    stencil_data_c m_Stencil;
    transform_data_s m_Transform;
    border_data_c m_Border;
    tx_d m_mIdentity;
    emp::cnt::vector<std::tuple<emp::gfx::buffer_t<ui16>, emp::gfx::buffer_t<v2f, v4f>>*> m_vpImMeshes;
    mutable size_t m_stImMesh;
    renderlet_c* m_pRenderlet;
    mutable size_t m_stRenderingRenderlet;
    float m_fMinScale;
    mutable vector<ui32> m_vui32Pixels;

    EMP_POINTER(emp::gfx::buffer_t<v2f, v2f>) m_pImQuadVertices;
    EMP_POINTER(emp::ose::compiler_c) m_pCompiler;
    EMP_POINTER(emp::ose::processor_c) m_pProcessor;

    emp::tim::time_c m_RefreshDirectoryTime;
    emp::tim::time_c m_RefreshSourceTime;
    emp::tim::time_c m_ThumbTime;
    map_t<astring, ui32> m_mInvalidPrograms;

    map_t<ui32, size_t> m_muiPixels;
    emp::gfx::target_s m_PngTarget;
    emp::gfx::target_s m_MsaaTarget0;
    emp::gfx::target_s m_MsaaTarget1;
    emp::gfx::target_s m_ResolveTarget;


    best_s m_Best;

    //astring m_sFilename;
    astring m_sMother;
    astring m_sFather;
    bool m_bProfile;
    bool m_bMutate;
    bool m_bSwitch;
    bool m_bLookup;
    bool m_bPng;
    bool m_bMove;
    bool m_bReset;
    bool m_bBakeOptions;
    bool m_bFirstBake;
    bool m_bResetOptions;
    bool m_bWriteOptions;
    bool m_bNoReload;
    bool m_bRandomSeed;

    astring m_sMoveFolder;


    array<renderlet_c, 8, true> m_aSimulated;
public:
    mutable renderlet_c m_Rendered;

private:
    size_t m_stCurrentSort;
    array<vector<best_s>, 2, true> m_avBests;
    vector<renderlet_c*> m_vpRenderlets;
    //mutable vector<renderlet_c*> m_vpFreeRenderlets;
    //mutable vector<renderlet_c*> m_vpRunningRenderlets;

    size_t m_stFileCount;
    vector<astring> m_vsFiles;
    vector<astring> m_vsToSortFiles;
    astring m_sPath;
    astring m_sWorkingDir;
    mutable astring m_sCurrentFilename;
    mutable int m_iNextFile;
    mutable size_t m_stNextTransition;
    mutable size_t m_stCurrentFile;
    std::random_device m_RandomDevice;
    emp::mat::random_float_s m_RandomFloat;

    //constexpr static const size_t sc_stRenderletCount = 3;


    renderer_e m_eMod;

    bool m_bDragLeft;
    bool m_bDragRight;

    bool m_bLeftPressed;
    bool m_bLeftReleased;
    bool m_bLeftDoubleClick;

    bool m_bRightPressed;
    bool m_bRightReleased;
    bool m_bRightDoubleClick;

    v2f m_v2DragPosition;
    v2f m_v2DragOrigin;
    v2f m_v2DragDelta;

    mutable float m_fTransitionTime;
    mutable float m_fTransitionDuration;
    float m_fTransitionDelta;
    emp::tim::time_c m_DeltaTime;

    rgen_option_s m_RgenOptions;
    EMP_XX_IF_PROFILE(EMP_POINTER(emp::prf::time_profiler_c) m_pProfiler;)

public:
    renderer_c(void);
    ~renderer_c(void);

public:
    EMP_RETURN bool create(void);
    void destroy(void);

public:
    //void set_filename(c_astring& a_sValue) { m_sFilename = a_sValue; }
    void set_profile(c_bool a_bValue) { m_bProfile = a_bValue; }
    void set_min_scale(c_float a_fValue) { m_fMinScale = a_fValue; }

public:
    EMP_RETURN emp::ose::return_e process(emp::ose::program_c const& a_pProgram, emp::ose::stack_c& a_rStack,
        c_ui32 a_ui32User);

    EMP_RETURN emp::ose::return_e append(void);
    EMP_RETURN emp::ose::return_e push(void);
    EMP_RETURN emp::ose::return_e pop(void);

    EMP_RETURN rgen_option_s& options() { return m_RgenOptions; }

public:
    //>> control_i
    EMP_RETURN state_data_s& state(void) { return m_State; }
    EMP_RETURN state_data_s const& state(void) const { return m_State; }

    EMP_RETURN transform_data_s& transform(void) { return m_Transform; }
    EMP_RETURN c_transform_data_s& transform(void) const { return m_Transform; }

    EMP_RETURN control_i* got_focus( c_focus_e, c_bool, c_bool, c_change_e) { return nullptr; }

    void focused(void) { }

    void unselect(bool const) { m_State.set_select(false); }

    void update(emp::tim::clock_c const& a_rClock);

    EMP_RETURN bool resize(void) { return true; }
    EMP_RETURN bool transform(tx_stack_d& a_rStack);

    void render(void) const;

    EMP_RETURN bool contains(control_i const* const) const { return false; }

    void set_parent(control_i* const a_pValue) { m_pParent = a_pValue; }
    EMP_RETURN control_i const* parent(void) const { return m_pParent; }
    EMP_RETURN control_i* parent(void) { return m_pParent; }

    EMP_RETURN emp::cnt::vector<control_i*> const* children(void) const { return nullptr; }
    //<< control_i

    //>> handler_i
    EMP_RETURN return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    EMP_RETURN bool resize_buffers(c_f64 a_dWidth, c_f64 a_dHeight);
    void destroy_buffers(void);
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

