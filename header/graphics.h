#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <ncurses.h>
#include <unistd.h>
#include "../header/common.h"

/* Индексы игровых полей */
#define FIELD_MY    0   // Мое игровое поле
#define FIELD_ENEMY 1   // Игровое поле противника

/* Ошибки */
#define GRAPH_ERR           -1
#define GRAPH_SMALL_WIND    -2

/* Общее */
int graph_init(void);
void graph_destroy(void);

/* Взаимодействие с игровыми полями */
void graph_print_chcell(WINDOW *, int);
void graph_refcell(int, int, int, int);
void graph_reffield(int, struct play_field *);

/* Взаимодействие со списками с выбором(меню и.т.д) */
int graph_selectone(char *, char **, int);
void graph_reflist(char *, char **, int, int);

#endif