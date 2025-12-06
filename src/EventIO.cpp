#include "EventIO.h"
#include <fstream>
#include <utility> 
#include <iostream> 


void SavePairs(const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& polygons, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    size_t numPairs = polygons.size();
    outFile.write(reinterpret_cast<const char*>(&numPairs), sizeof(numPairs));

    auto savePolygon = [&outFile](const Geom::Polygon& polygon) {
        size_t numPoints = polygon.Vertices.size();
        outFile.write(reinterpret_cast<const char*>(&numPoints), sizeof(numPoints));
        outFile.write(reinterpret_cast<const char*>(polygon.Vertices.data()), numPoints * sizeof(Geom::Point));
        };

    for (const auto& pair : polygons) {
        savePolygon(pair.first);
        savePolygon(pair.second);
    }

    outFile.close();
}

std::vector<std::pair<Geom::Polygon, Geom::Polygon>> LoadPairs(const std::string& filename) {
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

        Geom::Polygon polygon;
        polygon.Vertices.resize(numPoints);
        inFile.read(reinterpret_cast<char*>(polygon.Vertices.data()), numPoints * sizeof(Geom::Point));
        polygon.CalculateBarycenter();
        return polygon;
        };

    std::vector<std::pair<Geom::Polygon, Geom::Polygon>> polygons(numPairs);
    for (size_t i = 0; i < numPairs; ++i) {
        polygons[i].first = loadPolygon();
        polygons[i].second = loadPolygon();
    }

    inFile.close();
    return polygons;
}
