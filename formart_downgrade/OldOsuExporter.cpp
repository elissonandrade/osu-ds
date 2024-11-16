#include "OldOsuExporter.hpp"
#include <iostream>
#include <cstring>
#include <cmath>

// Construtor
OldOsuExporter::OldOsuExporter(const std::string& filePath) : filePath(filePath) {}

// Função auxiliar para converter uma string em bytes
std::vector<uint8_t> OldOsuExporter::stringToBytes(const std::string& str) {
    std::vector<uint8_t> bytes(str.begin(), str.end());
    //bytes.push_back('\0'); // Adiciona o caractere nulo para finalizar a string
    return bytes;
}

// Função auxiliar para converter um double em bytes
std::vector<uint8_t> OldOsuExporter::doubleToBytes(double value) {
    std::vector<uint8_t> bytes(sizeof(double));
    std::memcpy(bytes.data(), &value, sizeof(double));
    return bytes;
}

std::string OldOsuExporter::changeExtensionTo(const std::string& filename,const std::string& extension) {
    // Encontrar a posição do último ponto na string
    size_t dotPosition = filename.find_last_of(".");

    // Se houver um ponto, corta a extensão existente, caso contrário, usa o nome inteiro
    std::string baseName = (dotPosition != std::string::npos) 
                            ? filename.substr(0, dotPosition) 
                            : filename;

    // Adiciona a nova extensão ".ods"
    return baseName + "."+ extension;
}

void OldOsuExporter::WriteVarLength(std::ofstream& outFile, int length) {
    unsigned char temp = static_cast<unsigned char>(length & 0x7F);
    
    // Continua enquanto o comprimento for maior que 0 após o desvio de 7 bits
    while ((length >>= 7) > 0) {
        temp |= 0x80;  // Marca o byte como parte de uma sequência
        outFile.write(reinterpret_cast<char*>(&temp), sizeof(temp));
        temp = static_cast<unsigned char>(length & 0x7F);
    }

    // Escreve o último byte
    outFile.write(reinterpret_cast<char*>(&temp), sizeof(temp));
}


