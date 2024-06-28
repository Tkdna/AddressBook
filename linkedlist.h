#pragma once
#include <stdbool.h>

typedef struct USERDATA {
	int age;
	char name[32];
	char phone[32];
}USERDATA;

typedef struct MYNODE {
	bool bNew;
	void* pDataCache;
	char* pszKey;
	unsigned int sizeData;
	unsigned int offset;

	struct MYNODE* pPrev;
	struct MYNODE* pNext;
}MYNODE;

extern MYNODE g_HeadNode;
extern MYNODE g_TailNode;

void InitList(void);
int IsEmpty(void);
void ReleaseList(void);
void AddNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bNew,
	unsigned int offset);
void* SearchListByName(char* pszKey);
int SaveNewDataToFile(void);
int LoadListFromFile(void);
int SaveNodeToFile(MYNODE* pNode);

