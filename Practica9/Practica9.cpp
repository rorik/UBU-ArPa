#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>

#define STDOUT false //show matrix output
#define DEBUG false //show debug output

int *parseArgs(int, char *[]);
void printMatrix(float **, int, int);
float **createMatrix(int, int);
void fillMatrix(float **, const int, const int);
void freeMatrix(float **);
float **rowDistribution(float **, float **, const int *, int, int);
void printif(char *format...);
void printif_debug(char *format...);


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
	float **matrix_A, **matrix_B, **result;
	double start_time;

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
		fillMatrix(matrix_A, MATRIX_DIM[0], MATRIX_DIM[1]);
		fillMatrix(matrix_B, MATRIX_DIM[1], MATRIX_DIM[0]);
		printif("[FIRST MATRIX]:\n");
		printMatrix(matrix_A, MATRIX_DIM[0], MATRIX_DIM[1]);
		printif("[SECOND MATRIX]:\n");
		printMatrix(matrix_B, MATRIX_DIM[1], MATRIX_DIM[0]);
		fflush(stdout);
		start_time = MPI_Wtime();
	}
	
	result = rowDistribution(matrix_A, matrix_B, MATRIX_DIM, process_rank, processes_count);

	freeMatrix(matrix_A);
	freeMatrix(matrix_B);

	if (process_rank == 0) {
		freeMatrix(result);
		printf("Finished in %1.3lf seconds.", (MPI_Wtime() - start_time));
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
			matrix[i][j] = (float)rand() / (float)(RAND_MAX / 5); // float between 0 and 5
		}
	}
}

void freeMatrix(float **matrix) {
	free(matrix[0]);
	free(matrix);
}

float **rowDistribution(float **matrix_A, float **matrix_B, const int * dimensions, int process_rank, int processes_count) {
	int *sendcounts = (int *)malloc(dimensions[0] * sizeof(int));
	int *displs = (int *)malloc(dimensions[0] * sizeof(int));
	float *row = (float *)malloc(dimensions[1] * sizeof(float));
	float **result = NULL;

	/* Distribute B */
	MPI_Bcast(matrix_B[0], dimensions[0] * dimensions[1], MPI_FLOAT, 0, MPI_COMM_WORLD);

	/* Distribute all rows of A */
	if (process_rank == 0) {
		int current_row = 0;
		int received_rows = 0;
		result = createMatrix(dimensions[0], dimensions[0]);
		for (; current_row < ( processes_count - 1); current_row++) {
			if (current_row < dimensions[0]) {
				MPI_Send(matrix_A[current_row], dimensions[1], MPI_FLOAT, current_row + 1, current_row, MPI_COMM_WORLD);
			} else {
				MPI_Send(NULL, 0, MPI_FLOAT, current_row + 1, dimensions[0], MPI_COMM_WORLD);
			}
		}
		while (received_rows < dimensions[0]) {
			MPI_Status status;
			float *row_result = (float *)malloc(dimensions[0] * sizeof(float));
			MPI_Recv(row_result, dimensions[0], MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			received_rows++;
			result[status.MPI_TAG] = row_result;
			if (current_row < dimensions[0]) {
				MPI_Send(matrix_A[current_row], dimensions[1], MPI_FLOAT, status.MPI_SOURCE, current_row, MPI_COMM_WORLD);
				current_row++;
			}
			else {
				MPI_Send(NULL, 0, MPI_FLOAT, status.MPI_SOURCE, dimensions[0], MPI_COMM_WORLD);
			}
		}
		printif("[RESULT MATRIX]:\n");
		printMatrix(result, dimensions[0], dimensions[0]);
	} else {
		while (true) {
			MPI_Status status;
			float *row_result = (float *)malloc(dimensions[0] * sizeof(float));
			MPI_Recv(row, dimensions[1], MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (status.MPI_TAG == dimensions[0]) {
				printif_debug("[%d] Finished", process_rank);
				break;
			}
			printif_debug("[%d] row %d:\n", process_rank, status.MPI_TAG);
			for (int i = 0; i < dimensions[0]; i++) {
				row_result[i] = 0;
				for (int j = 0; j < dimensions[1]; j++) {
					row_result[i] += matrix_B[j][i] * row[j];
					printif_debug("%.2f*%.2f + ", matrix_B[j][i], row[j]);
				}
				printif_debug("\b\b= %5.2f\n", row_result[i]);
			}
			free(row);
			MPI_Send(row_result, dimensions[0], MPI_FLOAT, 0, status.MPI_TAG, MPI_COMM_WORLD);
		}
	}

	return result;
}

void printMatrix(float **matrix, const int height, const int width) {
#if STDOUT
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%6.2f ", matrix[i][j]);
		}
		printf("\n");
	}
#endif // STDOUT
}

void printif(char *format...) {
#if STDOUT
	printf(format);
#endif // STDOUT
}

void printif_debug(char *format...) {
#if DEBUG
	printf(format);
#endif // DEBUG
}