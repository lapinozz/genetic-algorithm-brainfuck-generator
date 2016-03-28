#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include "gene.h"
#include "threadpool.h"

#include <functional>

class GeneticAlgorithm
{
    public:

        using ComparaisonFunctionType = std::function<bool(const std::pair<Gene, int>&, const std::pair<Gene, int>&)>;
        using FitnessFunctionType = std::function<int(const Gene& gene)>;
        using NewGenerationCallbackType = std::function<bool(GeneticAlgorithm& ga)>;

        enum GeneSelectors : uint32_t
        {
            RANDOM              = 0x80000000,
            BEST                = 0x40000000,
            ROULETTE_SELECTION  = 0xC0000000,

            FLAG_BITSET = RANDOM | BEST | ROULETTE_SELECTION
        };

        enum Effects
        {
            MUTATION,
            RANDOM_MUTATION,
            DEEP_MUTATION,
            CROSSOVER,

            NONE
        };

        struct GenerationGenerator
        {
            Effects effect;

            union
            {
                struct //only used for crossover
                {
                    GeneSelectors crossWith;
                    size_t minimumCrossoverLenght;
                    size_t maximumCrossoverLenght;

                };
                Gene::MutationTypes type; //used when doing simple mutation
                Gene::MutationRates rates; //used for deep and random mutations
            };

            GeneSelectors selector;

            float probability; //this is passed as mutationRate for MUTATION, RANDOM_MUTATION and DEEP_MUTATION
        };

        GeneticAlgorithm(size_t populationSize = 100, size_t initGenomSize = 100);

        void reset();
        void start();

        void stop();

//        void sort();
        const Gene& getBest(size_t index) const;
        int getBestFitness() const;
        const Gene& selectGene(GeneSelectors selector);

        size_t getCurrentGeneration() const;

        void setComparaisonFunction(const ComparaisonFunctionType& compFunc);
        void setFitnessFunction(const FitnessFunctionType& fitnessFunc);
        void setNewGenerationCallback(const NewGenerationCallbackType& func);
        void setGenarationGenerator(const std::vector<std::pair<GenerationGenerator, int>>& generators);

    private:
        void nextGeneration();

        size_t mCurrentGeneration = 0;
        uint64_t mFitnessTotal = 0;

        size_t mPopulationSize; //we store it because the generator may not produce the same amoun of gene than initiale and we want to put back the initial amount of gene when reseting
        size_t mInitGenomSize; //   ^ same

        bool mStop;

        ThreadPool threadPool;

        ComparaisonFunctionType mComparaisonFunction;
        FitnessFunctionType mFintnessFunction;
        NewGenerationCallbackType mOnNewGeneration; //return true to stop
        std::vector<std::pair<GenerationGenerator, int>> mGenerationGenerator; // int is amount of gene to generate
        std::vector<std::pair<Gene, int>> mPopulation; // int is for fitness
};

#endif // GENETICALGORITHM_H
