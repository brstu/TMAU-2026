#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <limits>
#include <locale>
#include <cstdlib>
#include <clocale>
#include <sstream>
#include <algorithm>
#include <memory>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class Model {
public:
    virtual ~Model() = default;

    virtual vector<double> calculate(
        const vector<double>& u,
        int N
    ) = 0;
};

class Model110 : public Model {
private:
    double a1;
    double b1;
    double b2;

public:
    Model110(double a1, double b1, double b2)
        : a1(a1), b1(b1), b2(b2) {}

    vector<double> calculate(
        const vector<double>& u,
        int N
    ) override {
        vector y(N + 1, 0.0);

        for (int t = 0; t < N; ++t) {
            double u2 = (t >= 2) ? u[t - 2] : 0.0;
            y[t + 1] = a1 * y[t] + b1 * u[t] + b2 * u2;
        }

        return y;
    }

    void checkStability() const {
        cout << "\nАнализ устойчивости модели 1.10:\n";
        cout << "Корень характеристического уравнения: z = "
             << a1 << '\n';

        if (abs(a1) < 1.0) {
            cout << "Модель асимптотически устойчива.\n";
        } else if (abs(a1) > 1.0) {
            cout << "Модель неустойчива.\n";
        } else {
            cout << "Модель не является асимптотически устойчивой.\n";
        }
    }
};

class Model22 : public Model {
private:
    double a;
    double b;
    double Umin;
    double Umax;

public:
    Model22(double a, double b, double Umin = -1.0, double Umax = 1.0)
        : a(a), b(b), Umin(Umin), Umax(Umax) {}

    vector<double> calculate(
        const vector<double>& u,
        int N
    ) override {
        vector y(N + 1, 0.0);

        for (int t = 0; t < N; ++t) {
            double saturatedU = max(Umin, min(Umax, u[t]));
            y[t + 1] = a * y[t] + b * saturatedU;
        }

        return y;
    }
};

class Model34 : public Model {
private:
    double a;
    double dt;

public:
    Model34(double a, double dt) : a(a), dt(dt) {}

    vector<double> calculate(
        const vector<double>& u,
        int N
    ) override {
        vector y(N + 1, 0.0);
        y[0] = 1.0;

        for (int t = 0; t < N; ++t) {
            y[t + 1] = y[t] - dt * a * y[t] * y[t];
        }

        return y;
    }
};

// Enter — значение по умолчанию.
double readDouble(const string& prompt, double defaultValue) {
    while (true) {
        cout << prompt << " [" << defaultValue << "]: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            return defaultValue;
        }

        for (char& c : input) {
            if (c == ',') {
                c = '.';
            }
        }

        stringstream ss(input);

        if (double value; ss >> value) {
            if (char extra; !(ss >> extra) && isfinite(value)) {
                return value;
            }
        }

        cout << "Ошибка: введите корректное число.\n";
    }
}

// Enter — значение по умолчанию для целого числа.
int readInt(const string& prompt, int minValue, int defaultValue) {
    while (true) {
        cout << prompt << " [" << defaultValue << "]: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            return defaultValue;
        }

        stringstream ss(input);

        if (int value; ss >> value) {
            if (char extra; !(ss >> extra) && value >= minValue) {
                return value;
            }
        }

        cout << "Ошибка: введите целое число не меньше "
             << minValue << ".\n";
    }
}

vector<double> createSignal(int signalChoice, int N) {
    vector u(N + 1, 0.0);

    for (int t = 0; t < N; ++t) {
        if (signalChoice == 1) {
            u[t] = 1.0;
        } else if (signalChoice == 2) {
            u[t] = (t == 0) ? 1.0 : 0.0;
        } else {
            u[t] = sin(static_cast<double>(t));
        }
    }

    return u;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");
    locale::global(locale::classic());

    cout << "Выберите модель:\n";
    cout << "1. Линейная модель 1.10\n";
    cout << "2. Нелинейная модель 2.2\n";
    cout << "3. Дифференциальная модель 3.4\n";

    int modelChoice = readInt("Ваш выбор", 1, 1);

    while (modelChoice > 3) {
        cout << "Выберите 1, 2 или 3.\n";
        modelChoice = readInt("Ваш выбор", 1, 1);
    }

    unique_ptr<Model> model;
    string modelName;

    if (modelChoice == 1) {
        double a1 = readDouble("Введите a1", 0.5);
        double b1 = readDouble("Введите b1", 1.0);
        double b2 = readDouble("Введите b2", 0.3);

        auto linearModel = make_unique<Model110>(a1, b1, b2);
        linearModel->checkStability();

        model = move(linearModel);
        modelName = "model_1_10";
    } else if (modelChoice == 2) {
        double a = readDouble("Введите a", 0.5);
        double b = readDouble("Введите b", 1.0);

        model = make_unique<Model22>(a, b);
        modelName = "model_2_2";
    } else {
        double a = readDouble("Введите a", 0.1);
        double dt = readDouble("Введите dt", 0.1);

        while (dt <= 0.0) {
            cout << "dt должен быть больше нуля.\n";
            dt = readDouble("Введите dt", 0.1);
        }

        model = make_unique<Model34>(a, dt);
        modelName = "model_3_4";
    }

    cout << "\nВыберите входной сигнал:\n";
    cout << "1. Ступенчатый: u[t] = 1\n";
    cout << "2. Импульсный: u[0] = 1, остальные значения = 0\n";
    cout << "3. Гармонический: u[t] = sin(t)\n";

    int signalChoice = readInt("Ваш выбор", 1, 1);

    while (signalChoice > 3) {
        cout << "Выберите 1, 2 или 3.\n";
        signalChoice = readInt("Ваш выбор", 1, 1);
    }

    int N = readInt("Введите количество шагов N", 1, 10);

    vector u = createSignal(signalChoice, N);
    vector y = model->calculate(u, N);

    string signalName;

    if (signalChoice == 1) {
        signalName = "step";
    } else if (signalChoice == 2) {
        signalName = "impulse";
    } else {
        signalName = "sinusoidal";
    }

    ofstream dataFile("plot_data.txt");

    if (!dataFile) {
        cerr << "Ошибка: не удалось создать файл plot_data.txt.\n";
        return 1;
    }

    dataFile << "t,y\n";

    for (int t = 0; t <= N; ++t) {
        dataFile << t << ',' << y[t] << '\n';
    }

    dataFile.close();

    string command = "python plot.py " + modelName + " " + signalName;

    if (int result = system(command.c_str()); result == 0) {
        cout << "\nГрафик построен и сохранён в src.\n";
        cout << "Файл: " << modelName << "_" << signalName << ".png\n";
    } else {
        cout << "\nНе удалось запустить plot.py автоматически.\n";
        cout << "Проверьте, что Python и Matplotlib установлены.\n";
    }

    return 0;
}