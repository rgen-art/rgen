//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_CONTROL_I_H
#define EMP_GUI_CONTROL_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_handler_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { class font_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_DEBUG
#define EMP_GUI_RETURN_HOOK
#endif

#if defined EMP_GUI_RETURN_HOOK
EMP_RETURN return_e return_hook(c_return_e a_eValue);
EMP_RETURN return_e return_hook(emp::app::event_s const& a_rEvent, c_return_e a_eValue);
#define EMP_GUI_RETURN(x_Event, x_Value) return_hook(x_Event, x_Value)
#define EMP_GUI_RETURNE(x_Value) return_hook(x_Value)
#else
#define EMP_GUI_RETURN(x_Event, x_Value) x_Value
#define EMP_GUI_RETURNE(x_Value) x_Value
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_GUI_ALIGNMENT_E_NAME alignment_e
#define EMP_GUI_ALIGNMENT_E_VALUES\
    enum_value( TopLeft      )\
    enum_value( TopCenter    )\
    enum_value( TopRight     )\
    enum_value( MiddleLeft   )\
    enum_value( MiddleCenter )\
    enum_value( MiddleRight  )\
    enum_value( BottomLeft   )\
    enum_value( BottomCenter )\
    enum_value( BottomRight  )

#define enum_name EMP_GUI_ALIGNMENT_E_NAME
#define enum_values EMP_GUI_ALIGNMENT_E_VALUES
#define enum_type
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name border_e
#define enum_type
#define enum_values\
    enum_value( none   ,  0 << 0)\
    enum_value( Left   ,  1 << 0)\
    enum_value( Right  ,  1 << 1)\
    enum_value( Top    ,  1 << 2)\
    enum_value( Bottom ,  1 << 3)\
    enum_value( All    ,  0x0F)
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name change_e
#define enum_values\
    enum_value( none     )\
    enum_value( Mouse    )\
    enum_value( Wheel    )\
    enum_value( Keyboard )\
    enum_value( Code     )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_GUI_DOCK_E_NAME dock_e
#define EMP_GUI_DOCK_E_VALUES\
    enum_value( none   )\
    enum_value( Left   )\
    enum_value( Right  )\
    enum_value( Bottom )\
    enum_value( Top    )\
    enum_value( Fill   )

