#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	/* MPI initializers and varibles */
	int process_rank, processes_count, cpu_name_length;
	char cpu_name[128];
	MPI_Status mpi_status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* Main program variables */
	const int N_MAX = 100;
	int matrix[N_MAX*N_MAX * 2];

	/* The Main program */
	if (process_rank == 0) {
		printf("Primera matriz:\n");
		for (int i = 0; i < processes_count; i++) {
			for (int j = 0; j< processes_count; j++) {
				matrix[i * N_MAX + j] = (rand() % 100);
				printf("%d ", matrix[i * N_MAX + j]);
			}
			printf("\n");
			fflush(stdout);
		}

		printf("\nSegunda matriz:\n");
		for (int i = N_MAX; i < processes_count * 2; i++) {
			for (int j = 0; j < processes_count; j++) {
				matrix[i * N_MAX + j] = (rand() % 100);
				printf("%d ", matrix[i * N_MAX + j]);
			}
			printf("\n");
			fflush(stdout);
		}
	}
	MPI_Bcast(matrix, N_MAX * 2 * N_MAX, MPI_INT, 0, MPI_COMM_WORLD);

	int resultado[N_MAX];
	for (int j = 0; j < processes_count; j++) {
		resultado[j] = matrix[(process_rank) * processes_count + j] + matrix[(process_rank + N_MAX) * processes_count + j];
	}
	int destino[N_MAX][N_MAX];
	MPI_Gather(resultado, processes_count, MPI_INT, &destino, processes_count, MPI_INT, 0, MPI_COMM_WORLD);


	if (process_rank == 0) {
		printf("\nResultado:\n");
		for (int i = 0; i < processes_count; i++) {
			for (int j = 0; j < processes_count; j++) {
				printf("%d ", destino[i][j]);
			}
			printf("\n");
			fflush(stdout);
		}
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}