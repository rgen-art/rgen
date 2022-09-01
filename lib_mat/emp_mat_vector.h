//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_VECTOR_H
#define EMP_MAT_VECTOR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_algebra.h"
#include "emp_mat_trigonometry.h"

#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_intrin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
using vret = typename T::tret;

template <typename T>
using vparam = typename T::tparam;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(struct, v2)
public:
    T x;
    T y;

public:
    typedef T const c_T;
    typedef v2<T> type;
    typedef type const c_type;
    typedef c_type tret;
    typedef c_type& tparam;
        
    static constexpr EMP_INLINE EMP_RETURN v2<T> all_const(T const x) { return{ x, x }; }
    static EMP_INLINE EMP_RETURN v2<T> all(T const x);

    static EMP_INLINE EMP_RETURN T length(v2<T> const& v) { return sqr(dot(v)); }
    static EMP_INLINE EMP_RETURN T dot(v2<T> const& v0, v2<T> const& v1) { return v0.x * v1.x + v0.y * v1.y; }
    static EMP_INLINE EMP_RETURN T dot(v2<T> const v) { return v.x * v.x + v.y * v.y; }

    static EMP_INLINE EMP_RETURN v2<T> normalize(v2<T> const& v) { return div(v, length(v)); }
    static constexpr EMP_INLINE EMP_RETURN tret zero(void)      { return all(0); }
    static constexpr EMP_INLINE EMP_RETURN tret one(void)       { return all(1); }
    static constexpr EMP_INLINE EMP_RETURN tret half(void)      { return all(0.5f); }
    static constexpr EMP_INLINE EMP_RETURN tret half(tparam v)  { return mul(half(), v); }
    static constexpr EMP_INLINE EMP_RETURN tret neg(tparam v)   { return sub(zero(), v); }

    static EMP_INLINE EMP_RETURN v2<T> swizzle_yx(v2<T> const& v);

    static constexpr EMP_INLINE EMP_RETURN tret abs(tparam v)
    { return { mat::abs(v.x), mat::abs(v.y) }; }

    static constexpr EMP_INLINE EMP_RETURN T min(tparam v)
    { return emp::tt::minof(v.x, v.y); }

    static constexpr EMP_INLINE EMP_RETURN tret min(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::minof(v0.x, v1.x),
            emp::tt::minof(v0.y, v1.y)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN T max(tparam v)
    { return emp::tt::maxof(v.x, v.y); }

    static constexpr EMP_INLINE EMP_RETURN tret max(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::maxof(v0.x, v1.x),
            emp::tt::maxof(v0.y, v1.y)
        };
    }

    static EMP_INLINE EMP_RETURN v2<T> sub(v2<T> const& v0, v2<T> const& v1);
    static EMP_INLINE EMP_RETURN v2<T> add(v2<T> const& v0, v2<T> const& v1);

    static EMP_INLINE EMP_RETURN v2<T> div(v2<T> const&  v0, v2<T> const&  v1);
    static constexpr EMP_INLINE v2<T> div(v2<T> const& v, T const t) { return div(v, all(t)); }

    static EMP_INLINE EMP_RETURN v2<T> mul(v2<T> const&  v0, v2<T> const&  v1);
    static constexpr EMP_INLINE EMP_RETURN v2<T> mul(tparam v, T t)  { return mul(v, all(t)); }
    
    static EMP_INLINE EMP_RETURN v2<T> more(v2<T> const&  v0, v2<T> const&  v1);
    static EMP_INLINE EMP_RETURN v2<T> less(v2<T> const&  v0, v2<T> const&  v1);
    static EMP_INLINE EMP_RETURN v2<T> less_equal(v2<T> const&  v0, v2<T> const&  v1);
    static EMP_INLINE EMP_RETURN v2<T> more_equal(v2<T> const&  v0, v2<T> const&  v1);

    static constexpr EMP_INLINE EMP_RETURN tret select(tparam vs, tparam v0, tparam v1)
    {
        return
        {
            emp::tt::not_equal_<T>(vs.x, 0) ? v0.x : v1.x,
            emp::tt::not_equal_<T>(vs.y, 0) ? v0.y : v1.y
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret equal(tparam v0, tparam v1)
    {
        return
            {
                emp::tt::equal_(v0.x, v1.x) ? (T)1 : (T)0,
                emp::tt::equal_(v0.y, v1.y) ? (T)1 : (T)0
            };
    }

    static constexpr EMP_INLINE EMP_RETURN tret not_equal(tparam v0, tparam v1)
    { return equal(equal(v0, v1), zero()); }

    static EMP_INLINE EMP_RETURN T sqlen(v2<T> const&  v0);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T> using v2ret = vret<v2<T>>;
//template <typename T> using v2param = vparam<v2<T>>;

typedef v2<float> v2f;
typedef v2<bool> v2b;

EMP_TYPEDEF(v2f)
EMP_TYPEDEF(v2b)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(struct, v3)
public:
    T x;
    T y;
    T z;

public:
    typedef T const c_T;
    typedef v3<T> type;
    typedef type const c_type;
    typedef c_type tret;
    typedef c_type& tparam;

    static constexpr EMP_INLINE EMP_RETURN v3<T> all_const(T const x) { return{ x, x, x }; }
    static constexpr EMP_INLINE EMP_RETURN v3<T> all_const(v2<T> const& v, T const z) { return{ v.x, v.y, z }; }

    static EMP_INLINE EMP_RETURN v3<T> all(T const x);
    static EMP_INLINE EMP_RETURN v3<T> all(v2<T> const& v, T const z);

    static constexpr EMP_INLINE EMP_RETURN tret zero(void) { return all(0); }
    static constexpr EMP_INLINE EMP_RETURN tret one(void) { return all(1); }
    static constexpr EMP_INLINE EMP_RETURN tret half(void) { return all(0.5); }
    static constexpr EMP_INLINE EMP_RETURN tret min(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::minof(v0.x, v1.x),
            emp::tt::minof(v0.y, v1.y),
            emp::tt::minof(v0.z, v1.z)
        };
    }
    static constexpr EMP_INLINE EMP_RETURN tret max(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::maxof(v0.x, v1.x),
            emp::tt::maxof(v0.y, v1.y),
            emp::tt::maxof(v0.z, v1.z)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN v2<T> xy(v3<T> const& v);

    static constexpr EMP_INLINE EMP_RETURN v3<T> neg(v3<T> const& v0);
    static constexpr EMP_INLINE EMP_RETURN v3<T> sub(v3<T> const& v0, v3<T> const& v1);
    static constexpr EMP_INLINE EMP_RETURN v3<T> add(v3<T> const& v0, v3<T> const& v1);
    static constexpr EMP_INLINE EMP_RETURN v3<T> div(v3<T> const& v0, v3<T> const& v1);
    static constexpr EMP_INLINE EMP_RETURN v3<T> mul(v3<T> const& v0, v3<T> const& v1);
    static constexpr EMP_INLINE EMP_RETURN tret mul(tparam v, T t) { return mul(v, all(t)); }

    static constexpr EMP_INLINE EMP_RETURN tret equal(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::equal_(v0.x, v1.x) ? (T)1 : (T)0,
            emp::tt::equal_(v0.y, v1.y) ? (T)1 : (T)0,
            emp::tt::equal_(v0.z, v1.z) ? (T)1 : (T)0
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret not_equal(tparam v0, tparam v1)
    { return equal(equal(v0, v1), zero()); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T> using v3ret = vret<v3<T>>;
//template <typename T> using v3param = vparam<v3<T>>;

typedef v3<float> v3f;
typedef v3<bool> v3b;

EMP_TYPEDEF(v3f)
EMP_TYPEDEF(v3b)

#if defined EMP_XX_DEVICE_ARM
#else
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOINSTANCE(struct, sse_v4f_d)
    typedef __m128 type;
    typedef type const tparam;
    typedef type tret;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOINSTANCE(struct, sse_v4i_d)
    typedef __m128i type;
    typedef type const tparam;
    typedef type tret;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOINSTANCE(struct, sse_v4i)
    typedef sse_v4i_d::type type;
    typedef sse_v4i_d::tparam tparam;
    typedef sse_v4i_d::tret tret;

    static EMP_INLINE EMP_RETURN tret set(c_si32 i) { return _mm_set1_epi32(i); }
    static EMP_INLINE EMP_RETURN tret set(c_si32 i0, c_si32 i1, c_si32 i2, c_si32 i3) { return _mm_set_epi32(i0, i1, i2, i3); }

    //>> constants
    static EMP_INLINE EMP_RETURN tret zero(void) { return _mm_setzero_si128(); }
    static EMP_INLINE EMP_RETURN tret one(void) { return set(mat::one<si32>()); }
    static EMP_INLINE EMP_RETURN tret two(void) { return set(mat::two<si32>()); }
    static EMP_INLINE EMP_RETURN tret three(void) { return set(mat::three<si32>()); }
    //<< constants

    //>> locigal
    static EMP_INLINE EMP_RETURN tret bit_and(tparam v0, tparam v1) { return _mm_and_si128(v0, v1); }
    //<< logical

    //>> copy
    static EMP_INLINE EMP_RETURN tret bit_copy(sse_v4f_d::tparam v) { return _mm_castps_si128(v); }
    //<< copy

    //>> mask
    static EMP_INLINE EMP_RETURN tret equal_mask(tparam v0, tparam v1) { return _mm_cmpeq_epi32(v0, v1); }
    //<< mask

    //>> convert
    static EMP_INLINE EMP_RETURN tret convert(sse_v4f_d::tparam vf) { return _mm_cvtps_epi32(vf); }
    //<< convert

    //>> compare
    //<< compare

    //>> math
    //<< math

    //>> getter
    //<< getter

    //>> setter
    //<< setter
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOINSTANCE(struct, sse_v4f)
    typedef sse_v4f_d::type type;
    typedef sse_v4f_d::tparam tparam;
    typedef sse_v4f_d::tret tret;

    //>> control
    static void denormal_on(void) { _mm_setcsr((_mm_getcsr() & ~0x0040u) | (0x0040u)); }
    static void denormal_off(void) { _mm_setcsr((_mm_getcsr() & ~0x0040u) | (0x0000u)); }
    //<< control

    static EMP_INLINE EMP_RETURN tret set(c_float f) { return _mm_set1_ps(f); }

    static EMP_INLINE EMP_RETURN tret set(c_float f0, c_float f1, c_float f2, c_float f3) { return _mm_set_ps(f0, f1, f2, f3); }

#if defined __SSE4_1__
#define XTMP_EXTRACT(x_f, x_v, x_i)\
    _MM_EXTRACT_FLOAT((x_f), (x_v), (x_i));
#elif defined __SSE2__
#define XTMP_EXTRACT(x_f, x_v, x_i)\
    _mm_store_ss(&(x_f), _mm_shuffle_ps((x_v), (x_v),\
        _MM_SHUFFLE((x_i),(x_i),(x_i),(x_i))));
#else
#error
#endif
    //>> getter
    static EMP_INLINE EMP_RETURN float get_x(tparam v) { float f = 0; XTMP_EXTRACT(f, v, 3); return f; }
    static EMP_INLINE EMP_RETURN float get_y(tparam v) { float f = 0; XTMP_EXTRACT(f, v, 2); return f; }
    static EMP_INLINE EMP_RETURN float get_z(tparam v) { float f = 0; XTMP_EXTRACT(f, v, 1); return f; }
    static EMP_INLINE EMP_RETURN float get_w(tparam v) { float f = 0; XTMP_EXTRACT(f, v, 0); return f; }
    //<< getter
#undef XTMP_EXTRACT

#if defined __SSE4_1__
#define XTMP_INSERT(x_v, x_f, x_i)\
    _mm_insert_ps((x_v), set((x_f)), _MM_MK_INSERTPS_NDX(0x0, (x_i), 0x0));
#elif defined __SSE2__
#define XTMP_INSERT(x_v, x_f, x_i)\
   bit_select(bool_mask((x_i)==3,(x_i)==2,(x_i)==1,(x_i)==0),set((x_f)),(x_v));
#else
#error
#endif
    //>> setter
    static EMP_INLINE EMP_RETURN tret set_x(tparam v, float const f) { return XTMP_INSERT(v, f, 0x3); }
    static EMP_INLINE EMP_RETURN tret set_y(tparam v, float const f) { return XTMP_INSERT(v, f, 0x2); }
    static EMP_INLINE EMP_RETURN tret set_z(tparam v, float const f) { return XTMP_INSERT(v, f, 0x1); }
    static EMP_INLINE EMP_RETURN tret set_w(tparam v, float const f) { return XTMP_INSERT(v, f, 0x0); }
    //>> setter
#undef XTMP_EXTRACT

    //>> constants
    static EMP_INLINE EMP_RETURN tret zero(void) { return _mm_setzero_ps(); }
    static EMP_INLINE EMP_RETURN tret half(void) { return set(mat::half<float>()); }
    static EMP_INLINE EMP_RETURN tret one(void) { return set(mat::one<float>()); }
    static EMP_INLINE EMP_RETURN tret two(void) { return set(mat::two<float>()); }
    static EMP_INLINE EMP_RETURN tret four(void) { return set(mat::four<float>()); }

    static EMP_INLINE EMP_RETURN tret eight(void) { return set(mat::eight<float>()); }
    static EMP_INLINE EMP_RETURN tret negative(void) { return set(mat::negative<float>()); }
    static EMP_INLINE EMP_RETURN tret pi(void) { return set(mat::pi<float>()); }
    static EMP_INLINE EMP_RETURN tret pipi(void) { return set(mat::pipi<float>()); }
    static EMP_INLINE EMP_RETURN tret pi_over_two(void) { return set(mat::two_over_pi<float>()); }
    static EMP_INLINE EMP_RETURN tret two_over_pi(void) { return set(mat::two_over_pi<float>()); }
    static EMP_INLINE EMP_RETURN tret two_pi(void) { return set(mat::two_pi<float>()); }

    static EMP_INLINE EMP_RETURN tret sincos_cc0(void) { return set(mat::sincos_cc0<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_cc1(void) { return set(mat::sincos_cc1<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_cc2(void) { return set(mat::sincos_cc2<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_sc0(void) { return set(mat::sincos_sc0<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_sc1(void) { return set(mat::sincos_sc1<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_sc2(void) { return set(mat::sincos_sc2<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_kc1(void) { return set(mat::sincos_kc1<float>()); }
    static EMP_INLINE EMP_RETURN tret sincos_kc2(void) { return set(mat::sincos_kc2<float>()); }
    //<< constants

    //>> logical
    static EMP_INLINE EMP_RETURN tret bit_and(tparam v0, tparam v1) { return _mm_and_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret bit_and_not(tparam v0, tparam v1) { return _mm_andnot_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret bit_or(tparam v0, tparam v1) { return _mm_or_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret bit_xor(tparam v0, tparam v1) { return _mm_xor_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret bit_select(tparam s, tparam v0,tparam v1) { return bit_or(bit_and(s, v0), bit_and_not(s, v1)); }
    //<< logical

    //>> copy
    static EMP_INLINE EMP_RETURN tret bit_copy(sse_v4i_d::tparam vi) { return _mm_castsi128_ps(vi); }
    //<< copy

    //>> convert
    static EMP_INLINE EMP_RETURN tret convert(sse_v4i_d::tparam vi) { return _mm_cvtepi32_ps(vi); }
    //<< convert

    //<< compare mask
    static EMP_INLINE EMP_RETURN tret bool_mask(c_bool b) { return not_equal_mask(set(b ? 1.0f : 0.0f), zero()); }
    static EMP_INLINE EMP_RETURN tret bool_mask(c_bool b0, c_bool b1, c_bool b2, c_bool b3) { return not_equal_mask(set(b0 ? 1.0f : 0.0f, b1 ? 1.0f : 0.0f, b2 ? 1.0f : 0.0f, b3 ? 1.0f : 0.0f), zero()); }
    static EMP_INLINE EMP_RETURN tret equal_mask(tparam v0, tparam v1) { return _mm_cmpeq_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret not_equal_mask(tparam v0, tparam v1) { return _mm_cmpneq_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret less_mask(tparam v0, tparam v1) { return _mm_cmplt_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret less_equal_mask(tparam v0, tparam v1) { return _mm_cmple_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret more_mask(tparam v0, tparam v1) { return _mm_cmpgt_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret more_equal_mask(tparam v0, tparam v1) { return _mm_cmpge_ps(v0, v1); }
    //<< compare mask

    //>> compare bool
    static EMP_INLINE EMP_RETURN bool bool_equal(tparam v0, tparam v1) { return _mm_movemask_ps(not_equal_mask(v0, v1)) == 0; }
    static EMP_INLINE EMP_RETURN bool bool_not_equal(tparam v0, tparam v1) { return _mm_movemask_ps(equal_mask(v0, v1)) == 0; }
    //<< compere bool

    //>> compare unit
    static EMP_INLINE EMP_RETURN tret equal(tparam v0, tparam v1) { return bit_and(equal_mask(v0, v1), one()); }
    static EMP_INLINE EMP_RETURN tret not_equal(tparam v0, tparam v1) { return bit_and(not_equal_mask(v0, v1), one()); }
    static EMP_INLINE EMP_RETURN tret less(tparam v0, tparam v1) { return bit_and(less_mask(v0, v1), one()); }
    static EMP_INLINE EMP_RETURN tret less_equal(tparam v0, tparam v1) { return bit_and(less_equal_mask(v0, v1), one()); }
    static EMP_INLINE EMP_RETURN tret more(tparam v0, tparam v1) { return bit_and(more_mask(v0, v1), one()); }
    static EMP_INLINE EMP_RETURN tret more_equal(tparam v0, tparam v1) { return bit_and(more_equal_mask(v0, v1), one()); }
    //<< compare unit

    //> math
    static EMP_INLINE EMP_RETURN tret add(tparam v0, tparam v1) { return _mm_add_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret sub(tparam v0, tparam v1) { return _mm_sub_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret mul(tparam v0, tparam v1) { return _mm_mul_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret div(tparam v0, tparam v1) { return _mm_div_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret mul_add(tparam v0, tparam v1, tparam v2) { return add(mul(v0, v1), v2); }
    static EMP_INLINE EMP_RETURN tret neg(tparam v) { return bit_or(negative(), v); }
    static EMP_INLINE EMP_RETURN tret abs(tparam v) { return bit_and_not(negative(), v); }
    static EMP_INLINE EMP_RETURN tret max(tparam v0, tparam v1) { return _mm_max_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN tret min(tparam v0, tparam v1) { return _mm_min_ps(v0, v1); }
    static EMP_INLINE EMP_RETURN float max(tparam v)
    {
        return get_w(max(
            _mm_shuffle_ps(
            v,
            max(_mm_movehl_ps(v, v), v),
            _MM_SHUFFLE(1, 0, 1, 0)),
            v));
    }

    static EMP_INLINE EMP_RETURN tret sqr(tparam v) { return _mm_sqrt_ps(v); }

    static EMP_INLINE EMP_RETURN tret sin(tparam v)
    {
        type const vhalf = half();
        type const vone = one();
        type const vnegative = negative();

        type const vcc0 = sincos_cc0();
        type const vcc1 = sincos_cc1();
        type const vcc2 = sincos_cc2();

        type const vsc0 = sincos_sc0();
        type const vsc1 = sincos_sc1();
        type const vsc2 = sincos_sc2();

        type const vkc1 = sincos_kc1();
        type const vkc2 = sincos_kc2();

        sse_v4i::type const vizero = sse_v4i::zero();
        sse_v4i::type const vione = sse_v4i::one();
        sse_v4i::type const vitwo = sse_v4i::two();
        sse_v4i::type const vithree = sse_v4i::three();

        type const vtwo_over_pi = two_over_pi();
        type const vrange = mul(v, vtwo_over_pi);

        type const vquadrant = add(vrange, bit_select(vnegative,vrange,vhalf));
        sse_v4i::type const viquadran = sse_v4i::convert(vquadrant);

        type const vfq = convert(viquadran);
        type const vfp1 = sub(v, mul(vfq, vkc1));
        type const vremainder = sub(vfp1, mul(vfq, vkc2));

        type const xl2 = mul(vremainder, vremainder);
        type const xl3 = mul(xl2, vremainder);

        type const ct1 = mul_add(vcc0, xl2, vcc1);
        type const st1 = mul_add(vsc0, xl2, vsc1);

        type const ct2 = mul_add(ct1, xl2, vcc2);
        type const st2 = mul_add(st1, xl2, vsc2);

        type const cx = mul_add(ct2, xl2, vone);
        type const sx = mul_add(st2, xl3, vremainder);

        sse_v4i::type const vioffset = sse_v4i::bit_and(viquadran, vithree);
        sse_v4i::type const vimask1 = sse_v4i::equal_mask(sse_v4i::bit_and(vioffset, vione), vizero);
        sse_v4i::type const vimask2 = sse_v4i::equal_mask(sse_v4i::bit_and(vioffset, vitwo), vizero);

        type const res = bit_select(bit_copy(vimask1), sx, cx);
        return bit_select(bit_copy(vimask2), res, bit_xor(res, negative()));
    }

    // not simded yet
    static EMP_INLINE EMP_RETURN tret cos(tparam v)
    {
        return set(
            mat::cos(get_x(v)),
            mat::cos(get_y(v)),
            mat::cos(get_z(v)),
            mat::cos(get_w(v)));
    }

    // not simded yet
    static EMP_INLINE EMP_RETURN tret pow(tparam v0, tparam v1)
    {
        return set(
            mat::pow(get_x(v0), get_x(v1)),
            mat::pow(get_y(v0), get_y(v1)),
            mat::pow(get_z(v0), get_z(v1)),
            mat::pow(get_w(v0), get_w(v1)));
    }

    //<< math
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
struct v4
{
public:
    T x;
    T y;
    T z;
    T w;


    EMP_RETURN bool operator==(v4<T>& a_rFrom) const
    {
        return  emp::tt::equal_(x, a_rFrom.x) &&
                emp::tt::equal_(y, a_rFrom.y) &&
                emp::tt::equal_(z, a_rFrom.z) &&
                emp::tt::equal_(w, a_rFrom.w);
    }

    EMP_RETURN bool operator!=(v4<T>& a_rFrom) const { return operator==(a_rFrom) == false; }

public:
    typedef T const c_T;
    typedef v4 type;
    typedef type tret;
    typedef type const& tparam;

    static void denormal_on(void) {}
    static void denormal_off(void) {}

   


    static constexpr EMP_INLINE EMP_RETURN v4<T> zero(void) { return {0, 0, 0, 0}; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> one(void) { return {1, 1, 1, 1}; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> two(void) { return {2, 2, 2, 2}; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> four(void) { return {4, 4, 4, 4}; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> eight(void) { return {8, 8, 8, 8}; }

    static constexpr EMP_INLINE EMP_RETURN T get_x(tparam v) { return v.x; }
    static constexpr EMP_INLINE EMP_RETURN T get_y(tparam v) { return v.y; }
    static constexpr EMP_INLINE EMP_RETURN T get_z(tparam v) { return v.z; }
    static constexpr EMP_INLINE EMP_RETURN T get_w(tparam v) { return v.w; }

    static constexpr EMP_INLINE EMP_RETURN v4<T>
     bool_mask(c_bool b) { return b ? v4({1, 1, 1, 1}) : v4({0, 0, 0, 0}); }

    static constexpr EMP_INLINE EMP_RETURN v4<T> set(T const x) { return all_const(x); }
    static constexpr EMP_INLINE EMP_RETURN v4<T> set(T const x, T const y, T const z, T const w) { return {x, y, z, w }; }

    static constexpr EMP_INLINE EMP_RETURN v4<T> set_x(tparam v, T const t) { return set(  t, v.y, v.z, v.w); }
    static constexpr EMP_INLINE EMP_RETURN v4<T> set_y(tparam v, T const t) { return set(v.x,   t, v.z, v.w); }
    static constexpr EMP_INLINE EMP_RETURN v4<T> set_z(tparam v, T const t) { return set(v.x, v.y,   t, v.w); }
    static constexpr EMP_INLINE EMP_RETURN v4<T> set_w(tparam v, T const t) { return set(v.x, v.y, v.z,   t); }

    static constexpr EMP_INLINE EMP_RETURN v4<T> all_const(v2<T> const& v0, v2<T> const& v1) { return{ v0.x, v0.y, v1.x, v1.y }; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> all_const(v2<T> const& v, T const z, T const w) { return{ v.x, v.y, z, w }; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> all_const(v3<T> const& v, T const w) { return{ v.x, v.y, v.z, w }; }
    static constexpr EMP_INLINE EMP_RETURN v4<T> all_const(T const x) { return{ x, x, x, x }; }

    static EMP_INLINE EMP_RETURN v4<T> all(v2<T> const& v0, v2<T> const& v1);
    static EMP_INLINE EMP_RETURN v4<T> all(v2<T> const& v, T const z, T const w);
    static EMP_INLINE EMP_RETURN v4<T> all(v3<T> const& v, T const w);
    static EMP_INLINE EMP_RETURN v4<T> all(T const x);

    static constexpr EMP_INLINE EMP_RETURN tret splat_x(tparam v) { return all(v.x); }
    static constexpr EMP_INLINE EMP_RETURN tret splat_y(tparam v) { return all(v.y); }
    static constexpr EMP_INLINE EMP_RETURN tret splat_z(tparam v) { return all(v.z); }
    static constexpr EMP_INLINE EMP_RETURN tret splat_w(tparam v) { return all(v.w); }

    static constexpr EMP_INLINE EMP_RETURN v2<T> xy(v4<T> const& v);
    static constexpr EMP_INLINE EMP_RETURN v2<T> zw(v4<T> const& v);
    static constexpr EMP_INLINE EMP_RETURN v3<T> xyz(v4<T> const& v);

    static constexpr EMP_INLINE EMP_RETURN v4<T> add(v4<T> const& v0, v4<T> const& v1);
    static constexpr EMP_INLINE EMP_RETURN v4<T> sub(v4<T> const& v0, v4<T> const& v1);
    static constexpr EMP_INLINE EMP_RETURN v4<T> mul(v4<T> const& v0, v4<T> const& v1);

    static constexpr EMP_INLINE EMP_RETURN tret mul_w(tparam v0, T t) { return mul(v0, { 1, 1, 1, t }); }
    static constexpr EMP_INLINE EMP_RETURN tret mul(tparam v0, T t) { return mul(v0, all(t)); }

    static constexpr EMP_INLINE EMP_RETURN v4<T> div(v4<T> const& v0, v4<T> const&v1);

    static constexpr EMP_INLINE EMP_RETURN tret mul_add(tparam v0, tparam v1, tparam v2) { return add(mul(v0, v1), v2); }

    static constexpr EMP_INLINE EMP_RETURN tret select(tparam vs, tparam v0, tparam v1)
    {
        return
        {
            emp::tt::not_equal_<T>(vs.x, 0) ? v0.x : v1.x,
            emp::tt::not_equal_<T>(vs.y, 0) ? v0.y : v1.y,
            emp::tt::not_equal_<T>(vs.z, 0) ? v0.z : v1.z,
            emp::tt::not_equal_<T>(vs.w, 0) ? v0.w : v1.w
        };
    }
    static constexpr EMP_INLINE EMP_RETURN tret bit_select(tparam vs, tparam v0, tparam v1)
    { return select(vs, v0, v1); }

    static constexpr EMP_INLINE EMP_RETURN tret equal(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::equal_(v0.x, v1.x) ? (T)1 : (T)0,
            emp::tt::equal_(v0.y, v1.y) ? (T)1 : (T)0,
            emp::tt::equal_(v0.z, v1.z) ? (T)1 : (T)0,
            emp::tt::equal_(v0.w, v1.w) ? (T)1 : (T)0
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret not_equal(tparam v0, tparam v1)
    { return equal(equal(v0, v1), {0, 0, 0, 0}); }

    static constexpr EMP_INLINE EMP_RETURN tret sin(tparam v)
    {
        return
        {
            mat::sin(v.x),
            mat::sin(v.y),
            mat::sin(v.z),
            mat::sin(v.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret cos(tparam v)
    {
        return
        {
            mat::cos(v.x),
            mat::cos(v.y),
            mat::cos(v.z),
            mat::cos(v.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret sqr(tparam v)
    {
        return
        {
            mat::sqr(v.x),
            mat::sqr(v.y),
            mat::sqr(v.z),
            mat::sqr(v.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret abs(tparam v)
    {
        return
        {
            mat::abs(v.x),
            mat::abs(v.y),
            mat::abs(v.z),
            mat::abs(v.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret max(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::maxof(v0.x, v1.x),
            emp::tt::maxof(v0.y, v1.y),
            emp::tt::maxof(v0.z, v1.z),
            emp::tt::maxof(v0.w, v1.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret min(tparam v0, tparam v1)
    {
        return
        {
            emp::tt::minof(v0.x, v1.x),
            emp::tt::minof(v0.y, v1.y),
            emp::tt::minof(v0.z, v1.z),
            emp::tt::minof(v0.w, v1.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN T max(tparam v)
    { return emp::tt::maxof(emp::tt::maxof(emp::tt::maxof(v.x, v.y), v.z), v.w); }

    static constexpr EMP_INLINE EMP_RETURN tret pi(void) { return all(mat::pi<T>()); }
    static constexpr EMP_INLINE EMP_RETURN tret pipi(void) { return all(mat::pi<T>() * mat::pi<T>()); }

    static constexpr EMP_INLINE EMP_RETURN tret pow(tparam v0, tparam v1)
    {
        return
        {
            mat::pow(v0.x, v1.x),
            mat::pow(v0.y, v1.y),
            mat::pow(v0.z, v1.z),
            mat::pow(v0.w, v1.w)
        };
    }

    static constexpr EMP_INLINE EMP_RETURN tret more(tparam v0, tparam v1);
    static constexpr EMP_INLINE EMP_RETURN tret more_equal(tparam v0, tparam v1);
    static constexpr EMP_INLINE EMP_RETURN tret less(tparam v0, tparam v1);
    static constexpr EMP_INLINE EMP_RETURN tret less_equal(tparam v0, tparam v1);

    static constexpr EMP_INLINE EMP_RETURN tret more_mask(tparam v0, tparam v1) { return more(v0, v1); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T> using v4ret = vret<v4<T>>;
//template <typename T> using v4param = vparam<v4<T>>;

typedef v4<float> v4f;
typedef v4<bool> v4b;

EMP_TYPEDEF(v4f)
EMP_TYPEDEF(v4b)

#if defined EMP_XX_DEVICE_ARM
template <typename T>
using simd_v4 = v4<T>;
#else
template <typename T>
EMP_NOINSTANCE(struct, sse_v4) };

template <>
EMP_NOINSTANCE(struct, sse_v4<float>)
typedef sse_v4f type;
};

template <typename T>
using simd_v4 = typename sse_v4<T>::type;

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(std::is_pod<v2f>::value, "");
static_assert(std::is_pod<v3f>::value, "");
static_assert(std::is_pod<v4f>::value, "");

template <typename T> constexpr EMP_INLINE EMP_RETURN bool bool_and(v2<T> const& v0)
{
    EMP_CONST_ASSERT_NUMBER(v0);
    return (v0.x > 0) && (v0.y > 0);
}

template <typename T> constexpr EMP_INLINE EMP_RETURN bool bool_and(v3<T> const& v0)
{
    EMP_CONST_ASSERT_NUMBER(v0);
    return (v0.x > 0) && (v0.y > 0) && (v0.z > 0);
}

template <typename T> constexpr EMP_INLINE EMP_RETURN bool bool_and(v4<T> const& v0)
{
    EMP_CONST_ASSERT_NUMBER(v0);
    return (v0.x > 0) && (v0.y > 0) && (v0.z > 0) && (v0.w > 0);
}

template <typename T> constexpr EMP_INLINE EMP_RETURN bool bool_or(v2<T> const& v0)
{
    EMP_CONST_ASSERT_NUMBER(v0);
    return (v0.x > 0) || (v0.y > 0);
}

template <typename T> constexpr EMP_INLINE EMP_RETURN bool bool_or(v3<T> const&  v0)
{
    EMP_CONST_ASSERT_NUMBER(v0);
    return (v0.x > 0) || (v0.y > 0) || (v0.z > 0);
}

template <typename T> constexpr EMP_INLINE EMP_RETURN bool bool_or(v4<T> const&  v0)
{
    EMP_CONST_ASSERT_NUMBER(v0);
    return (v0.x > 0) || (v0.y > 0) || (v0.z > 0) || (v0.w > 0);
}

template <typename T>
constexpr EMP_INLINE EMP_RETURN bool bool_equal(T const& v0, T const& v1)
{ return bool_and(T::equal(v0, v1)); }

#if defined EMP_XX_DEVICE_ARM
#else
EMP_INLINE EMP_RETURN bool bool_equal(sse_v4f_d::type v0, sse_v4f_d::type v1)
{ return sse_v4f::bool_equal(v0, v1); }
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

    using namespace emp::mat;

#define XTMP_EQUAL(x_Type)\
template <> EMP_INLINE EMP_RETURN bool equal_<x_Type>(x_Type const t0, x_Type const t1)\
{ return bool_equal(t0, t1); }

XTMP_EQUAL(v2f)
XTMP_EQUAL(v3f)
XTMP_EQUAL(v4f)

#undef XTMP_EQUAL

#define XTMP_MIN_MAX(x_Call, x_Type)\
template <typename T> EMP_NOINSTANCE(struct, x_Call<x_Type<T>>)\
static constexpr x_Type<T> const value = x_Type<T>::all_const(emp::tt::x_Call<T>::value); };\
template <typename T> constexpr x_Type<T> const x_Call<x_Type<T>>::value;

EMP_PRAGMA_PUSH_IGNORE(EMP_W_STATIC_DATA_DEFAULT_CTOR)
XTMP_MIN_MAX(min, v2)
XTMP_MIN_MAX(max, v2)

XTMP_MIN_MAX(min, v3)
XTMP_MIN_MAX(max, v3)

XTMP_MIN_MAX(min, v4)
XTMP_MIN_MAX(max, v4)
EMP_PRAGMA_POP_IGNORE(EMP_W_STATIC_DATA_DEFAULT_CTOR)
#undef XTMP_MIN_MAX

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool is_valid(v2<T> const& v0)
    {
        return
            is_valid(v0.x) &&
            is_valid(v0.y);
    }
   
    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool is_not_valid(v2<T> const& v0)
    { return is_valid(v0) == false; }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool has_max(v2<T> const& v)
    { return is_max(v.x) || is_max(v.y); }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool has_min(v2<T> const& v)
    { return is_min(v.x) || is_min(v.y); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool is_valid(v3<T> const& v0)
    {
        return
            is_valid(v0.x) &&
            is_valid(v0.y) &&
            is_valid(v0.z);
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool is_not_valid(v3<T> const& v0)
    { return is_valid(v0) == false; }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool has_max(v3<T> const& v)
    { return is_max(v.x) || is_max(v.y) || is_max(v.z); }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool has_min(v3<T> const& v)
    { return is_min(v.x) || is_min(v.y) || is_min(v.z); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool is_valid(v4<T> const& v0)
    {
        return
            is_valid(v0.x) &&
            is_valid(v0.y) &&
            is_valid(v0.z) &&
            is_valid(v0.w);
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool is_not_valid(v4<T> const& v0)
    { return is_valid(v0) == false; }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool has_max(v4<T> const& v)
    { return is_max(v.x) || is_max(v.y) || is_max(v.z) || is_max(v.w); }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN bool has_min(v4<T> const& v)
    { return is_min(v.x) || is_min(v.y) || is_min(v.z) || is_min(v.w); }
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

    using namespace emp::tt;


    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::all(T const x)
    {
        EMP_CONST_ASSERT_NUMBER(x);
        return all_const(x);
    }
   
    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::swizzle_yx(v2<T> const& v)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        return{ v.y, v.x };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::add(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return{ v0.x + v1.x, v0.y + v1.y };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::sub(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return{ v0.x - v1.x, v0.y - v1.y };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::mul(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return{ v0.x * v1.x, v0.y * v1.y };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::div(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return{ v0.x / v1.x, v0.y / v1.y };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::more(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x > v1.x ? 1.0f : 0.0f,
            v0.y > v1.y ? 1.0f : 0.0f
        };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::less(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x < v1.x ? 1 : 0,
            v0.y < v1.y ? 1 : 0
        };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::less_equal(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x <= v1.x ? (T)1 : (T)0,
            v0.y <= v1.y ? (T)1 : (T)0
        };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v2<T> v2<T>::more_equal(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x >= v1.x ? (T)1 : (T)0,
            v0.y >= v1.y ? (T)1 : (T)0
        };
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T v2<T>::sqlen(v2<T> const& v0)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        T const fx = v0.x;
        T const fy = v0.y;
        return fx * fx + fy * fy;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    template <typename T>
    EMP_INLINE EMP_RETURN v3<T> v3<T>::all(T const x)
    {
        EMP_CONST_ASSERT_NUMBER(x);
        return all_const(x);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v3<T> v3<T>::all(v2<T> const& v, T const z)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        EMP_CONST_ASSERT_NUMBER(z);
        return all_const(v, z);
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v2<T> v3<T>::xy(v3<T> const& v)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        return {v.x, v.y};
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v3<T> v3<T>::add(v3<T> const& v0, v3<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        //EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x + v1.x,
            v0.y + v1.y,
            v0.z + v1.z
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v3<T> v3<T>::neg(v3<T> const& v0)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        return
        {
            - v0.x,
            - v0.y,
            - v0.z,
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v3<T> v3<T>::sub(v3<T> const& v0, v3<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x - v1.x,
            v0.y - v1.y,
            v0.z - v1.z,
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v3<T> v3<T>::mul(v3<T> const& v0, v3<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x * v1.x,
            v0.y * v1.y,
            v0.z * v1.z,
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v3<T> v3<T>::div(v3<T> const& v0, v3<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x / v1.x,
            v0.y / v1.y,
            v0.z / v1.z
        };
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    template <typename T>
    EMP_INLINE EMP_RETURN v4<T> v4<T>::all(v2<T> const& v0, v2<T> const& v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return v4<T>::all_const(v0, v1);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v4<T> v4<T>::all(v2<T> const& v, T const z, T const w)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        EMP_CONST_ASSERT_NUMBER(z);
        EMP_CONST_ASSERT_NUMBER(w);
        return v4<T>::all_const(v, z, w);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v4<T> v4<T>::all(v3<T> const& v, T const w)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        EMP_CONST_ASSERT_NUMBER(w);
        return v4<T>::all_const(v, w);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN v4<T> v4<T>::all(T const x)
    {
        EMP_CONST_ASSERT_NUMBER(x);
        return v4<T>::all_const(x);
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v2<T> v4<T>::xy(v4<T> const&  v)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        return{ v.x, v.y };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v2<T> v4<T>::zw(v4<T> const& v)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        return{ v.z, v.w };
    }
    
    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v3<T> v4<T>::xyz(v4<T> const&  v)
    {
        EMP_CONST_ASSERT_NUMBER(v);
        return{ v.x, v.y, v.z };
    }
    
    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::add(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x + v1.x,
            v0.y + v1.y,
            v0.z + v1.z,
            v0.w + v1.w,
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::sub(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x - v1.x,
            v0.y - v1.y,
            v0.z - v1.z,
            v0.w - v1.w
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::mul(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x * v1.x,
            v0.y * v1.y,
            v0.z * v1.z,
            v0.w * v1.w
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::div(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_CONST_ASSERT_NUMBER(v0);
        EMP_CONST_ASSERT_NUMBER(v1);
        return
        {
            v0.x / v1.x,
            v0.y / v1.y,
            v0.z / v1.z,
            v0.w / v1.w,
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::more(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_ASSERT_NUMBER(v0);
        EMP_ASSERT_NUMBER(v1);
        return
        {
            v0.x > v1.x ? (T)1 : (T)0,
            v0.y > v1.y ? (T)1 : (T)0,
            v0.z > v1.z ? (T)1 : (T)0,
            v0.w > v1.w ? (T)1 : (T)0
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::more_equal(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_ASSERT_NUMBER(v0);
        EMP_ASSERT_NUMBER(v1);
        return
        {
            v0.x >= v1.x ? (T)1 : (T)0,
            v0.y >= v1.y ? (T)1 : (T)0,
            v0.z >= v1.z ? (T)1 : (T)0,
            v0.w >= v1.w ? (T)1 : (T)0
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::less(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_ASSERT_NUMBER(v0);
        EMP_ASSERT_NUMBER(v1);
        return
        {
            v0.x < v1.x ? 1.0f : 0.0f,
            v0.y < v1.y ? 1.0f : 0.0f,
            v0.z < v1.z ? 1.0f : 0.0f,
            v0.w < v1.w ? 1.0f : 0.0f
        };
    }

    template <typename T>
    constexpr EMP_INLINE EMP_RETURN v4<T> v4<T>::less_equal(v4<T> const&  v0, v4<T> const&  v1)
    {
        EMP_ASSERT_NUMBER(v0);
        EMP_ASSERT_NUMBER(v1);
        return
        {
            v0.x <= v1.x ? 1.0f : 0.0f,
            v0.y <= v1.y ? 1.0f : 0.0f,
            v0.z <= v1.z ? 1.0f : 0.0f,
            v0.w <= v1.w ? 1.0f : 0.0f
        };
    }

} }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void sse_v4f::test_sin(void);
void sse_v4f::test_sin(void)
{
    for (float f = -2*pi<float>(); f <= 2*pi<float>();
        f+=0.001f)
    {
        float const fsin = sin(f);
        type const vsin = sin_v1(set(f));
        float const fDelta = fsin - x(vsin);
        if (abs(fDelta) > 0.00000001f)
        {
            EMP_LOG_INFO(
                "F #0, FSIN #1, VSIN #2, DELTA #3",f, fsin,x(vsin),fDelta);
        }
    }
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//type const two31 = set(0x1.0p31f,0x1.0p31f,0x1.0p31f,0x1.0p31f);
//Convert float to signed int, with AltiVec style overflow
//(i.e. large float -> 0x7fffffff instead of 0x80000000)
//__m128i _mm_cts(__m128 const v)
//{
//    type const overflow = _mm_cmpge_ps(v, two31);
//    type const result = _mm_cvtps_epi32(v);
//    return _mm_xor_ps(result, overflow);
//}
/*void srand_sse(uint seed );
void rand_sse(uint* );
__declspec( align(16) ) static __m128i cur_seed;
void srand_sse(uint seed )
{ cur_seed = _mm_set_epi32( seed, seed+1, seed, seed+1 ); }
inline void rand_sse(uint* result )
{
__declspec( align(16) ) __m128i cur_seed_split;
__declspec( align(16) ) __m128i multiplier;
__declspec( align(16) ) __m128i adder;
__declspec( align(16) ) __m128i mod_mask;
__declspec( align(16) ) __m128i sra_mask;
__declspec( align(16) ) __m128i sseresult;
__declspec( align(16) ) static c_uint mult[4] =
{ 214013, 17405, 214013, 69069 };
__declspec( align(16) ) static c_uint gadd[4] =
{ 2531011, 10395331, 13737667, 1 };
__declspec( align(16) ) static c_uint mask[4] =
{ 0xFFFFFFFF, 0, 0xFFFFFFFF, 0 };
__declspec( align(16) ) static c_uint masklo[4] =
{ 0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF };
adder = _mm_load_si128( (__m128i*) gadd);
multiplier = _mm_load_si128( (__m128i*) mult);
mod_mask = _mm_load_si128( (__m128i*) mask);
sra_mask = _mm_load_si128( (__m128i*) masklo);
cur_seed_split = _mm_shuffle_epi32( cur_seed, _MM_SHUFFLE( 2, 3, 0, 1 ) );
cur_seed = _mm_mul_epu32( cur_seed, multiplier );
multiplier = _mm_shuffle_epi32( multiplier, _MM_SHUFFLE( 2, 3, 0, 1 ) );
cur_seed_split = _mm_mul_epu32( cur_seed_split, multiplier );
cur_seed = _mm_and_si128( cur_seed, mod_mask);
cur_seed_split = _mm_and_si128( cur_seed_split, mod_mask );
cur_seed_split = _mm_shuffle_epi32( cur_seed_split, _MM_SHUFFLE( 2, 3, 0, 1));
cur_seed = _mm_or_si128( cur_seed, cur_seed_split );
cur_seed = _mm_add_epi32( cur_seed, adder);
#ifdef COMPATABILITY
// Add the lines below if you wish to reduce your results to 16-bit vals...
sseresult = _mm_srai_epi32( cur_seed, 16);
sseresult = _mm_and_si128( sseresult, sra_mask );
_mm_storeu_si128( (__m128i*) result, sseresult );
return;
#endif
_mm_storeu_si128( (__m128i*) result, cur_seed);
return;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
