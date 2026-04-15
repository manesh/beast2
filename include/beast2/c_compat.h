/*
 * Portable macros missing from some MSVC C headers (e.g. S_ISDIR).
 */
#ifndef BEAST2_C_COMPAT_H
#define BEAST2_C_COMPAT_H

#include <sys/stat.h>

#if defined(_WIN32)
#if !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif
#if !defined(S_ISREG)
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#endif
#endif

#endif
