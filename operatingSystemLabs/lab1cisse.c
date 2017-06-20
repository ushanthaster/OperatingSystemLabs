#include <stdio.h>
#include "mpi.h"

int main(int argc,char *argv[]){
	int np, rank,controler;
	char data[30];
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	for(controler = 0; controler < np; controler++){
		if(rank == 0){
			sprintf(data, "Greetings from Process %d", rank);
			MPI_Send(data, 30, MPI_CHAR, np-1, 0, MPI_COMM_WORLD);
			MPI_Recv(data, 30, MPI_CHAR, 1, 10, MPI_COMM_WORLD, &status);
			printf("%s\n", data);
			}else{
				if(rank == np - 1){
					MPI_Recv(data, 30, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
				}else{
					MPI_Recv(data, 30, MPI_CHAR, rank + 1, (rank + 1) * 10, MPI_COMM_WORLD, &status);
				}
		printf("%s\n", data);
				sprintf(data, "Greetings from Process %d", rank);
				MPI_Send(data, 30, MPI_CHAR, rank-1, rank * 10, MPI_COMM_WORLD);
			}
		}
		
		MPI_Finalize();
		return 0;
}

