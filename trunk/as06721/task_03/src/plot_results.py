"""Plot a CSV trajectory or regenerate the figures in the lab report."""

import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def read_trace(path):
    with path.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    if not rows:
        raise ValueError(f"No samples in {path}")
    return {
        field: [float(row[field]) for row in rows]
        for field in ("time", "target", "output", "error", "command")
    }


def style(axis):
    axis.grid(True, color="#e1e8ef", linewidth=0.8)
    axis.spines["top"].set_visible(False)
    axis.spines["right"].set_visible(False)
    axis.set_xlabel("Время, с")


def plot_trajectories(title, traces, destination, band=None):
    fig, axes = plt.subplots(len(traces), 2, figsize=(10.8, 3.1 * len(traces) + 0.7),
                             squeeze=False, layout="constrained")
    fig.suptitle(title, fontsize=16, fontweight="bold", color="#142c48")
    for trace, (output, command) in zip(traces, axes):
        time = trace["time"]
        target = trace["target"][0]
        output.plot(time, trace["output"], color="#17659a", linewidth=2, label="Выход")
        output.axhline(target, color="#7b4352", linestyle="--", label="Уставка")
        if band is not None:
            output.axhspan(target - band, target + band, color="#7b4352", alpha=0.12)
        output.set_title(f"Уставка {target:g} °C")
        output.set_ylabel("Температура, °C")
        output.legend(loc="lower right")

        command.step(time, trace["command"], where="pre", color="#087a5d", linewidth=1.75)
        command.set_title("Команда исполнительному устройству")
        command.set_ylabel("Команда u")
        style(output)
        style(command)
    destination.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(destination, dpi=150, facecolor="white")
    plt.close(fig)


def plot_tuning(source, destination):
    fig, (output, error) = plt.subplots(1, 2, figsize=(10.8, 4.3), layout="constrained")
    fig.suptitle("Модель 1.2 · последовательная настройка на 60 °C",
                 fontsize=16, fontweight="bold", color="#142c48")
    trials = (
        ("p", "П", "#933e62"),
        ("pi", "ПИ", "#087a5d"),
        ("pid", "ПИД", "#17659a"),
    )
    for key, label, color in trials:
        trace = read_trace(source / f"tuning_{key}.csv")
        output.plot(trace["time"], trace["output"], label=label, color=color, linewidth=2)
        error.semilogy(trace["time"], [max(abs(value), 1e-12) for value in trace["error"]],
                       label=label, color=color, linewidth=2)
    output.axhline(60, color="#6c7888", linestyle="--", label="Уставка")
    error.axhline(0.2, color="#6c7888", linestyle="--", label="Допуск")
    output.set_ylabel("Температура, °C")
    error.set_ylabel("Абсолютная ошибка, °C")
    for axis in (output, error):
        style(axis)
        axis.legend()
    destination.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(destination, dpi=150, facecolor="white")
    plt.close(fig)


def plot_modeb(source, destination):
    fig, axes = plt.subplots(3, 1, figsize=(10.8, 9), layout="constrained")
    fig.suptitle("Режим Б · остановка при |ошибка| ≤ 0,2 °C", fontsize=16)
    for axis, key in zip(axes, ("model12", "model27", "model35")):
        for target in (40, 60):
            trace = read_trace(source / f"modeb_{key}_{target}.csv")
            errors = [abs(e) for e in trace["error"]]
            reached = errors[-1] <= 0.2
            label = f"{target} °C: " + ("допуск" if reached else "лимит 200 шагов")
            line, = axis.semilogy(trace["time"], [max(e, 1e-12) for e in errors], label=label)
            axis.scatter(trace["time"][-1], max(errors[-1], 1e-12), color=line.get_color())
        axis.axhline(0.2, color="gray", linestyle="--", label="Допуск 0,2 °C")
        axis.set_title({"model12": "Модель 1.2", "model27": "Модель 2.7", "model35": "Модель 3.5"}[key])
        axis.set_ylabel("Абсолютная ошибка, °C")
        style(axis)
        axis.legend()
    destination.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(destination, dpi=150, facecolor="white")
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", type=Path, nargs="?", default=Path("results.csv"))
    parser.add_argument("--examples", action="store_true")
    parser.add_argument("--output-dir", type=Path, default=Path("doc/figures"),
                        help="Directory for the five figures generated with --examples")
    args = parser.parse_args()
    if args.examples:
        source = Path("build/examples")
        destination = args.output_dir
        for key, title in (
            ("model12", "Модель 1.2 · запаздывание управления"),
            ("model27", "Модель 2.7 · реле с гистерезисом"),
            ("model35", "Модель 3.5 · явный метод Эйлера"),
        ):
            traces = [read_trace(source / f"{key}_{target}.csv") for target in (40, 60)]
            plot_trajectories(title, traces, destination / f"{key}.png", band=0.2)
        plot_tuning(source, destination / "tuning.png")
        plot_modeb(source, destination / "modeb.png")
    else:
        plot_trajectories("Переходный процесс", [read_trace(args.csv)], args.csv.with_suffix(".png"))


if __name__ == "__main__":
    main()
