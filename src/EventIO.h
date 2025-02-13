#pragma once


#include "Geom.h"
#include <vector>
#include <string>


void savePairsOfPolygons(const std::vector<std::pair<Polygon, Polygon>>& polygons, const std::string& filename);
std::vector<std::pair<Polygon, Polygon>> loadPairsOfPolygons(const std::string& filename);