#include "app.hpp"

#include "console.hpp"
#include "results.hpp"
#include "signals.hpp"
#include "simulator.hpp"


int runApp(std::istream& in, std::ostream& out, const std::filesystem::path& docDir) {
    Console console(in, out);

    const int modelChoice = console.readChoice("Модель: 1 - Model 1.6, 2 - Model 2.10, 3 - Model 3.4. Выбор: ", MODEL_COUNT);
    const int inputChoice = console.readChoice("Воздействие: 1 - ступенчатое, 2 - импульсное, 3 - гармоническое. Выбор: ", INPUT_COUNT);
    const auto signal = makeSignal(inputChoice);
    const auto model = console.createModel(modelChoice);   // stability warning
    const int steps = console.readSteps();

    const auto samples = simulate(*model, *signal, steps);
    printTable(out, model->name(), signal->name(), samples);

    if (!saveCsv(docDir / "results.csv", model->name(), signal->name(), samples)) {
        out << "Не удалось сохранить файл " << (docDir / "results.csv") << "\n";

        return 1;
    }

    return 0;
}
