// HelloWorld_MPI.cpp: 定义控制台应用程序的入口点。
//

#include<stdio.h>
#include"stdafx.h"
#include<mpi.h>

int main(int argc, char *argv[])
{

	int myid, numprocs, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int test_init = false;
	MPI_Initialized(&test_init);
	if (test_init == 0)
		printf("Before MPI_init, the working environment has not initialized.\n");
	else
		printf("Before MPI_init, the working environment has initialized.\n");
	//用于并行环境初始化，MPI系统将通过argc、argv得到命令行参数
	MPI_Init(&argc, &argv);
	MPI_Initialized(&test_init);
	if (test_init == 0)
		printf(" After MPI_init, the working environment has not initialized.\n");
	else
		printf(" After MPI_init, working environment has initialized.\n");

	//当MPI初始化后，每一个活动进程变成了一个叫MPI_COMM_WORLD的通信域中的成员
	//得到本进程在通信空间中的rank值，即在组中的逻辑编号(相当于进程ID)
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	//获得进程个数size
	//获得通信子Comn中规定的group包含的进程的数量
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	//function head：MPI_Get_processor_name(char* name, int* resultlen)
	//获取处理器的名称，在返回的name中存储所在处理器的名称；resultlen存放所返回名字所占字节
	//namelen传递的是引用
	MPI_Get_processor_name(processor_name, &namelen);

	if (myid == 0) printf("number of processes: %d\n", numprocs);
	printf("%s: Hello world from process %d \n", processor_name, myid);
	MPI_Finalize(); // 退出MPI系统  
	return 0;


}
