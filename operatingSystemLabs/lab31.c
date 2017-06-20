#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

main(int argc, char* argv[]) {
    int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         source;        /* rank of sender       */
    int         dest;          /* rank of receiver     */
    int         i;
    int         tag = 0;       /* tag for messages     */
    double      *buffer;       /* storage for random numbers  */
    MPI_Status  status;        /* return status for    */
    double *x;
    int buff_size = 2000; 
    int num;
    double part_sum = 0;
    double sum = 0;
    double result = 0;

    /* Start up MPI */
    MPI_Init(&argc, &argv);
    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	
	//send the number
    if(my_rank == 0)
    {
        buffer = malloc(p*buff_size*sizeof(double));  //rand number
        srand(1);
        printf("Buffer: %d\n", p*buff_size);
        for(i = 0; i<p*buff_size; i++)
        {
            buffer[i] = (1.0 * rand()) / RAND_MAX;   
			printf("%f\n",buffer[i]);
        }        
    }
	//Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
    x = malloc(buff_size*sizeof(double));
    //printf("Value of x: %d\n", x);
	
	//send msg to other processes
    MPI_Scatter(buffer, buff_size, MPI_DOUBLE, x, buff_size, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
    part_sum=0.0;
    //calcualte SQRT of integral
    for(i = 0; i < buff_size; i++)
    {
        part_sum += sqrt((1 - (x[i]*x[i]) / (p*buff_size)));
    }

    MPI_Reduce(&part_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); //receive

    result = sum; //return SUM
	
    if(my_rank == 0)
    {
		printf("The computed result is: %f, The correct result is: %f with N = 1\n",result, 3.1415/1 );
		printf("The computed result is: %f, The correct result is: %f with N = 2\n",result, 3.1415/2 );
        printf("The computed result is: %f, The correct result is: %f with N = 4\n",result, 3.1415/4 );
		printf("The computed result is: %f, The correct result is: %f with N = 6\n",result, 3.1415/6 );
		printf("The computed result is: %f, The correct result is: %f with N = 8\n",result, 3.1415/8 );
		printf("The computed result is: %f, The correct result is: %f with N = 10\n",result, 3.1415/10 );
		
    }

    /* Shut down MPI */
    MPI_Finalize();
}
