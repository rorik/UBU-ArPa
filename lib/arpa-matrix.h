#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void printMatrix(int *matrix, int length, int width, int leftPadding) {
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			printf(" %*d", leftPadding, matrix[i * width + j]);
		}
		printf("\n");
	}
	fflush(stdout);
}

void printMatrix(int *matrix, int length, int width) {
	printMatrix(matrix, length, width, 1);
}

void * emptyMatrix(int length, int width, size_t elementSize) {
	void * matrix = malloc(length * width * elementSize);
	if (matrix == NULL) {
		fprintf(stderr, "Failed to allocate memory to array of size %dx%d\n", width, length);
		fflush(stdout);
		exit(1);
	}
	return matrix;
}

int * emptyIntMatrix(int length, int width) {
	return (int *)emptyMatrix(length, width, sizeof(int));
}

int * randomIntMatrix(int length, int width, int maxValue) {
	int * matrix = emptyIntMatrix(length, width);
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			matrix[i * width + j] = rand() % maxValue;
		}
	}
	return matrix;
}
