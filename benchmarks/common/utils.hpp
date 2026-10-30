#pragma once

#include <chrono>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

inline std::string load_file (const std::string& path)
{
    std::ifstream file (path, std::ios::binary);
    if (!file)
        throw std::runtime_error ("Cannot open: " + path);
    return {std::istreambuf_iterator<char> (file), {}};
}

inline std::uint64_t rdtsc () noexcept
{
    asm volatile ("" ::: "memory");
#if defined(__x86_64__) || defined(__i386__)
    return __builtin_ia32_rdtsc ();
#else
    std::uint64_t ticks;
    asm volatile ("mrs %0, cntvct_el0" : "=r"(ticks));
    return ticks;
#endif
}

namespace detail
{
    inline double calibrate_tsc () noexcept
    {
        using clk = std::chrono::steady_clock;
        auto t0 = clk::now ();
        std::uint64_t c0 = rdtsc ();
        auto deadline = t0 + std::chrono::milliseconds (20);
        while (clk::now () < deadline)
        {
        }
        std::uint64_t c1 = rdtsc ();
        auto t1 = clk::now ();
        double ns = static_cast<double> (std::chrono::duration_cast<std::chrono::nanoseconds> (t1 - t0).count ());
        return static_cast<double> (c1 - c0) / ns;
    }

    inline const double g_tsc_ghz = calibrate_tsc ();
}

inline double cycles_to_seconds (std::uint64_t cycles) noexcept
{
    return static_cast<double> (cycles) / detail::g_tsc_ghz * 1e-9;
}
