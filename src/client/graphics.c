#include "../header/graphics.h"

/* Цветовые пары */
#define COLOR_CELL_DEFAULT  0
#define COLOR_CELL_ACTIVE   1
#define COLOR_CELL_HIT      2
#define COLOR_CELL_SHIP     3

/**
 * Ячейки игрового поля. 2 - количество игровых полей. Подразумевается, то 
 * есть только поле игрока и поле его противника
 */
static WINDOW *cells[2][FIELD_LINES][FIELD_COLS];


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
    curs_set(0);

    // Включение цветов и инициализация цветовых пар
    start_color();
    assume_default_colors(COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_CELL_DEFAULT, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_CELL_ACTIVE, COLOR_RED, COLOR_BLUE);
    init_pair(COLOR_CELL_HIT, COLOR_WHITE, COLOR_RED);
    init_pair(COLOR_CELL_SHIP, COLOR_BLUE, COLOR_WHITE);

    // Проверяем вместится ли изображение
    if (COLS <= FIELD_COLS * 10 + 10 || LINES <= FIELD_LINES * 3 + 8){
        endwin();
        return GRAPH_SMALL_WIND;
    }

    // Создаем окна-ячейки игровых полей
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
 * Выводит обозначающий символ в ячейку в зависимости от значения в игровом 
 * поле.  
 * 
 * @param cell  Ячейка
 * @param val   Соответствующее значение в игровом поле (макросы FIELD_..)
 */
static void graph_cell_print(WINDOW *cell, int val)
{
    switch(val){
        case CELL_MISS:
            mvwprintw(cell, 1, 2, "O");
        break;

        case CELL_HIT:
            wattron(cell, COLOR_PAIR(COLOR_CELL_HIT));
            mvwprintw(cell, 1, 1, "   ");
            wattroff(cell, COLOR_PAIR(COLOR_CELL_HIT));
        break;

        case CELL_SHIP:
            wattron(cell, COLOR_PAIR(COLOR_CELL_SHIP));
            mvwprintw(cell, 1, 1, "   ");
            wattroff(cell, COLOR_PAIR(COLOR_CELL_SHIP));
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
void graph_field_refresh(int nfield, struct play_field *pfield)
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
			if (nfield == FIELD_MY)
            	graph_cell_print(current, pfield->prv[i][j]);
            
			if (nfield == FIELD_ENEMY)
				graph_cell_print(current, pfield->pub[i][j]);
            
			wrefresh(current);
            usleep(40000);
        }
    }

    // Определяем верхнюю правую границу поля
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
void graph_cell_refresh(int nfield, int line, int col, int val)
{
    WINDOW  *refcell = cells[nfield][line][col];    // Обновляемая ячейка

    wclear(refcell);
    box(refcell, 0, 0);

    graph_cell_print(refcell, val);
    wrefresh(refcell);
    return;
}


/**
 * Выделяет ячейку (Перекрашивает ее рамку)
 * 
 * @param line      Строка ячейки
 * @param col       Столбец ячейки
 * @param numpair   Цветовая пара (макросы COLOR_CELL_..)
 */
void graph_cell_repaint(int line, int col, int numpair)
{
    WINDOW *cell = cells[FIELD_ENEMY][line][col];   // Выделяемая ячека

    wattron(cell, COLOR_PAIR(numpair));
    box(cell, 0, 0);
    wattroff(cell, COLOR_PAIR(numpair));
    wrefresh(cell);
    return;
}

/**
 * Предоставляет пользователю возможность выбрать ячейку вражеского поля
 * 
 * @param line      Строка выбранной ячейки (будет помещен)
 * @param col       Столбец выбранной ячейки (будет помещен)
 * @param time      Время для хода
 *
 * @retval GRAPH_TIMEOUT Закончилось время выделенное для хода
 * @retval 0             Успешное завершение
 */
int graph_cell_get(int *line, int *col, struct timeval *time)
{
    int     key;                // Нажатая кнопка
    int     prev_line = *line;  // Предыдыдущее значение строки
    int     prev_col = *col;    // Предыдыдущее значение столбца
    fd_set  set;                // Набор отслеживаемых дескрипторов

    graph_cell_repaint(*line, *col, COLOR_CELL_ACTIVE);
    while (1){   
        // Отслеживаем событие ввода клавиши
        FD_ZERO(&set);
        FD_SET(0, &set);
        if (select(1, &set, NULL, NULL, time) == 0)
            return GRAPH_TIMEOUT;
        key = getch();

        // Запоминаем координаты предыдущей ячейки
        prev_col = *col;
        prev_line = *line;
        switch(key)
        {
            case KEY_UP:
                if (*line > 0)
                    (*line)--;  
            break;
            case KEY_DOWN:
                if (*line < FIELD_LINES - 1)
                    (*line)++;
            break;
            case KEY_LEFT:
                if (*col > 0)
                    (*col)--;
            break;
            case KEY_RIGHT:
                if (*col < FIELD_COLS - 1)
                    (*col)++;
            break;
            case '\n':
                return 0;
            break;
        }

        // Снимаем выделение с предыдущей ячейки и выделяем новую
        graph_cell_repaint(prev_line, prev_col, COLOR_CELL_DEFAULT);
        graph_cell_repaint(*line, *col, COLOR_CELL_ACTIVE);
    }

    refresh();
    return 0;
}


/**
 * Выводит список строк, выделяя выбранную
 * 
 * @param title     Заголовок списка
 * @param list      Список строк для выбора пользователя
 * @param lsize     Количество элементов(строк) в списке
 * @param selected  Выбраный элемент(строка)
 */
void graph_item_print(char *title, char **list, int lsize, int selected)
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
int graph_item_get(char *title, char **list, int lsize)
{
    int key;            // Нажатая кнопка
    int selected = 0;   // Выбраный элемент

    clear();
    graph_item_print(title, list, lsize, selected);
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
                graph_item_print(title, list, lsize, selected);
            break;
            case KEY_DOWN:
                if (selected >= lsize - 1)
                    continue;
                selected++;
                graph_item_print(title, list, lsize, selected);
            break;
        }
    }
    clear();
    refresh();
    return selected;
}

/**
 * Выводит сообщение пользователю в последних двух строках окна
 * 
 * @param msg     Выводимое сообщение
 */
void graph_print_msg(char *msg)
{
    // Очищаем последние 2 строки окна
    move(LINES -2, 0);
    clrtoeol();
    move(LINES -1, 0);
    clrtoeol();

    // Выводим сообщение
    mvprintw(LINES - 2, (COLS - strlen(msg)) / 2 , msg);
    refresh();
}

