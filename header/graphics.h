#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "../header/common.h"

/* Индексы игровых полей */
#define FIELD_MY    0   // Мое игровое поле
#define FIELD_ENEMY 1   // Игровое поле противника

/* Ошибки */
#define GRAPH_ERR           -1
#define GRAPH_SMALL_WIND    -2
#define GRAPH_TIMEOUT		-3

/* Общее */
int graph_init(void);
void graph_destroy(void);

/* Взаимодействие с игровыми полями */
void graph_field_refresh(int, struct play_field *);
int graph_cell_get(int *, int *, struct timeval *);
void graph_cell_refresh(int, int, int, int);

/* Взаимодействие со списками с выбором(меню и.т.д) */
int graph_item_get(char *, char **, int);

#endif