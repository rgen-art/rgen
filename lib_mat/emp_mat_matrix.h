//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_MATRIX_H
#define EMP_MAT_MATRIX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
struct m44
{
public:
    T f00;
    T f01;
    T f02;
    T f03;

    T f10;
    T f11;
    T f12;
    T f13;

    T f20;
    T f21;
    T f22;
    T f23;

    T f30;
    T f31;
    T f32;
    T f33;

public:
    typedef T const c_T;
    typedef m44 type;
    typedef type const& tparam;
    typedef v4<T> vtype;
    typedef vtype const c_vtype;

    static EMP_INLINE vret<vtype> row_x(tparam& m) { return { m.f00, m.f01, m.f02, m.f03 }; }
    static EMP_INLINE vret<vtype> row_y(tparam& m) { return { m.f10, m.f11, m.f12, m.f13 }; }
    static EMP_INLINE vret<vtype> row_z(tparam& m) { return { m.f20, m.f21, m.f22, m.f23 }; }
    static EMP_INLINE vret<vtype> row_w(tparam& m) { return { m.f30, m.f31, m.f32, m.f33 }; }
    static EMP_INLINE vret<vtype> col_x(tparam& m) { return { m.f00, m.f10, m.f20, m.f30 }; }
    static EMP_INLINE vret<vtype> col_y(tparam& m) { return { m.f01, m.f11, m.f21, m.f31 }; }
    static EMP_INLINE vret<vtype> col_z(tparam& m) { return { m.f02, m.f12, m.f22, m.f32 }; }
    static EMP_INLINE vret<vtype> col_w(tparam& m) { return { m.f03, m.f13, m.f23, m.f33 }; }

    static constexpr EMP_INLINE EMP_RETURN m44 ident(void) { return {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}; }

    static EMP_INLINE void set_row_x(type& m, vparam<vtype> v)
    {
        m.f00 = v.x;
        m.f01 = v.y;
        m.f02 = v.z;
        m.f03 = v.w;
    }

    static EMP_INLINE void set_row_y(type& m, vparam<vtype> v)
    {
        m.f10 = v.x;
        m.f11 = v.y;
        m.f12 = v.z;
        m.f13 = v.w;
    }

    static EMP_INLINE void set_row_z(type& m, vparam<vtype> v)
    {
        m.f20 = v.x;
        m.f21 = v.y;
        m.f22 = v.z;
        m.f23 = v.w;
    }

    static EMP_INLINE void set_row_w(type& m, vparam<vtype> v)
    {
        m.f30 = v.x;
        m.f31 = v.y;
        m.f32 = v.z;
        m.f33 = v.w;
    }

    static EMP_INLINE EMP_RETURN bool equals(tparam m0, tparam m1)
    {
        return
            emp::tt::equal_(m0.f00, m1.f00) &&
            emp::tt::equal_(m0.f01, m1.f01) &&
            emp::tt::equal_(m0.f02, m1.f02) &&
            emp::tt::equal_(m0.f03, m1.f03) &&
            emp::tt::equal_(m0.f10, m1.f10) &&
            emp::tt::equal_(m0.f11, m1.f11) &&
            emp::tt::equal_(m0.f12, m1.f12) &&
            emp::tt::equal_(m0.f13, m1.f13) &&
            emp::tt::equal_(m0.f20, m1.f20) &&
            emp::tt::equal_(m0.f21, m1.f21) &&
            emp::tt::equal_(m0.f22, m1.f22) &&
            emp::tt::equal_(m0.f23, m1.f23) &&
            emp::tt::equal_(m0.f30, m1.f30) &&
            emp::tt::equal_(m0.f31, m1.f31) &&
            emp::tt::equal_(m0.f32, m1.f32) &&
            emp::tt::equal_(m0.f33, m1.f33);
    }

    static void EMP_INLINE identity(type& m)
    {
        m.f00 = 1;
        m.f01 = 0;
        m.f02 = 0;
        m.f03 = 0;

        m.f10 = 0;
        m.f11 = 1;
        m.f12 = 0;
        m.f13 = 0;

        m.f20 = 0;
        m.f21 = 0;
        m.f22 = 1;
        m.f23 = 0;

        m.f30 = 0;
        m.f31 = 0;
        m.f32 = 0;
        m.f33 = 1;
    }

