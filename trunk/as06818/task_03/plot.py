#!/usr/bin/env python3
# Построение графиков переходных процессов ЛР3 по CSV-файлам
# демо-прогона (task03_as06818_app --demo). Запуск из каталога task_03:
#   python3 plot.py
# Результаты сохраняются в doc/img.

import csv
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

OUT_DIR = os.path.join("doc", "img")


def read_csv(filename):
    """Чтение CSV с заголовком; возвращает список строк-словарей."""
    with open(filename, newline="") as file:
        return list(csv.DictReader(file))


def draw_process(csv_name, open_csv_name, title, ylabel, png_name):
    """График переходного процесса в стилистике задания: зелёный —
    объект без регулятора, синий — y(t) под регулятором, красный — u(t)."""
    rows = read_csv(csv_name)
    tau = [int(row["tau"]) for row in rows]
    y = [float(row["y"]) for row in rows]
    u = [float(row["u"]) for row in rows]
    w = float(rows[0]["w"])

    fig, ax_y = plt.subplots(figsize=(8, 5))
    ax_u = ax_y.twinx()

    if open_csv_name:
        open_rows = read_csv(open_csv_name)
        ax_y.plot([int(row["tau"]) for row in open_rows],
                  [float(row["y"]) for row in open_rows],
                  color="green", label="объект без регулятора")

    ax_y.plot(tau, y, color="blue", label="y(t) с регулятором")
    ax_y.axhline(w, color="gray", linestyle="--", linewidth=1,
                 label="задание w")
    ax_u.plot(tau, u, color="red", linewidth=1.5, label="u(t)")

    ax_y.set_xlabel("такт t")
    ax_y.set_ylabel(ylabel, color="blue")
    ax_u.set_ylabel("управление u", color="red")
    ax_y.set_title(title)
    ax_y.grid(True, linestyle=":")

    # Подписи двух осей в одной легенде.
    lines = ax_y.get_legend_handles_labels()[0] + ax_u.get_legend_handles_labels()[0]
    labels = ax_y.get_legend_handles_labels()[1] + ax_u.get_legend_handles_labels()[1]
    ax_y.legend(lines, labels, loc="lower right", fontsize=9)

    fig.tight_layout()
    fig.savefig(os.path.join(OUT_DIR, png_name), dpi=140)
    plt.close(fig)
    print(png_name)


def draw_setpoints():
    """Итоговый ПИД при разных задаваемых температурах."""
    fig, (ax_y, ax_u) = plt.subplots(2, 1, figsize=(8, 7), sharex=True)

    for csv_name, label in [("task03_set25.csv", "w = 25"),
                            ("task03_set50.csv", "w = 50"),
                            ("task03_set75.csv", "w = 75")]:
        rows = read_csv(csv_name)
        tau = [int(row["tau"]) for row in rows]
        ax_y.plot(tau, [float(row["y"]) for row in rows], label=label)
        ax_u.plot(tau, [float(row["u"]) for row in rows], label=label)
        ax_y.axhline(float(rows[0]["w"]), linestyle=":", linewidth=1)

    ax_y.set_ylabel("температура y")
    ax_y.set_title("ПИД-регулятор (Kp=1.0, Ki=0.5, Kd=1.0) при разных заданиях")
    ax_y.grid(True, linestyle=":")
    ax_y.legend()
    ax_u.set_xlabel("такт t")
    ax_u.set_ylabel("управление u")
    ax_u.grid(True, linestyle=":")
    ax_u.axhline(100, color="red", linestyle="--", linewidth=1)
    ax_u.text(1, 102, "предел актуатора 100")

    fig.tight_layout()
    fig.savefig(os.path.join(OUT_DIR, "setpoints.png"), dpi=140)
    plt.close(fig)
    print("setpoints.png")


