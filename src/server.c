#include "../header/server.h"

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
        serv->client_1 = -1;
        serv->client_2 = -1; 
        
        sprintf(log_data,"Server is started with IP %s and with port %d", inet_ntoa(serv->addr.sin_addr), port);
        PrintLOG(serv, log_data);
        
        return serv;
        
        err_exit_1:
                close(serv->sock_d);
                free(serv);
                return NULL;
}
int RemoveServer(SERVER_S *serv)
{
        if(serv == NULL)
                return -1;      
        
        pthread_cancel(serv->network);
        close(serv->client_1);
        close(serv->client_2);
        close(serv->sock_d);
        PrintLOG(serv, "Server closed.");
        close(serv->log_d);
        free(serv);
        
        return 0;
}
int InitServices(SERVER_S *serv)
{
        if( pthread_create(&serv->network, NULL, NetworkService, serv) < 0 ){
                perror("Error on create service for network");

                return -1;
        }
        
        return 0;
}
void *NetworkService(void *args)
{
        SERVER_S *serv = (SERVER_S *)args;
        CLT_DATA client_msg;
        SRV_DATA server_msg;  
        int client;
        struct sockaddr_in client_addr;
        socklen_t addr_len;
        
        addr_len = sizeof(struct sockaddr_in);
        
        /*
                Процесс регистрации
        */
        while(serv->client_1 == -1 || serv->client_2 == -1){
                
                client = accept(serv->sock_d, (struct sockaddr *) &client_addr, &addr_len);
                if(client == -1){
                        perror("Error to accpt");
                        sleep(10);
                }
                
                if(pthread_mutex_lock(&serv->mutex) == 0){
                /*
                        Регистрируем
                */
                /*
                        Генерируем поле для клиента и отправляем
                        
                        генерация_карты(&server_msg.field);
                        server_msg.flg = //Ты зарегался, возми карту, или без флага
                        
                */      
                        
                        if(send(client, &server_msg, SIZE_SRV_DATA, 0) < 0){
                                        /*
                                                Не получилось отправить карту,
                                                ошибка регистрации,
                                                снова ждем клиента
                                        */
                                        perror("Error on send message to client");
                                        pthread_mutex_unlock(&serv->mutex);
                                        continue;
                        }
                        
                        if(serv->client_1 == -1){
                                
                                serv->client_1 = client;
                                serv->client_1_addr.sin_family = client_addr.sin_family;
                                serv->client_1_addr.sin_addr.s_addr = client_addr.sin_addr.s_addr;
                                serv->client_1_addr.sin_port = client_addr.sin_port;
                                memcpy(&serv->client_1_field, &server_msg.field, sizeof(struct play_field));
                        } else {
                                
                                serv->client_2 = client;
                                serv->client_2_addr.sin_family = client_addr.sin_family;
                                serv->client_2_addr.sin_addr.s_addr = client_addr.sin_addr.s_addr;
                                serv->client_2_addr.sin_port = client_addr.sin_port;
                                memcpy(&serv->client_2_field, &server_msg.field, sizeof(struct play_field));
                        }
                
                        pthread_mutex_unlock(&serv->mutex);
                }
        }
        /*
                Процесс игры
        */
        while(1){
                PrintLOG(serv, "Work...");
                sleep(5);
                
        }
}
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
void GetIP(SERVER_S *serv)
{
        int rd;
        char buff[32];
        int i;
        
        inet_aton("192.168.2.34", &serv->addr.sin_addr);
        return;
        
        while(1){
                write(1, "Input a server IP: ", strlen("Input a server IP: ") + 1);
                rd = read(0, buff, 32);
                for(i = 0; i < rd && ( (buff[i] >= '0' && buff[i] <= '9') || buff[i] == '.'); i++);
                if(i < 7)
                        continue;
                buff[i] = '\0';
                if(inet_aton(buff, &serv->addr.sin_addr) == 0){
                        printf("Warning: %s is not IP address\n", buff);
                        continue;
                }
                break;
        }
}
void PrintInformation(SERVER_S *serv)
{
        if(pthread_mutex_lock(&serv->mutex) == 0){
                printf("SeaFight server:\n");
                printf("\tIP: %s Port: %d\n", inet_ntoa(serv->addr.sin_addr), ntohs(serv->addr.sin_port));
                printf("Player 1:\n");
                if(serv->client_1 == -1){
                        printf("none\n");
                } else {
                        printf("\tIP: %s Port: %d\n", inet_ntoa(serv->client_1_addr.sin_addr), ntohs(serv->client_1_addr.sin_port));
                }
                printf("Player 2:\n");
                if(serv->client_2 == -1){
                        printf("none\n");
                } else {
                        printf("\tIP: %s Port: %d\n", inet_ntoa(serv->client_2_addr.sin_addr), ntohs(serv->client_2_addr.sin_port));
                }
                
                pthread_mutex_unlock(&serv->mutex);
        }
}
void PrintHelp()
{
        printf("Use next commands:\n");
        printf("\t"CM_CLOSE" - Stop and close server.\n");       
        printf("\t"CM_INFO" - Show information about this server.\n");
        printf("\t"CM_LOG" - Print LOG into the console. Press 'Enter' to return the console.\n");
        printf("\t"CM_HELP" - List of commands that you can use.\n");
}

void PrintLOG(SERVER_S *serv, char *buff)
{
        time_t ntime;
        char *time_buff;
        
        time(&ntime);
        time_buff = ctime(&ntime);        
        if(pthread_mutex_lock(&serv->mutex) == 0){
                if(serv->log_d > 0){
                        lseek(serv->log_d, 0, SEEK_END);
                        write(serv->log_d, time_buff, strlen(time_buff));
                        write(serv->log_d, buff, strlen(buff));
                        write(serv->log_d, "\n", 1);
                }
                if(serv->fl_log > 0){
                        write(1, time_buff, strlen(time_buff));
                        write(1, buff, strlen(buff));
                        write(1, "\n", 1);
                }
                pthread_mutex_unlock(&serv->mutex);
        }
}
