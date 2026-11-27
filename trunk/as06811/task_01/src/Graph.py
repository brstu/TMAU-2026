import csv
import shutil
import sys
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

MODELS = {"Model 1.10": "Модель 1.10", "Model 2.2": "Модель 2.2", "Model 3.4": "Модель 3.4"}
SIGNALS = {"Step": "ступенчатое", "Impulse": "импульсное", "Harmonic": "гармоническое"}


def main():
    script_dir = Path(__file__).resolve().parent
    csv_path = script_dir / (sys.argv[1] if len(sys.argv) > 1 else "result.csv")

    if not csv_path.exists():
        print(f"Ошибка: файл {csv_path} не найден.")
        return 1

    with csv_path.open(newline="", encoding="utf-8-sig") as file:
        rows = list(csv.DictReader(file))
    if not rows:
        print("Ошибка: CSV-файл пуст.")
        return 1

    steps = [int(r["step"]) for r in rows]
    inputs = [float(r["u_tau"]) for r in rows]
    outputs = [float(r["y_next"]) for r in rows]
    model, signal = rows[0]["model"], rows[0]["signal"]
    model_title = MODELS.get(model, model)
    signal_title = SIGNALS.get(signal, signal)

    print(f"\nРезультаты: {model_title}, воздействие: {signal_title}\n")
    print(f"{'Шаг':>8} {'u(tau)':>14} {'y(tau+1)':>14}")
    print("-" * 38)
    for step, u, y in zip(steps, inputs, outputs):
        print(f"{step:8d} {u:14.4f} {y:14.4f}")

    fig, (top, bottom) = plt.subplots(2, 1, figsize=(10, 7), sharex=True)
    top.plot(steps, inputs, marker="o")
    top.set_title(f"Входное воздействие: {signal_title}")
    top.set_ylabel("u(tau)")
    top.grid(True)
    bottom.plot(steps, outputs, marker="o")
    bottom.set_title(f"Результат моделирования: {model_title}")
    bottom.set_xlabel("Номер шага")
    bottom.set_ylabel("y(tau+1)")
    bottom.grid(True)
    bottom.xaxis.set_major_locator(MaxNLocator(integer=True))
    fig.tight_layout()

    # график и копия таблицы сразу в папку doc
    doc_dir = script_dir.parent / "doc"
    doc_dir.mkdir(exist_ok=True)
    name = f"{model.replace(' ', '_')}_{signal}"
    fig.savefig(doc_dir / f"{name}.png", dpi=150, bbox_inches="tight")
    shutil.copyfile(csv_path, doc_dir / f"{name}.csv")
    print(f"\nГрафик и таблица сохранены в {doc_dir}: {name}.png, {name}.csv")

    plt.show()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())