    static void EMP_INLINE projection(c_T a_fWidth, c_T a_fHeight, c_T a_fNear, c_T a_fFar, c_T a_fHorzScale, c_T a_fVertScale, m44& m)
    {
        EMP_ASSERT(a_fWidth > 0);
        EMP_ASSERT(a_fHeight > 0);

        EMP_ASSERT(a_fNear > 0);
        EMP_ASSERT(a_fFar > 0);
        EMP_ASSERT(a_fFar > a_fNear);

        EMP_ASSERT(a_fHorzScale > 0);
        EMP_ASSERT(a_fVertScale > 0);

        c_float fN = -a_fNear;
        c_float fF = -a_fFar;

        c_float f2N = 2 * fN;
        c_float fInvfsn = -1 / (fF - fN);

//(-f -n) * -1 / (-f --n)
//-(f+n) * -1/(n - f)

// 2 * -n * -f* -1/(-f --n)
// 2 * -n * -f * -1/(n - f)
// -2 * nf/(n-f)
        m.f00 = f2N * a_fHorzScale / a_fWidth;
        m.f01 = 0;
        m.f02 = 0;
        m.f03 = 0;

        m.f10 = 0;
        m.f11 = f2N * a_fVertScale / a_fHeight;
        m.f12 = 0;
        m.f13 = 0;

        m.f20 = 0;
        m.f21 = 0;
        m.f22 = (fF + fN) * fInvfsn;
        m.f23 = f2N * fN * fInvfsn;

        m.f30 = 0;
        m.f31 = 0;
        m.f32 = -1;
        m.f33 = 0;
    }

    static EMP_INLINE EMP_RETURN vret<vtype> mul_v4(tparam m, vparam<vtype> vin)
    {
         c_vtype v0 = vtype::mul(col_x(m), vin);
         c_vtype v1 = vtype::mul(col_y(m), vin);
         c_vtype v2 = vtype::mul(col_z(m), vin);
         c_vtype v3 = vtype::mul(col_w(m), vin);

        c_vtype vr0 = { v0.x, v1.x, v2.x, v3.x };
        c_vtype vr1 = { v0.y, v1.y, v2.y, v3.y };
        c_vtype vr2 = { v0.z, v1.z, v2.z, v3.z };
        c_vtype vr3 = { v0.w, v1.w, v2.w, v3.w };

        c_vtype vout = vtype::add(vtype::add(vr0, vr1), vtype::add(vr2, vr3));

        return vout;
    }

    static EMP_INLINE EMP_RETURN v3<T> transform_vector(v3<T> const a, m44<T> const& m)
    {
        return
        {
            a.x * m.f00 + a.y * m.f10 + a.z * m.f20,
            a.x * m.f01 + a.y * m.f11 + a.z * m.f21,
            a.x * m.f02 + a.y * m.f12 + a.z * m.f22
        };
    }
    static EMP_INLINE EMP_RETURN v3<T> transform_point(v3<T> const a, m44<T> const& m)
    {
        return
        {
            a.x * m.v00 + a.y * m.v10 + a.z * m.v20 + m.v30,
            a.x * m.v01 + a.y * m.v11 + a.z * m.v21 + m.v31,
            a.x * m.v02 + a.y * m.v12 + a.z * m.v22 + m.v32
        };
    }
    static void output(type const& EMP_XX_IF_LOG(m))
    {
        EMP_LOG_INFO("0 : #0, #1, #2, #3", m.f00, m.f01, m.f02, m.f03);
        EMP_LOG_INFO("1 : #0, #1, #2, #3", m.f10, m.f11, m.f12, m.f13);
        EMP_LOG_INFO("2 : #0, #1, #2, #3", m.f20, m.f21, m.f22, m.f23);
        EMP_LOG_INFO("3 : #0, #1, #2, #3", m.f30, m.f31, m.f32, m.f33);
    }