#define enum_name EMP_GUI_DOCK_E_NAME
#define enum_values EMP_GUI_DOCK_E_VALUES
#define enum_type
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name focus_e
#define enum_values\
    enum_value( none  )\
    enum_value( First )\
    enum_value( Last  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@1 refactor
#define EMP_GUI_RETURN_IF_USED(x_Call)\
do\
{\
    emp::gui::c_return_e eReturn = x_Call;\
    if (eReturn != emp::gui::return_e::Unused)\
    { return eReturn; }\
} while(0)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(class, control_i)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool outside(cpc_control_i a_pControl, c_v2f a_v2Size);
EMP_RETURN bool clipped(cpc_control_i a_pControl, c_v2f a_v2Size);
EMP_RETURN bool clipped(emp::cnt::shell<pc_control_i> const& a_rvpControls, c_v2f a_v2Size);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret control_to_client(c_m44f& a_mTransform, v2f::tparam a_v2Control);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret gui_to_gl(c_m44f& a_mTransform, v2f::tparam a_v2Client);
EMP_RETURN v2f::tret gl_to_gui(v2f::tparam a_v2Screen, c_m44f& a_rClient);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void draw_quad(c_v2f a_v2Position, c_float a_fSize, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire);
void draw_quad(c_v2f a_v2Position, c_v2f& a_v2Size, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire);
void draw_quad(c_m44f& a_rMatrix, c_v2f a_v2Size, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire);
void draw_quad_from_bounds(c_m44f& a_rMatrix, c_v2f a_v2LeftBottom, c_v2f a_v2RightTop, c_float a_fDepth, c_v4f a_v4Color, c_bool a_bWire); 


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//EMP_RETURN bool contains(cpc_control_i a_pContainer, cpc_control_i a_pContainee);
EMP_RETURN bool contains_mouse(c_m44f& a_mTransform, v2f::tparam a_v4Size, v2f::tparam a_v2Value);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool focus(cpc_control_i a_pValue);
EMP_RETURN bool parent_enable(cpc_control_i a_pParent);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <class T> using on_render_f = emp::fct::function_t<void(*)(T const&)>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(struct, skin_data_t)
public:
    astring name{};
    T const* data = nullptr;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(info_s)
public:
    EMP_FIELD_0( v2f   , position      , {0, 0} )
    EMP_FIELD( v2f   , size          , {0, 0} )
    EMP_FIELD( v2f   , function_size , {0, 0} )
    EMP_FIELD( float , xscale        , 1.0f   )
    EMP_FIELD( float , yscale        , 1.0f   )
    EMP_FIELD( float , scale         , 1.0f   )
    EMP_FIELD( bool  , expand_input  , false  )
    EMP_FIELD( bool  , expand_output , false  )
    EMP_FIELD( bool  , show_input    , false  )
    EMP_FIELD( bool  , show_output   , false  )
    EMP_FIELD( bool  , show_current  , false  )
    EMP_FIELD( bool  , xlog          , false  )
    EMP_FIELD( bool  , ylog          , false  )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(autosize_data_s)
public:
    EMP_FIELD( bool , x , false )
    EMP_FIELD( bool , y , false )

    auto& xy(c_bool a_bX, c_bool a_bY) { x = a_bX; y = a_bY; return *this; }
    auto& xy(c_bool a_bValue) { return xy(a_bValue, a_bValue); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(struct, transformation_data_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(border_data_s)
public:
    EMP_FIELD( v4f      , color  , { 0, 0, 0, 0 } )
    EMP_FIELD( border_e , border , border_e::All )

    void render(c_transformation_data_s& a_rTransform, c_float a_fDepth) const;
    void add_border(c_border_e a_eValue) { border = enum_from(border_e, enum_to(border) | enum_to(a_eValue)); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(raster_data_s)
public:
    EMP_FIELD( v4f, color, { 0, 0, 0, 0 } )

    void render(c_transformation_data_s& a_rTransform, c_float a_fDepth) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(layer_data_s)
public:
    raster_data_s raster;
    border_data_s border;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(color_data_s)
public:
    layer_data_s background{};
    layer_data_s foreground{};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(font_data_s)
public:
    EMP_FIELD( emp::gfx::font_c const* , pointer   , nullptr )
    EMP_FIELD( float                   , size      , 1       )
    EMP_FIELD( bool                    , autoscale , false   )

    EMP_RETURN float line_height(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(resolution_data_s)
private:
    float m_fScale = 1.1f;
    v2f m_v2Size = {0, 0};
    float m_fCurrent = 1;
    v2f m_v2Ratio = { 1, 1 };
    float m_fRatioXY = 1;
    v2f m_v2InvRatio = {1, 1};
    v2f m_v2ScaledRatio = {1, 1};

public:
    EMP_RETURN float scale(void) const { return m_fScale; }
    EMP_RETURN c_v2f& size(void) const { return m_v2Size; }
    EMP_RETURN float current(void) const { return m_fCurrent; }
    EMP_RETURN c_v2f& ratio(void) const { return m_v2Ratio; }
    EMP_RETURN float ratio_xy(void) const { return m_fRatioXY; }
    EMP_RETURN c_v2f& inv_ratio(void) const { return m_v2InvRatio; }
    EMP_RETURN c_v2f& scaled_ratio(void) const { return m_v2ScaledRatio; }

public:
    void refresh(void)
    {
        c_float fBaseWidth = 800.0f;
        c_float fBaseHeight = 600.0f;

        m_fCurrent = m_v2Size.x / m_v2Size.y;
        m_v2Ratio = v2f::div({fBaseWidth, fBaseWidth}, m_v2Size);
        m_fRatioXY = fBaseHeight / m_v2Size.y;
        m_v2InvRatio = v2f::div({1, 1}, m_v2Ratio);
        m_v2ScaledRatio = v2f::mul(m_v2Ratio, m_fScale);
    }

public:
    auto& scale_(float const a_fValue)
    {
        m_fScale = emp::tt::minof(10.0f, emp::tt::maxof(0.1f, a_fValue));
        refresh();
        return *this;
    }

    auto& size_(c_f64 a_fWidth, c_f64 a_fHeight)
    {
        m_v2Size.x = emp::ast::cast<float>(a_fWidth);
        m_v2Size.y = emp::ast::cast<float>(a_fHeight);
        refresh();
        return *this;
    }
};
EMP_TYPEDEF(resolution_data_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(padding_data_s)
public:
    EMP_FIELD( v2f                 , m_v2LeftTop     , { 0, 0 } )
    EMP_FIELD( v2f                 , m_v2RightBottom , { 0, 0 } )
    EMP_FIELD( bool                , scale        , true     )

    EMP_RETURN float left(void) const;
    EMP_RETURN float right(void) const;
    EMP_RETURN float top(void) const;
    EMP_RETURN float bottom(void) const;

    auto& left_(c_float a_fValue) { m_v2LeftTop.x = a_fValue; return *this; }
    auto& right_(c_float a_fValue) { m_v2RightBottom.x = a_fValue; return *this; }
    auto& top_(c_float a_fValue) { m_v2LeftTop.y = a_fValue; return *this; }
    auto& bottom_(c_float a_fValue) { m_v2RightBottom.y = a_fValue; return *this; }

    auto& x_(c_float a_fValue) { left_(a_fValue).right_(a_fValue); return *this; }
    auto& x_(c_float a_fLeft, c_float a_fRight) { left_(a_fLeft).right_(a_fRight); return *this; }

    auto& y_(c_float a_fValue) { top_(a_fValue).bottom_(a_fValue); return *this; }
    auto& y_(c_float a_fTop, c_float a_fBottom) { top_(a_fTop).bottom_(a_fBottom); return *this; }

    auto& xy_(c_float a_fX, c_float a_fY) { x_(a_fX).y_(a_fY); return *this; }

    EMP_RETURN v2f::tret left_top(void) const { return { left(), top() }; }
    EMP_RETURN v2f::tret right_bottom(void) const { return { right(), bottom() }; }
    EMP_RETURN v2f::tret xy(void) const { return { left() + right(), top() + bottom() }; }

    EMP_RETURN float x(void) const { return left() + right(); }
    EMP_RETURN float y(void) const { return top() + bottom(); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_STRUCT(spacing_data_s)
public:
    EMP_FIELD( v2f                      , size       , { 0, 0 } )

    EMP_RETURN float x(void) const;
    EMP_RETURN float y(void) const;

    auto& xy(c_float a_fValue) { size = {a_fValue, a_fValue }; return *this; }
    auto& xy(c_float a_fX, c_float a_fY) { size = {a_fX, a_fY}; return *this; }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
EMP_STRUCT(state_data_s)
public:
    EMP_FIELD(      bool            , visible       , true         )
    EMP_FIELD(      bool            , enable        , true         )
    EMP_FIELD(      bool            , hover         , false        )
    EMP_FIELD(      bool            , focus         , false        )
    EMP_FIELD(      bool            , select        , false        )
    EMP_FIELD(      bool            , check         , false        )
    EMP_FIELD(      bool            , navigable     , false        )
    EMP_FIELD(      bool            , focusable     , false        )
    EMP_FIELD(      bool            , hoverable     , true         )
    EMP_FIELD(      bool            , pushable      , false        )
    EMP_FIELD(      bool            , renderable    , true         )
    EMP_FIELD(      bool            , hover_through , false        )
    EMP_FIELD(      bool            , overlap       , false        )
    EMP_FIELD_MAX(  ui32            , index                        )
    EMP_FIELD(      dock_e          , dock          , dock_e::none )
    EMP_FIELD(      autosize_data_s , autosize                     )
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)
EMP_TYPEDEF(state_data_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
struct state_skin_t
{
    EMP_FIELD( T const* , normal , nullptr )
    EMP_FIELD( T const* , hover  , nullptr )
    EMP_FIELD( T const* , focus  , nullptr )
    EMP_FIELD( T const* , select , nullptr )
    EMP_FIELD( T const* , check  , nullptr )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN float depth(c_si32 a_si32Depth, c_float a_fDepth);
EMP_INLINE EMP_RETURN float depth(c_si32 a_si32Depth, c_float a_fDepth)
{ return -(emp::ast::cast<float>(a_si32Depth) * 0.01f + a_fDepth); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(transformation_data_s)
public:
    EMP_FIELD(m44f , matrix    , m44f::ident())
    EMP_FIELD(v2f  , position  , { 0, 0 } )
    EMP_FIELD(v2f  , size      , { 0, 0 } )
    EMP_FIELD(si32 , back_depth, 0        )
    EMP_FIELD(si32 , fore_depth, 0        )


public:
    transformation_data_s& position_(c_float a_fValue) { return position_(a_fValue, a_fValue); }
    transformation_data_s& position_(c_float a_fX, c_float a_fY) { position = {a_fX, a_fY}; return *this; }

    EMP_RETURN float x(void) const { return position.x; }
    transformation_data_s& x_(c_float a_fValue) { position.x = a_fValue; return *this; }

    EMP_RETURN float y(void) const { return position.y; }
    transformation_data_s& y_(c_float a_fValue) { position.y = a_fValue; return *this; }

    transformation_data_s& size_(c_float a_fValue) { return size_(a_fValue, a_fValue); }
    transformation_data_s& size_(c_float a_fW, c_float a_fH) { size = { a_fW, a_fH }; return *this; }

    EMP_RETURN float width(void) const { return size.x; }
    transformation_data_s& width_(c_float a_fValue) { size.x = a_fValue; return *this; }

    EMP_RETURN float height(void) const { return size.y; }
    transformation_data_s& height_(c_float a_fValue) { size.y = a_fValue; return *this; }
};
EMP_TYPEDEF(transformation_data_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename V>
EMP_NOCOPY(class, state_t)
public:
    typedef return_e (*pfunction_d)(V, T const, c_change_e);
    typedef emp::fct::function_t<pfunction_d> function_f;

public:
    T value = 0;
    EMP_FIELD( function_f , function, {} )
    V object{};
    change_e change = change_e::none;

public:
    state_t(void) = default;
    void reset(T const& a_tValue)
    {
        value = a_tValue;
        function = function_f();
        change = change_e::none;
    }
     
    void reset(V a_tObject, T const& a_tValue)
    {
        object = a_tObject;
        reset(a_tValue);
    }

    EMP_RETURN return_e set_state(V a_tObject, T const& a_rtValue, c_change_e a_eChange)
    {
        value = a_rtValue;
        change = a_eChange;
        if (function.is_valid())
        { return function(a_tObject, value, a_eChange); }
        return return_e::Unused;
    }

    EMP_RETURN bool is_valid(void) const
    { return function.is_valid(); }

    EMP_RETURN return_e set_state(T const& a_rtValue, c_change_e a_eChange)
    { return set_state(object, a_rtValue, a_eChange); }
};

template <typename T, typename V>
using c_state_t = state_t<T, V> const;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(color_s)
public:
    EMP_FIELD( v4f, rgba, { 0, 0, 0, 0 } )

    explicit color_s(c_ui32 a_ui32Value) :
        rgba(v4f::mul(
    {
        emp::ast::cast<float>((a_ui32Value & 0xFF000000) >> 24),
        emp::ast::cast<float>((a_ui32Value & 0x00FF0000) >> 16),
            emp::ast::cast<float>((a_ui32Value & 0x0000FF00) >> 8),
            emp::ast::cast<float>((a_ui32Value & 0x000000FF))
    },
            v4f::all(1.0f / 255.0f)))
    {}

    explicit color_s(v4f::tparam a_v4Value) :
        rgba(a_v4Value)
    {}

    color_s(c_ui8 ur, c_ui8 ug, c_ui8 ub, c_ui8 ua) :
        rgba(v4f::mul({ (float)ur, (float)ug, (float)ub, (float)ua }, v4f::all(1.0f / 255.0f)))
    {}

    color_s(c_float fr, c_float fg, c_float fb, c_float fa) :
        rgba({ fr, fg, fb, fa })
    {}

    EMP_RETURN float r(void) const { return rgba.x; }
    EMP_RETURN float g(void) const { return rgba.y; }
    EMP_RETURN float b(void) const { return rgba.z; }
    EMP_RETURN float a(void) const { return rgba.w; }

    EMP_RETURN c_v3f rgb(void) const { return v4f::xyz(rgba); }

    static EMP_RETURN c_v4f light(v4f::tparam a_v4Value);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(control_skin_s)
public:
    EMP_FIELD( c_color_s* , raster , nullptr )
    EMP_FIELD( c_color_s* , border , nullptr )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, stack_c)
public:
    stack_t<m44f> matrices{};
    stack_t<bool> enabled{};
    vector<si32> depths{};

    si32 depth = 0;
    si32 previous_depth = 0;

public:
    stack_c(void) = default;
    void push_depth(void)
    {
        ++depth;
        depths.add(depth);
    }

    void pop_depth(void)
    {
        c_size stCount = depths.count();
        if (stCount > 1)
        {
            si32& rsi32Depth = depths.ref(stCount - 2);
            rsi32Depth = emp::tt::maxof(rsi32Depth, depths.last_ref());
        }

        --depth;

        previous_depth = depths.last_ref();
        depths.remove_last();
    }

    void push_transform(transformation_data_s& a_rTransform, c_bool a_bEnabled)
    {
        //push_transform(position, depth, size, a_bEnable, a_rStack, matrix);
        m44f mModel;
        m44f::identity(mModel);

        c_v2f& v2Position = a_rTransform.position;
        m44f::translate(mModel, {v2Position.x, v2Position.y, 0}, mModel);

        push_matrix(mModel, a_bEnabled);

        a_rTransform.matrix = matrices.current();
    }

    void push_matrix(c_m44f& a_rMatrix, c_bool a_bEnabled)
    {
        matrices.push(a_rMatrix);
        enabled.push(a_bEnabled);
    }

    void pop_matrix(void)
    {
        matrices.pop();
        enabled.pop();
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(struct, depth_stacker_c) // @@0 _s
public:
    stack_c& stack;
    transformation_data_s* transform = nullptr;

public:
    depth_stacker_c(stack_c& a_rStack, transformation_data_s& a_rTransform): stack(a_rStack), transform(&a_rTransform)
    {
        transform->back_depth_(stack.depth);
        a_rStack.push_depth();
    }

    ~depth_stacker_c(void)
    {
        stack.pop_depth();
        transform->fore_depth_(stack.previous_depth);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(struct, matrix_stacker_c) // @@0 _s
public:
    stack_c& stack;
    transformation_data_s* transform = nullptr;

public:
    matrix_stacker_c(stack_c& a_rStack, transformation_data_s& a_rTransform, c_bool a_bEnable): stack(a_rStack), transform(&a_rTransform)
    { a_rStack.push_transform(a_rTransform, a_bEnable); }

    matrix_stacker_c(stack_c& a_rStack, c_m44f& a_rMatrix, c_bool a_bEnable) : stack(a_rStack)
    { a_rStack.push_matrix(a_rMatrix, a_bEnable); }

    ~matrix_stacker_c(void)
    { stack.pop_matrix(); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, control_i, public, handler_i)
private:
public:
    control_i(void) {}
    virtual ~control_i(void) = 0;

public:
#if defined EMP_XX_DEBUG
    virtual EMP_RETURN size_t debug_id(void) const { return 0; }
#endif

    virtual EMP_RETURN state_data_s& state(void) = 0;
    virtual EMP_RETURN c_state_data_s& state(void) const = 0;

    virtual EMP_RETURN transformation_data_s& transformation(void) = 0;
    virtual EMP_RETURN c_transformation_data_s& transformation(void) const = 0;

    virtual EMP_RETURN control_i* got_focus(c_focus_e a_eFocus, c_bool a_bSelect, c_bool a_bPush, c_change_e a_eChange) = 0;

    virtual void focused(void) = 0;
    virtual EMP_RETURN bool unselect(c_bool a_bTransmit) = 0;

    virtual EMP_RETURN bool update(emp::tim::clock_c const& a_rClock) = 0;
    virtual EMP_RETURN bool resize(void) = 0;
    virtual EMP_RETURN bool stack(stack_c& a_rStack) = 0;
    virtual EMP_RETURN bool transform(stack_c& a_rStack) = 0;
    virtual EMP_RETURN bool render(void) = 0;

    virtual void set_parent(pc_control_i a_pValue) = 0;
    virtual EMP_RETURN c_control_i* parent(void) const = 0;
    virtual EMP_RETURN control_i* parent(void) = 0;

    virtual EMP_RETURN vector<control_i*>& controls(void) = 0;
    virtual EMP_RETURN vector<control_i*> const& controls(void) const = 0;
};
EMP_TYPEDEF(control_i)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool foreach_control(cpc_control_i a_pValue, T const& a_rtValue)
{
    if (a_rtValue(a_pValue))
    { return true; }

    auto const& rvpChildren = a_pValue->controls();

    c_size stCount = rvpChildren.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (foreach_control(rvpChildren.val(stIndex), a_rtValue))
        { return true; }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool state_focus(cpc_control_i a_pValue)
{ return foreach_control(a_pValue, [](cpc_control_i a_pControl) { return a_pControl->state().focus; }); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool state_select(cpc_control_i a_pValue)
{ return foreach_control(a_pValue, [](cpc_control_i a_pControl) { return a_pControl->state().select; }); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool state_hover(cpc_control_i a_pValue)
{ return foreach_control(a_pValue, [](cpc_control_i a_pControl) { return a_pControl->state().hover; }); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool state_focusable(cpc_control_i a_pValue)
{ return foreach_control(a_pValue, [](cpc_control_i a_pControl) { return a_pControl->state().focusable; }); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool state_hoverable(cpc_control_i a_pValue)
{ return foreach_control(a_pValue, [](cpc_control_i a_pControl) { return a_pControl->state().hoverable; }); }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_control_i* current_focus(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool control_in_hierarchy(cpc_control_i a_pHierarchy, cpc_control_i a_pValue)
{ return foreach_control(a_pHierarchy, [a_pValue](cpc_control_i a_pControl) { return a_pControl == a_pValue; }); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

EMP_INLINE EMP_RETURN bool focus_in_hierarchy(cpc_control_i a_pValue)
{
    EMP_ASSERT(a_pValue != nullptr);
    if (a_pValue == nullptr)
    { return false; }

    cpc_control_i pFocus = current_focus();
    if (pFocus == nullptr)
    { return false; }

    return control_in_hierarchy(a_pValue, pFocus);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

