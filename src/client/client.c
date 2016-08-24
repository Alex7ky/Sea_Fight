#include "../../header/client.h"

int CreateSock() {
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	return sockfd;
}

void InitSockaddrIn(struct sockaddr_in *addr, const unsigned short int port,
                    const char *ip_addr) 
{
	addr->sin_family      = AF_INET;
	addr->sin_port        = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip_addr);
}

void InputServInfo(unsigned short int *port, 
                   char *ip_addr) {

	printf("Введите адрес сервера: ");

	if (fgets(ip_addr, 16, stdin) == NULL) {
		perror("fgets"); 
		exit(-1);
	}

	*port = 0;
	while ( !((*port > MIN_VAL) && (*port < MAX_VAL)) ) {
		printf("Введите порт сервера: ");
		scanf("%hu", port);
	}
}

int CreateConnectServ() {
	int sockfd;
	struct sockaddr_in addr;
	unsigned short int serv_port;
	char serv_ip_addr[16];

	sockfd = CreateSock();

	InputServInfo(&serv_port, serv_ip_addr);

	InitSockaddrIn(&addr, serv_port, serv_ip_addr);

	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect");
		exit(-1);
	}

	return sockfd;
}

int main (void)
{
	int sockfd;
	SRV_DATA srv_data;
	CLT_DATA clt_data;

	/* Установка соединения с сервером */
	sockfd = CreateConnectServ();
	printf("Waiting the second player..\n");

	memset(&clt_data, 0, sizeof(clt_data));

	/* Формируем запрос на генерацию кораблей */
	clt_data.flg = FLG_GEN_SHIPS;

	/* Отправляем запрос на сервер */
	if (send(sockfd, &clt_data, sizeof(clt_data), 0) < 0) {
		perror("Error send data (generation field)");
		exit(-1);
	}

	/* Ожидаем структуру с нашим полем */
	if (recv(sockfd, &srv_data, sizeof(srv_data), 0) < 0) {
		perror("Error recv data (generation field)");
		exit(-1);
	}

	if (srv_data.flg == FLG_EXIT) {
		printf("The second player left the game.\n");
		exit(-1);
	}

	int status = 0; 
	struct timeval tv;

	/* Инициализация графики */
	status = GraphInit();

	if (status == GRAPH_SMALL_WIND) {
		printf("Error small window.\n");
		exit(-1);
	}

	/* Установка кораблей клиента */
	GraphFieldRefresh(FIELD_MY, &srv_data.field);
	GraphFieldRefresh(FIELD_ENEMY, &srv_data.field);

	clt_data.posx = 0;
	clt_data.posy = 0;

	int flg_step = 0;

	while(1) {
		/* Принимаем информацию хода */
		if (recv(sockfd, &srv_data, sizeof(srv_data), 0) < 0) {
			perror("Error recv data (information step)");
			exit(-1);
		}

		if (flg_step == 1)
			GraphCellRefresh(FIELD_ENEMY, srv_data.posx, srv_data.posy, 
			                 srv_data.field.pub[srv_data.posx][srv_data.posy]);  
		if (flg_step == 0)
			GraphCellRefresh(FIELD_MY, srv_data.posx, srv_data.posy, 
			                 srv_data.field.prv[srv_data.posx][srv_data.posy]);

		/* Оправляем ход клиента */
		if (srv_data.flg == FLG_STEP) {
			GraphPrintMsg("Your step!\n");

			tv.tv_sec = 120;
			tv.tv_usec = 0;

			status = GraphCellGet(&clt_data.posx, &clt_data.posy, &tv);

			if (status == GRAPH_TIMEOUT) {
				clt_data.flg = FLG_EXIT;
			} else {
				clt_data.flg = FLG_STEP;
			}

			if (send(sockfd, &clt_data, sizeof(clt_data), 0) < 0) {
				perror("Error send data (information step)");
				exit(-1);
			}

			flg_step = 1;
		} 

		if (srv_data.flg == FLG_WAIT) {
			GraphPrintMsg("Please, waiting your step!\n");
			flg_step = 0;
		}
		if (srv_data.flg == FLG_EXIT) {
			GraphPrintMsg("Your opponent exit!");
			sleep(3);
			GraphDestroy();
			exit(-1);
		}
		if (srv_data.flg == FLG_WINNER) {
			GraphPrintMsg("You WINNER!");
			sleep(3);
			GraphDestroy();
			return 0;
		}
		if (srv_data.flg == FLG_LOSE) {
			GraphPrintMsg("You lost!");
			sleep(3);
			GraphDestroy();
			return 0;
		}
	}

	GraphDestroy();
	close(sockfd);

	return 0;
}
