#include <benchmark/benchmark.h>

#include ADAPTER_HEADER
#include "performance/datasets.hpp"

template <typename Adapter>
static void BM_Dtoa (benchmark::State& state, const Dataset* data)
{
    char buffer[64];
    for (auto _ : state)
    {
        std::uint64_t start = rdtsc ();
        for (double value : data->values)
        {
            char* end = Adapter::dtoa (buffer, value);
            benchmark::DoNotOptimize (end);
        }
        state.SetIterationTime (cycles_to_seconds (rdtsc () - start));
    }
    const double count = static_cast<double> (data->values.size ());
    state.SetItemsProcessed (state.iterations () * static_cast<int64_t> (count));
    state.counters["ns_per_value"] =
        benchmark::Counter (count * 1e-9, benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
    state.SetLabel (std::string (Adapter::name));
}

#define REGISTER_DTOA(Adapter, tag, data)                \
    BENCHMARK_CAPTURE (BM_Dtoa<Adapter>, tag, data)      \
        ->Name ("Dtoa/" #tag "/" #Adapter)               \
        ->Unit (benchmark::kMicrosecond)                 \
        ->UseManualTime ()

#define X(Adapter) REGISTER_DATASETS (REGISTER_DTOA, Adapter)

BENCH_ADAPTERS

BENCHMARK_MAIN ();
