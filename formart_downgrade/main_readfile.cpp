#include "read_file.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <caminho/para/arquivo.osu>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
	
    auto root = std::make_shared<TreeNode>("root", NodeType::OBJECT);
	
    // Preenche a árvore a partir do arquivo
    populateTreeFromFile(filePath, root);

    std::cout << "Estrutura da árvore carregada::" << std::endl;
    root->printTree();

    return 0;
}