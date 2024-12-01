#ifndef READ_FILE_H
#define READ_FILE_H

#include "TreeNode.hpp"
#include <string>
#include <map>

// Função para ler um arquivo e preencher a árvore
void populateTreeFromFile(const std::string& filename, std::shared_ptr<TreeNode> root);

#endif
