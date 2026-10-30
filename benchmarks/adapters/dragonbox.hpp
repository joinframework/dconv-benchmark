#pragma once

#include <string_view>

#include <dragonbox/dragonbox_to_chars.h>

struct DragonboxAdapter
{
    static constexpr std::string_view name = "dragonbox";

    static char* dtoa (char* buffer, double value)
    {
        return jkj::dragonbox::to_chars_n (value, buffer);
    }
};
