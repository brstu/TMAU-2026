"""Построение графика выбранной в C++ модели при выбранном входном сигнале."""

import csv
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
except ImportError:
    print("Не найдена библиотека matplotlib.")
    print("Установите её командой: py -m pip install matplotlib")
    raise SystemExit(1)

PROJECT_DIR = Path(__file__).resolve().parent
INPUT_LABELS = {
    "Step": "Ступенчатое: u(τ) = 1",
    "Impulse": "Импульсное: u(0) = 1, далее u(τ) = 0",
    "Harmonic": "Гармоническое: u(τ) = sin(τ)",
}


def find_csv() -> Path:
    candidates = []
    if len(sys.argv) > 1:
        supplied_path = Path(sys.argv[1]).expanduser()
        candidates.append(supplied_path)
        if not supplied_path.is_absolute():
            candidates.append(PROJECT_DIR / supplied_path)

    candidates.extend([PROJECT_DIR / "results.csv", Path.cwd() / "results.csv"])
    seen = set()
    for candidate in candidates:
        try:
            resolved = candidate.resolve()
        except OSError:
            continue
        if resolved in seen:
            continue
        seen.add(resolved)
        if resolved.is_file():
            return resolved

    print("Ошибка: не найден results.csv.")
    print(f"Папка скрипта: {PROJECT_DIR}")
    print(f"Текущая папка: {Path.cwd()}")
    print("Сначала запустите C++-программу.")
    raise SystemExit(1)


def read_csv(csv_path: Path) -> list[dict]:
    rows = []
    try:
        with csv_path.open("r", encoding="utf-8-sig", newline="") as csv_file:
            reader = csv.DictReader(csv_file)
            expected = {"model", "input", "tau", "u", "y"}
            if not expected.issubset(set(reader.fieldnames or [])):
                print("Ошибка: неправильный формат results.csv.")
                print("Ожидаемые столбцы: model,input,tau,u,y")
                raise SystemExit(1)

            for row in reader:
                rows.append({
                    "model": row["model"].strip(),
                    "input": row["input"].strip(),
                    "tau": int(row["tau"]),
                    "u": float(row["u"]),
                    "y": float(row["y"]),
                })
    except (OSError, ValueError, KeyError) as error:
        print(f"Не удалось прочитать {csv_path}: {error}")
        raise SystemExit(1)

    if not rows:
        print("В results.csv нет результатов моделирования.")
        raise SystemExit(1)
    return rows


def main() -> None:
    csv_path = find_csv()
    rows = read_csv(csv_path)

    models = list(dict.fromkeys(row["model"] for row in rows))
    inputs = list(dict.fromkeys(row["input"] for row in rows))

    if len(models) != 1:
        print("Ошибка: CSV должен содержать только одну выбранную модель.")
        print("Повторно запустите программу и выберите модель в начале.")
        raise SystemExit(1)
    if len(inputs) != 1:
        print("Ошибка: CSV должен содержать только один выбранный входной сигнал.")
        print("Повторно запустите программу и выберите входное воздействие в начале.")
        raise SystemExit(1)

    model_name = models[0]
    input_type = inputs[0]
    series = sorted(rows, key=lambda row: row["tau"])
    taus = [row["tau"] for row in series]
    values = [row["y"] for row in series]

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(taus, values, linewidth=2, label="Выход y(τ)")
    ax.set_title(f"{model_name}\nВходное воздействие: {INPUT_LABELS.get(input_type, input_type)}")
    ax.set_xlabel("Шаг симуляции τ")
    ax.set_ylabel("Выход y(τ)")
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()

    output_path = csv_path.parent / "results_plot.png"
    try:
        fig.savefig(output_path, dpi=160, bbox_inches="tight")
    except OSError as error:
        plt.close(fig)
        print(f"Не удалось сохранить график: {error}")
        raise SystemExit(1)

    print(f"Модель: {model_name}")
    print(f"Входное воздействие: {INPUT_LABELS.get(input_type, input_type)}")
    print(f"Данные прочитаны из: {csv_path}")
    print(f"График сохранён: {output_path}")
    print("Закройте окно графика, чтобы завершить программу.")
    try:
        plt.show()
    finally:
        plt.close(fig)


if __name__ == "__main__":
    main()
