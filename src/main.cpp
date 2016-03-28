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
    //open the file for the graph datas
    std::ofstream graphFile("graph.data", std::ios_base::trunc);

    GeneticAlgorithm ga(10000, 100);

    //You can set how the genomes will be ordered, which will influence the selection for BEST and ROULETTE_SELECTION
    ga.setComparaisonFunction(
        [](const auto& pair1, const auto& pair2)
        {
            return pair1.second == pair2.second ? pair1.first.size() < pair2.first.size() : pair1.second > pair2.second; //if they have same fitness return the smallest one
        });

    Interpretor inter;
    auto instructions = inter.getInstructions();

//    std::string targetString = "Hello World!";
    std::string targetString = "Kalle";

    //The targetFitness, the fitness function should return sizeOfString*256 for a perfect match
    int targetFitness = targetString.size()*256;

    //The Fitness function take a gene and should return it's fitness
    ga.setFitnessFunction([&instructions, &targetString, &targetFitness](const auto& gene)
    {
        //Since the fitness function is executed from multiple thread every static
        //should be thread_local to avoid concurency problems
        static thread_local Interpretor inter(100);

        //The program is stored as a vector of floats and has to be converted first
        auto values = gene.toValues(instructions);

//        std::cout << std::string(values.begin(), values.end()) << std::endl;

        //set the program into the brainfuck interpretor and execute it
        inter.setProgram(std::string(values.begin(), values.end()), true);
        inter.execute(targetFitness);

        int fitness = 0;

        //get the output of the program
        std::string output = inter.getOutput();

        size_t minLength = std::min(output.size(), targetString.size());

        //iterate over the output and for each char the less far it is from the output the more we add fitness
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

        //remove fitness for output too long
        fitness -= (std::abs((int)output.size() - (int)targetString.size()))*256;

        //a dynamic way of removing fitness that will make each char too much remove less fitness for long target and more fitness for short target
//      fitness -= (std::abs(output.size() - targetString.size()))*(500/targetString.size());

        //sometime I want to see how the output evoluate
//        if(fitness > 256)
//            std::cout << output << std::endl;

        //return the genome fitness
        return fitness;
    });

    //This is a callback generated each time a new population is generated
    //Return true to signal the complete end of whole program egeneration
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

        //If the current fitness equal or is superior to the target on
        //Then we can stop generating new proram
        return ga.getBestFitness() >= targetFitness;
    });

    //Vector of Generation Generator who describe how the next generation should be generated
    //The int component is to select how many new program should the generator generate
    std::vector<std::pair<GeneticAlgorithm::GenerationGenerator, int>> generators;

//    First we need to add the best genome without modifying it
//    We bassicaly just make sure to not loose our progress
    GeneticAlgorithm::GenerationGenerator generator;
//    So, no modification, effect = none
    generator.effect = GeneticAlgorithm::NONE;
//    We want to save the best
    generator.selector = GeneticAlgorithm::BEST;
//    And we only want to generate it once
    generators.emplace_back(generator, 1);


//    Exemple of crosseover generator between the best program and program selected using roulette selection
//    Note that cross over is pretty damn slow
//    generator.effect = GeneticAlgorithm::CROSSOVER;
//    generator.minimumCrossoverLenght = 0;
//    generator.maximumCrossoverLenght = -1;
//    generator.selector = GeneticAlgorithm::BEST;
//    generator.crossWith = GeneticAlgorithm::ROULETTE_SELECTION;
//    Here we want to generate 50 program using this generator
//    generators.emplace_back(generator2, 50);


    //Fill the rate for each mutation operation
    generator.rates.fill(1.f / (Gene::_MUT_SIZE - 1));
    generator.rates[Gene::MUT_SHIFT] = 0.f; //shift is freaking slow, forget it

    //dynamic generation count for those generator will depend on the target string length
    //auto generatorCount = targetString.size()*50;
    auto generatorCount = 30;

    //Adding a bunch of the most used form of mutation
    generator.selector = GeneticAlgorithm::BEST;
    generator.effect = GeneticAlgorithm::DEEP_MUTATION;

    //depending on the target output a certain combinaison of the following generator can best over other
//    generator.probability = 0.01f;
//    generators.emplace_back(generator, generatorCount);
//    generator.probability = 0.03f;
//    generators.emplace_back(generator, generatorCount);
//    generator.probability = 0.04f;
//    generators.emplace_back(generator, generatorCount);
    generator.probability = 0.05f;
    generators.emplace_back(generator, generatorCount);
//    generator.probability = 0.06f;
//    generators.emplace_back(generator, generatorCount);
//    generator.probability = 0.07f;
//    generators.emplace_back(generator, generatorCount);
//    generator.probability = 0.1f;
//    generators.emplace_back(generator, generatorCount);

    ga.setGenarationGenerator(generators);

    //And start it!
    ga.start();

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    //Retrieve the best program and execute it
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


    //I'm on Ubuntu and sometime generation can take quite some time
    //Rather than cehcking every 2 second I put that here so that it notifie me when finished
    system("notify-send -t 1000 \"Training comple\" \"\"");

    return 0;
}
