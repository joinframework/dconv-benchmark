#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <string_view>

#include <teju/double.h>

namespace teju_detail
{
    inline constexpr char digits2[] = "00010203040506070809101112131415161718192021222324252627282930313233343536373839"
                                      "40414243444546474849505152535455565758596061626364656667686970717273747576777879"
                                      "8081828384858687888990919293949596979899";

    inline char* writeScientific (char* buffer, uint64_t mantissa, int32_t exponent)
    {
        char digits[20];
        char* end = digits + sizeof (digits);
        char* cur = end;
        while (mantissa >= 100)
        {
            cur -= 2;
            std::memcpy (cur, digits2 + (mantissa % 100) * 2, 2);
            mantissa /= 100;
        }
        if (mantissa >= 10)
        {
            cur -= 2;
            std::memcpy (cur, digits2 + mantissa * 2, 2);
        }
        else
        {
            *--cur = static_cast<char> ('0' + mantissa);
        }

        int length = static_cast<int> (end - cur);
        while (length > 1 && cur[length - 1] == '0')
        {
            --length;
            ++exponent;
        }

        *buffer++ = cur[0];
        if (length > 1)
        {
            *buffer++ = '.';
            std::memcpy (buffer, cur + 1, length - 1);
            buffer += length - 1;
        }

        exponent += length - 1;
        *buffer++ = 'e';
        if (exponent < 0)
        {
            *buffer++ = '-';
            exponent = -exponent;
        }
        if (exponent >= 100)
        {
            *buffer++ = static_cast<char> ('0' + exponent / 100);
            exponent %= 100;
            std::memcpy (buffer, digits2 + exponent * 2, 2);
            return buffer + 2;
        }
        if (exponent >= 10)
        {
            std::memcpy (buffer, digits2 + exponent * 2, 2);
            return buffer + 2;
        }
        *buffer++ = static_cast<char> ('0' + exponent);
        return buffer;
    }
}

struct TejuAdapter
{
    static constexpr std::string_view name = "teju";

    static char* dtoa (char* buffer, double value)
    {
        if (std::signbit (value))
        {
            *buffer++ = '-';
            value = -value;
        }
        if (value == 0.0)
        {
            std::memcpy (buffer, "0e0", 3);
            return buffer + 3;
        }
        teju64_fields_t decimal = teju_double_to_decimal (value);
        return teju_detail::writeScientific (buffer, decimal.mantissa, decimal.exponent);
    }
};
