//this lab is for calculating defenate integrals
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

void printit();

int main ()
{
    //alright making variables for this
    //
    int size, rank, counter,scounter;
    double pi=3.141592653589793238462643;
    double temp_pi, calc_pi, int_size, part_sum, x;
    char response = 'y';

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int loopCounter = rank*(counter/size);
    //printf("naruto");
    if(rank==0)
    {
       printit();
    }

    while(response == 'y')
    {
	if(rank==0)
        {
	     printf ("---------------------------------\n");
	     printf("Enter the number of of intervals: ");
	     scanf("%d",&counter);
        }
	else
	     counter=0;
	MPI_Bcast(&counter,1,MPI_INT,0,MPI_COMM_WORLD);

 	if(counter ==0)
	     break;
	else
	{
	     int_size=1.0/(double)counter;
	     part_sum=0.0;
             for(scounter=loopCounter; scounter < (rank+1)*loopCounter; scounter++)
	     {
		x=int_size*((double)scounter-.05);
		part_sum+=(4.0/(1.0+x*x));
	     }
	     temp_pi=int_size*part_sum;
             MPI_Reduce(&temp_pi,&calc_pi,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	     if (rank == 0)
	     {                                                    
            	printf("pi is approximately %f and real pi is %f.  Error margin is %f \n\n",calc_pi,pi,(calc_pi - pi));
	     }

	}
	
        if(rank ==0)
	{
	  printf("\nCompute with different intervals (y/n): ");
	  scanf("%c",&response);
	}
    }

    MPI_Finalize();

   return 0;
}

void printit()
{
	printf( "\n*********************************\nWelcome to the pi calculator!\nprogrammer: K. Spry\nYou set the number of divisions\nfor estimating the  integral: \n\tf(x)=4/(1+x^2)\n*********************************\n");

}
