/* This project is done by the following students:
 * Names: Jonathan Koh (22045985) and Vum Suan Mang (22355071)
 *
 * File:      finalfloyd.c
 * Purpose:   This project implements  Floyd-Warshall algorithms to solve
 *			  the all-pairs-shortest path problem for a number of large graphs.
 *            
 * Compile:   mpicc -std=c99 finalfloyd.c
 * Run:       mpiexec -n <number of processes> ./finalfloyd <input_filename>
 * 			  (N.B. Program automatically writes to output file)
 *
 * Input:     filename containing the adjacency matrix (*.in files)
 * Output:    (*.out) file, after being updated by floyd-Warshall algorithm 
 *            containing the costs of the cheapest paths between all pairs of vertices.
 *
 * Notes:
 * 1.  	nv, the number of vertices should be evenly divisible by np, the number of processors.
 * 2.  	The non-diagonal zero entries in the matrix should be replaced 
 *     	with the constant INFINITY (or a large number define as below)
 * 3.  	The one-dimensional matrix (allrows_matrix) 
 *		is partitioned into block of rows (b_o_rows). 
 * 
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


const int INFINITY = 1000000;

void Read_matrix(int b_o_rows[], int nv, int my_rank, int np, 
					MPI_Comm comm, MPI_File fh);
void Print_matrix(int b_o_rows[], int nv, int my_rank, int np, 
					MPI_Comm comm);
void Floyd(int b_o_rows[], int nv, int my_rank, int np, MPI_Comm comm);
int Owner(int k, int np, int nv);
void get_common_row(int b_o_rows[], int nv, int np, int common_row[], int k);
void Print_row(int b_o_rows[], int nv, int my_rank, int i);

int main(int argc, char* argv[]) {
	int  nv; /* number of vertices */
	int* b_o_rows; /* block of rows (total no. of rows / no. of processors to be used)*/
	int np, my_rank; /* number of processors to be used and rank of a processor */

	/* Get the number of vertices from the input file */
	FILE *fptr;
	fptr = fopen(argv[1], "rb");
	fread(&nv,sizeof(int),1,fptr);
	fclose(fptr);
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &np);
	MPI_Comm_rank(comm, &my_rank);
	MPI_File fh;
	
	/* Opening input file */
	int openerror = MPI_File_open(comm, argv[1], MPI_MODE_RDONLY,
									MPI_INFO_NULL, &fh);
	if(openerror != MPI_SUCCESS) { 
		printf("Error opening input file: %d\n", openerror);
	}


	if (my_rank == 0) {
	  printf("Number of vertices: %d\n", nv);
	}
	/* Rank 0 broadcasts number of vertices to all processors to be used */
	MPI_Bcast(&nv, 1, MPI_INT, 0, comm);
	/* Allocating memory for block of rows (b_o_rows) */
	b_o_rows = malloc(nv*nv/np*sizeof(int));	  
	if (b_o_rows == NULL) { 
        printf("b_o_rows memory not allocated.\n"); 
        exit(EXIT_FAILURE); 
    } 
	
	if (my_rank == 0) printf("Reading matrix...\n");
	
	Read_matrix(b_o_rows, nv, my_rank, np, comm, fh);

	Floyd(b_o_rows, nv, my_rank, np, comm);

	if (my_rank == 0) {} 
	Print_matrix(b_o_rows, nv, my_rank, np, comm);
    
	
	free(b_o_rows);
    /* Closing input file */
	MPI_File_close(&fh);
   
	MPI_Finalize();

	return 0;
} 

/*---------------------------------------------------------------------
 * Function:  Read_matrix
 * Purpose:   Read in the local_matrix on process 0 and scatter it using a 
 *            block row distribution among the processes.
 * IN args:   All except b_o_rows
 * OUT arg:   b_o_rows (block of rows)
 */
void Read_matrix(int b_o_rows[], int nv, int my_rank, int np, 
					MPI_Comm comm, MPI_File fh) { 

	int* allrows_mat = NULL;

	MPI_Status status;
	
	int* allrows_mat_wzeros = NULL;


	if (my_rank == 0) {
		/* Allocating memory for the whole matrix after replacing non-diagonal zeros */
		allrows_mat = malloc(nv*nv*sizeof(int));	  
		if (allrows_mat == NULL) {
			printf("allrows_mat not allocated.\n"); 
			exit(EXIT_FAILURE);; 
		} 
		
		/* Allocating memory for the whole matrix containing some non-diagonal zeros*/
		allrows_mat_wzeros = malloc(nv*nv*sizeof(int));	  
		if (allrows_mat_wzeros == NULL) {
				printf("allrows_mat_wzeros not allocated.\n"); 
				exit(EXIT_FAILURE); 
		} 
		/* Reading input file */
		for(int i = 0; i < nv*nv; i++){
			int z = MPI_File_read_at(fh, sizeof(int)*(i+1), 
								allrows_mat_wzeros+i, 1, 
								MPI_INT, &status);
			if (z != MPI_SUCCESS){
				printf("%s \n", "Error! MPI_File_read_at");
				MPI_Abort(comm, z);
			} 
		}
		int k = 0;
	    for (int i = 0; i < nv; i++)
			for (int j = 0; j < nv; j++){
				if (allrows_mat_wzeros[k] == 0 && (i != j)) {
					/* Replacing zeros in non-diagonal position by INFINITY*/
					allrows_mat[k] = INFINITY;
					k++;
				} else {
					allrows_mat[k] = allrows_mat_wzeros[k];
					k++;
				}
			}
		/* Rank 0 scattering b_o_rows to other ranks */
		MPI_Scatter(allrows_mat, nv*nv/np, MPI_INT, 
					b_o_rows, nv*nv/np, MPI_INT, 0, comm);
		free(allrows_mat);
		free(allrows_mat_wzeros);
	} else {
		MPI_Scatter(allrows_mat, nv*nv/np, MPI_INT, 
		b_o_rows, nv*nv/np, MPI_INT, 0, comm);
		}

} 


