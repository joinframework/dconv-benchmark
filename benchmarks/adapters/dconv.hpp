#pragma once

#include <string_view>

#include <dconv/atod.hpp>
#include <dconv/dtoa.hpp>

struct DconvAdapter
{
    static constexpr std::string_view name = "dconv";

    static char* dtoa (char* buffer, double value)
    {
        return dconv::dtoa (buffer, value);
    }

    static bool atod (const char* first, const char* last, double& value)
    {
        return dconv::atod (first, last, value) == last;
    }
};

struct DconvGrisuAdapter
{
    static constexpr std::string_view name = "dconv/grisu";

    static char* dtoa (char* buffer, double value)
    {
        return dconv::grisu::dtoa (buffer, value);
    }
};

struct DconvGrisu3Adapter
{
    static constexpr std::string_view name = "dconv/grisu3";

    static char* dtoa (char* buffer, double value)
    {
        return dconv::grisu3::dtoa (buffer, value);
    }
};

struct DconvGrisu2Adapter
{
    static constexpr std::string_view name = "dconv/grisu2";

    static char* dtoa (char* buffer, double value)
    {
        return dconv::grisu2::dtoa (buffer, value);
    }
};
