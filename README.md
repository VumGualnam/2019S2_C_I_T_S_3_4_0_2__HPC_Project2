<!-- ABOUT THE PROJECT -->
## About The Project

The goal of this project is to design an implement parallel algorithms to solve
the all-pairs-shortest path problem for a number of large graphs using either
**Dijkstra’s** or **Floyd-Warshall** algorithms. Specifically, we used the **Floyd Warshall algorithm** to solve the all-
pairs-shortest path problem while using the C programming language with the Message 
Passing Interface (MPI) to write parallelized code. We have also reported our observations 
on how speedup is affected by changing the size of the graph (512 to 4096 vertices) and the 
total number of processors used (1 to 16). This is shown in **test #1** – Speed up for graphs of 
2048 vertices using 1, 2, 4, 8, 16 processors and **test #2** – Speed up for 4 processors 
against graphs of 256, 512, 1024, 2048, 4096 vertices.

### Built With  
* [C](https://en.wikipedia.org/wiki/C_(programming_language))
* [MPI (Messaging Passing Interface)](https://www.open-mpi.org/)

### How to compile and run 
* #### Compile and run manually  
  ```$ mpicc -std=c99 -o finalfloyd finalfloyd.c && mpiexec -n <number of processes> ./finalfloyd <input_filename>```  
#### or   
* #### Use the `makefile`
  ```$ make && make report && make test```

### Authors
* V.S. Mang (22355071)
* J. Koh (22045985)
