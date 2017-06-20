#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <mpi.h>

void butterfly_barrier(int p, int k, int rank){
    int i, j, s, out, x = 1;
    MPI_Status  status;
    for (i = 0; i < k; i++){
        s = 0;
        for(j = 0; j < p; j++){
            if(j % (int)pow(2, i+1) == 0){
                s = j;
            }
            out = ((j + (int)pow(2, i)) % (int)pow(2, i+1)) + s;
            if(rank == j){
                MPI_Send(&x, 1, MPI_INT, out, 10, MPI_COMM_WORLD);
            }
            if(rank == out){
                MPI_Recv(&x, 1, MPI_INT, j, 10, MPI_COMM_WORLD, &status);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, np, i, j, k;
    double t1, t2;
    MPI_Status  status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    k = log10(np) / log10(2);
    t1 = MPI_Wtime();
    // call to your butterfly function
    butterfly_barrier(np, k, rank);
    t2 = MPI_Wtime();
    printf("Elapsed time = %f seconds\n", t2-t1);
    MPI_Finalize();
    return 0;
    }
