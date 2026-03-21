/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                                 Avionicsduino ESP32 EFIS-EMS Display Module V 1.2
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Avionicsduino ESP32 EFIS-EMS Display Module V 1.2 is free software    
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


#include "globals.h"
#include "functions.h"
#include "efis.h"
#include "handlePushedButton.h"



LGFX tft;

CircularGauge rpmGauge(&tft, 0, 0, 70, 110, 220, 15, "RPM", true, 0);  // arguments: pointer to tft, display coordinates of the top left corner of the gauge, radius of the gauge, absolute starting angle of the first arc,
                                                                       // total angular extent of the arc in degrees, arc thickness, gauge title,
                                                                       // convert (or not) to an integer + round to the nearest ten, and number of positions after the decimal point.
CircularGauge fuelLevelGauge(&tft, 0, 185, 43, 90, 180, 7, "L", false, 1);
CircularGauge fuelFlowGauge(&tft, 86, 185, 43, 120, 180, 7, "L/H", false, 1);
LinearGauge oilPressureGauge(&tft, 175, 92, 140, 12, 'V', "OilP", false, 1);  // arguments: pointer to tft, display coordinates of the center of the bar, length and thickness in pixels,
                                                                              // orientation, title, convert (or not) to an integer + round to the nearest ten, and number of positions after the decimal point.
LinearGauge oilTempGauge(&tft, 232, 92, 140, 12, 'V', "OilT", false, 0);
LinearGauge vBatGauge(&tft, 70, 173, 140, 16, 'H', "Bat", false, 1);
CircularGauge mapGauge(&tft, 172, 185, 43, 120, 220, 7, "MAP", false, 1);
CircularGauge afrGauge(&tft, 172, 275, 43, 120, 220, 6, "AFR", false, 1);

LinearGauge cht2Gauge(&tft, 70, 310, 140, 12, 'H', "cht2", false, 0);
LinearGauge cht3Gauge(&tft, 70, 362, 140, 12, 'H', "cht3", false, 0);
LinearGauge egt3Gauge(&tft, 70, 414, 140, 12, 'H', "exh3", false, 0);
LinearGauge egt4Gauge(&tft, 70, 466, 140, 12, 'H', "exh4", false, 0);


void setup()
{
  Serial.begin(115200);
  delay(500);
  flashMemVariablesInit();  // Initialization of some variables saved in flash memory
  dysplayAndSpritesInit();  // Initialization of the horizon and creation of sprites
  initMenu();               // Menu system initialization
  initInterrupts();         // Push buttons Interrupts initialization

  // *************************** ESP-NOW init ***************************
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

  // ************************************* Other init **************************************

  unitsStringsInit();  //Initialization of the speed, pressure, and temperature units strings
  tft.setCursor(0, 0);

  rpmGauge.begin(rpmLimits);
  fuelLevelGauge.begin(fuelLevelLimits);
  fuelFlowGauge.begin(fuelFlowLimits);
  oilPressureGauge.begin(oilPressureLimits);
  oilTempGauge.begin(oilTempLimits);
  vBatGauge.begin(vBatLimits);
  mapGauge.begin(mapLimits);
  cht2Gauge.begin(cht2Limits);
  cht3Gauge.begin(cht3Limits);
  egt3Gauge.begin(egt3Limits);
  egt4Gauge.begin(egt4Limits);
  afrGauge.begin(afrLimits);

  tft.clear();
  drawFixedItems();

  startTime = xTaskGetTickCount() - TIMEOUT;  // Start counting elapsed seconds from power on to manage timers
}

