#pragma once

#include <limits>
#include <string_view>

#include <double-conversion/double-conversion.h>

struct DoubleConversionAdapter
{
    static constexpr std::string_view name = "double-conversion";

    static char* dtoa (char* buffer, double value)
    {
        double_conversion::StringBuilder builder (buffer, 64);
        double_conversion::DoubleToStringConverter::EcmaScriptConverter ().ToShortest (value, &builder);
        return buffer + builder.position ();
    }

    static bool atod (const char* first, const char* last, double& value)
    {
        static const double_conversion::StringToDoubleConverter converter (
            double_conversion::StringToDoubleConverter::NO_FLAGS, 0.0, std::numeric_limits<double>::quiet_NaN (),
            "Infinity", "NaN");
        int processed = 0;
        value = converter.StringToDouble (first, static_cast<int> (last - first), &processed);
        return processed == last - first;
    }
};
