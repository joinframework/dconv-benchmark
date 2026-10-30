#pragma once

#include <string_view>

#include <ryu/ryu.h>
#include <ryu/ryu_parse.h>

struct RyuAdapter
{
    static constexpr std::string_view name = "ryu";

    static char* dtoa (char* buffer, double value)
    {
        return buffer + d2s_buffered_n (value, buffer);
    }

    static bool atod (const char* first, const char* last, double& value)
    {
        return s2d_n (first, static_cast<int> (last - first), &value) == SUCCESS;
    }
};
