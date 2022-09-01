//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_ERROR_H
#define EMP_OSE_ERROR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_LOUSY
#define EMP_OSE_DEBUG
//#elif defined EMP_XX_DEBUG
#define EMP_OSE_DEBUG
#endif

//#define EMP_OSE_DEBUG
//#define EMP_OSE_DEBUG_FULL
#if defined EMP_XX_OS_OSX && defined EMP_XX_DEBUG
#define EMP_OSE_RENDERING
#else
#define EMP_OSE_RENDERING
#endif

#ifdef EMP_OSE_DEBUG
#define EMP_OSE_IF_DEBUG(x_Action) x_Action
#else
#define EMP_OSE_IF_DEBUG(x_Action)
#endif

#ifdef EMP_OSE_DEBUG_FULL
#define EMP_OSE_IF_DEBUG_FULL(x_Action) x_Action
#else
#define EMP_OSE_IF_DEBUG_FULL(x_Action)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name return_e
#define enum_values\
    enum_value( Error    )\
    enum_value( Continue )\
    enum_value( Break    )\
    enum_value( Return   )\
    enum_value( Halt     )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(info_s)
    size_t EMP_TT_MAX_VAR(m_stLine);
    size_t EMP_TT_MAX_VAR(m_stColumn);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

