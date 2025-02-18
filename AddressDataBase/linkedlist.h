#pragma once
#include <stdbool.h>

typedef struct USERDATA {
	int age;
	char name[32];
	char phone[32];
}USERDATA;

typedef struct MYNODE {
	bool bRemoved;
	void* pDataCache;
	char* pszKey;
	unsigned int offset;
	unsigned int sizeData;

	struct MYNODE* pPrev;
	struct MYNODE* pNext;

}MYNODE;


extern MYNODE g_HeadNode;
extern MYNODE g_TailNode;
static unsigned int g_listCount;

void InitList(void);
int IsEmpty(void);
void ReleaseList(void);
void AddNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bRemoved,
	unsigned int offset);
int LoadListFromFile(void);
void* SearchListByPhone(char* pszKey);
int SaveNewNodeToFile(USERDATA user, int offset);
int CheckDuplicate(const char* pszData);
int RemoveData(MYNODE* pNode);
void OverwriteNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bRemoved,
	unsigned int offset, MYNODE* pNode);
MYNODE* FindRemovedNode(void);
void SearchDataByName(char* name, char* phone, char* operator);
int getMaxOffset(void);
int EditPhoneNumber(MYNODE* pNode, const char* phone);