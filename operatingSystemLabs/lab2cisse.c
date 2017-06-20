#include "stdio.h"
#include "mpi.h"

main (int argc, char* argv[]){

	int x;
	int rank;
	int np, sum;

	MPI_Status status;

	MPI_Init(NULL,NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank) ;

	if(rank==0)

		MPI_Send(&x,1,MPI_INT, 1, 0, MPI_COMM_WORLD);
		
		if(rank==1)
				MPI_Recv(&x, 1, MPI_INT, 0,0,MPI_COMM_WORLD, &status);

			MPI_Bcast(&x,1,MPI_INT, 1, MPI_COMM_WORLD);
				if(rank==2)
				printf("x= %d\n",x);

			MPI_Reduce(&rank, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

			if(rank==0)
			printf("Sum = %d\n", sum);


			MPI_Finalize();
		}

