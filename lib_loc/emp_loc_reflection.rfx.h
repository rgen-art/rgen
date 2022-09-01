//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_LOC_REFLECTION_RFX_H
#define EMP_LOC_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_loc_locale.h"
#include "emp_loc_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::loc::string_s
#define type_to_string a_rValue.key
#define type_fields\
    enum_value_ex( astring , ( EMP_RFX_B , key   , key     ) )\
    enum_value_ex( u8string, ( EMP_RFX_B , value , value   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::loc::locale_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( astring                              , ( EMP_RFX_B , name    , m_sName    ) )\
    enum_value_ex( emp::cnt::vector<emp::loc::string_s> , ( EMP_RFX_B , strings , m_vStrings ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
