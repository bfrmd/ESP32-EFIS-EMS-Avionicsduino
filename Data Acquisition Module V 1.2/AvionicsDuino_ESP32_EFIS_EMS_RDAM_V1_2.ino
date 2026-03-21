
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                                 Avionicsduino ESP32 EFIS-EMS RDAM V 1.2
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Avionicsduino ESP32 EFIS-EMS RDAM V 1.2 is free software    
  MIT License (MIT)
  
  Copyright (c) 2026 AvionicsDuino - benjamin.fremond@avionicsduino.com
  
 https://avionicsduino.com/index.php/en/esp32-efis-ems/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
    
 *****************************************************************************************************************************/

// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                  External files inclusions
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "RDAMglobals.h"
#include "RDAMcanBus.h"
#include "RDAMsensors.h"
#include "RDAMtimers.h"
#include "RDAMespnow.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                                           SETUP
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(500);
  sensorsInit();
  canInit();
  timersInit();
  espNowInit();
  pinMode(GPIO_NUM_1, OUTPUT);  // For logic analyzer debugging.
  debug = false;
  debugEspNow = false;
  debugCAN = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                                                             LOOP
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop()
{
  digitalWrite(GPIO_NUM_1, !digitalRead(GPIO_NUM_1));
  sensorsRead();  // Execution occurs at each loop iteration. All data is filtered upon reading.
  
  if (timeout50ms)
    {
      if (esp_timer_is_active(oneshot_timer_50ms))  //check state of timer and stop it if needed..
        {
          ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer_50ms));
        }
      ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer_50ms, 50000));  //restart our one shot timer for 50 milliseconds
      timeout50ms = false;

      rc = xQueuePeek(CANmailbox, &canDataSnapshot, 0);  // Reading the CAN bus mailbox and modifying the EFIS-EMS variables accordingly
      if (rc != pdPASS) {}                               // Serial.print("NoCANdata\n");
      airDataComputation();
      espNowSendRecv();
    }

  if (timeout500ms)
    {
      if (esp_timer_is_active(oneshot_timer_500ms))  //check state of timer and stop it if needed..
        {
          ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer_500ms));
        }
      ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer_500ms, 500000));  //restart our one shot timer for 200 milliseconds
      timeout500ms = false;
      if (debug)
        {
          displayDebuggingData();
        }
    }
}
