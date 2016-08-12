#include "../header/net.h"
#include "../header/common.h"
#include "../header/server.h"

void Check_data(CLT_DATA *clt_data, struct play_field *field , int *current_ships)
{
	switch(field->prv[clt_data->posx][clt_data->posy])
	{
		case CELL_FREE:
		field->pub[clt_data->posx][clt_data->posy] = CELL_MISS;
		field->prv[clt_data->posx][clt_data->posy] = CELL_MISS;
		break;
		case CELL_SHIP:
		field->pub[clt_data->posx][clt_data->posy] = CELL_HIT;
		field->prv[clt_data->posx][clt_data->posy] = CELL_HIT;
		break;
	}
}
