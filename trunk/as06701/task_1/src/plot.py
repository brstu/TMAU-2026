import csv, sys, argparse
from collections import defaultdict
import matplotlib.pyplot as plt

INPUTS_ORDER = ["step", "impulse", "harmonic"]


def key(name):
    """'Model 2.7' / '2.7' / '27' -> '27'"""
    return "".join(c for c in name if c.isdigit())


def parse_args():
    p = argparse.ArgumentParser()
    p.add_argument("--csv", default="output.csv")
    p.add_argument("--png", default="result.png")
    p.add_argument("--plots", default=None,
                   help="list 'model[:input]', model like 27 or 'Model 2.7'")
    p.add_argument("--overlay", action="store_true",
                   help="all selected pairs on one subplot")
    p.add_argument("--no-tables", action="store_true")
    p.add_argument("--no-show", action="store_true")
    return p.parse_args()


def load(path):
    data = defaultdict(lambda: defaultdict(list))
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            data[row["model"]][row["input"]].append(
                (int(row["tau"]), float(row["u"]), float(row["y"])))
    for m in data:
        for i in data[m]:
            data[m][i].sort()
    return data


def resolve(data, raw):
    """'Для пример: 2.7:harmonic' -> ('Model 2.7', ['harmonic'])"""
    m_part, _, i_part = raw.partition(":")
    by_key = {key(m): m for m in data}
    model = by_key.get(key(m_part))
    if model is None:
        print(f"[warn] model '{m_part}' not in CSV", file=sys.stderr)
        return None
    inputs = ([i_part.strip().lower()] if i_part
              else [i for i in INPUTS_ORDER if i in data[model]])
    if not all(i in data[model] for i in inputs):
        print(f"[warn] input not found for {model}: {i_part}", file=sys.stderr)
        return None
    return (model, inputs)


def print_table(model, inp, rows):
    print(f"\n=== {model} | {inp} ===")
    print(f"{'tau':>5} | {'u':>10} | {'y':>12}")
    print("-" * 32)
    for t, u, y in rows:
        print(f"{t:>5} | {u:>10.4f} | {y:>12.4f}")


def build_panels(spec, overlay):
    """panel = [(label, model, input), ...]; returns list of panels."""
    if overlay:
        return [[(f"{m}/{i}", m, i) for m, ins in spec for i in ins]]
    return [[(i, m, i) for i in ins] for m, ins in spec]

def resolve_spec(data, plots):
    """Вернуть [(model, [inputs...]), ...]. Без plots — все модели."""
    if plots:
        spec = [r for r in (resolve(data, s.strip()) for s in plots.split(",")) if r]
        if not spec:
            sys.exit("[error] no --plots matched")
        return spec
    return [(m, [i for i in INPUTS_ORDER if i in data[m]]) for m in sorted(data)]


def print_all_tables(spec, data):
    for m, ins in spec:
        for i in ins:
            print_table(m, i, data[m][i])


def draw(panels, data, overlay, png, show):
    n = len(panels)
    _, axes = plt.subplots(1, n, figsize=(4.5 * n, 4), squeeze=False)
    for ax, panel in zip(axes[0], panels):
        for label, m, i in panel:
            rows = data[m][i]
            ax.plot([r[0] for r in rows], [r[2] for r in rows],
                    label=label, linewidth=1.2)
        ax.set_xlabel("tau")
        ax.set_ylabel("y")
        ax.set_title("overlay" if overlay else panel[0][1])
        ax.grid(True, alpha=0.3)
        ax.legend(fontsize=8)
    plt.tight_layout()
    plt.savefig(png, dpi=120)
    print(f"\nSaved {png}")
    if show:
        try:
            plt.show()
        except Exception as e:
            print(f"No window: {e}. See {png}")


def main():
    a = parse_args()
    data = load(a.csv)
    if not data:
        sys.exit(f"No data in {a.csv}")
    spec = resolve_spec(data, a.plots)
    if not a.no_tables:
        print_all_tables(spec, data)
    panels = build_panels(spec, a.overlay)
    draw(panels, data, a.overlay, a.png, not a.no_show)


if __name__ == "__main__":
    main()
