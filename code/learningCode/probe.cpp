#include"stdafx.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<iostream>

using namespace std;

int main(int args, char* argv[])
{
	MPI_Init(NULL, NULL);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	if (world_size != 2) {
		fprintf(stderr, "Must use two processes for this example\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int number_amount;
	if (world_rank == 0) {
		const int MAX_NUMBERS = 100;
		const char* str = new char[5];
		str = "hello";
		// Pick a random amont of integers to send to process one
		srand(time(NULL));
		//number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
		// Send the amount of integers to process one
		MPI_Send(str, 6, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
		//printf("0 sent %d numbers to 1\n", number_amount);
	}
	else if (world_rank == 1)
	{
		MPI_Status status;
		// Probe for an incoming message from process zero
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		// When probe returns, the status object has the size and other
		// attributes of the incoming message. Get the size of the message.
		MPI_Get_count(&status, MPI_CHAR, &number_amount);
		// Allocate a buffer just big enough to hold the incoming numbers
		char* number_buf = (char*)malloc(sizeof(char) * number_amount); //动态申请内存
																		// Now receive the message with the allocated buffer
		MPI_Recv(number_buf, number_amount+1, MPI_INT, 0, 0, MPI_COMM_WORLD,
			MPI_STATUS_IGNORE); // 注意这里要多一个char的位置。
		cout << number_buf << ".." << endl;
		free(number_buf);
	}
	MPI_Finalize();
	return 0;
}