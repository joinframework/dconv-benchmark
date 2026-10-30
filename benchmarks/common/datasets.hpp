#pragma once

#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include "common/utils.hpp"

struct Dataset
{
    std::vector<double> values;
    std::string blob;
    std::vector<std::pair<size_t, size_t>> texts;

    void add (double value, std::string_view text)
    {
        values.push_back (value);
        texts.emplace_back (blob.size (), text.size ());
        blob.append (text);
        blob.push_back ('\0');
    }

    std::string_view text (size_t i) const
    {
        return {blob.data () + texts[i].first, texts[i].second};
    }
};

inline constexpr size_t datasetSize = 1 << 16;

inline double randomFinite (std::mt19937_64& rng)
{
    for (;;)
    {
        uint64_t bits = rng ();
        double value;
        std::memcpy (&value, &bits, sizeof (value));
        if (std::isfinite (value))
        {
            return value;
        }
    }
}

inline std::string shortest (double value)
{
    char buffer[64];
    auto end = std::to_chars (buffer, buffer + sizeof (buffer), value).ptr;
    return {buffer, end};
}

inline std::string shortestScientific (double value)
{
    char buffer[64];
    auto end = std::to_chars (buffer, buffer + sizeof (buffer), value, std::chars_format::scientific).ptr;
    return {buffer, end};
}

inline Dataset makeRandom ()
{
    std::mt19937_64 rng (1);
    Dataset data;
    for (size_t i = 0; i < datasetSize; ++i)
    {
        double value = randomFinite (rng);
        data.add (value, shortestScientific (value));
    }
    return data;
}

inline Dataset makeUnit ()
{
    std::mt19937_64 rng (2);
    std::uniform_real_distribution<double> dist (0.0, 1.0);
    Dataset data;
    for (size_t i = 0; i < datasetSize; ++i)
    {
        double value = dist (rng);
        data.add (value, shortest (value));
    }
    return data;
}

inline Dataset makeInteger ()
{
    std::mt19937_64 rng (3);
    std::uniform_int_distribution<int64_t> dist (-(int64_t (1) << 53), int64_t (1) << 53);
    Dataset data;
    for (size_t i = 0; i < datasetSize; ++i)
    {
        double value = static_cast<double> (dist (rng));
        data.add (value, shortest (value));
    }
    return data;
}

inline Dataset makeDigits (int digits)
{
    std::mt19937_64 rng (100 + digits);
    Dataset data;
    while (data.values.size () < datasetSize)
    {
        char buffer[64];
        int length = std::snprintf (buffer, sizeof (buffer), "%.*e", digits - 1, randomFinite (rng));
        double value = 0.0;
        auto [ptr, ec] = std::from_chars (buffer, buffer + length, value);
        if (ec == std::errc () && ptr == buffer + length)
        {
            data.add (value, {buffer, static_cast<size_t> (length)});
        }
    }
    return data;
}

inline Dataset makeCanada (const std::string& path)
{
    const std::string json = load_file (path);
    Dataset data;
    const char* cur = json.data ();
    const char* end = cur + json.size ();
    while (cur < end)
    {
        if (*cur == '-' || (*cur >= '0' && *cur <= '9'))
        {
            const char* beg = cur;
            while (cur < end && std::strchr ("+-.0123456789eE", *cur) && *cur)
            {
                ++cur;
            }
            double value = 0.0;
            auto [ptr, ec] = std::from_chars (beg, cur, value);
            if (ec == std::errc () && ptr == cur)
            {
                data.add (value, {beg, static_cast<size_t> (cur - beg)});
            }
        }
        else
        {
            ++cur;
        }
    }
    return data;
}