void loop()
{
  topLoop = millis();  // measurement of FPS

  // ************************************************************** Refresh EMS data ********************************************************************
  rpmGauge.drawGauge((float)fromRDAMdata.rpmInt);
  fuelLevelGauge.drawGauge(fromRDAMdata.calcFuelLevel);
  fuelFlowGauge.drawGauge(fromRDAMdata.fuelFlow);
  oilPressureGauge.drawGauge(fromRDAMdata.oilP/1000);
  oilTempGauge.drawGauge(fromRDAMdata.oilT);
  vBatGauge.drawGauge(fromRDAMdata.vBat);
  mapGauge.drawGauge(fromRDAMdata.manifoldPressure);

  cht2Gauge.drawGauge(fromRDAMdata.cht2);
  cht3Gauge.drawGauge(fromRDAMdata.cht3);
  egt3Gauge.drawGauge(fromRDAMdata.egt3);
  egt4Gauge.drawGauge(fromRDAMdata.egt4);
  afrGauge.drawGauge(fromRDAMdata.afr);
  tft.setTextDatum(top_left);
  displayEngineTime();
  displayFuel();
  displayCurrent();
/*
  Serial.printf("MAP : %.2f%c", fromRDAMdata.manifoldPressure, '\t');
  Serial.printf("OilP : %.1f%c", fromRDAMdata.oilP / 1000, '\t');
  Serial.printf("OilT : %.1f%c", fromRDAMdata.oilT, '\t');
  Serial.printf("CHT2 : %.1f%c", fromRDAMdata.cht2, '\t');
  Serial.printf("CHT3 : %.1f%c", fromRDAMdata.cht3, '\t');
  Serial.printf("Ibus : %.1f%c", fromRDAMdata.iBus, '\t');
  Serial.printf("iBat : %.1f%c", fromRDAMdata.iBat, '\t');
  Serial.printf("AFR : %.1f%c", fromRDAMdata.afr, '\t');
  Serial.printf("EGT3 : %.0f%c", fromRDAMdata.egt3, '\t');
  Serial.printf("EGT4 : %.0f%c", fromRDAMdata.egt4, '\n');
  */

  // ******************************************************** Counting elapsed seconds from power on *****************************************************************
  countSeconds();

  // ************************************************************************************ Calculating the ball's position ****************************************************************************************
  ballOffset = (fromRDAMdata.AccY * -1 * BALL_RADIUS) / 0.27;
  if (ballOffset > BALL_RADIUS * 4) ballOffset = BALL_RADIUS * 4;
  if (ballOffset < -BALL_RADIUS * 4) ballOffset = -BALL_RADIUS * 4;

  //  ***************************************************************************** Handle any pushed button *******************************************************************************************
  handlePushedButton();

  // ************************************************************** Refresh EFIS screen ********************************************************************
  if (true)  // for future use in multi-page mode
    {
      spriteGradPitch.setPivot(GRADPITCH_WIDTH / 2, (GRADPITCH_HEIGHT / 2) + (fromRDAMdata.pitch * -PIX_PER_DEG));
      spriteIndexRoll.setPivot(11, 127);
      spriteArcRoll.setPivot(125, 145);
      for (uint8_t n = 0; n < NB_SLICES_SG; n++)
        {
          HorizArtdraw(fromRDAMdata.roll * DEG_TO_RAD, fromRDAMdata.pitch, n);  // roll in radians, pitch in degrees
          spriteGradPitch.pushRotateZoom(&spriteSkyGround, DL, CY - SHIFT_VAL_SG * n, -1 * fromRDAMdata.roll, 1, 1, TFT_BLUE);
          spriteIndexRoll.pushRotateZoom(&spriteSkyGround, DL, CY - SHIFT_VAL_SG * n, 0, 1, 1, TFT_BLUE);
          spriteArcRoll.pushRotateZoom(&spriteSkyGround, DL, CY - SHIFT_VAL_SG * n, -1 * fromRDAMdata.roll, 1, 1, TFT_BLUE);
          spriteMaquette.pushSprite(CX - MAQUETTE_WIDTH / 2, CY - 24 - SHIFT_VAL_SG * n, TFT_BLUE);
          if (n == 0) magHeadDraw(fromRDAMdata.magHeading);
          altiDraw(fromRDAMdata.qnhAltitude, n);
          speedDraw(fromRDAMdata.indicatedAirSpeed, n);
          if (n == NB_SLICES_SG - 1) billeDraw(ballOffset, n);
          textDraw(n);
          timerDraw(n);
          spriteSkyGround.pushSprite(HORIZON_X, HORIZON_Y + SHIFT_VAL_SG * n);
        }

      varioDraw(fromRDAMdata.baroVario);
      spriteVario.pushSprite(VARIO_X + 3, VARIO_Y, TFT_BLUE);
    }

  // ************************************************** Sending ESP-NOW messages *******************************************************
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&toModuleData, sizeof(toModuleData));
  if (result != ESP_OK)
    {
      //Serial.println("Error sending the data");
    }

  // ************************************************************** Display refresh rate of the screen ********************************************************************
  fps();  // refresh rate measurement and display
}
