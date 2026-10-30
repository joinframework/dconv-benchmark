#include <charconv>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include "adapters/dconv.hpp"
#include "adapters/double_conversion.hpp"
#include "adapters/drachennest.hpp"
#include "adapters/dragonbox.hpp"
#include "adapters/fast_float.hpp"
#include "adapters/fmt.hpp"
#include "adapters/gay.hpp"
#include "adapters/libc.hpp"
#include "adapters/milo.hpp"
#include "adapters/ryu.hpp"
#include "adapters/std.hpp"
#include "adapters/teju.hpp"
#include "common/datasets.hpp"

struct Score
{
    size_t pass = 0;
    size_t total = 0;
};

struct Result
{
    std::string library;
    bool hasDtoa = false;
    bool hasAtod = false;
    Score roundtrip;
    Score shortest;
    Score exact;
};

template <typename Adapter>
concept HasDtoa = requires (char* buffer, double value) { Adapter::dtoa (buffer, value); };

template <typename Adapter>
concept HasAtod = requires (const char* text, double& value) { Adapter::atod (text, text, value); };

static bool sameBits (double a, double b)
{
    return std::memcmp (&a, &b, sizeof (double)) == 0;
}

static int significantDigits (std::string_view text)
{
    std::string digits;
    for (char c : text)
    {
        if (c == 'e' || c == 'E')
            break;
        if (c >= '0' && c <= '9')
            digits.push_back (c);
    }
    size_t first = digits.find_first_not_of ('0');
    if (first == std::string::npos)
        return 0;
    size_t last = digits.find_last_not_of ('0');
    return static_cast<int> (last - first + 1);
}

static std::vector<double> dtoaInputs ()
{
    std::vector<double> values = {0.0,
                                  -0.0,
                                  1.0,
                                  0.1,
                                  1e23,
                                  9007199254740993.0,
                                  5e-324,
                                  2.2250738585072009e-308,
                                  2.2250738585072014e-308,
                                  1.7976931348623157e308,
                                  123456789.125,
                                  1e21,
                                  1e-7};
    std::mt19937_64 rng (10);
    for (size_t i = 0; i < 1000000; ++i)
        values.push_back (randomFinite (rng));
    for (int d = 1; d <= 17; ++d)
    {
        Dataset data = makeDigits (d);
        values.insert (values.end (), data.values.begin (), data.values.end ());
    }
    return values;
}

static std::vector<std::string> atodInputs ()
{
    std::vector<std::string> texts = {"0",
                                      "-0",
                                      "1",
                                      "0.1",
                                      "1e23",
                                      "9007199254740993",
                                      "9007199254740993.0",
                                      "4.9406564584124654e-324",
                                      "2.2250738585072011e-308",
                                      "2.2250738585072012e-308",
                                      "1.7976931348623157e308",
                                      "8.98846567431158e307",
                                      "89255.0e-22",
                                      "1e126",
                                      "2e126",
                                      "57507.7647e38",
                                      "0.759154e41",
                                      "7.038531e-26",
                                      "9214843084008499",
                                      "30078505129381147446200",
                                      "1777820000000000000001",
                                      "0.500000000000000166533453693773481063544750213623046875",
                                      "3.518437208883201171875e13",
                                      "62.5364939768271845828",
                                      "8.10109172351e-10",
                                      "1.50000000000000011102230246251565404236316680908203125",
                                      "9007199254740991.4999999999999999999999999999999995"};
    std::mt19937_64 rng (20);
    char buffer[128];
    for (size_t i = 0; i < 1000000; ++i)
        texts.push_back (shortest (randomFinite (rng)));
    for (size_t i = 0; i < 1000000; ++i)
    {
        int length = std::snprintf (buffer, sizeof (buffer), "%.17g", randomFinite (rng));
        texts.emplace_back (buffer, length);
    }
    std::uniform_int_distribution<int> length (18, 40);
    std::uniform_int_distribution<int> digit (0, 9);
    std::uniform_int_distribution<int> exponent (-280, 260);
    for (size_t i = 0; i < 200000; ++i)
    {
        std::string text (1, static_cast<char> ('1' + digit (rng) % 9));
        text.push_back ('.');
        for (int n = length (rng); n > 1; --n)
            text.push_back (static_cast<char> ('0' + digit (rng)));
        text += "e" + std::to_string (exponent (rng));
        texts.push_back (std::move (text));
    }
    for (size_t i = 0; i < 100000; ++i)
    {
        uint64_t bits = rng () & ((uint64_t (1) << 52) - 1);
        double value;
        std::memcpy (&value, &bits, sizeof (value));
        texts.push_back (shortest (value));
    }
    return texts;
}