void OldOsuExporter::WriteVarString(std::ofstream& outFile, const std::string& str){
	auto strData = stringToBytes(str);
	int strSize = strData.size();
	if(strSize>0){
		WriteVarLength(outFile,strSize);
		outFile.write(reinterpret_cast<const char*>(strData.data()), strSize);
	} else {//osuDS crashes with empty strings
		const std::string space = " ";
		auto strSpace = stringToBytes(space);
		int spaceSize = strSpace.size();
		WriteVarLength(outFile,spaceSize);
		outFile.write(reinterpret_cast<const char*>(strSpace.data()), spaceSize);
		
	}
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

void OldOsuExporter::writeOldOsu(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node) {
    if (!node) return;
	
    auto formatHeader = stringToBytes("ODS");
    outFile.write(reinterpret_cast<const char*>(formatHeader.data()), formatHeader.size());
	
    outFile.write(reinterpret_cast<const char*>(&VERSION_ODS), sizeof(VERSION_ODS));
	auto meta = node->getChild("Metadata");
	auto title = meta->getChild("TitleUnicode");
	if(title.use_count() == 0){
		title = meta->getChild("Title");
	}
	std::string strTitle = std::get<std::string>(title->value);
    WriteVarString(outFile, strTitle);
	
	auto artist = meta->getChild("ArtistUnicode");
	if(artist.use_count() == 0){
		artist = meta->getChild("Artist");
	}
	std::string strArtist = std::get<std::string>(artist->value);
    WriteVarString(outFile, strArtist);
	
	auto creator = meta->getChild("Creator");
	std::string strCreator = std::get<std::string>(creator->value);
    WriteVarString(outFile, strCreator);
	
	auto version = meta->getChild("Version");
	std::string strVersion = std::get<std::string>(version->value);
    WriteVarString(outFile, strVersion);
	
	auto general = node->getChild("General");
	auto audioFile = general->getChild("AudioFilename");
	std::string strAudioFile = std::get<std::string>(audioFile->value);
	strAudioFile.erase(0, strAudioFile.find_first_not_of(' '));
	
    WriteVarString(outFile, OldOsuExporter::changeExtensionTo(strAudioFile,"raw"));
	
	std::cout << "Reached here" <<strTitle<<std::endl;
	
	auto difficulty = node->getChild("Difficulty");
	
	const uint8_t hpDrain = std::round(std::get<double>(difficulty->getChild("HPDrainRate")->value));
    outFile.write(reinterpret_cast<const char*>(&hpDrain), sizeof(hpDrain));
	const uint8_t circleSize = std::round(std::get<double>(difficulty->getChild("CircleSize")->value));
    outFile.write(reinterpret_cast<const char*>(&hpDrain), sizeof(circleSize));
	const uint8_t diffOverall = std::round(std::get<double>(difficulty->getChild("OverallDifficulty")->value));
    outFile.write(reinterpret_cast<const char*>(&hpDrain), sizeof(diffOverall));
	
	const float sliderMulti = std::get<double>(difficulty->getChild("SliderMultiplier")->value);
    outFile.write(reinterpret_cast<const char*>(&sliderMulti), sizeof(sliderMulti));
	const float sliderTick = std::get<double>(difficulty->getChild("SliderTickRate")->value);
    outFile.write(reinterpret_cast<const char*>(&sliderTick), sizeof(sliderTick));
	const float hpDrop = 5.0;
    outFile.write(reinterpret_cast<const char*>(&hpDrop), sizeof(hpDrop));
	const uint8_t peppyStars = 5;
    outFile.write(reinterpret_cast<const char*>(&peppyStars), sizeof(peppyStars));
	const float eyupStars = 5.0;
    outFile.write(reinterpret_cast<const char*>(&eyupStars), sizeof(eyupStars));
	
	std::cout << "Reached after difficulties" <<std::endl;
	
	const auto& timingPoints = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(node->getChild("TimingPoints")->value);
	int tpCount = timingPoints.size();
	WriteVarLength(outFile,tpCount);
	
	std::cout << "Reached here" <<tpCount<<std::endl;
	// Escreve cada Timing Point
	double realBeatLength = std::get<double>(
							std::get<std::vector<std::shared_ptr<TreeNode>>>(timingPoints.at("0")->value).at(1)->value);
	for (int i=0; i<tpCount; i++ ) {
		std::string strIndex = std::to_string(i);
		const auto& tpProperties = std::get<std::vector<std::shared_ptr<TreeNode>>>(timingPoints.at(strIndex)->value);
		const int offset = std::get<double>(tpProperties.at(0)->value);
		outFile.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
		const float beatLength = std::get<double>(tpProperties.at(1)->value);
		if(beatLength > 0){
			outFile.write(reinterpret_cast<const char*>(&beatLength), sizeof(beatLength));
			realBeatLength = beatLength;
		}else {
			const float inheritedLength = realBeatLength*(-100/beatLength);
			outFile.write(reinterpret_cast<const char*>(&inheritedLength), sizeof(inheritedLength));
		}
		const uint8_t sampleSet = std::get<double>(tpProperties.at(3)->value);
		outFile.write(reinterpret_cast<const char*>(&sampleSet), sizeof(sampleSet));
	}
	
	
	const auto& eventsBreaks = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(node->getChild("Events")->value);
	int eventsCount = eventsBreaks.size();
	WriteVarLength(outFile,eventsCount);
	
	std::cout << "Reached after timings" <<eventsCount<<std::endl;
	for (int i=0; i<eventsCount; i++ ) {
		std::string strIndex = std::to_string(i);
		const auto& ebProperties = std::get<std::vector<std::shared_ptr<TreeNode>>>(eventsBreaks.at(strIndex)->value);
		const int startTime = std::get<double>(ebProperties.at(0)->value);
		outFile.write(reinterpret_cast<const char*>(&startTime), sizeof(startTime));
		const int endTime = std::get<double>(ebProperties.at(1)->value);
		outFile.write(reinterpret_cast<const char*>(&endTime), sizeof(endTime));
	}
	
		
	const auto& hitObjects = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(node->getChild("HitObjects")->value);
	int hitObjectsCount = hitObjects.size();
	WriteVarLength(outFile,hitObjectsCount);
	
	std::cout << "Reached after events" <<hitObjectsCount<<std::endl;
	for (int i=0; i<hitObjectsCount; i++ ) {
		std::string strIndex = std::to_string(i);
		const auto& hoProperties = std::get<std::vector<std::shared_ptr<TreeNode>>>(hitObjects.at(strIndex)->value);
		const int startTime = std::get<double>(hoProperties.at(2)->value);
		outFile.write(reinterpret_cast<const char*>(&startTime), sizeof(startTime));
		const uint8_t hoType = std::get<double>(hoProperties.at(3)->value);
		outFile.write(reinterpret_cast<const char*>(&hoType), sizeof(hoType));
		const short xOsu = std::get<double>(hoProperties.at(0)->value);
		outFile.write(reinterpret_cast<const char*>(&xOsu), sizeof(xOsu));
		const short yOsu = std::get<double>(hoProperties.at(1)->value);
		outFile.write(reinterpret_cast<const char*>(&yOsu), sizeof(yOsu));
		const uint8_t sound = std::get<double>(hoProperties.at(4)->value);
		outFile.write(reinterpret_cast<const char*>(&sound), sizeof(sound));
		if(hoType == 3){//spinners
			const int endTime = std::get<double>(hoProperties.at(5)->value);
			outFile.write(reinterpret_cast<const char*>(&endTime), sizeof(endTime));
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

    writeOldOsu(outFile, root);
    outFile.close();
    return true;
}
