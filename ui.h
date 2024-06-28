#pragma once

typedef enum MY_MENU { EXIT, ADD, RELOAD, PRINT, SEARCH } MY_MENU;

typedef enum MY_MENU;
MY_MENU PrintMenu(void);
void EventLoopRun(void);
int ReloadDataByName(void);