

#ifndef _$_HX_CORE_M10STACKTRACE
#define _$_HX_CORE_M10STACKTRACE

#include <cstddef>
#include <cstdint>
#include <include/c++/libc++.hh>
#include <include/types/types.hh>

#ifdef _WIN32
#include <dbghelp.h>
#include <windows.h>
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

namespace Backtrace {

#if defined(_MSC_VER)
#define HX_FUNCNAME __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define HX_FUNCNAME __PRETTY_FUNCTION__
#else
#define HX_FUNCNAME __func__
#endif

enum class FrameKind : uint8_t {
    Helix  = 0,
    Native = 1,
};

struct SrcLoc {
    const char *file;
    const char *func;
    uint32_t    line;
};

struct HelixFrame {
    const SrcLoc     *loc;
    const HelixFrame *prev;
};

#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
inline thread_local const HelixFrame *g_tls_helix_head = nullptr;
#else
inline const HelixFrame *g_tls_helix_head = nullptr;
#endif

struct HelixFrameScope {
    HelixFrame frame;
    explicit HelixFrameScope(const SrcLoc *loc) noexcept {
        frame.loc        = loc;
        frame.prev       = g_tls_helix_head;
        g_tls_helix_head = &frame;
    }
    ~HelixFrameScope() noexcept { g_tls_helix_head = frame.prev; }

    HelixFrameScope(const HelixFrameScope &)            = delete;
    HelixFrameScope &operator=(const HelixFrameScope &) = delete;
    HelixFrameScope(HelixFrameScope &&)                 = delete;
    HelixFrameScope &operator=(HelixFrameScope &&)      = delete;
};

#define HX_FUNC_SCOPE(file_cstr, line_num, func_cstr)               \
    static constexpr ::hx::SrcLoc _hx_loc{                          \
        (file_cstr), (func_cstr), static_cast<uint32_t>(line_num)}; \
    ::hx::HelixFrameScope _hx_scope(&_hx_loc)

#define HX_TRACE_CPP()                                           \
    static constexpr ::hx::SrcLoc _hx_cpp_loc{                   \
        __FILE__, HX_FUNCNAME, static_cast<uint32_t>(__LINE__)}; \
    ::hx::HelixFrameScope _hx_cpp_scope(&_hx_cpp_loc)

struct MergedEntry {
    const char *func;
    const char *file;
    uint32_t    line;
    FrameKind   kind;
};

inline const HelixFrame *helix_tls_head() noexcept { return g_tls_helix_head; }

inline uint32_t helix_tls_depth() noexcept {
    uint32_t          n = 0;
    const HelixFrame *p = g_tls_helix_head;
    while (p) {
        ++n;
        p = p->prev;
    }
    return n;
}



struct NativeCapture {

    void       **pcs;
    const char **names;
    uint32_t     count;
};

#if defined(_WIN32)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <dbghelp.h>
#include <windows.h>

namespace detail {


    constexpr uint32_t           kRing   = 128;
    constexpr uint32_t           kSymMax = 1024;
    inline thread_local char     g_sym_ring[kRing][kSymMax];
    inline thread_local uint32_t g_ring_idx = 0;

    inline char *next_ring_buf() noexcept {
        uint32_t i = g_ring_idx;
        g_ring_idx = (g_ring_idx + 1u) &
                     (kRing - 1u);
        return g_sym_ring[i];
    }

    inline bool dbghelp_init_once() noexcept {
        static thread_local bool inited = false;
        if (inited)
            return true;
        HANDLE proc = GetCurrentProcess();
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_NO_PROMPTS);
        if (!SymInitialize(proc, NULL, TRUE)) {

            inited = true;
            return false;
        }
        inited = true;
        return true;
    }

}

inline uint32_t native_capture_pcs(void **pcs, uint32_t max, uint32_t skip) noexcept {
    if (!pcs || max == 0)
        return 0;

    USHORT n =
        RtlCaptureStackBackTrace(static_cast<DWORD>(skip), static_cast<DWORD>(max), pcs, nullptr);
    return static_cast<uint32_t>(n);
}

inline uint32_t
native_resolve_symbols(void **pcs, uint32_t count, const char **names_out) noexcept {
    if (!pcs || !names_out)
        return 0;
    detail::dbghelp_init_once();

    HANDLE proc = GetCurrentProcess();

    alignas(SYMBOL_INFO) static thread_local unsigned char
                 sym_buf[sizeof(SYMBOL_INFO) + detail::kSymMax];
    PSYMBOL_INFO pSym  = reinterpret_cast<PSYMBOL_INFO>(sym_buf);
    pSym->MaxNameLen   = detail::kSymMax - 1;
    pSym->SizeOfStruct = sizeof(SYMBOL_INFO);

    uint32_t wrote = 0;
    for (uint32_t i = 0; i < count; ++i) {
        DWORD64     addr = (DWORD64)pcs[i];
        const char *name = nullptr;
        if (SymFromAddr(proc, addr, 0, pSym)) {

            char *dst = detail::next_ring_buf();

            char       *d   = dst;
            const char *s   = pSym->Name;
            size_t      cap = detail::kSymMax - 1;
            while (*s && cap) {
                *d++ = *s++;
                --cap;
            }
            *d   = '\0';
            name = dst;
        }
        names_out[i] = name;
        ++wrote;
    }
    return wrote;
}

