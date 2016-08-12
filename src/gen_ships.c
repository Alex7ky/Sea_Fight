#include "../header/common.h"
#include "../header/gen_ships.h"

/**
 * Функция проверки ячеек.
 */
bool freedom(int x, int y, struct play_field *my_play_field)
{
	int d[8][2] = {{0,1},{1,0},{0,-1},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1}};
	int i, dx, dy;
	bool ok;
	if((x >= 0) && (x < FIELD_COLS) && (y >= 0) 
		&& (y < FIELD_LINES) && (my_play_field->prv[x][y] == CELL_FREE)) {
		for(i = 0; i < 8; i++) {
			dx = x + d[i][0];
			dy = y + d[i][1];
			
			if ((dx >= -1) && (dx < FIELD_COLS) && (dy >= -1) 
				&& (dy < FIELD_LINES) && (my_play_field->prv[dx][dy] == CELL_FREE)) {	
				ok = true;
			}
			else {
				ok = false;
				return ok;
			}

		}
	}
	else 
		ok = false;
	return ok;
}; 

/**
 * Функция генерации кораблей.
 */
void gen_ships(struct play_field *my_play_field)
{	
	srand(time(NULL));
	int N,M;
	int i,x,y,kx,ky;
	bool ok;
	for(x = 0; x < FIELD_COLS; x++)
		for(y = 0; y < FIELD_LINES; y++)
			my_play_field->prv[x][y] = CELL_FREE;
	for(N = 4; N > 0; N--) {
		for(M = 0; M <= (4 - N); M++)
			do {
				x  = rand() % FIELD_COLS;
				y  = rand() % FIELD_LINES;
				
				kx = rand() % 2;
				if(kx == 0) 
					ky = 1;
				else
					ky = 0;
				ok = true;
				for(i = 0; i < N; i++)	
					if(!freedom(x + kx * i, y + ky * i, my_play_field))
						ok = false;
				if(ok)
					for(i = 0; i < N; i++)
						my_play_field->prv[x + kx * i][y + ky * i] = CELL_SHIP;
                                
                                /*printf("x %d - y %d\n", x, y);
                                for(x = 0; x < FIELD_COLS; x++){
		                        for(y = 0; y < FIELD_LINES; y++)
			                        printf("%d ", my_play_field->prv[x][y]);
			                printf("\n");
			        }
			        printf("_______________________________________\n");*/
 			}
			while(!ok);
			
	}
};
