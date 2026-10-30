#include <benchmark/benchmark.h>

#include ADAPTER_HEADER
#include "performance/datasets.hpp"

template <typename Adapter>
static void BM_Atod (benchmark::State& state, const Dataset* data)
{
    for (size_t i = 0; i < data->texts.size (); ++i)
    {
        std::string_view text = data->text (i);
        double value = 0.0;
        if (!Adapter::atod (text.data (), text.data () + text.size (), value))
        {
            state.SkipWithError ("Conversion failed");
            return;
        }
    }
    for (auto _ : state)
    {
        std::uint64_t start = rdtsc ();
        for (size_t i = 0; i < data->texts.size (); ++i)
        {
            const char* first = data->blob.data () + data->texts[i].first;
            double value = 0.0;
            Adapter::atod (first, first + data->texts[i].second, value);
            benchmark::DoNotOptimize (value);
        }
        state.SetIterationTime (cycles_to_seconds (rdtsc () - start));
    }
    const double count = static_cast<double> (data->texts.size ());
    state.SetItemsProcessed (state.iterations () * static_cast<int64_t> (count));
    state.counters["ns_per_value"] =
        benchmark::Counter (count * 1e-9, benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
    state.SetLabel (std::string (Adapter::name));
}

#define REGISTER_ATOD(Adapter, tag, data)                \
    BENCHMARK_CAPTURE (BM_Atod<Adapter>, tag, data)      \
        ->Name ("Atod/" #tag "/" #Adapter)               \
        ->Unit (benchmark::kMicrosecond)                 \
        ->UseManualTime ()

#define X(Adapter) REGISTER_DATASETS (REGISTER_ATOD, Adapter)

BENCH_ADAPTERS

BENCHMARK_MAIN ();
