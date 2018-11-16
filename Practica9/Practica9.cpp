#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>

#define STDOUT true //show output

int *parseArgs(int, char *[]);
void printMatrix(float **, int, int);
float **createMatrix(int, int);
void freeMatrix(float **);

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
	const int *MATRIX_DIM = parseArgs(argc, argv);
	float **matrix_A, **matrix_B, *row, *column;
	int *sendcounts_rows, *displs_rows, *sendcounts_columns, *displs_columns;

	/* The Main program */
	if (MATRIX_DIM == NULL || MATRIX_DIM[0] == 0 || MATRIX_DIM[1] == 0) {
		if (process_rank == 0) {
			printf("\nArguments parsing error:\nExpected matrix dimension, call the program using:\n");
			printf("\tbuild.exe ({int HEIGHT} {int WIDTH})|({int HEIGHT})\ne.g.:\n");
			printf("\tbuild.exe 8 6 = > 8x6 * 6x8 matrix\n\tbuild.exe 13 = > 13x13 * 13x13 matrix\n");
			fflush(stdout);
		}
		MPI_Finalize();
		return 0;
	}

	matrix_A = createMatrix(MATRIX_DIM[0], MATRIX_DIM[1]);
	matrix_B = createMatrix(MATRIX_DIM[1], MATRIX_DIM[0]);

	if (process_rank == 0) {
		randomizeSeed();
		for (int i = 0; i < MATRIX_DIM[0]; i++) {
			for (int j = 0; j < MATRIX_DIM[1]; j++) {
				matrix_A[i][j] = (float)rand() / (float)(RAND_MAX / 10); // float between 0 and 10
				matrix_B[j][i] = (float)rand() / (float)(RAND_MAX / 10); // float between 0 and 10
			}
		}
		#if STDOUT
			printf("[FIRST MATRIX]:\n");
			printMatrix(matrix_A, MATRIX_DIM[0], MATRIX_DIM[1]);
			printf("[SECOND MATRIX]:\n");
			printMatrix(matrix_B, MATRIX_DIM[1], MATRIX_DIM[0]);
			fflush(stdout);
		#endif // STDOUT
	}
	

	sendcounts_rows = (int *)malloc(MATRIX_DIM[0] * sizeof(int));
	displs_rows = (int *)malloc(MATRIX_DIM[0] * sizeof(int));
	row = (float *)malloc(MATRIX_DIM[1] * sizeof(float));
	for (int i = 0; i < MATRIX_DIM[0]; i++) {
		sendcounts_rows[i] = MATRIX_DIM[1];
		displs_rows[i] = MATRIX_DIM[1] * i;
	}
	MPI_Scatterv(matrix_A[0], sendcounts_rows, displs_rows, MPI_FLOAT, row, MATRIX_DIM[1], MPI_FLOAT, 0, MPI_COMM_WORLD);
	free(sendcounts_rows);
	free(displs_rows);
	if (process_rank == 0) {
		freeMatrix(matrix_A);
	}

	sendcounts_columns = (int *)malloc(MATRIX_DIM[1] * sizeof(int));
	displs_columns = (int *)malloc(MATRIX_DIM[1] * sizeof(int));
	column = (float *)malloc(MATRIX_DIM[0] * sizeof(float));
	for (int i = 0; i < MATRIX_DIM[1]; i++) {
		sendcounts_columns[i] = MATRIX_DIM[0];
		displs_columns[i] = MATRIX_DIM[0] * i;
	}
	MPI_Scatterv(matrix_B[0], sendcounts_columns, displs_columns, MPI_FLOAT, column, MATRIX_DIM[0], MPI_FLOAT, 0, MPI_COMM_WORLD);
	free(sendcounts_columns);
	free(displs_columns);
	if (process_rank == 0) {
		freeMatrix(matrix_B);
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}

int *parseArgs(int argc, char *argv[]) {
	if (argc < 2) {
		return NULL;
	}

	int *dimension = (int *)malloc(2 * sizeof(int));
	dimension[0] = (int)abs(strtol(argv[1], NULL, 0));
	if (argc > 2) {
		dimension[1] = (int)abs(strtol(argv[2], NULL, 0));
	} else {
		dimension[1] = dimension[0];
	}
	return dimension;
}

void printMatrix(float **matrix, const int height, const int width) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%5.2f ", matrix[i][j]);
		}
		printf("\n");
	}
}

float **createMatrix(const int height, const int width) {
	float **matrix;
	matrix = (float **)malloc(height * sizeof(float *));
	matrix[0] = (float *)malloc(width * height * sizeof(float));
	for (int i = 1; i < height; i++) {
		matrix[i] = matrix[i-1] + width;
	}
	return matrix;
}

void freeMatrix(float **matrix) {
	free(matrix[0]);
	free(matrix);
}