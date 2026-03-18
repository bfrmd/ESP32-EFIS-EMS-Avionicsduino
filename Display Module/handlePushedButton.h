#ifndef HANDLEPUSHEDBUTTON_H
#define HANDLEPUSHEDBUTTON_H

#include"globals.h"

void initInterrupts();
void button0_ISR();
void button1_ISR();
void button2_ISR();
void button3_ISR();
void button4_ISR();
void button5_ISR();
void handlePushedButton();

void initInterrupts()
{
  pinMode(GPIO_NUM_1, INPUT);
  attachInterrupt(GPIO_NUM_1, button0_ISR, RISING);

  pinMode(GPIO_NUM_2, INPUT);
  attachInterrupt(GPIO_NUM_2, button1_ISR, RISING);

  pinMode(GPIO_NUM_42, INPUT);
  attachInterrupt(GPIO_NUM_42, button2_ISR, RISING);

  pinMode(GPIO_NUM_41, INPUT);
  attachInterrupt(GPIO_NUM_41, button3_ISR, RISING);

  pinMode(GPIO_NUM_40, INPUT);
  attachInterrupt(GPIO_NUM_40, button4_ISR, RISING);

  pinMode(GPIO_NUM_39, INPUT);
  attachInterrupt(GPIO_NUM_39, button5_ISR, RISING);

  numPushedButton = 6;
}

void IRAM_ATTR button0_ISR()
{
  numPushedButton = 0;
}

void IRAM_ATTR button1_ISR()
{
  numPushedButton = 1;
}

void IRAM_ATTR button2_ISR()
{
  numPushedButton = 2;
}

void IRAM_ATTR button3_ISR()
{
  numPushedButton = 3;
}

void IRAM_ATTR button4_ISR()
{
  numPushedButton = 4;
}

void IRAM_ATTR button5_ISR()
{
  numPushedButton = 5;
}

