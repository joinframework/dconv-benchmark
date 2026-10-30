#pragma once

#include <string_view>
#include <system_error>

#include <fast_float/fast_float.h>

struct FastFloatAdapter
{
    static constexpr std::string_view name = "fast_float";

    static bool atod (const char* first, const char* last, double& value)
    {
        auto [ptr, ec] = fast_float::from_chars (first, last, value);
        return ec == std::errc () && ptr == last;
    }
};
