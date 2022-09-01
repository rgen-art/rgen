//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GFX_DEVICE_H
#define EMP_GFX_DEVICE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_limit.h"
#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*#define enum_name value_e
#define enum_type
#define enum_values\
    enum_value(none)\
    enum_value(Float32)\
    enum_value(UInt16)
#include "emp_tt_enum.def.h"*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*#define enum_name element_e
#define enum_values\
    enum_value( none       )\
    enum_value( Index      )\
    enum_value( Vertex     )
#include "emp_tt_enum.def.h"*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name attribute_e
#define enum_type
#define enum_values\
    enum_value( none       )\
    enum_value( v4Position )\
    enum_value( v3Position )\
    enum_value( v2Position )\
    enum_value( v4Texcoord )\
    enum_value( v3Texcoord )\
    enum_value( v2Texcoord )\
    enum_value( v4Color    )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name shader_e
#define enum_values\
    enum_value( none   )\
    enum_value( Vertex )\
    enum_value( Pixel  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name uniform_e
#define enum_type
#define enum_values\
    enum_value( none          )\
    enum_value( m44World      )\
    enum_value( m44View       )\
    enum_value( m44Projection )\
    enum_value( m44Transform  )\
    enum_value( m22Transform  )\
    enum_value( v4Color       )\
    enum_value( t2Texture_0   )\
    enum_value( t2Texture_1   )\
    enum_value( fTime         )\
    enum_value( fDuration     )\
    enum_value( fProgress     )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name usage_e
#define enum_values\
    enum_value( none      )\
    enum_value( Static    )\
    enum_value( Dynamic   )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name primitive_e
#define enum_values\
    enum_value( none       )\
    enum_value( Lines      )\
    enum_value( Triangles  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name type_e
#define enum_values\
    enum_value( none )\
    enum_value( UI8  )\
    enum_value( UI16 )\
    enum_value( UI32 )\
    enum_value( F32  )\
    enum_value( VTX  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename... T>
EMP_NOINSTANCE(struct, buffer_type_t)
static constexpr c_type_e value = type_e::VTX;
};

template <>
EMP_NOINSTANCE(struct, buffer_type_t<ui8>)
static constexpr c_type_e value = type_e::UI8;
};

template <>
EMP_NOINSTANCE(struct, buffer_type_t<ui16>)
static constexpr c_type_e value = type_e::UI16;
};

template <>
EMP_NOINSTANCE(struct, buffer_type_t<ui32>)
static constexpr c_type_e value = type_e::UI32;
};


template <typename T>
EMP_RETURN T buffer_element_t(void);

EMP_PRAGMA_PUSH_PACK(1)
template <typename T0, typename T1>
struct vertex_2
{
    T0 v0;
    T1 v1;
};
EMP_PRAGMA_POP_PACK()

template <typename... T, ENABLE_IF(sizeof...(T) == 2)>
EMP_RETURN vertex_2<T...> buffer_element_t(void);

template <typename... T>
using buffer_element = decltype(buffer_element_t<T...>());

static_assert(std::is_pod<vertex_2<v2f, v4f>>::value, "");

