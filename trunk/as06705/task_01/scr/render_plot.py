import sys
import os
import csv


def safe_filename(name):
    if not name:
        return ""
    if name in (".", ".."):
        return ""
    if name.startswith("."):
        return ""
    if "/" in name or "\\" in name:
        return ""
    for ch in name:
        if not (ch.isalnum() or ch in "._-"):
            return ""
    if not name.lower().endswith(".csv"):
        return ""
    return name


def resolve_in_cwd(fname):
    cwd = os.path.realpath(os.getcwd())
    full = os.path.realpath(os.path.join(cwd, fname))
    if full != cwd and not full.startswith(cwd + os.sep):
        return ""
    return full


def read_series(full_csv):
    tau, u, y = [], [], []
    with open(full_csv, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            tau.append(int(row["tau"]))
            u.append(float(row["u"]))
            y.append(float(row["y"]))
    return tau, u, y


def save_plot(tau, u, y, title, out_png):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    _, ax = plt.subplots(figsize=(10, 5))
    ax.plot(tau, y, "b-o", markersize=3, linewidth=1.5, label="y(tau)")
    ax.set_xlabel("tau")
    ax.set_ylabel("y", color="b")
    ax.tick_params(axis="y", labelcolor="b")
    ax.grid(True, alpha=0.3)
    ax.set_title(title)

    ax2 = ax.twinx()
    ax2.plot(tau, u, "r--", linewidth=1.2, label="u(tau)")
    ax2.set_ylabel("u", color="r")
    ax2.tick_params(axis="y", labelcolor="r")

    plt.tight_layout()

    try:
        plt.savefig(out_png, dpi=150)
        plt.close()
        return out_png
    except PermissionError:
        stem, ext = os.path.splitext(out_png)
        for i in range(1, 100):
            candidate = f"{stem}_{i}{ext}"
            try:
                plt.savefig(candidate, dpi=150)
                plt.close()
                return candidate
            except PermissionError:
                continue
        plt.close()
        return ""


def main(argv):
    if len(argv) < 2:
        print("Usage: python render_plot.py <file.csv>")
        return 1

    raw = argv[1]
    fname = safe_filename(raw)
    if not fname:
        print(f"Invalid filename: {raw!r}")
        print("Expected a plain *.csv file name in the current directory.")
        return 1

    full_csv = resolve_in_cwd(fname)
    if not full_csv:
        print(f"Path escapes working directory: {raw!r}")
        return 1

    if not os.path.exists(full_csv):
        print(f"File not found: {fname}")
        print(f"cwd = {os.path.realpath(os.getcwd())}")
        return 1

    try:
        tau, u, y = read_series(full_csv)
    except (KeyError, ValueError) as exc:
        print(f"Invalid CSV format: {exc}")
        return 1

    try:
        out_png = save_plot(tau, u, y, fname, fname.replace(".csv", ".png"))
    except ImportError:
        print("matplotlib not installed. Run: py -3 -m pip install matplotlib")
        return 1

    if not out_png:
        print(f"Cannot save PNG next to {fname.replace('.csv', '.png')}")
        return 1

    abs_png = os.path.abspath(out_png)
    print(f"Saved: {abs_png}")

    if not os.path.exists(abs_png):
        print("ERROR: file was not created")
        return 1

    return 0


if __name__ == "__main__":  # pragma: no cover
    sys.exit(main(sys.argv))