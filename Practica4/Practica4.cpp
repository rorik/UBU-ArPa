#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>
int main(int argc, char *argv[])
{
	/* MPI initializers and variables */
	int process_rank, processes_count, cpu_name_length;
	char cpu_name[128];
	MPI_Status mpi_status;
	MPI_Comm mpi_comm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* Main program variables */
	const int DIMENSIONS = 2, N = squarestFactor(processes_count), M = processes_count / N;
	const int DIM_SIZE[DIMENSIONS] = { N, M }, PERIODS[DIMENSIONS] = {0};
	int *matrix_A, *matrix_B, *result, process_coords[DIMENSIONS];
	int addition, array_index;

	/* The Main program */
	randomizeSeed();
	if (process_rank == 0) {
		printf("\nPrimera matriz:\n");
		matrix_A = randomIntMatrix(M, N, 50);
		printMatrix(matrix_A, M, N, 3);

		printf("\nSegunda matriz:\n");
		matrix_B = randomIntMatrix(M, N, 50);
		printMatrix(matrix_B, M, N, 3);
	}
	else {
		matrix_A = emptyIntMatrix(M, N);
		matrix_B = emptyIntMatrix(M, N);
	}
	result = emptyIntMatrix(M, N);

	MPI_Cart_create(MPI_COMM_WORLD, 2, DIM_SIZE, PERIODS, 1, &mpi_comm);
	MPI_Cart_coords(mpi_comm, process_rank, 2, process_coords);
	MPI_Bcast(matrix_A, M* N, MPI_INT, 0, mpi_comm);
	MPI_Bcast(matrix_B, M* N, MPI_INT, 0, mpi_comm);

	array_index = process_coords[0] + process_coords[1] * N; // coord.x + coord.y * width
	addition = matrix_A[array_index] + matrix_B[array_index];
	MPI_Gather(&addition, 1, MPI_INT, result, 1, MPI_INT, 0, mpi_comm);
	if (process_rank == 0) {
		printf("\nSuma:\n");
		printMatrix(result, M, N, 3);
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}
