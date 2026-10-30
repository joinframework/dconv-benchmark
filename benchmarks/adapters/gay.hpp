#pragma once

#include <cstring>
#include <string_view>

extern "C" char* g_fmt (char* buffer, double value);
extern "C" double gay_strtod (const char* str, char** end);

struct GayAdapter
{
    static constexpr std::string_view name = "gay";

    static char* dtoa (char* buffer, double value)
    {
        g_fmt (buffer, value);
        return buffer + std::strlen (buffer);
    }

    static bool atod (const char* first, const char* last, double& value)
    {
        char* end = nullptr;
        value = gay_strtod (first, &end);
        return end == last;
    }
};
