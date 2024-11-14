#ifndef OLDOSUEXPORTER_HPP
#define OLDOSUEXPORTER_HPP

#include "TreeNode.hpp"
#include <fstream>
#include <vector>
#include <string>

class OldOsuExporter {
public:
    // Construtor que recebe o caminho para o arquivo de exportação
    OldOsuExporter(const std::string& filePath);

    // Método para exportar a árvore para um arquivo binário
    bool exportTree(const std::shared_ptr<TreeNode>& root);

private:
    std::string filePath;

    // Função auxiliar para escrever um nó
    void writeNode(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node);

    // Função para converter uma string para bytes
    std::vector<uint8_t> stringToBytes(const std::string& str);

    // Função para converter um número para bytes
    std::vector<uint8_t> doubleToBytes(double value);
};

#endif // OLDOSUEXPORTER_HPP
