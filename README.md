# vlog5_multithreading

Making a multithreading application is easy, and I'm sure you will be able to make one after you see my video and study the source code.

What is hard it deciding where to divide task between threads.  One task can be computationally intensive or on high complexity, while the other task can have really low complexity therefore it is really hard to balance them.

Also, you must understand that, not all task can be processed by more than one thread/process.

To compile it you can choose to compile and type all the required parameters manually or simply run make and your computer will compile according to the included makefile.  The makefile will produce an executable named part_2

Non Source Code file:
1. customer_file contains the list of customer that will be arriving and their intention
2. simulation_log contains the output of the simulation
3. makefile, contains the make information

The source code was made using VI, intended to be compiled on GNU C and run on Linux system. It might or might not run on other system UNIX or its clone. 

Disclaimer: The source code, while tested and does satisfy my needs, does contains some bug and not very high performance.  I advise that you should not use it in production.  You should only use the source code for studying purposes only.

Video available in https://www.youtube.com/watch?v=73e3lKHsVfs