    static EMP_INLINE void mul(tparam m1, tparam m0, type& mout)
    {
        c_vtype v00 = row_x(m0);
        c_vtype v01 = row_y(m0);
        c_vtype v02 = row_z(m0);
        c_vtype v03 = row_w(m0);

        c_vtype v10 = row_x(m1);
        c_vtype v11 = row_y(m1);
        c_vtype v12 = row_z(m1);
        c_vtype v13 = row_w(m1);

        vtype v4Zero = vtype::all(0);

        vtype vr0;
        vtype vr1;
        vtype vr2;
        vtype vr3;

        vr0 = vtype::mul_add(vtype::splat_x(v00), v10, v4Zero);
        vr1 = vtype::mul_add(vtype::splat_x(v01), v10, v4Zero);
        vr2 = vtype::mul_add(vtype::splat_x(v02), v10, v4Zero);
        vr3 = vtype::mul_add(vtype::splat_x(v03), v10, v4Zero);

        vr0 = vtype::mul_add(vtype::splat_y(v00), v11, vr0);
        vr1 = vtype::mul_add(vtype::splat_y(v01), v11, vr1);
        vr2 = vtype::mul_add(vtype::splat_y(v02), v11, vr2);
        vr3 = vtype::mul_add(vtype::splat_y(v03), v11, vr3);

        vr0 = vtype::mul_add(vtype::splat_z(v00), v12, vr0);
        vr1 = vtype::mul_add(vtype::splat_z(v01), v12, vr1);
        vr2 = vtype::mul_add(vtype::splat_z(v02), v12, vr2);
        vr3 = vtype::mul_add(vtype::splat_z(v03), v12, vr3);

        vr0 = vtype::mul_add(vtype::splat_w(v00), v13, vr0);
        vr1 = vtype::mul_add(vtype::splat_w(v01), v13, vr1);
        vr2 = vtype::mul_add(vtype::splat_w(v02), v13, vr2);
        vr3 = vtype::mul_add(vtype::splat_w(v03), v13, vr3);

        set_row_x(mout, vr0);
        set_row_y(mout, vr1);
        set_row_z(mout, vr2);
        set_row_w(mout, vr3);
    }

    static EMP_INLINE void rotation_x(c_T a_fAngle, type& mout)
    {
        c_float fCos = cosf(a_fAngle);
        c_float fSin = sinf(a_fAngle);

        mout.f00 = 1;
        mout.f01 = 0;
        mout.f02 = 0;
        mout.f03 = 0;

        mout.f10 = 0;
        mout.f11 = fCos;
        mout.f12 = -fSin;
        mout.f13 = 0;

        mout.f20 = 0;
        mout.f21 = fSin;
        mout.f22 = fCos;
        mout.f23 = 0;

        mout.f30 = 0;
        mout.f31 = 0;
        mout.f32 = 0;
        mout.f33 = 1;
    }

    static EMP_INLINE void rotation_y(c_T a_fAngle, type& mout)
    {
        c_float fCos = cosf(a_fAngle);
        c_float fSin = sinf(a_fAngle);

        mout.f00 = fCos;
        mout.f01 = 0;
        mout.f02 = fSin;
        mout.f03 = 0;

        mout.f10 = 0;
        mout.f11 = 1;
        mout.f12 = 0;
        mout.f13 = 0;

        mout.f20 = -fSin;
        mout.f21 = 0;
        mout.f22 = fCos;
        mout.f23 = 0;

        mout.f30 = 0;
        mout.f31 = 0;
        mout.f32 = 0;
        mout.f33 = 1;
    }

    static EMP_INLINE void rotation_z(c_T a_fAngle, type& mout)
    {
        c_float fCos = cosf(a_fAngle);
        c_float fSin = sinf(a_fAngle);

        mout.f00 = fCos;
        mout.f01 = -fSin;
        mout.f02 = 0;
        mout.f03 = 0;

        mout.f10 = fSin;
        mout.f11 = fCos;
        mout.f12 = 0;
        mout.f13 = 0;

        mout.f20 = 0;
        mout.f21 = 0;
        mout.f22 = 1;
        mout.f23 = 0;

        mout.f30 = 0;
        mout.f31 = 0;
        mout.f32 = 0;
        mout.f33 = 1;
    }

