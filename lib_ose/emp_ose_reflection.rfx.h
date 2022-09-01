//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_compiler.h"

#include "emp_ose_renderer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

#include "emp_mat_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::ose::node_mutation_s
#define type_fields\
    enum_value_ex( bool  , ( EMP_RFX_B  , active    , m_bActive    ) )\
    enum_value_ex( ui32  , ( EMP_RFX_B  , count     , m_ui32Count  ) )\
    enum_value_ex( ui32  , ( EMP_RFX_B  , base      , m_ui32Base   ) )\
    enum_value_ex( float , ( EMP_RFX_B  , scale     , m_fScale     ) )\
    enum_value_ex( float , ( EMP_RFX_B  , offset    , m_fOffset    ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::ose::mutation_s
#define type_fields\
    enum_value_ex( emp::ose::node_mutation_s, ( EMP_RFX_B, literal , literal ) )\
    enum_value_ex( emp::ose::node_mutation_s, ( EMP_RFX_B, variant , variant ) )\
    enum_value_ex( emp::ose::node_mutation_s, ( EMP_RFX_B, call    , call    ) )\
    enum_value_ex( emp::ose::node_mutation_s, ( EMP_RFX_B, data    , data    ) )\
    enum_value_ex( emp::ose::node_mutation_s, ( EMP_RFX_B, trailing, trailing) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::ose::crossover_s
#define type_fields\
    enum_value_ex( bool , ( EMP_RFX_B  , active   , m_bActive    ) )\
    enum_value_ex( ui32 , ( EMP_RFX_B  , count    , m_ui32Count  ) )\
    enum_value_ex( ui32 , ( EMP_RFX_B  , base     , m_ui32Base   ) )\
    enum_value_ex( ui32 , ( EMP_RFX_B  , offset   , m_ui32Offset ) )\
    enum_value_ex( bool , ( EMP_RFX_B  , xfather  , m_bXFather   ) )\
    enum_value_ex( bool , ( EMP_RFX_B  , xmother  , m_bXMother   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
