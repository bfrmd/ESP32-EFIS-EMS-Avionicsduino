#include "RDAMcanBus.h"

void canInit()  // CAN controller initialization
{
  twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_PIN, RX_GPIO_PIN, TWAI_MODE_NORMAL);
  general_config.tx_queue_len = 3000;
  general_config.rx_queue_len = 3000;  //Number of messages RX queue can hold
  twai_timing_config_t timing_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  if (twai_driver_install(&general_config, &timing_config, &filter_config) == ESP_OK)
    {
      Serial.println("CAN driver installed");
      if (twai_start() == ESP_OK)
        {
          Serial.println("CAN driver started");
          uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
          if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
            {
              Serial.println("CAN alerts reconfigured");
              canControllerReady = true;
              Serial.println("CAN controller ready");
            }
          else Serial.println("Failed to reconfigure alerts");
        }
      else Serial.println("Failed to start driver");
    }
  else Serial.println("Failed to install driver");

  msgQNHaltitudesVario.identifier = 46;
  msgQNHaltitudesVario.data_length_code = 8;
  msgSpeeds_Wind.identifier = 48;
  msgSpeeds_Wind.data_length_code = 8;
  msgPA.identifier = 50;
  msgPA.data_length_code = 4;
  msgCHT.identifier = 52;
  msgCHT.data_length_code = 8;
  msgHuile.identifier = 54;
  msgHuile.data_length_code = 8;
  msgEGT.identifier = 56;
  msgEGT.data_length_code = 8;
  msgAFR_Vbus.identifier = 58;
  msgAFR_Vbus.data_length_code = 8;
  msgCurrents.identifier = 60;
  msgCurrents.data_length_code = 8;

  xTaskCreatePinnedToCore(TaskCANcode, "TaskCAN", 3000, NULL, 1, &TaskCAN, 1);  // CAN bus task creation. Pinned to core 1 to prevent watchdog bites when this task is free to run on core 0
  CANmailbox = xQueueCreate(1, sizeof(CanDataStructure));                       // CAN bus task maibox creation
  assert(CANmailbox);

  airDataMutex = xSemaphoreCreateMutex();
  if (airDataMutex == NULL)
    {
      Serial.println("Failed to create airDataMutex!");
      while (1)
        ;
    }
}

