#include <mpi.h>
#include <stdio.h>
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
	int data; // The number to be distributed to each process.

	/* The Main program */
	if (process_rank == 0) {
		int read = -1; // The number of characters read in scanf, should be 2 (number and enter).
		char enter = '\0'; // The last character read in scanf, should be a new line (\n).
		while (read != 2 || enter != '\n') {
			printf("\nIntroduce el dato: ");
			fflush(stdout);
			read = scanf("%d%c", &data, &enter);
			fflush(stdin);
		}
		for (int i = 1; i < processes_count; i++) { // Send a message to each process.
			MPI_Send(&data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

	}
	else {
		MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpi_status);
		printf("Proceso %d ha recibido el mensaje: %d\n", process_rank, data);
	}

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}