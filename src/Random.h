#include <random>

// Random number generator utility
namespace RandomCustom
{
	std::random_device& GetRandomDevice();
	std::mt19937& GetRdnEngine();
	float GetRdnFloatUniform(std::uniform_real_distribution<float>& dis);
	bool Get5050TrueFalse();
}

