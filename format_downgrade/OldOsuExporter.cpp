#include "OldOsuExporter.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <cmath>

// Construtor
OldOsuExporter::OldOsuExporter(const std::string& filePath) : filePath(filePath) {}

// Função para calcular um ponto da curva de Bézier cúbica
Point OldOsuExporter::bezierPoint(float t, const Point& p0, const Point& p1, const Point& p2, const Point& p3) {
    float x = std::pow(1 - t, 3) * p0.first +
              3 * std::pow(1 - t, 2) * t * p1.first +
              3 * (1 - t) * std::pow(t, 2) * p2.first +
              std::pow(t, 3) * p3.first;

    float y = std::pow(1 - t, 3) * p0.second +
              3 * std::pow(1 - t, 2) * t * p1.second +
              3 * (1 - t) * std::pow(t, 2) * p2.second +
              std::pow(t, 3) * p3.second;

    return {x, y};
}

// Função para gerar pontos na curva de Bézier com base nos pontos de controle
std::vector<std::pair<int, int>> OldOsuExporter::generateBezierCurve(const Point& p0, const Point& p1, const Point& p2, const Point& p3) {
    std::vector<std::pair<int, int>> curvePoints;

    // Adiciona o ponto inicial
    curvePoints.push_back(std::pair<int, int>((int)p0.first,(int)p0.second));

    // Incremento de t para gerar pontos ao longo da curva
    float tStep = 0.01; // 100 pontos ao longo da curva
    float maxDistance = 10.0; // Distância máxima entre pontos consecutivos

    Point lastPoint = p0;
    for (float t = tStep; t <= 1.0; t += tStep) {
        Point currentPoint = bezierPoint(t, p0, p1, p2, p3);

        // Calcula a distância euclidiana entre os pontos
        float distance = std::sqrt(
            std::pow(currentPoint.first - lastPoint.first, 2) +
            std::pow(currentPoint.second - lastPoint.second, 2)
        );

        // Se a distância for maior que o limite, adicionar o ponto
        if (distance <= maxDistance) {
            curvePoints.push_back(std::pair<int, int>((int)currentPoint.first,(int)currentPoint.second));
            lastPoint = currentPoint;
        }
    }

    // Adiciona o ponto final
    curvePoints.push_back(std::pair<int, int>((int)p3.first,(int)p3.second));

    return curvePoints;
}

std::pair<Point, float> OldOsuExporter::findCircle(const Point& p1, const Point& p2, const Point& p3) {
    float x1 = p1.first, y1 = p1.second;
    float x2 = p2.first, y2 = p2.second;
    float x3 = p3.first, y3 = p3.second;

    // Calcula os determinantes
    float A = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    float B = (x1 * x1 + y1 * y1) * (y3 - y2) +
              (x2 * x2 + y2 * y2) * (y1 - y3) +
              (x3 * x3 + y3 * y3) * (y2 - y1);

    float C = (x1 * x1 + y1 * y1) * (x2 - x3) +
              (x2 * x2 + y2 * y2) * (x3 - x1) +
              (x3 * x3 + y3 * y3) * (x1 - x2);

    float D = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3) +
              (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1) +
              (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);

    float cx = -B / (2 * A);
    float cy = -C / (2 * A);
    float radius = std::sqrt((B * B + C * C - 4 * A * D) / (4 * A * A));

    return {{cx, cy}, radius};
}

// Função para calcular o ângulo entre o centro e um ponto
float OldOsuExporter::angleBetween(const Point& center, const Point& point) {
    return std::atan2(point.second - center.second, point.first - center.first);
}


// Normaliza o ângulo para o intervalo [0, 2π]
float OldOsuExporter::normalizeAngle(float angle) {
    while (angle < 0) angle += 2 * PI;
    while (angle >= 2 * PI) angle -= 2 * PI;
    return angle;
}

