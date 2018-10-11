#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>
int main(int argc, char *argv[])
{
	/* MPI initializers and variables */
	int process_rank, processes_count, cpu_name_length;
	char cpu_name[128];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* Main program variables */
	int *matrix_A, *matrix_B, *addition, *result;

	/* The Main program */
	randomizeSeed();
	if (process_rank == 0) {
		printf("Primera matriz:\n");
		matrix_A = randomIntMatrix(processes_count, processes_count, 51);
		printMatrix(matrix_A, processes_count, processes_count, 3);

		printf("\nSegunda matriz:\n");
		matrix_B = randomIntMatrix(processes_count, processes_count, 50);
		printMatrix(matrix_B, processes_count, processes_count, 3);

		result = emptyIntMatrix(processes_count, processes_count);
	}
	else {
		matrix_A = emptyIntMatrix(processes_count, processes_count);
		matrix_B = emptyIntMatrix(processes_count, processes_count);
		result = (int *)nullMatrix();
	}
	MPI_Bcast(matrix_A, processes_count * processes_count, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(matrix_B, processes_count * processes_count, MPI_INT, 0, MPI_COMM_WORLD);

	addition = emptyIntMatrix(1, processes_count);
	for (int j = 0, row = process_rank * processes_count; j < processes_count; j++) {
		addition[j] = matrix_A[row + j] + matrix_B[row + j];
	}
	MPI_Gather(addition, processes_count, MPI_INT, result, processes_count, MPI_INT, 0, MPI_COMM_WORLD);

	if (process_rank == 0) {
		printf("\nResultado:\n");
		printMatrix(result, processes_count, processes_count, 3);
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}