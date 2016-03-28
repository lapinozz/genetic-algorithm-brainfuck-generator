#include "randomgenerator.h"

thread_local RandomGenerator RandomGenerator::mInstance;

RandomGenerator& RandomGenerator::get()
{
    return RandomGenerator::mInstance;
}
