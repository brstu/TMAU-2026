"""Визуализация результатов: python plot.py [каталог_с_csv] [--show]"""
import argparse
import csv
import re
import sys
from pathlib import Path

import matplotlib

CSV_NAME = re.compile(r"^model_\d+\.\d+_(step|impulse|sine)\.csv$")


def parse_args():
    parser = argparse.ArgumentParser(description="Графики по csv-файлам программы sim")
    parser.add_argument("folder", nargs="?", default="output",
                        help="каталог с csv внутри текущей папки (по умолчанию: output)")
    parser.add_argument("--show", action="store_true", help="показывать графики на экране")
    return parser.parse_args()


def safe_folder(name):
    """Каталог должен существовать и находиться внутри текущей рабочей папки."""
    base = Path.cwd().resolve()
    folder = (base / name).resolve()
    try:
        folder.relative_to(base)
    except ValueError:
        sys.exit(f"Каталог '{name}' должен находиться внутри {base}")
    if not folder.is_dir():
        sys.exit(f"Каталог '{folder}' не найден. Сначала запустите sim.exe")
    return folder


def read_csv(path):
    tau, u, y = [], [], []
    with path.open(newline="", encoding="utf-8") as fh:
        for row in csv.DictReader(fh):
            tau.append(int(row["tau"]))
            u.append(float(row["u"]))
            y.append(float(row["y"]))
    return tau, u, y


def main():
    args = parse_args()
    if not args.show:
        matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib.ticker import MaxNLocator

    folder = safe_folder(args.folder)
    files = sorted(p for p in folder.glob("model_*.csv") if CSV_NAME.match(p.name))
    if not files:
        sys.exit(f"Нет csv-файлов в '{folder}'. Сначала запустите sim.exe")
    plots = folder / "plots"
    plots.mkdir(exist_ok=True)

    for path in files:
        tau, u, y = read_csv(path)
        fig, ax = plt.subplots(figsize=(8, 4))
        ax.step(tau, u, where="post", color="tab:orange", label="u(τ)")
        ax.plot(tau[:len(y)], y, "o-", ms=3, color="tab:blue", label="y(τ)")
        ax.set_title(path.stem)
        ax.set_xlabel("τ")
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.grid(alpha=.3)
        ax.legend()
        fig.tight_layout()
        fig.savefig(plots / (path.stem + ".png"), dpi=120)
        if args.show:
            plt.show()
        plt.close(fig)
    print(f"Готово: {len(files)} графиков в {plots}")


if __name__ == "__main__":
    main()