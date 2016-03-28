#ifndef GENE_H
#define GENE_H

#include <unistd.h>
#include <vector>
#include <array>

class Gene
{
    public:
        Gene(size_t size = 0, bool initialize = true);
        Gene(const std::vector<float>& genes);

        template<typename T>
        std::vector<T> toValues(const std::vector<T>& range) const;

        size_t size() const;

        std::vector<float> getGenes() const;
        void setGenes(const std::vector<float>& genes);

        enum MutationTypes
        {
            MUT_INSERT,
            MUT_DELETE,
            MUT_REPLACE,
            MUT_SHIFT,
            MUT_SWAP_LEFT,
            MUT_SWAP_RIGHT,
            MUT_NONE,

            _MUT_SIZE, //the mutation count
            _MUT_FIRST = MUT_INSERT //used when iterating
        };

        using MutationRates = std::array<float, _MUT_SIZE>;

        void crossover(const Gene& other, size_t minimumCrossoverLenght = 0, size_t maximumCrossoverLenght = -1);
        void mutate(MutationTypes type, float mutationRate = 1.f, size_t pos = -1);
        void mutate(MutationRates rates, float mutationRate = 1.f, size_t pos = -1);
        void deepMutate(MutationRates rates, float mutationRate = 1.f);

    private:
        std::vector<float> mGenes;
};

template<typename T>
std::vector<T> Gene::toValues(const std::vector<T>& range) const
{
    int x = 0;
    std::vector<T> values(mGenes.size());
    for(const auto& gene : mGenes)
        values[x++] = range[range.size() * gene];

    return values;
}

#endif // GENE_H
