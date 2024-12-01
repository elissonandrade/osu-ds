#include "TreeNode.hpp"

// Construtores para inicializar os diferentes tipos de nós
TreeNode::TreeNode(const std::string& k, const std::string& v)
    : key(k), type(NodeType::STRING), value(v) {}

TreeNode::TreeNode(const std::string& k, double v)
    : key(k), type(NodeType::NUMBER), value(v) {}

TreeNode::TreeNode(const std::string& k, NodeType t)
    : key(k), type(t){
		
    if (type == NodeType::OBJECT) {
        value = std::unordered_map<std::string, std::shared_ptr<TreeNode>>{};
    } else if (type == NodeType::ARRAY) {
        value = std::vector<std::shared_ptr<TreeNode>>{};
    } else if (type == NodeType::STRING) {
        value = std::string{};  // Ou um valor padrão para String
    } else if (type == NodeType::NUMBER) {
        value = 0.0;  // Ou um valor padrão para Número
    }
}

// Função para adicionar um filho
void TreeNode::addChild(std::shared_ptr<TreeNode> child) {
    if (type == NodeType::OBJECT) {
        std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(value)[child->key] = child;
    } else if (type == NodeType::ARRAY) {
        std::get<std::vector<std::shared_ptr<TreeNode>>>(value).push_back(child);
    }
}

// Função para acessar um filho por chave (somente para objetos)
std::shared_ptr<TreeNode> TreeNode::getChild(const std::string& key) {
    if (type == NodeType::OBJECT) {
        auto& children = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(value);
        if (children.find(key) != children.end()) {
            return children[key];
        }
    }
    return nullptr;
}

// Função para acessar um filho por índice (somente para arrays)
std::shared_ptr<TreeNode> TreeNode::getChildAt(size_t index) {
    if (type == NodeType::ARRAY) {
        auto& children = std::get<std::vector<std::shared_ptr<TreeNode>>>(value);
        if (index < children.size()) {
            return children[index];
        }
    }
    return nullptr;
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
        case NodeType::OBJECT: {
            std::cout << "{\n";
            for (const auto& child : std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(value)) {
                child.second->printTree(depth + 1);
            }
            std::cout << indent << "}\n";
            break;
        }
        case NodeType::ARRAY: {
            std::cout << "[\n";
            for (const auto& child : std::get<std::vector<std::shared_ptr<TreeNode>>>(value)) {
                child->printTree(depth + 1);
            }
            std::cout << indent << "]\n";
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
