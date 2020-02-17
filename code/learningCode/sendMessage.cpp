#include<stdio.h>
#include"stdafx.h" // 其他的include要在这行语句下面
#include<mpi.h>
#include<string.h>

int main(int argc, char *argv[] )
{
	int numprocs, myid, source;
	MPI_Status status;
	char Message[100];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	if (myid != 0)
	{
		strcpy_s(Message, "hello World!");
		// MPI_Send(void* buff,int count, MPI_DataType datatype, int dest,int tag, MPI_COMM_WORLD)
		MPI_Send(Message, strlen(Message) + 1, MPI_CHAR, 0, 99, MPI_COMM_WORLD);
	}
	else
	{
		for (source = 1; source < numprocs; source++)
		{
			// MPI_Send(void* buff,int count, MPI_DataType datatype, int source,int tag, MPI_COMM_WORLD)
			MPI_Recv(Message, 100, MPI_CHAR, source, 99, MPI_COMM_WORLD, &status);
			printf("我是%d号进程，接收到第%d号进程发送的消息：%s\n", myid,source, Message);
		}
	}
	MPI_Finalize();
	return 0;