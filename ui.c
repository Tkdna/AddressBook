#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "linkedlist.h"
#include "ui.h"


MY_MENU PrintMenu(void)
{
	MY_MENU input = 0;

	system("cls");
	printf("[1]ADD\t[2]RELOAD\t[3]PRINT\t[4]SEARCH\t[0]EXIT\n");
	printf("��ȣ �Է� : ");
	scanf_s("%d%*c", &input);

	return input;
}

void PrintList(wait)
{
	MYNODE* pTmp = &g_HeadNode;
	while (pTmp != NULL)
	{
		printf("[%p] %s, new: %d, offset: %d ",
			pTmp, pTmp->pszKey, pTmp->bNew, pTmp->offset);

		if (pTmp->pDataCache != NULL)
		{
			USERDATA* pUser = pTmp->pDataCache;
			printf("%d, %s, [%p]\n",
				pUser->age, pUser->phone, pTmp->pNext);
		}
		else
		{
			printf("[%p]\n", pTmp->pNext);
		}

		pTmp = pTmp->pNext;
		
	}
	putchar('\n');

	if (wait)
		_getch();

}

void AddNewUser(void)
{
	USERDATA user = { 0 };

	printf("age: ");
	scanf_s("%d%*c", &user.age);
	printf("name: ");
	gets_s(user.name, sizeof(user.name));
	printf("phone: ");
	gets_s(user.phone, sizeof(user.phone));

	AddNewNode(user.name, &user, sizeof(user), true, 0);

	return;
}

void EditUserData(MYNODE* pNode)
{
	USERDATA* pUser = pNode->pDataCache;
	printf("New Phone Number : ");
	char szBuffer[32];
	gets_s(szBuffer, sizeof(szBuffer));
	if (strlen(szBuffer) > 0)
	{
		memcpy(pUser->phone, szBuffer, sizeof(szBuffer));

		printf("Do you want to commit now?(Y/n)\n");
		char ch = _getche();
		if(ch == 'y' || ch == 'Y')
		{
			SaveNodeToFile(pNode);
		}
	}
}

//�̸��� �˻��ؼ� ĳ�ó�� �����
//pNode : ĳ���� ����� ���Ḯ��Ʈ �ּ�
int ReloadDataByName(void)
{
	char name[32] = { 0 };

	printf("name: ");
	gets_s(name, sizeof(name));

	MYNODE* pNode = SearchListByName(name); //�̸����� ã��
	
	if (pNode != NULL)
	{
		FILE* fp = NULL;
		fopen_s(&fp, "listdata.dat", "rb");

		USERDATA user = { 0 };
		USERDATA* pDelete = NULL;
		fseek(fp, pNode->offset * sizeof(USERDATA), SEEK_SET); //���Ḯ��Ʈ��忡 ����� offset���� �� ���� �а�
		if (fread(&user, sizeof(USERDATA), 1, fp) > 0) //���� �о user�� ����
		{
			pDelete = pNode->pDataCache; //���� ĳ�̳�� ���
			pNode->pDataCache = malloc(sizeof(USERDATA)); //�ּ� �ٽ� �Ҵ�
			memcpy(pNode->pDataCache, &user, sizeof(USERDATA)); //user�� ������ �Ҵ�� ĳ�ó��, pDataCache�� ī��
			free(pDelete); //���� ĳ�̳�� free
			
			fclose(fp);
			return 1;
		}
		return 0;
	}
	return 0;
	
}

void SearchByName(void)
{
	char name[32] = { 0 };

	printf("name: ");
	gets_s(name, sizeof(name));

	MYNODE* pNode = SearchListByName(name); //pNode�� ĳ���� ����� �ּҸ� ����ִ� ���Ḯ��Ʈ�� �ּ�
	if (pNode != NULL)
	{
		USERDATA* pUser = pNode->pDataCache; //pUser�� ĳ�̵� ����� �ּ� : USERDATA*
		if (pUser != NULL)
		{
			printf("Found: %d, %s, %s\n",
				pUser->age, pUser->name, pUser->phone); //ĳ�̵� ��� ���

			printf("Do you want to edit? (y/n)\n");
			char ch = _getch();
			if (ch == 'y' || ch == 'Y')
				EditUserData(pNode); //�����ҰŸ� ĳ���� ��� �ּҸ� ����ִ� ���Ḯ��Ʈ �ּ� �Ѱ���
		}
		else
			puts("Cache data not found.");
	}
	else
		puts("Not found.");

	_getch();
}


void EventLoopRun(void)
{
	MY_MENU menu = 0;
	while ((menu=PrintMenu()) != 0)
	{
		switch (menu)
		{
		case ADD:
			AddNewUser();
			break;
		case RELOAD:
			ReloadDataByName();  
			break;
		case PRINT:
			PrintList();
			break;
		case SEARCH:
			SearchByName();
			break;
		default:
			break;
		}
		
	}
	puts("Bye~!");
}