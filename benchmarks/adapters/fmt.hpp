#pragma once

#include <string_view>

#include <fmt/compile.h>
#include <fmt/format.h>

struct FmtAdapter
{
    static constexpr std::string_view name = "fmt";

    static char* dtoa (char* buffer, double value)
    {
        return fmt::format_to_n (buffer, 64, FMT_COMPILE ("{}"), value).out;
    }
};
