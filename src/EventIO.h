#pragma once


#include "Geom.h"
#include <vector>
#include <string>


void SavePairs(const std::vector<std::pair<Geom::Polygon, Geom::Polygon>>& polygons, const std::string& filename);
std::vector<std::pair<Geom::Polygon, Geom::Polygon>> LoadPairs(const std::string& filename);