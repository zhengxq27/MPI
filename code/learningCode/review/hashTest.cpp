#include<iostream>
#include<stdio.h>
#include<string>
#include<functional>
#include<map>
#include<fstream>


using namespace std;

int main()
{
	string s = "hello world!";
	hash<string> hash_fn;
	size_t hash = hash_fn(s);
	string str_hash = to_string(hash);
	
	
	map<string,string> localMemory;
	pair<map<string,string>::iterator,bool>a = localMemory.insert(pair<string,string>(str_hash,"test"));
	cout << "insert value is: " << a.second << endl;
	map<string,string>::iterator result = localMemory.find(str_hash);
	cout << (*result).second << endl;
	
	size_t result1 = localMemory.erase(str_hash); 
	cout << localMemory.erase("test") << endl; //0表示元素不在容器中 
	cout << result1 << endl; // 删除成功 
	
	
//	ifstream in("text1.txt");
//	string line;
//	int pos = 0;
//	string str1,str2;
//	size_t first;
//	while(getline(in,line))
//	{
//		pos = line.find(" ");
//		str1 = line.substr(0,pos);
//		str2 = line.substr(pos+1,line.size()-pos);
//		localMemory.insert(pair<string,string>(str1,str2));
//	}
//	map<string,string>::iterator iter = localMemory.begin();
//	while(iter != localMemory.end())
//	{
//		cout << iter->first << " " << iter->second << endl;
//		iter++;
//	}
	return 0;
} 
