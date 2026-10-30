#pragma once

#include <charconv>
#include <string_view>
#include <system_error>

struct StdAdapter
{
    static constexpr std::string_view name = "std";

    static char* dtoa (char* buffer, double value)
    {
        return std::to_chars (buffer, buffer + 64, value).ptr;
    }

    static bool atod (const char* first, const char* last, double& value)
    {
        auto [ptr, ec] = std::from_chars (first, last, value);
        return ec == std::errc () && ptr == last;
    }
};
