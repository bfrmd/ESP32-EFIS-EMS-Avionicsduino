#include "efis.h"


// ******************************************************************** Screen initialization and sprite creation ***********************************************************************
void dysplayAndSpritesInit()
{
  ledcAttachChannel(BACKLIGHT_PWM_PIN, PWM_FREQ, PWM_RESOLUTION, PWM_CHANNEL);
  ledcWrite(BACKLIGHT_PWM_PIN, (uint32_t)(backLight*16));

  tft.init();
  tft.setRotation(2);
  
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(220, 125);
  tft.println("AvionicsDuino");
  tft.setCursor(180, 200);
  tft.println("ESP32 EFIS-EMS");
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(200, 300);
  tft.println("Display Module Software V 1.2");
  tft.setCursor(200, 350);
  tft.println("RDAM Software V 1.2");
  delay(3000);

  tft.initDMA();
  tft.startWrite();
  delay(50);

  spriteSkyGround.setColorDepth(16);
  spriteIndexRoll.setColorDepth(16);
  spriteArcRoll.setColorDepth(16);
  spriteVario.setColorDepth(16);
  spriteAlti1.setColorDepth(16);
  spriteAlti2.setColorDepth(16);
  spriteMagHead.setColorDepth(16);
  spriteSpeed1.setColorDepth(16);
  spriteSpeed2.setColorDepth(16);
  spriteFlapSpeed.setColorDepth(16);
  spriteLimitSpeeds.setColorDepth(16);

  spriteGradPitch.setBuffer((void*)gradPitch, GRADPITCH_WIDTH, GRADPITCH_HEIGHT, 8);
  spriteMaquette.setBuffer((void*)maquette, MAQUETTE_WIDTH, MAQUETTE_HEIGHT, 8);

  tft.setFont(&Font4);

  if (!spriteSkyGround.createSprite(HORIZON_WIDTH, SHIFT_VAL_SG))
    {
      tft.println(F("Create spriteSkyGround failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteIndexRoll.createSprite(INDEXROLL_WIDTH, INDEXROLL_HEIGHT))
    {
      tft.println(F("Create spriteIndexRoll failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteArcRoll.createSprite(ARCROLL_WIDTH, ARCROLL_HEIGHT))
    {
      tft.println(F("Create spriteArcRoll failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteVario.createSprite(SPRITE_VARIO_WIDTH, SPRITE_VARIO_HEIGHT))
    {
      tft.println(F("Create spriteVario failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteAlti1.createSprite(SPRITE_ALTI1_WIDTH, SPRITE_ALTI1_HEIGHT))
    {
      tft.println(F("Create spriteAlti1 failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteAlti2.createSprite(SPRITE_ALTI2_WIDTH, SPRITE_ALTI2_HEIGHT))
    {
      tft.println(F("Create spriteAlti2 failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteMagHead.createSprite(SPRITE_MAGHEAD_WIDTH, SPRITE_MAGHEAD_HEIGHT))
    {
      tft.println(F("Create spriteMagHead failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteSpeed1.createSprite(SPRITE_SPEED1_WIDTH, SPRITE_SPEED1_HEIGHT))
    {
      tft.println(F("Create spriteSpeed1 failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteSpeed2.createSprite(SPRITE_SPEED2_WIDTH, SPRITE_SPEED2_HEIGHT))
    {
      tft.println(F("Create spriteSpeed2 failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteFlapSpeed.createSprite(SPRITESPEED_WIDTH, SPRITESPEEDPIX_EXTENT))
    {
      tft.println(F("Create spriteFlapSpeed failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteLimitSpeeds.createSprite(SPRITESPEED_WIDTH, SPRITESPEEDPIX_EXTENT))
    {
      tft.println(F("Create spriteLimitSpeeds failed. Program stopped working"));
      while (1) {}
    }

  spriteAlti1.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteAlti1.setTextDatum(top_right);
  spriteAlti1.setFont(&FreeSansBold18pt7b);
  spriteAlti2.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteAlti2.setTextDatum(top_right);
  spriteAlti2.setFont(&FreeSansBold18pt7b);
  spriteMagHead.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteMagHead.setFont(&FreeSansBold18pt7b);
  spriteSpeed1.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteSpeed1.setTextDatum(top_right);
  spriteSpeed1.setFont(&FreeSansBold18pt7b);
  spriteSpeed2.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteSpeed2.setTextDatum(top_right);
  spriteSpeed2.setFont(&FreeSansBold18pt7b);


  spriteArcRoll.fillSprite(TFT_BLUE);


  upperLimWhiteArc = whiteArc[1] * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  lengthWhiteArc = (whiteArc[1] - whiteArc[0]) * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  upperLimYellowArc = yellowArc[1] * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  lengthYellowArc = (yellowArc[1] - yellowArc[0]) * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  upperLimGreenArc = greenArc[1] * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  lengthGreenArc = (greenArc[1] - greenArc[0]) * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  upperLimRedArc = redArc[1] * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  lengthRedArc = (redArc[1] - redArc[0]) * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;


#define RAYON_GRADUATIONS 145
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 14, 210, 330, TFT_WHITE);  // Arc of the roll graduations
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS, 269.5, 270.5, TFT_WHITE);   // Graduation 0°
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS, 210, 211, TFT_WHITE);       // Graduation -60°
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS, 329, 330, TFT_WHITE);       // Graduation +60°
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS, 239.5, 240.5, TFT_WHITE);   // Graduation -30°
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS, 299.5, 300.5, TFT_WHITE);   // Graduation +30°

  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 7, 224.5, 225.5, TFT_WHITE);  // Graduation -45
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 7, 249.5, 250.5, TFT_WHITE);  // Graduation -20
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 7, 259.5, 260.5, TFT_WHITE);  // Graduation -10
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 7, 279.5, 280.5, TFT_WHITE);  // Graduation +10
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 7, 289.5, 290.5, TFT_WHITE);  // Graduation +20
  spriteArcRoll.fillArc(125, 145, RAYON_GRADUATIONS - 15, RAYON_GRADUATIONS - 7, 314.5, 315.5, TFT_WHITE);  // Graduation +45

  spriteIndexRoll.fillSprite(TFT_BLUE);
  spriteIndexRoll.fillTriangle(11, 0, 0, 19, 20, 19, TFT_WHITE);
}


void HorizArtdraw(float aRoll, float aPitch, uint8_t i)
{
  if ((aRoll > PI / 2) | (aRoll < -PI / 2))
    {
      color1 = GROUND_COLOR;
      color2 = SKY_COLOR;
    }
  else
    {
      color1 = SKY_COLOR;
      color2 = GROUND_COLOR;
    }


  float tanroll = tan(aRoll);
  float sinroll = sin(aRoll);
  float cosroll = cos(aRoll);
  if (tanroll > 600)
    {
      tanroll = 600;
    }
  if (tanroll < -600)
    {
      tanroll = -600;
    }
  g = DH + (PIX_PER_DEG * aPitch * (cosroll + (sinroll * tanroll))) + (DL * tanroll);
  d = g - (largeur * tanroll);
  if (g < 0)
    {
      casG = 10;
    }
  else if (g > hauteur)
    {
      casG = 30;
    }
  else
    {
      casG = 20;
    }
  if (d < 0)
    {
      casD = 1;
    }
  else if (d > hauteur)
    {
      casD = 3;
    }
  else
    {
      casD = 2;
    }


  switch (casG + casD)
    {
      case 22:
        spriteSkyGround.fillSprite(color1);
        fillQuadrangle(LG, LB - SHIFT_VAL_SG * i, LG, g + LH - SHIFT_VAL_SG * i, LD, d + LH - SHIFT_VAL_SG * i, LD, LB - SHIFT_VAL_SG * i, color2);
        spriteSkyGround.drawLine(LG, g + LH - SHIFT_VAL_SG * i, LD, d + LH - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 13:
        h = (g / tanroll);
        b = (g - hauteur) / tanroll;
        spriteSkyGround.fillScreen(color2);
        fillQuadrangle(h + LG, LH - SHIFT_VAL_SG * i, LD, LH - SHIFT_VAL_SG * i, LD, LB - SHIFT_VAL_SG * i, b + LG, LB - SHIFT_VAL_SG * i, color1);
        spriteSkyGround.drawLine(h + LG, LH - SHIFT_VAL_SG * i, b + LG, LB - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 31:
        h = (g / tanroll);
        b = (g - hauteur) / tanroll;
        spriteSkyGround.fillScreen(color1);
        fillQuadrangle(h + LG, LH - SHIFT_VAL_SG * i, LD, LH - SHIFT_VAL_SG * i, LD, LB - SHIFT_VAL_SG * i, b + LG, LB - SHIFT_VAL_SG * i, color2);
        spriteSkyGround.drawLine(h + LG, LH - SHIFT_VAL_SG * i, b + LG, LB - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 12:
        h = min(g / tanroll, (float)2 * DL);
        spriteSkyGround.fillScreen(color2);
        spriteSkyGround.fillTriangle(h + LG, LH - SHIFT_VAL_SG * i, LD, LH - SHIFT_VAL_SG * i, LD, d + LH - SHIFT_VAL_SG * i, color1);
        spriteSkyGround.drawLine(h + LG, LH - SHIFT_VAL_SG * i, LD, d + LH - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 21:
        h = min(g / tanroll, (float)2 * DL);
        spriteSkyGround.fillScreen(color2);
        spriteSkyGround.fillTriangle(LG, g + LH - SHIFT_VAL_SG * i, LG, LH - SHIFT_VAL_SG * i, h + LG, LH - SHIFT_VAL_SG * i, color1);
        spriteSkyGround.drawLine(LG, g + LH - SHIFT_VAL_SG * i, h + LG, LH - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 23:
        b = min((g - hauteur) / tanroll, (float)2 * DL);
        spriteSkyGround.fillScreen(color1);
        spriteSkyGround.fillTriangle(LG, g + LH - SHIFT_VAL_SG * i, b + LG, LB - SHIFT_VAL_SG * i, LG, LB - SHIFT_VAL_SG * i, color2);
        spriteSkyGround.drawLine(LG, g + LH - SHIFT_VAL_SG * i, b + LG, LB - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 32:
        b = min((g - hauteur) / tanroll, (float)2 * DL);
        spriteSkyGround.fillScreen(color1);
        spriteSkyGround.fillTriangle(b + LG, LB - SHIFT_VAL_SG * i, LD, LB - SHIFT_VAL_SG * i, LD, d + LH - SHIFT_VAL_SG * i, color2);
        spriteSkyGround.drawLine(b + LG, LB - SHIFT_VAL_SG * i, LD, d + LH - SHIFT_VAL_SG * i, TFT_WHITE);
        break;
      case 33:
        spriteSkyGround.fillScreen(color1);
        break;
      case 11:
        spriteSkyGround.fillScreen(color2);
        break;
      default:
        break;
    }
}


void speedDraw(float _speed, uint8_t i)  // The pressure sensors module transmits IAS in the unit selected by the user
{
  static const uint32_t x1 = 42;
  static const uint32_t y1 = HORIZON_HEIGHT / 2 - 15;
  static const uint32_t x2 = x1 + WIDTH_CHAR * 2;
  static const uint32_t y2 = y1 - (SPRITE_SPEED2_HEIGHT - SPRITE_SPEED1_HEIGHT) / 2;


  spriteSpeed1.fillScreen(TFT_BLACK);
  spriteSpeed2.fillScreen(TFT_BLACK);


  spriteSkyGround.drawRect(x1 - 1, y1 - 1 - SHIFT_VAL_SG * i, SPRITE_SPEED1_WIDTH + 2, SPRITE_SPEED1_HEIGHT + 2, TFT_LIGHTGREY);
  spriteSkyGround.drawRect(x2 - 1, y2 - 1 - SHIFT_VAL_SG * i, SPRITE_SPEED2_WIDTH + 2, SPRITE_SPEED2_HEIGHT + 2, TFT_LIGHTGREY);
  spriteSkyGround.fillTriangle(x1 - 15, HORIZON_HEIGHT / 2 - SHIFT_VAL_SG * i, 41, y1 - SHIFT_VAL_SG * i, 41, HORIZON_HEIGHT / 2 + 13 - SHIFT_VAL_SG * i, TFT_BLACK);
  spriteSkyGround.drawLine(x1 - 15, HORIZON_HEIGHT / 2 - SHIFT_VAL_SG * i, 41, y1 - 1 - SHIFT_VAL_SG * i, TFT_LIGHTGREY);
  spriteSkyGround.drawLine(x1 - 15, HORIZON_HEIGHT / 2 - SHIFT_VAL_SG * i, 41, HORIZON_HEIGHT / 2 + 14 - SHIFT_VAL_SG * i, TFT_LIGHTGREY);


  float offsetU = fmod(_speed, 1) * HEIGHT_CHAR;
  int unites = (int(_speed / 1)) % 10;
  int dizaines = (int(_speed / 10)) % 10;
  int centaines = (int(_speed / 100)) % 10;


  spriteSpeed2.drawNumber((unites + 2) >= 10 ? unites - 8 : (unites + 2), WIDTH_CHAR, COORD_YL + (offsetU)-HEIGHT_CHAR * 2);
  spriteSpeed2.drawNumber((unites + 1) >= 10 ? unites - 9 : (unites + 1), WIDTH_CHAR, COORD_YL + (offsetU)-HEIGHT_CHAR);
  spriteSpeed2.drawNumber(unites, WIDTH_CHAR, COORD_YL + (offsetU));
  spriteSpeed2.drawNumber((unites - 1) < 0 ? unites + 9 : (unites - 1), WIDTH_CHAR, COORD_YL + (offsetU) + HEIGHT_CHAR);
  spriteSpeed2.drawNumber((unites - 2) < 0 ? unites + 8 : (unites - 2), WIDTH_CHAR, COORD_YL + (offsetU) + HEIGHT_CHAR * 2);




  if (unites == 9)
    {
      spriteSpeed1.drawNumber((dizaines + 1) >= 10 ? dizaines - 9 : (dizaines + 1), WIDTH_CHAR * 2, COORD_YS + (offsetU)-HEIGHT_CHAR);
      if (_speed >= 10.0)
        {
          spriteSpeed1.drawNumber(dizaines, WIDTH_CHAR * 2, COORD_YS + (offsetU));
          spriteSpeed1.drawNumber((dizaines - 1) < 0 ? dizaines + 9 : (dizaines - 1), WIDTH_CHAR * 2, COORD_YS + (offsetU) + HEIGHT_CHAR);
        }
    }
  else
    {
      if (_speed >= 10.0)
        {
          spriteSpeed1.drawNumber(dizaines, WIDTH_CHAR * 2, COORD_YS);
        }
    }


  if (dizaines == 9 && unites == 9)
    {
      spriteSpeed1.drawNumber((centaines + 1) >= 10 ? centaines - 9 : (centaines + 1), WIDTH_CHAR, COORD_YS + (offsetU)-HEIGHT_CHAR);
      if (_speed >= 100.0)
        {
          spriteSpeed1.drawNumber(centaines, WIDTH_CHAR, COORD_YS + (offsetU));
          spriteSpeed1.drawNumber((centaines - 1) < 0 ? centaines + 9 : (centaines - 1), WIDTH_CHAR, COORD_YS + (offsetU) + HEIGHT_CHAR);
        }
    }
  else
    {
      if (_speed >= 100.0)
        {
          spriteSpeed1.drawNumber(centaines, WIDTH_CHAR, COORD_YS);
        }
    }


  spriteSpeed1.pushSprite(x1, y1 - SHIFT_VAL_SG * i);
  spriteSpeed2.pushSprite(x2, y2 - SHIFT_VAL_SG * i);


  // Now we display the speed limits "arcs" (actually speed bars)
  // The float parameter _speed is sent by the pessure sensors module in the unit chosen by the user.
  // The limits of the different arcs are in Km/h. So _speed, if not in Km/h, must first be converted in Km/h. Then rounded and cast to int.
  int16_t speedInt;
  if (speedUnit == 0) speedInt = (int)(_speed + 0.5);  // speed unit is Km/h
  else speedInt = (int)(toModuleData.speedConversionFactor * _speed + 0.5);


  // ---------- white bar -----------------
  spriteFlapSpeed.fillSprite(TFT_BLUE);
  spriteLimitSpeeds.fillSprite(TFT_BLUE);
  // We consider a vertical speed scale starting at 0 km/h at the bottom. 100 km/h corresponds to 260 pixels.
  // The sprite “moves” virtually along this scale, centered on the current speed _speed
  // Since the sprite has a vertical range of 100 km/h/260 pixels, it is centered at _speed*260/100 pixels. Its upper edge (the origin of the sprite's coordinates) is 50 km/h/130 pixels higher. Therefore:
  int16_t upperLimSprite = SPRITESPEEDPIX_EXTENT / 2 + speedInt * SPRITESPEEDPIX_EXTENT / SPRITESPEEDKMH_EXTENT;
  // The white arc extends from whiteArc[0] to whiteArc[1], or 90 to 170 km/h, or 234 to 442 pixels.
  // Knowing these coordinates, we can draw the white arc in the sprite; in fact, only the part visible in the sprite will be drawn.
  //tft.drawNumber(upperLimSprite, 300, 405);
  //tft.drawNumber(upperLimWhiteArc, 300, 425);
  //tft.drawNumber(lengthWhiteArc, 300, 445);


  if (speedInt - 50 <= whiteArc[1])  // Only if the white arc appears in the sprite at a height of 100 km/h, then we calculate the Y coordinates of the beginning and end of the white arc relative to the top of the sprite.
    {
      spriteFlapSpeed.fillRect(0, upperLimSprite - upperLimWhiteArc, SPRITESPEED_WIDTH, lengthWhiteArc, TFT_WHITE);
      spriteFlapSpeed.pushSprite(2, HORIZON_HEIGHT / 2 - SPRITESPEEDPIX_EXTENT / 2 - SHIFT_VAL_SG * i, TFT_BLUE);
    }
  // ---------- green yellow red bar ----------
  spriteLimitSpeeds.fillRect(0, upperLimSprite - upperLimGreenArc, SPRITESPEED_WIDTH, lengthGreenArc, TFT_GREEN);
  spriteLimitSpeeds.fillRect(0, upperLimSprite - upperLimYellowArc, SPRITESPEED_WIDTH, lengthYellowArc, TFT_YELLOW);
  spriteLimitSpeeds.fillRect(0, upperLimSprite - upperLimRedArc, SPRITESPEED_WIDTH, lengthRedArc, TFT_RED);
  spriteLimitSpeeds.pushSprite(14, HORIZON_HEIGHT / 2 - SPRITESPEEDPIX_EXTENT / 2 - SHIFT_VAL_SG * i, TFT_BLUE);
}


void altiDraw(float _alti, uint8_t i)
{
  static const uint32_t x1 = HORIZON_WIDTH - 102;
  static const uint32_t y1 = HORIZON_HEIGHT / 2 - 15;
  static const uint32_t x2 = x1 + WIDTH_CHAR * 3;
  static const uint32_t y2 = y1 - (SPRITE_ALTI2_HEIGHT - SPRITE_ALTI1_HEIGHT) / 2;


  spriteAlti1.fillScreen(TFT_BLACK);
  spriteAlti2.fillScreen(TFT_BLACK);


  if (_alti < 0)
    {
      spriteSkyGround.drawRect(x1 - 1, y1 - 1 - SHIFT_VAL_SG * i, SPRITE_ALTI1_WIDTH + 2, SPRITE_ALTI1_HEIGHT + 2, TFT_YELLOW);
      spriteSkyGround.drawRect(x2 - 1, y2 - 1 - SHIFT_VAL_SG * i, SPRITE_ALTI2_WIDTH + 2, SPRITE_ALTI2_HEIGHT + 2, TFT_YELLOW);
      spriteAlti1.fillScreen(TFT_RED);
      spriteAlti2.fillScreen(TFT_RED);
      spriteAlti1.setFont(&FreeSansBold12pt7b);
      spriteAlti2.setFont(&FreeSansBold12pt7b);
      spriteAlti1.setTextColor(TFT_WHITE, TFT_RED);
      spriteAlti2.setTextColor(TFT_WHITE, TFT_RED);
      spriteAlti1.drawString("neg.", 60, 3);
      spriteAlti2.drawString("alt.", 37, 18);
      spriteAlti1.setFont(&FreeSansBold18pt7b);
      spriteAlti2.setFont(&FreeSansBold18pt7b);
      spriteAlti1.setTextColor(TFT_WHITE, TFT_BLACK);
      spriteAlti2.setTextColor(TFT_WHITE, TFT_BLACK);
      spriteAlti1.pushSprite(x1, y1 - SHIFT_VAL_SG * i);
      spriteAlti2.pushSprite(x2, y2 - SHIFT_VAL_SG * i);
      return;
    }


  spriteSkyGround.drawRect(x1 - 1, y1 - 1 - SHIFT_VAL_SG * i, SPRITE_ALTI1_WIDTH + 2, SPRITE_ALTI1_HEIGHT + 2, TFT_LIGHTGREY);
  spriteSkyGround.drawRect(x2 - 1, y2 - 1 - SHIFT_VAL_SG * i, SPRITE_ALTI2_WIDTH + 2, SPRITE_ALTI2_HEIGHT + 2, TFT_LIGHTGREY);


  float offsetdizaines = fmod(_alti, 10);
  float offsetD = fmod(_alti, 10) * HEIGHT_CHAR / 10.0;


  int32_t yl = COORD_YL + offsetD + 0.5;
  int32_t ys = COORD_YS + offsetD + 0.5;


  //int unites = (int(_alti / 1)) % 10;
  int dizaines = (int(_alti / 10)) % 10;
  int centaines = (int(_alti / 100)) % 10;
  int milliers = (int(_alti / 1000)) % 10;
  int dixmilliers = (int(_alti / 10000)) % 10;


  spriteAlti2.drawNumber((dizaines + 2) >= 10 ? dizaines - 8 : (dizaines + 2), WIDTH_CHAR, yl - HEIGHT_CHAR * 2);
  spriteAlti2.drawNumber(0, WIDTH_CHAR * 2, yl - HEIGHT_CHAR * 2);
  spriteAlti2.drawNumber((dizaines + 1) >= 10 ? dizaines - 9 : (dizaines + 1), WIDTH_CHAR, yl - HEIGHT_CHAR);
  spriteAlti2.drawNumber(0, WIDTH_CHAR * 2, yl - HEIGHT_CHAR);
  spriteAlti2.drawNumber(dizaines, WIDTH_CHAR, yl);
  spriteAlti2.drawNumber(0, WIDTH_CHAR * 2, yl);
  spriteAlti2.drawNumber((dizaines - 1) < 0 ? dizaines + 9 : (dizaines - 1), WIDTH_CHAR, yl + HEIGHT_CHAR);
  spriteAlti2.drawNumber(0, WIDTH_CHAR * 2, yl + HEIGHT_CHAR);
  spriteAlti2.drawNumber((dizaines - 2) < 0 ? dizaines + 8 : (dizaines - 2), WIDTH_CHAR, yl + HEIGHT_CHAR * 2);
  spriteAlti2.drawNumber(0, WIDTH_CHAR * 2, yl + HEIGHT_CHAR * 2);


  if (dizaines == 9)
    {
      spriteAlti1.drawNumber((centaines + 1) >= 10 ? centaines - 9 : (centaines + 1), WIDTH_CHAR * 3, ys - HEIGHT_CHAR);
      if (_alti >= 100.0)
        {
          spriteAlti1.drawNumber(centaines, WIDTH_CHAR * 3, ys);
          spriteAlti1.drawNumber((centaines - 1) < 0 ? centaines + 9 : (centaines - 1), WIDTH_CHAR * 3, ys + HEIGHT_CHAR);
        }
    }
  else
    {
      if (_alti >= 100.0)
        {
          spriteAlti1.drawNumber(centaines, WIDTH_CHAR * 3, COORD_YS);
        }
    }


  if (centaines == 9 && dizaines == 9)
    {
      spriteAlti1.drawNumber((milliers + 1) >= 10 ? milliers - 9 : (milliers + 1), WIDTH_CHAR * 2, ys - HEIGHT_CHAR);
      if (_alti >= 1000.0)
        {
          spriteAlti1.drawNumber(milliers, WIDTH_CHAR * 2, ys);
          spriteAlti1.drawNumber((milliers - 1) < 0 ? milliers + 9 : (milliers - 1), WIDTH_CHAR * 2, ys + HEIGHT_CHAR);
        }
    }
  else
    {
      if (_alti >= 1000.0)
        {
          spriteAlti1.drawNumber(milliers, WIDTH_CHAR * 2, COORD_YS);
        }
    }


  if (milliers == 9 && centaines == 9 && dizaines == 9)
    {
      spriteAlti1.drawNumber((dixmilliers + 1) >= 10 ? dixmilliers - 9 : (dixmilliers + 1), WIDTH_CHAR, ys - HEIGHT_CHAR);
      if (_alti >= 10000.0)
        {
          spriteAlti1.drawNumber(dixmilliers, WIDTH_CHAR, ys);
          spriteAlti1.drawNumber((dixmilliers - 1) < 0 ? dixmilliers + 9 : (dixmilliers - 1), WIDTH_CHAR, ys + HEIGHT_CHAR);
        }
    }
  else
    {
      if (_alti >= 10000.0)
        {
          spriteAlti1.drawNumber(dixmilliers, WIDTH_CHAR, COORD_YS);
        }
    }


  spriteAlti1.pushSprite(x1, y1 - SHIFT_VAL_SG * i);
  spriteAlti2.pushSprite(x2, y2 - SHIFT_VAL_SG * i);
}


void magHeadDraw(float _magHead)
{
  spriteSkyGround.setTextDatum(top_center);
  spriteSkyGround.setFont(&Font4);
  spriteSkyGround.setTextColor(TFT_WHITE);


  spriteSkyGround.drawNumber(330, HORIZON_WIDTH / 2 - 300 - (_magHead * 10), 0);
  spriteSkyGround.fillRect(HORIZON_WIDTH / 2 - 300 - (_magHead * 10), 25, 3, 15, TFT_WHITE);
  spriteSkyGround.fillRect(HORIZON_WIDTH / 2 - 300 + 50 - (_magHead * 10), 30, 3, 10, TFT_WHITE);


  spriteSkyGround.drawNumber(340, HORIZON_WIDTH / 2 - 200 - (_magHead * 10), 0);
  spriteSkyGround.fillRect(HORIZON_WIDTH / 2 - 200 - (_magHead * 10), 25, 3, 15, TFT_WHITE);
  spriteSkyGround.fillRect(HORIZON_WIDTH / 2 - 200 + 50 - (_magHead * 10), 30, 3, 10, TFT_WHITE);


  spriteSkyGround.drawNumber(350, HORIZON_WIDTH / 2 - 100 - (_magHead * 10), 0);
  spriteSkyGround.fillRect(HORIZON_WIDTH / 2 - 100 - (_magHead * 10), 25, 3, 15, TFT_WHITE);
  spriteSkyGround.fillRect(HORIZON_WIDTH / 2 - 100 + 50 - (_magHead * 10), 30, 3, 10, TFT_WHITE);


  for (int8_t n = 0; n < 39; n++)
    {
      spriteSkyGround.drawNumber((n * 10) % 360, HORIZON_WIDTH / 2 + n * 100 - (_magHead * 10), 0);
      spriteSkyGround.fillRect(HORIZON_WIDTH / 2 + n * 100 - (_magHead * 10), 25, 3, 15, TFT_WHITE);
      spriteSkyGround.fillRect(HORIZON_WIDTH / 2 + n * 100 + 50 - (_magHead * 10), 30, 3, 10, TFT_WHITE);
    }
  spriteMagHead.setCursor(2, 2);
  spriteMagHead.printf("%03.0f", _magHead);
  spriteMagHead.pushSprite(HORIZON_WIDTH / 2 - 28, 1);
  spriteSkyGround.drawRect(HORIZON_WIDTH / 2 - 29, 0, 63, 33, TFT_LIGHTGREY);
}


void varioDraw(float _vario)
{
  spriteVario.clear();
  spriteVario.fillRect(0, SPRITE_VARIO_HEIGHT / 2, SPRITE_VARIO_WIDTH, -1 * (_vario * SPRITE_VARIO_HEIGHT / 4000), (_vario >= 0 ? TFT_GREEN : TFT_MAGENTA));
  for (uint8_t n = 0; n < 5; n++)
    {
      spriteVario.fillRect(0, -1 + n * SPRITE_VARIO_HEIGHT / 4, SPRITE_VARIO_WIDTH, 2, TFT_WHITE);
      spriteVario.fillRect(0, -1 + n * SPRITE_VARIO_HEIGHT / 4 + SPRITE_VARIO_HEIGHT / 8, SPRITE_VARIO_WIDTH, 2, TFT_WHITE);
    }
}


void billeDraw(int8_t ofs, uint8_t i)
{
  int32_t n = SHIFT_VAL_SG * i;
  int32_t m = HORIZON_HEIGHT - BALL_RADIUS - 1 - n;


  spriteSkyGround.fillCircle(DL + ofs, m, BALL_RADIUS, TFT_BLACK);
  spriteSkyGround.fillCircle(DL + ofs, m, BALL_RADIUS - 2, TFT_WHITE);
  spriteSkyGround.fillRect(DL - 19, HORIZON_HEIGHT - 30 - n, 4, 30, TFT_BLACK);
  spriteSkyGround.fillRect(DL - 18, HORIZON_HEIGHT - 29 - n, 2, 29, TFT_WHITE);
  spriteSkyGround.fillRect(DL + 16, HORIZON_HEIGHT - 30 - n, 4, 30, TFT_BLACK);
  spriteSkyGround.fillRect(DL + 17, HORIZON_HEIGHT - 29 - n, 2, 29, TFT_WHITE);
}


void textDraw(uint8_t n)
{
  spriteSkyGround.setTextColor(TFT_WHITE);
  spriteSkyGround.setFont(&Font4);
  // ---------- display QNH/QFE + alt QFE----------
  spriteSkyGround.setTextDatum(top_right);
  if (baroSetUnit == 1)  // if baro unit is InHg*100
    {
      if (displayQFE)
        {
          spriteSkyGround.drawNumber(fromRDAMdata.qfeAltitudeInt, HORIZON_WIDTH - 5, HORIZON_HEIGHT - 75 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawString("QFE", HORIZON_WIDTH - 80, HORIZON_HEIGHT - 50 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawString("QNH", HORIZON_WIDTH - 80, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawFloat((float)toModuleData.QFE / 100, 2, HORIZON_WIDTH - 5, HORIZON_HEIGHT - 50 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawFloat((float)toModuleData.QNH / 100, 2, HORIZON_WIDTH - 5, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
        }
      else
        {
          spriteSkyGround.setCursor(HORIZON_WIDTH - 80, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
          spriteSkyGround.printf("%02u.%02u", (toModuleData.QNH / 100), (toModuleData.QNH % 100));
        }
    }
  else  // if baro unit is hPa
    {
      uint16_t i, j;
      if (displayQFE)
        {
          i = (int)(toModuleData.QFE * 0.338646 + 0.5);  // conversion from InHG*100 to hPa
          j = (int)(toModuleData.QNH * 0.338646 + 0.5);  // conversion from InHG*100 to hPa
          spriteSkyGround.drawNumber(fromRDAMdata.qfeAltitudeInt, HORIZON_WIDTH - 5, HORIZON_HEIGHT - 75 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawString("QFE", HORIZON_WIDTH - 70, HORIZON_HEIGHT - 50 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawString("QNH", HORIZON_WIDTH - 70, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawNumber(i, HORIZON_WIDTH - 5, HORIZON_HEIGHT - 50 - SHIFT_VAL_SG * n);
          spriteSkyGround.drawNumber(j, HORIZON_WIDTH - 5, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
        }
      else
        {
          spriteSkyGround.setCursor(HORIZON_WIDTH - 75, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
          j = (int)(toModuleData.QNH * 0.338646 + 0.5);  // conversion from InHG*100 to hPa
          spriteSkyGround.printf("%4d", j);
        }
    }
  spriteSkyGround.setTextDatum(top_left);


  // ---------- GNSS time display----------
  spriteSkyGround.setTextColor(TFT_GREEN);
  spriteSkyGround.setCursor(27, HORIZON_HEIGHT - 25 - SHIFT_VAL_SG * n);
  int8_t hourCorr = fromRDAMdata.hourGNSS + locTimeCorr;
  if (hourCorr < 0) hourCorr += 24;
  if (hourCorr >= 24) hourCorr -= 24;
  spriteSkyGround.printf("%02d:%02d:%02d", hourCorr, fromRDAMdata.minuteGNSS, fromRDAMdata.secondGNSS);
  spriteSkyGround.setTextColor(TFT_WHITE);


  // ---------- Speed ​​unit display ----------
  spriteSkyGround.setCursor(50, DH + 35 - SHIFT_VAL_SG * n);
  spriteSkyGround.setFont(&FreeSans9pt7b);
  spriteSkyGround.print(strSpeedUnit);


  // ---------- True Air Speed display ----------
  spriteSkyGround.setTextDatum(top_right);
  spriteSkyGround.setFont(&Font4);
  spriteSkyGround.drawNumber(fromRDAMdata.trueAirSpeed, 100, DH - 55 - SHIFT_VAL_SG * n);  // The temporary (green) pressure sensors module (or the future RDAM) transmits TAS in the unit selected by the user


  // ---------- OAT and G display----------
  drawTemp(fromRDAMdata.oat, 85, 70, n);
  spriteSkyGround.setCursor(27, DH + 70 - SHIFT_VAL_SG * n);
  spriteSkyGround.printf("G: %3.1f", fromRDAMdata.AccZ * -1 / 9.80665);  // conversion from m/s² to g

  // ---------- Display of pressure altitude and density altitude ----------
  spriteSkyGround.setTextDatum(top_right);
  int32_t altpress = (((int)fromRDAMdata.pressureAltitude + 0.5) / 10) * 10;
  spriteSkyGround.drawNumber(altpress, HORIZON_WIDTH - 5, DH + 35 - SHIFT_VAL_SG * n);
  spriteSkyGround.drawNumber(fromRDAMdata.densityAltitudeInt, HORIZON_WIDTH - 5, DH - 55 - SHIFT_VAL_SG * n);

  // ----------  Display GNSS track, altitude and speed ----------
  spriteSkyGround.setTextColor(TFT_GREEN);
  spriteSkyGround.drawNumber((int)(fromRDAMdata.groundSpeedGNSS / toModuleData.speedConversionFactor + 0.5), 100, DH - 81 - SHIFT_VAL_SG * n);  // The AHRS transmits GNSS ground speed in Km/h
  int trkInt;
  if (fromRDAMdata.groundSpeedGNSS < 5) trkInt = 0;  // A trk equal to 0 is displayed if the speed is too low.
  else
    {
      trkInt = (int)(fromRDAMdata.trk + 0.5);
      if (trkInt >= 360) trkInt = 0;
    }
  spriteSkyGround.setFont(&FreeSansBold18pt7b);
  spriteSkyGround.setTextColor(TFT_GREEN, TFT_BLACK);
  spriteSkyGround.setCursor(HORIZON_WIDTH - 80, 50 - SHIFT_VAL_SG * n);
  spriteSkyGround.printf("%03d", trkInt);
  spriteSkyGround.drawRect(HORIZON_WIDTH - 81, 49 - SHIFT_VAL_SG * n, 59, 34, TFT_LIGHTGREY);
  spriteSkyGround.setFont(&Font4);
  spriteSkyGround.setTextColor(TFT_GREEN);
  int16_t altGNSS = int((fromRDAMdata.altitudeGNSS + 5) / 10.0) * 10;
  spriteSkyGround.drawNumber(altGNSS, HORIZON_WIDTH - 5, DH - 85 - SHIFT_VAL_SG * n);
  spriteSkyGround.setTextColor(TFT_WHITE);


  // ---------- Various displays once every 5 seconds in the menu area, if the menu is closed  ----------
  if (!menuOpen && (millis() >= period5000msStartTime + PERIOD_5000MS_TIMEOUT))
    {
      period5000msStartTime = millis();

      tft.setFont(&Font4);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

      tft.drawRect(MENU_X_COORDINATE + 2, MENU_Y_COORDINATE - 2, MENU_WIDTH - 2, MENU_HEIGHT + 2, TFT_WHITE);
      tft.drawFastVLine(MENU_X_COORDINATE + 143, MENU_Y_COORDINATE - 2, MENU_HEIGHT + 2);
      tft.drawFastVLine(MENU_X_COORDINATE + 262, MENU_Y_COORDINATE - 2, MENU_HEIGHT + 2);
      tft.drawFastVLine(MENU_X_COORDINATE + 413, MENU_Y_COORDINATE - 2, MENU_HEIGHT + 2);

      tft.setCursor(264 + 34 + 2, 402);
      tft.print("Wind");
      tft.setCursor(266 + 2, 427);
      tft.print("Dir.");
      tft.setCursor(326 + 16 + 2, 427);
      tft.printf("  %03d", fromRDAMdata.windDirectionInt);
      tft.setCursor(266 + 2, 452);
      tft.print("Speed");
      tft.setCursor(326 + 16 + 2, 452);
      tft.printf("  %03d", fromRDAMdata.windSpeedInt);

      tft.setCursor(MENU_X_COORDINATE + 145 + 5, 410);
      tft.print("NbSat");
      tft.setCursor(MENU_X_COORDINATE + 138 + 77 + 5, 410);
      tft.printf("% 3d", fromRDAMdata.satInView);

      tft.setCursor(MENU_X_COORDINATE + 145 + 5, 445);
      tft.print("IAT");
      tft.setCursor(MENU_X_COORDINATE + 138 + 50 + 5, 445);
      fromRDAMdata.iat = (tempUnit ? fromRDAMdata.iat * 1.8 + 32 : fromRDAMdata.iat);  // Celsius to Farenheit conversion if necessary
      tft.printf("% 3.0f %c", fromRDAMdata.iat, tempUnit ? 'F' : 'C');

      tft.setCursor(MENU_X_COORDINATE + 264 + 4 + 2, 410);
      tft.print("DewPt");
      tft.setCursor(MENU_X_COORDINATE + 264 + 4 + 2 + 66 + 10, 410);
      fromRDAMdata.dewPoint = (tempUnit ? fromRDAMdata.dewPoint * 1.8 + 32 : fromRDAMdata.dewPoint);  // Celsius to Farenheit conversion if necessary
      tft.printf("% 3.0f %c", fromRDAMdata.dewPoint, tempUnit ? 'F' : 'C');

      tft.setCursor(MENU_X_COORDINATE + 264 + 4 + 2, 445);
      tft.print("Rel.H");
      tft.setCursor(MENU_X_COORDINATE + 264 + 4 + 2 + 66 + 10, 445);
      tft.printf("%2d%s", int(fromRDAMdata.rh + 0.5), " %");

      tft.setCursor(MENU_X_COORDINATE + 385 + 3 + 34, 410);
      tft.print("Gmax");
      tft.setCursor(MENU_X_COORDINATE + 385 + 64 + 8 + 34, 410);
      tft.printf(" %3.1f", accZmax* -1 / 9.80665); // conversion from m/s² to g

      tft.setCursor(MENU_X_COORDINATE + 385 + 3 + 34, 440);
      tft.print("Gmin");
      tft.setCursor(MENU_X_COORDINATE + 385 + 64 + 8 + 34, 440);
      tft.printf(" %3.1f", accZmin* -1 / 9.80665); // conversion from m/s² to g

      tft.setTextColor(TFT_WHITE);
    }
}

// *************************************************************** Counting elapsed seconds from power ON **************************************************
void countSeconds()
{
  if (xTaskGetTickCount() >= startTime + TIMEOUT)
    {
      startTime += TIMEOUT;
      seconds++;
    }
}

// *************************************************************** Managing timers **************************************************
void timerDraw(uint8_t n)
{
  if (timerDownFlag)  // Timer down
    {
      spriteSkyGround.setCursor(27, HORIZON_HEIGHT - 75 - SHIFT_VAL_SG * n);
      spriteSkyGround.setTextColor(TFT_PINK);
      spriteSkyGround.printf("%02d:%02d", timerDownValueSec / 60, timerDownValueSec % 60);
      spriteSkyGround.setTextColor(TFT_WHITE);
      if (seconds > prevSecondsDown)
        {
          timerDownValueSec--;
          prevSecondsDown = seconds;
        }
      if (timerDownValueSec == -1) timerDownFlag = false;
    }

  if (timerUpFlag)  // Timer up
    {
      spriteSkyGround.setCursor(27, HORIZON_HEIGHT - 50 - SHIFT_VAL_SG * n);
      uint32_t nbSec = timerUpValueSec - timerUpStartTime;
      uint32_t h = (nbSec) / 3600;
      uint32_t m = ((nbSec) % 3600) / 60;
      uint32_t s = (nbSec) % 60;
      spriteSkyGround.setTextColor(TFT_YELLOW);
      spriteSkyGround.printf("%02u:%02u:%02u", h, m, s);
      spriteSkyGround.setTextColor(TFT_WHITE);
      if (seconds > prevSecondsUp)
        {
          timerUpValueSec++;
          prevSecondsUp = seconds;
        }
    }
}

void drawFixedItems()
{
  // Drawing of the fixed part of the variometer
  tft.fillRect(VARIO_X, VARIO_Y - 1, 3, SPRITE_VARIO_HEIGHT + 2, TFT_WHITE);
  for (uint8_t n = 0; n < 5; n++)
    {
      tft.fillRect(VARIO_X + 3, VARIO_Y - 1 + n * (SPRITE_VARIO_HEIGHT / 4), 20, 2, TFT_WHITE);
      if (n < 4) tft.fillRect(VARIO_X + 3, VARIO_Y - 1 + n * (SPRITE_VARIO_HEIGHT / 4) + (SPRITE_VARIO_HEIGHT / 8), 15, 2, TFT_WHITE);
      tft.setCursor(VARIO_X + 18, 0 + n * (SPRITE_VARIO_HEIGHT / 4));
      tft.setFont(&FreeSansBold12pt7b);
      tft.print(abs(2 - n));
    }
  spriteVario.fillScreen(TFT_BLUE);
}


// ************************************************************************************** Two useful primitive graphics functions ********************************************************************************************


void fillPentagon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint16_t color)
{
  spriteSkyGround.fillTriangle(x0, y0, x1, y1, x4, y4, color);
  spriteSkyGround.fillTriangle(x1, y1, x2, y2, x4, y4, color);
  spriteSkyGround.fillTriangle(x2, y2, x3, y3, x4, y4, color);
}


void fillQuadrangle(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
  spriteSkyGround.fillTriangle(x0, y0, x1, y1, x3, y3, color);
  spriteSkyGround.fillTriangle(x1, y1, x2, y2, x3, y3, color);
}
