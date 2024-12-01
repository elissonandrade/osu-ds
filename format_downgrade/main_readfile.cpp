#include "read_file.h"
#include "OldOsuExporter.hpp"
#include "Mp3ToRawConverter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>


// Função para substituir o nome do arquivo em um caminho absoluto
std::string substituirNomeArquivo(const std::string& caminhoCompleto, const std::string& novoNome) {
    // Encontrar a última ocorrência de '/' ou '\\' para determinar onde começa o nome do arquivo
    size_t pos = caminhoCompleto.find_last_of("/\\");
    
    // Se a posição não for encontrada, retornar o caminho sem alteração
    if (pos == std::string::npos) {
        return caminhoCompleto;
    }

    // Criar o novo caminho com o nome substituído
    std::string novoCaminho = caminhoCompleto.substr(0, pos + 1) + novoNome;
    return novoCaminho;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <caminho/para/arquivo.osu>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
	
    auto root = std::make_shared<TreeNode>("root", NodeType::OBJECT);
	
    // Preenche a árvore a partir do arquivo
    populateTreeFromFile(filePath, root);
	
    std::cout << "Estrutura da árvore carregada:" << std::endl;
    root->printTree();

    OldOsuExporter exporter(OldOsuExporter::changeExtensionTo(filePath,"ods"));
	
	exporter.exportTree(root);
	
	Mp3ToRawConverter converter;
	auto general = root->getChild("General");
	auto audioFile = general->getChild("AudioFilename");
	std::string strAudioFile = std::get<std::string>(audioFile->value);
	
	strAudioFile.erase(0, strAudioFile.find_first_not_of(' '));
    std::string inputPath = substituirNomeArquivo(filePath,strAudioFile);
    std::string outputPath = OldOsuExporter::changeExtensionTo(inputPath,"raw");

    if (converter.convert(inputPath, outputPath)) {
        std::cout << "Arquivo convertido com sucesso!" << std::endl;
    } else {
        std::cerr << "Falha na conversão do arquivo." << std::endl;
    }
	
    return 0;
}