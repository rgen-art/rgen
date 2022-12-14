//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_NOOP_H
#define EMP_XX_NOOP_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_MULTI_STATEMENT_BEGIN() do {
#define EMP_XX_MULTI_STATEMENT_END() } while(0)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_NOOP()\
EMP_XX_MULTI_STATEMENT_BEGIN()\
    (void)0;\
EMP_XX_MULTI_STATEMENT_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

