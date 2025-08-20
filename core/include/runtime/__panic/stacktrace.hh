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
///-------------------------------------------------------------------------------- Lib-Helix ---///

#ifndef _$_HX_CORE_M10STACKTRACE
#define _$_HX_CORE_M10STACKTRACE

#include <include/config/config.h>
#include <include/c++/libc++.hh>
#include <include/types/types.hh>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <string.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN
namespace StackTrace {

struct StackFrame {
    libcxx::string function;
    libcxx::string file;
    int            line;

    StackFrame() : line(0) {}
};

// Forward declaration
libcxx::vector<StackFrame> get_frames(int max_depth = 32);

#ifdef _WIN32

inline libcxx::vector<StackFrame> get_frames(int max_depth) {
    libcxx::vector<StackFrame> trace;
    trace.reserve(max_depth);

    static const bool initialized = []() {
        HANDLE process = GetCurrentProcess();
        if (!SymInitialize(process, nullptr, TRUE)) {
            return false;
        }
        return true;
    }();

    if (!initialized) {
        return trace;
    }

    void* stack[32];
    USHORT frames = CaptureStackBackTrace(0, libcxx::min(max_depth, 32), stack, nullptr);
    HANDLE process = GetCurrentProcess();

    alignas(16) char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    IMAGEHLP_LINE64 lineInfo;
    lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    for (USHORT i = 0; i < frames; ++i) {
        StackFrame frame;
        DWORD64 address = (DWORD64)(stack[i]);

        if (SymFromAddr(process, address, nullptr, symbol)) {
            frame.function = symbol->Name;
        } else {
            frame.function = "???";
        }

        DWORD displacement = 0;
        if (SymGetLineFromAddr64(process, address, &displacement, &lineInfo)) {
            frame.file = lineInfo.FileName ? lineInfo.FileName : "???";
            frame.line = static_cast<int>(lineInfo.LineNumber);
        } else {
            frame.file = "???";
            frame.line = 0;
        }

        trace.push_back(frame);
    }

    return trace;
}

#else // POSIX (Linux/macOS)

inline libcxx::vector<StackFrame> get_frames(int max_depth) {
    libcxx::vector<StackFrame> trace;
    trace.reserve(max_depth);

    void* stack[32];
    int size = backtrace(stack, libcxx::min(max_depth, 32));

    // Get symbols for additional info
    char** symbols = backtrace_symbols(stack, size);

    for (int i = 1; i < size; ++i) { // Skip frame 0 (get_frames)
        StackFrame frame;
        Dl_info info;

        // Get function name
        if (dladdr(stack[i], &info) && info.dli_sname && info.dli_sname[0] != '\0') {
            int status = -1;
            char* demangled = nullptr;
            if (info.dli_sname[0] == '_') {
                demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            }
            frame.function = (status == 0 && demangled) ? demangled : info.dli_sname;
            free(demangled);
        } else {
            frame.function = "???";
        }

        // Try to extract file from backtrace_symbols
        if (symbols && i < size && symbols[i]) {
            // Format: <binary> (<function>+<offset>) [<address>]
            // or sometimes includes file:line in debug builds
            char* symbol = symbols[i];
            char* paren = strchr(symbol, '(');
            if (paren) {
                *paren = '\0'; // Isolate binary path
                frame.file = symbol; // May be binary path or source file
                // Check for file:line pattern (rare without atos)
                char* colon = strrchr(symbol, ':');
                if (colon && colon[1] >= '0' && colon[1] <= '9') {
                    frame.line = atoi(colon + 1);
                    *colon = '\0';
                    frame.file = symbol;
                }
            } else {
                frame.file = info.dli_fname ? info.dli_fname : "???";
            }
        } else {
            frame.file = info.dli_fname ? info.dli_fname : "???";
        }

        // Line numbers unavailable without external tools
        frame.line = 0;

        trace.push_back(frame);
    }

    if (symbols) {
        free(symbols);
    }

    return trace;
}

#endif

} // namespace StackTrace
H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif