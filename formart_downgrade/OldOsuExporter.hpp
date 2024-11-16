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
	
	// Função para trocar a extensão de um arquivo
	static std::string changeExtensionTo(const std::string& filename,const std::string& extension);

    // Método para exportar a árvore para um arquivo binário
    bool exportTree(const std::shared_ptr<TreeNode>& root);

private:
	int DEGRESS_IN_CIRCLE = 32768;
	uint8_t VERSION_ODS = 1;
	
    std::string filePath;

    // Função auxiliar para escrever um nó
    void writeNode(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node);
	
    // Função auxiliar para escrever um nó
    void writeOldOsu(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node);

    // Função para converter uma string para bytes
    std::vector<uint8_t> stringToBytes(const std::string& str);

    // Função para converter um número para bytes
    std::vector<uint8_t> doubleToBytes(double value);
	
	void WriteVarLength(std::ofstream& outFile, int length);
	
	void WriteVarString(std::ofstream& outFile, const std::string& str);
};

#endif // OLDOSUEXPORTER_HPP
