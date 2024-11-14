#ifndef BINARY_FILE_WRITER_H
#define BINARY_FILE_WRITER_H

#include <vector>
#include <string>
#include <cstdint>

class BinaryFileWriter {
public:
    // Construtor que aceita o caminho do arquivo
    BinaryFileWriter(const std::string& filePath);

    // Método para escrever dados binários no arquivo
    bool write(const std::vector<uint8_t>& data);

    // Método para definir o caminho do arquivo
    void setFilePath(const std::string& filePath);

    // Método para obter o caminho atual do arquivo
    std::string getFilePath() const;

private:
    std::string filePath;
};

#endif // BINARY_FILE_WRITER_H
