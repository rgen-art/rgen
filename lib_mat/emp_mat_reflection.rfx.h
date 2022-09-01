//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_REFLECTION_RFX_H
#define EMP_MAT_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_glyph.h"
#include "emp_mat_nurbs.h"
#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::mat::v4f
#define type_pod
#define type_to_string_impl \
{\
    u8string sx;\
    u8string sy;\
    u8string sz;\
    u8string sw;\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.x, a_pFormater, sx));\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.y, a_pFormater, sy));\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.z, a_pFormater, sz));\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.w, a_pFormater, sw ));\
    emp::tt::silent(STR_FORMATU8(a_rsOut, "{ #0; #1; #2; #3 }", sx, sy, sz, sw));\
    return true;\
}
#define type_fields\
    enum_value_ex( float, ( EMP_RFX_B , x , x ) )\
    enum_value_ex( float, ( EMP_RFX_B , y , y ) )\
    enum_value_ex( float, ( EMP_RFX_B , z , z ) )\
    enum_value_ex( float, ( EMP_RFX_B , w , w ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::mat::v3f
#define type_pod
#define type_to_string_impl \
{\
    u8string sx;\
    u8string sy;\
    u8string sz;\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.x, a_pFormater, sx));\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.y, a_pFormater, sy));\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.z, a_pFormater, sz));\
    emp::tt::silent(STR_FORMATU8(a_rsOut, "{ #0; #1; #2; }", sx, sy, sz));\
    return true;\
}
#define type_fields\
    enum_value_ex( float, ( EMP_RFX_B , x , x ) )\
    enum_value_ex( float, ( EMP_RFX_B , y , y ) )\
    enum_value_ex( float, ( EMP_RFX_B , z , z ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::mat::v2f
#define type_pod
#define type_to_string_impl \
{\
    u8string sx;\
    u8string sy;\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.x, a_pFormater, sx));\
    EMP_RET_FALSE_IF_NOT(to_string_impl(a_rValue.y, a_pFormater, sy));\
    emp::tt::silent(STR_FORMATU8(a_rsOut, "{ #0; #1; }", sx, sy));\
    return true;\
}
#define type_fields\
    enum_value_ex( float , ( EMP_RFX_B , x , x ) )\
    enum_value_ex( float , ( EMP_RFX_B , y , y ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::mat::nurbs_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( astring                         , ( EMP_RFX_B , name   , m_sName     ) )\
    enum_value_ex( emp::cnt::vector<float>         , ( EMP_RFX_B , knots  , m_vfKnots   ) )\
    enum_value_ex( emp::cnt::vector<emp::mat::v4f> , ( EMP_RFX_B , points , m_vv4Points ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*#define type_name emp::mat::point_s
#define type_to_string_impl \
{\
    u8string s;\
    EMP_RET_FALSE_IF_NOT(to_string(a_rValue.position, nullptr, s));\
    STR_FORMATU8(a_rsOut, "#0, #1", a_rValue.control, s);\
    return true;\
}
#define type_fields\
    enum_value_ex( bool          , ( EMP_RFX_B , control  , control  ) )\
    enum_value_ex( emp::mat::v2f , ( EMP_RFX_B , position , position ) )
#include "emp_rfx_class.def.h"*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::mat::outline_s
#define type_to_string a_rValue.name
#define type_fields\
    enum_value_ex( astring                         , ( EMP_RFX_B , name     , name     ) )\
    enum_value_ex( emp::cnt::vector<emp::mat::v2f> , ( EMP_RFX_B , points   , points   ) )\
    enum_value_ex( emp::cnt::vector<bool>          , ( EMP_RFX_B , controls , controls ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::mat::glyph_s
#define type_to_string a_rValue.name
#define type_fields\
    enum_value_ex( astring                                , ( EMP_RFX_B , name     , name      ) )\
    enum_value_ex( emp::cnt::vector<emp::mat::outline_s*> , ( EMP_RFX_B , outlines , outlines  ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
