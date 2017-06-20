
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

// A function that will return the size of a bucket without -1s
int newBucketSize(int* bucket, int size)
{
	int n = 0, newSize = 0;
	
	for(n = 0; n < size; n++)
	{
		if(bucket[n] != -1 && bucket[n] < 10000)
			newSize++;
	}
	return newSize;
}

// A function to allocate the required memory and initiate a one dimensional array of the given size with the given number
int* allocateAndInitiateOneD(int x, int initialNum)
{
	int runs = 0;
	int* oneArray;

	oneArray = malloc(sizeof(int) * x);
	for(runs = 0; runs < x; runs++)
		oneArray[runs] = initialNum;
		
	return oneArray;
}

// A function to allocate the required memory and initiate a one dimensional array of the given size with the given number
int** allocateAndInitiateTwoD(int x, int y, int initialNum)
{
	int runs = 0;
	int innerRuns = 0;
	int** twoArray;
	
	twoArray = malloc(sizeof(int*) * x);
	for(runs = 0; runs < x; runs++)
		twoArray[runs] = malloc(sizeof(int) * y);
	
	for(runs = 0; runs < x; runs++)
		for(innerRuns = 0; innerRuns < y; innerRuns++)
			twoArray[runs][innerRuns] = initialNum;
	
	return twoArray;
}

// A function to call a selection sort on a bucket of the given size
void sortBucket(int* bucket, int size)
{
	int i, j, iMin, swap;
	for(j = 0; j < size-1; j++)
	{
		iMin = j;
		
		for(i = j + 1; i < size; i++)
		{
			if(bucket[i] < bucket[iMin])
				iMin = i;
		}
		if(iMin != j)
		{
			swap = bucket[j];
			bucket[j] = bucket[iMin];
			bucket[iMin] = swap;
		}
	}
}

// A function to trim -1s from a bucket of the given size
int* trimBucket(int* bucket, int size)
{
	int n = 0, i = 0;
	int* newBucket;
	
	i = newBucketSize(bucket, size);
	
	newBucket = allocateAndInitiateOneD(i, -1);
	i = 0;
	
	for(n = 0; n < size; n++)
	{
		if(bucket[n] != -1 && bucket[n] < 10000)
		{
			newBucket[i] = bucket[n];
			i++;
		}
			
	}
	free(bucket);
	return newBucket;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv); // Start up MPI
	
	int process_rank;
	int process_count;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank); // Find process rank
	MPI_Comm_size(MPI_COMM_WORLD, &process_count); // Stores the number of processes
	
	int newBucket_size = 0;
	int runs = 0; // A variable to control loop runs
	int innerRuns = 0; // A variable to control inner loop runs
	int deepRuns = 0; // A variable to control triple nested loops
	int maxNumber = 10000; // Range of numbers to generate
	int listSize = 2520; // Number of elements to sort - 2520 is evenly divisible by all numbers 1-10, if a higher number of processes than 10 is used, find the next number that would be evenly divisible
	int smallBucket_start = 0;
	int smallBucket_end = (maxNumber / process_count);
	int bigBucket_size = (listSize / process_count);
	int smallBucket_size = bigBucket_size;
	
	int* trimmedBigBucket;
	int** smallBucket_ranges = allocateAndInitiateTwoD(process_count, 2, -1);
	int* smallBucket_count = allocateAndInitiateOneD(process_count, 0); // Array to keep track of the number of elements in the local small bucket
	int* theList = allocateAndInitiateOneD(listSize, -1); // Array used in process 0 to create the random numbers and distribute them
	int* bigBucket = allocateAndInitiateOneD(bigBucket_size, -1); // Array to hold all numbers in the process
	int** smallBuckets = allocateAndInitiateTwoD(process_count, smallBucket_size, -1); // Array to hold small buckets
	int** smallBucketsAllToAll = allocateAndInitiateTwoD(process_count, smallBucket_size, -1); // Array to hold small buckets post all to all
	
	// Find lower limit of each bucket (lowest number it will hold)
	for(runs = 0; runs < process_count; runs++)
	{
		smallBucket_ranges[runs][0] = smallBucket_start;
		smallBucket_start += (maxNumber / process_count);
	}
	
	// Find the upper limit of each bucket (highest number it will hold)
	for(runs = 0; runs < process_count; runs++)
	{
		smallBucket_ranges[runs][1] = smallBucket_end;
		smallBucket_end += (maxNumber / process_count);
	}
	
	// Generate random numbers in process 0 to fill the list and to be sorted
	if(process_rank == 0) 
	{
		printf("\nGenerating %d random numbers, ranging from 0 to %d.\n", listSize, maxNumber - 1);
		printf("\nThe unsorted list is:\n");
		srand(time(NULL));
		for(runs = 0; runs < listSize; runs++)
		{
			theList[runs] = rand() % maxNumber;
			printf("%d ", theList[runs]);
		}
	}
	
	// Send pieces of theList to each process equal to the list size divided by the number of processes
	MPI_Scatter(theList, bigBucket_size, MPI_INT, bigBucket, bigBucket_size, MPI_INT, 0, MPI_COMM_WORLD); 
	
	// Place the numbers into a bucket of their range
	for(runs = 0; runs < process_count; runs++)
	{
		for(innerRuns = 0; innerRuns < bigBucket_size; innerRuns++)
		{
			if(bigBucket[innerRuns] >= smallBucket_ranges[runs][0] && bigBucket[innerRuns] < smallBucket_ranges[runs][1])
			{
				smallBuckets[runs][smallBucket_count[runs]] = bigBucket[innerRuns];
				smallBucket_count[runs]++;
			}
		}
	}	
	
	// Share all buckets to all processes
	MPI_Alltoall(&smallBuckets[0][0], smallBucket_size, MPI_INT, &smallBucketsAllToAll[0][0], smallBucket_size, MPI_INT, MPI_COMM_WORLD);
	
	bigBucket = allocateAndInitiateOneD(listSize, -1);
	deepRuns = 0;
	
	// Put the small buckets into their respective larger bucket of the same range
	for(runs = 0; runs < process_count; runs++) 
	{
		for(innerRuns = 0; innerRuns < smallBucket_size; innerRuns++)
		{
			bigBucket[deepRuns] = smallBucketsAllToAll[runs][innerRuns];
			deepRuns++;
		}
	}
	
	// Gather all big buckets from processes into one large list
	theList = allocateAndInitiateOneD(listSize * process_count, -1);
	
	MPI_Gather(bigBucket, listSize, MPI_INT, theList, listSize, MPI_INT, 0, MPI_COMM_WORLD);
	
	// Sort the remaining unsorted list
	sortBucket(theList, listSize * process_count);
	
	// Trim the -1s from the list
	newBucket_size = newBucketSize(theList, listSize * process_count);
	
	trimmedBigBucket = allocateAndInitiateOneD(newBucket_size, -1);
	
	trimmedBigBucket = trimBucket(theList, listSize * process_count);
	
	
	if(process_rank == 0)
	{
		printf("\n\nThe sorted list is: \n");
		for(runs = 0; runs < listSize; runs++)
		{
			printf("%d ", trimmedBigBucket[runs]);
		}
		printf("\n");
	}

	// Free the memory used by these arrays
	free(smallBucket_count);
	free(smallBuckets);
	free(bigBucket);
	free(theList);
	free(smallBucketsAllToAll);
	
	MPI_Finalize(); // Shut down MPI
	
	return 0;
}

