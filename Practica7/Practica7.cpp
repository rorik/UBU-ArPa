#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>

/* Method definitions */
int main(int argc, char *argv[]) {
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
	const int N = 5;
	int *matrix;
	int *block_size_top;
	int *block_size_bottom;
	MPI_Aint *offset_top;
	MPI_Aint *offset_bottom;
	MPI_Datatype half_matrix_top;
	MPI_Datatype half_matrix_bottom;

	/* The Main program */
	offset_top = emptyIntMatrix(1, N);
	offset_bottom = emptyIntMatrix(1, N);
	block_size_top = emptyIntMatrix(1, N);
	block_size_bottom = emptyIntMatrix(1, N);
	for (int i = 0; i < N; i++) {
		offset_top[i] = i * N;
		offset_bottom[i] = offset_top[i];
		block_size_top[i] = N - i;
		block_size_bottom[i] = i + 1;
	}
	MPI_Type_indexed(N, block_size_top, offset_top, MPI_INT, &half_matrix_top);
	MPI_Type_indexed(N, block_size_bottom, offset_bottom, MPI_INT, &half_matrix_bottom);
	MPI_Type_commit(&half_matrix_top);
	MPI_Type_commit(&half_matrix_bottom);

	if (process_rank == 0) {
		printf("[TOP_OFFSET]:\n");
		printMatrix(offset_top, 1, N);
		printf("[BOT_OFFSET]:\n");
		printMatrix(offset_bottom, 1, N);
		printf("[TOP_BSIZE]:\n");
		printMatrix(block_size_top, 1, N);
		printf("[BOT_BSIZE]:\n");
		printMatrix(block_size_bottom, 1, N);
		fflush(stdout);
		randomizeSeed();
		matrix = randomIntMatrix(N, N, 100);
		MPI_Send(matrix, 1, half_matrix_top, 1, 0, MPI_COMM_WORLD); // Send the half_matrix_top to 1
		MPI_Send(matrix, 1, half_matrix_bottom, 2, 0, MPI_COMM_WORLD); // Send the half_matrix_bottom to 2
		printf("[SENT]:\n");
		printMatrix(matrix, N, N);
	} else if (process_rank == 1) {
		matrix = randomIntMatrix(N, N, 1);
		printf("[P1] Waiting for matrix...\n");
		fflush(stdout);
		MPI_Recv(matrix, 1, half_matrix_top, 0, 0, MPI_COMM_WORLD, &mpi_status); // Get the half_matrix_top from 0
		printf("[RCV1]:\n");
		printMatrix(matrix, N, N);
		/*for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (j < block_size_top[i]) {
					printf("%2d ", matrix[j]);
				} else {
					printf("   ", matrix[j]);
				}
			}
			printf("\n");
		}*/
		fflush(stdout);
	} else if (process_rank == 2) {
		matrix = randomIntMatrix(N, N, 1);
		printf("[P2] Waiting for matrix...\n");
		fflush(stdout);
		MPI_Recv(matrix, 1, half_matrix_bottom, 0, 0, MPI_COMM_WORLD, &mpi_status); // Get the half_matrix_bottom from 0
		printf("[RCV2]:\n");
		printMatrix(matrix, N, N);
		/*for (int i = 0, matrix_index = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (j >= block_size_bottom[i]) {
					printf("%2d ", matrix[j]);
				} else {
					printf("   ", matrix[j]);
				}
			}
			printf("\n");
		}*/
		fflush(stdout);
	}
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}