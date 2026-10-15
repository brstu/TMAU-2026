import csv
from pathlib import Path

import matplotlib.pyplot as plt


DOC_DIR = Path(__file__).resolve().parent.parent / "doc"
INPUTS = [("step", "Step"), ("impulse", "Impulse"), ("harmonic", "Harmonic")]
SERIES = ["y_1.6", "y_2.10", "y_3.4"]


def load(path):
    with open(path, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f))

    return {key: [float(row[key]) for row in rows] for key in rows[0]}


def main():
    fig, axes = plt.subplots(len(INPUTS), 1, figsize=(9, 10), sharex=True)

    for ax, (name, title) in zip(axes, INPUTS):
        data = load(DOC_DIR / f"results_{name}.csv")

        for key in SERIES:
            ax.plot(data["tau"], data[key], marker="o", markersize=3, label=key)

        ax.plot(data["tau"], data["u"], "--", color="gray", label="u")
        ax.set_title(f"{title} input")
        ax.grid(True)
        ax.legend()
        
    axes[-1].set_xlabel("tau")
    fig.tight_layout()
    fig.savefig(DOC_DIR / "results.png", dpi=130)
    plt.show()


if __name__ == "__main__":
    main()