void TaskCANcode(void* parameter)  // CAN bus Task function
{
  for (;;)
    {
      if (canControllerReady)
        {
          uint32_t alerts_triggered;
          twai_status_info_t status_info;

          // *********************************************************** Checking if alert triggered *********************************************
          twai_read_alerts(&alerts_triggered, 0);
          twai_get_status_info(&status_info);

          // Handle the alerts
          if (alerts_triggered & TWAI_ALERT_ERR_PASS & debugCAN)
            Serial.println("Alert: TWAI controller has become error passive.");
          else if (alerts_triggered & TWAI_ALERT_BUS_ERROR & debugCAN)
            {
              Serial.print("Alert: a Bit, Stuff, CRC, Form, or ACK error has occurred on the bus. ");
              Serial.printf("Bus error count: %d\n", status_info.bus_error_count);
            }
          else if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL & debugCAN)
            {
              Serial.println("Alert: the RX queue is full causing received frames to be lost.");
              Serial.printf("RX buffered: %d\t", status_info.msgs_to_rx);
              Serial.printf("RX miussed: %d\n", status_info.rx_missed_count);
              Serial.printf("RX overrun %d\n", status_info.rx_overrun_count);
            }
          else if (alerts_triggered & TWAI_ALERT_RX_DATA)  // One or more message received ? --> handle all
            {
              // **************************************************************** Querying the CAN Bus receive queue *************************************************************************************
              twai_message_t message;
              while (twai_receive(&message, 0) == ESP_OK)
                {
                  switch (message.identifier)  // canDataRealTime is updated in real time as data arrives on the CAN bus
                    {
                      case 42:
                        canDataRealTime.oat = *(float*)(message.data + 0);
                        break;

                      case 44:
                        canDataRealTime.rh = *(float*)(message.data + 0);
                        break;

                      case 28:
                        canDataRealTime.magHeading = *(float*)(message.data + 0);               // The remote magnetometer transmits to the CAN Bus a filtered magHeading float value in degrees from -180° to +180°.
                        if (canDataRealTime.magHeading < 0) canDataRealTime.magHeading += 360;  // Conversion from -180 -> +180 to 0 -> +360°
                        break;

                      case 20:
                        canDataRealTime.roll = (*(float*)(message.data + 0)) * (180 / PI);  // data received in radians from the AHRS, unfiltered, and converted to degrees.
                        canDataRealTime.pitch = *(float*)(message.data + 4);                // data received in degrees from the AHRS, unfiltered.
                        break;

                      case 22:
                        canDataRealTime.AccY = *(float*)(message.data + 0);  // The AHRS transmits unfiltered accY and AccZ values, expressed in m/s², to the CAN Bus.
                        canDataRealTime.AccY = iirFilter(previousFilteredAccYValue, canDataRealTime.AccY, 0.04);
                        previousFilteredAccYValue = canDataRealTime.AccY;

                        canDataRealTime.AccZ = *(float*)(message.data + 4);
                        break;

                      case 24:
                        canDataRealTime.Vz = (*(float*)(message.data + 0)) * 196.8504;  // data received from the AHRS in m/s, unfiltered, it is converted to ft/min.
                        canDataRealTime.trk = *(float*)(message.data + 4);              // The AHRS transmits to the CAN Bus an unfiltered trk float value, in degrees, from -180° to +180°.
                        if (canDataRealTime.trk < 0) canDataRealTime.trk += 360;        // Conversion from -180/+180 to 0/+360°
                        break;

                      case 34:
                        canDataRealTime.hourGNSS = *(uint8_t*)(message.data + 4);
                        canDataRealTime.minuteGNSS = *(uint8_t*)(message.data + 5);
                        canDataRealTime.secondGNSS = *(uint8_t*)(message.data + 6);
                        canDataRealTime.satInView = *(uint8_t*)(message.data + 7);
                        break;

                      case 30:
                        canDataRealTime.altitudeGNSS = *(float*)(message.data + 0);
                        canDataRealTime.groundSpeedGNSS = *(float*)(message.data + 4);
                        break;

                      case 26:
                        canDataRealTime.lowerLimit = *(uint8_t*)(message.data + 0);
                        canDataRealTime.upperLimit = *(uint8_t*)(message.data + 1);
                        canDataRealTime.rpmInt = *(uint16_t*)(message.data + 2);
                        canDataRealTime.vBat = *(float*)(message.data + 4);
                        break;

                      case 32:
                        canDataRealTime.fuelFlow = *(float*)(message.data + 0);
                        canDataRealTime.calcFuelLevel = *(float*)(message.data + 4);
                        break;

                      case 33:
                        canDataRealTime.totalNbSec = *(uint16_t*)(message.data + 0);
                        canDataRealTime.fuelUsed = *(float*)(message.data + 2);
                        break;

                      default: break;
                    }
                }
              // *********************************************** Sending new data that has just been received to the CANmailbox *******************************************************
              rc = xQueueOverwrite(CANmailbox, &canDataRealTime);
              assert(rc == pdPASS);
            }
          // ****************************************** If it's time, send data to the CAN Bus for the FDR *****************************************************************

          // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
          // !!!!!!! This routine should not be a problem, because the task reads global variables which can indeed be modified by the other task, but the variables in question are atomic.  !!!!!!!!
          // !!!!!!! However, since the variable alignment and padding in the structures is unsure, this routine is protected with a mutex. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
          // !!!!!!! The same applies to the routine that modifies these variables in the RDAMsensors.cpp file, namely the airDataComputation() function. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
          // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


          if (timeout200ms && xSemaphoreTake(airDataMutex, MUTEX_TIMEOUT))  // Sending data to the CAN Bus at 5 Hz if airDataMutex OK
            {
              if (esp_timer_is_active(oneshot_timer_200ms))  //check state of timer and stop it if needed..
                {
                  ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer_200ms));
                }
              ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer_200ms, 200000));  //restart our one shot timer for 200 milliseconds
              timeout200ms = false;
              int16_t qnhAltitudeInt = int16_t(moduleData.qnhAltitude + 0.5);
              int16_t baroVarioInt = int16_t(moduleData.baroVario + 0.5);

              for (uint8_t i = 0; i < 2; i++)
                {
                  msgQNHaltitudesVario.data[i] = ((byte*)&fromDisplayData.QNH)[i];
                  msgQNHaltitudesVario.data[i + 2] = ((byte*)&qnhAltitudeInt)[i];
                  msgQNHaltitudesVario.data[i + 4] = ((byte*)&moduleData.densityAltitudeInt)[i];
                  msgQNHaltitudesVario.data[i + 6] = ((byte*)&baroVarioInt)[i];
                  msgSpeeds_Wind.data[i] = ((byte*)&nonFilteredIndicatedAirSpeed)[i];
                  msgSpeeds_Wind.data[i + 2] = ((byte*)&nonFilteredTrueAirSpeed)[i];
                  msgSpeeds_Wind.data[i + 4] = ((byte*)&moduleData.windSpeedInt)[i];
                  msgSpeeds_Wind.data[i + 6] = ((byte*)&moduleData.windDirectionInt)[i];
                }
              for (uint8_t i = 0; i < 4; i++)
                {
                  msgPA.data[i] = ((byte*)&moduleData.manifoldPressure)[i];
                  msgCHT.data[i] = ((byte*)&moduleData.cht2)[i];
                  msgCHT.data[i + 4] = ((byte*)&moduleData.cht3)[i];
                  msgHuile.data[i] = ((byte*)&moduleData.oilT)[i];
                  msgHuile.data[i + 4] = ((byte*)&moduleData.oilP)[i];
                  msgEGT.data[i] = ((byte*)&moduleData.egt3)[i];
                  msgEGT.data[i + 4] = ((byte*)&moduleData.egt4)[i];
                  msgAFR_Vbus.data[i] = ((byte*)&moduleData.afr)[i];
                  msgCurrents.data[i] = ((byte*)&moduleData.iBus)[i];
                  msgCurrents.data[i + 4] = ((byte*)&moduleData.iBat)[i];
                }
              //Queue messages for transmission
              if (twai_transmit(&msgQNHaltitudesVario, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgQNHaltitudesVario for transmission\n");
              if (twai_transmit(&msgSpeeds_Wind, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgSpeeds_Wind for transmission\n");
              if (twai_transmit(&msgPA, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgPA for transmission\n");
              if (twai_transmit(&msgCHT, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgCHT for transmission\n");
              if (twai_transmit(&msgHuile, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgHuile for transmission\n");
              if (twai_transmit(&msgEGT, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgEGT for transmission\n");
              if (twai_transmit(&msgAFR_Vbus, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgAFR_Vbus for transmission\n");
              if (twai_transmit(&msgCurrents, portMAX_DELAY) != ESP_OK & debugCAN) Serial.print("Failed to queue msgCurrents for transmission\n");
              xSemaphoreGive(airDataMutex);
            }
        }
      vTaskDelay(1);
    }
}
