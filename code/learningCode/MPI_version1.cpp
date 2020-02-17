// Program that computes the standard deviation of an array of elements in parallel using
// MPI_Reduce.
//
#include"stdafx.h"
#include<iostream>
#include<string>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include<functional>
#include<map>

using namespace std;
struct Info
{
	int tag;
	int dest;
};

int addPair(int dest, string key_value_pair);
string queryPair(int dest, string key);
int deletePair(int dest, string key);
int modifyPair(int dest, string key_value_pair);

int main(int argc, char* argv[])
{
	MPI_Init(NULL, NULL);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (world_rank == 0)
	{
		string choice;
Menu:	cout << endl;
		cout << "_________________________option____________________________" << endl << endl;
		cout << "1.add a Pair" << endl;
		cout << "2.delete a Pair" << endl;
		cout << "3.query a Pair" << endl;
		cout << "4.modify a Pair" << endl;
		cout << "5.exit system" << endl;
		cout << "___________________________________________________________" << endl;
		cout << "type in a number between 1 and 5 to choose an operation: " << endl;
		cin >> choice;
		Info info;
		hash<string>hash_fn;
		if (choice == "1")
		{
			cout << "enter the Pair<string,string> you want to add, the format is like 'key value':" << endl;
			string key, value;
			cin >> key >> value;
			string key_value_pair = key + "-" + value;
			hash<string>hash_fn;
			size_t hash = hash_fn(key);
			int dest = hash % 9 + 1; // 设定进程0为主进程，进程1-9为从进程。主进程不存储数据，从进程存储数据
									 //cout << dest << endl;
			info.dest = dest;
			info.tag = 1; //添加Pair的tag为1
			MPI_Bcast(&info, sizeof(info), MPI_INT, 0, MPI_COMM_WORLD);
			//成功插入返回值1，否则返回值0；
			int result = addPair(dest, key_value_pair); 
			if (result)
				cout << "add successfull." << endl;
			else
				cout << "there already is a pair with same key, fail to add." << endl;
			system("pause");
			goto Menu;
		}
		else if (choice == "2")
		{
			cout << "enter the key of the pair you want to delete:" << endl;
			string key;
			cin >> key;
			size_t hash = hash_fn(key);
			int dest = hash % 9 + 1;
			info.dest = dest;
			info.tag = 2; //删除Pair的tag为2
			MPI_Bcast(&info, sizeof(info), MPI_INT, 0, MPI_COMM_WORLD);
			int result = deletePair(dest, key);
			if (result == 1)
				cout << "delete successfully." << endl;
			else
				cout << "there is no such a pair with key '" << key << "'" << endl;
			system("pause");
			goto Menu;

		}
		else if (choice == "3")
		{
			cout << "enter the key of the pair you want to query:" << endl;
			string key;
			cin >> key;
			size_t hash = hash_fn(key);
			int dest = hash % 9 + 1;
			info.dest = dest;
			info.tag = 3; // 查询pair的tag为3
			MPI_Bcast(&info, sizeof(info), MPI_INT, 0, MPI_COMM_WORLD);
			string value = queryPair(dest, key);
			if (value == "not found.")
				cout << "there is not a pair with a key '" << key << "' " << endl;
			else
				cout << "the querying pair is <" << key << "," << value << ">" << endl;
			system("pause");
			goto Menu;

		}
		else if (choice == "4")
		{
			cout << "enter the key of the pair you want to modify:" << endl;
			string key;
			cin >> key;
			cout << "enter the new value :" << endl;
			string value;
			cin >> value;
			string key_value_pair = key + "-" + value;
			size_t hash = hash_fn(key);
			int dest = hash % 9 + 1;
			info.dest = dest;
			info.tag = 4;
			MPI_Bcast(&info, sizeof(info), MPI_INT, 0, MPI_COMM_WORLD);
			int result = modifyPair(dest, key_value_pair);
			if (result == 1)
				cout << "modify successful." << endl;
			else
				cout << "there is no such a pair with key '" << key << "'" << endl;
			system("pause");
			goto Menu;
		}
		else if (choice == "5")
		{
			info.tag = 5;
			info.dest = -1;
			MPI_Bcast(&info, sizeof(info), MPI_INT, 0, MPI_COMM_WORLD);
			cout << "process " << world_rank << " exit." << endl;
		}
		else 
		{
			cout << "your input is illegal." << endl;
			system("pause");
			goto Menu;
		}

	}
	else
	{
		Info info; //用于获取信息
		map<string, string> localMemory; //进程的本地存储
		do
		{
			MPI_Bcast(&info, sizeof(info), MPI_INT, 0, MPI_COMM_WORLD);
			if (world_rank == info.dest)
			{
				if (info.tag == 1)
				{
					MPI_Status status;
					MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
					int number_count;
					MPI_Get_count(&status, MPI_CHAR, &number_count);
					char* pair_buf = (char*)malloc(sizeof(char) * number_count);
					MPI_Recv(pair_buf, number_count+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					string str = pair_buf;
					int pos = str.find('-');
					string key = str.substr(0, pos);
					string value = str.substr(pos + 1);
					pair<map<string,string>::iterator,bool>result = localMemory.insert(pair<string, string>(key, value));
					int success = 1, fail = 0;
					if (result.second == 1)
					{
						cout << "process " << world_rank << " add a new pair <" << key << "," << value << ">" << endl;
						MPI_Send(&success, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
					}
					else {
						MPI_Send(&fail, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
					}
						
				}
				else if (info.tag == 2)
				{
					MPI_Status status;
					MPI_Probe(0, 2, MPI_COMM_WORLD, &status);
					int number_count;
					MPI_Get_count(&status, MPI_CHAR, &number_count);
					char* buf = (char*)malloc(sizeof(char) * number_count);
					MPI_Recv(buf, number_count, MPI_CHAR, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					string key = buf; // 接收key

					int result = localMemory.erase(key); // 0表示元素不在容器中，1表示删除成功。
					if (result == 1)
					{
						cout << "process " << world_rank << " delelte a pair with key '" << key << "'" << endl;
					}
					MPI_Send(&result, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);

				}
				else if (info.tag == 3)
				{
					MPI_Status status;
					MPI_Probe(0, 3, MPI_COMM_WORLD, &status);
					int number_count;
					MPI_Get_count(&status, MPI_CHAR, &number_count);
					char* buf = (char*)malloc(sizeof(char) * number_count);
					MPI_Recv(buf, number_count+1, MPI_CHAR, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					string key = buf;
					map<string, string>::iterator iter = localMemory.find(key);
					string value;
					if (iter != localMemory.end()) {
						value = (*iter).second;
					}
					else
						value = "not found.";
					char* send = (char*)value.c_str();
					MPI_Send(send, value.size()+1, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
				}
				else if (info.tag == 4)
				{
					MPI_Status status;
					MPI_Probe(0, 4, MPI_COMM_WORLD, &status);
					int number_count;
					MPI_Get_count(&status, MPI_CHAR, &number_count);
					char* buf = (char*)malloc(sizeof(char) * number_count);
					MPI_Recv(buf, number_count + 1, MPI_CHAR, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					string key_value_pair = buf;
					int pos = key_value_pair.find('-');
					string key = key_value_pair.substr(0, pos);
					string value = key_value_pair.substr(pos + 1);
					map<string, string>::iterator iter = localMemory.find(key);
					int result;
					if (iter != localMemory.end()) {
						iter->second = value;
						result = 1; //result == 1表示修改成功
						cout << "process " << world_rank << " modify a pair with key '" << key << "'" << endl;
					}
					else {
						result = 0; //result == 0表示键值对不存在
					}
					MPI_Send(&result, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
				}
			}
		} while (info.tag != 5);
		cout << "process " << world_rank << " exit." << endl;
	}
	MPI_Finalize();
	return 0;
}

int addPair(int dest, string key_value_pair)
{
	char* send = (char*)key_value_pair.c_str();
	//传指针即为传引用
	MPI_Send(send, key_value_pair.size()+1, MPI_CHAR, dest, 1, MPI_COMM_WORLD); // tag为1表示添加

	int result; // 返回结果
	MPI_Recv(&result, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	return result;
}

string queryPair(int dest, string key)
{
	char* send = (char*)key.c_str();
	MPI_Send(send, key.size()+1, MPI_CHAR, dest, 3, MPI_COMM_WORLD);

	MPI_Status status;
	MPI_Probe(dest, 3, MPI_COMM_WORLD, &status);
	int number_count;
	MPI_Get_count(&status, MPI_CHAR, &number_count);
	char* cvalue = (char*)malloc(sizeof(char) * number_count);
	MPI_Recv(cvalue, number_count+1, MPI_CHAR, dest, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	string value = cvalue;
	return value;
}

int deletePair(int dest, string key)
{
	char* send = (char*)key.c_str(); //发送key
	MPI_Send(send, key.size() + 1, MPI_CHAR, dest, 2, MPI_COMM_WORLD);

	
	int result; // 返回结果
	MPI_Recv(&result, 1, MPI_INT, dest, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	return result;
}

int modifyPair(int dest, string key_value_pair)
{
	char* send = (char*)key_value_pair.c_str();
	MPI_Send(send, key_value_pair.size() + 1, MPI_CHAR, dest, 4, MPI_COMM_WORLD);

	int result;
	MPI_Recv(&result, 1, MPI_INT, dest, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	return result;
}
