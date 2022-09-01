//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_ATTRIBUTE_H
#define EMP_XX_ATTRIBUTE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_xx_pragma.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined(__GNUC__) // @@0 EMP_XX_COMPILER_

#define EMP_SILENT(...) __VA_ARGS__ EMP_UNUSED
#define EMP_UNUSED __attribute__ ((unused))
#define EMP_WARN_UNUSED __attribute__ ((warn_unused_result))
#define EMP_ALWAYS_INLINE
// __attribute__ ((always_inline))
#define EMP_NOINLINE __attribute__ ((noinline))
#define EMP_NORETURN __attribute__ ((noreturn))
#define EMP_RETURN EMP_WARN_UNUSED

#elif defined(EMP_XX_COMPILER_MSC)

#include "emp_hh_sal.h"

#define EMP_SILENT(...)
#define EMP_UNUSED
#define EMP_WARN_UNUSED
#define EMP_ALWAYS_INLINE
#define EMP_NOINLINE
#define EMP_NORETURN
#define EMP_RETURN _Check_return_

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_INLINE inline EMP_ALWAYS_INLINE
#define EMP_INTERN static

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

