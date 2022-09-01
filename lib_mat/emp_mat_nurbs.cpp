//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_mat_nurbs.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_copy.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nurbs_c::nurbs_c(void):
m_sName(),
m_vfKnots(),
m_vv4Points(),
m_vstPoints(),
m_stDegree(2),
m_vv4Samples()
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*nurbs_c::nurbs_c(nurbs_c const& a_rValue):
m_vfKnots(),
m_vv4Points(),
m_vstPoints(),
m_stDegree(2),
m_vv4Samples()
{
    operator=(a_rValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nurbs_c& nurbs_c::operator=(nurbs_c const& a_rValue)
{
    emp::cnt::copy(a_rValue.m_vfKnots, m_vfKnots);
    emp::cnt::copy(a_rValue.m_vv4Points, m_vv4Points);
    emp::cnt::copy(a_rValue.m_vstPoints, m_vstPoints);
    emp::cnt::copy(a_rValue.m_vv4Samples, m_vv4Samples);

    m_stDegree = a_rValue.m_stDegree;
    return *this;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::create(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::destroy(void)
{
    m_sName = astring();
    m_vv4Points.resize(0);
    m_vstPoints.resize(0);
    m_vfKnots.resize(0);
    m_vv4Samples.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool nurbs_c::pre_load(void)
{
    create();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool nurbs_c::post_load(void)
{
    if (m_vstPoints.count() == 0)
    { compute_knots(); }
    if (m_vv4Samples.count() == 0)
    { compute_samples(); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::compute_knots(void)
{
    c_size stCount = m_vv4Points.count();
    if (stCount >= 2)
    {
        c_size stDegree = degree();
        c_size stKnots = stDegree * 2 + stCount;

        float const fRatio = 1 / emp::ast::cast<float>(stCount);
        m_vfKnots.resize(0);
        m_vstPoints.resize(0);
        for (size_t stIndex = 0; stIndex < stKnots; ++stIndex)
        {
            c_size stKnot = emp::tt::minof(
                emp::tt::maxof(stIndex, stDegree) - stDegree,
                            stKnots - stDegree * 2);

            float const fKnot = emp::ast::cast<float>(stKnot) * fRatio;

            m_vstPoints.add(emp::tt::minof(stKnot, stCount - 1));
            m_vfKnots.add(fKnot);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::compute_samples(void)
{
    if (m_vv4Points.count() >= 2)
    {
//        EMP_LOG_INFO("SAMPLES");
        c_size stCount = m_vfKnots.count();
        m_vv4Samples.resize(stCount);

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            float const fT =
                emp::tt::minof(
                    emp::tt::maxof(0.0f, m_vfKnots.val(stIndex)), 1.0f);
            m_vv4Samples.set(stIndex, sample(fT));
//EMP_LOG_INFO(
//  "SAMPLES : i : #0, k : #1, pi : #2, px : #3, ps : #4",
//  stIndex,
//  m_vfKnots.val(stIndex),
//  m_vstPoints.val(stIndex),
//  emp::mat::v4f::get_x(m_vv4Points.ref(m_vstPoints.val(stIndex))),
//  emp::mat::v4f::get_x(m_vv4Samples.ref(stIndex)));
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t nurbs_c::get_point_index(float const a_fU) const
{
    EMP_ASSERT(points().count() > 1);
    c_float fBegin = m_vv4Points.ref(0).x;
    c_size stDegree = degree();
    c_size stCount = m_vstPoints.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (stIndex + 1 < stCount)
        {
            if (a_fU <= fBegin)
            { return stDegree; }
            if (m_vv4Samples.ref(stIndex + 1).x >= a_fU)
            {
                return stIndex;
            }
        }
        else
        {
            return stIndex - stDegree;
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN size_t nurbs_c::get_point_index(float const a_fU) const
{
    c_size stCount = m_vstPoints.count();
    c_size sti = degree();
    for (size_t stIndex = 0; stIndex < stCount - 1; ++stIndex)
    {
        emp::mat::v4f::tret v4Point =
            m_vv4Points.ref(m_vstPoints.val(stIndex + 1));
        float const fX = emp::mat::v4f::get_x(v4Point);
        if (fX >= a_fU)
        { return sti + stIndex; }
    }
    EMP_ASSERT_UNREACHABLE();
    return sti;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN size_t nurbs_c::get_knot_index(float const a_fU) const
{
    c_size stCount = m_vfKnots.count();
    c_size sti = degree();
    for (size_t stIndex = 0; stIndex < stCount - 1; ++stIndex)
    {
        if (m_vfKnots.val(stIndex + 1) >= a_fU)
        { return sti + stIndex; }
    }
    EMP_ASSERT_UNREACHABLE();
    return sti;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t nurbs_c::get_knot_index(float const a_fU) const
{
    EMP_ASSERT(points().count() > 1);
    c_size stDegree = degree();
    c_size stCount = m_vfKnots.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (stIndex + 1 < stCount)
        {
            if (a_fU <= 0.0f)
            { return stDegree; }
            if (m_vfKnots.val(stIndex + 1) >= a_fU)
            {
                return stIndex;
            }
        }
        else
        { return stIndex; }
    }
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::v4f::tret nurbs_c::sample(c_float a_fTime, c_size a_stDegree) const
{
    EMP_ASSERT(points().count() > 0);
    EMP_ASSERT(a_fTime >= 0);
    EMP_ASSERT(a_fTime <= 1);

    c_size stPointCount = points().count();
    c_size stDegree = emp::tt::minof(stPointCount - 1, a_stDegree);
    c_size stOrder = stDegree + 1;

    c_float fU = a_fTime;

    c_size stKnot = get_knot_index(fU);

    c_size stI = stKnot;

    EMP_ASSERT(stOrder <= 10);
    emp::cnt::array<v3f, 10, true> av3Points;
    av3Points.resize(stOrder);

    for (size_t stIndex = 0; stIndex < av3Points.count(); ++stIndex)
    { av3Points.set(stIndex, emp::mat::v4f::xyz(m_vv4Points.ref(m_vstPoints.val(stI - stDegree + 1 + stIndex)))); }

    for (size_t stK = 1; stK <= stDegree; ++stK)
    {
        for (size_t stJ = stDegree; stJ >= stK; --stJ)
        {
            emp::mat::v3f::tret v3J0 = av3Points.ref(stJ - 1);
            emp::mat::v3f::tret v3J1 = av3Points.ref(stJ);
            emp::mat::v3f::tret v3JDelta = emp::mat::v3f::sub(v3J1, v3J0);

            c_float fU0 = m_vfKnots.val(stI - stDegree + stJ);
            float const fU1 = m_vfKnots.val(stI - stK + 1 + stJ);

            float const fWeight = (fU - fU0) / (fU1 - fU0);

            emp::mat::v2f v2 = emp::mat::v2f::all(fWeight);
            v2 = emp::mat::v2f::one(); // @@0 wtf..

            av3Points.set(stJ,
                emp::mat::v3f::add(v3J0,
                    emp::mat::v3f::mul(v3JDelta,
                        emp::mat::v3f::all(fWeight))));
        }
    }

    return emp::mat::v4f::all(av3Points.ref(stDegree), 1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::v4f::tret nurbs_c::sample(float const a_fTime) const
{ return sample(a_fTime, degree()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float nurbs_c::solve(float const a_fValue) const
{
    EMP_ASSERT(points().count() > 0);
    c_size stDegree = emp::tt::minof(points().count() - 1, degree());
    if (stDegree == 1)
    { return solve_linear(a_fValue); }
    else if (stDegree >= 2)
    { return solve_quadratic(a_fValue); }
    EMP_ASSERT_UNREACHABLE();
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float nurbs_c::solve_quadratic(float const a_fValue) const
{
    EMP_ASSERT(m_vv4Points.count() > 2);

    c_size stDegree = emp::tt::minof(points().count() - 1, degree());
    c_size sti = get_point_index(a_fValue);

    //EMP_LOG_INFO("STI : #0, #1", sti, a_fValue);

    c_size st0 = sti - stDegree + 1u;
    c_size st1 = sti - stDegree + 2u;
    c_size st2 = sti - stDegree + 3u;

    c_v4f& v0 = m_vv4Points.ref(m_vstPoints.val(st0));
    c_v4f& v1 = m_vv4Points.ref(m_vstPoints.val(st1));
    c_v4f& v2 = m_vv4Points.ref(m_vstPoints.val(st2));

    c_float p0 = v0.x;
    c_float p1 = v1.x;
    c_float p2 = v2.x;

    c_float p01 = p1 - p0;
    c_float p12 = p2 - p1;

    size_t k = 0;
    size_t j = 0;

    k = 1;
    j = 2;
    c_size e00 = sti - stDegree + j;
    c_size e01 = sti - k + 1 + j;
    c_float k00 = m_vfKnots.val(e00);
    c_float k01 = m_vfKnots.val(e01);

    k = 1;
    j = 1;
    c_size e10 = sti - stDegree + j;
    c_size e11 = sti - k + 1 + j;
    c_float k10 = m_vfKnots.val(e10);
    c_float k11 = m_vfKnots.val(e11);

    k = 2;
    j = 2;
    c_size e20 = sti - stDegree + j;
    c_size e21 = sti - k + 1 + j;
    c_float k20 = m_vfKnots.val(e20);
    c_float k21 = m_vfKnots.val(e21);

    c_float b0 = 1 / (k01 - k00);
    c_float b1 = 1 / (k11 - k10);
    c_float b2 = 1 / (k21 - k20);

    c_float a0 = -k00 * b0;
    c_float a1 = -k10 * b1;
    c_float a2 = -k20 * b2;

    c_float A = b2*(b0*p12 - b1*p01);
    c_float B = p01*(b1 + b2 - a2*b1 - b2*a1) + p12*(a2*b0 + b2*a0);
    c_float C = p0 + p01*(a1 + a2 - a2*a1)+ p12*a2*a0 - a_fValue;

    c_float fE = 0.000001f;
    float fU = -1.0f;
    if (emp::mat::abs(A) < fE)
    {
        if (emp::mat::abs(B) < fE)
        { return 0.0f; }
        else
        {
            fU = -C / B;
            EMP_ASSERT_NUMBER(fU);
            EMP_ASSERT(fU >= 0);
            EMP_ASSERT(fU <= 1);
                EMP_ASSERT_NUMBER(fU);
                EMP_ASSERT(fU >= 0);
                EMP_ASSERT(fU <= 1);
        }
    }
    else
    {
        c_float fInv2A = 1 / (2 * A);
        c_float D = B*B - 4*A*C;
        if (D > fE)
        {
            c_float RD = sqrtf(D);
            c_float r0 = (-B + RD) * fInv2A;
            c_float r1 = (-B - RD) * fInv2A;
            if (r0 >= (k00 - fE)  && r0 <= (k01 + fE))
            { fU = emp::tt::minof(emp::tt::maxof(0.0f, r0), 1.0f); }
            else if (r1 >= (k00 - fE)  && r1 <= (k01 + fE))
            { fU = emp::tt::minof(emp::tt::maxof(0.0f, r1), 1.0f); }
            else
            { EMP_ASSERT(false); }
        }
        else if (D < -fE)
        {
            return 1.0f;
            //EMP_ASSERT(false);
        }
        else
        {
            fU = emp::tt::minof(emp::tt::maxof(0.0f, -B * fInv2A), 1.0f);
        }
    }
    EMP_ASSERT_NUMBER(fU);
    EMP_ASSERT(fU >= 0);
    EMP_ASSERT(fU <= 1);
    return fU;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN float nurbs_c::solve_quadratic(float const a_fValue) const
{
    //EMP_LOG_INFO("SOLVE : #0", a_fValue);
    EMP_ASSERT(m_vv4Points.count() > 2);

    size_t sti = get_point_index(a_fValue);

    double k0 = m_vfKnots.val(sti);
    double k1 = m_vfKnots.val(sti + 1);

    k0 = m_vfKnots.val(sti);
    k1 = m_vfKnots.val(sti + 1);
    p1 = emp::mat::v4f::get_x(sample(emp::ast::cast<float>(k1)));
    double p0 = emp::mat::v4f::get_x(sample(emp::ast::cast<float>(k0)));
    EMP_ASSERT(a_fValue >= p0);
    EMP_ASSERT(a_fValue <= p1);
    size_t iter = 0;
    while (true)
    {
        double k = k0 + 0.5 * (k1 - k0);
        double p = emp::mat::v4f::get_x(sample(emp::ast::cast<float>(k)));
        //float const fk0 = emp::ast::cast<float>(k0);
        //float const fk1 = emp::ast::cast<float>(k1);
        //float const fk = emp::ast::cast<float>(k);
        //float const fp0 = emp::ast::cast<float>(p0);
        //float const fp1 = emp::ast::cast<float>(p1);
        //float const fp = emp::ast::cast<float>(p);
        //EMP_LOG_INFO("#0, #1, #2, #3, #4, #5", fk0, fk, fk1, fp0, fp, fp1);
        if (p >= a_fValue)
        {
            p1 = p;
            k1 = k;
        }
        else if (p <= a_fValue)
        {
            p0  = p;
            k0 = k;
        }
        else
        { EMP_ASSERT(false); }
        if (emp::mat::abs(p - a_fValue) < 0.0001)
        { return emp::ast::cast<float>(k); }
        iter++;
        if (iter == 50)
        { EMP_ASSERT(false);
            break;}
    }
    EMP_ASSERT(false);
    return 0;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float nurbs_c::solve_linear(c_float a_fValue) const
{
    EMP_ASSERT(m_vv4Points.count() > 1);
    EMP_ASSERT(a_fValue >= m_vv4Points.first_val().x);
    EMP_ASSERT(a_fValue <= m_vv4Points.last_val().x);

    EMP_XX_IF_ASSERT(bool bFound = false);
    c_size stCount = m_vv4Points.count();
    size_t st0 = 0;
    size_t st1 = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        st0 = st1;
        st1 = stIndex;
        emp::mat::v4f::tret v4Point = m_vv4Points.ref(stIndex);
        c_float fX = v4Point.x;
        if (fX >= a_fValue)
        {
            EMP_XX_IF_ASSERT(bFound = true);
            break;
        }
    }
    st1 = st0 + 1;
    EMP_ASSERT(bFound);

    c_v4f& v0 = m_vv4Points.ref(st0);
    c_v4f& v1 = m_vv4Points.ref(st1);
    c_v4f& vDelta = emp::mat::v4f::sub(v1, v0);

    c_float fX = v0.x;
    c_float fD = vDelta.x;

    c_float r0 = (a_fValue - fX) / fD;
    EMP_ASSERT_NUMBER(r0);

    c_float fk0 = m_vfKnots.val(degree() + st0);
    c_float fk1 = m_vfKnots.val(degree() + st1);
    c_float fU = r0 * (fk1 - fk0) + fk0;
    EMP_ASSERT_NUMBER(fU);
    EMP_ASSERT(fU >= 0);
    EMP_ASSERT(fU <= 1);
    return fU;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::set_point(c_size a_stIndex, emp::mat::v4f::tparam a_v4Position)
{
    if (emp::mat::bool_equal(
        a_v4Position, m_vv4Points.ref(a_stIndex)) == false)
    {
        m_vv4Points.set(a_stIndex, a_v4Position);
        compute_samples();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t nurbs_c::move_point(c_size a_stIndex,
                                    emp::mat::v4f::tparam a_v4Delta)
{
    c_v4f& v4Current = points().ref(a_stIndex);

    m_vv4Points.set(a_stIndex, emp::mat::v4f::add(v4Current, a_v4Delta));

    c_float fCurrentX = v4Current.x;

    size_t stNewIndex = a_stIndex;

    if (a_v4Delta.x < 0)
    {
        c_size stCount = emp::tt::maxof<size_t>(1, a_stIndex);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stOffset = stCount - stIndex - 1;
            if (points().ref(stOffset).x < fCurrentX)
            { break; }
            stNewIndex = stOffset;
        }
    }
    else
    {
        c_size stCount = points().count();
        for (size_t stIndex = a_stIndex + 1; stIndex < stCount; ++stIndex)
        {
            if (points().ref(stIndex).x > fCurrentX)
            { break; }
            stNewIndex = stIndex;
        }
    }

    if (stNewIndex != a_stIndex)
    {
        m_vv4Points.swap(a_stIndex, stNewIndex);
    }
    compute_samples();
    return stNewIndex;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::add_knot(float const a_fValue)
{
    m_vfKnots.add(a_fValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::add_point(emp::mat::v4f::tparam a_v4Point, c_bool EMP_SILENT(a_bAutoKnot))
{
    m_vv4Points.add(a_v4Point);
    //if (a_bAutoKnot)
    {
        compute_knots();
        compute_samples();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::insert_point(emp::mat::v4f::tparam a_v4Position)
{
    c_size stCount = points().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::v4f::tret v4Point = points().ref(stIndex);
        if (v4Point.x > a_v4Position.x)
        {
            insert_point(stIndex, a_v4Position);
            return;
        }
    }
    add_point(a_v4Position, true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::insert_point(c_size a_stIndex, emp::mat::v4f::tparam a_v4Point)
{
    m_vv4Points.insert(a_stIndex, a_v4Point);
    compute_knots();
    compute_samples();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::remove_point(c_size a_stIndex)
{
    m_vv4Points.remove(a_stIndex);
    compute_knots();
    compute_samples();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool nurbs_c::contains(emp::mat::v4f::tparam a_v4Client,
                                emp::mat::v4f::tparam a_v4Point,
                                emp::mat::v4f::tparam a_v4Size)
{
    float const fWidth = emp::mat::v4f::get_x(a_v4Size);
    float const fHeight = emp::mat::v4f::get_y(a_v4Size);

    float const fHalfWidth = fWidth * 0.5f;
    float const fHalfHeight = fHeight * 0.5f;

    float const fPointX = emp::mat::v4f::get_x(a_v4Point);
    float const fPointY = emp::mat::v4f::get_y(a_v4Point);

    float const fX0 = fPointX - fHalfWidth;
    float const fX1 = fPointX + fHalfWidth;

    float const fY0 = fPointY - fHalfHeight;
    float const fY1 = fPointY + fHalfHeight;

    float const fX = emp::mat::v4f::get_x(a_v4Client);
    float const fY = emp::mat::v4f::get_y(a_v4Client);

    if (fX >= fX0 &&
        fX <= fX1 &&
        fY >= fY0 &&
        fY <= fY1)
    { return true; }
    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*size_t nurbs_c::contains(   emp::mat::v4f::tparam a_v4Position,
                            emp::mat::v4f::tparam a_v4Size) const
{
    c_size stCount = points().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (contains(a_v4Position, points().ref(stIndex), a_v4Size))
        { return stIndex; }
    }
    return emp::tt::max<size_t>::value;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nurbs_c::set_degree(c_size a_stValue)
{
    m_stDegree = a_stValue;
    compute_knots();
    compute_samples();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
