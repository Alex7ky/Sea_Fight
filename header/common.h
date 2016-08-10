#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CNT_SHIPS   10      // Количество кораблей
#define FIELD_COLS   10     // Ширина игрового поля
#define FIELD_LINES  10     // Высота игрового поля

#define CELL_FREE   0       // Удара по ячейке не было
#define CELL_MISS   1       // Удар мимо
#define CELL_HIT    2       // Попадение по ячейке корабля

/**
 * Структура, описывающая корабль (его положение)
 */
struct ship
{
    int startx;     // Координата X начала
    int starty;     // Координата Y начала
    int endx;       // Координата X конца
    int endy;       // Координата Y конца
}

/**
 * Структура, описывающая игровое поле
 */
struct play_field
{
    char field[FIELD_COLS][FIELD_LINES];    // Удары по полю
    struct ship ships[CNT_SHIPS];           // Корабли
}
