#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>
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

	/* The Main program */
	if (process_rank == 0) {
		randomizeSeed();
		printf("[%d] Master", process_rank);
	} else {
		printf("[%d] Hello World!", process_rank);
	}
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}