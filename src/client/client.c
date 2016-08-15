#include "../../header/client.h"

static int create_sock() {
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// создание TCP-сокета
    
    if(sockfd < 0) {
        perror("socket");
        exit(-1);
    }

	return sockfd;
}

static struct sockaddr_in create_sockaddr_in(const unsigned short int port, 
	                                     const char *ip_addr) 
{
	struct sockaddr_in addr;

    //задаем параметры сервера
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr("192.168.2.34");//ip_addr);

    return addr;
}

static void input_serv_info(unsigned short int *port, 
                            char *ip_addr) {

	printf("Введите адрес сервера: ");
	
	if (fgets(ip_addr, 16, stdin) == NULL) {
		perror("fgets"); 
		exit(-1);
	}
	//Проверка корретности ip адреса
		
	*port = 0;
	while ( !((*port > 1024) && (*port < 32000)) ) {
		printf("Введите порт сервера: ");
		scanf("%hu", port);
	}
}

static int create_connect_serv() {
	int sockfd;
	struct sockaddr_in addr;
	unsigned short int serv_port;
    char serv_ip_addr[16];

    sockfd = create_sock();
    
    input_serv_info(&serv_port, serv_ip_addr);

    addr = create_sockaddr_in(serv_port, serv_ip_addr);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(-1);
    }

    return sockfd;
}

int main (void)
{
	int sockfd;
    SRV_DATA srv_data;	  // структура от
    CLT_DATA clt_data;    // структура отправленная

	// Установка соединения с сервером
    sockfd = create_connect_serv();
	printf("Ожидаем второго игрока..\n");

   	// Формируем запрос на генерацию кораблей
    clt_data.flg = FLG_GEN_SHIPS;

	// Отпалвяем запрос на сервер
	if (send(sockfd, &clt_data, sizeof(clt_data), 0) < 0) {
		perror("send");
		exit(-1);
	}
	
	// Ожидаем структуру с нашим полем 
	if (recv(sockfd, &srv_data, sizeof(srv_data), 0) < 0) {
		perror("recv");
		exit(-1);
	}

	if (srv_data.flg == FLG_EXIT) {
		printf("Второй игрок покинул игру");
		exit(-1);
	}

	int status = 0; 
	struct timeval tv;

	// Инициализация графики
	status = graph_init();

	if (status == GRAPH_SMALL_WIND) {
		printf("err: small window!\n");
		exit(-1);
	}
	// Установка кораблей клиента
	graph_field_refresh(FIELD_MY, &srv_data.field);
	graph_field_refresh(FIELD_ENEMY, &srv_data.field);
	
	printf("test0\n");
	
	clt_data.posx = 0;
	clt_data.posy = 0;

	int flg_step = 0;

	while(1) {
		// Принимаем информацию хода
		if (recv(sockfd, &srv_data, sizeof(srv_data), 0) < 0) {
			perror("recv");
			exit(-1);
		}
		graph_print_msg("Recv\n");
		
		if (flg_step == 1)
			graph_cell_refresh(FIELD_ENEMY, srv_data.posx, srv_data.posy, 
					srv_data.field.pub[srv_data.posx][srv_data.posy]);	
		if (flg_step == 0)
				graph_cell_refresh(FIELD_MY, srv_data.posx, srv_data.posy, 
					srv_data.field.prv[srv_data.posx][srv_data.posy]);

		// Оправляем ход клиента 
		if (srv_data.flg == FLG_STEP) {
			graph_print_msg("Your step!\n");
	
			tv.tv_sec = 120;
			tv.tv_usec = 0;

			status = graph_cell_get(&clt_data.posx, &clt_data.posy, &tv);

			if (status == GRAPH_TIMEOUT) {
				clt_data.flg = FLG_EXIT;
			} else {
				clt_data.flg = FLG_STEP;
			}
		
			if (send(sockfd, &clt_data, sizeof(clt_data), 0) < 0) {
				perror("send");
				exit(-1);
			}

			flg_step = 1;
		} 

		if (srv_data.flg == FLG_WAIT) {
			graph_print_msg("Please wait enemy!\n");
			flg_step = 0;
		}
		if (srv_data.flg == FLG_EXIT) {
			graph_print_msg("Player 2 Exit!");
			sleep(3);
			graph_destroy();
			exit(-1);
		}
		if (srv_data.flg == FLG_WINNER) {
			graph_print_msg("You WINNER!");
			sleep(3);
			graph_destroy();
			return 0;
		}
		if (srv_data.flg == FLG_LOSE) {
			graph_print_msg("You lose!");
			sleep(3);
			graph_destroy();
			return 0;
		}
	}
	
	graph_destroy();
	close(sockfd);
	
	return 0;
}
