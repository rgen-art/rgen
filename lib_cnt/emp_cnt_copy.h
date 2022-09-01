//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_COPY_H
#define EMP_CNT_COPY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> void copy(T const& a_rFrom, T& a_rTo);
template <typename T> void copy(T const& a_rFrom, T& a_rTo)
{
    c_size stCount = a_rFrom.count();
    a_rTo.resize(a_rFrom.count());
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_rTo.set(stIndex, a_rFrom.ref(stIndex)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename TA, typename TB> void copy(TA const& a_rFrom, TB& a_rTo);
template <typename TA, typename TB> void copy(TA const& a_rFrom, TB& a_rTo)
{
    c_size stCount = a_rFrom.count();
    a_rTo.resize(a_rFrom.count());
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { a_rTo.set(stIndex, a_rFrom.ref(stIndex)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> void append(T const& a_rFrom, T& a_rTo);
template <typename T> void append(T const& a_rFrom, T& a_rTo)
{
    c_size stFromCount = a_rFrom.count();
    c_size stToCount = a_rTo.count();
    c_size stCount = stFromCount + stToCount;
    a_rTo.resize(stCount);
    for (size_t stIndex = 0; stIndex < stFromCount; ++stIndex)
    { a_rTo.set(stToCount + stIndex, a_rFrom.ref(stIndex)); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