#elif defined(__APPLE__) || defined(__linux__)

#include <dlfcn.h>
#include <execinfo.h>

inline uint32_t native_capture_pcs(void **pcs, uint32_t max, uint32_t skip) noexcept {
    if (!pcs || max == 0)
        return 0;

    int n = ::backtrace(pcs, static_cast<int>(max + skip));
    if (n <= 0)
        return 0;

    int out = n - static_cast<int>(skip);
    if (out <= 0)
        return 0;

    for (int i = 0; i < out; ++i)
        pcs[i] = pcs[i + skip];
    return static_cast<uint32_t>(out);
}

inline uint32_t
native_resolve_symbols(void **pcs, uint32_t count, const char **names_out) noexcept {
    if (!pcs || !names_out)
        return 0;
    for (uint32_t i = 0; i < count; ++i) {
        Dl_info     info{};
        const char *name = nullptr;
        if (dladdr(pcs[i], &info) && info.dli_sname) {
            name = info.dli_sname;
        }
        names_out[i] = name;
    }
    return count;
}

#else
inline uint32_t native_capture_pcs(void **, uint32_t, uint32_t) noexcept { return 0; }
inline uint32_t native_resolve_symbols(void **, uint32_t, const char **) noexcept { return 0; }
#endif


inline uint32_t helix_snapshot_locs(const SrcLoc **dst, uint32_t max) noexcept {
    if (!dst || max == 0)
        return 0;
    uint32_t          n = 0;
    const HelixFrame *p = g_tls_helix_head;
    while (p && n < max) {
        dst[n++] = p->loc;
        p        = p->prev;
    }
    return n;
}

inline uint32_t merge_helix_and_native(const SrcLoc *const *helix_locs,
                                       uint32_t             hcount,
                                       const char *const   *native_names,
                                       uint32_t             ncount,
                                       MergedEntry         *out,
                                       uint32_t             max_out,
                                       bool                 emit_trailing_native = true) noexcept {
    if (!out || max_out == 0)
        return 0;

    uint32_t oi = 0;
    uint32_t ni = 0;



    auto same_func = [](const char *a, const char *b) noexcept -> bool {
        if (a == b)
            return a != nullptr;
        if (!a || !b)
            return false;

        while (*a && *b) {
            if (*a != *b)
                return false;
            ++a;
            ++b;
        }
        return *a == *b;
    };

    for (uint32_t hi = 0; hi < hcount; ++hi) {
        const SrcLoc *hl    = helix_locs[hi];
        const char   *hname = hl ? hl->func : nullptr;


        uint32_t consumed = 0;
        while (ni < ncount) {
            const char *nname = native_names[ni];
            if (hname && nname && same_func(hname, nname)) {

                break;
            }
            ++ni;
            ++consumed;
        }
        if (consumed > 0) {
            if (oi < max_out)
                out[oi++] = MergedEntry{nullptr, nullptr, 0U, FrameKind::Native};
        }


        if (oi < max_out) {
            out[oi].func = hname;
            out[oi].file = hl ? hl->file : nullptr;
            out[oi].line = hl ? hl->line : 0U;
            out[oi].kind = FrameKind::Helix;
            ++oi;
        }


        if (ni < ncount) {
            const char *nname = native_names[ni];
            if (hname && nname && same_func(hname, nname)) {
                ++ni;
            }
        }
    }


    if (emit_trailing_native && ni < ncount) {
        if (oi < max_out)
            out[oi++] = MergedEntry{
                .func = nullptr, .file = nullptr, .line = 0U, .kind = FrameKind::Native};
        while (ni < ncount && oi < max_out) {
            out[oi++] = MergedEntry{
                .func = native_names[ni], .file = nullptr, .line = 0U, .kind = FrameKind::Native};
            ++ni;
        }
    }

    return oi;
}

inline uint32_t helix_merge_backtrace_symbols(void         **pcs_buf,
                                              const char   **names_buf,
                                              const SrcLoc **helix_ptrs_buf,
                                              MergedEntry   *merged_out,
                                              uint32_t       max_frames,
                                              uint32_t       skip_native          = 0,
                                              bool           emit_trailing_native = true) noexcept {
    if (!pcs_buf || !names_buf || !helix_ptrs_buf || !merged_out || max_frames == 0)
        return 0;

    uint32_t ncount = native_capture_pcs(pcs_buf, max_frames, skip_native);
    native_resolve_symbols(pcs_buf, ncount, names_buf);

    uint32_t hcount = helix_snapshot_locs(helix_ptrs_buf, max_frames);

    return merge_helix_and_native(
        helix_ptrs_buf, hcount, names_buf, ncount, merged_out, max_frames, emit_trailing_native);
}

}

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif