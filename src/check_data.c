#include "../header/net.h"
#include "../header/common.h"
#include "../header/check_data.h"
/*
	Hokerhell create two maps with ships for player 1 (play_field Player1_map) and palyer 2  (play_field Player2_map) 
*/
int killed; //if equal 2 ship is killed
SRV_DATA Players_srv_data[2];

ifkill(CLT_DATA clt_data,SRV_DATA *srv_data,play_field Player_map)
{
	if (clt_data.posx + 1 < FIELD_COLS)
		if (srv_data->field[clt_data.posx + 1][clt_data.posy] == 3)
}

void check(CLT_DATA clt_data,SRV_DATA *srv_data,play_field Player_map)
{
	switch(Player_map[clt_data.posx][clt_data.posy])
	{
		case 0:
		srv_data->field[clt_data.posx][clt_data.posy] = 1;
		break;
		case 3:
		srv_data->field[clt_data.posx][clt_data.posy] == 2;
		//killed=0
		//ifkill(clt_data,srv_data,Player_map);
		//if (killed == 2)
		//	print_kill(clt_data,srv_data,Player_map);
		break;
	}
}

void check_data(CLT_DATA clt_data, int player)
{
	if (player)
		check(clt_data,&Players_srv_data[1],Player1_map);
	check(clt_data,&Players_srv_data[0],Player2_map);
}
