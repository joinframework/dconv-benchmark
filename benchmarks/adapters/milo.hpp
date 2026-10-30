#pragma once

#include <cassert>
#include <cmath>
#include <cstring>
#include <string_view>

#include <dtoa_milo.h>

struct MiloAdapter
{
    static constexpr std::string_view name = "milo";

    static char* dtoa (char* buffer, double value)
    {
        dtoa_milo (value, buffer);
        return buffer + std::strlen (buffer);
    }
};
