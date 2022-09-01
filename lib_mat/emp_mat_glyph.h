//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_POLYGON_H
#define EMP_MAT_POLYGON_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool point_segment_sq_distance(v2f::tparam p, v2f::tparam v0, v2f::tparam v1, float& a_rfOut);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(struct, outline_s) // @@1 remove class ?
public:
    astring name;
    vector<v2f> points;
    vector<bool> controls;

public:
    outline_s(void):
        name(),
        points(),
        controls()
    {}

public:
    EMP_RETURN bool sample(c_float a_fValue, v2f& a_rv2Out) const;
};
EMP_TYPEDEF(outline_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(struct, glyph_s) // @@1 remove class ?
public:
    astring name;
    vector<outline_s*> outlines;

public:
    glyph_s(void):
        name(),
        outlines()
    {}

public:
    EMP_RETURN bool triangulate(vector<shell<c_v3f>>& a_rvsPoints,
        vector<float>& a_rvInVertices, c_size a_stInStride,
        vector<float>& a_rvOutVertices, c_size a_stOutStride);
};
EMP_TYPEDEF(glyph_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool triangulate_inside(vector<outline_s*> const& a_rvOutlines, vector<float>& a_rvVertices, size_t& a_rstStride);
EMP_RETURN bool triangulate_outside(vector<outline_s*> const& a_rvOutlines, vector<float>& a_rvfVertices, size_t& a_rstStride);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
