//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_mat_glyph.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_ccw(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2);
EMP_RETURN bool is_ccw(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2)
{
    c_float x0 = p0.x;
    c_float y0 = p0.y;
    c_float x1 = p1.x;
    c_float y1 = p1.y;
    c_float x2 = p2.x;
    c_float y2 = p2.y;
    
    c_float f = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
    return f >= 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_ccw(cpc_v2f* const a_ppP, c_size a_stCount);
EMP_RETURN bool is_ccw(cpc_v2f* const a_ppP, c_size a_stCount)
{
    if (a_stCount == 3)
    { return is_ccw(*a_ppP[0], *a_ppP[1], *a_ppP[2]); }
    emp::cnt::shell<cpc_v2f> spP(a_ppP, a_stCount, true);

    c_size stCount = spP.count();
    EMP_ASSERT(stCount > 2);

    float fSum = 0.0f;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stP0 = (stIndex + 0) % stCount;
        c_size stP1 = (stIndex + 1) % stCount;
        cpc_v2f p0 = spP.val(stP0);
        cpc_v2f p1 = spP.val(stP1);
        c_float x0 = p0->x;
        c_float y0 = p0->y;
        c_float x1 = p1->x;
        c_float y1 = p1->y;
        c_float f = (x1 - x0) * (y1 + y0);
        fSum += f;
    }

    return fSum < 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float sq_distance(v2f::tparam p0, v2f::tparam p1); // @@0 dot
EMP_RETURN float sq_distance(v2f::tparam p0, v2f::tparam p1) // @@0 dot
{
    c_float x0 = p0.x;
    c_float y0 = p0.y;
    c_float x1 = p1.x;
    c_float y1 = p1.y;

    c_float dx = x1 - x0;
    c_float dy = y1 - y0;

    return dx * dx + dy * dy;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float point_line_sq_distance(v2f::tparam p, v2f::tparam v0, v2f::tparam v1);
EMP_RETURN float point_line_sq_distance(v2f::tparam p, v2f::tparam v0, v2f::tparam v1)
{
    c_v2f ab = v2f::sub(v1, v0);
    c_v2f ap = v2f::sub(p, v0);
    c_v2f vn = v2f::mul(ap, ab);

    c_float n = vn.x + vn.y;
    EMP_ASSERT_NUMBER(n);

    c_v2f vd = v2f::mul(ab, ab);
    c_float d = vd.x + vd.y;
    EMP_ASSERT_NUMBER(d);

    c_float s = n / d;
    EMP_ASSERT_NUMBER(s);

    c_v2f np = v2f::add(v0, v2f::mul(ab, v2f::all(s)));
    c_v2f vnd = v2f::sub(np, p);
    c_v2f vsqd = v2f::mul(vnd, vnd);

    c_float sq = vsqd.x + vsqd.y;
    EMP_ASSERT_NUMBER(sq);
    return sq;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool segment_intersect_segment(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2, v2f::tparam p3);
EMP_RETURN bool segment_intersect_segment(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2, v2f::tparam p3)
{
    //            (Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy)
    //        r = -----------------------------  (eqn 1)
    //            (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
    //            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
    //        s = -----------------------------  (eqn 2)
    //            (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
    //        If 0<=r<=1 & 0<=s<=1, intersection exists
    //        If r<0 or r>1 or s<0 or s>1 line segments do not intersect
    //        If the denominator in eqn 1 is zero, AB & CD are parallel
    //        If the numerator in eqn 1 is also zero, AB & CD are collinear.
    c_float fEpsilon = emp::tt::epsilon<float>();

    c_float ax = p0.x;
    c_float ay = p0.y;
    c_float bx = p1.x;
    c_float by = p1.y;
    c_float cx = p2.x;
    c_float cy = p2.y;
    c_float dx = p3.x;
    c_float dy = p3.y;

    c_float bxax = bx - ax;
    c_float dycy = dy - cy;
    c_float byay = by - ay;
    c_float dxcx = dx - cx;
    c_float d = bxax * dycy - byay * dxcx;
    if (emp::mat::abs(d) < fEpsilon)
    { return false; }

    c_float aycy = ay - cy;
    c_float axcx = ax - cx;
    c_float rn = aycy * dxcx - axcx * dycy;
    if (emp::mat::abs(rn) < fEpsilon)
    { return false; }

    c_float id = 1 / d;

    c_float r = rn * id;
    if (r < 0 || r > 1)
    { return false; }

    c_float s = (aycy * bxax - axcx * byay) * id;
    if (s < 0 || s > 1)
    { return false; }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void circumcenter(v2f::tparam A, v2f::tparam B, v2f::tparam C, v2f& a_rCenter);
void circumcenter(v2f::tparam A, v2f::tparam B, v2f::tparam C, v2f& a_rCenter)
{
    c_float ax = A.x;
    c_float ay = A.y;
    c_float bx = B.x;
    c_float by = B.y;
    c_float cx = C.x;
    c_float cy = C.y;

    c_float bycy = by - cy;
    c_float cyay = cy - ay;
    c_float ayby = ay - by;

    c_float id = 1 / (2 * (ax * bycy + bx * cyay + cx * ayby));

    c_float ax2 = ax * ax;
    c_float ay2 = ay * ay;
    c_float bx2 = bx * bx;
    c_float by2 = by * by;
    c_float cx2 = cx * cx;
    c_float cy2 = cy * cy;

    c_float ay2ax2 = ay2 + ax2;
    c_float by2bx2 = by2 + bx2;
    c_float cy2cx2 = cy2 + cx2;

    c_float ccx = (ay2ax2 * bycy + by2bx2 * cyay + cy2cx2 * ayby) * id;

    c_float cxbx = cx - bx;
    c_float axcx = ax - cx;
    c_float bxax = bx - ax;

    c_float ccy = (ay2ax2 * cxbx + by2bx2 * axcx + cy2cx2 * bxax) * id;

    a_rCenter = { ccx, ccy };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool point_on_segment(v2f::tparam p, v2f::tparam l0, v2f::tparam l1);
EMP_RETURN bool point_on_segment(v2f::tparam p, v2f::tparam l0, v2f::tparam l1)
{
    c_float x = p.x;
    c_float y = p.y;

    c_float x0 = l0.x;
    c_float y0 = l0.y;

    c_float x1 = l1.x;
    c_float y1 = l1.y;

    c_float abx = x1-x0;
    c_float aby = y1-y0;

    c_float apx = x-x0;
    c_float apy = y-y0;

    c_float n = apx*abx + apy*aby;
    if (n < 0.0f)
    { return false; }

    c_float d = abx*abx + aby*aby;
    if (n > d)
    { return false; }

    c_float s = n / d;
    c_float px = x0 + s * abx;
    c_float py = y0 + s * aby;

    c_float dx = px - x;
    c_float dy = py - y;

    float sq = dx*dx + dy*dy;

    return sq < emp::mat::epsilon<float>();

    /*Vector2 D = B-A;
    float numer = dot(P-A,D);
    if (numer <= 0.0f)
        return A;
    float denom = dot(D,D);
    if (numer >= denom)
        return B;
    return A + (numer/denom) * D;}*/
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, tricell_c)
public:
    bool need_flip;

    c_v2f* p0;
    c_v2f* p1;
    c_v2f* p2;

    tricell_c* c0;
    tricell_c* c1;
    tricell_c* c2;


public:
    tricell_c(void):
        need_flip(false), p0(), p1(), p2(), c0(nullptr), c1(nullptr), c2(nullptr)
    {}

    EMP_RETURN c_v2f* get_point(cpc_v2f a_p0, cpc_v2f a_p1) const
    {
        EMP_ASSERT(p0 != nullptr);
        EMP_ASSERT(p1 != nullptr);
        EMP_ASSERT(p2 != nullptr);

        if (p0 != a_p0 && p0 != a_p1) { return p0; }
        if (p1 != a_p0 && p1 != a_p1) { return p1; }
        if (p2 != a_p0 && p2 != a_p1) { return p2; }

        EMP_ASSERT_UNREACHABLE();
        return nullptr;
    }

    EMP_RETURN tricell_c* get_cell(cpc_v2f a_p) const
    {
        EMP_ASSERT(p0 != nullptr);
        EMP_ASSERT(p1 != nullptr);
        EMP_ASSERT(p2 != nullptr);

        if (a_p == p0) { return c0; }
        if (a_p == p1) { return c1; }
        if (a_p == p2) { return c2; }

        EMP_ASSERT_UNREACHABLE();
        return nullptr;
    }

    void reset(cpc_v2f ap0, cpc_v2f ap1, cpc_v2f ap2)
    {
        p0 = ap0;
        p1 = ap1;
        p2 = ap2;
        c0 = nullptr;
        c1 = nullptr;
        c2 = nullptr;

        need_flip = false;

        EMP_ASSERT(p0 != nullptr);
        EMP_ASSERT(p1 != nullptr);
        EMP_ASSERT(p2 != nullptr);
        EMP_ASSERT(is_ccw(*p0, *p1, *p2));
    }

    void reset(tricell_c const& a_rFrom)
    {
        p0 = a_rFrom.p0;
        p1 = a_rFrom.p1;
        p2 = a_rFrom.p2;
        c0 = a_rFrom.c0;
        c1 = a_rFrom.c1;
        c2 = a_rFrom.c2;

        need_flip = a_rFrom.need_flip;

        EMP_ASSERT(p0 != nullptr);
        EMP_ASSERT(p1 != nullptr);
        EMP_ASSERT(p2 != nullptr);
        EMP_ASSERT(is_ccw(*p0, *p1, *p2));
    }

    void reset(cpc_v2f a_p0, cpc_v2f a_p1, cpc_v2f a_p2, tricell_c* const a_c0, tricell_c* const a_c1, tricell_c* const a_c2)
    {
        p0 = a_p0;
        p1 = a_p1;
        p2 = a_p2;
        c0 = a_c0;
        c1 = a_c1;
        c2 = a_c2;

        need_flip = false;

        EMP_ASSERT(p0 != nullptr);
        EMP_ASSERT(p1 != nullptr);
        EMP_ASSERT(p2 != nullptr);
        EMP_ASSERT(is_ccw(*p0, *p1, *p2));
    }

    static void bind_cell(tricell_c* const a_pTo, cpc_v2f a_p0, cpc_v2f a_p1, tricell_c* const a_pFrom)
    {
        EMP_ASSERT(a_pTo != nullptr);
        EMP_ASSERT(a_p0 != nullptr);
        EMP_ASSERT(a_p1 != nullptr);

        if (a_pFrom)
        {
            cpc_v2f c00 = a_pFrom->p0;
            cpc_v2f c01 = a_pFrom->p1;
            cpc_v2f c02 = a_pFrom->p2;
            if (a_p0 == c00 && a_p1 == c02)
            { a_pFrom->c2 = a_pTo; }
            else if (a_p0 == c01 && a_p1 == c00)
            { a_pFrom->c0 = a_pTo; }
            else if (a_p0 == c02 && a_p1 == c01)
            { a_pFrom->c1 = a_pTo; }
            else
            { EMP_ASSERT_UNREACHABLE(); }
        }
    }

    void rebind(void)
    {
        bind_cell(this, p0, p1, c0);
        bind_cell(this, p1, p2, c1);
        bind_cell(this, p2, p0, c2);
    }

    static EMP_RETURN bool is_valid_cell(tricell_c const* const a_pTo, cpc_v2f a_p0, cpc_v2f a_p1, tricell_c const* const a_pFrom)
    {
        EMP_ASSERT(a_pTo != nullptr);
        EMP_ASSERT(a_p0 != nullptr);
        EMP_ASSERT(a_p1 != nullptr);

        if (a_pFrom)
        {
            if (a_pFrom->c0 == a_pTo)
            {
                if (a_p0 != a_pFrom->p1 || a_p1 != a_pFrom->p0)
                {
                    EMP_ASSERT(false);
                    return false;
                }
            }
            else if (a_pFrom->c1 == a_pTo)
            {
                if (a_p0 != a_pFrom->p2 || a_p1 != a_pFrom->p1)
                {
                    EMP_ASSERT(false);
                    return false;
                }
            }
            else if (a_pFrom->c2 == a_pTo)
            {
                if (a_p0 != a_pFrom->p0 || a_p1 != a_pFrom->p2)
                {
                    EMP_ASSERT(false);
                    return false;
                }
            }
            else
            {
                EMP_ASSERT(false);
                return false;
            }
        }

        return true;
    }

    EMP_RETURN bool is_valid(void) const
    {
        if (p0 == nullptr ||
            p1 == nullptr ||
            p2 == nullptr)
        {
            EMP_ASSERT(false);
            return false;
        }

        if (is_valid_cell(this, p0, p1, c0) == false)
        {
            EMP_ASSERT(false);
            return false;
        }

        if (is_valid_cell(this, p1, p2, c1) == false)
        {
            EMP_ASSERT(false);
            return false;
        }

        if (is_valid_cell(this, p2, p0, c2) == false)
        {
            EMP_ASSERT(false);
            return false;
        }

        if (is_ccw(*p0, *p1, *p2) == false)
        {
            EMP_ASSERT(false);
            return false;
        }

        return true;
    }
};
EMP_TYPEDEF(tricell_c)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, polycell_c)
public:
    vector<v2f const*> points;
    vector<polycell_c*> cells;
    bool need_flip;
    size_t index;

public:
    polycell_c(void):
        points(),
        cells(),
        need_flip(false),
        EMP_TT_MAX_CTOR(index)
    {}

public:
    void set(c_size a_stIndex, cpc_v2f a_pPoint, polycell_c* const a_pCell)
    {
        EMP_ASSERT(a_pPoint != nullptr);
        points.set(a_stIndex, a_pPoint);
        cells.set(a_stIndex, a_pCell);
    }

    EMP_RETURN c_v2f* get_point(cpc_v2f a_pP0, cpc_v2f a_pP1) const
    {
        EMP_ASSERT(a_pP0 != nullptr);
        EMP_ASSERT(a_pP1 != nullptr);

        c_size stCount = points.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            cpc_v2f pP = points.val(stIndex);
            if (pP != a_pP0 && pP != a_pP1)
            {
                return pP;
            }
        }

        EMP_ASSERT_UNREACHABLE();
        return nullptr;
    }

    EMP_RETURN polycell_c* get_cell(cpc_v2f a_pP) const
    {
        EMP_ASSERT(a_pP != nullptr);

        c_size stCount = points.count();
        EMP_ASSERT(stCount == cells.count());

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            cpc_v2f pP = points.val(stIndex);
            if (pP == a_pP)
            {
                return cells.val(stIndex);
            }
        }

        EMP_ASSERT_UNREACHABLE();
        return nullptr;
    }
};
EMP_TYPEDEF(polycell_c)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN polycell_c* add_empty_cell(vector<polycell_c*>& a_rCells)
{
    polycell_c* pCell = EMP_ALLOCATE(polycell_c);
    c_size stIndex = a_rCells.count();
    pCell->index = stIndex;
    a_rCells.add(pCell);
    return pCell;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN polycell_c* add_cell(vector<polycell_c*>& a_rCells)
{
    polycell_c* const pCell = add_empty_cell(a_rCells);
    pCell->points.resize(3);
    pCell->cells.resize(3);
    return pCell;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN polycell_c* try_add_cell(vector<polycell_c*>& a_rCells, polycell_c const* const a_pRoot)
{
    if (a_pRoot == nullptr)
    { return nullptr; }
    return add_cell(a_rCells);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_ASSERT
EMP_INTERN EMP_RETURN bool is_valid(polycell_c const* const a_pRoot, cpc_v2f a_pP0, cpc_v2f a_pP1, polycell_c const* const a_pChild)
{
    EMP_ASSERT(a_pRoot != nullptr);

    if (a_pChild == nullptr)
    { return true; }

    c_size stPointCount = a_pChild->points.count();
    c_size stCellCount = a_pChild->cells.count();
    if (stPointCount != stCellCount || stPointCount == 0)
    {
        EMP_ASSERT_UNREACHABLE();
        return false;
    }

    for (size_t stCell = 0; stCell < stCellCount; ++stCell)
    {
        cpc_v2f pP0 = a_pChild->points.val((stCell + 0) % stPointCount);
        cpc_v2f pP1 = a_pChild->points.val((stCell + 1) % stPointCount);

        if (a_pP0 == pP1 && a_pP1 == pP0 &&
            a_pChild->cells.val(stCell) == a_pRoot)
        { return true; }
    }

    EMP_ASSERT_UNREACHABLE();
    return false;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_ASSERT
EMP_INTERN EMP_RETURN bool is_valid(polycell_c const* const a_pRoot)
{
    if (a_pRoot == nullptr)
    {
        EMP_ASSERT_UNREACHABLE();
        return false;
    }

    c_size stPointCount = a_pRoot->points.count();
    c_size stCellCount = a_pRoot->cells.count();
    EMP_ASSERT(stPointCount == stCellCount);

    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    {
        c_size stP0 = (stPoint + 0) % stPointCount;
        c_size stP1 = (stPoint + 1) % stPointCount;
        cpc_v2f pP0 = a_pRoot->points.val(stP0);
        cpc_v2f pP1 = a_pRoot->points.val(stP1);

        polycell_c const* const pCell = a_pRoot->cells.val(stPoint);
        if (is_valid(a_pRoot, pP0, pP1, pCell) == false)
        {
            EMP_ASSERT_UNREACHABLE();
            return false;
        }
    }

    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    { EMP_ASSERT(a_pRoot->points.val(stPoint) != nullptr); }

    for (size_t stCell = 0; stCell < stCellCount; ++stCell)
    { EMP_ASSERT(a_pRoot->cells.val(stCell) != a_pRoot); }

    if (is_ccw(a_pRoot->points.first_const_ptr(), a_pRoot->points.count()) == false)
    {
        EMP_ASSERT_UNREACHABLE();
        return false;
    }

    return true;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_ASSERT
EMP_INTERN EMP_RETURN bool try_is_valid(polycell_c const* const a_pCell)
{
    if (a_pCell == nullptr)
    { return true; }
    return is_valid(a_pCell);
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void rebind(polycell_c* const a_pRoot, cpc_v2f a_pP0, cpc_v2f a_pP1, polycell_c* const a_pChild)
{
    EMP_ASSERT(a_pRoot != nullptr);
    EMP_ASSERT(a_pP0 != nullptr);
    EMP_ASSERT(a_pP1 != nullptr);
    EMP_ASSERT(a_pChild != nullptr);

    c_size stPointCount = a_pChild->points.count();

    EMP_XX_IF_ASSERT(c_size stCellCount = a_pChild->cells.count();)
    EMP_ASSERT(stPointCount == stCellCount);

    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    {
        c_size stP0 = (stPoint + 0) % stPointCount;
        c_size stP1 = (stPoint + 1) % stPointCount;
        cpc_v2f pP0 = a_pChild->points.val(stP0);
        cpc_v2f pP1 = a_pChild->points.val(stP1);
        EMP_ASSERT(pP0 != nullptr);
        EMP_ASSERT(pP1 != nullptr);

        if (a_pP0 == pP1 && a_pP1 == pP0)
        {
            a_pChild->cells.set(stPoint, a_pRoot);
            return;
        }
    }

    EMP_ASSERT_UNREACHABLE();
    return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void rebind(polycell_c* const a_pRoot)
{
    EMP_ASSERT(a_pRoot != nullptr);

    c_size stPointCount = a_pRoot->points.count();
    EMP_XX_IF_ASSERT(c_size stCellCount = a_pRoot->cells.count();)
    EMP_ASSERT(stPointCount == stCellCount);

    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    {
        polycell_c* const pCell = a_pRoot->cells.val(stPoint);
        if (pCell)
        {
            c_size stP0 = (stPoint + 0) % stPointCount;
            c_size stP1 = (stPoint + 1) % stPointCount;
            cpc_v2f pP0 = a_pRoot->points.val(stP0);
            cpc_v2f pP1 = a_pRoot->points.val(stP1);
            rebind(a_pRoot, pP0, pP1, pCell);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool swap(vector<polycell_c*>& a_rCells, c_size a_stCount, polycell_c* const a_pCell);
EMP_INTERN EMP_RETURN bool swap(vector<polycell_c*>& a_rCells, c_size a_stCount, polycell_c* const a_pCell)
{
    if (a_pCell == nullptr || a_pCell->need_flip)
    { return false; }

    a_pCell->need_flip = true;

    c_size stFrom = a_pCell->index;
    c_size stTo = a_stCount % a_rCells.count();
    if (stFrom != stTo)
    {
        a_rCells.swap(stFrom, stTo);

        polycell_c* pTo = a_rCells.val(stTo);
        EMP_ASSERT(pTo != nullptr);
        pTo->index = stTo;

        polycell_c* pFrom = a_rCells.val(stFrom);
        EMP_ASSERT(pFrom != nullptr);
        pFrom->index = stFrom;

        EMP_ASSERT(a_pCell == pTo);
        EMP_ASSERT(pTo != pFrom);

        rebind(pTo);
        rebind(pFrom);

        pTo->need_flip = true;
        EMP_ASSERT(pFrom->need_flip == false);

        EMP_ASSERT(is_valid(pTo));
        EMP_ASSERT(is_valid(pFrom));
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN size_t swap_children(vector<polycell_c*>& a_rCells, c_size a_stCount, polycell_c* const a_pRoot);
EMP_INTERN EMP_RETURN size_t swap_children(vector<polycell_c*>& a_rCells, c_size a_stCount, polycell_c* const a_pRoot)
{
    size_t stSwapCount = 0;

    EMP_ASSERT(a_pRoot != nullptr);

    c_size stChildCount = a_pRoot->cells.count();
    EMP_ASSERT(stChildCount > 0);

    for (size_t stChild = 0; stChild < stChildCount; ++stChild)
    {
        polycell_c* const pCell = a_pRoot->cells.val(stChild);
        if (swap(a_rCells, a_stCount + stSwapCount, pCell))
        {
            ++stSwapCount;
        }
    }

    return stSwapCount;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool is_in_circumcenter(c_v2f& a_rCenter, polycell_c const* const a_pCircle,
    polycell_c const* const a_pCell)
{
    EMP_ASSERT(a_pCircle != nullptr);

    if (a_pCell == nullptr)
    { return false; }

    c_size stCircleCount = a_pCircle->points.count();
    c_size stCellCount = a_pCell->points.count();

    EMP_ASSERT(stCircleCount == 3);
    EMP_ASSERT(stCellCount == 3);

    cpc_v2f p0 = a_pCircle->points.val(0);
    c_float r2 = sq_distance(*p0, a_rCenter);

    for (size_t stCell = 0; stCell < stCellCount; ++stCell)
    {
        cpc_v2f pPCell = a_pCell->points.val(stCell);
        bool bFromCircle = false;
        for (size_t stCircle = 0; stCircle < stCircleCount; ++stCircle)
        {
            cpc_v2f pPCircle = a_pCircle->points.val(stCircle);
            if (pPCell == pPCircle)
            {
                bFromCircle = true;
                break;
            }
        }

        if (bFromCircle == false)
        {
            float  fEpsilon = emp::mat::epsilon<float>();
            c_float d2 = sq_distance(*pPCell, a_rCenter);

            float fR2D2 = emp::ast::cast<float>(emp::ast::cast<float>(r2) - emp::ast::cast<float>(d2));
            if (fR2D2 > emp::ast::cast<float>(fEpsilon))
            { return true; }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void get_indices(polycell_c* const a_pCell, cpc_v2f a_pP0, cpc_v2f a_pP1, size_t& a_rI0, size_t& a_rI1)
{
    EMP_ASSERT(a_pCell != nullptr);
    EMP_ASSERT(a_pP0 != nullptr);
    EMP_ASSERT(a_pP1 != nullptr);

    bool bP0Found = false;
    bool bP1Found = false;

    c_size stCount = a_pCell->points.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_v2f pP = a_pCell->points.val(stIndex);
        if (pP == a_pP0)
        {
            a_rI0 = stIndex;
            bP0Found = true;
            if (bP1Found)
            {
                EMP_ASSERT(a_rI0 != a_rI1);
                return;
            }
        }
        if (pP == a_pP1)
        {
            a_rI1 = stIndex;
            bP1Found = true;
            if (bP0Found)
            {
                EMP_ASSERT(a_rI0 != a_rI1);
                return;
            }
        }
    }

    EMP_ASSERT_UNREACHABLE();
    return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool get_previous(polycell_c* const a_pCell, c_size a_stEnd, size_t& a_rIndex)
{
    c_size stCount = a_pCell->points.count();
    EMP_ASSERT(stCount > 0);

    if (a_rIndex == a_stEnd)
    { return false; }

    a_rIndex = (a_rIndex == 0 ? stCount : a_rIndex) - 1;

    if (a_rIndex == a_stEnd)
    { return false; }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool get_next(polycell_c* const a_pCell, c_size a_stEnd, size_t& a_rIndex)
{
    c_size stCount = a_pCell->points.count();
    EMP_ASSERT(stCount > 0);

    if (a_rIndex == a_stEnd)
    { return false; }

    ++a_rIndex;
    a_rIndex %= stCount;

    if (a_rIndex == a_stEnd)
    { return false; }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void join(emp::cnt::vector<polycell_c*>& a_rCells, polycell_c* const a_pLeft, cpc_v2f a_pPL0, cpc_v2f a_pPL1)
{
    EMP_ASSERT(a_pPL0 != nullptr);
    EMP_ASSERT(a_pPL1 != nullptr);
    EMP_ASSERT(a_pPL0 != a_pPL1);
    EMP_ASSERT(a_pLeft != nullptr);

    size_t stL0 = 0;
    size_t stL1 = 0;
    get_indices(a_pLeft, a_pPL0, a_pPL1, stL0, stL1);

    polycell_c* const pRight = a_pLeft->cells.val(stL0);

    cpc_v2f pPR0 = a_pPL1;
    cpc_v2f pPR1 = a_pPL0;
    size_t stR0 = 0;
    size_t stR1 = 0;
    get_indices(pRight, pPR0, pPR1, stR0, stR1);

    size_t stRight = stR0;
    size_t stLeft = stL0;

    //EMP_LOG_DEBUG("join : #0 -> #1", a_pLeft->index, pRight->index);

    EMP_ASSERT(pRight->cells.val(stR1) != a_pLeft);

    a_pLeft->set(stL0, pRight->points.val(stR1), pRight->cells.val(stR1));
    while (true)
    {
        c_bool bRight = get_previous(pRight, stR1, stRight);
        c_bool bLeft = get_next(a_pLeft, stL1, stLeft);

        /*EMP_LOG_DEBUG("before");
        for (size_t stIndex = 0; stIndex < a_pLeft->points.count(); ++stIndex)
        {
            cpc_point_s p = a_pLeft->points.val(stIndex);
            EMP_LOG_DEBUG("#0 : #1, #2", stIndex, p->x, p->y);
        }*/

        if (bRight && bLeft)
        {
            EMP_LOG_DEBUG("set : #0", stLeft);

            polycell_c* const pCurrent = pRight->cells.val(stRight);
            EMP_ASSERT(pCurrent != a_pLeft);

            a_pLeft->points.set(stLeft, pRight->points.val(stRight));
            a_pLeft->cells.set(stLeft, pCurrent);
        }
        else if (bRight)
        {
            //EMP_LOG_DEBUG("insert : #0", stL1);
            EMP_ASSERT(stL1 == stLeft);
            polycell_c* const pCurrent = pRight->cells.val(stRight);
            EMP_ASSERT(pCurrent != a_pLeft);

            a_pLeft->points.insert(stL1, pRight->points.val(stRight));
            a_pLeft->cells.insert(stL1, pCurrent);
        }
        else if (bLeft)
        {
            EMP_LOG_DEBUG("remove : #0", stLeft);
            a_pLeft->points.remove(stLeft);
            a_pLeft->cells.remove(stLeft);
        }
        else
        {
            break;
        }

        /*EMP_LOG_DEBUG("after");
        for (size_t stIndex = 0; stIndex < a_pLeft->points.count(); ++stIndex)
        {
            cpc_point_s p = a_pLeft->points.val(stIndex);
            EMP_LOG_DEBUG("#0 : #1, #2", stIndex, p->x, p->y);
        }*/
    }

    rebind(a_pLeft);
    EMP_ASSERT(is_valid(a_pLeft));

    c_size stLast = a_rCells.count() - 1;
    polycell_c* const pLast = a_rCells.val(stLast);
    pLast->index = pRight->index;

    a_rCells.swap(pRight->index, stLast);
    a_rCells.resize(stLast);

    EMP_DEALLOCATE(pRight);

    for (size_t stIndex = 0; stIndex < a_pLeft->cells.count(); ++stIndex)
    {
        polycell_c const* const pCell = a_pLeft->cells.val(stIndex);
        if (pCell)
        {
            //UI EMP_LOG_DEBUG("child : #0", pCell->index);
        }
    }

#ifdef EMP_XX_ASSERT
    for (size_t stIndex = 0; stIndex < a_pLeft->points.count(); ++stIndex)
    {
        c_size stP0 = (stIndex + 0) % a_pLeft->points.count();
        c_size stP1 = (stIndex + 1) % a_pLeft->points.count();
        cpc_v2f p0 = a_pLeft->points.val(stP0);
        cpc_v2f p1 = a_pLeft->points.val(stP1);
        if (p0 == a_pPL0)
        {
            EMP_ASSERT(p1 != a_pPL1);
        }
        if (p0 == a_pPL1)
        {
            EMP_ASSERT(p1 != a_pPL0);
        }
    }
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void split(polycell_c* const a_pLeft, polycell_c* const a_pRight, cpc_v2f a_pP0, cpc_v2f a_pP1)
{
    EMP_ASSERT(a_pP0 != nullptr);
    EMP_ASSERT(a_pP1 != nullptr);
    EMP_ASSERT(a_pLeft != nullptr);
    EMP_ASSERT(a_pRight != nullptr);

    EMP_ASSERT(a_pLeft->points.count() > 3);
    EMP_ASSERT(a_pLeft->cells.count() > 3);

    EMP_ASSERT(a_pRight->points.count() == 0);
    EMP_ASSERT(a_pRight->cells.count() == 0);

    //    EMP_LOG_DEBUG(  "before split : left #0, #1 | right #2, #3",
    //      a_pLeft->index, a_pLeft->points.count(),
    //      a_pRight->index, a_pRight->points.count());

    size_t stPL0 = 0;
    size_t stPL1 = 0;
    get_indices(a_pLeft, a_pP0, a_pP1, stPL0, stPL1);

    EMP_ASSERT(((stPL0 + 1) % a_pLeft->points.count()) != stPL1);
    EMP_ASSERT(((stPL1 + 1) % a_pLeft->points.count()) != stPL0);

    a_pRight->points.add(a_pLeft->points.val(stPL0));
    a_pRight->cells.add(a_pLeft->cells.val(stPL0));

    size_t stLeft = stPL0;
    while (true)
    {
        if (get_next(a_pLeft, stPL1, stLeft))
        {
            //EMP_LOG_DEBUG("split add : #0", stLeft);
            a_pRight->points.add(a_pLeft->points.val(stLeft));
            a_pRight->cells.add(a_pLeft->cells.val(stLeft));
        }
        else
        {
            //EMP_LOG_DEBUG("split break");
            break;
        }
    }

    a_pRight->points.add(a_pLeft->points.val(stPL1));
    a_pRight->cells.add(a_pLeft);

    EMP_ASSERT(a_pRight->points.count() > 0);
    EMP_ASSERT(a_pRight->cells.count() > 0);
    EMP_ASSERT(a_pRight->points.count() == a_pRight->cells.count());

    c_size stPointCount = a_pLeft->points.count();

    EMP_XX_IF_ASSERT(c_size stCellCount = a_pLeft->cells.count();)
    EMP_ASSERT(stPointCount == stCellCount);

    a_pLeft->cells.set(stPL0, a_pRight);

    if (stPL0 < stPL1)
    {
        c_size stBegin = stPL0 + 1;
        c_size stEnd = stPL1 - 1;
        EMP_ASSERT(stBegin <= stEnd);

        //EMP_LOG_DEBUG("R : #0", a_pLeft->points.count());
        a_pLeft->points.remove(stBegin, stEnd);
        a_pLeft->cells.remove(stBegin, stEnd);
        //EMP_LOG_DEBUG("0 : #0,#1", a_pLeft->points.ref(0)->x, a_pLeft->points.ref(0)->y);
        //EMP_LOG_DEBUG("1 : #0,#1", a_pLeft->points.ref(1)->x, a_pLeft->points.ref(1)->y);
        //EMP_LOG_DEBUG("2 : #0,#1", a_pLeft->points.ref(2)->x, a_pLeft->points.ref(2)->y);
    }
    else
    {
        if (stPL0 < stPointCount - 1)
        {
            c_size stBegin = stPL0 + 1;
            //EMP_LOG_DEBUG("RE : #0", a_pLeft->points.count());
            a_pLeft->points.remove(stBegin, stPointCount - 1);
            a_pLeft->cells.remove(stBegin, stPointCount - 1);
        }

        if (stPL1)
        {
            size_t stEnd = stPL1 - 1;
            //EMP_LOG_DEBUG("REM : #0", a_pLeft->points.count());
            a_pLeft->points.remove(0, stEnd);
            a_pLeft->cells.remove(0, stEnd);
        }
    }

    rebind(a_pLeft);
    // EMP_LOG_DEBUG("B0 : #0,#1", a_pLeft->points.ref(0)->x, a_pLeft->points.ref(0)->y);
    // EMP_LOG_DEBUG("B1 : #0,#1", a_pLeft->points.ref(1)->x, a_pLeft->points.ref(1)->y);
    // EMP_LOG_DEBUG("/2 : #0,#1", a_pLeft->points.ref(2)->x, a_pLeft->points.ref(2)->y);
    rebind(a_pRight);

    //EMP_LOG_DEBUG("after split : left #0, #1 | right #2, #3",
    //    a_pLeft->index, a_pLeft->points.count(), a_pRight->index, a_pRight->points.count());

    EMP_ASSERT(is_valid(a_pLeft));
    EMP_ASSERT(is_valid(a_pRight));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN float triangle_sign(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2);
EMP_INTERN EMP_RETURN float triangle_sign(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2)
{
    c_float x0 = p0.x;
    c_float y0 = p0.y;
    c_float x1 = p1.x;
    c_float y1 = p1.y;
    c_float x2 = p2.x;
    c_float y2 = p2.y;

    return (x0-x2) * (y1-y2) - (x1-x2) * (y0-y2);
    //return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool point_in_triangle(v2f::tparam p, v2f::tparam p0, v2f::tparam p1, v2f::tparam p2);
EMP_RETURN bool point_in_triangle(v2f::tparam p, v2f::tparam p0, v2f::tparam p1, v2f::tparam p2)
{
    c_bool b0 = triangle_sign(p, p0, p1) < 0.0f;
    c_bool b1 = triangle_sign(p, p1, p2) < 0.0f;
    c_bool b2 = triangle_sign(p, p2, p0) < 0.0f;

    return ((b0 == b1) && (b1 == b2));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void earcut(emp::cnt::vector<polycell_c*>& a_rCells, polycell_c* const a_pCell)
{
    EMP_ASSERT(a_pCell != nullptr);
    EMP_ASSERT(a_pCell->points.count() > 3);
    EMP_ASSERT(a_pCell->cells.count() > 3);
    EMP_ASSERT(a_pCell->points.count() == a_pCell->cells.count());

    //EMP_LOG_DEBUG("earcut");
    polycell_c* pCurrent = a_pCell;
    while (pCurrent->points.count() > 3)
    {
        for (size_t stConvex = 0; stConvex < pCurrent->points.count(); ++stConvex)
        {
            c_size stCount = pCurrent->points.count();

            c_size stP0 = (stConvex + 0) % stCount;
            c_size stP1 = (stConvex + 1) % stCount;
            c_size stP2 = (stConvex + 2) % stCount;

            cpc_v2f p0 = pCurrent->points.val(stP0);
            cpc_v2f p1 = pCurrent->points.val(stP1);
            cpc_v2f p2 = pCurrent->points.val(stP2);

            if (is_ccw(*p0, *p1, *p2))
            {
                bool bValidEar = true;
                for (size_t stConcave = 0; stConcave < stCount; ++stConcave)
                {
                    c_size stC0 = (stConcave + 0) % stCount;
                    c_size stC1 = (stConcave + 1) % stCount;
                    c_size stC2 = (stConcave + 2) % stCount;

                    if (stC1 == stP0 || stC1 == stP1 || stC1 == stP2)
                    {
                        continue;
                    }

                    cpc_v2f c0 = pCurrent->points.val(stC0);
                    cpc_v2f c1 = pCurrent->points.val(stC1);
                    cpc_v2f c2 = pCurrent->points.val(stC2);

                    if (is_ccw(*c0, *c1, *c2) == false &&
                        point_in_triangle(*c1, *p0, *p1, *p2))
                    {
                        bValidEar = false;
                    }
                }

                if (bValidEar)
                {
                    polycell_c* const pRight = add_empty_cell(a_rCells);
                    split(pCurrent, pRight, p2, p0);
                    if (pCurrent->points.count() > 3)
                    {
                        EMP_ASSERT(pRight->points.count() == 3);
                        // @@0  : llvw warning ?
                        //pCurrent = pCurrent;
                    }
                    else if (pRight->points.count() > 3)
                    {
                        EMP_ASSERT(pCurrent->points.count() == 3);
                        pCurrent = pRight;
                    }

                    if (pCurrent->points.count() == 3)
                    {
                        break;
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void flip(polycell_c* const a_pLeft, polycell_c* const a_pRight);
EMP_INTERN void flip(polycell_c* const a_pLeft, polycell_c* const a_pRight)
{
    EMP_ASSERT(a_pLeft != nullptr);
    EMP_ASSERT(a_pRight != nullptr);

    c_size stLeftCount = a_pLeft->cells.count();
    c_size stRightCount = a_pRight->cells.count();

    c_size st3 = 3;

    EMP_ASSERT(stLeftCount == st3);
    EMP_ASSERT(stRightCount == st3);
    EMP_ASSERT(a_pLeft->points.count() == stLeftCount);
    EMP_ASSERT(a_pRight->points.count() == stRightCount);

    for (size_t stLeft = 0; stLeft < stLeftCount; ++stLeft)
    {
        polycell_c const* const pLeftCell = a_pLeft->cells.val(stLeft);
        if (pLeftCell == a_pRight)
        {
            for (size_t stRight = 0; stRight < stRightCount; ++stRight)
            {
                polycell_c const* const pRightCell = a_pRight->cells.val(stRight);
                if (pRightCell == a_pLeft)
                {
                    c_size stL0 = (stLeft + 0) % st3;
                    c_size stL1 = (stLeft + 1) % st3;
                    c_size stL2 = (stLeft + 2) % st3;
                    EMP_XX_IF_ASSERT(cpc_v2f PL0 = a_pLeft->points.val(stL0);)
                    cpc_v2f PL1 = a_pLeft->points.val(stL1);
                    cpc_v2f PL2 = a_pLeft->points.val(stL2);
                    polycell_c* const CL0 = a_pLeft->cells.val(stL0);
                    polycell_c* const CL1 = a_pLeft->cells.val(stL1);
                    polycell_c* const CL2 = a_pLeft->cells.val(stL2);

                    c_size stR0 = (stRight + 0) % st3;
                    c_size stR1 = (stRight + 1) % st3;
                    c_size stR2 = (stRight + 2) % st3;
                    EMP_XX_IF_ASSERT(cpc_v2f PR0 = a_pRight->points.val(stR0);)
                    cpc_v2f PR1 = a_pRight->points.val(stR1);
                    cpc_v2f PR2 = a_pRight->points.val(stR2);
                    polycell_c* const CR0 = a_pRight->cells.val(stR0);
                    polycell_c* const CR1 = a_pRight->cells.val(stR1);
                    polycell_c* const CR2 = a_pRight->cells.val(stR2);

                    EMP_ASSERT(PL0 && PL1 && PL2 && PR0 && PR1 && PR2);
                    EMP_ASSERT(PL0 == PR1);
                    EMP_ASSERT(PL1 == PR0);
                    EMP_ASSERT(PL2 != PR2);

                    EMP_ASSERT(CL0 == a_pRight);
                    EMP_ASSERT(CR0 == a_pLeft);

                    EMP_ASSERT(CL0 != CR0 && CL0 != CR1 && CL0 != CR2);
                    EMP_ASSERT(CL1 != CR0 && CL1 != CR1 && CL1 != CR2);
                    EMP_ASSERT(CL2 != CR0 && CL2 != CR1 && CL2 != CR2);

                    c_bool bLeftFlip = a_pLeft->need_flip;
                    c_bool bRightFlip = a_pRight->need_flip;

                    EMP_ASSERT(is_ccw(*PL0, *PL1, *PL2));
                    EMP_ASSERT(is_ccw(*PR0, *PR1, *PR2));
                    EMP_ASSERT(is_ccw(*PR2, *PL1, *PL2));
                    EMP_ASSERT(is_ccw(*PL2, *PR1, *PR2));

                    a_pLeft->set(0, PR2, CR2);
                    a_pLeft->set(1, PL1, CL1);
                    a_pLeft->set(2, PL2, CL0);

                    a_pRight->set(0, PL2, CL2);
                    a_pRight->set(1, PR1, CR1);
                    a_pRight->set(2, PR2, CR0);

                    rebind(a_pLeft);
                    rebind(a_pRight);

                    a_pLeft->need_flip = bLeftFlip;
                    a_pRight->need_flip = bRightFlip;

                    EMP_ASSERT(is_valid(a_pLeft));
                    EMP_ASSERT(is_valid(a_pRight));
                    return;
                }
            }
        }
    }

    EMP_ASSERT_UNREACHABLE();
    return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool segment_intersect_cell(cpc_v2f a_pP0, cpc_v2f a_pP1, polycell_c const* const a_pCell,
    c_v2f*& a_rpP0, c_v2f*& a_rpP1)
{
    EMP_ASSERT(a_pP0 != nullptr);
    EMP_ASSERT(a_pP1 != nullptr);
    EMP_ASSERT(a_pCell != nullptr);
    EMP_ASSERT(a_rpP0 == nullptr);
    EMP_ASSERT(a_rpP1 == nullptr);

    /*c_size stPointCount = a_pCell->points.count();
    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    {
        c_size stP0 = (stPoint + 0) % stPointCount;
        c_size stP1 = (stPoint + 1) % stPointCount;
        cpc_point_s pP0 = a_pCell->points.val(stP0);
        cpc_point_s pP1 = a_pCell->points.val(stP1);

        if (lines_are_collinear(a_pP0, a_pP1, pP0, pP1))
        {
            EMP_LOG_DEBUG("COLLINEAR");
            return false;
        }
    }*/

    c_size stPointCount = a_pCell->points.count();
    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    {
        c_size stP0 = (stPoint + 0) % stPointCount;
        c_size stP1 = (stPoint + 1) % stPointCount;
        cpc_v2f pP0 = a_pCell->points.val(stP0);
        cpc_v2f pP1 = a_pCell->points.val(stP1);

        if (a_pP0 != pP0 &&
            a_pP0 != pP1 &&
            a_pP1 != pP0 &&
            a_pP1 != pP1)
        {
            if (segment_intersect_segment(*a_pP0, *a_pP1, *pP0, *pP1))
            {
                EMP_ASSERT(a_pP1 != pP1);
                //EMP_LOG_DEBUG("intersect");
                //EMP_LOG_DEBUG("p0: #0, #1 | p1 #2, #3", a_pP0->x, a_pP0->y, a_pP1->x, a_pP1->y);
                //EMP_LOG_DEBUG("p0: #0, #1 | p1 #2, #3", pP0->x, pP0->y, pP1->x, pP1->y);
                a_rpP0 = pP0;
                a_rpP1 = pP1;
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool point_in_cell(cpc_point_s p,
                                        polycell_c const* const c)
{
    EMP_ASSERT(p);
    EMP_ASSERT(c);

    for (size_t stIndex = 0; stIndex < c->points.count(); ++stIndex)
    {
        if (p == c->points.val(stIndex))
        {
            return true;
        }
    }
    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool point_in_cells(
                                emp::cnt::vector<polycell_c*> const& a_rCells,
                                cpc_point_s p,
                                c_size EMP_SILENT(a_stPoint))
{
    bool bFound = false;
    for (size_t stCell = 0; stCell < a_rCells.count(); ++stCell)
    {
        polycell_c const* const pCell = a_rCells.val(stCell);
        if (point_in_cell(p, pCell))
        {
            //UI EMP_LOG_DEBUG("P#0 in cell : #1", a_stPoint, stCell);
            bFound = true;
        }
    }
    return bFound;
 }*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN void dump_point(cpc_point_s p, c_size i)
{
    EMP_ASSERT(p);
    EMP_LOG_DEBUG("#0 : #1, #2", i, p->x, p->y);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void dump_points(vector<c_point_s*> const& a_vpP)
{
    c_size stCount = a_vpP.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        dump_point(a_vpP.val(stIndex), stIndex);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void dump_points(
                emp::cnt::shell<cpc_point_s> const& a_vpP)
{
    c_size stCount = a_vpP.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        dump_point(a_vpP.val(stIndex), stIndex);
    }
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void constraint(emp::cnt::shell<cpc_v2f> const& a_rspPoints, emp::cnt::vector<polycell_c*>& a_rCells);
void constraint(emp::cnt::shell<cpc_v2f> const& a_rspPoints, emp::cnt::vector<polycell_c*>& a_rCells)
{
    EMP_ASSERT(a_rCells.count() > 0);

    c_size stPointCount = a_rspPoints.count();
    EMP_ASSERT(stPointCount > 2);

    //EMP_LOG_DEBUG("CONSTRAINTS");
    //dump_points(a_rspPoints);

    for (size_t stIndex = 0; stIndex < stPointCount; ++stIndex)
    {
        //EMP_LOG_DEBUG("SEGMENT : #0", stSegment);

        c_size stP0 = (stIndex + 0) % stPointCount;
        c_size stP1 = (stIndex + 1) % stPointCount;

        cpc_v2f pP0 = a_rspPoints.val(stP0);
        cpc_v2f pP1 = a_rspPoints.val(stP1);

        for (size_t stCell = 0; stCell < a_rCells.count(); ++stCell)
        {
            //EMP_LOG_DEBUG("CELL : #0", stCell);
            polycell_c* const pCell = a_rCells.val(stCell);

            //bool b0 = false;
            //bool b1 = false;
            //bool bIntersect = false;
            while (true)
            {
                //if (point_in_cells(a_rCells, pP0, 0)) {}
                //if (point_in_cells(a_rCells, pP1, 1)) {}

                //c_bool bBefore0 = point_in_cell(pP0, pCell);
                //c_bool bBefore1 = point_in_cell(pP1, pCell);

                //if (point_in_cell(pP0, pCell)) { b0 = true; }
                //if (point_in_cell(pP1, pCell)) { b1 = true; }

                c_v2f* pPL0 = nullptr;
                c_v2f* pPL1 = nullptr;
                if (segment_intersect_cell(pP0, pP1, pCell, pPL0, pPL1))
                {
                    /*EMP_LOG_DEBUG("cell");
                    dump_points(pCell->points);
                    EMP_LOG_DEBUG("constraint");
                    dump_point(pP0, 0);
                    dump_point(pP1, 1);
                    EMP_LOG_DEBUG("axe");
                    dump_point(pPL0, 0);
                    dump_point(pPL1, 1);*/

                    //bIntersect = true;
                    join(a_rCells, pCell, pPL0, pPL1);
                    //EMP_ASSERT(bBefore0 == false || point_in_cell(pP0, pCell));
                    //EMP_ASSERT(bBefore1 == false || point_in_cell(pP1, pCell));
                }
                else
                {
                    break;
                }
            }

            /*if (bIntersect)
            {
                EMP_ASSERT(b0);
                EMP_ASSERT(b1);
                EMP_ASSERT(pCell->points.count() > 3);
                EMP_ASSERT(point_in_cell(pP0, pCell));
                EMP_ASSERT(point_in_cell(pP1, pCell));
            }*/

            if (pCell->points.count() > 3)
            {
                polycell_c* const pRight = add_empty_cell(a_rCells);
                split(pCell, pRight, pP0, pP1);
                if (pRight->points.count() > 3)
                { earcut(a_rCells, pRight); }
                if (pCell->points.count() > 3)
                { earcut(a_rCells, pCell); }
            }
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool delaunay(emp::cnt::shell<cpc_v2f> const& a_rspPoints, emp::cnt::vector<polycell_c*>& a_rCells);
EMP_RETURN bool delaunay(emp::cnt::shell<cpc_v2f> const& a_rspPoints, emp::cnt::vector<polycell_c*>& a_rCells)
{
    EMP_ASSERT(a_rspPoints.count() > 0);
    EMP_ASSERT(a_rCells.count() > 0);

    c_size stPointCount = a_rspPoints.count();
    for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
    {
        cpc_v2f np = a_rspPoints.val(stPoint);

        size_t stFlipCount = 0;
        bool bFound = false;
        for (size_t stCell = 0; stCell < a_rCells.count(); ++stCell)
        {
            polycell_c const* const oc = a_rCells.val(stCell);
            EMP_ASSERT(is_valid(oc));
            EMP_ASSERT(oc->cells.count() == 3);
            EMP_ASSERT(oc->points.count() == 3);

            EMP_ASSERT(try_is_valid(oc->cells.val(0)));
            EMP_ASSERT(try_is_valid(oc->cells.val(1)));
            EMP_ASSERT(try_is_valid(oc->cells.val(2)));

            cpc_v2f p0 = oc->points.val(0);
            cpc_v2f p1 = oc->points.val(1);
            cpc_v2f p2 = oc->points.val(2);

            c_bool bInTriangle = point_in_triangle(*np, *p0, *p1, *p2);
            if (bInTriangle == false)
            { continue; }

            c_float fEpsilon = emp::mat::epsilon<float>();
            c_float fSqEpsilon = fEpsilon * fEpsilon;

            if (sq_distance(*np, *p0) < fSqEpsilon ||
                sq_distance(*np, *p1) < fSqEpsilon ||
                sq_distance(*np, *p2) < fSqEpsilon)
            {
                EMP_LOG_ERROR("Duplicate point");
                return false;
            }

            size_t stEdgeOffset = 0;

            c_float fSq01 = point_line_sq_distance(*np, *p0, *p1);
            c_float fSq12 = point_line_sq_distance(*np, *p1, *p2);
            c_float fSq20 = point_line_sq_distance(*np, *p2, *p0);

            //EMP_LOG_DEBUG("01 = #0", fSq01);
            //EMP_LOG_DEBUG("12 = #0", fSq12);
            //EMP_LOG_DEBUG("20 = #0", fSq20);

            if (fSq01 < fSqEpsilon)
            {
                /*EMP_LOG_DEBUG("on 01");
                dump_point(p0, 0);
                dump_point(p1, 1);
                dump_point(p2, 2);
                dump_point(np, 3);*/

                stEdgeOffset = 1;
            }
            else if (fSq12 < fSqEpsilon)
            {
                //EMP_LOG_DEBUG("on 12");
                stEdgeOffset = 2;
            }
            else if (fSq20 < fSqEpsilon)
            {
                //EMP_LOG_DEBUG("on 20");
                stEdgeOffset = 3;
            }

            polycell_c* const nc0 = a_rCells.val(stCell);
            EMP_ASSERT(nc0 != nullptr);

            polycell_c* const nc1 = add_cell(a_rCells);
            EMP_ASSERT(nc1 != nullptr);

            // @@2 static set
            array<polycell_c*, 4, true> aCells;
            aCells.add(nc0);
            aCells.add(nc1);

            if (stEdgeOffset)
            {
                c_size stEdgeIndex = stEdgeOffset - 1;

                polycell_c* const nc3 = oc->cells.val(stEdgeIndex);
                polycell_c* const nc2 = try_add_cell(a_rCells, nc3);
                EMP_ASSERT((((nc2 && nc3) ? 1 : 0) ^ ((nc2 == nullptr && nc3 == nullptr) ? 1 : 0)) != 0);

                c_size stOff0 = (stEdgeOffset + 0) % 3;
                c_size stOff1 = (stEdgeOffset + 1) % 3;
                c_size stOff2 = (stEdgeOffset + 2) % 3;

                cpc_v2f dop0 = oc->points.val(stOff0);
                cpc_v2f dop1 = oc->points.val(stOff1);
                cpc_v2f dop2 = oc->points.val(stOff2);

                polycell_c* const doc0 = oc->cells.val(stOff0);
                polycell_c* const doc1 = oc->cells.val(stOff1);

                nc0->set(0, dop0, doc0);
                nc0->set(1, dop1, nc1);
                nc0->set(2, np  , nc3);

                nc1->set(0, dop1, doc1);
                nc1->set(1, dop2, nc2);
                nc1->set(2, np  , nc0);

                if (nc2)
                {
                    cpc_v2f dop3 = nc3->get_point(dop2, dop0);

                    nc2->set(0, dop2, nc3->get_cell(dop2));
                    nc2->set(1, dop3, nc3);
                    nc2->set(2, np  , nc1);

                    nc3->set(0, dop3, nc3->get_cell(dop3));
                    nc3->set(1, dop0, nc0);
                    nc3->set(2, np  , nc2);

                    aCells.add(nc2);
                    aCells.add(nc3);
                }
            }
            else // 012
            {
                polycell_c* const oc0 = oc->cells.val(0);
                polycell_c* const oc1 = oc->cells.val(1);
                polycell_c* const oc2 = oc->cells.val(2);

                polycell_c* const nc2 = add_cell(a_rCells);

                EMP_ASSERT(is_ccw(*p0, *p1, *np));
                EMP_ASSERT(is_ccw(*p1, *p2, *np));
                EMP_ASSERT(is_ccw(*p2, *p0, *np));

                nc0->set(0, p0, oc0);
                nc0->set(1, p1, nc1);
                nc0->set(2, np, nc2);

                nc1->set(0, p1, oc1);
                nc1->set(1, p2, nc2);
                nc1->set(2, np, nc0);

                nc2->set(0, p2, oc2);
                nc2->set(1, p0, nc0);
                nc2->set(2, np, nc1);

                aCells.add(nc2);
            }

            c_size stCount = aCells.count();
            EMP_ASSERT(stCount >= 2);

            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { rebind(aCells.val(stIndex)); }

            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { EMP_ASSERT(is_valid(aCells.val(stIndex))); }

            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { stFlipCount += swap_children(a_rCells, stFlipCount, aCells.val(stIndex)); }

            bFound = true;
            break;
        }

        if (bFound == false)
        {
            EMP_LOG_ERROR("Point do not lie in any cell");
            return false;
        }

        for (size_t stRoot = 0; stRoot < stFlipCount; ++stRoot)
        {
            //EMP_LOG_DEBUG("flip : #0", stRoot);
            c_size stRootCellIndex = stRoot % a_rCells.count();
            polycell_c* pRoot = a_rCells.val(stRootCellIndex);

            EMP_ASSERT(pRoot != nullptr);
            EMP_ASSERT(pRoot->need_flip);

            pRoot->need_flip = false;

            v2f oCenter;
            circumcenter(*pRoot->points.val(0), *pRoot->points.val(1), *pRoot->points.val(2), oCenter);

            c_size stChildCount = pRoot->cells.count();
            for (size_t stChild = 0; stChild < stChildCount; ++stChild)
            {
                polycell_c* const pChild = pRoot->cells.val(stChild);
                if (is_in_circumcenter(oCenter, pRoot, pChild))
                {
                    flip(pRoot, pChild);
                    stFlipCount +=
                        swap_children(a_rCells, stFlipCount, pChild);
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f* add_unique_point(vector<v2f*>& a_rvpMesh, v2f::tparam a_pPoint);
EMP_RETURN v2f* add_unique_point(vector<v2f*>& a_rvpMesh, v2f::tparam a_pPoint)
{
    EMP_ASSERT(abs(-1.0f) > 0);

    c_float x = a_pPoint.x;
    c_float y = a_pPoint.y;

    c_float fEpsilon = epsilon<float>();
    c_size stCount = a_rvpMesh.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        v2f* pPoint = a_rvpMesh.val(stIndex);
        c_float dx = abs(pPoint->x - x);
        c_float dy = abs(pPoint->y - y);

        if (dx < fEpsilon && dy < fEpsilon)
        { return pPoint; }
    }

    v2f* pPoint = EMP_ALLOCATE(v2f); // @@1 batch
    *pPoint = { x, y }; // @@1 perf
    a_rvpMesh.add(pPoint);
    return pPoint;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void add_not_duplicate(vector<v2f*>& a_rvpPoints, v2f* p); // @@perf map ?
EMP_INTERN void add_not_duplicate(vector<v2f*>& a_rvpPoints, v2f* p)
{
    EMP_ASSERT(p != nullptr);

    c_size stCount = a_rvpPoints.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (a_rvpPoints.val(stIndex) == p)
        { return; }
    }

    a_rvpPoints.add(p);
    return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool extract(c_vector<outline_s*>& a_rvsPoints, vector<v2f*>& a_rvrMesh, vector<v2f*>& a_rvpPoints,
    vector<vector<v2f*>*>& a_rvrvpOutlines);
EMP_RETURN bool extract(c_vector<outline_s*>& a_rvsPoints, vector<v2f*>& a_rvrMesh, vector<v2f*>& a_rvpPoints,
    vector<vector<v2f*>*>& a_rvrvpOutlines)
{
    size_t stSumPointCount = 0;

    c_size stOutlineCount = a_rvsPoints.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    { stSumPointCount += a_rvsPoints.ref(stOutline)->points.count(); }

    EMP_RET_FALSE_IF_NOT(stSumPointCount > 0);

    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        cpc_outline_s pOutline = a_rvsPoints.ref(stOutline);
        vector<v2f> const& rsPoints = pOutline->points;
        vector<bool> const& rsControls = pOutline->controls;

        c_v2f* p0 = nullptr;
        c_v2f* p1 = nullptr;
        c_v2f* p2 = nullptr;

        v2f* pTri0 = nullptr;
        v2f* pTri1 = nullptr;
        v2f* pTri2 = nullptr;

        bool b1 = false;
        bool b2 = false;

        vector<v2f*>* pvpConstraints = EMP_ALLOCATE(vector<v2f*>); // @@1 batch
        a_rvrvpOutlines.add(pvpConstraints);

        c_size stPointCount = rsPoints.count();
        for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
        {
            p0 = p1;
            p1 = p2;
            p2 = rsPoints.ptr(stPoint);

            b1 = b2;
            b2 = rsControls.val(stPoint);

            pTri0 = pTri1;
            pTri1 = pTri2;
            pTri2 = add_unique_point(a_rvrMesh, *p2);

            if (p1)
            {
                c_bool bControl = p0 && b1; // @@0 p0 or p1 ??
                c_bool bCCW = bControl && is_ccw(*pTri0, *pTri1, *pTri2);

                if (bControl == false && b2 == false)
                {
                    add_not_duplicate(*pvpConstraints, pTri1);
                    add_not_duplicate(*pvpConstraints, pTri2);

                    add_not_duplicate(a_rvpPoints, pTri1);
                    add_not_duplicate(a_rvpPoints, pTri2);
                }
                else if (bControl)
                {
                    if (bCCW == false)
                    {
                        add_not_duplicate(*pvpConstraints, pTri0);
                        add_not_duplicate(*pvpConstraints, pTri2);

                        add_not_duplicate(a_rvpPoints, pTri0);
                        add_not_duplicate(a_rvpPoints, pTri2);
                    }
                    else
                    {
                        add_not_duplicate(*pvpConstraints, pTri0);
                        add_not_duplicate(*pvpConstraints, pTri1);
                        add_not_duplicate(*pvpConstraints, pTri2);

                        add_not_duplicate(a_rvpPoints, pTri0);
                        add_not_duplicate(a_rvpPoints, pTri1);
                        add_not_duplicate(a_rvpPoints, pTri2);
                    }
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool point_in_list(cpc_v2f p, c_vector<v2f*>& a_rvpPoints);
EMP_INTERN EMP_RETURN bool point_in_list(cpc_v2f p, c_vector<v2f*>& a_rvpPoints)
{
    EMP_ASSERT(p != nullptr);

    c_size stCount = a_rvpPoints.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (p == a_rvpPoints.val(stIndex))
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool split_outline(cpc_v2f p, vector<v2f*>& a_rvrMesh, vector<v2f*>& EMP_SILENT(a_rvpPoints),
    vector<vector<v2f*>*>& a_rvrvpOutlines);
EMP_INTERN EMP_RETURN bool split_outline(cpc_v2f p, vector<v2f*>& a_rvrMesh, vector<v2f*>& EMP_SILENT(a_rvpPoints),
    vector<vector<v2f*>*>& a_rvrvpOutlines)
{
    c_size stOutlineCount = a_rvrvpOutlines.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        vector<v2f*>* pvpConstraints = a_rvrvpOutlines.val(stOutline);
        EMP_ASSERT(pvpConstraints != nullptr);

        vector<v2f*>& rvpConstraints = *pvpConstraints;

        for (size_t stIndex = 0; stIndex < rvpConstraints.count(); ++stIndex)
        {
            c_size stCount = rvpConstraints.count();
            c_size i0 = (stIndex + 0) % stCount;
            c_size i1 = (stIndex + 1) % stCount;

            cpc_v2f pi0 = rvpConstraints.val(i0);
            cpc_v2f pi1 = rvpConstraints.val(i1);

            if (pi0 != p &&
                pi1 != p)
            {
                /*point_s oXcm;
                oXcm.control = false;
                oXcm.position = *p;*/

                v2f* pTri = add_unique_point(a_rvrMesh, *p);
                if (point_in_list(p, rvpConstraints) == false &&
                    emp::mat::point_on_segment(*p, *pi0, *pi1))
                {
                    //EMP_LOG_DEBUG("p #0, #1", p->x, p->y);
                    //EMP_LOG_DEBUG("pi0 #0, #1", pi0->x, pi0->y);
                    //EMP_LOG_DEBUG("pi1 #0, #1", pi1->x, pi1->y);

                    //EMP_LOG_DEBUG("inserting constraint point : #0 : #1", rvpConstraints.count(), a_rvrMesh.count());
                    rvpConstraints.insert(i1, pTri);
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool split_outlines(vector<v2f*>& a_rvrMesh, vector<v2f*>& a_rvpPoints,
    vector<vector<v2f*>*>& a_rvrvpOutlines);
EMP_INTERN EMP_RETURN bool split_outlines(vector<v2f*>& a_rvrMesh, vector<v2f*>& a_rvpPoints,
    vector<vector<v2f*>*>& a_rvrvpOutlines)
{
    c_size stOutlineCount = a_rvrvpOutlines.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        vector<v2f*>* pvpConstraints = a_rvrvpOutlines.val(stOutline);
        EMP_ASSERT(pvpConstraints != nullptr);

        vector<v2f*>& rvpConstraints = *pvpConstraints;

        for (size_t stIndex = 0; stIndex < rvpConstraints.count(); ++stIndex)
        {
            cpc_v2f p = rvpConstraints.val(stIndex);

            if (split_outline(p, a_rvrMesh, a_rvpPoints, a_rvrvpOutlines) == false)
            { return false; }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool compute_boundary_triangle(shell<cpc_v2f> const& a_rspPoints, v2f& a_rP0, v2f& a_rP1, v2f& a_rP2);
EMP_INTERN EMP_RETURN bool compute_boundary_triangle(shell<cpc_v2f> const& a_rspPoints, v2f& a_rP0, v2f& a_rP1, v2f& a_rP2)
{
    float EMP_TT_MAX_VAR(fMinX);
    float EMP_TT_MAX_VAR(fMinY);
    
    float EMP_TT_MIN_VAR(fMaxX);
    float EMP_TT_MIN_VAR(fMaxY);

    c_size stCount = a_rspPoints.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_v2f p = a_rspPoints.val(stIndex);
        c_float x = p->x;
        c_float y = p->y;

        fMinX = emp::tt::minof(fMinX, x);
        fMinY = emp::tt::minof(fMinY, y);

        fMaxX = emp::tt::maxof(fMaxX, x);
        fMaxY = emp::tt::maxof(fMaxY, y);
    }

    if ((fMaxX > fMinX) == false ||
        (fMaxY > fMinY) == false)
    { return false; }

    EMP_ASSERT(fMaxX > fMinX);
    EMP_ASSERT(fMaxY > fMinY);

    c_float fDeltaXX = (fMaxX - fMinX);
    c_float fDeltaX = fDeltaXX * 5.0f;
    c_float fDeltaY = (fMaxY - fMinY) * 5.0f;

    /*float const fSqrt7_3 = sqrtf(21.0f);
    float const fHeightX = fMinX + 0.5f * fDeltaX;

    float const fHeightDeltaX = fSqrt7_3 * fDeltaY;

    a_rP0.x = fHeightX - fHeightDeltaX;
    a_rP0.y = fMinY - fDeltaY;

    a_rP1.x = fHeightX + fHeightDeltaX;
    a_rP1.y = fMinY - fDeltaY;

    a_rP2.x = fHeightX;
    a_rP2.y = fMaxY + fDeltaY;*/

    a_rP0 = { fMinX - fDeltaX, fMinY - fDeltaY };
    a_rP1 = { fMaxX + fDeltaX, fMinY - fDeltaY };
    a_rP2 = { fMinX + fDeltaXX * 0.5f, fMaxY + fDeltaY };

#ifdef EMP_XX_ASSERT
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cpc_v2f p = a_rspPoints.val(stIndex);
        EMP_ASSERT(point_in_triangle(*p, a_rP0, a_rP1, a_rP2));
    }
#endif

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void center_of_triangle(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2, v2f& c);
EMP_INTERN void center_of_triangle(v2f::tparam p0, v2f::tparam p1, v2f::tparam p2, v2f& c)
{ c = v2f::mul(v2f::add(p0, v2f::add(p1, p2)), v2f::all(1 / 3.0f)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool point_in_polygon(v2f::tparam p, shell<cpc_v2f> const& a_rspPoints)
{
    EMP_ASSERT(a_rspPoints.count() > 0);

    c_float x = p.x;
    c_float y = p.y;

    bool c = false;

    c_size stCount = a_rspPoints.count();

    size_t i = 0;
    size_t j = stCount - 1;

    for (; i < stCount; j = i++)
    {
        cpc_v2f pi = a_rspPoints.val(i);
        cpc_v2f pj = a_rspPoints.val(j);

        c_float xpi = pi->x;
        c_float ypi = pi->y;
        c_float xpj = pj->x;
        c_float ypj = pj->y;

        if ((((ypi <= y) && (y < ypj)) ||
             ((ypj <= y) && (y < ypi))) &&
            (x < (xpj - xpi) * (y - ypi) / (ypj - ypi) + xpi))
        {
          c = !c;
        }
    }

    return c;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool point_segment_sq_distance(v2f::tparam p, v2f::tparam v0, v2f::tparam v1, float& a_rfOut)
{
    c_v2f ab = v2f::sub(v1, v0);
    c_v2f ap = v2f::sub(p, v0);

    c_v2f vn = v2f::mul(ap, ab);
    c_float n = vn.x + vn.y;
    if (n < 0.0f)
    { return false; }
    EMP_ASSERT_NUMBER(n);

    c_v2f vd = v2f::mul(ab, ab);
    c_float d = vd.x + vd.y;
    if (n > d)
    { return false; }
    EMP_ASSERT_NUMBER(d);

    c_float s = n / d;
    EMP_ASSERT_NUMBER(s);

    c_v2f np = v2f::add(v0, v2f::mul(ab, v2f::all(s)));
    c_v2f vnd = v2f::sub(np, p);
    c_v2f vsqd = v2f::mul(vnd, vnd);

    c_float sq = vsqd.x + vsqd.y;
    EMP_ASSERT_NUMBER(sq);
    a_rfOut = sq;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool line_intersect_outlines(v2f::tparam l0, v2f::tparam l1, c_vector<vector<v2f*>*>& a_rvrvpOutlines)
{
    c_size stOutlineCount = a_rvrvpOutlines.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        c_vector<v2f*>* const pvpConstraints = a_rvrvpOutlines.val(stOutline);
        EMP_ASSERT(pvpConstraints);

        c_vector<v2f*>& rvpConstraints = *pvpConstraints;

        c_size stCount = rvpConstraints.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size i0 = (stIndex + 0) % stCount;
            c_size i1 = (stIndex + 1) % stCount;

            cpc_point_s pi0 = rvpConstraints.val(i0);
            cpc_point_s pi1 = rvpConstraints.val(i1);

            if (pi0 != l0 &&
                pi0 != l1 &&
                pi1 != l0 &&
                pi1 != l1)
            {
                if (emp::tri::segment_intersect_segment(pi0, pi1, l0, l1))
                {
                    return true;
                }
            }
        }
    }

    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool polygon_is_complex(c_vector<vector<v2f*>*>& a_rvrvpOutlines)
{
    c_size stOutlineCount = a_rvrvpOutlines.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        cpc_vector<v2f*> pvpConstraints = a_rvrvpOutlines.val(stOutline);
        EMP_ASSERT(pvpConstraints);

        c_vector<v2f*>& rvpConstraints = *pvpConstraints;

        c_size stCount = rvpConstraints.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size i0 = (stIndex + 0) % stCount;
            c_size i1 = (stIndex + 1) % stCount;

            cpc_point_s pi0 = rvpConstraints.val(i0);
            cpc_point_s pi1 = rvpConstraints.val(i1);

            if (line_intersect_outlines(pi0, pi1, a_rvrvpOutlines))
            {
                return true;
            }
        }
    }

    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool triangulate_inside(vector<outline_s*> const& a_rvOutlines, vector<float>& a_rvVertices, size_t& a_rstStride)
{
    a_rstStride = 2;

    vector<v2f*> vpMesh; // @@1 move up
    vector<v2f*> vpPoints; // @@1 move up
    vector<vector<v2f*>*> vpvpOutlines; // @@1 move up
    vector<polycell_c*> vpCells; // @@1 move up

    auto const oFinally = emp::tt::finally([&vpMesh, &vpvpOutlines, &vpCells] // @@0 move up
        {
            EMP_ASSERT_BOOL_CALL(emp::cnt::foreach(vpMesh, emp::mem::safe_delete_reset_t()));
            EMP_ASSERT_BOOL_CALL(emp::cnt::foreach(vpvpOutlines, emp::mem::safe_delete_reset_t()));
            EMP_ASSERT_BOOL_CALL(emp::cnt::foreach(vpCells, emp::mem::safe_delete_reset_t()));
        });

    EMP_RET_FALSE_IF_NOT(extract(a_rvOutlines, vpMesh, vpPoints, vpvpOutlines));
    EMP_RET_FALSE_IF_NOT(split_outlines(vpMesh, vpPoints, vpvpOutlines));

    /*if (polygon_is_complex(vrvpOutlines))
    {
        static size_t stComplexCount = 0;
        EMP_ASSERT_BOOL_CALL(
        emp::cnt::foreach(vrMesh, emp::mem::safe_delete_reset_t()));
        EMP_ASSERT_BOOL_CALL(
        emp::cnt::foreach(vrvpOutlines, emp::mem::safe_delete_reset_t()));
        EMP_LOG_INFO("Can not handle complex polygons : #0", stComplexCount);
        ++stComplexCount;
        return true;
    }*/

    c_shell<cpc_v2f> spPoints(vpPoints);

    v2f p0;
    v2f p1;
    v2f p2;

    EMP_RET_FALSE_IF_NOT(compute_boundary_triangle(spPoints, p0, p1, p2));

    polycell_c* pRoot = EMP_ALLOCATE(polycell_c);
    pRoot->index = 0;
    pRoot->points.resize(3);
    pRoot->points.resize(3);
    pRoot->set(0, &p0, nullptr);
    pRoot->set(1, &p1, nullptr);
    pRoot->set(2, &p2, nullptr);
    vpCells.add(pRoot);

    EMP_RET_FALSE_IF_NOT(delaunay(spPoints, vpCells));

    c_size stOutlineCount = vpvpOutlines.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        cpc_vector<v2f*> pvpConstraints = vpvpOutlines.val(stOutline);
        EMP_ASSERT(pvpConstraints != nullptr);
        c_vector<v2f*>& rvpConstraints = *pvpConstraints;

        c_size stConstraintCount = rvpConstraints.count();
        if (stConstraintCount > 2)
        {
            c_shell<cpc_v2f> spOutline(rvpConstraints);
            constraint(spOutline, vpCells);
        }
    }

    for (size_t stCell = 0; stCell < vpCells.count(); ++stCell)
    {
        cpc_polycell_c pCell = vpCells.val(stCell);
        EMP_ASSERT(pCell->points.count() == 3);

        cpc_v2f t0 = pCell->points.val(0);
        cpc_v2f t1 = pCell->points.val(1);
        cpc_v2f t2 = pCell->points.val(2);

        int i = 0;

        for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
        {
            cpc_vector<v2f*> pvpConstraints = vpvpOutlines.val(stOutline);
            EMP_ASSERT(pvpConstraints != nullptr);
            c_vector<v2f*>& rvpConstraints = *pvpConstraints;

            c_size stConstraintCount = rvpConstraints.count();
            if (stConstraintCount > 2)
            {
                shell<cpc_v2f> spOutline(rvpConstraints);

                v2f c;
                center_of_triangle(*t0, *t1, *t2, c);
                if (point_in_polygon(c, spOutline))
                {
                    if (is_ccw(spOutline.first_ptr(), spOutline.count()))
                    { --i; }
                    else { ++i; }
                }
            }
        }

        if (i > 0)
        {
            a_rvVertices.add(t0->x);
            a_rvVertices.add(t0->y);

            a_rvVertices.add(t1->x);
            a_rvVertices.add(t1->y);

            a_rvVertices.add(t2->x);
            a_rvVertices.add(t2->y);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool triangulate_outside(vector<outline_s*> const& a_rvOutlines, vector<float>& a_rvfVertices, size_t& a_rstStride)
{
    a_rstStride = 3;

    c_v2f* pP0 = nullptr;
    c_v2f* pP1 = nullptr;
    c_v2f* pP2 = nullptr;

    bool b0 = false;
    bool b1 = false;
    bool b2 = false;

    c_size stOutlineCount = a_rvOutlines.count();
    for (size_t stOutline = 0; stOutline < stOutlineCount; ++stOutline)
    {
        cpc_outline_s pOutline = a_rvOutlines.ref(stOutline);

        vector<v2f> const& rsPoints = a_rvOutlines.ref(stOutline)->points;
        vector<bool> const& rvbControls = pOutline->controls;

        c_size stPointCount = rsPoints.count();
        for (size_t stPoint = 0; stPoint < stPointCount; ++stPoint)
        {
            c_v2f& rPoint = rsPoints.ref(stPoint);
            pP0 = pP1;
            pP1 = pP2;
            pP2 = &rPoint;

            b0 = b1;
            b1 = b2;
            b2 = rvbControls.val(stPoint);

            if (pP1 && b1 &&
                pP0 && (b0 == false) &&
                pP2 && (b2 == false)) //-V560
            {
                c_bool bCCW = emp::mat::is_ccw(*pP0, *pP1, *pP2);
                c_float fSign = bCCW ? -1.0f : 1.0f;

                a_rvfVertices.add(pP0->x);
                a_rvfVertices.add(pP0->y);
                a_rvfVertices.add(fSign);

                a_rvfVertices.add(pP1->x);
                a_rvfVertices.add(pP1->y);
                a_rvfVertices.add(fSign);

                a_rvfVertices.add(pP2->x);
                a_rvfVertices.add(pP2->y);
                a_rvfVertices.add(fSign);
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool outline_s::sample(c_float a_fValue, v2f& a_rv2Out) const
{
    using namespace emp::mat;

    EMP_ASSERT(points.count() > 0);
    EMP_ASSERT(a_fValue >= 0.0f);
    EMP_ASSERT(a_fValue <= 1.0f);

    c_size stCount = points.count();

    size_t stDuration = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (controls.val(stIndex) == false)
        { ++stDuration; }
    }

    if (stDuration <= 1)
    { return false; }

    c_float fDuration = emp::ast::cast<float>(stDuration);
    c_float fAbsTime = a_fValue * fDuration;
    c_size stAbsTime = emp::ast::cast<size_t>(fAbsTime);
    c_float fTime = (fAbsTime - emp::ast::cast<float>(stAbsTime));

    size_t sti = 0;
    size_t stC = 0;
    for (size_t stIndex = 0; stIndex <= stCount; ++stIndex)
    {
        c_size stOffset = sti % stCount;
        if (controls.val(stOffset) == false)
        { ++stC; }
        if (stC > stAbsTime)
        { break; }
        ++sti;
    }

    sti %= stCount;

    c_v2f& pi = points.ref(sti);
    c_bool bi = controls.val(sti);

    c_v2f* p0 = nullptr;
    c_v2f* p1 = nullptr;
    c_v2f* p2 = nullptr;

    c_float fT = fTime;
    c_float fNT = 1.0f - fT;

    if (bi == false)
    {
        c_size stii = (sti + 1) % stCount;
        c_v2f& pii = points.ref(stii);
        c_bool bii = controls.val(stii);
        if (bii == false)
        {
            a_rv2Out = v2f::add(v2f::mul(pi, v2f::all(fNT)), v2f::mul(pii, v2f::all(fT)));
            return true;
        }
        else
        {
            c_size stiii = (stii + 1) % stCount;
            c_v2f& piii = points.ref(stiii);
            c_bool biii = controls.val(stiii);
            if (biii)
            { return false; }

            p0 = &pi;
            p1 = &pii;
            p2 = &piii;
        }
    }
    else
    {
        c_size stii = sti > 0 ? sti - 1 : stCount - 1;
        c_v2f& pii = points.ref(stii);
        c_bool bii = controls.val(stii);
        if (bii)
        { return false; }

        c_size stiii = (sti + 1) % stCount;
        c_v2f& piii = points.ref(stiii);
        c_bool biii = controls.val(stiii);
        if (biii)
        { return false; }

        p0 = &pii;
        p1 = &pi;
        p2 = &piii;
    }

    c_float fNT2 = fNT * fNT;
    c_v2f vNT2 = v2f::all(fNT2);

    c_float fT2 = fT * fT;
    c_v2f vT2 = v2f::all(fT2);

    c_float f2NTT = 2 * fNT * fT;
    c_v2f v2NTT = v2f::all(f2NTT);

    //P(t) = P0*(1-t)^2 + P1*2*(1-t)*t + P2*t^2
    a_rv2Out = v2f::add(v2f::mul(*p0, vNT2), v2f::add(v2f::mul(*p1, v2NTT), v2f::mul(*p2, vT2)));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
