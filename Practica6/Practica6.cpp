#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/* Method declarations */
long long unsigned int factorial(int, bool*);

/* Method definitions */
int main(int argc, char *argv[]) {
	/* MPI initializers and variables */
	int process_rank;
	MPI_Status mpi_status;
	MPI_Request mpi_request_main;
	MPI_Request mpi_request_flag;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	/* Main program variables */
	int number; // The number to get it's factorial
	long long unsigned int result; // The result of the factorial operation
	bool overflow; // The flag that indicates if the result has overflowed

	/* The Main program */
	if (process_rank == 0) {
		while (true) {
			int read = -1; // The number of characters read in scanf, should be 2 (number and enter)
			char enter = '\0'; // The last character read in scanf, should be a new line (\n)
			number = -1; // The number to be read
			while (read != 2 || enter != '\n' || number < 0) { // Loop until a valid number is provided
				printf("Introduce el numero: ");
				fflush(stdout);
				read = scanf("%d%c", &number, &enter);
				fseek(stdin, 0, SEEK_END); // Clean stdin
			}
			MPI_Isend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &mpi_request_main); // Send the number to 1
			printf("Esperando resultado...");
			fflush(stdout);
			MPI_Irecv(&result, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD, &mpi_request_main); // Get the result from 1
			MPI_Irecv(&overflow, 1, MPI_C_BOOL, 1, 1, MPI_COMM_WORLD, &mpi_request_flag); // Get the overflow flag from 1
			MPI_Wait(&mpi_request_main, &mpi_status); // Wait until the result has arrived
			MPI_Wait(&mpi_request_flag, &mpi_status); // Wait until the overflow flag has arrived
			printf("\rResultado: %d! = %llu %s\n", number, result, overflow ? "(!!! - OVERFLOW - !!!)" : "    ");
			fflush(stdout);
		}
	}
	else if (process_rank == 1) {
		while (true) {
			MPI_Irecv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpi_request_main); // Get the number from 1
			MPI_Wait(&mpi_request_main, &mpi_status); // Wait until the number has arrived
			result = factorial(number, &overflow);
			MPI_Isend(&result, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &mpi_request_main); // Send the result to 1
			MPI_Isend(&overflow, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD, &mpi_request_flag); // Send the overflow flag to 1
		}
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}


/*                                                    *\
|* factorial(int, bool)                               *|
|* Returns the factorial of a given number.           *|
|* Params:                                            *|
|*   - number: the number to be factorialized.        *|
|*   - overflow_flag: a bool flag that indicates      *|
|*       if the calculation has overflowed.           *|
|* Return:                                            *|
|*   - long long unsigned int:                        *|
|*       The result of the calculation or             *|
|*       ULLONG_MAX if overflowed.                    *|
\*                                                    */
long long unsigned int factorial(int number, bool *overflow_flag) {
	if (number < 0) {
		printf("Non-negative integer expected, but got %d instead.", number);
		exit(1);
	}
	long long unsigned int result = 1;
	for (int i = 1; i <= number; i++) {
		if (ULLONG_MAX / i < result) { // Return UULONG_MAX if overflowed
			*overflow_flag = true;
			return ULLONG_MAX;
		}
		result *= i;
	}
	*overflow_flag = false;
	return result;
}