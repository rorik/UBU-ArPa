#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa-matrix.h>
#include <arpa-utils.h>
int main(int argc, char *argv[])
{
	/* MPI initializers and variables */
	int process_rank, processes_count;
	MPI_Comm mpi_intercomm;
	MPI_Comm mpi_intracomm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	/* Main program variables */
	const int PARENT_MESSAGE_LENGTH = 17;
	const int SIBLING_MESSAGE_LENGTH = 19;
	char * parent_message = (char *)malloc((1 + PARENT_MESSAGE_LENGTH) * sizeof(char));
	char * sibling_message = (char *)malloc(SIBLING_MESSAGE_LENGTH * sizeof(char));

	/* The Main program */
	MPI_Comm_get_parent(&mpi_intercomm);
	MPI_Intercomm_merge(mpi_intercomm, 1, &mpi_intracomm);
	if (process_rank == 0) {
		sibling_message = "Hello from sibling";
	}
	MPI_Bcast((void*)parent_message, PARENT_MESSAGE_LENGTH, MPI_CHAR, 0, mpi_intracomm);
	parent_message[PARENT_MESSAGE_LENGTH] = '\0';
	printf("Child [%d] PARENT = %s\n", process_rank, parent_message);
	MPI_Bcast((void*)sibling_message, SIBLING_MESSAGE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
	printf("Child [%d] SIBLING = %s\n", process_rank, sibling_message);
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}