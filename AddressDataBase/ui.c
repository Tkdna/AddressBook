#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "linkedlist.h"
#include "ui.h"
#include <Windows.h>



MY_MENU PrintMenu(void)
{
	MY_MENU input = 0;

	system("cls");
	printf("[1]ADD\t[2]PRINT\t[3]SEARCH\t[4]REMOVE\t[5]EDIT\t[0]EXIT\n");
	printf("��ȣ �Է�: ");
	scanf_s("%d%*c", &input);

	return input;
}

void PrintList(void)
{
	MYNODE* pTmp = &g_HeadNode;
	while (pTmp != NULL)
	{
		printf("[%p] %s, removed: %d, offset: %d ",
			pTmp, pTmp->pszKey, pTmp->bRemoved, pTmp->offset);

		/*if (pTmp->pDataCache != NULL)
		{
			USERDATA* pUser = (USERDATA*)pTmp->pDataCache;
			printf("%d, %s, [%p]\n",
				pUser->age, pUser->phone, pTmp->pNext);
		}
		else
		{
			printf("[%p]\n", pTmp->pNext);
		}*/

		printf("[%p]\n", pTmp->pNext);
		pTmp = pTmp->pNext;

	}
	putchar('\n');

	_getch();

}

void AddNewUser(void)
{
	USERDATA user = { 0 };
	//memset(&user, 0, sizeof(USERDATA));

	printf("age: ");
	scanf_s("%d%*c", &user.age);
	printf("name: ");
	gets_s(user.name, sizeof(user.name));	//user.name��ŭ 
	printf("phone: ");
	gets_s(user.phone, sizeof(user.phone));
	//�ߺ��̸� ��� �ݺ�
	while (CheckDuplicate(user.phone) == 1) {
		printf("��ȭ��ȣ�� �ߺ��� �� �����ϴ�.\n");
		printf("phone: ");
		gets_s(user.phone, sizeof(user.phone));
	}

	MYNODE* pTmp = FindRemovedNode();	//������ ��尡������
	

	if (pTmp != NULL)
	{
		//������ ��忡�� ��������Ʈ
		OverwriteNewNode(user.phone, &user, sizeof(user), false, 0, pTmp);
		SaveNewNodeToFile(user, pTmp->offset);
		free(pTmp);
		return;
	}
	else {
		int MaxOffset = getMaxOffset() + 1;
		AddNewNode(user.phone, &user, sizeof(user), false, MaxOffset);
		SaveNewNodeToFile(user, MaxOffset);
	}
	return;
}


void RemoveUser() {
	USERDATA user = { 0 };
	printf("��ȭ��ȣ: ");
	gets_s(user.phone, sizeof(user.phone));
	MYNODE* pNode = SearchListByPhone(user.phone);
	if (pNode == NULL) {
		printf("��ġ�ϴ� ��ȭ��ȣ�� �������� �ʽ��ϴ�.\n");
		_getch();
		return;
	}
	else {
		RemoveData(pNode);
		printf("���� �Ϸ�\n");
		_getch();
		return;
	}
}

void EditUser() {
	USERDATA user = { 0 };
	printf("��ȭ��ȣ: ");
	gets_s(user.phone, sizeof(user.phone));
	MYNODE* pNode = SearchListByPhone(user.phone);
	if (pNode == NULL) {
		printf("��ġ�ϴ� ��ȭ��ȣ�� �������� �ʽ��ϴ�.\n");
		_getch();
		return;
	}
	else {
		//printf("name: %s, phone: %s, age: %d\n", user.name, user.phone, user.age);
		printf("�ش� �����͸� ���� �Ͻðڽ��ϱ�? (y/n)");
		char ch = _getch();
		putchar('\n');
		if (ch == 'y' || ch == 'Y') {
			printf("New Phone Number: ");
			gets_s(user.phone, sizeof(user.phone));
			while (CheckDuplicate(user.phone) == 1) {
				printf("��ȭ��ȣ�� �ߺ��� �� �����ϴ�.\n");
				printf("New Phone Number: ");
				
				gets_s(user.phone, sizeof(user.phone));
			}

			EditPhoneNumber(pNode, user.phone);
			printf("���� �Ϸ�\n");
			_getch();
		}
		return;
	}
}

