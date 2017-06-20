#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int numnodes,myid,mpi_err;
#define mpi_root 0

void init_it(int  *argc, char ***argv);

void init_it(int  *argc, char ***argv) {
	mpi_err = MPI_Init(argc,argv);
    mpi_err = MPI_Comm_size( MPI_COMM_WORLD, &numnodes );
    mpi_err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
}

int main(int argc,char *argv[]){

	int *sray,*displacements,*counts,*allray;
	int size,mysize,i;
	int x[1000],xnew[1000];
    x[0]=10;
    x[1000]=250;
	init_it(&argc,&argv);
	mysize=myid+1;

	if(myid == mpi_root){
		counts=(int*)malloc(numnodes*sizeof(int));
		displacements=(int*)malloc(numnodes*sizeof(int));
	}

	mpi_err = MPI_Gather((void*)mysize,1,MPI_INT, 
					     (void*)counts,  1,MPI_INT, 
					     mpi_root,MPI_COMM_WORLD);

	if(myid == mpi_root){
		
		for( i=1;i<1000;i++){
			xnew[i]=(x[i-1]+x[i+1])/2.0;
		}
	
	allray=(int*)malloc(sizeof(int)*mysize);
	mpi_err = MPI_Scatterv(sray,counts,displacements,MPI_INT, 
	                       allray, mysize,           MPI_INT,
	                 mpi_root,                                
	                 MPI_COMM_WORLD);
	                
	for(i=0;i<1000;i+50)
	    x[i]=xnew[i];
		printf("%d ",xnew[i]);
	printf("\n");

    mpi_err = MPI_Finalize();
}
