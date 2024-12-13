#include "imageEncoding.h"

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "channel.h"
#include "math.h"
#include "encoder.h"

void imageEncodingStart(const CommonParams& params) {
    double p = userInputNumber<double>("Iveskite klaidos tikimybe p: ", 0.0, 1.0);
    Channel channel;

    // input image
    std::string imagePath = userInputString("Iveskite paveikslelio kelia");
    int width, height, channels;
    uint8_t* imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
    if (imageData == nullptr) {
        std::print("Klaida! Nepavyko nuskaityti paveikslelio.\n");
        return;
    }

    // split image into vectors
    std::span<const uint8_t> imageSpan(imageData, width * height * channels);
    size_t lastVectorPadding = 0;
    std::vector<vec> originalVectors = vectorsFromData(imageSpan, params.k, lastVectorPadding);
    stbi_image_free(imageData);

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

    // get image paths
    std::filesystem::path path(imagePath);
    std::string stem = path.stem().string();
    std::string unencodedPath = path.replace_filename(stem + "-unencoded.bmp").string();
    std::string encodedPath = path.replace_filename(stem + "-encoded.bmp").string();

    // convert vectors to images
    std::vector<uint8_t> unencodedData = vectorsToData(receivedUnencodedVectors, params.k, lastVectorPadding);
    std::vector<uint8_t> encodedData = vectorsToData(decodedVectors, params.k, lastVectorPadding);
    stbi_write_bmp(unencodedPath.c_str(), width, height, channels, unencodedData.data());
    std::print("Paveikslelis be uzkodavimo issaugotas '{}'\n", unencodedPath);
    stbi_write_bmp(encodedPath.c_str(), width, height, channels, encodedData.data());
    std::print("Paveikslelis su uzkodavimu issaugotas '{}'\n", encodedPath);
}