#include "../header/net.h"
#include "../header/common.h"
#include "../header/check_data.h"

void Check_data (struct CLT_DATA clt_data, struct play_field *private_field, struct play_field *user_field)
{
	switch(private_field->field[clt_data.posx][clt_data.posy])
	{
		case CELL_FREE:
		user_field->field[clt_data.posx][clt_data.posy] = CELL_MISS;
		break;
		case CELL_SHIP:
		user_field->field[clt_data.posx][clt_data.posy] = CELL_HIT;
		break;
	}
}
