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
		//0�ʱ�ȭ �� �͵��� ������ �͵��̹Ƿ� bRemoved true�� ����
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

//pData �Ű������� NULL�� �ƴϸ� ĳ��
void AddNewNode(const char* pszKey,
	const void* pData, unsigned int sizeData, bool bRemoved,
	unsigned int offset)
{
	MYNODE* pNewNode = calloc(1, sizeof(MYNODE));	//���Ḯ��Ʈ ��� �����Ҵ�
	int keylen = strlen(pszKey);					
	pNewNode->pszKey = malloc(keylen + 1);			//Ű�� ���� �޸� �����Ҵ�
	strcpy_s(pNewNode->pszKey, keylen + 1, pszKey);

	if (pData != NULL)
	{
		USERDATA* pNewData = malloc(sizeData);		//������ ��� �����Ҵ�
		memcpy(pNewData, pData, sizeData);			//
		pNewNode->pDataCache = pNewData;
		pNewNode->sizeData = sizeData;
	}

	//pNewNode->bNew = bNew;
	pNewNode->bRemoved = bRemoved;
	pNewNode->offset = offset;

	//���Ḯ��Ʈ �ǵڿ� ����
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
	free(pNode->pszKey);		//���� ����� key free

	if (pData != NULL)
	{
		USERDATA* pNewData = malloc(sizeData);		
		memcpy(pNewData, pData, sizeData);			
		pNewNode->pDataCache = pNewData;
		pNewNode->sizeData = sizeData;
	}

	pNewNode->bRemoved = bRemoved;

	pNewNode->offset = pNode->offset;	//������ ����� offset�� ����
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

//�ش� ��ȣ�� ����ִ� ����� �ּ� ��ȯ
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
	//�ߺ����� 0, �ߺ����� 1
	if (pTmp == NULL) {		//pTmp�� ���̸� �ߺ�����
		return 0;
	}
	else {
		return 1;
	}
}

//������ ������ 0���� �ʱ�ȭ
//bRemoved ��ŷ
//key�� 0 �ʱ�ȭ
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

//������ ��� ã�� �Լ�
MYNODE* FindRemovedNode(void)
{
	MYNODE* pTmp = g_HeadNode.pNext;

	//���Ḯ��Ʈ��忡�� ������ ��带 ã��
	//������ ��尡 �ִ°�� �ش� ����� �ڸ��� �߰�
	while (pTmp != &g_TailNode) {
		if (pTmp->bRemoved == true) {
			return pTmp;
		}
		pTmp = pTmp->pNext;
	}

	return NULL;
}

//���Ͽ��� �о �����̸��̸� ��� ����
void SearchDataByName(const char* name, const char* phone, const char* operator) {
	int found = 0;
	int cnt = 0;
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "rb");
	USERDATA user = { 0 };

	//�̸��� �˻��Ѱ��
	if (operator==NULL) {
		//�̸��� ��ġ�ϸ� ���
		while (fread(&user, sizeof(USERDATA), 1, fp) != 0) {
			if (strcmp(user.name, name) == 0) {
				printf("[offset %d] name: %s, phone: %s, age: %d\n",cnt, user.name, user.phone, user.age);
				
				found = 1;
			}
			memset(&user, 0, sizeof(USERDATA));
			++cnt;
		}
	}
	//and�� ���
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
	//or�� ���
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
		printf("��ġ�ϴ� �����Ͱ� �������� �ʽ��ϴ�.\n");
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