template <typename Adapter>
static Result check (const std::vector<double>& values, const std::vector<std::string>& texts)
{
    Result result;
    result.library = std::string (Adapter::name);

    if constexpr (HasDtoa<Adapter>)
    {
        result.hasDtoa = true;
        char buffer[64];
        for (double value : values)
        {
            char* end = Adapter::dtoa (buffer, value);
            std::string_view text (buffer, end - buffer);
            double parsed = 0.0;
            auto [ptr, ec] = std::from_chars (text.data (), text.data () + text.size (), parsed);
            ++result.roundtrip.total;
            if (ec == std::errc () && ptr == text.data () + text.size () && sameBits (parsed, value))
                ++result.roundtrip.pass;
            ++result.shortest.total;
            if (significantDigits (text) <= significantDigits (shortest (value)))
                ++result.shortest.pass;
        }
    }

    if constexpr (HasAtod<Adapter>)
    {
        result.hasAtod = true;
        for (const std::string& text : texts)
        {
            double expected = 0.0;
            auto [ptr, ec] = std::from_chars (text.data (), text.data () + text.size (), expected);
            if (ec != std::errc () || ptr != text.data () + text.size ())
                continue;
            double value = 0.0;
            ++result.exact.total;
            if (Adapter::atod (text.data (), text.data () + text.size (), value) && sameBits (value, expected))
                ++result.exact.pass;
        }
    }

    return result;
}

template <typename... Adapters>
static std::vector<Result> checkAll (const std::vector<double>& values, const std::vector<std::string>& texts)
{
    std::vector<Result> results;
    (results.push_back (check<Adapters> (values, texts)), ...);
    return results;
}

static void writeScore (std::ostream& out, const char* key, const Score& score, bool last)
{
    out << "      \"" << key << "\": {\"pass\": " << score.pass << ", \"total\": " << score.total << "}"
        << (last ? "\n" : ",\n");
}

int main (int argc, char** argv)
{
    std::string output;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string (argv[i]) == "--output" && i + 1 < argc)
            output = argv[++i];
    }

    const std::vector<double> values = dtoaInputs ();
    const std::vector<std::string> texts = atodInputs ();

    std::vector<Result> results =
        checkAll<DconvAdapter, DconvGrisuAdapter, DconvGrisu2Adapter, DconvGrisu3Adapter, DoubleConversionAdapter, DrachennestGrisu3Adapter, DrachennestSchubfachAdapter,
                 DragonboxAdapter, FastFloatAdapter, FmtAdapter, GayAdapter, LibcAdapter, MiloAdapter,
                 RyuAdapter, StdAdapter, TejuAdapter> (values, texts);

    auto percent = [] (const Score& score) {
        return score.total ? 100.0 * static_cast<double> (score.pass) / static_cast<double> (score.total) : 0.0;
    };

    std::cout << std::left << std::setw (24) << "library" << std::right << std::setw (14) << "roundtrip" << std::setw (14)
              << "shortest" << std::setw (14) << "atod exact" << "\n";
    for (const Result& r : results)
    {
        std::cout << std::left << std::setw (24) << r.library << std::right << std::fixed << std::setprecision (4);
        if (r.hasDtoa)
            std::cout << std::setw (13) << percent (r.roundtrip) << "%" << std::setw (13) << percent (r.shortest) << "%";
        else
            std::cout << std::setw (14) << "-" << std::setw (14) << "-";
        if (r.hasAtod)
            std::cout << std::setw (13) << percent (r.exact) << "%";
        else
            std::cout << std::setw (14) << "-";
        std::cout << "\n";
    }

    if (!output.empty ())
    {
        std::ofstream out (output);
        out << "{\n  \"results\": [\n";
        for (size_t i = 0; i < results.size (); ++i)
        {
            const Result& r = results[i];
            out << "    {\n      \"library\": \"" << r.library << "\",\n";
            if (r.hasDtoa)
            {
                writeScore (out, "roundtrip", r.roundtrip, false);
                writeScore (out, "shortest", r.shortest, !r.hasAtod);
            }
            if (r.hasAtod)
                writeScore (out, "exact", r.exact, true);
            out << "    }" << (i + 1 < results.size () ? ",\n" : "\n");
        }
        out << "  ]\n}\n";
    }

    return 0;
}
