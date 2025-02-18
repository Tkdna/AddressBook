#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "ui.h"

MYNODE g_HeadNode = { 0 };
MYNODE g_TailNode = { 0 };
static unsigned int g_listCount = 0;

void InitList(void) {
	ReleaseList();
	g_HeadNode.pNext = &g_TailNode;
	g_TailNode.pPrev = &g_HeadNode;
	g_listCount = 0;
}

int IsEmpty(void) {
	if (g_HeadNode.pNext == &g_TailNode ||
		g_HeadNode.pNext == NULL)
		return 1;

	return 0;
}

void ReleaseList(void)
{
	if (IsEmpty() == 1)
		return;

	MYNODE* pTmp = g_HeadNode.pNext;
	MYNODE* pDelete = { 0 };
	while (pTmp != &g_TailNode)
	{
		pDelete = pTmp;
		pTmp = pTmp->pNext;

		if (pDelete->pDataCache != NULL)
			free(pDelete->pDataCache);
		
		free(pDelete->pszKey);
		free(pDelete);
	}
	g_HeadNode.pNext = &g_TailNode;
	g_TailNode.pPrev = &g_HeadNode;
	g_listCount = 0;
	return;
}

int LoadListFromFile(void)
{
	ReleaseList();
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb");
	if (fp == NULL)
		return 0;

	USERDATA user = { 0 };
	unsigned int offset = 0;

	while (fread(&user, sizeof(USERDATA), 1, fp) != 0)
	{
		//0초기화 된 것들은 삭제된 것들이므로 bRemoved true로 설정
		if (user.age == 0 && strcmp("", user.name) == 0 && strcmp("", user.phone) == 0) {
			AddNewNode(user.phone, NULL, sizeof(USERDATA), true, offset);
			memset(&user, 0, sizeof(USERDATA));
			++offset;
			continue;
		}

		AddNewNode(user.phone, NULL, sizeof(USERDATA), false, offset);
		memset(&user, 0, sizeof(USERDATA));
		++offset;
	}

	fclose(fp);
	return 1;
}

//pData 매개변수가 NULL이 아니면 캐싱
void AddNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bRemoved,
	unsigned int offset)
{
	MYNODE* pNewNode = calloc(1, sizeof(MYNODE));	//연결리스트 노드 동적할당
	int keylen = strlen(pszKey);					
	pNewNode->pszKey = malloc(keylen + 1);			//키를 담을 메모리 동적할당
	strcpy_s(pNewNode->pszKey, keylen + 1, pszKey);

	if (pData != NULL)
	{
		USERDATA* pNewData = malloc(sizeData);		//데이터 노드 동적할당
		memcpy(pNewData, pData, sizeData);			//
		pNewNode->pDataCache = pNewData;
		pNewNode->sizeData = sizeData;
	}

	//pNewNode->bNew = bNew;
	pNewNode->bRemoved = bRemoved;
	pNewNode->offset = offset;

	//연결리스트 맨뒤에 붙임
	MYNODE* pPrevNode = g_TailNode.pPrev;
	pNewNode->pNext = &g_TailNode;
	pNewNode->pPrev = pPrevNode;
	pPrevNode->pNext = pNewNode;
	g_TailNode.pPrev = pNewNode;

	++g_listCount;
}

//
void OverwriteNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bRemoved,
	unsigned int offset, MYNODE* pNode)
{
	MYNODE* pNewNode = calloc(1, sizeof(MYNODE));	
	int keylen = strlen(pszKey);
	pNewNode->pszKey = malloc(keylen + 1);			
	strcpy_s(pNewNode->pszKey, keylen + 1, pszKey);
	free(pNode->pszKey);		//기존 노드의 key free

	if (pData != NULL)
	{
		USERDATA* pNewData = malloc(sizeData);		
		memcpy(pNewData, pData, sizeData);			
		pNewNode->pDataCache = pNewData;
		pNewNode->sizeData = sizeData;
	}

	pNewNode->bRemoved = bRemoved;

	pNewNode->offset = pNode->offset;	//삭제된 노드의 offset을 가짐
	pNewNode->pPrev = pNode->pPrev;		
	pNewNode->pNext = pNode->pNext;

	MYNODE* pPrevNode = pNode->pPrev;
	MYNODE* pNextNode = pNode->pNext;
	pPrevNode->pNext = pNewNode;		
	pNextNode->pPrev = pNewNode;

	++g_listCount;
}


