#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	/* MPI initializers and variables */
	int process_rank, processes_count, cpu_name_length;
	char cpu_name[128];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* The Main program */
	printf("[Maquina %s]> Proceso %d de %d: Hola Mundo!\n", cpu_name, process_rank, processes_count);
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}