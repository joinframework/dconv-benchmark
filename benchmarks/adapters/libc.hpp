#pragma once

#include <cstdio>
#include <cstdlib>
#include <string_view>

struct LibcAdapter
{
    static constexpr std::string_view name = "libc";

    static char* dtoa (char* buffer, double value)
    {
        return buffer + std::snprintf (buffer, 64, "%.17g", value);
    }

    static bool atod (const char* first, const char* last, double& value)
    {
        char* end = nullptr;
        value = std::strtod (first, &end);
        return end == last;
    }
};
