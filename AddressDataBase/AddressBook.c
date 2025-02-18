#include <stdio.h>
#include "test.h"
#include "linkedlist.h"
#include "ui.h"

int main(void) {

	//CreateDummyData();

	InitList();

	LoadListFromFile();

	EventLoopRun();

	ReleaseList();

	return 0;
}