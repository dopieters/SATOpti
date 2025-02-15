#include "Random.h"



namespace RandomCustom
{
    std::random_device& GetRandomDevice() {
        static std::random_device rd;
        return rd;
    }


    std::mt19937& GetRdnEngine() {
        auto& rd = GetRandomDevice();
        static std::mt19937 gen(rd());
        return gen;
    }

    float GetRdnFloatUniform(std::uniform_real_distribution<float>& dis) {
        return dis(GetRdnEngine());

    }

    bool Get5050TrueFalse()
    {
        static std::bernoulli_distribution bDis(0.5);
        return bDis(GetRdnEngine());
    }

}

