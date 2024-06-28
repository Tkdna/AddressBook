#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ui.h"
#include "linkedlist.h"


MYNODE g_HeadNode = { 0 };
MYNODE g_TailNode = { 0 };
static unsigned int g_listCount = 0;


//리스트 초기화 -> 리스트 비우고 헤드더미 테일더미 연결(최초상태)
void InitList(void)
{
	ReleaseList();
	g_HeadNode.pNext = &g_TailNode;
	g_TailNode.pPrev = &g_HeadNode;
	g_listCount = 0;
}

//비어있는지 확인
int IsEmpty(void)
{
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

//pData매개변수는 캐싱할때 파일에서 읽은 USERDATA user의 주소
void AddNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bNew,
	unsigned int offset)
{
	MYNODE* pNewNode = calloc(1, sizeof(MYNODE));
	int keylen = (int)strlen(pszKey);
	pNewNode->pszKey = malloc(keylen + 1);
	strcpy_s(pNewNode->pszKey, keylen + 1, pszKey);

	if (pData != NULL)
	{
		void* pNewData = malloc(sizeData);
		memcpy(pNewData, pData, sizeData);
		pNewNode->pDataCache = pNewData;
		pNewNode->sizeData = sizeData;
	}

	pNewNode->bNew = bNew;
	pNewNode->offset = offset;

	MYNODE* pPrevNode = g_TailNode.pPrev;
	pNewNode->pNext = &g_TailNode;
	pNewNode->pPrev = pPrevNode;
	pPrevNode->pNext = pNewNode;
	g_TailNode.pPrev = pNewNode;

	++g_listCount;
}

int LoadListFromFile(void)
{
	ReleaseList();
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb");
	if (fp == NULL)
		return;

	USERDATA user = { 0 };
	unsigned int offset = 0;

	//fp포인터가 가리키는 버퍼에 오픈된 파일을 USERDATA만큼 1씩 user에 담음, user초기화 하고 다음꺼 담고 하면서 연결리스트 생성
	while (fread(&user, sizeof(USERDATA), 1, fp) != 0)
	{
		AddNewNode(user.name, NULL, sizeof(USERDATA), false, offset);
		memset(&user, 0, sizeof(USERDATA));
		++offset;
	}

	fclose(fp);
	return 1;
}

void* SearchListByName(char* pszKey)
{
	MYNODE* pTmp = g_HeadNode.pNext;
	while (pTmp != &g_TailNode)
	{
		if (strcmp(pTmp->pszKey, pszKey)== 0)
		{
			if (pTmp->pDataCache != NULL)
				return pTmp;
			
			FILE* fp = NULL;
			fopen_s(&fp, "listdata.dat", "rb");

			USERDATA user = { 0 };
			fseek(fp, pTmp->offset * sizeof(USERDATA), SEEK_SET);
			if (fread(&user, sizeof(USERDATA), 1, fp) > 0)
			{
				pTmp->pDataCache = malloc(sizeof(USERDATA));
				memcpy(pTmp->pDataCache, &user, sizeof(USERDATA));
				pTmp->sizeData = sizeof(USERDATA);

				fclose(fp);
				return pTmp;
			}
			
			fclose(fp);
		}
		pTmp = pTmp->pNext;
	}
	return NULL;
}


//캐싱되어있는 노드를 파일에 저장
int SaveNodeToFile(MYNODE* pNode)
{
	if (pNode->pDataCache == NULL || pNode->sizeData == 0)
		return 0;

	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb+");
	
	if (fp == NULL)
		return;

	fseek(fp, pNode->offset*sizeof(USERDATA), SEEK_SET);

	if (fwrite(pNode->pDataCache, sizeof(USERDATA), 1, fp) > 0)
	{
		fclose(fp);
		return 1;
	}

	return 0;
}

int SaveNewDataToFile(void)
{
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "ab+");
	if (fp == NULL)
		return 0;

	MYNODE* pTmp = g_HeadNode.pNext;
	while (pTmp != &g_TailNode)
	{
		if (pTmp->bNew == true)
		{
			fwrite(pTmp->pDataCache, pTmp->sizeData, 1, fp);
		}
		pTmp = pTmp->pNext;
	}
	fclose(fp);
	return 1;
}

