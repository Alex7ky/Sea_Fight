#ifndef _SERVER_H_
#define _SERVER_H_

#include "net.h"
#include "gen_ships.h"
#include "check_data.h"

#include <stdio.h>
#include <string.h>
/*
        close()
*/
#include <unistd.h>
/*
        socket()
*/
#include <sys/types.h>
#include <sys/socket.h>
/*
        inet_aton()
*/
#include <arpa/inet.h>
#include <netinet/in.h>
/*
        pthreads
*/
#include <pthread.h>
/*
        time_t
        time()
        ctime()
*/
#include <time.h>
/*
        open()
*/
#include <fcntl.h>
/*
        Поиск порта
                FPSTART - нижняя граница портов
                FPLEN - верхняя граница
*/
#define FPSTART 1120
#define FPLEN 10000
/*
        SERVER_S - информация о сервере
                sock_d - дескриптор сокета
                client_1 - дескриптор потока для первого клиента
                client_2 - дескриптор потока для второго клиента
                fl_log - выводить лог в консоль
                log_d - дескриптор файла
                client_1_field - поле первого клиента
                client_2_field - поле второго клиента
                addr - структура адреса для сервера
                client_1_addr - адрес первого клиента
                client_2_addr - адрес второго клиента
                network - сервис для управления пакетами (взаимодействие клиент-сервер-клиент)
                mutex - мютекc для блокирования этой структуры перед чтением/записью
*/
#define SERVER_S struct mServerInformation
SERVER_S {
        int sock_d;
        int client_1;
        int client_2;
        char fl_log;
        int log_d;
        int client_1_ships;
        int client_2_ships;
        struct play_field client_1_field;
        struct play_field client_2_field;
        struct sockaddr_in addr;
        struct sockaddr_in client_1_addr;
        struct sockaddr_in client_2_addr;
        pthread_t network;
        pthread_mutex_t mutex;
};
/*
        Команды командной строки
                CM_CLOSE - остановить и закрыть сервер
                CM_HELP - помощь по командм
                CM_INFO - информация о сервере
                CM_LOG - выводить лог
*/
#define CM_CLOSE "close"
#define CM_HELP "help"
#define CM_INFO "info"
#define CM_LOG "log"
/*
        Функции сервера
*/
SERVER_S *InitServer();
int RemoveServer(SERVER_S *serv);
int InitServices(SERVER_S *serv);
void *NetworkService(void *);
void CreateAnswer(SERVER_S *serv, CLT_DATA msg, int from);
void InitRegistration(SERVER_S *serv);
int InitCommandLine(SERVER_S *serv);
void GetIP(SERVER_S *serv);
void PrintInformation(SERVER_S *serv);
void PrintHelp();
void PrintLOG(SERVER_S *serv, char *buff);

#endif
