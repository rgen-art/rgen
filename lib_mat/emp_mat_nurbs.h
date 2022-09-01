//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_NURBS_H
#define EMP_MAT_NURBS_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, nurbs_c)
public:
    typedef vector<float> tv_knots;
    typedef vector<v4f> tv_points;

private:
    astring m_sName;
    tv_knots m_vfKnots;
    tv_points m_vv4Points;
    vector<size_t> m_vstPoints;
    size_t m_stDegree;
    tv_points m_vv4Samples;

public:
    nurbs_c(void);

    void create(void);
    void destroy(void);

    EMP_RETURN bool pre_load(void);
    EMP_RETURN bool post_load(void);

    EMP_RETURN tv_knots const& knots(void) const { return m_vfKnots; }
    EMP_RETURN tv_points const& points(void) const { return m_vv4Points; }

    EMP_RETURN float solve(c_float a_fValue) const;
    EMP_RETURN emp::mat::v4f::tret sample(c_float a_fTime) const;
    EMP_RETURN emp::mat::v4f::tret sample(c_float a_fTime, c_size a_stDegree) const;

    EMP_RETURN size_t move_point(c_size a_stIndex, emp::mat::v4f::tparam a_v4Delta);

    void add_knot(c_float a_fKnot);
    void add_point(emp::mat::v4f::tparam a_v4Point, c_bool a_bAutoKnot);
    void insert_point(emp::mat::v4f::tparam a_v4Point);
    void insert_point(c_size a_stIndex, emp::mat::v4f::tparam a_v4Point);
    void remove_point(c_size a_stIndex);
    void set_point(c_size a_stIndex, emp::mat::v4f::tparam a_v4Point);
    //static EMP_RETURN bool contains( emp::mat::v4f::tparam a_v4Client, emp::mat::v4f::tparam a_v4Point,
    //    emp::mat::v4f::tparam a_v4Size);
    // EMP_RETURN size_t contains(emp::mat::v4f::tparam a_v4Position, emp::mat::v4f::tparam a_v4Size) const;

    EMP_RETURN size_t degree(void) const { return m_stDegree; }
    void set_degree(c_size a_stValue);

    EMP_RETURN c_astring& name(void) const { return m_sName; }

private:
    EMP_RETURN float solve_linear(c_float a_fValue) const;
    EMP_RETURN float solve_quadratic(c_float a_fValue) const;
    //EMP_RETURN float solve_cubic(float const a_fValue);

    void compute_knots(void);
    void compute_samples(void);

    EMP_RETURN size_t get_knot_index(c_float a_fU) const;
    EMP_RETURN size_t get_point_index(c_float a_fV) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
