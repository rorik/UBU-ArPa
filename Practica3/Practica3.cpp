#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	/* MPI initializers and variables */
	int process_rank, processes_count, cpu_name_length;
	char cpu_name[128];
	MPI_Status mpi_status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* Main program variables */
	const int N = 5, M = N;
	int matrix_A[N*M], matrix_B[N*M];
	int value_A, value_B, product, result;

	/* The Main program */
	if (process_rank == 0) {
		if (processes_count < M * N) {
			printf("\nExisten menos procesos que elementos de matriz, por lo que el result no va a ser completo.\n");
			printf("Se necesitan por lo menos %d procesos, pero solo %d han sido lanzados.\n\n", N*M, processes_count);
			fflush(stdout);
		}
		printf("Primera matriz:\n");
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				matrix_A[i*N + j] = (rand() % 10);
				printf("%d ", matrix_A[i*N + j]);
			}
			printf("\n");
		}
		fflush(stdout);

		printf("\nSegunda matriz:\n");
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				matrix_B[i*N + j] = (rand() % 10);
				printf("%d ", matrix_B[i*N + j]);
			}
			printf("\n");
		}
		fflush(stdout);
	}

	MPI_Scatter(matrix_A, 1, MPI_INT, &value_A, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(matrix_B, 1, MPI_INT, &value_B, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (process_rank < N * M) {
		product = value_A * value_B;
	}
	else {
		product = 0;
	}
	MPI_Reduce(&product, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (process_rank == 0) {
		if (processes_count < M * N) {
			printf("\nresult: %d (!!! NO HAN SIDO PROCESADOS TODOS LOS ELEMENTOS !!!)\n", result);
		}
		else {
			printf("\nresult: %d\n", result);
		}
		fflush(stdout);
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}