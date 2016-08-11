#include "../header/graphics.h"

#define CNT_WINDOW  3   // Количество окон

/* Индексы окон */
#define WIN_INSTEP  0   // Окно для ввода хода
#define WIN_BINSTEP 1   // Граница вокруг окна ввода хода

static WINDOW *windows[CNT_WINDOW];                 // Графические окна
static WINDOW *cells[2][FIELD_LINES][FIELD_COLS];   // Ячейки игрового поля

/**
 * Полностью инициализирует графику приложения. Функция обязательна 
 * для вызова перед взаимодействием с графикой
 *
 * @retvel  0 Успешное завершение
 * @retval -1 Ошибка при инициализации
 */
int graph_init(void)
{
    int basex;              // Координата X начала игрового поля
    int basey;              // Координата Y начала игрового поля

    //Инициализация ncurses
    initscr();
    refresh();
    noecho();
    keypad(stdscr, TRUE);

    // Проверяем вместится ли изображение
    if (COLS <= FIELD_COLS * 10 + 10|| LINES <= FIELD_LINES + 10){
        endwin();
        return GRAPH_SMALL_WIND;
    }

    // Создаем окна

    // Создаем окна-ячейки моего игровых полей
    basex = (COLS / 2 - FIELD_COLS * 5) / 2;
    basey = 3;
    for (int field = 0; field < 2; field++){
        for (int i = 0; i < FIELD_LINES; i++){
            for(int j = 0; j < FIELD_COLS; j++){
                cells[field][i][j] = newwin(3, 5, basey + i * 3, basex + j*5);
                if (cells[field][i][j] == NULL)
                    return GRAPH_ERR;;
            }
        }
        // Смещаем начало игрового поля для 
        basex += COLS / 2;
    }
    return 0;
}


/**
 * Отключает графику приложения. Необходимо обязательно вызывать при
 * завершении программы
 */
void graph_destroy(void)
{
    // Удаляем ячейки игровых полей
    for (int field = 0; field < 2; field++){
        for (int i = 0; i < FIELD_LINES; i++){
            for(int j = 0; j < FIELD_COLS; j++){
                delwin(cells[field][i][j]);
            }
        }
    }
    // Отключаем графику
    endwin();
    return;
}


/**
 * Выводит обозначающий символ в ячейку в зависимости от значения в поле. 
 * Функция является вспомогательной только внутри данного модуля
 * 
 * @param cell  Ячейка
 * @param val   Соответствующее значение в игровом поле (макросы FIELD_..)
 */
void graph_print_chcell(WINDOW *cell, int val)
{
    switch(val){
        case CELL_MISS:
            mvwprintw(cell, 1, 2, "O");
        break;

        case CELL_HIT:
            mvwprintw(cell, 1, 2, "X");
        break;

        case CELL_SHIP:
            mvwprintw(cell, 1, 2, "#");
        break;

        default:
            mvwprintw(cell, 1, 2, " ");
        break;
    }
}


/**
 * Полностью перерисовывает игровое окно. Для изменения одной или нескольких 
 * ячеек рекомендуется использовать graph_refcell()
 * 
 * @param nfield    Поле в котором обновляем (макросы FIELD_..)
 * @param field     Указатель на игровое поле
 */
void graph_reffield(int nfield, struct play_field *pfield)
{
    WINDOW  *current;   // Текущая обновляемая ячейка
    int     basex;      // Координата X начала игрового поля
    int     basey;      // Координата Y начала игрового поля
    int     ch;         // Выводимый символ

    /**
     * В циклах проходим по строкам и столбцам, очищая ячейки и польностью 
     * их перерисовывая
     */
    for (int i = 0; i < FIELD_LINES; i++){
        for(int j = 0; j < FIELD_COLS; j++){
            current = cells[nfield][i][j];
            // Перерисовываем рамку
            wclear(current);
            box(current, 0, 0);
            //Вставляем соответствующий символ в ячейку
            graph_print_chcell(current, pfield->field[i][j]);
            wrefresh(current);
            usleep(10000);
        }
    }

    // Нумеруем поля. Определяем верхнюю правую границу поля
    if (nfield == FIELD_ENEMY)
        basex = ((COLS / 2 - FIELD_COLS * 5) / 2) * 3 + FIELD_COLS * 5;
    else
        basex = ((COLS / 2 - FIELD_COLS * 5) / 2);
    basey = 3;

    // Нумеруем столбцы
    ch = 'A';
    for(int i = 0; i < FIELD_COLS; i++)
        mvprintw(basey - 1, basex + 2 + i * 5, "%c", ch++);
    // Нумеруем строки
    ch = '0';
    for(int i = 0; i < FIELD_LINES; i++)
        mvprintw(basey + 1 + i * 3, basex - 2, "%c", ch++);
    refresh();
    return;
}


/**
 * Полностью перерисовывает ячейку поля
 * 
 * @param nfield    Поле в котором обновляем (макросы FIELD_..)
 * @param line      Строка
 * @param col       Столбец
 * @param val       Новое значение (макросы CELL_..)
 */
void graph_refcell(int nfield, int line, int col, int val)
{
    WINDOW  *refcell = cells[nfield][line][col];    // Обновляемая ячейка

    wclear(refcell);
    box(refcell, 0, 0);

    graph_print_chcell(refcell, val);
    wrefresh(refcell);
    return;
}


/**
 * Выводит список строк, предоставляя пользователю выбор одного из них
 * 
 * @param title     Заголовок списка
 * @param list      Список строк для выбора пользователя
 * @param lsize     Количество элементов(строк) в списке
 * @param selected  Выбраный элемент(строка)
 */
void graph_reflist(char *title, char **list, int lsize, int selected)
{
    int offset = 0;

    if (selected > (LINES - 4))
        offset = selected - (LINES - 4);

    clear();
    mvprintw(1, (COLS - strlen(title)) / 2 , title);
    for (int i = offset, line = 3; i < lsize && line < LINES; i++){
        if (i == selected)
            mvprintw(line++,(COLS - strlen(list[i])) / 2-3,">> %s <<",list[i]);
        else
            mvprintw(line++,(COLS - strlen(list[i])) / 2, list[i]);
    }
    refresh();
}


/**
 * Выводит список строк, предоставляя пользователю выбор одного из них
 * 
 * @param title     Заголовок списка
 * @param list      Список строк для выбора пользователя
 * @param lsize     Количество элементов(строк) в списке
 *
 * @retval          Номер выбранного элемента(строки)
 */
int graph_selectone(char *title, char **list, int lsize)
{
    int key;            // Нажатая кнопка
    int selected = 0;   // Выбраный элемент

    clear();
    graph_reflist(title, list, lsize, selected);
    while ((key = getch()) != '\n')
    {
        /**
         * Обновляем картинку только тогда, когда когда произошло смещение
         * курсора. Поэтому обновление находится в case
         */
        switch(key)
        {
            case KEY_UP:
                if (selected <= 0)
                    continue;
                selected --;
                graph_reflist(title, list, lsize, selected);
            break;
            case KEY_DOWN:
                if (selected >= lsize - 1)
                    continue;
                selected++;
                graph_reflist(title, list, lsize, selected);
            break;
        }
    }
    clear();
    refresh();
    return selected;
}