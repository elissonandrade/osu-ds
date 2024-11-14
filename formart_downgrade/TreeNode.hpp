#ifndef TREENODE_HPP
#define TREENODE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <memory>

// Enum para definir o tipo do nó
enum class NodeType { STRING, NUMBER, OBJECT, ARRAY };

// Definições dos tipos possíveis para os nós
using NodeValue = std::variant<std::string, double, std::vector<std::shared_ptr<struct TreeNode>>>;

// Estrutura do nó da árvore
struct TreeNode {
    std::string key;        // Chave (opcional para arrays)
    NodeType type;          // Tipo do nó
    NodeValue value;        // Valor do nó (string, número ou vetor de filhos)

    // Construtores
    TreeNode(const std::string& k, const std::string& v);
    TreeNode(const std::string& k, double v);
    TreeNode(const std::string& k, NodeType t);

    // Função para adicionar filhos
    void addChild(std::shared_ptr<TreeNode> child);

    // Função para exibir a árvore (recursiva)
    void printTree(int depth = 0) const;
};

// Função para criar um exemplo de JSON-like
std::shared_ptr<TreeNode> createSampleJSON();

#endif // TREENODE_HPP