// Função para gerar uma curva circular a partir de três pontos
std::vector<std::pair<int, int>> OldOsuExporter::generateCircularCurve(const Point& p1, const Point& p2, const Point& p3, float maxDistance) {
    std::vector<std::pair<int, int>> curvePoints;
    auto [center, radius] = findCircle(p1, p2, p3);

    float startAngle = normalizeAngle(angleBetween(center, p1));
    float midAngle = normalizeAngle(angleBetween(center, p2));
    float endAngle = normalizeAngle(angleBetween(center, p3));

    // Incremento de ângulo para gerar pontos ao longo da curva
    float angleStep = maxDistance / radius;
	
    // Determina a direção da curva (anti-horária ou horária)
    bool clockwise = (endAngle > startAngle) ^ (midAngle > startAngle) ^ (midAngle > endAngle);

    if (clockwise) {
        if (startAngle < endAngle) startAngle += 2 * PI;
		angleStep = -angleStep;
        //std::swap(startAngle, endAngle);
    } else {
        if (endAngle < startAngle) endAngle += 2 * PI;
    }
    //angleStep = std::min(angleStep, 0.1f); // Para uma resolução alta da curva

    
    //curvePoints.push_back(p1);
    // Gerando os pontos ao longo do arco
    for (float angle = startAngle; clockwise ? angle >= endAngle : angle <= endAngle; angle += angleStep) {
        float x = center.first + radius * std::cos(angle);
        float y = center.second + radius * std::sin(angle);
        curvePoints.push_back({x, y});
    }
    //curvePoints.push_back(p3);

    return curvePoints;
}

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

std::vector<std::pair<int, int>> OldOsuExporter::parseNumberPairs(const std::string& input) {
    std::vector<std::pair<int, int>> pairs;
    
    // Pular a primeira letra (X|) e começar a partir do índice 2
    std::string data = input.substr(2); 

    std::stringstream ss(data);
    std::string pair;

    // Separar a string pelos '|'
    while (std::getline(ss, pair, '|')) {
        // Encontrar o separador ':'
        size_t pos = pair.find(':');
        if (pos != std::string::npos) {
            // Extrair e converter x e y para inteiros
            int x = std::stoi(pair.substr(0, pos));
            int y = std::stoi(pair.substr(pos + 1));
            pairs.emplace_back(x, y);
        }
    }

    return pairs;
}

std::vector<std::pair<int, int>> OldOsuExporter::createSlider(const std::vector<std::pair<int, int>>& points) {
    std::vector<std::pair<int, int>> segments;

    // Criar segmentos ligando pontos sucessivos
	
	const double threshold = 10;
	segments.push_back(points[0]);
	double total=0;
    for (size_t i = 1; i < points.size(); ) {
		double distance = this->calcularDistancia(segments.back(),points[i]);
		if(total <= threshold ){
			total += distance;
		}
		if(total > threshold){
			float factor = (float)((distance+threshold-total)/distance);
			segments.push_back(this->lerp(segments.back(), points[i],factor));
			total -= threshold;
		}
		if (((int)this->calcularDistancia(segments.back(),points[i])) <= threshold){
			i++;
		}
    }
	if(((int)this->calcularDistancia(segments.back(),points[points.size()-1])) >0) {
		segments.push_back(points[points.size()-1]);
	}
	
    return segments;
}

std::vector<std::pair<int, int>> OldOsuExporter::createTicks(const std::vector<std::pair<int, int>>& points, const float spatialLength, const float difficultyMultiplier, const float sliderTick) {
	std::vector<std::pair<int, int>> ticks;
	
	int length = (int) (100.0*difficultyMultiplier)/sliderTick;
	int maxPoints = (((int)spatialLength) -1)/length;
	
	double total = 0;
	std::cout << "Generatin ticks for " <<maxPoints<<" max points and a lenght of "<<length<<std::endl;
	for(int i = 0; (ticks.size()< maxPoints )&& (i <(points.size() -1));i++ ){
		total += this->calcularDistancia(points[i], points[i + 1]);
		std::cout << "Creating tick for " <<total<<std::endl;
		if(total >= length){
			ticks.push_back(points[i + 1]);
			total -= length;
		}
	}
	return ticks;
}	

