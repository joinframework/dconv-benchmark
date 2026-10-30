#!/usr/bin/env python3

import json
import math
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.colors import to_rgb


LIBRARY_ORDER = [
    "dconv",
    "dconv/grisu",
    "dconv/grisu2",
    "dconv/grisu3",
    "double-conversion",
    "drachennest/grisu3",
    "drachennest/schubfach",
    "dragonbox",
    "fast_float",
    "fmt",
    "gay",
    "libc",
    "milo",
    "ryu",
    "std",
    "teju",
]

DATASET_ORDER = ["Canada", "Random", "Unit", "Integer"]

_PALETTE = [plt.get_cmap("tab20")(i) for i in (0, 2, 4, 6, 8, 10, 12, 16, 18, 1, 3, 5, 9, 11, 13, 17, 19)]
_LIB_COLORS: dict[str, tuple] = {}

def _lib_color(name: str) -> tuple:
    if name not in _LIB_COLORS:
        family   = name.split("/")[0]
        families = list(dict.fromkeys(lib.split("/")[0] for lib in LIBRARY_ORDER))
        idx      = families.index(family) if family in families else len(families) + len(_LIB_COLORS)
        variants = sorted((lib for lib in LIBRARY_ORDER if lib.split("/")[0] == family), key=lambda lib: lib != family)
        rank     = variants.index(name) if name in variants else 0
        r, g, b  = to_rgb(_PALETTE[idx % len(_PALETTE)])
        t        = 0.55 * rank / max(len(variants), 1)
        _LIB_COLORS[name] = (r + (1 - r) * t, g + (1 - g) * t, b + (1 - b) * t)
    return _LIB_COLORS[name]

def load_results(paths: list[str]) -> pd.DataFrame:
    rows = []
    for path in paths:
        with open(path) as f:
            data = json.load(f)
        for b in data.get("benchmarks", []):
            if b.get("run_type") != "iteration" or b.get("error_occurred"):
                continue
            parts = b["name"].split("/")
            if len(parts) < 3:
                continue
            rows.append({
                "type":    parts[0],
                "dataset": parts[1],
                "library": b.get("label", parts[2]),
                "run":     path,
                "ns":      b.get("ns_per_value", 0),
            })
    return pd.DataFrame(rows)

def _best(df: pd.DataFrame) -> pd.DataFrame:
    return df.groupby(["library", "dataset"])["ns"].min().reset_index()

def _summarise(best: pd.DataFrame) -> pd.Series:
    return best.groupby("library")["ns"].mean().sort_index(ascending=False)

def _style_ax(ax: plt.Axes, axis: str = "x") -> None:
    ax.grid(axis=axis, alpha=0.3, linestyle="--")
    ax.set_axisbelow(True)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)

def _barh_ax(ax: plt.Axes, summary: pd.Series, title: str) -> None:
    libs   = summary.index.tolist()
    values = summary.tolist()
    colors = [_lib_color(lib) for lib in libs]
    bars = ax.barh(libs, values, color=colors, alpha=0.87, height=0.65, edgecolor="white")
    ax.bar_label(bars, labels=[f"{v:.3g}" if v < 1000 else f"{v:.0f}" for v in values], padding=4, fontsize=8)
    ax.set_title(title, fontsize=11, fontweight="bold", loc="left", pad=6)
    ax.set_xlabel("Time per value (ns)")
    ax.set_xlim(0, summary.max() * 1.18 if not summary.empty else 1)
    _style_ax(ax)

def _plot_datasets(df: pd.DataFrame, output_dir: Path, bench_type: str, filename: str, title: str) -> None:
    sub_df = df[(df["type"] == bench_type) & (df["dataset"].isin(DATASET_ORDER))]
    if sub_df.empty:
        return

    best     = _best(sub_df)
    datasets = [d for d in DATASET_ORDER if d in best["dataset"].unique()]
    n_subs   = 1 + len(datasets)
    n_libs   = best["library"].nunique()
    row_h    = max(3.5, n_libs * 0.38 + 0.8)

    fig, axes = plt.subplots(n_subs, 1, figsize=(10, row_h * n_subs))
    fig.suptitle(title, fontsize=14, fontweight="bold")

    _barh_ax(axes[0], _summarise(best), "Overall (mean of datasets)")
    for idx, dataset in enumerate(datasets, start=1):
        _barh_ax(axes[idx], _summarise(best[best["dataset"] == dataset]), dataset)

    fig.tight_layout(rect=[0, 0, 1, 0.97])
    out = output_dir / filename
    fig.savefig(out, dpi=150, bbox_inches="tight")
    print(f"  Saved: {out}")
    plt.close(fig)

