//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_HH_STDIO_H
#define EMP_HH_STDIO_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE_STL
#ifdef __STRICT_ANSI__
// @@0 ugly
#undef __STRICT_ANSI__
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#define __STRICT_ANSI__
#else
#include <stdio.h>
#include <sys/stat.h>

#include <fcntl.h>
#endif

#if defined EMP_XX_OS_WIN
#else
#include <unistd.h>
#endif
EMP_PRAGMA_POP_IGNORE_STL

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

