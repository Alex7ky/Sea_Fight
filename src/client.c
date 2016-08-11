#include "../header/client.h"
#include "../header/net.h"

void obmen (int argc, char **argv)
{
    int sock;				    // дискрипторы сокетов
    struct sockaddr_in addr;		    // структура с адресом
    int bytes_read = 0;           	    // кол-во принятых байт
    struct SRV_DATA;			    // структура принятая
    struct CLT_DATA;                        // структура отправленная

    sock = socket(AF_INET, SOCK_STREAM, 0);	// создание TCP-сокета
    if(sock < 0)
    {
        perror("socket");
        exit(-1);
    }
    //задаем параметры сервера
    addr.sin_family = AF_INET;          // домены Internet
    addr.sin_port = htons(3428);        // или любой другой порт...
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)   // установка соединения с сервером
    {
        perror("Подключение");
        exit(-1);
    }
     while (1){
        // Отправляем
        if (!strcmp(CLT_DATA, "exit"))
            break;
        send(sock, CLT_DATA, strlen(CLT_DATA) + 1, 0);
        //Принимаем
        if (recv(sock, SRV_DATA, 256, 0) > 0)
    }
     // Завершение
    close(sock);
    exit(0);
}
