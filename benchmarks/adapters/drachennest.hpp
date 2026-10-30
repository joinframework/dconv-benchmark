#pragma once

#include <string_view>

#include <grisu3.h>
#include <schubfach_64.h>

struct DrachennestGrisu3Adapter
{
    static constexpr std::string_view name = "drachennest/grisu3";

    static char* dtoa (char* buffer, double value)
    {
        return grisu3::Dtoa (buffer, value);
    }
};

struct DrachennestSchubfachAdapter
{
    static constexpr std::string_view name = "drachennest/schubfach";

    static char* dtoa (char* buffer, double value)
    {
        return schubfach::Dtoa (buffer, value);
    }
};
