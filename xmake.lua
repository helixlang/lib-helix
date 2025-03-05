-- Project setup
set_project("core")
set_version("0.0.1-alpha+5932a", {soname = false}) -- No SONAME since it's static
set_description("Helix Core Lib")
add_rules("mode.debug", "mode.release")

-- ABI and runtime detection functions (simplified for static linking)
local function get_abi()
    if os.host() == "windows" then
        return "msvc"
    elseif os.host() == "linux" then
        return "gnu"
    elseif os.host() == "macosx" then
        return "llvm"
    end
    return "unknown"
end

local function get_runtime(abi)
    if abi == "msvc" then
        return "MT" -- Static runtime for MSVC
    else
        return "static" -- Attempt static C++ runtime for GCC/Clang
    end
end

-- Build configuration functions
local function setup_debug()
    set_symbols("debug") -- Include debug symbols
    set_optimize("none") -- No optimization
    add_defines("DEBUG", "H_DEBUG_M") -- Define DEBUG and H_DEBUG_M
    set_warnings("all", "extra")
end

local function setup_release()
    set_symbols("hidden") -- Minimize symbol visibility
    set_optimize("aggressive") -- Max optimization
    add_defines("NDEBUG") -- Define NDEBUG
    set_warnings("none")
end

local function setup_build_folder(abi)
    set_targetdir("$(buildir)/$(mode)/$(arch)-" .. abi .. "-$(os)/lib")
end

local function setup_env(abi, runtime)
    -- Platform-specific setup
    if os.host() == "windows" then
        add_cxflags("/MT") -- Static runtime for MSVC
    else
        add_ldflags("-static-libstdc++") -- Static libstdc++ for GCC/Clang
        -- Note: -static for full static linking (incl. libc) may not work universally
    end

    -- Debug/Release setup
    if is_mode("debug") then
        setup_debug()
    else
        setup_release()
    end

    set_policy("build.across_targets_in_parallel", true)
    set_runtimes(runtime)
end

-- Source and header setup
local function core_setup()
    add_includedirs("core")
    add_files("core/lib/*.cc")
    add_headerfiles("core/**.h")
    add_headerfiles("core/**.hh")
end

-- Determine ABI and runtime
local abi = get_abi()
local runtime = get_runtime(abi)

-- Setup environment and build folder
setup_build_folder(abi)
setup_env(abi, runtime)
core_setup()

-- Main library target
target("-helix")
    set_kind("static") -- Static library
    set_prefixname("lib") -- Outputs libhelixcore.a

    -- Build info (optional, aligned with your compiler's style)
    before_build(function(target)
        print("\nBuilding Helix Core Library:")
        print("  Mode: " .. (is_mode("debug") and "debug" or "release"))
        print("  OS: " .. os.host())
        print("  ABI: " .. abi)
        print("  Runtime: " .. runtime)
        print("  Output: " .. target:targetfile() .. "\n")
    end)