int SaveNewNodeToFile(USERDATA user, int offset) {

	if (offset >= 0) {
		FILE* fp = NULL;
		fopen_s(&fp, "listdata.dat", "rb+");
		if (fp == NULL)
			return 0;

		fseek(fp, offset * sizeof(USERDATA), SEEK_SET);
		fwrite(&user, sizeof(USERDATA), 1, fp);
		fclose(fp);
		return 1;
	}
	/*FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "ab+");
	if (fp == NULL)
		return 0;
	
	fwrite(&user, sizeof(USERDATA), 1, fp);
	fclose(fp);
	return 1;*/
}

int getMaxOffset(void) {
	
	return g_TailNode.pPrev->offset;
}

//해당 번호를 들고있는 노드의 주소 반환
void* SearchListByPhone(char* pszKey)
{
	MYNODE* pTmp = g_HeadNode.pNext;
	while (pTmp != &g_TailNode) {
		if (strcmp(pTmp->pszKey, pszKey) == 0) {
			return pTmp;
		}
		pTmp = pTmp->pNext;
	}
	return NULL;
}

int CheckDuplicate(const char* pszData) {
	MYNODE* pTmp = SearchListByPhone(pszData);
	//중복없음 0, 중복있음 1
	if (pTmp == NULL) {		//pTmp가 널이면 중복없음
		return 0;
	}
	else {
		return 1;
	}
}

//파일의 데이터 0으로 초기화
//bRemoved 마킹
//key값 0 초기화
int RemoveData(MYNODE* pNode)
{
	USERDATA user = { 0 };
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb+");

	if (fp == NULL)
		return 0;

	fseek(fp, pNode->offset * sizeof(USERDATA), SEEK_SET);
	if (fwrite(&user, sizeof(USERDATA), 1, fp) > 0)
	{
		pNode->bRemoved = true;
		memset(pNode->pszKey, 0, sizeof(pNode->pszKey));
		fclose(fp);
		return 1;
	}

	fclose(fp);
	return 0;
}

//삭제된 노드 찾는 함수
MYNODE* FindRemovedNode(void)
{
	MYNODE* pTmp = g_HeadNode.pNext;

	//연결리스트노드에서 삭제된 노드를 찾음
	//삭제된 노드가 있는경우 해당 노드의 자리에 추가
	while (pTmp != &g_TailNode) {
		if (pTmp->bRemoved == true) {
			return pTmp;
		}
		pTmp = pTmp->pNext;
	}

	return NULL;
}

//파일에서 읽어서 같은이름이면 노드 생성
void SearchDataByName(const char* name, const char* phone, const char* operator) {
	int found = 0;
	int cnt = 0;
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb");
	USERDATA user = { 0 };

	//이름만 검색한경우
	if (operator==NULL) {
		//이름이 일치하면 출력
		while (fread(&user, sizeof(USERDATA), 1, fp) != 0) {
			if (strcmp(user.name, name) == 0) {
				printf("[offset %d] name: %s, phone: %s, age: %d\n",cnt, user.name, user.phone, user.age);
				
				found = 1;
			}
			memset(&user, 0, sizeof(USERDATA));
			++cnt;
		}
	}
	//and인 경우
	else if (strcmp(operator, "and")==0) {
		while (fread(&user, sizeof(USERDATA), 1, fp) != 0) {
			if (strcmp(user.name, name) == 0 && strcmp(user.phone, phone) == 0) {
				printf("[offset %d] name: %s, phone: %s, age: %d\n",cnt, user.name, user.phone, user.age);
				found = 1;
			}
			memset(&user, 0, sizeof(USERDATA));
			++cnt;
		}
	}
	//or인 경우
	else if (strcmp(operator, "or")==0) {
		while (fread(&user, sizeof(USERDATA), 1, fp) != 0) {
			if (strcmp(user.name, name) == 0 || strcmp(user.phone, phone) == 0) {
				printf("[offset %d] name: %s, phone: %s, age: %d\n",cnt, user.name, user.phone, user.age);
				found = 1;
			}
			memset(&user, 0, sizeof(USERDATA));
			++cnt;
		}
	}
	
	if (found == 0) {
		printf("일치하는 데이터가 존재하지 않습니다.\n");
	}

	fclose(fp);
	return;
}

int EditPhoneNumber(MYNODE* pNode, const char* phone) {
	USERDATA user = { 0 };
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb+");

	if (fp == NULL)
		return 0;

	fseek(fp, pNode->offset * sizeof(USERDATA) + sizeof(user.age) + sizeof(user.name), SEEK_SET);

	if (fwrite(phone, sizeof(user.phone), 1, fp) > 0)
	{
		strcpy(pNode->pszKey, phone);
		fclose(fp);
		return 1;
	}
	
	fclose(fp);
	return 0;
}