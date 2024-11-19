#ifndef OLDOSUEXPORTER_HPP
#define OLDOSUEXPORTER_HPP

#include "TreeNode.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <utility>

using Point = std::pair<float, float>;

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
	
    const double PI = 3.141592653589793;
	
    std::string filePath;

    // Função auxiliar para escrever um nó
    void writeNode(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node);
	
    // Função auxiliar para escrever um nó
    void writeOldOsu(std::ofstream& outFile, const std::shared_ptr<TreeNode>& node);

    // Função para converter uma string para bytes
    std::vector<uint8_t> stringToBytes(const std::string& str);

    // Função para converter um número para bytes
    std::vector<uint8_t> doubleToBytes(double value);
	
	std::vector<std::pair<int, int>> parseNumberPairs(const std::string& input);
	
	std::vector<std::pair<int, int>> createSlider(const std::vector<std::pair<int, int>>& points);
	
	std::vector<std::pair<int, int>> createTicks(const std::vector<std::pair<int, int>>& points,const float spatialLength, const float difficultyMultiplier, const float sliderTick);
	
	float calcularDistancia(const std::pair<int, int>& p1, const std::pair<int, int>& p2);
	
	int calculateTravelSpeed(const std::shared_ptr<TreeNode>& node,const int startTime,const float spatialLength, const float difficultyMultiplier);
	
	int calculateAngle(const std::pair<int, int>& a, const std::pair<int, int>& b);
	
	Point bezierPoint(float t, const Point& p0, const Point& p1, const Point& p2, const Point& p3);
	
	std::vector<std::pair<int, int>> generateBezierCurve(const Point& p0, const Point& p1, const Point& p2, const Point& p3);
	
	std::vector<std::pair<int, int>> generateCircularCurve(const Point& p1, const Point& p2, const Point& p3, float maxDistance = 10.0f);
	
	float angleBetween(const Point& center, const Point& point);
	
	std::pair<Point, float> findCircle(const Point& p1, const Point& p2, const Point& p3);
	
	std::pair<short, short> lerp(const std::pair<int, int>& a, const std::pair<int, int>& b, float amount);
	
	void WriteVarLength(std::ofstream& outFile, int length);
	
	void WriteVarString(std::ofstream& outFile, const std::string& str);
};

#endif // OLDOSUEXPORTER_HPP