float OldOsuExporter::calcularDistancia(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
    return std::sqrt(std::pow(p2.first - p1.first, 2) + std::pow(p2.second - p1.second, 2));
}

std::pair<short, short> OldOsuExporter::lerp(const std::pair<int, int>& a, const std::pair<int, int>& b, float amount) {
	std::pair<short, short> v;
	v.first = (short) (a.first + (b.first - a.first)*amount);
	v.second = (short) (a.second + (b.second - a.second)*amount);
	std::cout << "Estimated point (" <<v.first<<","<<v.second<<") considering amount of "<<amount<<std::endl;
	return v;
}

int OldOsuExporter::calculateTravelSpeed(const std::shared_ptr<TreeNode>& node, const int startTime, const float spatialLength, const float difficultyMultiplier){
	
	const auto& timingPoints = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(node->getChild("TimingPoints")->value);
	int tpCount = timingPoints.size();
	// Escreve primeiro timing point
	double realBeatLength = std::get<double>(
							std::get<std::vector<std::shared_ptr<TreeNode>>>(timingPoints.at("0")->value).at(1)->value);
	int currentTime = 0;
	double inheritedLength = -1;
	for (int i=0; i<tpCount; i++ ) {
		std::string strIndex = std::to_string(i);
		const auto& tpProperties = std::get<std::vector<std::shared_ptr<TreeNode>>>(timingPoints.at(strIndex)->value);
		const int offset = std::get<double>(tpProperties.at(0)->value);
		currentTime += offset;
		if(currentTime > startTime){
			break;
		}
		const float beatLength = std::get<double>(tpProperties.at(1)->value);
		if(beatLength > 0){
			realBeatLength = beatLength;
			inheritedLength = -1;
		}else {
			inheritedLength = realBeatLength*(-100/beatLength);
		}
		
	}
	
	if(inheritedLength < 0){
		inheritedLength = realBeatLength;
	}
	
	return (int) inheritedLength*spatialLength/(100*difficultyMultiplier);
}

