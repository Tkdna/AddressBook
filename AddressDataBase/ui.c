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
	printf("번호 입력: ");
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
	gets_s(user.name, sizeof(user.name));	//user.name만큼 
	printf("phone: ");
	gets_s(user.phone, sizeof(user.phone));
	//중복이면 계속 반복
	while (CheckDuplicate(user.phone) == 1) {
		printf("전화번호는 중복될 수 없습니다.\n");
		printf("phone: ");
		gets_s(user.phone, sizeof(user.phone));
	}

	MYNODE* pTmp = FindRemovedNode();	//삭제된 노드가져오기
	

	if (pTmp != NULL)
	{
		//삭제된 노드에다 오버라이트
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
	printf("전화번호: ");
	gets_s(user.phone, sizeof(user.phone));
	MYNODE* pNode = SearchListByPhone(user.phone);
	if (pNode == NULL) {
		printf("일치하는 전화번호가 존재하지 않습니다.\n");
		_getch();
		return;
	}
	else {
		RemoveData(pNode);
		printf("삭제 완료\n");
		_getch();
		return;
	}
}

void EditUser() {
	USERDATA user = { 0 };
	printf("전화번호: ");
	gets_s(user.phone, sizeof(user.phone));
	MYNODE* pNode = SearchListByPhone(user.phone);
	if (pNode == NULL) {
		printf("일치하는 전화번호가 존재하지 않습니다.\n");
		_getch();
		return;
	}
	else {
		//printf("name: %s, phone: %s, age: %d\n", user.name, user.phone, user.age);
		printf("해당 데이터를 수정 하시겠습니까? (y/n)");
		char ch = _getch();
		putchar('\n');
		if (ch == 'y' || ch == 'Y') {
			printf("New Phone Number: ");
			gets_s(user.phone, sizeof(user.phone));
			while (CheckDuplicate(user.phone) == 1) {
				printf("전화번호는 중복될 수 없습니다.\n");
				printf("New Phone Number: ");
				
				gets_s(user.phone, sizeof(user.phone));
			}

			EditPhoneNumber(pNode, user.phone);
			printf("수정 완료\n");
			_getch();
		}
		return;
	}
}

//검색
void SearchUser() {
	char query[60];
	const char delimeters[] = " ='";
	char* context = NULL;
	char* tokens[5];
	int tokenCount = 0;

	printf("쿼리입력: ");
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
				printf("쿼리 형식에 맞게 입력해주세요.");
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
				printf("쿼리 형식에 맞게 입력해주세요.");
				_getch();
				return;
			}
		}
	}

	//이름만 입력한경우
	if (name[0] != '\0' && phone[0] == '\0') {
		LARGE_INTEGER start, end, frequency;
		QueryPerformanceFrequency(&frequency);

		// 시작 시간 측정
		QueryPerformanceCounter(&start);

		SearchDataByName(name, NULL, NULL);

		// 종료 시간 측정
		QueryPerformanceCounter(&end);

		// 시간 차이 계산 (나노초 단위로 변환)
		LONGLONG elapsed = end.QuadPart - start.QuadPart;
		double nanoseconds = (double)elapsed * 1e9 / frequency.QuadPart;

		printf("수행속도: %.0f ns\n", nanoseconds);
	}
	else if (name[0] != '\0' && phone[0] != '\0') {			//이름, 번호 모두 입력한경우
		if (strcmp(operator, "and") == 0 || strcmp(operator, "or") ==0) {
			LARGE_INTEGER start, end, frequency;
			QueryPerformanceFrequency(&frequency);

			// 시작 시간 측정
			QueryPerformanceCounter(&start);

			SearchDataByName(name, phone, operator);

			// 종료 시간 측정
			QueryPerformanceCounter(&end);

			// 시간 차이 계산 (나노초 단위로 변환)
			LONGLONG elapsed = end.QuadPart - start.QuadPart;
			double nanoseconds = (double)elapsed * 1e9 / frequency.QuadPart;

			printf("수행속도: %.0f ns\n", nanoseconds);
		}
		else {
			printf("쿼리형식에 맞게 입력해주세요.");
		}
	}
	else {
		printf("쿼리형식에 맞게 입력해주세요.");
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