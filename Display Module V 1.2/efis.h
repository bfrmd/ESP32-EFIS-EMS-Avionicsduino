#ifndef EFIS_H
#define EFIS_H

#include"globals.h"
#include "images.h"

void dysplayAndSpritesInit();
void HorizArtdraw(float aRoll, float aPitch, uint8_t i);
void speedDraw(float _speed, uint8_t i);
void altiDraw(float _alti, uint8_t i);
void magHeadDraw(float _magHead);
void varioDraw(float _vario);
void billeDraw(int8_t ofs, uint8_t i);
void textDraw(uint8_t n);

void drawFixedItems();
void fillPentagon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint16_t color);
void fillQuadrangle(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);

void drawTemp(float tmprtr, uint16_t x, uint16_t y, uint8_t n); // this function is also defined as extern in functions.h

void countSeconds();
void timerDraw(uint8_t n);

#endif
