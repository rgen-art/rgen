//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_INTERSECT_H
#define EMP_MAT_INTERSECT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN bool point_inside_rectangle(
    v2f::tparam a_v2Point,
    v2f::tparam a_v2Position,
    v2f::tparam a_v2Size);
inline EMP_RETURN bool point_inside_rectangle(
    v2f::tparam a_v2Point,
    v2f::tparam a_v2Position,
    v2f::tparam a_v2Size)
{
    return bool_and(v4f::more_equal(
        v4f::all(a_v2Point, a_v2Position),
        v4f::all(v2f::add(a_v2Position, a_v2Size), a_v2Point)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN bool rectangle_outside_rectangle(
    v2f::tparam a_v2PositionA,
    v2f::tparam a_v2SizeA,
    v2f::tparam a_v2PositionB,
    v2f::tparam a_v2SizeB);
inline EMP_RETURN bool rectangle_outside_rectangle(
    v2f::tparam a_v2PositionA,
    v2f::tparam a_v2SizeA,
    v2f::tparam a_v2PositionB,
    v2f::tparam a_v2SizeB)
{
    c_v4f v4P0 = v4f::all(a_v2PositionA, a_v2PositionB);
    c_v4f v4P1 = v4f::add(v4P0, v4f::all(a_v2SizeA, a_v2SizeB));

    c_v4f v4Min = v4f::min(v4P0, v4P1);
    c_v4f v4Max = v4f::max(v4P0, v4P1);

    return bool_or(v4f::more_equal(v4Min, v4Max));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN bool rectangle_inside_rectangle(
    v2f::tparam a_v2PositionA,
    v2f::tparam a_v2SizeA,
    v2f::tparam a_v2PositionB,
    v2f::tparam a_v2SizeB);
inline EMP_RETURN bool rectangle_inside_rectangle(
    v2f::tparam a_v2PositionA,
    v2f::tparam a_v2SizeA,
    v2f::tparam a_v2PositionB,
    v2f::tparam a_v2SizeB)
{
    c_v4f v4P0 = v4f::all(a_v2PositionA, a_v2PositionB);
    c_v4f v4P1 = v4f::add(v4P0, v4f::all(a_v2SizeA, a_v2SizeB));

    c_v4f v4Min = emp::mat::v4f::min(v4P0, v4P1);
    c_v4f v4Max = emp::mat::v4f::max(v4P0, v4P1);

    return bool_and(v4f::more_equal(v4Min, v4Max));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN bool rectangle_intersect_rectangle(
    v2f::tparam a_v2PositionA,
    v2f::tparam a_v2SizeA,
    v2f::tparam a_v2PositionB,
    v2f::tparam a_v2SizeB);
inline EMP_RETURN bool rectangle_intersect_rectangle(
    v2f::tparam a_v2PositionA,
    v2f::tparam a_v2SizeA,
    v2f::tparam a_v2PositionB,
    v2f::tparam a_v2SizeB)
{
    c_v4f v4P0 = v4f::all(a_v2PositionA, a_v2PositionB);
    c_v4f v4P1 = v4f::add(v4P0, v4f::all(a_v2SizeA, a_v2SizeB));

    c_v4f v4Min = emp::mat::v4f::min(v4P0, v4P1);
    c_v4f v4Max = emp::mat::v4f::max(v4P0, v4P1);

    return emp::mat::bool_or(emp::mat::v4f::more(v4Min, v4Max)) == false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN bool line_inside_rectangle(
    v2f::tparam a_v2Line0,
    v2f::tparam a_v2Line1,
    v2f::tparam a_v2Position,
    v2f::tparam a_v2Size);
inline EMP_RETURN bool line_inside_rectangle(
    v2f::tparam a_v2Line0,
    v2f::tparam a_v2Line1,
    v2f::tparam a_v2Position,
    v2f::tparam a_v2Size)
{
    return
        point_inside_rectangle(a_v2Line0, a_v2Position, a_v2Size) &&
        point_inside_rectangle(a_v2Line1, a_v2Position, a_v2Size);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN bool line_intersect_rectangle(
    v2f::tparam a_v2Line0,
    v2f::tparam a_v2Line1,
    v2f::tparam a_v2Position,
    v2f::tparam a_v2Size);
inline EMP_RETURN bool line_intersect_rectangle(
    v2f::tparam a_v2Line0,
    v2f::tparam a_v2Line1,
    v2f::tparam a_v2Position,
    v2f::tparam a_v2Size)
{ return rectangle_outside_rectangle(a_v2Line0, v2f::sub(a_v2Line1, a_v2Line0), a_v2Position, a_v2Size); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
