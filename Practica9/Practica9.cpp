#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>

#define STDOUT true //show output

unsigned long *parseArgs(int, char *[]);

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
	const unsigned long *MATRIX_DIM = parseArgs(argc, argv);
	float **matrix_A, **matrix_B, *matrix_A_malloc_start, *matrix_B_malloc_start;

	/* The Main program */
	if (MATRIX_DIM == NULL || MATRIX_DIM[0] == 0 || MATRIX_DIM[1] == 0) {
		if (process_rank == 0) {
			printf("\nArguments parsing error:\nExpected matrix dimension, call the program using:\n");
			printf("\tbuild.exe ({int HEIGHT} {int WIDTH})|({int HEIGHT})\ne.g.:\n");
			printf("\tbuild.exe 8 6 = > 8x6 * 6x8 matrix\n\tbuild.exe 13 = > 13x13 * 13x13 matrix\n");
		}
		MPI_Finalize();
		return 0;
	}
	matrix_A = (float **)malloc(MATRIX_DIM[0] * sizeof(float *));
	matrix_A_malloc_start = (float *)malloc(MATRIX_DIM[0] * MATRIX_DIM[1] * sizeof(float));
	for (int i = 0; i < MATRIX_DIM[0]; i++) {
		matrix_A[i] = matrix_A_malloc_start + i * MATRIX_DIM[1];
	}
	matrix_B = (float **)malloc(MATRIX_DIM[1] * sizeof(float *));
	matrix_B_malloc_start = (float *)malloc(MATRIX_DIM[0] * MATRIX_DIM[1] * sizeof(float));
	for (int i = 0; i < MATRIX_DIM[1]; i++) {
		matrix_B[i] = matrix_B_malloc_start + i * MATRIX_DIM[0];
	}


	if (process_rank == 0) {
		randomizeSeed();
		#if STDOUT
			printf("[MATRIX_A]:\n");
		#endif // STDOUT
		for (int i = 0; i < MATRIX_DIM[0]; i++) {
			for (int j = 0; j < MATRIX_DIM[1]; j++) {
				matrix_A[i][j] = (float)rand() / (float)(RAND_MAX / 10); // float between 0 and 10
				matrix_B[j][i] = (float)rand() / (float)(RAND_MAX / 10); // float between 0 and 10
				#if STDOUT
					printf("%5.2f ", matrix_A[i][j]);
				#endif // STDOUT
			}
			#if STDOUT
				printf("\n");
			#endif // STDOUT
		}
		#if STDOUT
			printf("[MATRIX_B]:\n");
		#endif // STDOUT
		for (int i = 0; i < MATRIX_DIM[1]; i++) {
			for (int j = 0; j < MATRIX_DIM[0]; j++) {
				#if STDOUT
					printf("%5.2f ", matrix_B[i][j]);
				#endif // STDOUT
			}
			#if STDOUT
				printf("\n");
			#endif // STDOUT
		}
	}

	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}

unsigned long *parseArgs(int argc, char *argv[]) {
	if (argc < 2) {
		return NULL;
	}

	unsigned long *dimension = (unsigned long *)malloc(2 * sizeof(unsigned long));
	dimension[0] = abs(strtol(argv[1], NULL, 0));
	if (argc > 2) {
		dimension[1] = abs(strtol(argv[2], NULL, 0));
	} else {
		dimension[1] = dimension[0];
	}
	return dimension;
}