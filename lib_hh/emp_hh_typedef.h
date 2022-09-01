//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_HH_TYPEDEF_H
#define EMP_HH_TYPEDEF_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_stddef.h"
#include "emp_hh_stdint.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_TYPEDEF     ( int                   )
EMP_TYPEDEF_EX  ( int8_t        , si8   )
EMP_TYPEDEF_EX  ( int16_t       , si16  )
EMP_TYPEDEF_EX  ( int32_t       , si32  )
EMP_TYPEDEF_EX  ( int64_t       , si64  )

EMP_TYPEDEF_EX  ( unsigned int  , uint  )
EMP_TYPEDEF_EX  ( uint8_t       , ui8   )
EMP_TYPEDEF_EX  ( uint16_t      , ui16  )
EMP_TYPEDEF_EX  ( uint32_t      , ui32  )
EMP_TYPEDEF_EX  ( uint64_t      , ui64  )

EMP_TYPEDEF_EX  ( size_t        , size  )

EMP_TYPEDEF     ( char                  )
EMP_TYPEDEF_EX  ( unsigned char , uchar )

EMP_TYPEDEF     ( bool                  )

EMP_TYPEDEF     ( float                 )
EMP_TYPEDEF_EX  ( float         , f32   )
EMP_TYPEDEF     ( double                )
EMP_TYPEDEF_EX  ( double        , f64   )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

