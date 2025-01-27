///--- The Helix Project ------------------------------------------------------------------------///
///                                                                                              ///
///   Part of the Helix Project, under the Attribution 4.0 International license (CC BY 4.0).    ///
///   You are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   For more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   SPDX-License-Identifier: CC-BY-4.0                                                         ///
///   Copyright (c) 2024 The Helix Project (CC BY 4.0)                                           ///
///                                                                                              ///
///------------------------------------------------------------------------------------ Helix ---///

#ifndef __$LIBHELIX_LIBC__
#define __$LIBHELIX_LIBC__

#include "config.h"
#include "primitives.h"

H_NAMESPACE_BEGIN

namespace libc {
#include <stdio.h>
#include <errno.h>
#include <fenv.h>
#include <locale.h>
#include <time.h>
#include <wchar.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <uchar.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include "cxxabi.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#include <direct.h>
#include <io.h>
#include <process.h>
#endif
template <typename T>
using va_array = T[];

template <typename T, usize N>
using array = T[N];
}  // namespace libc

using _H_RESERVED$char = char;

H_NAMESPACE_END
#endif