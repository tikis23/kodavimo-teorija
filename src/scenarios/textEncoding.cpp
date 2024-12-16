#include "textEncoding.h"

#include "../channel.h"
#include "../math.h"
#include "../encoder.h"

void textEncodingStart(const CommonParams& params) {
    double p = userInputNumber<double>("Iveskite klaidos tikimybe p: ", 0.0, 1.0);
    Channel channel;

    // input text and split it into vectors
    std::string inputText = userInputMultilineString("Iveskite teksta");
    size_t lastVectorPadding = 0;
    std::vector<vec> originalVectors = vectorsFromString(inputText, params.k, lastVectorPadding);

    // send throught channel original vectors
    std::vector<vec> receivedUnencodedVectors = originalVectors;
    for (auto& v : receivedUnencodedVectors) {
        v = channel.sendVector(v, params.k, p);
    }

    // encode vectors
    std::vector<vec> encodedVectors = originalVectors;
    for (auto& v : encodedVectors) {
        v = encode(v, params.gTransposed);
    }

    // send through channel encoded vectors
    std::vector<vec> receivedEncodedVectors = encodedVectors;
    for (auto& v : receivedEncodedVectors) {
        v = channel.sendVector(v, params.n, p);
    }

    // decode received vectors
    std::vector<vec> decodedVectors = receivedEncodedVectors;
    for (auto& v : decodedVectors) {
        v = decode(v, params.syndromes, params.h);
    }

    // convert vectors back to text
    std::string unencodedText = vectorsToString(receivedUnencodedVectors, params.k, lastVectorPadding);
    std::string encodedText = vectorsToString(decodedVectors, params.k, lastVectorPadding);

    // print results
    size_t unencodedErrorCount = 0;
    size_t encodedErrorCount = 0;
    for (size_t i = 0; i < inputText.size(); i++) {
        if (inputText[i] != unencodedText[i]) unencodedErrorCount++;
        if (inputText[i] != encodedText[i]) encodedErrorCount++;
    }
    std::print("Siustas tekstas be kodavimo ({} klaidos tekste):\n{}\n", unencodedErrorCount, unencodedText);
    std::print("Siustas tekstas su kodavimu ({} klaidos tekste):\n{}\n", encodedErrorCount, encodedText);
}