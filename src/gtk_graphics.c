
#include "gtk_graphics.h"

/**
 * Создаём новый контейнер hbox содержащий изображение и возвращаем контейнер.
 */
GtkWidget *png_box(gchar *png_filename)
{    
	GtkWidget *box;      // коробка в которую будем упоковывать изоброжение  	 
	GtkWidget *image;    // создаём контейнер для изображения и текста 
	
	box = gtk_box_new (FALSE, 0);   
	gtk_container_set_border_width (GTK_CONTAINER(box), 0); 
	
	// определяем файл с изображением    
	image = gtk_image_new_from_file (png_filename);

	// упаковываем изображение в контейнер
	gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);   
	gtk_widget_show (image);    

	return box;
}

/**
 * Завершение игры, закрытие окна
 */ 
void exit_game(GtkWidget *window, gpointer data) 
{
	gtk_main_quit();
}

/**
 * Обработка нажатия кнопки, на одну из ячеек игрового поля
 */
void cell_clicked (GtkWidget *widget, struct cells_field *field)
{   

	if (array_enemy[field->i] == CELL_MISS) return;
	if (array_enemy[field->i] == CELL_HIT) return;

	int posx, posy;
	
	posx = field->i / 10;
	posy = field->i % 10;

//	send(posx, posy);
//  recv(field);
//  array_enemy[] = field[i][j]

	int i;
	for (i = 0; i < ALL_CELLS; i++)
		array_enemy[i] = CELL_FREE;
	refresh_field(field, array_enemy);

	return; 
}

void refresh_field (struct cells_field *field, const char *cells_field)
{
	int i;

	for (i = 0; i < ALL_CELLS; i++) {
		if (cells_field[i] == CELL_FREE)
			field[i].cells_img = png_box("o.png");
		if (cells_field[i] == CELL_MISS)
			field[i].cells_img = png_box("s.png");
		if (cells_field[i] == CELL_HIT)
			field[i].cells_img = png_box("kr.png");
		if (cells_field[i] == CELL_SHIP)
			field[i].cells_img = png_box("k.png");

		gtk_button_set_image(GTK_BUTTON(field[i].cells_buttom), field[i].cells_img);
	}	
}

void step_enemy (struct cells_field *field)
{       
	refresh_field(field, array_enemy);

	return; 
}


/**
 * Создаем основной контейнер, в который будет помещаться, 
 * все остальные элементы
 */
GtkWidget *create_main_container(struct cells_field *field_user,
								struct cells_field *field_enemy) {

	GtkWidget *osnovnoe_okno;
	GtkWidget *playing_field_box, *playing_field_1, *playing_field_2;
	GtkWidget *menu;

	/* Создаем основную коробку куда будут упаковываться все остальные виджеты */
	osnovnoe_okno = gtk_box_new(TRUE, 0);
	playing_field_box = gtk_box_new(FALSE, 5);
	
	menu = create_menu(field_user, field_enemy); 
	gtk_box_pack_start(GTK_BOX(osnovnoe_okno), menu, FALSE, FALSE, 10);

	/* Упаковка самой таблицы в основное окно */
	playing_field_1 = create_game_field(field_user);
	playing_field_2 = create_game_field(field_enemy);
	
	gtk_box_pack_start(GTK_BOX(playing_field_box), playing_field_1, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(playing_field_box), playing_field_2, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(osnovnoe_okno), playing_field_box, TRUE, FALSE, 0);
	return (osnovnoe_okno);
}   

/**
 * Создание игрового поля 
 */
