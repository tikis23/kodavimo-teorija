#include "vectorEncoding.h"

#include "../channel.h"
#include "../math.h"
#include "../encoder.h"

void vectorEncodingStart(const CommonParams& params) {
    double p = userInputNumber<double>("Iveskite klaidos tikimybe p: ", 0.0, 1.0);
    Channel channel;

    // input vector and encode it
    vec originalVector = userInputVector("Iveskite vektoriu", params.k);
    vec encodedVector = encode(originalVector, params.gTransposed);
    std::print("Uzkoduotas vektorius: {}\n", printVec(encodedVector, params.n));

    // send through channel
    vec receivedVector = channel.sendVector(encodedVector, params.n, p);
    constexpr std::string_view receivedVectorMessage("Vektorius po siuntimo kanalu: ");
    std::print("{}{}\n", receivedVectorMessage, printVec(receivedVector, params.n));

    // show flipped bits and error count
    uint32_t errorCount = 0;
    std::string errorLocations;
    for (size_t i = 0; i < params.n; i++) {
        auto encodedBit  = (encodedVector  >> (params.n - i - 1)) & 1;
        auto receivedBit = (receivedVector >> (params.n - i - 1)) & 1;
        if (encodedBit == receivedBit) errorLocations += "-";
        else {
            errorLocations += "^";
            errorCount++;
        }
    }
    std::string errorCountStr = std::format("Klaidu skaicius: {}", errorCount);
    std::string alignErrors(receivedVectorMessage.size() - errorCountStr.size(), ' ');
    std::print("{}{}{}\n", errorCountStr, alignErrors, errorLocations);

    // vector editing
    if (userInputChoice("Ar norite redaguoti vektoriu?")) {
        receivedVector = userInputVector("Iveskite redaguota vektoriu", params.n);
    }

    // decode received vector
    vec decodedVector = decode(receivedVector, params.syndromes, params.h);
    std::print("Originalus vektorius: {}\n", printVec(originalVector, params.k));
    std::print("Dekoduotas vektorius: {}\n", printVec(decodedVector, params.k));
}