def _plot_digits(df: pd.DataFrame, output_dir: Path, bench_type: str, filename: str, title: str) -> None:
    sub_df = df[(df["type"] == bench_type) & (df["dataset"].str.startswith("Digits"))]
    if sub_df.empty:
        return

    best = _best(sub_df)
    best["digits"] = best["dataset"].str.removeprefix("Digits").astype(int)

    fig, ax = plt.subplots(figsize=(10, 6))
    fig.suptitle(title, fontsize=14, fontweight="bold")
    libs = [lib for lib in LIBRARY_ORDER if lib in best["library"].unique()]
    libs += sorted(set(best["library"]) - set(libs))
    for lib in libs:
        series = best[best["library"] == lib].sort_values("digits")
        ax.plot(series["digits"], series["ns"], marker="o", markersize=3, linewidth=1.5, label=lib, color=_lib_color(lib))

    ax.set_yscale("log")
    ax.set_xticks(range(1, 18))
    ax.set_xlabel("Significant digits")
    ax.set_ylabel("Time per value (ns, log scale)")
    ax.legend(fontsize=8, ncol=2, frameon=False)
    _style_ax(ax, "both")

    fig.tight_layout(rect=[0, 0, 1, 0.95])
    out = output_dir / filename
    fig.savefig(out, dpi=150, bbox_inches="tight")
    print(f"  Saved: {out}")
    plt.close(fig)

_CONF_CATEGORIES = ["roundtrip", "shortest", "exact"]
_CONF_CAT_LABELS = ["Dtoa Roundtrip", "Dtoa Shortest", "Atod Exact"]


def plot_conformance(conf_path: str, output_dir: Path) -> None:
    with open(conf_path) as f:
        data = json.load(f)
    results = data.get("results", [])
    if not results:
        return

    n_libs = len(results)
    row_h  = max(3.0, n_libs * 0.38 + 0.7)

    fig, axes = plt.subplots(len(_CONF_CATEGORIES), 1, figsize=(10, row_h * len(_CONF_CATEGORIES)))
    fig.suptitle("Double Conversion Benchmark — Conformance", fontsize=14, fontweight="bold")

    for ax, c, label in zip(axes, _CONF_CATEGORIES, _CONF_CAT_LABELS):
        rows   = sorted((r for r in results if c in r and r[c]["total"] > 0), key=lambda r: r["library"], reverse=True)
        libs   = [r["library"] for r in rows]
        vals   = [r[c]["pass"] / r[c]["total"] * 100 for r in rows]
        colors = [_lib_color(lib) for lib in libs]

        ax.barh(libs, vals, color=colors, alpha=0.87, height=0.65, edgecolor="white")
        for i, r in enumerate(rows):
            failed = r[c]["total"] - r[c]["pass"]
            text = "100%" if failed == 0 else f"{math.floor(vals[i] * 1e4) / 1e4:.4f}% ({failed} failed)"
            ax.text(min(vals[i] + 0.8, 101), i, text, va="center", fontsize=8)

        ax.set_xlabel("%")
        ax.set_xlim(0, 115)
        ax.set_title(label, fontsize=11, fontweight="bold", loc="left", pad=6)
        _style_ax(ax)

    fig.tight_layout(rect=[0, 0, 1, 0.97])
    out = output_dir / "conformance.png"
    fig.savefig(out, dpi=150, bbox_inches="tight")
    print(f"  Saved: {out}")
    plt.close(fig)

def _is_conformance(path: str) -> bool:
    try:
        with open(path) as f:
            data = json.load(f)
        return "results" in data and "benchmarks" not in data
    except Exception:
        return False


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(1)

    output_dir = Path(__file__).parent
    print(f"Generating plots in: {output_dir}")

    bench_paths = [p for p in sys.argv[1:] if not _is_conformance(p)]
    conf_paths  = [p for p in sys.argv[1:] if _is_conformance(p)]

    if bench_paths:
        df = load_results(bench_paths)
        if not df.empty:
            _plot_datasets(df, output_dir, "Dtoa", "performance_dtoa.png", "Double Conversion Benchmark — Double to String")
            _plot_datasets(df, output_dir, "Atod", "performance_atod.png", "Double Conversion Benchmark — String to Double")
            _plot_digits(df, output_dir, "Dtoa", "digits_dtoa.png", "Double to String — Time by Significant Digits")
            _plot_digits(df, output_dir, "Atod", "digits_atod.png", "String to Double — Time by Significant Digits")

    for conf_path in conf_paths:
        plot_conformance(conf_path, output_dir)

    print("Done.")


if __name__ == "__main__":
    main()
