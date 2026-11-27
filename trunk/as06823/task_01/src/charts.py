import csv
import sys
from pathlib import Path

import matplotlib.pyplot as plt


INPUT_COLOR = "tab:blue"
OUTPUT_COLOR = "tab:orange"
ZERO_LINE_COLOR = "black"

SIGNALS = {
    "step": ("Ступенчатое воздействие", "u(τ) = 1"),
    "impulse": ("Импульсное воздействие", "u(0) = 1, далее u(τ) = 0"),
    "harmonic": ("Гармоническое воздействие", "u(τ) = sin(τ)"),
}

MODEL_SLUGS = {
    "Model 1.6": "1.6",
    "Model 2.8": "2.8",
    "Model 3.10": "3.10",
}


def resolve_csv_path():
    project_dir = Path(__file__).resolve().parent.parent

    # Разрешаем только ожидаемое имя файла,
    # а не произвольный путь из командной строки.
    if len(sys.argv) > 2 or (
        len(sys.argv) == 2 and sys.argv[1] != "results.csv"
    ):
        print("Разрешено использовать только файл results.csv.")
        return None

    csv_path = (Path.cwd() / "results.csv").resolve()

    # Дополнительно убеждаемся, что CSV находится внутри проекта.
    try:
        csv_path.relative_to(project_dir)
    except ValueError:
        print("CSV-файл должен находиться внутри каталога проекта.")
        return None

    if not csv_path.is_file():
        print(f"Файл с результатами не найден: {csv_path}")
        return None

    return csv_path


def main():
    csv_path = resolve_csv_path()

    if csv_path is None:
        return 1

    src_dir = Path(__file__).resolve().parent
    doc_dir = src_dir.parent / "doc"
    doc_dir.mkdir(parents=True, exist_ok=True)

    with csv_path.open("r", newline="", encoding="utf-8") as file:
        rows = list(csv.DictReader(file))

    if not rows:
        print("CSV-файл не содержит результатов.")
        return 1

    signal_id = rows[0]["signal"]
    model_name = rows[0]["model"]

    if signal_id not in SIGNALS:
        print(f"Неизвестный тип воздействия: {signal_id}")
        return 1

    if model_name not in MODEL_SLUGS:
        print(f"Неизвестная модель: {model_name}")
        return 1

    if any(
        row["signal"] != signal_id or row["model"] != model_name
        for row in rows
    ):
        print("CSV должен содержать данные одной модели и одного воздействия.")
        return 1

    title, formula = SIGNALS[signal_id]

    tau = [int(row["tau"]) for row in rows]
    inputs = [float(row["u"]) for row in rows]
    outputs = [float(row["y"]) for row in rows]

    print(f"Входное воздействие: {title}")
    print(f"Формула: {formula}")
    print(f"Модель: {model_name}")

    # Одно изображение с двумя графиками.
    fig, (ax1, ax2) = plt.subplots(
        2, 1,
        figsize=(11, 8),
        sharex=True,
    )

    # Верхний график: входное воздействие.
    ax1.plot(
        tau,
        inputs,
        color=INPUT_COLOR,
        marker="o",
        markersize=4,
        linewidth=1.8,
        label="Вход u(τ)",
    )

    ax1.set_title(f"1. Входное воздействие: {title}\n{formula}")
    ax1.set_ylabel("u(τ)")
    ax1.grid(True, alpha=0.3)
    ax1.axhline(0, color=ZERO_LINE_COLOR, linewidth=0.7)
    ax1.legend()

    # Нижний график: выход модели.
    ax2.plot(
        tau,
        outputs,
        color=OUTPUT_COLOR,
        marker="o",
        markersize=4,
        linewidth=1.8,
        label=f"Выход {model_name}",
    )

    ax2.set_title(f"2. Входное воздействие и выход {model_name}")
    ax2.set_xlabel("Момент времени τ")
    ax2.set_ylabel("Выход y(τ+1)", color=OUTPUT_COLOR)
    ax2.tick_params(axis="y", labelcolor=OUTPUT_COLOR)
    ax2.grid(True, alpha=0.3)

    # Отдельная правая шкала для входного воздействия.
    ax_input = ax2.twinx()

    ax_input.plot(
        tau,
        inputs,
        color=INPUT_COLOR,
        linestyle="--",
        marker="s",
        markersize=3,
        linewidth=1.6,
        label="Вход u(τ)",
    )

    ax_input.set_ylabel("Входное воздействие u(τ)", color=INPUT_COLOR)
    ax_input.tick_params(axis="y", labelcolor=INPUT_COLOR)

    lines1, labels1 = ax2.get_legend_handles_labels()
    lines2, labels2 = ax_input.get_legend_handles_labels()
    ax2.legend(lines1 + lines2, labels1 + labels2, loc="best")

    fig.suptitle("Результаты моделирования", fontsize=15)
    fig.tight_layout(rect=[0, 0, 1, 0.95])

    # Только разрешённые номера моделей и типы воздействий
    # используются в имени выходного файла.
    model_slug = MODEL_SLUGS[model_name]
    image_path = doc_dir / f"module_{model_slug}_{signal_id}.png"

    fig.savefig(image_path, dpi=160, bbox_inches="tight")

    print(f"Оба графика сохранены в одном изображении:\n{image_path.resolve()}")

    plt.show()
    plt.close(fig)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
