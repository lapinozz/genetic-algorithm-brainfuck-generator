#include "interpretor.h"
#include "geneticalgorithm.h"
#include "gene.h"
#include "threadpool.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>

int main()
{
    std::ofstream graphFile("graph.data", std::ios_base::trunc);

    Interpretor inter;
//    inter.setProgram("++++++++[>+>++>+++>++++>+++++>++++++>+++++++>++++++++>+++++++++>++++++++++>+++++++++++>++++++++++++>+++++++++++++>++++++++++++++>+++++++++++++++>++++++++++++++++<<<<<<<<<<<<<<<<-]>>>>>>>>>>>>>>>----.++++<<<<<<<<<<<<<<<>>>>>>>>>>>>>---.+++<<<<<<<<<<<<<>>>>>>>>>>>>>>+++.---<<<<<<<<<<<<<<>>>>>>>>>>>>>>>----.++++<<<<<<<<<<<<<<<.");
//    inter.setProgram("+++++++++++++++++++++++++++++++++[->++++++++++++++++++++++++<]>^");
//    inter.setProgram("+[+++++-+>++>++-++++++<<]>++.[+.]");
//    inter.setProgram("<+++++++.");
//    inter.setProgram("^^^^^-^]>^><-]^");
//    inter.setProgram("^^^^^-^]>^><-]^");
//    inter.setProgram("+.");

//    std::cout << (int)inter.getOutput()[0] << " : " << inter.getOutput() << " : " << inter.execute(1000) << std::endl;
    std::cout << inter.getOutput().size() << " : " << inter.execute(10000) << std::endl;
    std::cout << std::thread::hardware_concurrency() << std::endl;

    GeneticAlgorithm ga(10000, 100);

    ga.setComparaisonFunction(
        [](const auto& pair1, const auto& pair2)
        {
            return pair1.second == pair2.second ? pair1.first.size() < pair2.first.size() : pair1.second > pair2.second; //if they have same fitness return the smallest one
        });

    auto instructions = inter.getInstructions();
//    std::string targetString = "Hello World!";
    std::string targetString = "Kalle";
    int targetFitness = targetString.size()*256;

    ga.setFitnessFunction([&instructions, &targetString, &targetFitness](const auto& gene)
    {
        static thread_local Interpretor inter(100);

        auto values = gene.toValues(instructions);
//        std::cout << std::string(values.begin(), values.end()) << std::endl;
        inter.setProgram(std::string(values.begin(), values.end()), true);
        inter.execute(targetFitness);

        int fitness = 0;
        std::string output = inter.getOutput();
//
        size_t minLength = std::min(output.size(), targetString.size());

        float faultCount = 1.f;
        size_t x = 0;
        for(;x < minLength; x++)
        {
            int charFitness = 256 - std::abs(targetString[x] - output[x]);
            fitness += charFitness / faultCount;

            if(charFitness != 256)
//                faultCount *= 2;
//                faultCount *= 1.5f;
                break;
//                continue;
        }

//            fitness -= (std::abs(output.size() - targetString.size()))*(500/targetString.size());
            fitness -= (std::abs((int)output.size() - (int)targetString.size()))*256;


//        if(fitness > 256)
//            std::cout << output << std::endl;

        return fitness;
    });

    ga.setNewGenerationCallback([&](const auto& ga)
    {
        static auto startTime = std::chrono::steady_clock::now();
        static bool toSetup = true;
        if(toSetup)
        {
            toSetup = false;
            startTime = std::chrono::steady_clock::now();
        }

        std::cout << ga.getBestFitness() << " : " << targetFitness << std::endl;
        graphFile << ga.getBestFitness() << " " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-startTime).count()/100000 << std::endl;
        return ga.getBestFitness() >= targetFitness;
    });

    GeneticAlgorithm::GenerationGenerator generator;
    generator.rates.fill(1.f / (Gene::_MUT_SIZE - 1));
    generator.rates[Gene::MUT_SHIFT] = 0.f; //shift is freaking slow, forget it

    GeneticAlgorithm::GenerationGenerator generator3;
    generator3.effect = GeneticAlgorithm::CROSSOVER;
    generator3.crossWith = GeneticAlgorithm::ROULETTE_SELECTION;
    generator3.minimumCrossoverLenght = 0;
    generator3.maximumCrossoverLenght = -1;
    generator3.selector = GeneticAlgorithm::ROULETTE_SELECTION;

    GeneticAlgorithm::GenerationGenerator generator2;
    generator2.effect = GeneticAlgorithm::NONE;
    generator2.selector = GeneticAlgorithm::BEST;

    std::vector<std::pair<GeneticAlgorithm::GenerationGenerator, int>> generators;
    generators.emplace_back(generator2, 1);
//    ((uint32_t&)generator2.selector)++;
//    generators.emplace_back(generator2, 1);
//    ((uint32_t&)generator2.selector)++;
//    generators.emplace_back(generator2, 1);
//    ((uint32_t&)generator2.selector)++;
//    generators.emplace_back(generator2, 1);
//    generator2.crossWith = GeneticAlgorithm::ROULETTE_SELECTION;
    generator2.crossWith = GeneticAlgorithm::RANDOM;
    generators.emplace_back(generator2, 50);

//    generators.emplace_back(generator3, 100); //crossover is super duper slow

//    generator.selector = GeneticAlgorithm::BEST;
    generator.selector = GeneticAlgorithm::ROULETTE_SELECTION;
    generator.effect = GeneticAlgorithm::DEEP_MUTATION;
    generator.probability = 0.5f;
    generators.emplace_back(generator, 50);

    auto generatorCount = targetString.size()*50;
//    auto generatorCount = 200;

    generator.selector = GeneticAlgorithm::BEST;
    generator.effect = GeneticAlgorithm::DEEP_MUTATION;
    generator.probability = 0.01f;
    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.03f;
    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.04f;
    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.05f;
    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.06f;
    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.07f;
    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.1f;
    generators.emplace_back(generator, generatorCount);

    ga.setGenarationGenerator(generators);
    ga.start();

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    auto values = ga.getBest(0).toValues(instructions);
    inter.setProgram(std::string(values.begin(), values.end()), true);
    inter.execute(targetFitness);
    std::cout << std::string(values.begin(), values.end()) << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << inter.getOutput();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << ga.getBestFitness();

    system("notify-send -t 1000 \"Training comple\" \"\"");

    return 0;
}
