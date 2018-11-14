#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	/* MPI initializers and variables */
	int process_rank, processes_count, cpu_name_length;
	char cpu_name[128];
	MPI_Status mpi_status;
	MPI_Comm mpi_intercomm;
	MPI_Comm mpi_intracomm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* Main program variables */
	const char * CHILD_COMMAND = "./out/Practica8/child.exe";
	const unsigned int CHILD_COUNT = 6;
	const char * PARENT_MESSAGE = "Hello from parent";

	/* The Main program */
	MPI_Comm_spawn(CHILD_COMMAND, MPI_ARGV_NULL, CHILD_COUNT, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &mpi_intercomm, MPI_ERRCODES_IGNORE);
	MPI_Intercomm_merge(mpi_intercomm, 0, &mpi_intracomm);
	printf("Parent [%d] = %s", process_rank, PARENT_MESSAGE);
	MPI_Bcast((void*)PARENT_MESSAGE, strlen(PARENT_MESSAGE), MPI_CHAR, 0, mpi_intracomm);
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}