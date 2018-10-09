#include <mpi.h>
#include <stdio.h>
/* Made by Roderick D. for Arquitecturas Paralelas - UBU - 2018/2019
 * https://rorik.me | https://github.com/rorik | about@rorik.me
 * This software is protected under GNU General Public License v3.0,
 * for more information check https://github.com/rorik/UBU-ArPa
 */
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

	/* The Main program */
	printf("[%d] Hello World!", process_rank);
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}