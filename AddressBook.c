#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "linkedlist.h"
#include "ui.h"
#include "test.h"

int main(void)
{	
	InitList();
	//SaveBigDataToFile();
	LoadListFromFile();

	EventLoopRun();

	SaveNewDataToFile();
	ReleaseList();
	return 0;
}
