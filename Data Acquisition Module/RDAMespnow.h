#ifndef RDAMESPNOW_H
#define RDAMESPNOW_H

#include "RDAMglobals.h"

void espNowInit();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void espNowSendRecv();

#endif