//�˻�
void SearchUser() {
	char query[60];
	const char delimeters[] = " ='";
	char* context = NULL;
	char* tokens[5];
	int tokenCount = 0;

	printf("�����Է�: ");
	gets_s(query, sizeof(query));

	char* token = strtok_s(query, delimeters, &context);

	while (token != NULL)
	{
		tokens[tokenCount] = token;
		tokenCount++;
		token = strtok_s(NULL, delimeters, &context);
	}
	
	char name[32] = { 0 };
	char operator[4] = { 0 };
	char phone[32] = { 0 };

	for (int i = 0; i < tokenCount; i++) {
		if (strcmp(tokens[i], "name") == 0) {
			if (i + 1 < tokenCount) {
				strcpy(name, tokens[i + 1]);
			}
			else {
				printf("���� ���Ŀ� �°� �Է����ּ���.");
				_getch();
				return;
			}
		}
		else if (strcmp(tokens[i], "and") == 0 || strcmp(tokens[i], "or") == 0) {
			strcpy(operator, tokens[i]);
		}
		else if (strcmp(tokens[i], "phone") == 0) {
			if (i + 1 < tokenCount) {
				strcpy(phone, tokens[i + 1]);
			}
			else {
				printf("���� ���Ŀ� �°� �Է����ּ���.");
				_getch();
				return;
			}
		}
	}

	//�̸��� �Է��Ѱ��
	if (name[0] != '\0' && phone[0] == '\0') {
		LARGE_INTEGER start, end, frequency;
		QueryPerformanceFrequency(&frequency);

		// ���� �ð� ����
		QueryPerformanceCounter(&start);

		SearchDataByName(name, NULL, NULL);

		// ���� �ð� ����
		QueryPerformanceCounter(&end);

		// �ð� ���� ��� (������ ������ ��ȯ)
		LONGLONG elapsed = end.QuadPart - start.QuadPart;
		double nanoseconds = (double)elapsed * 1e9 / frequency.QuadPart;

		printf("����ӵ�: %.0f ns\n", nanoseconds);
	}
	else if (name[0] != '\0' && phone[0] != '\0') {			//�̸�, ��ȣ ��� �Է��Ѱ��
		if (strcmp(operator, "and") == 0 || strcmp(operator, "or") ==0) {
			LARGE_INTEGER start, end, frequency;
			QueryPerformanceFrequency(&frequency);

			// ���� �ð� ����
			QueryPerformanceCounter(&start);

			SearchDataByName(name, phone, operator);

			// ���� �ð� ����
			QueryPerformanceCounter(&end);

			// �ð� ���� ��� (������ ������ ��ȯ)
			LONGLONG elapsed = end.QuadPart - start.QuadPart;
			double nanoseconds = (double)elapsed * 1e9 / frequency.QuadPart;

			printf("����ӵ�: %.0f ns\n", nanoseconds);
		}
		else {
			printf("�������Ŀ� �°� �Է����ּ���.");
		}
	}
	else {
		printf("�������Ŀ� �°� �Է����ּ���.");
	}
	
	_getch();

	return;
}



int EventLoopRun(void)
{
	MenuFunction menuFunctions[6] = {
	NULL,           // EXIT
	AddNewUser,     // ADD
	PrintList,		// PRINT
	SearchUser,     // SEARCH
	RemoveUser,		// DELETE
	EditUser
	};

	MY_MENU menu = 0;
	while ((menu = PrintMenu()) != 0)
	{
		switch (menu)
		{
		case ADD:
			menuFunctions[menu]();
			break;
		case PRINT:
			menuFunctions[menu]();
			break;
		case SEARCH:
			menuFunctions[menu]();
			break;
		case REMOVE:
			menuFunctions[menu]();
			break;
		case EDIT:
			menuFunctions[menu]();
			break;
		default:
			break;
		}
	}

	puts("Bye~!");

	return 0;
}