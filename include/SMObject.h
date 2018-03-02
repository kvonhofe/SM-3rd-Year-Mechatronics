#ifndef SMOBJECT_H //header file sentries 
#define SMOBJECT_H

#include <Windows.h> //we need this because we are using handles 
#include <tchar.h>
//#include <SMFcn.h>

class SMObject
{
	HANDLE CreateHandle; //handle to the created memory 
	HANDLE AccessHandle; //handle to the accessed memory
	TCHAR* szName; //a global tag name to the memory block(similar to filr names however it is not created on the disk. It is created in memory
	int Size;  //size of shared memory in bytes 
public:
	void *pData;  //pointer to the shared memory block
	int SMCreateError;  // create error flag
	int SMAccessError;   // access error flag
public:
	SMObject();
	SMObject(TCHAR* szname, int size);  //takes in a tag name and shared memory block size 

	~SMObject();
	int SMCreate();   //creating shared memory block (PM calls this after creating the object using line 20)
	int SMAccess();   //granting access to the shared memory block 
};
#endif


