#include "Utilities.h"

ScopeTimeMeasure::ScopeTimeMeasure(const std::string& n):
	m_scopeName(n)
{
	m_start = std::chrono::high_resolution_clock::now();
}

ScopeTimeMeasure::~ScopeTimeMeasure()
{
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - m_start;
	std::cout << m_scopeName << " took " << duration.count() << " ms" << std::endl;
}
