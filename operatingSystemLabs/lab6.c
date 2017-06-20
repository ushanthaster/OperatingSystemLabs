#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mpi.h"

int main (void)
{
  int mySize, myRank,counter, buffer,sortPoint,temp;
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
  MPI_Comm_size(MPI_COMM_WORLD,&mySize);
  MPI_Status status;
  int *myArray;
  int max_int=1000;
  if(myRank==0)
  {
    printf("Printing random generated numbers \n");
    srand(mySize);
    myArray=malloc(mySize*sizeof(int));
    for(counter=0; counter < mySize; counter++)
    {
       myArray[counter]=rand()%max_int;
       printf("%6d",myArray[counter]);
    }
    printf("\n-----------------------------\n");
  }
  if(myRank==0)
  {
     buffer=myArray[0];
     for(counter=1; counter < mySize; counter++)
     {
	temp=myArray[counter];
        if(buffer < temp)
	{
	   myArray[counter-1]=buffer;
	   myArray[counter]=temp;
	   sortPoint=buffer;
	   MPI_Send(&temp,1,MPI_INT,counter,0,MPI_COMM_WORLD);
	   temp=buffer;
	   printf("%d",buffer);
	}
	/*else
	  MPI_Recv(&sortPoint,1,MPI_INT,counter-1,myRank*10,MPI_COMM_WORLD,&status)*/
     
    }
  }
  MPI_Finalize();
}
