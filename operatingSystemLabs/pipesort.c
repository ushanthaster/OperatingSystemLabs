#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int tag = 1;

void Compare(int myid, int step, int *small, int *recd)
{
	if(step == 0)
		*small = *recd;
	else
		if(*recd > *small)
		{
			MPI_Send(recd, 1, MPI_INT, myid + 1, tag, MPI_COMM_WORLD);
			printf("Node %d sends %d to %d.\n", myid, *recd, myid + 1);
				
		}
		else
		{
			MPI_Send(small, 1, MPI_INT, myid + 1, tag, MPI_COMM_WORLD);
			printf("Node %d sends %d to %d.\n", myid, *small, myid + 1);
			*small = *recd;
		}
}	

void Collect(int myid, int p, int small, int *sorted)
{
	MPI_Status status;
	int k;
	if(myid == 0)
	{
		sorted[0] = small;
		for(k = 1; k < p; k++)
			MPI_Recv(&sorted[k], 1, MPI_INT, k, tag, MPI_COMM_WORLD, &status);
		printf("The sorted sequence : ");
		for(k = 0; k < p; k++) printf(" %d", sorted[k]);
		printf("\n");
	}
	else
		MPI_Send(&small, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
}

int main (int argc, char *argv[])
{
	int *n;
	int i;
	int p;
	int j;
	int g;
	int s = 0;
	
	MPI_Init(&argc, &argv);
	MPI_Status status;
	
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &i);

	if(i == 0) 
	{
		n = (int*)calloc(p,sizeof(int));
		srand(time(NULL));
		for(j = 0; j < p; j++) n[j] = rand() % 100;
		printf("The %d numbers to sort : ", p);
		for(j = 0; j < p; j++) printf(" %d", n[j]);
			printf("\n");
	}

	for(j = 0; j < p - i; j++) 
		if(i == 0)
		{
			g = n[j];
			printf("Manager gets %d.\n",n[j]);
			Compare(i, j, &s, &g);
		}
		else
		{
			MPI_Recv(&g, 1, MPI_INT, i - 1, tag, MPI_COMM_WORLD, &status);
			printf("Node %d receives %d.\n", i, g);
			Compare(i, j, &s, &g);
		}

	MPI_Barrier(MPI_COMM_WORLD);
		
	Collect(i, p, s, n);

	MPI_Finalize();
	return 0;
}

