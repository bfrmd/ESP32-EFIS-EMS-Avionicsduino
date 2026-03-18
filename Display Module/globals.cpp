#include "globals.h"


CircularGauge::CircularGauge(LGFX* _ptrtft, uint16_t _centerX, uint16_t _centerY, uint16_t _radius, float _startAngle, float _range, int _thicknessArc, char* _strCaption, bool _intRound = true, int _nbDec = 1)  // Conctructor
{
  ptr = _ptrtft;
  centerX = _centerX;
  centerY = _centerY;
  radius = _radius;
  angle[0] = _startAngle;
  range = _range;  // the angular extent of the arc
  thicknessArc = _thicknessArc;
  strcpy(strCaption, _strCaption);
  intRound = _intRound;
  nbDec = _nbDec;
}

void CircularGauge::begin(float* _arr)  // The array passed as pointer must contain the 6 limit values in their own unit.
{
  for (uint8_t n = 0; n < 6; n++)  // the 6 limit values are loaded in a float array
    {
      value[n] = _arr[n];
    }

  angle[5] = angle[0] + range;      // the end angle of the arc is equal to its start angle, plus its total extent which is the last element of the 7-element array passed to the class begin function.
  for (uint8_t n = 1; n <= 4; n++)  // the 4 intermediate limit angles are loaded in a float array
    {
      //
      angle[n] = angle[0] + (range * (value[n] - value[0])) / (value[5] - value[0]);
    }
  // ---------- Creation of the sprites ----------
  spriteCanvasCirc.setColorDepth(8);
  spriteNeedleWhite.setColorDepth(8);
  spriteNeedleBlack.setColorDepth(8);
  ptr->setFont(&Font4);
  if (!spriteNeedleBlack.createSprite(radius - thicknessArc + 4, 11))
    {
      ptr->println(F("Create spriteNeedleBlack failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteNeedleWhite.createSprite(radius - thicknessArc + 4, 11))
    {
      ptr->println(F("Create spriteNeedleWhite failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteCanvasCirc.createSprite(radius * 2, radius * 2))
    {
      ptr->println(F("Create spriteCanvasCirc failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteLineCirc.createSprite(thicknessArc + 2, 1))
    {
      ptr->println(F("Create spriteLineCirc failed. Program stopped working"));
      while (1) {}
    }

  spriteCanvasCirc.setTextDatum(middle_center);
  spriteCanvasCirc.setFont(&DejaVu24);
  spriteCanvasCirc.setTextColor(TFT_WHITE, TFT_BLACK);

  // ---------- drawing of the different sprites ----------
  spriteLineCirc.fillSprite(TFT_BLACK);
  spriteNeedleBlack.fillSprite(TFT_BLACK);
  spriteNeedleWhite.fillSprite(TFT_BLACK);
  spriteNeedleWhite.drawWedgeLine(5, 5, radius - thicknessArc + 4, 5, 5, 0, TFT_WHITE);
  spriteNeedleWhite.setPivot(5, 5);
  spriteNeedleBlack.setPivot(5, 5);
  spriteLineCirc.setPivot(-radius + thicknessArc, 0);
  spriteCanvasCirc.fillSprite(TFT_BLACK);

  // ---------- drawing of the different arcs of the main sprite ------------
  for (uint8_t n = 0; n < 5; n++)
    {
      spriteCanvasCirc.fillArc(radius, radius, radius - thicknessArc, radius, angle[n], angle[n + 1], color[n]);
    }
  for (uint8_t n = 1; n < 5; n++)
    {
      spriteLineCirc.pushRotateZoom(&spriteCanvasCirc, radius, radius, angle[n], 1, 1);
    }
  //spriteCanvasCirc.pushSprite(ptr, centerX, centerY);
}

void CircularGauge::drawGauge(float _value)
{
  needleAngle = angle[0] + (range * (_value - value[0])) / (value[5] - value[0]);  //The needle angle is calculated from the current value, the angular extent range and the lower and upper limit values of the arc
  spriteNeedleBlack.pushRotateZoom(&spriteCanvasCirc, radius, radius, previousAngle, 1, 1);
  previousAngle = needleAngle;
  spriteNeedleWhite.pushRotateZoom(&spriteCanvasCirc, radius, radius, needleAngle, 1, 1);
  int majX, majYn, majYs;
  if (radius >= 60)
    {
      majX = 25;
      majYn = 25;
      majYs = 45;
    }
  else
    {
      majX = 10;
      majYn = 15;
      majYs = 35;
    }
  if (!intRound)
    {
      spriteCanvasCirc.drawFloat(_value, nbDec, radius + majX, radius + majYn);
    }
  else
    {
      int intValue = (int(_value + 5) / 10) * 10;
      spriteCanvasCirc.drawNumber(intValue, radius + majX, radius + majYn);
    }
  spriteCanvasCirc.drawString(strCaption, radius + majX, radius + majYs);

  previousAngle = needleAngle;
  spriteCanvasCirc.pushSprite(ptr, centerX, centerY);
}

LinearGauge::LinearGauge(LGFX* _ptrtft, uint16_t _centerX, uint16_t _centerY, uint16_t _gaugeLength, uint16_t _gaugeThickness, char _orientation, char* _strCaption, bool _intRound, int _nbDec)  // Conctructor
{
  ptr = _ptrtft;
  centerX = _centerX;
  centerY = _centerY;
  gaugeLength = _gaugeLength;
  gaugeThickness = _gaugeThickness;
  indexLength = gaugeThickness + 8;
  orientation = _orientation;
  strcpy(strCaption, _strCaption);
  intRound = _intRound;
  nbDec = _nbDec;
}

void LinearGauge::begin(float* _arr)
{
  for (uint8_t n = 0; n < 6; n++)  // the 6 limit values are loaded in a float array
    {
      value[n] = _arr[n];
    }
  valuePix[0] = 0;  // Now we calculate these limits in pixels
  valuePix[5] = gaugeLength;
  for (uint8_t n = 1; n <= 4; n++)  // the 4 intermediate limits in pixels are stored in a float array
    {

      valuePix[n] = valuePix[0] + (gaugeLength * (value[n] - value[0])) / (value[5] - value[0]);
    }
  spriteCanvasLinear.setColorDepth(8);
  spriteIndex.setColorDepth(8);
  spriteCaptionLinear.setColorDepth(8);
  spriteValueLinear.setColorDepth(8);

  if (!spriteCanvasLinear.createSprite(gaugeLength, indexLength))
    {
      ptr->println(F("Create spriteCanvasLinear failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteIndex.createSprite(8, indexLength))
    {
      ptr->println(F("Create spriteIndex failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteCaptionLinear.createSprite(SPRITECAPTION_WIDTH, SPRITECAPTION_HEIGHT))
    {
      ptr->println(F("Create spriteCaptionLinear failed. Program stopped working"));
      while (1) {}
    }

  if (!spriteValueLinear.createSprite(SPRITEVALUE_WIDTH, SPRITEVALUE_HEIGHT))
    {
      ptr->println(F("Create spriteValueLinear failed. Program stopped working"));
      while (1) {}
    }

  spriteCaptionLinear.fillSprite(TFT_BLACK);
  spriteCaptionLinear.setFont(&DejaVu24);
  spriteValueLinear.setFont(&DejaVu24);
  spriteCaptionLinear.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteValueLinear.setTextColor(TFT_WHITE, TFT_BLACK);
  spriteCanvasLinear.setPivot(gaugeLength / 2, indexLength / 2);
  if (orientation == 'H')
    {
      spriteCaptionLinear.setTextDatum(middle_left);
      spriteCaptionLinear.drawString(strCaption, 0, SPRITECAPTION_HEIGHT / 2 + 2);
      spriteValueLinear.setTextDatum(middle_right);
    }
  else
    {
      spriteCaptionLinear.setTextDatum(middle_center);
      spriteCaptionLinear.drawString(strCaption, SPRITECAPTION_WIDTH / 2, SPRITECAPTION_HEIGHT / 2 + 2);
      spriteCaptionLinear.setPivot(SPRITECAPTION_WIDTH / 2, SPRITECAPTION_HEIGHT / 2);
      spriteValueLinear.setTextDatum(middle_center);
      spriteValueLinear.setPivot(SPRITEVALUE_WIDTH / 2, SPRITEVALUE_HEIGHT / 2);
    }

  // ---------- drawing of the index sprite ----------
  spriteIndex.fillSprite(TFT_BLACK);
  spriteIndex.fillRect(1, 0, 6, indexLength, TFT_WHITE);

}

void LinearGauge::drawGauge(float _value)
{
// ---------- drawing the different sections of the main sprite ------------
spriteCanvasLinear.fillSprite(TFT_BLACK);
  for (uint8_t n = 0; n < 5; n++)
    {
      spriteCanvasLinear.fillRect(valuePix[n], (indexLength - gaugeThickness) / 2, (valuePix[n + 1] - valuePix[n]), gaugeThickness, color[n]);
    }
  for (uint8_t n = 1; n <= 4; n++)
    {
      spriteCanvasLinear.drawFastVLine(valuePix[n], (indexLength - gaugeThickness) / 2, gaugeThickness, TFT_BLACK);
    }

  float indexPos = gaugeLength * (_value - value[0]) / (value[5] - value[0]) - 4;
  spriteIndex.pushSprite(&spriteCanvasLinear, indexPos, 0);
  spriteValueLinear.fillSprite(TFT_BLACK);
  if (!intRound)
    {
      if (orientation == 'H')
        {
          spriteValueLinear.drawFloat(_value, nbDec, SPRITEVALUE_WIDTH, SPRITEVALUE_HEIGHT / 2 + 2);
        }
      else
        {
          spriteValueLinear.drawFloat(_value, nbDec, SPRITEVALUE_WIDTH / 2, SPRITEVALUE_HEIGHT / 2 + 2);
        }
    }
  else
    {
      int intValue = (int(_value + 5) / 10) * 10;
      if (orientation == 'H')
        {
          spriteValueLinear.drawNumber(intValue, SPRITEVALUE_WIDTH / 2, SPRITEVALUE_HEIGHT / 2 + 2);
        }
      else
        {
          spriteValueLinear.drawNumber(intValue, SPRITEVALUE_WIDTH / 2, SPRITEVALUE_HEIGHT / 2 + 2);
        }
    }
  if (orientation == 'H')
    {
      spriteCanvasLinear.pushRotateZoom(ptr, centerX, centerY, 0, 1, 1);
      spriteCaptionLinear.pushSprite(ptr, centerX - gaugeLength / 2, centerY - indexLength / 2 - SPRITECAPTION_HEIGHT - 3);
      spriteValueLinear.pushSprite(ptr, centerX + gaugeLength / 2 - SPRITEVALUE_WIDTH, centerY - indexLength / 2 - SPRITEVALUE_HEIGHT - 3);
    }
  else
    {
      spriteCanvasLinear.pushRotateZoom(ptr, centerX, centerY, -90, 1, 1);
      spriteCaptionLinear.pushRotateZoom(ptr, centerX, centerY + gaugeLength / 2 + SPRITECAPTION_HEIGHT / 2 + 3, 0, 1, 1);
      spriteValueLinear.pushRotateZoom(ptr, centerX, centerY - gaugeLength / 2 - SPRITEVALUE_HEIGHT / 2 - 3, 0, 1, 1);
    }
}
