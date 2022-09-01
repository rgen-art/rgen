//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_VECTOR_H
#define EMP_CNT_VECTOR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_container.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_reallocable.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
using vector = container_t<T, emp::mem::reallocable_t<T, false>>;

/*template <typename T>
EMP_NOCOPY_BASE(class,vector, private, container_t<T, emp::mem::reallocable_t<T, false>>)
public:
    using typename tbase1::type;

    using tbase1::allocate;
    using tbase1::resize;
    using tbase1::remove;
    using tbase1::remove_last;
    using tbase1::remove_unordered;
    using tbase1::insert;

    using tbase1::is_valid_index;

    using tbase1::set;
    using tbase1::set_last;
    using tbase1::add;
    using tbase1::push;
    using tbase1::swap;
    using tbase1::swap_last;

    using tbase1::val;
    using tbase1::ref;
    using tbase1::ptr;
    using tbase1::last_val;
    using tbase1::last_ref;
    using tbase1::last_ptr;
    using tbase1::first_val;
    using tbase1::first_ref;

    using tbase1::storage;

    using tbase1::size;
    using tbase1::count;

    using typename tbase1::iterator;
    using typename tbase1::const_iterator;

    using tbase1::begin;
    using tbase1::end;

public:
    vector(void)
    { }

    EMP_INLINE ~vector(void)
    {}
};*/

template <typename T> using c_vector = vector<T> const;
template <typename T> using cp_vector = vector<T> const*;
template <typename T> using cpc_vector = vector<T> const* const;


template <typename T> using pod_vector = container_t<T, emp::mem::reallocable_t<T, true>>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
EMP_NOCOPY_BASE(class,pod_vector, private, container_t<T, reallocable_storage<T, true>>)
public:
    using typename tbase1::type;

    using tbase1::allocate;
    using tbase1::resize;
    using tbase1::remove;
    using tbase1::remove_last;
    using tbase1::remove_unordered;
    using tbase1::insert;

    using tbase1::is_valid_index;

    using tbase1::set;
    using tbase1::set_last;
    using tbase1::add;
    using tbase1::push;
    using tbase1::swap;
    using tbase1::swap_last;

    using tbase1::val;
    using tbase1::ref;
    using tbase1::ptr;
    using tbase1::last_val;
    using tbase1::last_ref;
    using tbase1::last_ptr;
    using tbase1::first_val;
    using tbase1::first_ref;

    using tbase1::storage;

    using tbase1::size;
    using tbase1::count;

    using typename tbase1::iterator;
    using typename tbase1::const_iterator;

    using tbase1::begin;
    using tbase1::end;

public:
    pod_vector(void)
    { }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
