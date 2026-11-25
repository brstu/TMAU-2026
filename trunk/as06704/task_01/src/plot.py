import csv
import sys
from pathlib import Path


def find_results_csv():
    script_dir = Path(__file__).resolve().parent
    roots = [script_dir, Path.cwd()]
    checked = set()

    for root in roots:
        for directory in (root, *root.parents):
            candidate = directory / "results.csv"
            if candidate in checked:
                continue
            checked.add(candidate)
            if candidate.is_file():
                return candidate

    print("Файл results.csv не найден.")
    print("Сначала запустите TiMAY.exe и выполните моделирование.")
    print(f"Папка скрипта: {script_dir}")
    print(f"Текущая рабочая папка: {Path.cwd()}")
    return None


def short_signal_name(signal_name):
    name = (signal_name or "").lower()
    if "импульс" in name:
        return "Импульсное воздействие"
    if "ступенчат" in name:
        return "Ступенчатое воздействие"
    if "гармонич" in name:
        return "Гармоническое воздействие"
    return signal_name or "Входное воздействие"


def short_model_name(model_name):
    if not model_name:
        return ""
    return model_name.split(" — ", 1)[0].strip()


def main():
    filename = find_results_csv()
    if filename is None:
        return 1

    print(f"Чтение результатов: {filename}")

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Для построения графика установите Matplotlib: py -m pip install matplotlib")
        return 1

    tau_values = []
    input_values = []
    output_values = []
    model_name = ""
    signal_name = ""

    try:
        with filename.open("r", newline="", encoding="utf-8-sig") as file:
            reader = csv.DictReader(file)
            required_columns = {"tau", "u_tau", "y_tau", "model", "signal"}
            columns = set(reader.fieldnames or [])
            if not required_columns.issubset(columns):
                print("В results.csv нет информации о выбранной модели и сигнале.")
                print("Нужны столбцы: tau, u_tau, y_tau, model, signal.")
                print("Пересоберите проект из обновлённых main.cpp, csv_writer.h и csv_writer.cpp,")
                print("затем запустите моделирование заново.")
                return 1

            for row in reader:
                tau_values.append(float(row["tau"]))
                input_values.append(float(row["u_tau"]))
                output_values.append(float(row["y_tau"]))
                if row.get("model"):
                    model_name = row["model"].strip().strip('"')
                if row.get("signal"):
                    signal_name = row["signal"].strip().strip('"')

    except (OSError, ValueError, KeyError, csv.Error) as error:
        print(f"Не удалось прочитать результаты: {error}")
        return 1

    if not tau_values:
        print(f"В файле {filename} нет данных для построения графика.")
        return 1

    model_title = short_model_name(model_name)
    if not model_title or not signal_name:
        print("В CSV не записано название модели или входного воздействия.")
        print("Запустите моделирование заново в пересобранной версии TiMAY.")
        return 1

    title = f"{model_title} — {short_signal_name(signal_name)}"

    figure, axis = plt.subplots(figsize=(11, 6))
    axis.plot(tau_values, output_values, label="y(τ) — выходной сигнал", linewidth=2)
    axis.plot(tau_values, input_values, label="u(τ) — входной сигнал", linewidth=2, linestyle="--")
    axis.set_title(title, fontsize=14, fontweight="bold")
    axis.set_xlabel("Шаг τ")
    axis.set_ylabel("Значение")
    axis.grid(True, alpha=0.3)
    axis.legend(loc="best")

    figure.tight_layout()
    image_file = filename.parent / "results_plot.png"
    figure.savefig(image_file, dpi=150, bbox_inches="tight")
    print(f"Название графика: {title}")
    print(f"График сохранен в файл: {image_file}")
    plt.show()
    return 0


if __name__ == "__main__":
    sys.exit(main())