def draw_mode_b():
    """Режим Б: симуляция до |e| < eps — момент остановки отмечен."""
    fig, ax = plt.subplots(figsize=(8, 5))

    for csv_name, label in [
            ("task03_modeb25_eps05.csv", "eps = 0.5"),
            ("task03_modeb25_eps01.csv", "eps = 0.1"),
            ("task03_modeb25_eps005.csv", "eps = 0.05")]:
        rows = read_csv(csv_name)
        tau = [int(row["tau"]) for row in rows]
        y = [float(row["y"]) for row in rows]
        ax.plot(tau, y, marker=".", markersize=4, label=label)
        ax.axvline(tau[-1], linestyle=":", linewidth=1)

    ax.axhline(25, color="gray", linestyle="--", linewidth=1, label="задание w = 25")
    ax.set_xlabel("такт t")
    ax.set_ylabel("температура y")
    ax.set_title("Режим Б: останов при |e| < eps (окно 3 такта)")
    ax.grid(True, linestyle=":")
    ax.legend()

    fig.tight_layout()
    fig.savefig(os.path.join(OUT_DIR, "modeb.png"), dpi=140)
    plt.close(fig)
    print("modeb.png")


def draw_tuning_comparison(csv_names, labels, title, ylabel, png_name,
                           open_csv_name=None):
    """Сравнение П/ПИ/ПИД на одном графике."""
    fig, ax = plt.subplots(figsize=(8, 5))

    if open_csv_name:
        open_rows = read_csv(open_csv_name)
        ax.plot([int(row["tau"]) for row in open_rows],
                [float(row["y"]) for row in open_rows],
                color="green", label="объект без регулятора")

    for csv_name, label in zip(csv_names, labels):
        rows = read_csv(csv_name)
        ax.plot([int(row["tau"]) for row in rows],
                [float(row["y"]) for row in rows], label=label)

    w = float(read_csv(csv_names[0])[0]["w"])
    ax.axhline(w, color="gray", linestyle="--", linewidth=1, label="задание w")
    ax.set_xlabel("такт t")
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(True, linestyle=":")
    ax.legend(loc="lower right", fontsize=9)

    fig.tight_layout()
    fig.savefig(os.path.join(OUT_DIR, png_name), dpi=140)
    plt.close(fig)
    print(png_name)


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    draw_process("task03_tune1_p.csv", "task03_open_linear.csv",
                 "Настройка, шаг 1: П-регулятор, Kp = 0.4 (w = 25)",
                 "температура y", "tune1_p.png")
    draw_process("task03_tune2_p.csv", "task03_open_linear.csv",
                 "Настройка, шаг 2: П-регулятор, Kp = 2.0 (w = 25)",
                 "температура y", "tune2_p.png")
    draw_process("task03_tune3_pi.csv", "task03_open_linear.csv",
                 "Настройка, шаг 3: ПИ-регулятор, Kp = 0.4, Ki = 0.3 (w = 25)",
                 "температура y", "tune3_pi.png")
    draw_process("task03_tune4_pid.csv", "task03_open_linear.csv",
                 "Настройка, шаг 4: ПИД, Kp = 1.0, Ki = 0.5, Kd = 1.0 (w = 25)",
                 "температура y", "tune4_pid.png")
    draw_setpoints()
    draw_mode_b()
    draw_tuning_comparison(
        ["task03_m2_p.csv", "task03_m2_pi.csv", "task03_m2_pid.csv"],
        ["П: Kp = 0.6", "ПИ: Kp = 0.6, Ki = 0.2",
         "ПИД: Kp = 0.6, Ki = 0.25, Kd = 0.4"],
        "Модель 2.2 (насыщение): П, ПИ и ПИД при w = 0.5",
        "выход y", "m2_tuning.png", "task03_open_m2.csv")
    draw_tuning_comparison(
        ["task03_m3_p.csv", "task03_m3_pi.csv", "task03_m3_pid.csv"],
        ["П: Kp = 1.0", "ПИ: Kp = 1.0, Ki = 0.5",
         "ПИД: Kp = 2.0, Ki = 0.3, Kd = 1.0"],
        "Уравнение 3.6 (Эйлер): П, ПИ и ПИД при w = 1.0",
        "выход y", "m3_tuning.png", "task03_open_m3.csv")


if __name__ == "__main__":
    main()
