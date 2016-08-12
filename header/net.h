#ifndef __NET_H__
#define __NET_H__

#include "common.h"

#define FLG_WINNER  1   // Данный игрок победил
#define FLG_LOSE    2   // Данный игрок проиграл
#define FLG_EXIT   -1   // Если клиент вышел из игры (Не завершив игру)
#define FLG_GEN_SHIPS 3 // Запрос на генерацию кораблей
#define FLG_WAIT 4      // Ждать хода
#define FLG_STEP 5      // Ход

#define CLT_DATA struct client_data
#define SIZE_CLT_DATA sizeof(CLT_DATA)

#define SRV_DATA struct server_data
#define SIZE_SRV_DATA sizeof(SRV_DATA)

/**
 * Структура, описывающая передачу данных от клиента к серверу
 */
CLT_DATA {
  int posx;   // Положение карабля по X
  int posy;   // Положение карабля по Y
  int flg;    // Тип сообщения
};

/**
 * Структура, описывающая передачу данных от сервера к клиенту
 */
SRV_DATA {
  struct play_field field;  // Игровое поле  
  int flg;                  // Тип сообщения
};

#endif
