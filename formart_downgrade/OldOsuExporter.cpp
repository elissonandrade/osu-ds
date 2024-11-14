#include "OldOsuExporter.hpp"
#include <iostream>
#include <cstring>

// Construtor
OldOsuExporter::OldOsuExporter(const std::string& filePath) : filePath(filePath) {}

// Função auxiliar para converter uma string em bytes
std::vector<uint8_t> OldOsuExporter::stringToBytes(const std::string& str) {
    std::vector<uint8_t> bytes(str.begin(), str.end());
    bytes.push_back('\0'); // Adiciona o caractere nulo para finalizar a string
    return bytes;
}

// Função auxiliar para converter um double em bytes
std::vector<uint8_t> OldOsuExporter::doubleToBytes(double value) {
    std::vector<uint8_t> bytes(sizeof(double));
    std::memcpy(bytes.data(), &value, sizeof(double));
    return bytes;
}

// Função auxiliar para escrever um nó no arquivo
void OldOsuExporter::writeNode(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node) {
    if (!node) return;

    // Escreve a chave
    auto keyBytes = stringToBytes(node->key);
    uint32_t keySize = keyBytes.size();
    outFile.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
    outFile.write(reinterpret_cast<const char*>(keyBytes.data()), keySize);

    // Escreve o tipo do nó
    uint32_t nodeType = static_cast<uint32_t>(node->type);
    outFile.write(reinterpret_cast<const char*>(&nodeType), sizeof(nodeType));

    // Escreve o valor do nó baseado no tipo
    switch (node->type) {
        case NodeType::STRING: {
            auto valueBytes = stringToBytes(std::get<std::string>(node->value));
            uint32_t valueSize = valueBytes.size();
            outFile.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
            outFile.write(reinterpret_cast<const char*>(valueBytes.data()), valueSize);
            break;
        }
        case NodeType::NUMBER: {
            auto valueBytes = doubleToBytes(std::get<double>(node->value));
            outFile.write(reinterpret_cast<const char*>(valueBytes.data()), valueBytes.size());
            break;
        }
        case NodeType::OBJECT:
        case NodeType::ARRAY: {
            const auto& children = std::get<std::vector<std::shared_ptr<TreeNode>>>(node->value);
            uint32_t childrenCount = children.size();
            outFile.write(reinterpret_cast<const char*>(&childrenCount), sizeof(childrenCount));

            // Escreve recursivamente cada filho
            for (const auto& child : children) {
                writeNode(outFile, child);
            }
            break;
        }
    }
}

// Método para exportar a árvore para um arquivo binário
bool OldOsuExporter::exportTree(const std::shared_ptr<TreeNode>& root) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Erro ao abrir o arquivo para exportação!" << std::endl;
        return false;
    }

    writeNode(outFile, root);
    outFile.close();
    return true;
}
