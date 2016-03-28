#include "gene.h"

#include "randomgenerator.h"

#include <algorithm>

Gene::Gene(size_t size, bool initialize) : mGenes(size)
{
    if(initialize)
    {
        for(auto& g : mGenes)
            g = RandomGenerator::get().getFloat(0.f, 1.f);
    }

}

Gene::Gene(const std::vector<float>& genes) : mGenes(genes)
{

}

size_t Gene::size() const
{
    return mGenes.size();
}

std::vector<float> Gene::getGenes() const
{
    return mGenes;
}

void Gene::setGenes(const std::vector<float>& genes)
{
    mGenes = genes;
}

void Gene::crossover(const Gene& other, size_t minimumCrossoverLenght, size_t maximumCrossoverLenght)
{
    size_t shortest = size() < other.size() ? size() : other.size();

    if(maximumCrossoverLenght == -1)
        maximumCrossoverLenght = shortest/3;
//        maximumCrossoverLenght = 3;

    for(int x = 0; x < shortest; x += RandomGenerator::get().getInt(minimumCrossoverLenght, maximumCrossoverLenght))
    {
        size_t amount = RandomGenerator::get().getInt(minimumCrossoverLenght, maximumCrossoverLenght);
        for(int y = x; y < x + amount && y < shortest; y++)
            mGenes[y] = other.mGenes[y];
        x += amount;
    }

    if(size() > other.size())
    {
        size_t amount = RandomGenerator::get().getInt(shortest, size());
        mGenes.resize(amount);
    }
    else if(size() < other.size())
    {
        size_t amount = RandomGenerator::get().getInt(shortest, other.size());
        mGenes.insert(mGenes.end(), other.mGenes.begin() + shortest, other.mGenes.begin() + amount);
    }
}

void Gene::mutate(MutationTypes type, float mutationRate, size_t pos)
{
    if(mutationRate != 1 && mutationRate <= RandomGenerator::get().getFloat(0.f, 1.f))
        return;

    if(pos == -1 && size() == 0)
        pos = 0;
    else if(pos == -1)
        pos = RandomGenerator::get().getInt((size_t)0, size() - 1);

    if(type == MUT_INSERT)
    {
        mGenes.insert(mGenes.begin() + pos, RandomGenerator::get().getFloat(0.f, 1.f));
    }
    else if(type == MUT_DELETE)
    {
        if(mGenes.size())
            mGenes.erase(mGenes.begin() + pos);
    }
    else if(type == MUT_REPLACE)
    {
        if(mGenes.size())
            mGenes[pos] = RandomGenerator::get().getFloat(0.f, 1.f);
    }
    else if(type == MUT_SHIFT)
    {
        if(RandomGenerator::get().getFloat(0.f, 1.f) <= 0.5f)
            std::rotate(mGenes.begin(), mGenes.begin()+1,mGenes.end());
        else
            std::rotate(mGenes.rbegin(), mGenes.rbegin()+1,mGenes.rend());
    }
    else if(type == MUT_SWAP_LEFT)
        std::swap(mGenes[pos], mGenes[pos == 0 ? size() - 1 : pos - 1]);
    else if(type == MUT_SWAP_RIGHT)
        std::swap(mGenes[pos], mGenes[pos == size() - 1 ? 0 : pos + 1]);
}

void Gene::mutate(MutationRates rates, float mutationRate, size_t pos)
{
//    float selection = RandomGenerator::get().getFloat(0.f, std::accumulate(rates.begin(), rates.end(), 0.f));
    float selection = RandomGenerator::get().getFloat(0.f, 1.f);
    float current = 0;

    MutationTypes mutation = _MUT_FIRST;

    for(;mutation < _MUT_SIZE; ((int&)mutation)++) //YES I KNOW
    {
        current += rates[mutation];
        if(current > selection)
            break;
    }

    return mutate(mutation, mutationRate, pos);
}

void Gene::deepMutate(MutationRates rates, float mutationRate)
{
    for(int x = 0; x < mGenes.size(); x++)
        mutate(rates, mutationRate, x);
}