/*---------------------------------------------------------------------
 * Function:  Print_matrix
 * Purpose:   Gather the distributed matrix onto process 0 and print it.
 * IN args:   All parameters are IN
 * OUT args:  No OUT parameters
 */
void Print_matrix(int b_o_rows[], int nv, int my_rank, int np, 
					MPI_Comm comm) {
	int* allrows_mat = NULL;
	FILE *fptr;
	/* Name for output file */
	char outfile_name [10]; 
	sprintf(outfile_name, "%d%s", nv, ".out");


	if (my_rank == 0) {
		/* Allocating memory for all the rows of matrix */
		allrows_mat = malloc(nv*nv*sizeof(int));
		if (allrows_mat == NULL) {
			perror("Error allocating allrows_mat\n");
		} 
		if (my_rank == 0){
			/* Opening output file */
			fptr = fopen(outfile_name, "wb+");
			if (fptr == NULL){
				perror("Error opening output file: \n");
			}
		}
		
		/* Writing number of vertices to output file */
//		int a[] = {nv};
//		fwrite(a, 1, sizeof(int), fptr);
		int a = nv;
		// Printing to qsub generated output file as qsub never gives .out file
		printf("%d ", a);
		// Writing to .out file
		fwrite(&a, 1, sizeof(int), fptr);
		
		/* Rank 0 gathers computed shortest paths from all processors */
		MPI_Gather(b_o_rows, nv*nv/np, MPI_INT, 
                 allrows_mat, nv*nv/np, MPI_INT, 0, comm);
		
		fseek(fptr, sizeof(int), SEEK_SET);
		
		int h = 0;
		for (int i = 0; i < nv*nv; i++) {
			// Printing to qsub generated output file	
			printf("%d ", allrows_mat[h]); 
			// Writing to .out file	
			fwrite(allrows_mat+h, 1, sizeof(int), fptr); 
		h++;
		}

		free(allrows_mat);
		fclose(fptr);

	} else {
		/* Rank 0 gathers computed shortest paths from all processors */
		MPI_Gather(b_o_rows, nv*nv/np, MPI_INT, 
					allrows_mat, nv*nv/np, MPI_INT, 0, comm);
	}
} 

/*---------------------------------------------------------------------
 * Function:    Floyd
 * Purpose:     Implement a distributed version of Floyd's algorithm for
 *              finding the shortest path between all pairs of vertices.
 *              The adjacency matrix is distributed by block rows.
 * IN args:     All except b_o_rows
 * IN/OUT arg:  b_o_rows:  On input the adjacency matrix.  On output
 *              the matrix of lowests costs between all pairs of
 *              vertices
 */
void Floyd(int b_o_rows[], int nv, int my_rank, int np, MPI_Comm comm) {
	int global_k, local_i, global_j, temp;
	int root;  /* Rank of processor that holds common row */
	
	/* Allocating memory for common row */
	int* common_row = malloc(nv*sizeof(int));

	MPI_Barrier(MPI_COMM_WORLD);
	double starttime = MPI_Wtime();

	for (global_k = 0; global_k < nv; global_k++) {
		/* Finding which block of rows (processor/rank) will have the common row */
		root = Owner(global_k, np, nv);
		
		if (my_rank == root)
		get_common_row(b_o_rows, nv, np, common_row, global_k);
		/* Broadcasting a common row to be used for updating all other rows */
		MPI_Bcast(common_row, nv, MPI_INT, root, comm);
		for (local_i = 0; local_i < nv/np; local_i++)
			for (global_j = 0; global_j < nv; global_j++) {
				temp = b_o_rows[local_i*nv + global_k] + common_row[global_j];
				if (temp < b_o_rows[local_i*nv+global_j])
				b_o_rows[local_i*nv + global_j] = temp;
			}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	double endtime = MPI_Wtime();
	printf("Total time taken by rank %d : %f\n", my_rank, endtime - starttime);

	free(common_row);
} 

/*---------------------------------------------------------------------
 * Function:  Owner
 * Purpose:   Return rank of process that owns common_row
 * IN args:   All
 */
int Owner(int k, int np, int nv) {
	return k/(nv/np);
}

/*---------------------------------------------------------------------
 * Function:  get_common_row
 * Purpose:   get the common_row from block of rows(b_o_rows)
 * IN args:   All except common_row
 * OUT arg:   common_row
 */
void get_common_row(int b_o_rows[], int nv, int np, int common_row[], int k) {
   int j;
   int local_k = k % (nv/np);

   for (j = 0; j < nv; j++)
		common_row[j] = b_o_rows[local_k*nv + j];
}
