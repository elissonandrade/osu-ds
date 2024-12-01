#include "Mp3ToRawConverter.h"
#include <samplerate.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>
#include <algorithm>

Mp3ToRawConverter::Mp3ToRawConverter() {
    mpgHandle = nullptr;
    initializeDecoder();
}

Mp3ToRawConverter::~Mp3ToRawConverter() {
    cleanupDecoder();
}

bool Mp3ToRawConverter::initializeDecoder() {
    if (mpg123_init() != MPG123_OK) {
        std::cerr << "Erro ao inicializar a biblioteca mpg123!" << std::endl;
        return false;
    }
    mpgHandle = mpg123_new(nullptr, nullptr);
    if (!mpgHandle) {
        std::cerr << "Erro ao criar o manipulador mpg123!" << std::endl;
        return false;
    }
    mpg123_param(mpgHandle, MPG123_ADD_FLAGS, MPG123_FORCE_MONO, 0);
    return true;
}

void Mp3ToRawConverter::cleanupDecoder() {
    if (mpgHandle) {
        mpg123_close(mpgHandle);
        mpg123_delete(mpgHandle);
        mpg123_exit();
    }
}

int8_t Mp3ToRawConverter::convertTo8BitSigned(float sample) {
    sample = std::clamp(sample, -1.0f, 1.0f);
    return static_cast<int8_t>(std::round(sample * 127));
}

bool Mp3ToRawConverter::convert(const std::string& inputFilePath, const std::string& outputFilePath) {
    if (mpg123_open(mpgHandle, inputFilePath.c_str()) != MPG123_OK) {
        std::cerr << "Falha ao abrir o arquivo de entrada: " << inputFilePath << std::endl;
        return false;
    }

    if (mpg123_getformat(mpgHandle, &sampleRate, &channels, &encoding) != MPG123_OK) {
        std::cerr << "Falha ao obter formato de áudio!" << std::endl;
        return false;
    }

    std::vector<unsigned char> buffer(4096);
    std::vector<float> samples;
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    size_t bytesRead;
    int error;

    SRC_STATE* srcState = src_new(SRC_SINC_FASTEST, channels, &error);
    if (!srcState) {
        std::cerr << "Erro ao inicializar o conversor de taxa de amostragem!" << std::endl;
        return false;
    }

    SRC_DATA srcData = {};
    std::vector<float> srcBuffer(4096);

    while (mpg123_read(mpgHandle, buffer.data(), buffer.size(), &bytesRead) == MPG123_OK) {
        for (size_t i = 0; i < bytesRead; i += channels * 2) {
            int16_t sample = (buffer[i + 1] << 8) | buffer[i];
            samples.push_back(static_cast<float>(sample) / 32768.0f);
        }

        srcData.data_in = samples.data();
        srcData.input_frames = samples.size() / channels;
        srcData.data_out = srcBuffer.data();
        srcData.output_frames = srcBuffer.size() / channels;
        srcData.src_ratio = static_cast<double>(targetSampleRate) / sampleRate;

        if (src_process(srcState, &srcData) != 0) {
            std::cerr << "Erro durante a conversão de taxa de amostragem!" << std::endl;
            return false;
        }

        for (int i = 0; i < srcData.output_frames_gen; ++i) {
            int8_t outSample = convertTo8BitSigned(srcBuffer[i]);
            outputFile.write(reinterpret_cast<char*>(&outSample), sizeof(int8_t));
        }

        samples.clear();
    }

    src_delete(srcState);
    outputFile.close();
    return true;
}
