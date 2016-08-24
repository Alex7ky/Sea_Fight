#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "common.h"

/* Индексы игровых полей */
#define FIELD_MY    0   /* Мое игровое поле        */
#define FIELD_ENEMY 1   /* Игровое поле противника */

/* Ошибки */
#define GRAPH_ERR           -1
#define GRAPH_SMALL_WIND    -2
#define GRAPH_TIMEOUT		-3

/* Общее */
int GraphInit(void);
void GraphDestroy(void);

/* Взаимодействие с игровыми полями */
void GraphFieldRefresh(int, struct play_field *);
int GraphCellGet(int *, int *, struct timeval *);
void GraphCellRefresh(int, int, int, int);

/* Взаимодействие со списками с выбором(меню и.т.д) */
int GraphItemGet(char *, char **, int);
void GraphPrintMsg(char *);
#endif