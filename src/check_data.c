#include "../header/net.h"
#include "../header/common.h"
#include "../header/check_data.h"
#include "../header/server.h"

void Check(CLT_DATA *clt_data,char ** map_pub,char **map_prv)
{
	switch(map_prv[clt_data.posx][clt_data.posy])
	{
		case CELL_FREE:
		map_pub[clt_data.posx][clt_data.posy] = CELL_MISS;
		break;
		case CELL_SHIP:
		map_pub[clt_data.posx][clt_data.posy] = CELL_HIT;
		break;
	}
}

void Check_data (CLT_DATA *clt_data, struct play_field * cli_1 ,struct play_field cli_2, int id)
{
	if (id)
		Check(clt_data,&cli_1->pub,&cli_2->prv);
	else
		Check(clt_data,&cli_2->pub,&cli_1->prv);
}
