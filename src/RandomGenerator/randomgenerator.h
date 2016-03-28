#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <memory>
#include <random>

//I wanted a thread local wrapper for the c++11 Random number generation

class RandomGenerator
{
public:
    static RandomGenerator& get();

    template<typename T>
    T getInt(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(mEngine);
    }

    template<typename T>
    T getFloat(T min = 0.f, T max = 1.f)
    {
        return std::uniform_real_distribution<T>{min, max}(mEngine);
    }

private:
    RandomGenerator() = default;
    static thread_local RandomGenerator mInstance;

//    std::mt19937 mEngine{1};
    std::mt19937 mEngine{std::random_device{}()};
//    std::ranlux48_base mEngine{std::random_device{}()};
};

#endif // RANDOMGENERATOR_H
