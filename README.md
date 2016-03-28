# genetic-algorithm-brainfuck-generator

This is a project inspired by http://www.primaryobjects.com/2013/01/27/using-artificial-intelligence-to-write-self-modifying-improving-programs/


It use genetic algorithm to evolv brainfuck program.
For now the only program that have been generated print text.
It use threads to run and mutates multiple program programs at the same time.
The mutation and the fitness function were made to be the most modulable possible so it's very easy to try to generate new kind of program.

The program also output the current best fitness and the current time at each new generation so that a graph can be generated using gnuplot, I included the gnuplot script so if you have it installed you can just open a terminal and go to the project directory then type "gnuplot png.plot"

The dynamic nature of the "Generation Generator" let you specifie how the next generation of program will be generated in a simple way will keeping control over it. It is especially usefull if the generation rule shoudl change over time, for exemple you might want the half of the population to be generated with a high mutation rate, a quarter with a low mutation rate and another quarter using crossover with short crossing section. And later when the fitness is at half of the target one maybe you'll want half of the generation to be generated using crossover with longer crossing section and the other half with very low mutation rate. You can also set for each generator if they should select the program to mutate using roulette selection, completly at random, or choose the one with best fitness.
