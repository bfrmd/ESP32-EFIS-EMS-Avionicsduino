#include "RDAMespnow.h"

void espNowInit()
{
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
    {
      Serial.println("Error initializing ESP-NOW");
      return;
    }

  // Registration of the two callback functions.
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      return;
    }
}

// This callback function is executed when new data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (status != ESP_NOW_SEND_SUCCESS)
    {
      Serial.println("Delivery Fail");
    }
}

// This callback function is executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&fromDisplayData, incomingData, sizeof(fromDisplayData));
  flagAffiche = true;
}



void espNowSendRecv()
{
  if (flagAffiche && debugEspNow)
    {
      Serial.printf("QNH : % 2d%c", fromDisplayData.QNH, '\t');
      Serial.printf("QFE : % 2d%c", fromDisplayData.QFE, '\t');
      Serial.printf("speedConversionFactor : %5.3f%c", fromDisplayData.speedConversionFactor, '\t');
      Serial.printf("pressureCorr : % 3d%c", fromDisplayData.pressureCorr, '\t');
      Serial.printf("magDev : % 3d%c", fromDisplayData.magDev, '\n');
      flagAffiche = false;
    }

  // Send message via ESP-NOW each loop() iteration
  memcpy(&toDisplayData, &canDataSnapshot, sizeof(canDataSnapshot)); // copy the contents of canDataSnapshot to the beginning of toDisplayData
  memcpy(&toDisplayData.manifoldPressure, &moduleData, sizeof(moduleData)); //copy the contents of moduleData into toDisplayData begining from toDisplayData.manifoldPressure

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&toDisplayData, sizeof(toDisplayData));

  if (result != ESP_OK)
    {
      Serial.println("Error sending the data");
    }
}
