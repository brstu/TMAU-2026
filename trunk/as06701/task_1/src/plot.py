import csv
import sys
import argparse
from collections import defaultdict

import matplotlib.pyplot as plt

INPUTS_ORDER = ["step", "impulse", "harmonic"]
# Вроде немного поменял, да комментов добавил, откуда все эти строки?
# Я ххотел сказать, везде подобавлял приколюх, а оно даже не сломалось, что за чудеса?


def key(name):
    """'Model 2.7' / '2.7' / '27' -> '27'.

    Извлекает все цифры из строки. Для наших имён моделей коллизий нет
    ('1.5' -> '15', '2.7' -> '27', '3.9' -> '39')."""
    return "".join(c for c in name if c.isdigit())


def parse_args():
    p = argparse.ArgumentParser(
        description="Plot discrete-model simulation results from CSV.")
    p.add_argument("--csv", default="output.csv",
                   help="input CSV (default: output.csv)")
    p.add_argument("--png", default="result.png",
                   help="output PNG (default: result.png)")
    p.add_argument("--plots", default=None,
                   help="comma-separated list 'model[:input]'; "
                        "model like 27 or 'Model 2.7'; "
                        "input like step/impulse/harmonic/all")
    p.add_argument("--overlay", action="store_true",
                   help="all selected pairs on one subplot")
    p.add_argument("--no-tables", action="store_true")
    p.add_argument("--no-show", action="store_true")
    return p.parse_args()


def load(path):
    """Грузат CSV в data[model][input] = [(step, u, y), ...].

    Так как я в файле с++ изменил формат выходных данных, где вместо тау теперь шаг, то видоизменил функцию."""
    data = defaultdict(lambda: defaultdict(list))
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        fields = reader.fieldnames or []
        # NEW: C++ теперь пишет 'step'; 'tau' оставлен для старых CSV.
        step_col = "step" if "step" in fields else "tau"
        for row in reader:
            data[row["model"]][row["input"]].append(
                (int(row[step_col]), float(row["u"]), float(row["y"])))
    for m in data:
        for i in data[m]:
            data[m][i].sort()
    return data


def resolve(data, raw, by_key):
    m_part, _, i_part = raw.partition(":")
    m_part = m_part.strip()
    i_part = i_part.strip().lower()

    model = by_key.get(key(m_part))
    if model is None:
        print(f"[warn] model '{m_part}' not in CSV", file=sys.stderr)
        return None

    # Добавлено: '27:all' (или '27:') — синоним "все входы этой модели".
    if not i_part or i_part == "all":
        inputs = [i for i in INPUTS_ORDER if i in data[model]]
    elif i_part in data[model]:
        inputs = [i_part]
    else:
        print(f"[warn] input '{i_part}' not found for {model}",
              file=sys.stderr)
        return None

    if not inputs:
        print(f"[warn] model '{model}' has no matching inputs",
              file=sys.stderr)
        return None

    return (model, inputs)


def build_spec(data, raw_plots):
    """
    '27:step,27:impulse' склеивается в одну запись
    ('Model 2.7', ['step', 'impulse']), чтобы получить ОДНУ панель
    с двумя кривыми, а не две панели по одной."""
    by_key = {key(m): m for m in data}
    merged = {}
    for raw in raw_plots.split(","):
        r = resolve(data, raw.strip(), by_key)
        if r is None:
            continue
        model, inputs = r
        merged.setdefault(model, [])
        for i in inputs:
            if i not in merged[model]:
                merged[model].append(i)
    return list(merged.items())


def print_table(model, inp, rows):
    print(f"\n=== {model} | {inp} ===")
    # заголовок 'step' (раньше был 'tau'), значение — 1-based.
    print(f"{'step':>5} | {'u':>10} | {'y':>12}")
    print("-" * 32)
    for t, u, y in rows:
        print(f"{t:>5} | {u:>10.4f} | {y:>12.4f}")


def build_panels(spec, overlay):
    """panel = [(label, model, input), ...]; возвращает список панелек.

    - overlay=True: Одна панель.
    - overlay=False: Панель на модель."""
    if overlay:
        return [[(f"{m}/{i}", m, i) for m, ins in spec for i in ins]]
    return [[(i, m, i) for i in ins] for m, ins in spec]


def main():
    a = parse_args()
    data = load(a.csv)
    if not data:
        sys.exit(f"No data in {a.csv}")

    if a.plots:
        spec = build_spec(data, a.plots)
    else:
        spec = [(m, [i for i in INPUTS_ORDER if i in data[m]])
                for m in sorted(data)]

    if not spec:
        sys.exit("[error] no plots matched")

    if not a.no_tables:
        for m, ins in spec:
            for i in ins:
                print_table(m, i, data[m][i])

    panels = build_panels(spec, a.overlay)
    n = len(panels)
    fig, axes = plt.subplots(1, n, figsize=(4.5 * n, 4), squeeze=False)
    for ax, panel in zip(axes[0], panels):
        for label, m, i in panel:
            rows = data[m][i]
            ax.plot([r[0] for r in rows], [r[2] for r in rows],
                    label=label, linewidth=1.2)
        ax.set_xlabel("step")   # Изменил: было 'tau'
        ax.set_ylabel("y")
        if a.overlay:
            ax.set_title("overlay: " + ", ".join(m for m, _ in spec))
        else:
            ax.set_title(panel[0][1])
        ax.grid(True, alpha=0.3)
        ax.legend(fontsize=8)

    plt.tight_layout()
    plt.savefig(a.png, dpi=120)
    print(f"\nSaved {a.png}")
    if not a.no_show:
        try:
            plt.show()
        except Exception as e:
            print(f"No window: {e}. See {a.png}")


if __name__ == "__main__":
    main()
