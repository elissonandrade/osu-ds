#include "BinaryFileWriter.hpp"
#include <fstream>
#include <iostream>

// Construtor
BinaryFileWriter::BinaryFileWriter(const std::string& filePath) : filePath(filePath) {}

// Método para definir o caminho do arquivo
void BinaryFileWriter::setFilePath(const std::string& filePath) {
    this->filePath = filePath;
}

// Método para obter o caminho atual do arquivo
std::string BinaryFileWriter::getFilePath() const {
    return filePath;
}

// Método para escrever dados binários no arquivo
bool BinaryFileWriter::write(const std::vector<uint8_t>& data) {
    // Abre o arquivo em modo binário
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Erro ao abrir o arquivo para escrita!" << std::endl;
        return false;
    }

    // Escreve os dados binários no arquivo
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());

    // Verifica se a escrita foi bem-sucedida
    if (!outFile.good()) {
        std::cerr << "Erro ao escrever dados no arquivo!" << std::endl;
        return false;
    }

    // Fecha o arquivo
    outFile.close();
    return true;
}
