# genetic-algorithm-brainfuck-generator

This is a project inspired by http://www.primaryobjects.com/2013/01/27/using-artificial-intelligence-to-write-self-modifying-improving-programs/


It use genetic algorithm to evolv brainfuck program.
For now the only program that have been generated print text.
It use threads to run and mutates multiple program programs at the same time.
The mutation and the fitness function were made to be the most modulable possible so it's very easy to try to generate new kind of program.

The program also output the current best fitness and the current time at each new generation so that a graph can be generated using gnuplot, I included the gnuplot script so if you have it installed you can just open a terminal and go to the project directory then type "gnuplot png.plot"
