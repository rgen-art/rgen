//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_VECTOR_MAP_H
#define EMP_CNT_VECTOR_MAP_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_copy.h"
#include "emp_cnt_map.h"
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K, typename V>
EMP_NOCOPY(struct, vector_map_t)
public:
    typedef emp::cnt::map_t<K, V> map_d;
    typedef emp::cnt::vector<V> vector_d;

private:
    map_d m_Map;
    vector_d m_Vector;

public:
    explicit vector_map_t(c_size a_stSize):
        m_Map(a_stSize),
        m_Vector()
    {}

public:
    EMP_RETURN size_t count(void) const
    { return m_Vector.count(); }

public:
    void clear(void)
    {
        m_Vector.resize(0);
        m_Map.clear();
    }

public:
    void set(c_size a_stIndex, K const& a_rKey, V const& a_rValue)
    {
        m_Vector.set(a_stIndex, a_rValue);
        m_Map.set(a_rKey, a_rValue);
    }

    EMP_RETURN bool add(K const& a_rKey, V const& a_rValue)
    {
        if (m_Map.get(a_rKey) != m_Map.end())
        { return false; }

        m_Vector.add(a_rValue);
        m_Map.set(a_rKey, a_rValue);
        return true;
    }

public:
    void copy(vector_map_t const& a_rFrom)
    {
        emp::cnt::copy(a_rFrom.m_Vector, m_Vector);
        emp::cnt::copy(a_rFrom.m_Map, m_Map);
    }

    EMP_RETURN vector_d const& vector(void) const { return m_Vector; } 
    EMP_RETURN vector_d& vector(void) { return m_Vector; }

    EMP_RETURN map_d const& map(void) const { return m_Map; }
    EMP_RETURN map_d& map(void) { return m_Map; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
