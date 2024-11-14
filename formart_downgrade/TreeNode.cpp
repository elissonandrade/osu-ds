#include "TreeNode.hpp"

// Construtores para inicializar os diferentes tipos de nós
TreeNode::TreeNode(const std::string& k, const std::string& v)
    : key(k), type(NodeType::STRING), value(v) {}

TreeNode::TreeNode(const std::string& k, double v)
    : key(k), type(NodeType::NUMBER), value(v) {}

TreeNode::TreeNode(const std::string& k, NodeType t)
    : key(k), type(t), value(std::vector<std::shared_ptr<TreeNode>>{}) {}

// Função para adicionar um filho
void TreeNode::addChild(std::shared_ptr<TreeNode> child) {
    if (type == NodeType::OBJECT || type == NodeType::ARRAY) {
        std::get<std::vector<std::shared_ptr<TreeNode>>>(value).push_back(child);
    }
}

// Função recursiva para imprimir a árvore
void TreeNode::printTree(int depth) const {
    std::string indent(depth * 2, ' ');

    if (!key.empty()) std::cout << indent << "\"" << key << "\": ";

    switch (type) {
        case NodeType::STRING:
            std::cout << "\"" << std::get<std::string>(value) << "\"\n";
            break;
        case NodeType::NUMBER:
            std::cout << std::get<double>(value) << "\n";
            break;
        case NodeType::OBJECT:
        case NodeType::ARRAY: {
            std::cout << "{\n";
            for (const auto& child : std::get<std::vector<std::shared_ptr<TreeNode>>>(value)) {
                child->printTree(depth + 1);
            }
            std::cout << indent << "}\n";
            break;
        }
    }
}

// Função para criar uma estrutura JSON-like de exemplo
std::shared_ptr<TreeNode> createSampleJSON() {
    auto root = std::make_shared<TreeNode>("", NodeType::OBJECT);

    // Adicionando chave-valor (string)
    root->addChild(std::make_shared<TreeNode>("name", "John"));

    // Adicionando chave-valor (número)
    root->addChild(std::make_shared<TreeNode>("age", 30));

    // Adicionando um array
    auto hobbiesNode = std::make_shared<TreeNode>("hobbies", NodeType::ARRAY);
    hobbiesNode->addChild(std::make_shared<TreeNode>("", "reading"));
    hobbiesNode->addChild(std::make_shared<TreeNode>("", "chess"));
    root->addChild(hobbiesNode);

    // Adicionando um objeto aninhado
    auto addressNode = std::make_shared<TreeNode>("address", NodeType::OBJECT);
    addressNode->addChild(std::make_shared<TreeNode>("city", "New York"));
    addressNode->addChild(std::make_shared<TreeNode>("zip", "10001"));
    root->addChild(addressNode);

    return root;
}
