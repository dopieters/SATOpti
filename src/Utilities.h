#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include "Geom.h"

inline void ClearTerminal() {
	std::cout << "\x1B[2J\x1B[H"; // ANSI escape code
}

template <typename Func>
inline bool measureExecutionTime(Func func, const std::string& funcName, Polygon A, Polygon B) {
	auto start = std::chrono::high_resolution_clock::now();
	bool isIntersect = func(A, B);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::string s = isIntersect ? "Intersect" : "don't intersect";
	std::cout << funcName << " took " << duration.count() << " ms" << s << "\n";
	return isIntersect;
}

template <typename Func>
inline std::pair<bool, float> measureExecutionTime(Func func, Polygon A, Polygon B) {
	auto start = std::chrono::high_resolution_clock::now();
	bool isIntersect = func(A, B);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	return {isIntersect, duration.count()};
}


class ScopeTimeMeasure
{
public:
	ScopeTimeMeasure(const std::string& n);
	~ScopeTimeMeasure();

private:
	std::chrono::steady_clock::time_point m_start;
	std::string m_scopeName;
};

