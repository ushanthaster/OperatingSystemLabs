#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

main(int argc, char* argv[])
{
    int my_rank;       
    int NUM_PROCS;             
    int source;        
    int dest;          
    int tag = 0;       
    int NUM_POINTS=1000;
    float x[1000];
    float CONVERGENCE_RATIO=.001;
    MPI_Status status;       
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCS);
    float *my_points;
    float left = 0, right =0,  l,r,new_val,conv;
    
    if (my_rank==0)
    {

        left = 10;
        int i;
        for (i = 0; i<=NUM_POINTS/2; i++)
        {
            x[i]=0;
            x[NUM_POINTS-i] = 0;
        }
        printf("Array Loaded...\n" );
    }
    else if (my_rank == NUM_PROCS -1)
    {
        right = 250;
    }

    NUM_POINTS = ceil(1000/NUM_PROCS);
    my_points = (float*)malloc(NUM_POINTS*sizeof(float));
    printf("Scatter started for Proc %d \n", my_rank);
    MPI_Scatter(&x, NUM_POINTS, MPI_FLOAT, my_points, NUM_POINTS, MPI_FLOAT, 0, MPI_COMM_WORLD);
    printf("Scatter complete, Proc %d got %d points\n", my_rank,NUM_POINTS);
    int iteration =0, converged_points = 0;
    int converged = 0;
    
    while (!converged && iteration < 10000)
    {
        converged_points = 0;
        
        if (my_rank % 2 == 0)
        {
            if (my_rank!=NUM_PROCS-1) MPI_Send(&my_points[NUM_POINTS-1], 1, MPI_FLOAT, my_rank + 1, 1,MPI_COMM_WORLD);
            if (my_rank!=0) MPI_Send(&my_points[0], 1, MPI_FLOAT, my_rank - 1, 2,MPI_COMM_WORLD);
            if (my_rank!=0) MPI_Recv(&left, 1, MPI_FLOAT, my_rank - 1, 1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if (my_rank!=NUM_PROCS-1)  MPI_Recv(&right, 2, MPI_FLOAT, my_rank + 1, 2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            
        }
        if (my_rank % 2 != 0)
        {
            if (my_rank!=0) MPI_Recv(&left, 1, MPI_FLOAT, my_rank - 1, 1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if (my_rank!=NUM_PROCS-1)  MPI_Recv(&right, 2, MPI_FLOAT, my_rank + 1, 2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if (my_rank!=NUM_PROCS-1)MPI_Send(&my_points[NUM_POINTS-1], 1, MPI_FLOAT, my_rank + 1, 1,MPI_COMM_WORLD);
            if (my_rank!=0) MPI_Send(&my_points[0], 1, MPI_FLOAT, my_rank - 1, 2,MPI_COMM_WORLD);
            
        }
        
        int p;
        for (p = 0; p<NUM_POINTS; p++)
        {
            if (p==0)
                l=left;
            else
                l=my_points[p-1];
            
            if (p == (NUM_POINTS - 1))
                r=right;
            else
                r=my_points[p+1];
                
            new_val = 0.5 * (l + r);
            conv = abs(new_val - my_points[p]);
                       
            my_points[p] = new_val;
            
            if ( conv <= CONVERGENCE_RATIO )
                converged_points++;
        }
        
        iteration++;
        
        if (iteration % 500 == 0)
        {
           
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Gather(my_points, NUM_POINTS, MPI_FLOAT, &x, NUM_POINTS, MPI_FLOAT, 0,  MPI_COMM_WORLD);
            if (my_rank==0)
            {
                int t;       
                printf(" 10 |");
                for (t=0; t<10; t++)
                {
                    printf(" %f |",x[t*100]);
                }
                printf(" 250 ;\n\n\n");
            }
        }
    }
    printf("Finished...\n");
    MPI_Finalize();
} 

