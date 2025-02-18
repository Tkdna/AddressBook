#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "test.h"
#include "ui.h"
#include "linkedlist.h"


int CreateDummyData(void) {
	FILE* fp = NULL;
	fopen_s(&fp, "listdata.dat", "ab");
	if (fp == NULL)
		return 0;

	USERDATA user = { 0 };
	unsigned int cnt = 0;

	srand(time(NULL));

	while (cnt != 10000) {
		CreateRandomName(&user.name);
		CreateRandomPhoneNumber(&user.phone, cnt);
		CreateRandomAge(&user.age);
		fwrite(&user, sizeof(USERDATA), 1, fp);

		++cnt;
	}

	fclose(fp);
	return 1;
}

void CreateRandomName(char* pszData)
{
	pszData[0] = 'A' + rand() % 26;	//첫글자 대문자
	//3개의 소문자 랜덤 소문자 조합하여 최종 랜덤한 이름 생성
	for (int i = 1; i < 4; ++i) {
		pszData[i] = 'a' + rand() % 26;
	}
	pszData[4] = '\0';
	return;
}


void CreateRandomPhoneNumber(char* pszData, const int cnt)
{
	sprintf(pszData, "010-%04d-%04d", cnt / 10000, cnt % 10000);
	pszData[13] = '\0';
	return;
}

void CreateRandomAge(int* age)
{
	*age = (rand() % 90) + 1;
	return;
}