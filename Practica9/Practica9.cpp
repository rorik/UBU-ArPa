#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>

#define STDOUT true //show output

int *parseArgs(int, char *[]);
void printMatrix(float **, int, int);
float **createMatrix(int, int);
void fillMatrix(float **, const int, const int);
void freeMatrix(float **);
void rowDistribution(float **, float **, const int *, int);


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
	float **matrix_A, **matrix_B;

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
		fillMatrix(matrix_A, MATRIX_DIM[0], MATRIX_DIM[1]);
		fillMatrix(matrix_B, MATRIX_DIM[1], MATRIX_DIM[0]);
		#if STDOUT
			printf("[FIRST MATRIX]:\n");
			printMatrix(matrix_A, MATRIX_DIM[0], MATRIX_DIM[1]);
			printf("[SECOND MATRIX]:\n");
			printMatrix(matrix_B, MATRIX_DIM[1], MATRIX_DIM[0]);
			fflush(stdout);
		#endif // STDOUT
	}
	
	rowDistribution(matrix_A, matrix_B, MATRIX_DIM, process_rank);

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

void fillMatrix(float **matrix, const int height, const int width) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			matrix[i][j] = (float)rand() / (float)(RAND_MAX / 10); // float between 0 and 10
		}
	}
}

void freeMatrix(float **matrix) {
	free(matrix[0]);
	free(matrix);
}

void rowDistribution(float **matrix_A, float **matrix_B, const int * dimensions, int process_rank) {
	int *sendcounts_rows = (int *)malloc(dimensions[0] * sizeof(int));
	int *displs_rows = (int *)malloc(dimensions[0] * sizeof(int));
	int *recvcounts = (int *)malloc(dimensions[0] * sizeof(int));
	int *recvdispls = (int *)malloc(dimensions[0] * sizeof(int));
	float *row = (float *)malloc(dimensions[1] * sizeof(float));
	float *column_result = (float *)malloc(dimensions[0] * sizeof(float));
	float **result;

	/* Distribute B */
	MPI_Bcast(matrix_B[0], dimensions[0] * dimensions[1], MPI_FLOAT, 0, MPI_COMM_WORLD);

	/* Distribute all rows of A */
	for (int i = 0; i < dimensions[0]; i++) {
		sendcounts_rows[i] = dimensions[1];
		displs_rows[i] = dimensions[1] * i;
		recvcounts[i] = dimensions[0];
		recvdispls[i] = dimensions[0] * i;
	}
	MPI_Scatterv(matrix_A[0], sendcounts_rows, displs_rows, MPI_FLOAT, row, dimensions[1], MPI_FLOAT, 0, MPI_COMM_WORLD);
	free(sendcounts_rows);
	free(displs_rows);
	if (process_rank == 0) {
		freeMatrix(matrix_A);
	}

	/* Multiplicate the each column of B with the row */
	#if STDOUT
		printf("[%d]:\n", process_rank);
	#endif // STDOUT
	for (int i = 0; i < dimensions[0]; i++) {
		float addition = matrix_B[dimensions[1] - 1][i] + row[dimensions[0] - 1];
		for (int j = 0; j < dimensions[1] - 1; j++) {
			addition += matrix_B[j][i] + row[j];
			#if STDOUT
				printf("%.2f * %.2f + ", matrix_B[j][i], row[j]);
			#endif // STDOUT
		}
		column_result[i] = addition;
		#if STDOUT
			printf("%.2f * %.2f = %.2f\n", matrix_B[dimensions[1] - 1][i], row[dimensions[0] - 1], addition);
		#endif // STDOUT
	}

	if (process_rank == 0) {
		result = createMatrix(dimensions[0], dimensions[0]);
	}


}