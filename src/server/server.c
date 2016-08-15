#include "../../header/server.h"

int main()
{
        SERVER_S *server = NULL;
        
        printf("Welcome to SeaFight server!\n");
        if( (server = InitServer()) == NULL){
                write(2, "Error of create server", strlen("Error of create server"));
                exit(1);
        }
        if(InitServices(server) != 0){
                RemoveServer(server);
                write(2, "Error of create server", strlen("Error of create server"));
                exit(1);
        }
        printf("Server started. Use 'info'.\n");
        
        InitCommandLine(server);

        RemoveServer(server);
        
        return 0;
}
/*
        Инициализируем сервер
        создаем сокет, привязываем его к ip и порту
        ip - вводит пользователь
        порт - сервер назначает самостоятельно
*/
SERVER_S *InitServer()
{
        int port;
        char log_data[128];
        SERVER_S *serv;
        socklen_t addr_len;
        
        serv = malloc(sizeof(SERVER_S));
        if(serv == NULL)
                return NULL;        
        pthread_mutex_init(&serv->mutex, NULL);
        /*
                Включаем логирование
        */
        serv->log_d = open("server.sf.log", O_CREAT | O_WRONLY, 0666);
        if(serv->log_d <= 0){
                perror("Error on create log file");
                serv->log_d = -1;
        }
        serv->fl_log = 0;        
        
        /*
                Создаем сокет
        */
        serv->sock_d = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(serv->sock_d <= 0){
                perror("Error on create socket");
                free(serv);
                return NULL;
        }
        serv->addr.sin_family = AF_INET;
        /*
                Получение ip
        */
        GetIP(serv);       
        /*
                Поиск порта и привязка сокета
        */
        addr_len = sizeof(struct sockaddr_in);
        port = FPSTART;
        while(port != FPLEN){
                serv->addr.sin_port = htons(port);
                if(bind(serv->sock_d, (struct sockaddr *) &serv->addr, addr_len) < 0){
                        port++;
                } else {
                        break;
                }
        }
        if(port == FPLEN){
                serv->addr.sin_port = htons(port);
                if(bind(serv->sock_d, (struct sockaddr *) &serv->addr, addr_len) < 0){
                        perror("Error on binding socket");
                        goto err_exit_1;
                }
        }
        if(listen(serv->sock_d, 10) != 0){
                perror("Error listen socket");
                goto err_exit_1;
        }
        /*
                "Обнуление" некоторых данных
        */
        serv->client_1 = -1;
        serv->client_2 = -1;
        
        serv->client_1_ships = 10;
        serv->client_2_ships = 10;
        
        memset(&serv->client_1_field, 0, sizeof(struct play_field));
        memset(&serv->client_2_field, 0, sizeof(struct play_field));
        
        sprintf(log_data,"Server is started with IP %s and with port %d", inet_ntoa(serv->addr.sin_addr), port);
        PrintLOG(serv, log_data);
        
        return serv;
        
        err_exit_1:
                close(serv->sock_d);
                free(serv);
                return NULL;
}
/*
        Удаляем всю структуру сервера
        останавливаем потоки, закрываем сокеты и освобождаем память
*/
int RemoveServer(SERVER_S *serv)
{
        if(serv == NULL)
                return -1;      
        
        pthread_cancel(serv->network);
        close(serv->client_1);
        close(serv->client_2);
        close(serv->sock_d);
        PrintLOG(serv, "Server closed\n\n");
        close(serv->log_d);
        free(serv);
        
        return 0;
}
/*
        Запускаем все сервисы
                network - обеспечивает взаимодействие клиентов с сервером
*/
int InitServices(SERVER_S *serv)
{
        if( pthread_create(&serv->network, NULL, NetworkService, serv) < 0 ){
                perror("Error on create service for network");

                return -1;
        }
        
        return 0;
}
/*
        Поток сервиса network
*/
void *NetworkService(void *args)
{
        SERVER_S *serv = (SERVER_S *)args;
        CLT_DATA client_msg;
        int rd;
        int stat;
         
        InitRegistration(serv);
        /*
                Процесс игры
        */        
        PrintLOG(serv, "Start game");
        while(1){
                /*
                        Первый игрок
                */
                do{
                        rd = recv(serv->client_1, &client_msg, SIZE_CLT_DATA, 0);
                        if( rd < 0){
                                perror("Error on receive message from client 1");
                        }
                        if(rd > 0)
                                stat = CreateAnswer(serv, client_msg, 1);
                        else {                        
                                stat = 2;
                                client_msg.flg = FLG_EXIT;
                                CreateAnswer(serv, client_msg, 1);
                                PrintLOG(serv, "Client 1 is out");
                        }
                } while (stat != 0);
                /*
                        Второй игрок
                */
                do{
                        rd = recv(serv->client_2, &client_msg, SIZE_CLT_DATA, 0);
                        if( rd < 0){
                                perror("Error on receive message from client 2");
                        }
                        if(rd > 0)
                                stat = CreateAnswer(serv, client_msg, 2);
                        else {                        
                                stat = 2;
                                client_msg.flg = FLG_EXIT;
                                CreateAnswer(serv, client_msg, 2);
                                PrintLOG(serv, "Client 2 is out");
                        }
                  } while (stat != 0);
                  
                  if(stat == 2)
                        break;
        }
        
        PrintLOG(serv, "End of game");
        if(pthread_mutex_lock(&serv->mutex) == 0){
                close(serv->client_1);
                serv->client_1 = -1;
                close(serv->client_2);
                serv->client_2 = -1;
                close(serv->sock_d);
                serv->sock_d = -1;
                pthread_mutex_unlock(&serv->mutex);
        }
        pthread_exit(0);
}
/*
        Обрабатываем входящие сообщения и отвечаем
*/
int CreateAnswer(SERVER_S *serv, CLT_DATA msg, int from)
{
        SRV_DATA server_msg;
        int client;
        int client_ships;
        int shot_rez;
        int stat = 0;
        
        memset(&server_msg.field, 0, sizeof(struct play_field));
        switch(msg.flg){
                case FLG_GEN_SHIPS:                        
                        if(from == 1){
                                PrintLOG(serv, "New packet from client 1: FLG_GEN_SHIPS");
                                gen_ships(&server_msg.field);
                                client = serv->client_1;
                                server_msg.flg = FLG_GEN_SHIPS;
                                memcpy(&serv->client_1_field, &server_msg.field, sizeof(struct play_field));
                                if(send(client, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 1");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 1");
                        } else {
                                PrintLOG(serv, "New packet from client 2: FLG_GEN_SHIPS");
                                /*
                                        Засыпаем на время. Иначе генерирует одинаковые карты
                                */
                                sleep(1);
                                gen_ships(&server_msg.field);
                                client = serv->client_2;
                                server_msg.flg = FLG_GEN_SHIPS;
                                memcpy(&serv->client_2_field, &server_msg.field, sizeof(struct play_field));
                                if(send(client, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 2");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 2");
                                /*
                                        Оповещение о начале игры
                                */
                                memset(&server_msg.field, 0, sizeof(struct play_field));
                                server_msg.posx = 0;
                                server_msg.posy = 0;                               
                                server_msg.flg = FLG_STEP;
                                //memcpy(server_msg.field.prv, serv->client_1_field.prv, sizeof(char) * FIELD_COLS * FIELD_LINES);
                                if(send(serv->client_1, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 1");                                        
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 1");
                                server_msg.flg = FLG_WAIT;
                                //memcpy(server_msg.field.prv, serv->client_2_field.prv, sizeof(char) * FIELD_COLS * FIELD_LINES);
                                if(send(serv->client_2, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 2");                                        
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 2");
                        }                        
                        
                break;
                case FLG_STEP:
                        /*
                                Запоминаем вражеский ход
                        */
                        server_msg.posx = msg.posx;
                        server_msg.posy = msg.posy;
                        
                        if(from == 1){
                                PrintLOG(serv, "New packet from client 1: FLG_STEP");
                                client = serv->client_1;
                                server_msg.flg = FLG_STEP;                                
                                /*
                                        "Стреляем" по второму игроку
                                */
                                client_ships = serv->client_2_ships;
                                shot_rez = Check_data(&msg, &serv->client_2_field , &client_ships);
                                /*
                                        Сообщение первому игроку
                                */
                                if(shot_rez == CELL_MISS)
                                        server_msg.flg = FLG_WAIT;
                                else {
                                        server_msg.flg = FLG_STEP;
                                        stat = 1;
                                }
                                if(pthread_mutex_lock(&serv->mutex) == 0){
                                        if(serv->client_2_ships > client_ships){
                                                /*
                                                        Если корабль потоплен
                                                */
                                                //какое-то действие
                                                serv->client_2_ships = client_ships;
                                        }                                        
                                        pthread_mutex_unlock(&serv->mutex);
                                }
                                if(client_ships < 1){
                                        /*
                                                Если первый игрок победил
                                        */
                                        server_msg.flg = FLG_WINNER;
                                }
                                memcpy(server_msg.field.pub, serv->client_2_field.pub, sizeof(char) * FIELD_COLS * FIELD_LINES);
                                if(send(serv->client_1, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 1");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 1");
                                /*
                                        Сообщение второму игроку
                                */
                                if(shot_rez == CELL_MISS)
                                        server_msg.flg = FLG_STEP;
                                else
                                        server_msg.flg = FLG_WAIT;
                                if(client_ships < 1){
                                        /*
                                                Если первый игрок победил
                                        */
                                        server_msg.flg = FLG_LOSE;
                                }
                                memcpy(server_msg.field.prv, serv->client_2_field.prv, sizeof(char) * FIELD_COLS * FIELD_LINES);
                                if(send(serv->client_2, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 2");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 2");
                        } else {
                                PrintLOG(serv, "New packet from client 2: FLG_STEP");
                                client = serv->client_2;
                                server_msg.flg = FLG_WAIT;
                                /*
                                        "Стреляем" по первому игроку
                                */
                                client_ships = serv->client_1_ships;
                                shot_rez = Check_data(&msg, &serv->client_1_field , &client_ships);                                
                                /*
                                        Сообщение второму игроку
                                */
                                if(shot_rez == CELL_MISS)
                                        server_msg.flg = FLG_WAIT;
                                else {
                                        server_msg.flg = FLG_STEP;
                                        stat = 1;
                                }
                                if(pthread_mutex_lock(&serv->mutex) == 0){
                                        if(serv->client_1_ships > client_ships){
                                                /*
                                                        Если корабль потоплен
                                                */
                                                //какое-то действие
                                                serv->client_1_ships = client_ships;
                                        }                                        
                                        pthread_mutex_unlock(&serv->mutex);
                                }
                                if(client_ships < 1){
                                        /*
                                                Если второй игрок победил
                                        */
                                        server_msg.flg = FLG_WINNER;
                                }                                
                                memcpy(server_msg.field.pub, serv->client_1_field.pub, sizeof(char) * FIELD_COLS * FIELD_LINES);
                                if(send(serv->client_2, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 2");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 2");
                                /*
                                        Сообщение первому игроку
                                */
                                if(shot_rez == CELL_MISS)
                                        server_msg.flg = FLG_STEP;
                                else
                                        server_msg.flg = FLG_WAIT;
                                if(client_ships < 1){
                                        /*
                                                Если второй игрок победил
                                        */
                                        server_msg.flg = FLG_LOSE;
                                }
                                memcpy(server_msg.field.prv, serv->client_1_field.prv, sizeof(char) * FIELD_COLS * FIELD_LINES);
                                if(send(serv->client_1, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 1");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 1");
                        }                        
                break;
                case FLG_EXIT:
                        stat = 2;
                        if(from == 1){
                                PrintLOG(serv, "New packet from client 1: FLG_EXIT");
                                /*
                                        Первый игрок вышел
                                        говорим об этом второму игроку
                                */
                                server_msg.flg = FLG_EXIT;
                                if(send(serv->client_2, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 2");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 2");
                        } else {
                                PrintLOG(serv, "New packet from client 2: FLG_EXIT");
                                /*
                                        Второй игрок вышел
                                        говорим об этом первому игроку
                                */
                                server_msg.flg = FLG_EXIT;
                                if(send(serv->client_1, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        perror("Error on send message to client 1");
                                } else
                                        PrintLOG(serv, "The answer has been sent to client 1");
                        }
                break;
        }
        
        return stat;
}
/*
        Процесс регистрации
*/
void InitRegistration(SERVER_S *serv)
{ 
        int client;
        struct sockaddr_in client_addr;
        socklen_t addr_len;
        char log_data[128];
        
        addr_len = sizeof(struct sockaddr_in);
        
        PrintLOG(serv, "Start registration");
        while(serv->client_1 == -1 || serv->client_2 == -1){
                /*
                        Ожидание подключения
                */
                client = accept(serv->sock_d, (struct sockaddr *) &client_addr, &addr_len);
                if(client == -1){
                        perror("Error to accpt");
                        sleep(3);
                        continue;
                }
                sprintf(log_data, "New connect: %s (%d)", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                PrintLOG(serv, log_data);
                if(pthread_mutex_lock(&serv->mutex) == 0){
                /*
                        Регистрируем
                */                            
                        if(serv->client_1 == -1){                                               
                                serv->client_1 = client;
                                serv->client_1_addr.sin_family = client_addr.sin_family;
                                serv->client_1_addr.sin_addr.s_addr = client_addr.sin_addr.s_addr;
                                serv->client_1_addr.sin_port = client_addr.sin_port;
                        } else {
                                serv->client_2 = client;
                                serv->client_2_addr.sin_family = client_addr.sin_family;
                                serv->client_2_addr.sin_addr.s_addr = client_addr.sin_addr.s_addr;
                                serv->client_2_addr.sin_port = client_addr.sin_port;
                        }
                
                        pthread_mutex_unlock(&serv->mutex);
                        sprintf(log_data, "Registered: %s (%d)", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                        PrintLOG(serv, log_data);
                }
        }
}
/*
        Прием и обработка консольных команд
*/
int InitCommandLine(SERVER_S *serv)
{
        char command[32];
        int readed;
        int i;
        
        while(1){
                write(1,"> ", 3);
                readed = read(0, command, 32);
                for(i = 0; i < readed && command[i] >= 'a' && command[i] <= 'z'; i++);
                if(i > 1){
                        command[i] = '\0';
                } else {
                        continue;
                }
                if(strcmp(command, CM_INFO) == 0){
                        PrintInformation(serv);
                        continue;
                }
                if(strcmp(command, CM_CLOSE) == 0){
                        return 0;
                }
                if(strcmp(command, CM_HELP) == 0){
                        PrintHelp();
                        continue;
                }
                if(strcmp(command, CM_LOG) == 0){
                        write(1, "Press Enter to return\n", strlen("Press Enter to return\n"));
                        if(pthread_mutex_lock(&serv->mutex) == 0){
                                serv->fl_log = 1;
                                pthread_mutex_unlock(&serv->mutex);                        
                        }
                        read(0, command, 32);
                        if(pthread_mutex_lock(&serv->mutex) == 0){
                                serv->fl_log = 0;
                                pthread_mutex_unlock(&serv->mutex);                        
                        }
                        continue;
                }
                printf("Warning: '%s' is not a command. Use 'help'.\n", command);
        }
}
/*
        Запрашиваем ввод IP у пользователя
        Возвращает корректный IP, но не проверяет можно ли к нему привязаться
*/
void GetIP(SERVER_S *serv)
{
        int rd;
        char buff[32];
        int i;
        
        /*
                Если лень вводить
        */       
        inet_aton("192.168.2.34", &serv->addr.sin_addr);
        return;
        
        
        while(1){
                write(1, "Input a server IP: ", strlen("Input a server IP: ") + 1);
                rd = read(0, buff, 32);
                /*
                        Проверяем формат введенных данных
                */
                for(i = 0; i < rd && ( (buff[i] >= '0' && buff[i] <= '9') || buff[i] == '.'); i++);
                if(i < 7)
                        continue;               
                /*
                        Проверяем корректный ли IP
                */
                buff[i] = '\0';
                if(inet_aton(buff, &serv->addr.sin_addr) == 0){
                        printf("Warning: %s is not IP address\n", buff);
                        continue;
                }
                /*
                        Все хорошо, выходим из цикла и функции
                */
                break;
        }
}
/*
        Печать информации по команде CM_INFO
*/
void PrintInformation(SERVER_S *serv)
{
        if(pthread_mutex_lock(&serv->mutex) == 0){
                /*
                        Информация о сервере
                */
                printf("SeaFight server: ");
                if(serv->sock_d == -1){
                        printf("closed\n");
                } else {
                        printf("\n\tIP: %s\n\tPort: %d\n", inet_ntoa(serv->addr.sin_addr), ntohs(serv->addr.sin_port));
                }
                /*
                        Информация о клиентах
                */
                printf("Player 1: ");
                if(serv->client_1 == -1){
                        printf("none\n");
                } else {
                        printf("\n\tIP: %s Port: %d\n", inet_ntoa(serv->client_1_addr.sin_addr), ntohs(serv->client_1_addr.sin_port));
                        printf("\tShips: %d\n", serv->client_1_ships);
                        PrintField(&serv->client_1_field);
                }
                printf("Player 2: ");
                if(serv->client_2 == -1){
                        printf("none\n");
                } else {
                        printf("\n\tIP: %s Port: %d\n", inet_ntoa(serv->client_2_addr.sin_addr), ntohs(serv->client_2_addr.sin_port));
                        printf("\tShips: %d\n", serv->client_1_ships);
                        PrintField(&serv->client_2_field);
                }
                
                pthread_mutex_unlock(&serv->mutex);
        }
}
/*
        Печать по команде CM_HELP
*/
void PrintHelp()
{
        printf("Use next commands:\n");
        printf("\t"CM_CLOSE" - Stop and close server.\n");       
        printf("\t"CM_INFO" - Show information about this server.\n");
        printf("\t"CM_LOG" - Print LOG into the console. Press 'Enter' to return the console.\n");
        printf("\t"CM_HELP" - List of commands that you can use.\n");
}
/*
        Печать лога
*/
void PrintLOG(SERVER_S *serv, char *buff)
{
        time_t ntime;
        char *time_buff;
        
        time(&ntime);
        time_buff = ctime(&ntime);        
        if(pthread_mutex_lock(&serv->mutex) == 0){
                /*
                        Печать лога в файл
                */
                if(serv->log_d > 0){
                        lseek(serv->log_d, 0, SEEK_END);
                        write(serv->log_d, time_buff, strlen(time_buff));
                        write(serv->log_d, buff, strlen(buff));
                        write(serv->log_d, "\n", 1);
                }
                /*
                        Печать лога в консоль
                */
                if(serv->fl_log > 0){
                        write(1, time_buff, strlen(time_buff));
                        write(1, buff, strlen(buff));
                        write(1, "\n", 1);
                }                
                pthread_mutex_unlock(&serv->mutex);
        }
}
/*
        Печать содержимого struct play_field
*/
void PrintField(struct play_field *field)
{
        int i, j;
        
        printf("Private:\n");
        for(i = 0; i < FIELD_COLS; i++){
                printf("\t");
                for(j = 0; j < FIELD_LINES; j++)
                        printf("%d ", field->prv[i][j]);
                printf("\n");
        }
        printf("Public:\n");
        for(i = 0; i < FIELD_COLS; i++){
                printf("\t");
                for(j = 0; j < FIELD_LINES; j++)
                        printf("%d ", field->pub[i][j]);
                printf("\n");
        }
}
