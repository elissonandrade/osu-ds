#ifndef MP3_TO_RAW_CONVERTER_H
#define MP3_TO_RAW_CONVERTER_H

#include <string>
#include <cstdint>
#include <mpg123.h>

class Mp3ToRawConverter {
public:
    Mp3ToRawConverter();
    ~Mp3ToRawConverter();

    // Método para converter MP3 para RAW
    bool convert(const std::string& inputFilePath, const std::string& outputFilePath);

private:
    // Inicializar e limpar o decodificador mpg123
    bool initializeDecoder();
    void cleanupDecoder();

    // Conversão de amostra para 8-bit signed
    int8_t convertTo8BitSigned(float sample);

    // Manipulador do mpg123
    mpg123_handle *mpgHandle;
    int channels, encoding;
    long sampleRate;

    // Definições de taxa de amostragem
    static constexpr int targetSampleRate = 22050;
    static constexpr int targetChannels = 1; // Mono
};

#endif // MP3_TO_RAW_CONVERTER_H