/*template <typename... T, ENABLE_IF(emp::tt::more<size_t, sizeof...(T), 1>::value)>
typename buffer_element<T...>::T0 const& first_element(buffer_element<T...> const& a_From)
{ return a_From.v0; }

template <typename T>
T const& first_element(T const& a_From)
{ return a_From; }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct attribute_s
{
    type_e m_eType = type_e::none;
    size_t m_stCount = 0;
    size_t m_stSize = 0;

    EMP_RETURN bool operator==(attribute_s const& a_rFrom) const
    {
        return
            m_eType == a_rFrom.m_eType &&
            m_stCount == a_rFrom.m_stCount &&
            m_stSize == a_rFrom.m_stSize;
    }

    EMP_RETURN bool operator!=(attribute_s const& a_rFrom) const
    { return operator==(a_rFrom) == false; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct target_s
{
    ui32 EMP_TT_MAX_VAR(m_ui32Index);
    ui32 EMP_TT_MAX_VAR(m_ui32Framebuffer);
    ui32 EMP_TT_MAX_VAR(m_ui32Renderbuffer);
    ui32 EMP_TT_MAX_VAR(m_ui32Texture);
    ui32 m_ui32Width = 0;
    ui32 m_ui32Height = 0;
    bool m_bResizable = false;
    bool m_bLocked = false;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, declaration_c)
public:
    declaration_c(void) :
        m_stStride(0),
        m_vAttributes()
    {}

    size_t m_stStride;
    vector<attribute_s> m_vAttributes;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void unlock_buffer(c_size a_stName);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <size_t t_stIndex, typename T, typename V>
EMP_INLINE EMP_RETURN bool get_element(T& EMP_SILENT(a_rOut), V const& EMP_SILENT(a_rElement))
{
    return false;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, ui8>(ui32& a_rOut, ui8 const& a_Value)
{
    a_rOut = a_Value;
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, ui16>(ui32& a_rOut, ui16 const& a_Value)
{
    a_rOut = a_Value;
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, v2f>(v4f& a_rOut, v2f const& a_Value)
{
    a_rOut = v4f::all(a_Value, 0, 0);
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, buffer_element<v2f, v2f>>(v4f& a_rOut, buffer_element<v2f, v2f> const& a_Value)
{
    a_rOut = v4f::all(a_Value.v0, 0, 0);
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, buffer_element<v2f, v4f>>(v4f& a_rOut, buffer_element<v2f, v4f> const& a_Value)
{
    a_rOut = v4f::all(a_Value.v0, 0, 0);
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, buffer_element<v4f, v4f>>(v4f& a_rOut, buffer_element<v4f, v4f> const& a_Value)
{
    a_rOut = a_Value.v0;
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, v2f>(ui32&, v2f const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, v4f>(ui32&, v4f const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, buffer_element<v2f, v2f>>(ui32&, buffer_element<v2f, v2f> const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, buffer_element<v2f, v4f>>(ui32&, buffer_element<v2f, v4f> const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, ui32, buffer_element<v4f, v4f>>(ui32&, buffer_element<v4f, v4f> const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, ui8>(v4f&, ui8 const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, ui32>(v4f&, ui32 const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<0, v4f, ui16>(v4f&, ui16 const&)
{ return false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <>
EMP_INLINE EMP_RETURN bool get_element<1, ui32, buffer_element<v2f, v2f>>(ui32&, buffer_element<v2f, v2f> const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<1, ui32, buffer_element<v2f, v4f>>(ui32&, buffer_element<v2f, v4f> const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<1, ui32, buffer_element<v4f, v4f>>(ui32&, buffer_element<v4f, v4f> const&)
{ return false; }

template <>
EMP_INLINE EMP_RETURN bool get_element<1, v4f, buffer_element<v2f, v2f>>(v4f& a_rOut, buffer_element<v2f, v2f> const& a_Value)
{
    a_rOut = v4f::all(a_Value.v1, 0, 0);
    return false;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<1, v4f, buffer_element<v2f, v4f>>(v4f& a_rOut, buffer_element<v2f, v4f> const& a_Value)
{
    a_rOut = a_Value.v1;
    return true;
}

template <>
EMP_INLINE EMP_RETURN bool get_element<1, v4f, buffer_element<v4f, v4f>>(v4f& a_rOut, buffer_element<v4f, v4f> const& a_Value)
{
    a_rOut = a_Value.v1;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void min_element(T const& a_Left, T const& a_Right, T& a_rOut) { a_rOut = emp::tt::minof(a_Left, a_Right); }

EMP_INLINE void min_element(c_v2f& a_Left, c_v2f& a_Right, v2f& a_rOut);
EMP_INLINE void min_element(c_v2f& a_Left, c_v2f& a_Right, v2f& a_rOut) { a_rOut = v2f::min(a_Left, a_Right); }

EMP_INLINE void min_element(c_v3f& a_Left, c_v3f& a_Right, v3f& a_rOut);
EMP_INLINE void min_element(c_v3f& a_Left, c_v3f& a_Right, v3f& a_rOut) { a_rOut = v3f::min(a_Left, a_Right); }

EMP_INLINE void min_element(c_v4f& a_Left, c_v4f& a_Right, v4f& a_rOut);
EMP_INLINE void min_element(c_v4f& a_Left, c_v4f& a_Right, v4f& a_rOut) { a_rOut = v4f::min(a_Left, a_Right); }

template <typename T0, typename T1>
EMP_INLINE void min_element(vertex_2<T0, T1> const& a_Left, vertex_2<T0, T1> const& a_Right, vertex_2<T0, T1>& a_rOut)
{
    min_element(a_Left.v0, a_Right.v0, a_rOut.v0);
    min_element(a_Left.v1, a_Right.v1, a_rOut.v1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void max_element(T const& a_Left, T const& a_Right, T& a_rOut) { a_rOut = emp::tt::maxof(a_Left, a_Right); }

EMP_INLINE void max_element(c_v2f& a_Left, c_v2f& a_Right, v2f& a_rOut);
EMP_INLINE void max_element(c_v2f& a_Left, c_v2f& a_Right, v2f& a_rOut) { a_rOut = v2f::max(a_Left, a_Right); }

EMP_INLINE void max_element(c_v3f& a_Left, c_v3f& a_Right, v3f& a_rOut);
EMP_INLINE void max_element(c_v3f& a_Left, c_v3f& a_Right, v3f& a_rOut) { a_rOut = v3f::max(a_Left, a_Right); }

EMP_INLINE void max_element(c_v4f& a_Left, c_v4f& a_Right, v4f& a_rOut);
EMP_INLINE void max_element(c_v4f& a_Left, c_v4f& a_Right, v4f& a_rOut) { a_rOut = v4f::max(a_Left, a_Right); }

template <typename T0, typename T1>
EMP_INLINE void max_element(vertex_2<T0, T1> const& a_Left, vertex_2<T0, T1> const& a_Right, vertex_2<T0, T1>& a_rOut)
{
    max_element(a_Left.v0, a_Right.v0, a_rOut.v0);
    max_element(a_Left.v1, a_Right.v1, a_rOut.v1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void size_element(T const& a_Max, T const& a_Min, T& a_rOut) { a_rOut = emp::ast::cast<T>(a_Max - a_Min); }

EMP_INLINE void size_element(c_v2f& a_Left, c_v2f& a_Right, v2f& a_rOut);
EMP_INLINE void size_element(c_v2f& a_Left, c_v2f& a_Right, v2f& a_rOut) { a_rOut = v2f::sub(a_Left, a_Right); }

EMP_INLINE void size_element(c_v3f& a_Left, c_v3f& a_Right, v3f& a_rOut);
EMP_INLINE void size_element(c_v3f& a_Left, c_v3f& a_Right, v3f& a_rOut) { a_rOut = v3f::sub(a_Left, a_Right); }

EMP_INLINE void size_element(c_v4f& a_Left, c_v4f& a_Right, v4f& a_rOut);
EMP_INLINE void size_element(c_v4f& a_Left, c_v4f& a_Right, v4f& a_rOut) { a_rOut = v4f::sub(a_Left, a_Right); }

template <typename T0, typename T1>
EMP_INLINE void size_element(vertex_2<T0, T1> const& a_Left, vertex_2<T0, T1> const& a_Right, vertex_2<T0, T1>& a_rOut)
{
    size_element(a_Left.v0, a_Right.v0, a_rOut.v0);
    size_element(a_Left.v1, a_Right.v1, a_rOut.v1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void zero_element(T& a_rOut) { a_rOut = 0; }

EMP_INLINE void zero_element(v2f& a_rOut);
EMP_INLINE void zero_element(v2f& a_rOut) { a_rOut = v2f::zero(); } // @@0 init

EMP_INLINE void zero_element(v3f& a_rOut);
EMP_INLINE void zero_element(v3f& a_rOut) { a_rOut = v3f::zero(); } // @@0 init

EMP_INLINE void zero_element(v4f& a_rOut);
EMP_INLINE void zero_element(v4f& a_rOut) { a_rOut = v4f::all(0); } // @@0 init

template <typename T0, typename T1>
EMP_INLINE void zero_element(vertex_2<T0, T1>& a_rOut)
{
    zero_element(a_rOut.v0);
    zero_element(a_rOut.v1);
}

} }

namespace emp { namespace tt {
    using namespace emp::gfx;

    template <typename T0, typename T1>
    constexpr EMP_INLINE EMP_RETURN bool is_valid(vertex_2<T0, T1> const& v)
    { return is_valid(v.v0) && is_valid(v.v1); }

    template <typename T0, typename T1>
    constexpr EMP_INLINE EMP_RETURN bool is_not_valid(vertex_2<T0, T1> const& v)
    { return is_valid(v) == false; }

    template <typename T0, typename T1>
    constexpr EMP_INLINE EMP_RETURN bool has_max(vertex_2<T0, T1> const& v)
    { return has_max(v.v0) || has_max(v.v1); }

    template <typename T0, typename T1>
    constexpr EMP_INLINE EMP_RETURN bool has_min(vertex_2<T0, T1> const& v)
    { return has_min(v.v0) || has_min(v.v1); }
} }

namespace emp { namespace gfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename... T>
EMP_RETURN bool get_declaration(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, buffer_i)
public:
    virtual EMP_RETURN bool changed(void) const = 0;
    virtual void set_processed(void) const = 0;
    virtual EMP_RETURN size_t name(void) const = 0;
    virtual void set_name(c_size a_stValue) = 0;
    virtual EMP_RETURN size_t count(void) const = 0;
    virtual EMP_RETURN size_t size(void) const = 0;
    virtual EMP_RETURN usage_e usage(void) const = 0;
    virtual EMP_RETURN declaration_c const* declaration(void) const = 0;
    virtual EMP_RETURN type_e type(void) const = 0;
    virtual EMP_RETURN char const* data(void) const = 0;
    virtual EMP_RETURN bool compute_bounds(void) = 0;

#define XTMP_GET_BOUNDS(x_Type)\
    virtual EMP_RETURN bool get_bounds(c_size a_stSlot, x_Type& a_rMin, x_Type& a_rMax, x_Type& a_rSize, x_Type& a_rCenter) = 0;

    //XTMP_GET_BOUNDS(ui8)
    //XTMP_GET_BOUNDS(ui16)
    XTMP_GET_BOUNDS(ui32)
    //XTMP_GET_BOUNDS(f32)
    //XTMP_GET_BOUNDS(v2f)
    //XTMP_GET_BOUNDS(v3f)
    XTMP_GET_BOUNDS(v4f)

#undef XTMP_GET_BOUNDS

    buffer_i(void) {}
    virtual ~buffer_i(void) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename... T>
EMP_NOCOPY_BASE(class, buffer_t, public, buffer_i)
public:
    using telement = buffer_element<T...>;
    static_assert(std::is_pod<telement>::value, "");

public:

    vector<telement> m_vData;
    size_t m_stName;

    telement m_Min;
    telement m_Max;
    telement m_Size;
    telement m_Center;

    mutable bool m_bChanged;

    usage_e m_eUsage;
    static size_t s_stDeclaration;

public:
    buffer_t(void) :
        m_vData(),
        EMP_TT_MAX_CTOR(m_stName),
        m_Min(),
        m_Max(),
        m_Size(),
        m_Center(),
        m_bChanged(true),
        m_eUsage(usage_e::Static)
    {}

    ~buffer_t(void)
    {
        destroy();
    }

    EMP_RETURN bool post_load(void)
    {
        // @@1 perf resize done in rfx
        // @@0 duplicate code
        c_size stCount = m_vData.count();
        size_t stBytes = stCount * sizeof(telement);

        size_t stSize = 1;
        while (stSize <= stBytes) // @@0 legit ?
        {
            stSize <<= 1;
        }

        c_size stNewCount = 1 + (stSize / sizeof(telement));
        m_vData.resize(stNewCount);
        m_vData.resize(stCount);

        EMP_RET_FALSE_IF_NOT(compute_bounds());
        EMP_RET_FALSE_IF_NOT(reg_declaration());
        return true;
    }

    EMP_RETURN bool reg_declaration(void);

    void create(c_usage_e a_eUsage)
    {
        destroy();
        m_eUsage = a_eUsage;
        EMP_ASSERT_BOOL_CALL(reg_declaration());
    }

    void destroy(void)
    {
        m_vData.clear();
        unlock_buffer(m_stName);
        EMP_TT_MAX_VAR(m_stName);
        m_bChanged = true;
        m_eUsage = usage_e::none;
    }

    EMP_RETURN size_t name(void) const { return m_stName; }
    void set_name(c_size a_stValue) { m_stName = a_stValue; }

    EMP_RETURN bool changed(void) const { return m_bChanged; }
    void set_processed(void) const { m_bChanged = false; }

    EMP_RETURN char const* data(void) const { return reinterpret_cast<char const*>(m_vData.first_const_ptr()); }

    void add(T const&... a_Value)
    {
        c_size stCount = m_vData.count();

        size_t stNextBytes = (stCount + 1) * sizeof(telement);

        size_t stSize = 1;
        while (stSize <= stNextBytes) // @@0 legit ?
        {
            stSize <<= 1;
        }

        c_size stNewCount = 1 + (stSize / sizeof(telement));
        m_vData.resize(stNewCount);
        m_vData.resize(stCount);
        m_vData.add({ a_Value... });
        m_bChanged = true;
    }

    EMP_RETURN size_t count(void) const { return m_vData.count(); }
    EMP_RETURN size_t size(void) const { return m_vData.count() * sizeof(telement); } // @@0 move in emp::cnt::container_t

    EMP_RETURN declaration_c const* declaration(void) const;

    EMP_RETURN type_e type(void) const { return buffer_type_t<T...>::value; }

    EMP_RETURN usage_e usage(void) const { return m_eUsage; }
    EMP_RETURN bool compute_bounds(void)
    {
        c_size stCount = m_vData.count();

        m_Min = { emp::tt::max<T>::value... };
        m_Max = { emp::tt::min<T>::value... };

        if (stCount > 0)
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                min_element(m_Min, m_vData.ref(stIndex), m_Min);
                max_element(m_Max, m_vData.ref(stIndex), m_Max);
            }
            size_element(m_Max, m_Min, m_Size);
        }
        else
        {
            zero_element(m_Min);
            zero_element(m_Max);
            zero_element(m_Size);
        }

        EMP_RET_FALSE_IF(emp::tt::is_not_valid(m_Min) || emp::tt::is_not_valid(m_Max) || emp::tt::is_not_valid(m_Size));
        return true;
    }

#define XTMP_GET_BOUNDS(x_Type)\
    EMP_RETURN bool get_bounds(c_size a_stSlot, x_Type& a_rMin, x_Type& a_rMax, x_Type& a_rSize, x_Type& a_rCenter)\
    { return get_bounds_impl<sizeof...(T), x_Type, T...>::apply(*this, a_stSlot, a_rMin, a_rMax, a_rSize, a_rCenter); }

    //XTMP_GET_BOUNDS(ui8)
    //XTMP_GET_BOUNDS(ui16)
    XTMP_GET_BOUNDS(ui32)
        //XTMP_GET_BOUNDS(f32)
        //XTMP_GET_BOUNDS(v2f)
        //XTMP_GET_BOUNDS(v3f)
        XTMP_GET_BOUNDS(v4f)

#undef XTMP_GET_BOUNDS

        template <size_t t_stCount, typename U, typename... V>
    struct get_bounds_impl
    {
        static EMP_RETURN bool apply(buffer_t& a_rThis, c_size a_stSlot, U& a_rMin, U& a_rMax, U& a_rSize, U& a_rCenter)
        {
            if (t_stCount == a_stSlot)
            {
                return get_element_impl<t_stCount - 1, U, V...>(a_rThis, a_rMin, a_rMax, a_rSize, a_rCenter);
            }
            return get_bounds_impl<t_stCount - 1, U, V...>::apply(a_rThis, a_stSlot, a_rMin, a_rMax, a_rSize, a_rCenter);
        }
    };

    template <typename U, typename...V>
    struct get_bounds_impl<0, U, V...>
    {
        static EMP_RETURN bool apply(buffer_t& a_rThis, c_size a_stSlot, U& a_rMin, U& a_rMax, U& a_rSize, U& a_rCenter)
        {
            if (0 == a_stSlot)
            {
                return get_element_impl<0, U, V...>(a_rThis, a_rMin, a_rMax, a_rSize, a_rCenter);
            }
            return false;
        }
    };

    template <size_t t_stIndex, typename U, typename... V>
    static EMP_RETURN bool get_element_impl(buffer_t& a_rThis, U& a_rMin, U& a_rMax, U& a_rSize, U& a_rCenter)
    {
        EMP_RET_FALSE_IF_NOT((get_element<t_stIndex, U, buffer_element<V...>>(a_rMin, a_rThis.m_Min)));
        EMP_RET_FALSE_IF_NOT((get_element<t_stIndex, U, buffer_element<V...>>(a_rMax, a_rThis.m_Max)));
        EMP_RET_FALSE_IF_NOT((get_element<t_stIndex, U, buffer_element<V...>>(a_rSize, a_rThis.m_Size)));
        EMP_RET_FALSE_IF_NOT((get_element<t_stIndex, U, buffer_element<V...>>(a_rCenter, a_rThis.m_Center)));
        return true;
    }
};

template <typename...T> size_t buffer_t<T...>::s_stDeclaration = emp::tt::max<size_t>::value;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void emit_line(size_t a_stCount, buffer_t<T>& a_rIndices)
{
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 0));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 1));
}

template <typename T>
void emit_triangle(size_t a_stCount, buffer_t<T>& a_rIndices)
{
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 0));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 1));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 2));
}

template <typename T>
void emit_quad(size_t a_stCount, buffer_t<T>& a_rIndices)
{
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 0u));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 1u));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 2u));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 2u));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 1u));
    a_rIndices.add(emp::ast::cast<T>(a_stCount + 3u));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
