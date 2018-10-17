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
	MPI_Info mpi_info;
	MPI_File mpi_file;
	MPI_Status mpi_status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	MPI_Get_processor_name(cpu_name, &cpu_name_length);

	/* Main program variables */
	const int CHAR_REPEAT = 5;
	const char * FILE_DIR = "arpa5.txt";
	const int OFFSET = sizeof(char) * CHAR_REPEAT;
	char * write_data = (char *)emptyMatrix(1, CHAR_REPEAT + 1, sizeof(char));
	char * read_data = (char *)emptyMatrix(1, CHAR_REPEAT + 1, sizeof(char));
	
	/* The Main program */
	for (int i = 0; i < CHAR_REPEAT; i++) {
		write_data[i] = process_rank + 48;
	}
	write_data[CHAR_REPEAT] = '\0';
	MPI_File_open(MPI_COMM_WORLD, FILE_DIR, MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &mpi_file);
	MPI_File_set_view(mpi_file, OFFSET, MPI_CHAR, MPI_CHAR, "internal", MPI_INFO_NULL);
	MPI_File_write_at(mpi_file, OFFSET * process_rank, write_data, CHAR_REPEAT, MPI_CHAR, &mpi_status);
	MPI_File_read_at(mpi_file, OFFSET * process_rank, read_data, CHAR_REPEAT, MPI_CHAR, &mpi_status);
	MPI_File_close(&mpi_file);
	read_data[CHAR_REPEAT] = '\0';
	printf("[%d] %s vs %s", process_rank, write_data, read_data);
	fflush(stdout);

	/* Finalize the program */
	MPI_Finalize();
	return 0;
}