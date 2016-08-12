#include "../header/client.h"
#include "../header/net.h"

static int create_sock() {
	int sockfd;
	struct sockaddr_in addr;

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
    addr.sin_addr.s_addr = inet_addr(ip_addr);

    return addr;
}

static void input_serv_info(unsigned short int *port, 
                            char *ip_addr) {
	int flg; 
		
	*port = 0;
	while ( !((*port > 1024) && (*port < 32000)) ) {
		printf("Введите порт сервера: ");
		scanf("%hu", port);
	}
	printf("Введите адрес сервера: ");
	fgets(ip_addr, 16, stdin);
	//Проверка корретности ip адреса
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
    int bytes_read = 0;   // кол-во принятых байт
    SRV_DATA srv_data;	  // структура от
    CLT_DATA clt_data;    // структура отправленная
    int id_user;          // идентификатор пользователя (кому пренадлежит ход)

	// Установка соединения с сервером
    sockfd = create_connect_serv();

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
	int status = 0; 
	struct timeval tv;

	// Инициализация графики
	graph_init();
	
	// Установка кораблей клиента
	graph_field_refresh(FIELD_MY, srv_data.field.pub);
	
	clt_data.posx = 0;
	clt_data.posy = 0;

	int current_step = 0;

	while(1) {
		// Оправляем ход клиента 
		if (srv_data.flg == FLG_STEP) {
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
		}
		// Принимаем результат хода
		if (recv(sockfd, &srv_data, sizeof(srv_data), 0) < 0) {
			perror("recv");
			exit(-1);
		}
		// Определяем какому игроку предоставляется ход
		if (srv_data.flg == FLG_STEP)
			current_step = FIELD_MY;
		
		if (srv_data.flg == FLG_WAIT) 
			current_step = FIELD_ENEMY;
		
		if (srv_data.flg == FLG_STEP)
			graph_field_refresh(current_step, srv_data.field);
	}
	
	close(sockfd);
	
	return 0;
}
