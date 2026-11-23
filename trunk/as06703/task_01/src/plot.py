from pathlib import Path
import csv
import sys


MODEL_NAMES = {
    "Model_1_4": "Model 1.4 — дискретный интегратор с потерями",
    "Model_2_6": "Model 2.6 — асимметричная полиномиальная модель",
    "Model_3_8": "Model 3.8 — линейно-квадратичное затухание",
}

SIGNAL_NAMES = {
    "step": "Ступенчатое воздействие",
    "impulse": "Импульсное воздействие",
    "harmonic": "Гармоническое воздействие",
}

SIGNAL_TITLE_NAMES = {
    "step": "Ступенчатое",
    "impulse": "Импульсное",
    "harmonic": "Гармоническое",
}


def read_result_file(file_path):
    tau_values = []
    input_values = []
    output_values = []

    with file_path.open("r", encoding="utf-8-sig", newline="") as file:
        reader = csv.DictReader(file, delimiter=";")
        required_columns = {"tau", "u_tau", "y_tau"}
        if not reader.fieldnames or not required_columns.issubset(reader.fieldnames):
            raise ValueError(f"В файле {file_path} отсутствуют необходимые столбцы.")

        for row in reader:
            tau_values.append(int(row["tau"]))
            input_values.append(float(row["u_tau"]))
            output_values.append(float(row["y_tau"]))

    return tau_values, input_values, output_values


def main():
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")

    if len(sys.argv) < 3:
        print("Использование: py plot.py Model_1_4 файл1.csv [файл2.csv ...]")
        return 1

    model_id = sys.argv[1]
    model_title = MODEL_NAMES.get(model_id, model_id)
    result_files = [Path(name) for name in sys.argv[2:]]

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Для построения графиков установите matplotlib: py -m pip install matplotlib")
        return 1

    # Явно выбираем шрифт с поддержкой кириллицы и математических символов.
    plt.rcParams["font.family"] = "DejaVu Sans"
    plt.rcParams["axes.unicode_minus"] = False

    results = []
    for file_path in result_files:
        if not file_path.exists():
            print(f"Файл с результатами не найден: {file_path}")
            continue

        tau, inputs, outputs = read_result_file(file_path)
        if tau:
            results.append((file_path, tau, inputs, outputs))

    if not results:
        print("Нет данных для построения графика.")
        return 1

    figure, output_axis = plt.subplots(figsize=(12, 7))
    input_axis = output_axis.twinx()
    colors = plt.rcParams["axes.prop_cycle"].by_key().get("color", [None])
    selected_signals = []

    for index, (file_path, tau, inputs, outputs) in enumerate(results):
        suffix = file_path.stem.replace("results_", "")
        signal_name = SIGNAL_NAMES.get(suffix, suffix)
        signal_title = SIGNAL_TITLE_NAMES.get(suffix, signal_name)
        selected_signals.append(signal_title)
        color = colors[index % len(colors)] if colors else None

        output_axis.plot(
            tau,
            outputs,
            color=color,
            marker="o",
            markersize=4,
            linewidth=2,
            label=f"Выход y(τ) — {signal_name}",
        )
        input_axis.plot(
            tau,
            inputs,
            color=color,
            linestyle="--",
            linewidth=1.6,
            alpha=0.65,
            label=f"Вход u(τ) — {signal_name}",
        )

    output_axis.set_title(
        f"{model_title}\nВходное воздействие: {', '.join(selected_signals)}",
        fontsize=12,
        pad=16,
    )
    output_axis.set_xlabel("Шаг времени, τ")
    output_axis.set_ylabel("Выход модели, y(τ)")
    input_axis.set_ylabel("Входное воздействие, u(τ)")
    output_axis.grid(True, alpha=0.3)

    output_lines, output_labels = output_axis.get_legend_handles_labels()
    input_lines, input_labels = input_axis.get_legend_handles_labels()
    output_axis.legend(
        output_lines + input_lines,
        output_labels + input_labels,
        loc="best",
        fontsize=9,
    )

    figure.tight_layout()
    output_path = Path("results_plot.png")
    figure.savefig(output_path, dpi=180, bbox_inches="tight")
    print(f"График для модели '{model_title}' сохранён: {output_path.resolve()}", flush=True)
    plt.show()
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, KeyError) as error:
        print(f"Ошибка построения графика: {error}")
        raise SystemExit(1)