int OldOsuExporter::calculateAngle(const std::pair<int, int>& a, const std::pair<int, int>& b){
	return (int)(DEGRESS_IN_CIRCLE*atan2(b.second - a.second, b.first - a.first)/(2*PI));
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
	std::cout << "Reading metadata" <<std::endl;
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
	std::cout << "Reading filename" <<std::endl;
	auto audioFile = general->getChild("AudioFilename");
	std::string strAudioFile = std::get<std::string>(audioFile->value);
	strAudioFile.erase(0, strAudioFile.find_first_not_of(' '));
	
    WriteVarString(outFile, OldOsuExporter::changeExtensionTo(strAudioFile,"raw"));
	
	std::cout << "Read metadata of " <<strTitle<<std::endl;
	
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
	const float hpDrop = 0.005;
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
	
	
	const auto& events = std::get<std::unordered_map<std::string, std::shared_ptr<TreeNode>>>(node->getChild("Events")->value);
	int eventsCount = events.size();
	
	std::cout << "Reached after timings" <<eventsCount<<std::endl;
	
	std::vector<std::vector<std::shared_ptr<TreeNode>>> eventsBreaks;
	for (int i=0; i<eventsCount; i++ ) {
		std::string strIndex = std::to_string(i);
		const auto& ebProperties = std::get<std::vector<std::shared_ptr<TreeNode>>>(events.at(strIndex)->value);
		if((ebProperties.at(0)->type == NodeType::STRING && std::get<std::string>(ebProperties.at(0)->value).compare("Break") == 0) ||
		(ebProperties.at(0)->type == NodeType::NUMBER && std::get<double>(ebProperties.at(0)->value) == 2)){
			eventsBreaks.push_back(ebProperties);
		}
	}
	
	WriteVarLength(outFile,eventsBreaks.size());
	
	for(const std::vector<std::shared_ptr<TreeNode>> &ebProperties : eventsBreaks) {
		const int startTime = std::get<double>(ebProperties.at(1)->value);
		outFile.write(reinterpret_cast<const char*>(&startTime), sizeof(startTime));
		const int endTime = std::get<double>(ebProperties.at(2)->value);
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
		const uint8_t spinnerMask = 1<<3;
		const uint8_t sliderMask = 1<<1;
		
		if((spinnerMask & hoType) != 0){//spinners
			std::cout << "Found spinner on " <<i<<std::endl;
			const int endTime = std::get<double>(hoProperties.at(5)->value);
			outFile.write(reinterpret_cast<const char*>(&endTime), sizeof(endTime));
		} else if((sliderMask & hoType) != 0){//sliders
			std::cout << "Found slider on " <<i<<std::endl;
			
			const short repeats = std::get<double>(hoProperties.at(6)->value);
			outFile.write(reinterpret_cast<const char*>(&repeats), sizeof(repeats));
			
			const float spatialLength = std::get<double>(hoProperties.at(7)->value);			
			const int travelSpeed = calculateTravelSpeed(node,startTime,spatialLength,sliderMulti);
			outFile.write(reinterpret_cast<const char*>(&travelSpeed), sizeof(travelSpeed));
			
			std::cout << "Calculate travel speed of " <<travelSpeed<<std::endl;
			
			const std::string strPoints = std::get<std::string>(hoProperties.at(5)->value);
			// Obter os pares de números
			std::vector<std::pair<int, int>> points = this->parseNumberPairs(strPoints);
			
			points.insert(points.begin(),std::pair<int, int>(xOsu,yOsu));
			
			// Criar segmentos
			std::vector<std::pair<int, int>> segments;
			
			if(strPoints.at(0) == 'B'){
				segments = this->generateBezierCurve(points[0],points[1],points[2],points[3]);
				
			} else if(strPoints.at(0) == 'P'){
				segments = generateCircularCurve(points[0],points[1],points[2]);
			} else {
				segments = this->createSlider(points);
			}
			const short segmentCount = segments.size() -1;
			
			std::cout << "Created segment vetor of size " <<segmentCount<<std::endl;
			WriteVarLength(outFile,segmentCount+1);
			
			int angle = 0;
			for (int i = 0; i <= segmentCount; i++)
			{
				short xLocal = (short)segments[i].first;
				short yLocal = (short)segments[i].second;
				outFile.write(reinterpret_cast<const char*>(&xLocal),sizeof(short));
				outFile.write(reinterpret_cast<const char*>(&yLocal),sizeof(short));

				if (i < segmentCount){
					angle = this->calculateAngle(segments[i], segments[i + 1]);
				}
				outFile.write(reinterpret_cast<const char*>(&angle),sizeof(int));
				
				std::cout << "On point (" <<xLocal<<","<<yLocal<<") calculate a angle of "<<angle<<std::endl;
			}
			
			
			std::cout << "Filled segments, now using ticks for " <<sliderTick<<std::endl;
			auto ticks = this->createTicks(segments,spatialLength,sliderMulti,sliderTick);
			
			size_t ticksCount = ticks.size();
			
			std::cout << "Created tick vetor of size " <<ticksCount<<std::endl;
			WriteVarLength(outFile,ticksCount);
			
			for (int i = 0; i < ticksCount; i++)
			{
				short xLocal = (short)ticks[i].first;
				short yLocal = (short)ticks[i].second;
				outFile.write(reinterpret_cast<const char*>(&xLocal),sizeof(short));
				outFile.write(reinterpret_cast<const char*>(&yLocal),sizeof(short));

				
			}
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