    static EMP_INLINE void flip(c_float a_fAngle, type& mout)
    {
        c_float fAngle = 2 * a_fAngle;
        c_float fCos2X = cosf(fAngle);
        c_float fSin2X = sinf(fAngle);

        mout.f00 =  fCos2X;
        mout.f01 =  fSin2X;
        mout.f02 = 0;
        mout.f03 = 0;

        mout.f10 =  fSin2X;
        mout.f11 = -fCos2X;
        mout.f12 = 0;
        mout.f13 = 0;

        mout.f20 = 0;
        mout.f21 = 0;
        mout.f22 = 1;
        mout.f23 = 0;

        mout.f30 = 0;
        mout.f31 = 0;
        mout.f32 = 0;
        mout.f33 = 1;
    }

    static EMP_INLINE void scale(tparam m, typename v3<T>::tparam vin, type& mout)
    {
        typename v3<T>::type const vres = v3<T>::mul({ m.f00, m.f11, m.f22 }, vin);

        mout.f00 = vres.x;
        mout.f11 = vres.y;
        mout.f22 = vres.z;
    }

    static EMP_INLINE void translate(tparam m, typename v3<T>::tparam vin, type& mout)
    {
        typename v3<T>::type const vres = v3<T>::add({ m.f30, m.f31, m.f32 }, vin);

        mout.f30 = vres.x;
        mout.f31 = vres.y;
        mout.f32 = vres.z;
    }

    static EMP_INLINE EMP_RETURN typename v3<T>::tret translation_v3(tparam m)
    { return v3<T>::make(m.f30, m.f31, m.f32); }

    static EMP_INLINE void transpose(tparam m, type& mout)
    {
        c_float f00 = m.f00;
        c_float f01 = m.f01;
        c_float f02 = m.f02;
        c_float f03 = m.f03;

        c_float f10 = m.f10;
        c_float f11 = m.f11;
        c_float f12 = m.f12;
        c_float f13 = m.f13;

        c_float f20 = m.f20;
        c_float f21 = m.f21;
        c_float f22 = m.f22;
        c_float f23 = m.f23;

        c_float f30 = m.f30;
        c_float f31 = m.f31;
        c_float f32 = m.f32;
        c_float f33 = m.f33;

        mout.f00 = f00;
        mout.f01 = f10;
        mout.f02 = f20;
        mout.f03 = f30;

        mout.f10 = f01;
        mout.f11 = f11;
        mout.f12 = f21;
        mout.f13 = f31;

        mout.f20 = f02;
        mout.f21 = f12;
        mout.f22 = f22;
        mout.f23 = f32;

        mout.f30 = f03;
        mout.f31 = f13;
        mout.f32 = f23;
        mout.f33 = f33;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef m44<float> m44f;

EMP_TYPEDEF(m44f)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE void stack_mul(T const& a_tCurrent, T const& a_tValue, T& a_rtOut)
{ return T::mul(a_tCurrent, a_tValue, a_rtOut); }

template <>
EMP_INLINE void stack_mul(c_bool& a_bCurrent, c_bool& a_bValue, bool& a_rbOut)
{ a_rbOut = a_bCurrent && a_bValue; }

template <typename T>
EMP_NOCOPY(class, stack_t)
public:
    emp::cnt::vector<T> data;

public:
    stack_t(void):
        data()
    {}

    ~stack_t(void)
    {}

public:
    void destroy(void)
    {
        data.resize(0);
    }

    void push(T const& a_rValue)
    {
        c_size stCount = data.count();
        if (stCount > 0)
        {
            T& tNext = data.push();
            T const& tCurrent = data.ref(stCount - 1);
            stack_mul(tCurrent, a_rValue, tNext);
        }
        else
        {
            data.add(a_rValue);
        }
    }

    void pop(void)
    {
        if (data.count())
        { data.resize(data.count() - 1); }
        else
        { EMP_ASSERT_UNREACHABLE(); }
    }

    EMP_RETURN size_t count(void) const { return data.count(); }
    EMP_RETURN T const& current(void) const { return data.last_ref(); }

    //void set_last(T const& a_rValue) { m_vtData.last_ref() = a_rValue; }
    void add(T const& a_rValue) { data.add(a_rValue); }

public:
    EMP_RETURN size_t byte_count(void) const
    { return data.size() * sizeof(T); }
};

typedef m44<float> m44f;
EMP_TYPEDEF(m44f)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static_assert(std::is_pod<m44f>::value, "");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
