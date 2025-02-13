#include "EventIO.h"
#include <fstream>
#include <utility> 
#include <iostream> 


void savePairsOfPolygons(const std::vector<std::pair<Polygon, Polygon>>& polygons, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    size_t numPairs = polygons.size();
    outFile.write(reinterpret_cast<const char*>(&numPairs), sizeof(numPairs));

    auto savePolygon = [&outFile](const Polygon& polygon) {
        size_t numPoints = polygon.vertices.size();
        outFile.write(reinterpret_cast<const char*>(&numPoints), sizeof(numPoints));
        outFile.write(reinterpret_cast<const char*>(polygon.vertices.data()), numPoints * sizeof(Point));
        };

    for (const auto& pair : polygons) {
        savePolygon(pair.first);
        savePolygon(pair.second);
    }

    outFile.close();
}

std::vector<std::pair<Polygon, Polygon>> loadPairsOfPolygons(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file for reading." << std::endl;
        return {};
    }

    size_t numPairs;
    inFile.read(reinterpret_cast<char*>(&numPairs), sizeof(numPairs));

    auto loadPolygon = [&inFile](){
        size_t numPoints;
        inFile.read(reinterpret_cast<char*>(&numPoints), sizeof(numPoints));

        Polygon polygon;
        polygon.vertices.resize(numPoints);
        inFile.read(reinterpret_cast<char*>(polygon.vertices.data()), numPoints * sizeof(Point));
        polygon.CalculateBarycenter();
        return polygon;
        };

    std::vector<std::pair<Polygon, Polygon>> polygons(numPairs);
    for (size_t i = 0; i < numPairs; ++i) {
        polygons[i].first = loadPolygon();
        polygons[i].second = loadPolygon();
    }

    inFile.close();
    return polygons;
}
