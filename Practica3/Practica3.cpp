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
	const int N = squarestFactor(processes_count);
	int *matrix_A, *matrix_B;
	int value_A, value_B, product, result;

	/* The Main program */
	randomizeSeed();
	if (processes_count / N != N) {
		if (process_rank == 0) {
			printf("\nEl numero de procesos lanzados (%d) no se corresponde con una matriz cuadrada.", processes_count);
			printf("\nSe requiere que sea un numero cuadrado.\n\n");
			fflush(stdout);
		}
		return 1;
	}
	if (process_rank == 0) {
		printf("\nPrimera matriz:\n");
		matrix_A = randomIntMatrix(N, N, 100);
		printMatrix(matrix_A, N, N, 3);

		printf("\nSegunda matriz:\n");
		matrix_B = randomIntMatrix(N, N, 100);
		printMatrix(matrix_B, N, N, 3);
	}

	MPI_Scatter(matrix_A, 1, MPI_INT, &value_A, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(matrix_B, 1, MPI_INT, &value_B, 1, MPI_INT, 0, MPI_COMM_WORLD);
	product = value_A * value_B;
	MPI_Reduce(&product, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (process_rank == 0) {
		printf("\nResultado: %d\n", result);
		fflush(stdout);
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}