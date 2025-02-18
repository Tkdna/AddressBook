#pragma once

typedef enum { EXIT, ADD, PRINT, SEARCH, REMOVE, EDIT} MY_MENU;
typedef void(*MenuFunction)(void);

MY_MENU PrintMenu(void);
int EventLoopRun(void);
void PrintList(void);
void AddNewUser(void);
void RemoveUser();
void SearchUser();
void EditUser();