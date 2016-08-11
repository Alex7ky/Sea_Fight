#ifndef __GTK_GRAPHICS_H__
#define __GTK_GRAPHICS_H__

#include "common.h"
#include <gtk-3.0/gtk/gtk.h> 

#define ALL_CELLS FIELD_LINES*FIELD_COLS

/** 
 * Ячейки поля (кнопки), изображение внутри кнопки, индекс 
 */
struct cells_field {
	GtkWidget *cells_img;
	GtkWidget *cells_buttom;
	int i;
};

/**
 * Игровое поля пользователя, и противника
 */
char array_user[ALL_CELLS];
char array_enemy[ALL_CELLS];

struct cells_field *GLOBAL_STRUCT;

void exit_game(GtkWidget *window, gpointer data);
void cell_clicked (GtkWidget *widget, struct cells_field *field);
void refresh_field (struct cells_field *field, const char *cells_field);
void step_enemy (struct cells_field *field);

GtkWidget *png_box(gchar *png_filename);
GtkWidget *create_main_container(struct cells_field *field_user,
                                 struct cells_field *field_enemy);
GtkWidget *create_game_field(struct cells_field *field);
GtkWidget *create_menu(struct cells_field *field_user, 
                       struct cells_field *field_enemy);

#endif