GtkWidget *create_game_field(struct cells_field *field) {
	GtkWidget *osnovnoe_okno;
	GtkWidget *g_widget;
	GtkWidget *playing_field;
	GtkWidget *abc_widget, *abc_field, *abc_label;
	GtkWidget *label_field, *label_symbol;
	gchar abc[5];
	gchar number[5];
	int  i, j, g;
	
	/* Создаем основную коробку куда будут упаковываться все остальные виджеты */
	osnovnoe_okno = gtk_box_new(TRUE, 0);
	g_widget = gtk_box_new(FALSE, 5);
	abc_widget = gtk_box_new(FALSE, 0);
	abc_field = gtk_grid_new ();
	for (i = 0; i < 10; i++) {
		abc[0] = 'A' + i;
		abc[1] = '\0';
		abc_label = gtk_label_new(abc);
		gtk_widget_set_size_request (GTK_WIDGET(abc_label), 37, 1);
		gtk_grid_attach(GTK_GRID(abc_field), abc_label, i, 0, 1, 1);   
	}
	
	
	gtk_box_pack_start(GTK_BOX(abc_widget), abc_field, FALSE, FALSE, 23);
	gtk_box_pack_start(GTK_BOX(osnovnoe_okno), abc_widget, FALSE, FALSE, 0);
	
	label_field = gtk_grid_new ();
	for (i = 0; i < 10; i++) {
		number[0] = '0' + i + 1;
		number[1] = '\0';
		if (i == 9) { 
			number[0] = '1';
			number[1] = '0';
			number[2] = ' ';
			number[3] = '\0';
		}
		label_symbol = gtk_label_new(number);
		gtk_widget_set_size_request (GTK_WIDGET(label_symbol), 1, 34);
		gtk_grid_attach(GTK_GRID(label_field), label_symbol, 0, i, 1, 1);   
	}
	
	/* Создаем таблицу с контейнерами (10 на 10) */
	playing_field = gtk_grid_new ();
	gtk_grid_set_row_spacing(GTK_GRID (playing_field), 1);
	gtk_grid_set_column_spacing (GTK_GRID(label_field), 1);
	for (i = 0, j = 0, g = 0; i < ALL_CELLS; i++, j++) {
		field[i].cells_buttom = gtk_button_new();
		gtk_widget_set_size_request (GTK_WIDGET(field[i].cells_buttom), 30, 30);
		field[i].cells_img = png_box("o.png");
		gtk_button_set_image(GTK_BUTTON(field[i].cells_buttom), field[i].cells_img);
		if (j >= FIELD_LINES) {
			j = 0;
			g++;
		}
		/* Упаковка изображения в таблицу */
		gtk_grid_attach(GTK_GRID(playing_field), field[i].cells_buttom, g, j, 1, 1);
	}
	
	for (i = 0; i < ALL_CELLS; i++) {
		field[i].cells_img = png_box("o.png");
		gtk_button_set_image(GTK_BUTTON(field[i].cells_buttom), field[i].cells_img);
	}
	
	gtk_box_pack_start(GTK_BOX(g_widget), label_field, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(g_widget), playing_field, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(osnovnoe_okno), g_widget, FALSE, FALSE, 10);
	
	return osnovnoe_okno;
}

/** 
 * Создание виджета меню
 */
GtkWidget *create_menu(struct cells_field *field_user, 
						struct cells_field *field_enemy) {

	GtkWidget *menu;
	GtkWidget *menu_bar;
	GtkWidget *menu_items_1;
	GtkWidget *root_menu;
	char buf[50] = "Генерация кораблей";

	menu = gtk_menu_new();
	menu_items_1 = gtk_menu_item_new_with_label (buf);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_items_1);    
	//g_signal_connect_swapped (G_OBJECT (menu_items_1), "activate", G_CALLBACK (restart_game), field_user);   
	gtk_widget_show (menu_items_1);

	root_menu = gtk_menu_item_new_with_label ("Настройки");
	gtk_widget_show (root_menu);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);

	menu_bar = gtk_menu_bar_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), root_menu);

	return menu_bar;
}


int main(int argc, char *argv[])
{   
	GtkWidget *main_window;
	GtkWidget *main_container;

	gtk_init(&argc, &argv); 
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (GTK_WIDGET(main_window), 600, 400);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(main_window), "Морской бой");
	gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 2);
	
	struct cells_field field_user[ALL_CELLS];
	struct cells_field field_enemy[ALL_CELLS];
	GLOBAL_STRUCT = field_user;

	main_container = create_main_container(field_user, field_enemy);
	gtk_container_add(GTK_CONTAINER(main_window), main_container);
	
	int i;  
	
	// определяем сигнал нажатия для каждой ячейки игрового поля
	// присваеваем сигнал cell_clicked к данному сигналу
	for (i = 0; i < ALL_CELLS; i++) {
		field_user[i].i = i;
		field_enemy[i].i = i;
		g_signal_connect(G_OBJECT(field_enemy[i].cells_buttom), "clicked", G_CALLBACK (cell_clicked), field_enemy); 
	}

	// определяем сигнал выхода (закрытия окна), 
	// присваиваем функцию exit_game к данному сигналу
	g_signal_connect_swapped(G_OBJECT(main_window), "destroy", G_CALLBACK(exit_game), NULL);

	gtk_widget_show_all(main_window);
	
	gtk_main();

	return 0;
}
