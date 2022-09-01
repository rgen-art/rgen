//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_FOREACH_H
#define EMP_CNT_FOREACH_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_logical.h"
#include "emp_tt_const.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
using iterator = emp::tt::if_else<emp::tt::is_const<T>::value, typename T::const_iterator, typename T::iterator>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Container, typename t_Functor> bool foreach(t_Container& a_rContainer, t_Functor const& a_rFunctor);
template <typename t_Container, typename t_Functor> bool foreach(t_Container& a_rContainer, t_Functor const& a_rFunctor)
{
    iterator<t_Container> const itEnd(a_rContainer.end());
    for (iterator<t_Container> itCurrent = a_rContainer.begin(); itCurrent != itEnd; ++itCurrent)
    {
        if (a_rFunctor(itCurrent.ref()) == false)
        { return false; }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> void delete_null_resize(vector<T*>& a_rVector);
template <typename T> void delete_null_resize(vector<T*>& a_rVector)
{
    c_size stCount = a_rVector.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        T* pPointer = a_rVector.val(stIndex);
        EMP_DEALLOCATE(pPointer);
        a_rVector.set(stIndex, nullptr);
    }
    a_rVector.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
