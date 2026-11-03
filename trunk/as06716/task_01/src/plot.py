import csv
import re
from pathlib import Path

import matplotlib.pyplot as plt


DOC_DIR = Path(__file__).resolve().parent.parent / "doc"


def load(path):
    with open(path, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f))


def main():
    rows = load(DOC_DIR / "results.csv")
    model = rows[0]["model"]
    signal = rows[0]["input"]
    tau = [int(row["tau"]) for row in rows]
    u = [float(row["u"]) for row in rows]
    y = [float(row["y"]) for row in rows]

    fig, ax = plt.subplots(figsize=(9, 5))
    ax.plot(tau, y, marker="o", markersize=3, label="y")
    ax.plot(tau, u, "--", color="gray", label="u")
    ax.set_title(f"{model}, {signal} input")
    ax.set_xlabel("tau")
    ax.grid(True)
    ax.legend()
    fig.tight_layout()

    name = re.sub(r"\W+", "_", f"{model}_{signal}").strip("_")
    output = DOC_DIR / f"{name}.png"
    fig.savefig(output, dpi=130)
    plt.show()


if __name__ == "__main__":
    main()
