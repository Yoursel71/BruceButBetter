#!/usr/bin/env python3
"""Plot a spectrum CSV captured on the device.

Handles both capture formats:
  - sub-GHz  (RF -> Spectrum -> CSV):    freq_mhz,rssi_dbm
  - 2.4 GHz  (NRF24 -> Spectrum -> CSV): channel,freq_mhz,activity

Usage:
    python plot_spectrum.py /path/to/spectrum_12345.csv
    python plot_spectrum.py nrf_spectrum_678.csv -o band.png

Needs matplotlib (pip install matplotlib).
"""
import argparse
import csv
import sys


def load(path):
    with open(path, newline="") as fh:
        rows = list(csv.reader(fh))
    if not rows:
        sys.exit("empty file")
    header = [c.strip().lower() for c in rows[0]]
    data = [r for r in rows[1:] if r]
    if "rssi_dbm" in header:  # sub-GHz RSSI sweep
        fi, vi = header.index("freq_mhz"), header.index("rssi_dbm")
        x = [float(r[fi]) for r in data]
        y = [float(r[vi]) for r in data]
        return x, y, "Frequency (MHz)", "RSSI (dBm)", "Sub-GHz RSSI sweep"
    if "activity" in header:  # 2.4 GHz channel activity
        fi, vi = header.index("freq_mhz"), header.index("activity")
        x = [float(r[fi]) for r in data]
        y = [float(r[vi]) for r in data]
        return x, y, "Frequency (MHz)", "Activity", "2.4 GHz channel activity"
    sys.exit(f"unrecognised header: {header}")


def main():
    ap = argparse.ArgumentParser(description="Plot a BruceButBetter spectrum CSV.")
    ap.add_argument("csv", help="captured CSV file")
    ap.add_argument("-o", "--out", help="save to PNG instead of showing a window")
    args = ap.parse_args()

    try:
        import matplotlib

        if args.out:
            matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        sys.exit("matplotlib not installed -> pip install matplotlib")

    x, y, xlabel, ylabel, title = load(args.csv)
    peak = max(range(len(y)), key=lambda i: y[i])

    plt.figure(figsize=(11, 4))
    plt.fill_between(x, y, min(y), color="#2ee6d6", alpha=0.35)
    plt.plot(x, y, color="#1f6feb", linewidth=1)
    plt.scatter([x[peak]], [y[peak]], color="#ff5c5c", zorder=5,
                label=f"peak {x[peak]:.3f} MHz / {y[peak]:g}")
    plt.title(f"{title} - {args.csv}")
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend(loc="upper right")
    plt.grid(alpha=0.25)
    plt.tight_layout()

    if args.out:
        plt.savefig(args.out, dpi=130)
        print(f"saved {args.out}")
    else:
        plt.show()


if __name__ == "__main__":
    main()
