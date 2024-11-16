#include "read_file.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

// Função que divide a string com base no delimitador
std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    // Utiliza std::getline para dividir a string
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Função utilitária para remover espaços extras
std::string trim(const std::string &str) {
	if(str.empty())
		return "";
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Função para identificar o tipo de valor (número ou string)
NodeValue parseValue(const std::string &value) {
    if (std::isdigit(value[0]) || (value[0] == '-' && std::isdigit(value[1]))) {
        return std::stod(value);  // Valor numérico
    } else {
        return value;  // Valor string
    }
}

// Função para ler o arquivo e preencher a árvore
void populateTreeFromFile(const std::string& filename, std::shared_ptr<TreeNode> root) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << filename << "\n";
        return;
    }

    // Usando pilha para controlar a profundidade de objetos e arrays
    std::vector<std::shared_ptr<TreeNode>> nodeStack;
    nodeStack.push_back(root);

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.find("osu file format ") == 0) continue; // Ignora cabeçalho

        // Separar chave e valor (esperando um formato "key: value")
        size_t delimiterPos = line.find(":");
        //if (delimiterPos == std::string::npos) continue;
        
        std::string key = delimiterPos == std::string::npos ?"":trim(line.substr(0, delimiterPos));
        std::string value = delimiterPos == std::string::npos ?"":trim(line.substr(delimiterPos + 1));
        // Identificar se é um objeto ou array
		
        if (line.find("[") == 0  ) {//|| line.find("//")== 0
			
            // Início de um objeto
			size_t endTitle = line.find("]");
			std::string title = endTitle == std::string::npos ?
								line.substr(2) :
								line.substr(1, endTitle-1);
            auto newNode = std::make_shared<TreeNode>(title, NodeType::OBJECT);
            nodeStack.back()->addChild(newNode);
            nodeStack.push_back(newNode);
        } else if (line.find(",") != std::string::npos) {
            // Início de um array
			std::string properKey = "0";
			if((nodeStack.back()->type) == NodeType::OBJECT){
				auto parentMap = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(nodeStack.back()->value);
				if(parentMap.size()>0){
					std::vector<int> keys;
					for (const auto& pair : parentMap) {
						keys.push_back(std::stoi(pair.first));
					}

					// Ordenar as chaves de forma alfabética
					std::sort(keys.begin(), keys.end());

					// A última chave será a última no vetor ordenado
					int lastKey = keys.back();
					properKey = std::to_string(lastKey +1);
				}
			}
            auto newNode = std::make_shared<TreeNode>(properKey, NodeType::ARRAY);
			std::vector<std::string> result = split(line, ',');
			// Imprime os resultados
			for (const auto &token : result) {
				// Adicionando chave-valor
				NodeValue nodeValue = parseValue(token);// Determina o tipo de nó e cria o novo nó
				std::shared_ptr<TreeNode> newArrayEntry;
				if (std::holds_alternative<double>(nodeValue)) {
					newArrayEntry = std::make_shared<TreeNode>("", std::get<double>(nodeValue));
				} else {
					newArrayEntry = std::make_shared<TreeNode>("", std::get<std::string>(nodeValue));
				}
				newNode->addChild(newArrayEntry);
			}
            nodeStack.back()->addChild(newNode);
            //nodeStack.push_back(newNode);
        } else if (line.empty() && nodeStack.size() > 1) {
            // Fim de um objeto ou array
            nodeStack.pop_back();
        } else if (delimiterPos != std::string::npos) {
            // Adicionando chave-valor
            NodeValue nodeValue = parseValue(value);// Determina o tipo de nó e cria o novo nó
			std::shared_ptr<TreeNode> newNode;
			if (std::holds_alternative<double>(nodeValue)) {
				newNode = std::make_shared<TreeNode>(key, std::get<double>(nodeValue));
			} else {
				newNode = std::make_shared<TreeNode>(key, std::get<std::string>(nodeValue));
			}
            nodeStack.back()->addChild(newNode);
        }
    }

    file.close();
}