void handlePushedButton()
{
  if (numPushedButton < 6)
    {
      if (!menuOpen)  // Here, we handle any keys that are pushed when the menu is closed.
        {
          indexCurrentMenuOption = 0;  // A first index value is assigned when the menu is opened, on the first menu option ("FLIGHT").
          menuOpen = true;
          drawMenu(indexCurrentMenuOption);
        }
      else  // Here, we handle any key pushed when the menu is open. indexCurrentMenuOption is the index of the 1st option of the current menu.
            // indexCurrentMenuOption can be modified by the closeMenu and the drawMenu functions.
        {
          if (setVal == false)  // Here we handle pushed keys that do not concern the setting of a value.
            {
              numTouchedButton = numPushedButton;
              if (numTouchedButton < menu[indexCurrentMenuOption].numberOfSiblingItems)  // we make sure that the key has indeed affected a button currently displayed
                {
                  switch (menu[indexCurrentMenuOption + numTouchedButton].menuItemAction)
                    {
                      case 'Q':  // Exit menu
                        closeMenu();
                        break;
                      case 'S':  // open a child submenu
                        drawMenu(menu[indexCurrentMenuOption + numTouchedButton].indexChildParentItem);
                        break;
                      case 'B':  // Back to the parent menu/submenu
                        drawMenu(menu[indexCurrentMenuOption + numTouchedButton].indexChildParentItem);
                        break;
                      case 'P':
                        setVal = true;                                                           // From here, we deal with touch keys that concern the setting of a value
                        switch (menu[indexCurrentMenuOption + numTouchedButton].menuItemNumber)  // The ptrGen pointer will be assigned to the variable to be modified
                          {
                            case 101:
                              ptrGen = &toModuleData.QNH;
                              strcpy(efisPrefsKey, "QNH");
                              break;
                            case 102:
                              ptrGen = &toModuleData.QFE;
                              strcpy(efisPrefsKey, "QFE");
                              break;
                            case 112:
                              ptrGen = &toModuleData.magDev;
                              strcpy(efisPrefsKey, "magDev");
                              break;
                            case 113:
                              ptrGen = &locTimeCorr;
                              strcpy(efisPrefsKey, "locTimeCorr");
                              break;
                            case 1032:
                              ptrGen = &timerDownPeriod;
                              strcpy(efisPrefsKey, "timerDwnPeriod");
                              break;
                            case 1141:
                              ptrGen = &backLight;
                              strcpy(efisPrefsKey, "backLight");
                              break;
                            case 1142:
                              ptrGen = &toModuleData.pressureCorr;
                              strcpy(efisPrefsKey, "pressureCorr");
                              break;
                            default:  // default of switch case 'P' and menuItemNumber
                              ptrGen = nullptr;
                              break;
                          }
                        drawSetValMenu();
                        tft.setTextPadding(BUTTON_WIDTH * 2);
                        tft.setTextDatum(middle_center);
                        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                        tft.setFont(&FreeSansBold24pt7b);
                        drawValToSet();
                        //tft.drawNumber(*ptrGen, MENU_X_COORDINATE + 3 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE + BUTTON_HEIGHT / 2);
                        tft.setTextDatum(top_left);
                        tft.setFont(&FreeSansBold9pt7b);
                        break;
                      case 'A':  // Now the deal with all other actions
                        switch (menu[indexCurrentMenuOption + numTouchedButton].menuItemNumber)
                          {
                            case 1031:  // Start or stop the up timer
                              timerUpStartTime = seconds;
                              timerUpValueSec = seconds;
                              prevSecondsUp = seconds;
                              timerUpFlag = !timerUpFlag;
                              break;
                            case 1143:  // Reset the max positive and negative G accelerations
                              accZmax = -9.80665;
                              accZmin = -9.80665;
                              efisPrefs.begin("EFISPrefs", false);  // Open the flash memory namespace in RW mode to save the new values
                              efisPrefs.putFloat("accZmax", -9.80665);
                              efisPrefs.putFloat("accZmin", -9.80665);
                              efisPrefs.end();
                              closeMenu();
                              break;
                            case 11111:
                              speedUnit = 0;
                              toModuleData.speedConversionFactor = 1.0;
                              strcpy(strSpeedUnit, "Km/h");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("speedUnit", speedUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11112:
                              speedUnit = 1;
                              toModuleData.speedConversionFactor = 1.852;
                              strcpy(strSpeedUnit, "Kts");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("speedUnit", speedUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11113:
                              speedUnit = 2;
                              toModuleData.speedConversionFactor = 1.609;
                              strcpy(strSpeedUnit, "MPH");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("speedUnit", speedUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11121:
                              baroSetUnit = 0;
                              strcpy(strBaroSetUnit, "hPa");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("baroSetUnit", baroSetUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11122:
                              baroSetUnit = 1;
                              strcpy(strBaroSetUnit, "InHg");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("baroSetUnit", baroSetUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11131:
                              tempUnit = 0;
                              strcpy(strTempUnit, "C");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("tempUnit", tempUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11132:
                              tempUnit = 1;
                              strcpy(strTempUnit, "F");
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putUChar("tempUnit", tempUnit);
                              efisPrefs.end();
                              drawMenu(menu[indexCurrentMenuOption].indexChildParentItem);
                              break;
                            case 11441:
                              displayQFE = !displayQFE;
                              efisPrefs.begin("EFISPrefs", false);
                              efisPrefs.putBool("displayQFE", displayQFE);
                              efisPrefs.end();
                              break;
                            default: break;
                          }
                      default:  // default of switch case action menuItemAction
                        break;
                    }
                }
            }
          else  // Here, we deal with the touch keys that concern the setting of a value. Touching the BACK or QUIT keys validates the new value.
            {
              numTouchedButton = numPushedButton;
              tft.setTextPadding(BUTTON_WIDTH * 2);
              tft.setTextDatum(middle_center);
              tft.setTextColor(TFT_YELLOW, TFT_BLACK);
              tft.setFont(&FreeSansBold24pt7b);
              switch (numTouchedButton)
                {
                  case 0:
                    validateSaveValue();
                    drawMenu(indexCurrentMenuOption);
                    break;
                  case 4:
                    if (baroSetUnit == 0 && (ptrGen == &toModuleData.QNH || ptrGen == &toModuleData.QFE))  // if QNH/QFE is adjusted in hPa, then it must be incremented by 3
                      {
                        *ptrGen += 3;
                      }
                    else *ptrGen += 1;
                    checkDisplayValue();
                    if(ptrGen == &backLight) // If the adjusted value is backLight, the display backlight is adjusted accordingly.
                    {
                      ledcWrite(BACKLIGHT_PWM_PIN, (uint32_t)(backLight*16));
                    }
                    break;
                  case 1:
                    if (baroSetUnit == 0 && (ptrGen == &toModuleData.QNH || ptrGen == &toModuleData.QFE))  // if QNH/QFE is adjusted in hPa, then it must be incremented by 3
                      {
                        *ptrGen -= 3;
                      }
                    else *ptrGen -= 1;
                    checkDisplayValue();
                    if(ptrGen == &backLight)
                    {
                      ledcWrite(BACKLIGHT_PWM_PIN, (uint32_t)(backLight*16));
                    }
                    break;
                  case 5:
                    validateSaveValue();
                    closeMenu();
                    break;
                  default:
                    break;
                }
              tft.setFont(&FreeSansBold9pt7b);
            }
        }
    }
  numPushedButton = 6;
}

#endif