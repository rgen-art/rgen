//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GFX_REFLECTION_RFX_H
#define EMP_GFX_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_icon.h"
#include "emp_gfx_model.h"
#include "emp_gfx_text.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

#include "emp_mat_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::buffer_i
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::buffer_t<ui8>
#define type_fields\
    enum_value_ex( emp::cnt::vector<ui8>, (EMP_RFX_B, data, m_vData) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::buffer_t<ui16>
#define type_fields\
    enum_value_ex( emp::cnt::vector<ui16>, (EMP_RFX_B, data, m_vData) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::vertex_2<emp::mat::v2f,emp::mat::v4f>
#define type_pod
#define type_fields\
    enum_value_ex( emp::mat::v2f, (EMP_RFX_B, v0, v0))\
    enum_value_ex( emp::mat::v4f, (EMP_RFX_B, v1, v1))
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::vertex_2<emp::mat::v4f,emp::mat::v4f>
#define type_pod
#define type_fields\
    enum_value_ex( emp::mat::v4f, (EMP_RFX_B, v0, v0))\
    enum_value_ex( emp::mat::v4f, (EMP_RFX_B, v1, v1))
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::buffer_t<emp::mat::v2f,emp::mat::v4f>
#define type_fields\
    enum_value_ex( (emp::cnt::vector<emp::gfx::vertex_2<emp::mat::v2f,emp::mat::v4f>>), (EMP_RFX_B, data, m_vData) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::buffer_t<emp::mat::v4f,emp::mat::v4f>
#define type_fields\
    enum_value_ex( (emp::cnt::vector<emp::gfx::vertex_2<emp::mat::v4f,emp::mat::v4f>>), (EMP_RFX_B, data, m_vData) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::glyph_s
#define type_pod
#define type_fields\
    enum_value_ex( ui32          , ( EMP_RFX_B , code , m_ui32Code ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::font_c
#define type_fields\
    enum_value_ex( astring                              , ( EMP_RFX_B , name              , m_sName))\
    enum_value_ex( float                                , ( EMP_RFX_B , line_height       , m_fLineHeight))\
    enum_value_ex( float                                , ( EMP_RFX_B , line_under_height , m_fLineUnderHeight))\
    enum_value_ex( float                                , ( EMP_RFX_B , line_over_height  , m_fLineOverHeight))\
    enum_value_ex( float                                , ( EMP_RFX_B , line_space        , m_fLineSpace))\
    enum_value_ex( float                                , ( EMP_RFX_B , char_width        , m_fCharWidth))\
    enum_value_ex( float                                , ( EMP_RFX_B , char_space        , m_fCharSpace))\
    enum_value_ex( float                                , ( EMP_RFX_B , space_width       , m_fSpaceWidth))\
    enum_value_ex( bool                                 , ( EMP_RFX_B , aspect_ratio      , m_bAspectRatio))\
    enum_value_ex( emp::cnt::vector<emp::gfx::glyph_s>  , ( EMP_RFX_B , glyphs            , m_vGlyphs))\
    enum_value_ex( emp::cnt::vector<emp::gfx::model_c*> , ( EMP_RFX_B , models            , m_vpModels))
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::mesh_c
#define type_fields\
    enum_value_ex( emp::gfx::buffer_i* , ( EMP_RFX_B , indices  , m_pIndices  ) )\
    enum_value_ex( emp::gfx::buffer_i* , ( EMP_RFX_B , vertices , m_pVertices ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::model_c
#define type_to_string STR_AFORMAT("Meshes : #0", a_rValue.meshes().count())
#define type_fields\
   enum_value_ex( emp::cnt::vector<emp::gfx::mesh_c*>, ( EMP_RFX_B , meshes , m_vpMeshes ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gfx::icon_c
#define type_fields\
    enum_value_ex( astring                              , ( EMP_RFX_B , name   , m_sName    ) )\
    enum_value_ex( emp::cnt::vector<emp::mat::glyph_s*> , ( EMP_RFX_B , curves , m_vpGlyphs ) )\
    enum_value_ex( emp::gfx::model_c *                  , ( EMP_RFX_B , model  , m_pModel   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

