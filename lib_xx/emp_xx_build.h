//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_BUILD_H
#define EMP_XX_BUILD_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_COUNTER __COUNTER__
#define EMP_XX_LINE __LINE__
#define EMP_XX_FUNCTION __FUNCTION__
#if defined EMP_XX_COMPILER_MSC
#define EMP_XX_SIGNATURE __FUNCSIG__
#else
#define EMP_XX_SIGNATURE __PRETTY_FUNCTION__
#endif
#define EMP_XX_FILE __FILE__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

