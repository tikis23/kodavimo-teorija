#include "io.h"
#include "scenarios/vectorEncoding.h"
#include "scenarios/textEncoding.h"
#include "scenarios/imageEncoding.h"

// Allows user to select a scenario.
// args:
//   p - ref to program parameters used in scenarios. Can be modified if user chooses to.
// returns:
//   false if user chose to exit, true otherwise.
bool chooseMode(CommonParams& p) {
    int32_t selection = userInputChoiceArray("Pasirinkite veiksma", {
        "vektoriaus kodavimas",
        "teksto kodavimas",
        "nuotraukos kodavimas",
        "keisti programos parametrus",
        "iseiti",
    });
    switch (selection) {
    case 1:
        vectorEncodingStart(p);
        break;
    case 2:
        textEncodingStart(p);
        break;
    case 3:
        imageEncodingStart(p);
        break;
    case 4:
        p = userInputCommonParameters();
        break;
    default:
        return false;
        break;
    }
    return true;
}

int main() {
    CommonParams p = userInputCommonParameters();
    while (chooseMode(p)) {}
    return 0;
}