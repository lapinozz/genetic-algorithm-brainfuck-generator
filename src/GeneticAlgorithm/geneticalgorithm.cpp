#include "geneticalgorithm.h"

#include <cassert>
#include <algorithm>

#include "randomgenerator.h"

GeneticAlgorithm::GeneticAlgorithm(size_t populationSize, size_t initGenomSize) :
    mPopulationSize(populationSize),
    mInitGenomSize(initGenomSize),
    threadPool(std::thread::hardware_concurrency())
{
    reset();                           // p1 for pair1    p2 for pair2
    mComparaisonFunction = [](const auto& p1, const auto& p2){return p1.second > p2.second;};
}

void GeneticAlgorithm::reset()
{
    mPopulation.clear();
    for(size_t x = 0; x < mPopulationSize; x++)
        mPopulation.emplace_back(Gene{mInitGenomSize, true}, 0);

    mCurrentGeneration = 0;
    mFitnessTotal = 0;
}

void GeneticAlgorithm::start()
{
    mStop = false;

    assert(mFintnessFunction);
    assert(mOnNewGeneration);

    while(true)
    {
        for(auto& pair : mPopulation)
            threadPool.enqueue([&]()
            {
                pair.second = mFintnessFunction(pair.first);
            });

        threadPool.wait();

        std::sort(mPopulation.begin(), mPopulation.end(), mComparaisonFunction);

        mStop = mOnNewGeneration(*this);

        if(mStop || !mPopulation.size())
            break;

        nextGeneration();
    }
}

void GeneticAlgorithm::nextGeneration()
{
    mFitnessTotal = 0;
    for(const auto& pair : mPopulation)
        mFitnessTotal += pair.second;

    int newPopulationCount= 0;
    for(const auto& pair : mGenerationGenerator)
        newPopulationCount += pair.second;

    decltype(mPopulation) nextPopulation(newPopulationCount);

    int offset = 0;
    for(const auto& pair : mGenerationGenerator)
    {
        offset += pair.second;
        const auto& generator = pair.first;
        for(int x = offset - pair.second; x < offset; x++)
        {
            threadPool.enqueue([this, x, &nextPopulation, generator]()
            {
                Gene newGene(selectGene(generator.selector));

                if(generator.effect == MUTATION)
                    newGene.mutate(generator.type, generator.probability);
                else if(generator.effect == RANDOM_MUTATION)
                    newGene.mutate(generator.rates, generator.probability);
                else if(generator.effect == DEEP_MUTATION)
                    newGene.deepMutate(generator.rates, generator.probability);
                else if(generator.effect == CROSSOVER)
                    newGene.crossover(selectGene(generator.crossWith), generator.minimumCrossoverLenght, generator.maximumCrossoverLenght);

                nextPopulation[x] = {newGene, 0};
            });
        }
    }

    threadPool.wait();

    mPopulation = nextPopulation;

    mCurrentGeneration++;
}


void GeneticAlgorithm::stop()
{
    mStop = true;
}

const Gene& GeneticAlgorithm::getBest(size_t index) const
{
    return mPopulation[index].first;
}

int GeneticAlgorithm::getBestFitness() const
{
    return mPopulation[0].second;
}

const Gene& GeneticAlgorithm::selectGene(GeneSelectors selector)
{
    auto flag = selector & GeneSelectors::FLAG_BITSET;
    if(flag == RANDOM)
        return mPopulation[RandomGenerator::get().getInt((size_t)0, mPopulation.size()-1)].first;
    else if(flag == ROULETTE_SELECTION)
    {
        auto selected = RandomGenerator::get().getInt((uint64_t)0, mFitnessTotal);
        uint64_t current = 0;
        size_t x = 0;
        for(;x < mPopulation.size(); x++)
        {
            if(current > selected)
                break;

            current += mPopulation[x].second;
        }

        return mPopulation[x - 1].first;
    }
    else if(flag == BEST)
        return getBest(selector & ~BEST);

    assert(("bad selector", false));
}

size_t GeneticAlgorithm::getCurrentGeneration() const
{
    return mCurrentGeneration;
}

void GeneticAlgorithm::setComparaisonFunction(const ComparaisonFunctionType& compFunc)
{
    mComparaisonFunction = compFunc;
}

void GeneticAlgorithm::setFitnessFunction(const FitnessFunctionType& fitnessFunc)
{
    mFintnessFunction = fitnessFunc;
}

void GeneticAlgorithm::setNewGenerationCallback(const NewGenerationCallbackType& func)
{
    mOnNewGeneration = func;
}

void GeneticAlgorithm::setGenarationGenerator(const std::vector<std::pair<GenerationGenerator, int>>& generators)
{
    mGenerationGenerator = generators;
}
