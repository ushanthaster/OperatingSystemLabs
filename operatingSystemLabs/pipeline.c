/*insertion sort template.c 
 * pipelines*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "mpi.h"

#define MAX_NUM 1000;

int main (void)
{
    int myRank, mySize, source,dest,tag,counter,buffer;
    int *myArray;
    int temp=0,x=0;
    counter=0;
    
    //making mpi calls
    //
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    MPI_Comm_size(MPI_COMM_WORLD,&mySize);
    MPI_Status status;
    if(myRank==0)
    {
       myArray=malloc(mySize*sizeof(int));
       srand(mySize);
       printf("The generated numbers are: \n");
       for(counter = 0; counter < mySize; counter++)
       {
	  myArray[counter]=rand()%MAX_NUM;
	  printf("%6d",myArray[counter]);
	  if((counter+1) % 10 == 0) printf("\n");
       }
    }
    if(myRank==0)
    {
	temp=myArray[0];
	for(counter=1; counter <= mySize; counter++)
	{
	    buffer=myArray[counter];
	    if(temp<buffer)
	    {
		myArray[counter-1]=temp;
		MPI_Send(&temp,1,MPI_INT,counter,tag,MPI_COMM_WORLD);
	        temp=buffer;
	    }
	    else
		MPI_Recv(&temp,1,MPI_INT,counter-1,tag,MPI_COMM_WORLD,&status);
	}
    }

    for(counter=0; counter < mySize; counter++)
        MPI_Recv(&temp,1,MPI_INT,counter,tag,MPI_COMM_WORLD,&status);
    if(myRank==0)
    {
	printf("sorting the numbers \n");
	for(counter=0; counter < mySize; counter++)
	{
	   printf("%6d\n",myArray[counter]);
	   if((counter+1)%10==0)
	     printf("\n");
	}
    }

    MPI_Finalize();
 
}
