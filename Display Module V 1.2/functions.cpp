#include <stdint.h>
#include "functions.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                    Functions used when sorting the menu options array and initializing menu options
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void initArray()  // During setup() execution, this function calculates and initializes the "numberOfSiblingItems" and "indexChildParentItem" fields of each record.
{
  for (uint8_t i = 0; i < numberOfMenuItems(); i++)
    {
      uint16_t n = menu[i].menuItemNumber;
      int8_t j = 0;
      if ((n / 10) * 10 == n)
        {
          while (menu[i + j].menuItemNumber < n + 10) j++;
          for (uint8_t k = 0; k < j; k++) menu[i + k].numberOfSiblingItems = j;
          j = 0;
        }
      if (menu[i].menuItemAction == 'S')
        {
          while (menu[i + j].menuItemNumber != n * 10) j++;
          menu[i].indexChildParentItem = i + j;
          j = 0;
        }
      if (menu[i].menuItemAction == 'B')
        {
          while (menu[i + j].menuItemNumber != (n / 100) * 10) j--;
          menu[i].indexChildParentItem = i + j;
        }
    }
}

void bubbleSortArray()  // Sorts the array by ascending numberOfMenuItems
{
  char* text;
  uint32_t number;
  char letter;
  for (byte i = numberOfMenuItems() - 1; i > 0; i--)
    {
      for (byte j = 1; j <= i; j++)
        {
          if (menu[j - 1].menuItemNumber > menu[j].menuItemNumber)
            {
              text = menu[j].menuItemLabel;
              number = menu[j].menuItemNumber;
              letter = menu[j].menuItemAction;
              menu[j].menuItemLabel = menu[j - 1].menuItemLabel;
              menu[j].menuItemNumber = menu[j - 1].menuItemNumber;
              menu[j].menuItemAction = menu[j - 1].menuItemAction;
              menu[j - 1].menuItemLabel = text;
              menu[j - 1].menuItemNumber = number;
              menu[j - 1].menuItemAction = letter;
            }
        }
    }
}

uint8_t numberOfMenuItems()  //Returns the number of rows in the array
{
  uint8_t i = 0;
  while (menu[i].menuItemNumber != 1000000)
    {
      i = i + 1;
    }
  return i;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                          Menu related functions
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void drawMenu(uint8_t ndx)  // This function should receive the index of the first option of the menu/submenu to open
{
  tft.fillRect(MENU_X_COORDINATE, MENU_Y_COORDINATE - 2, MENU_WIDTH, MENU_HEIGHT + 2, TFT_BLACK);  // clearing the menu area
  indexCurrentMenuOption = ndx;

  for (uint8_t n = 0; n < menu[ndx].numberOfSiblingItems; n++)
    {
      drawButton(MENU_X_COORDINATE + n * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE, menu[ndx + n].menuItemLabel, 9);
    }
}

void drawSetValMenu()
{
  tft.fillRect(MENU_X_COORDINATE, MENU_Y_COORDINATE, MENU_WIDTH, MENU_HEIGHT, TFT_BLACK);  // clearing the menu area
  drawButton(MENU_X_COORDINATE, MENU_Y_COORDINATE, "BACK", 9);
  drawButton(MENU_X_COORDINATE + 1 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE, "-", 24);
  drawButton(MENU_X_COORDINATE + 4 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE, "+", 24);
  drawButton(MENU_X_COORDINATE + 5 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE, "QUIT", 9);
}

void drawValToSet()
{
  if (ptrGen == &toModuleData.QNH || ptrGen == &toModuleData.QFE)
    {
      if (baroSetUnit == 1)  // then we will display the QNH/QFE in inches of mercury
        {
          tft.drawFloat((float)*ptrGen / 100, 2, MENU_X_COORDINATE + 3 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE + BUTTON_HEIGHT / 2);
        }
      else  // otherwise we will display the QNH/QFE in hPa
        {
          float i = *ptrGen * 0.338646;
          uint16_t j = int(i + 0.5);
          tft.drawNumber(j, MENU_X_COORDINATE + 3 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE + BUTTON_HEIGHT / 2);
        }
    }
  else tft.drawNumber(*ptrGen, MENU_X_COORDINATE + 3 * (BUTTON_WIDTH + BUTTON_MARGIN) + BUTTON_MARGIN, MENU_Y_COORDINATE + BUTTON_HEIGHT / 2);
}

void drawButton(uint16_t x, uint16_t y, char* caption, int8_t charSize)  //Originally, this program was written for a touchscreen, but this proved unusable on an airplane. A non-touchscreen matte display and physical push buttons were chosen instead.
{
  tft.fillRoundRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, 10, 0x632C);                 // very dark (100, 100, 100)
  tft.fillRoundRect(x, y, BUTTON_WIDTH - 4, BUTTON_HEIGHT - 4, 10, 0xE71C);         // very light (225, 225, 225)
  tft.fillRoundRect(x + 4, y + 4, BUTTON_WIDTH - 8, BUTTON_HEIGHT - 8, 8, 0xA514);  // intermediate (165, 165, 465)
  tft.setTextDatum(middle_center);
  tft.setTextColor(TFT_BLACK);
  switch (charSize)
    {
      case 9:
        tft.setFont(&FreeSansBold9pt7b);
        break;
      case 24:
        tft.setFont(&FreeSansBold24pt7b);
        break;
      default:
        tft.setFont(&FreeSansBold9pt7b);
        break;
    }
  char* ptr = strchr(caption, '_');  // Is there a '_' char in the button caption ? If not, ptr will be NULL.
  if (ptr == nullptr)                // if true, there is no '_' char, the caption will be displayed on one line;
    {
      tft.drawString(caption, x - 2 + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT / 2);
    }
  else  //If there is a '_' char in the caption, we will split it to display it on two lines (without using strtok which malfunctions in that case);
    {
      char str1[8] = "";  // two variable char buffers
      char str2[8] = "";
      uint8_t ndx = ptr - caption;
      for (uint8_t n = 0; n < ndx; n++) str1[n] = caption[n];
      for (uint8_t n = ndx + 1; n < strlen(caption); n++) str2[n - (ndx + 1)] = caption[n];
      tft.drawString(str1, x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT / 2 - 10);
      tft.drawString(str2, x + BUTTON_WIDTH / 2, y + BUTTON_HEIGHT / 2 + 15);
    }
  tft.setTextDatum(top_left);
  tft.setFont(&FreeSansBold9pt7b);
}

void closeMenu()
{
  tft.fillRect(MENU_X_COORDINATE, MENU_Y_COORDINATE, MENU_WIDTH, MENU_HEIGHT, TFT_BLACK);  // clearing the menu area
  menuOpen = false;
  setVal = false;
  indexCurrentMenuOption = 0;
}

bool verifNdx(uint8_t ndx)  // For debugging purposes, this function allows you to check that a value is in a list.
{
  uint8_t tab[] = { 0, 3, 8, 14, 18, 23, 28, 33, 37 };
  bool exists = std::find(std::begin(tab), std::end(tab), ndx) != std::end(tab);
  return exists;
}

void checkDisplayValue()  // This function ensures that certain parameters modified in the menus remain within the authorized limits, then displays the new value.
{
  if (timerDownPeriod > 60) timerDownPeriod = 60;
  if (timerDownPeriod < 0) timerDownPeriod = 0;
  if (locTimeCorr < -12) locTimeCorr = -12;
  if (locTimeCorr > 12) locTimeCorr = 12;
  if (backLight > 16) backLight = 16;
  if (backLight < 1) backLight = 1;
  drawValToSet();
}

void validateSaveValue()  // This function validate the modification of a value, and save it on the flash memory.
{
  setVal = false;
  tft.setTextDatum(top_left);
  tft.setTextPadding(0);
  tft.setFont(&FreeSansBold9pt7b);
  efisPrefs.begin("EFISPrefs", false);
  efisPrefs.putShort(efisPrefsKey, *ptrGen);
  efisPrefs.end();
  if (strcmp(efisPrefsKey, "timerDwnPeriod") == 0)  // If the value just validated is timerDownPeriod, the corresponding timer can start counting down.
    {
      prevSecondsDown = seconds;
      timerDownFlag = true;  //
      timerDownValueSec = *ptrGen * 60;
    }
  ptrGen = nullptr;
}

uint8_t indexFirstSiblingOption(uint8_t anyIndex)  // This function returns, from any given index, the index of the first sibling option (menuItemNumber ending with a zero)
{
  int8_t i = 0;
  while (menu[anyIndex + i].menuItemNumber > ((menu[anyIndex].menuItemNumber) / 10) * 10) i--;
  return (anyIndex + i);
}

void initMenu()
{
  // ******************************************************************************* Initializing and sorting menu options  **************************************************************************************************
  // To facilitate the management of menu options (additions, deletions, modifications), these options are displayed during setup on the serial monitor, before and after sorting.
  uint8_t nb = numberOfMenuItems();
  Serial.println();
  Serial.println("Avionicsduino ESP32 EFIS-EMS Display Module V 1.2");
  Serial.print("Number of useful rows in the table : ");
  Serial.println(nb);
  Serial.println("******** Table before sorting and index initialization **********");
  Serial.println("Index----Label--------------Number---Action---NbSibOpt--indexChildParentItem");
  for (byte n = 1; n <= nb + 1; n++)
    {
      Serial.printf("%2d\t", (n - 1));
      Serial.printf("%-8s\t", (menu[n - 1].menuItemLabel));
      Serial.printf("%7d\t\t", (menu[n - 1].menuItemNumber));
      Serial.print(menu[n - 1].menuItemAction);
      Serial.printf("\t%2d\t", (menu[n - 1].numberOfSiblingItems));
      Serial.printf("%2d", (menu[n - 1].indexChildParentItem));
      Serial.println();
    }
  bubbleSortArray();
  initArray();  // Initialization of the fields "numberOfSiblingItems" and indexChildParentItem"

  Serial.println();
  Serial.println("******** Table after sorting and index initialization **********");
  Serial.println("Index----Label--------------Number---Action---NbSibOpt--indexChildParentItem");
  for (byte n = 1; n <= nb + 1; n++)
    {
      Serial.printf("%2d\t", (n - 1));
      Serial.printf("%-8s\t", (menu[n - 1].menuItemLabel));
      Serial.printf("%7d\t\t", (menu[n - 1].menuItemNumber));
      Serial.print(menu[n - 1].menuItemAction);
      Serial.printf("\t%2d\t", (menu[n - 1].numberOfSiblingItems));
      Serial.printf("%2d", (menu[n - 1].indexChildParentItem));
      Serial.println();
    }
}
// ********************************************************************** Initialization of some variables saved in flash memory  ***********************************************************************************
void flashMemVariablesInit()
{
  efisPrefs.begin("EFISPrefs", true);                        // Open the namespace EFISPrefs (or create it if it doesn't exist) in Read Only mode (true).
  bool alreadyInitialized = efisPrefs.isKey("alreadyInit");  // Test for the existence of the "alreadyInit" key.

  if (!alreadyInitialized)  // If alreadyInitialized is 'false', then the key "alreadyInit" does not yet exist --> this is first-time run. We need to set up all the namespace keys.
    {
      efisPrefs.end();                      // close the namespace in RO mode and...
      efisPrefs.begin("EFISPrefs", false);  // reopen it in RW mode (false) to create the necessary keys and store the initial "factory default" values.
      efisPrefs.putShort("QNH", 2992);
      efisPrefs.putShort("QFE", 2992);
      efisPrefs.putBool("displayQFE", false);
      efisPrefs.putShort("magDev", 3);
      efisPrefs.putShort("locTimeCorr", 1);
      efisPrefs.putShort("pressureCorr", -20);
      efisPrefs.putShort("timerDwnPeriod", 3);
      efisPrefs.putFloat("accZmax", -9.80665);
      efisPrefs.putFloat("accZmin", -9.80665);
      efisPrefs.putUChar("speedUnit", 0);
      efisPrefs.putUChar("baroSetUnit", 0);
      efisPrefs.putUChar("tempUnit", 0);
      efisPrefs.putShort("backLight", 7);
      efisPrefs.putBool("alreadyInit", true);  // Create the "already initialized" key and store a value.
      efisPrefs.end();                         // The "factory defaults" are created and stored so close the namespace in RW mode and...
      efisPrefs.begin("EFISPrefs", true);      //  Reopen it in RO mode (true) so the setup code outside this first-time run 'if' block can retrieve the run-time values from the "EFISPrefs" namespace.
      // In the future, in case there are any changes in the keys of the EFISPrefs namespace (addition, removal, modification of the name of a key...etc),
      // it will be necessary to compile and upload the program by changing to "enabled" the option "Erase all flash before sketch upload" in the "Tools" menu. Thus, the namespace will be entirely recreated.
      // Be cautious if other data are stored  in flash memory : everything will be deleted.
    }

  // Retrieve the operational parameters from the namespace and save them into their run-time variables.
  toModuleData.QNH = efisPrefs.getShort("QNH");
  toModuleData.QFE = efisPrefs.getShort("QFE");
  displayQFE = efisPrefs.getBool("displayQFE");
  toModuleData.magDev = efisPrefs.getShort("magDev");
  locTimeCorr = efisPrefs.getShort("locTimeCorr");
  toModuleData.pressureCorr = efisPrefs.getShort("pressureCorr");
  timerDownPeriod = efisPrefs.getShort("timerDwnPeriod");
  accZmax = efisPrefs.getFloat("accZmax");
  accZmin = efisPrefs.getFloat("accZmin");
  speedUnit = efisPrefs.getUChar("speedUnit");
  baroSetUnit = efisPrefs.getUChar("baroSetUnit");
  tempUnit = efisPrefs.getUChar("tempUnit");
  backLight = efisPrefs.getShort("backLight");
  efisPrefs.end();  // Last run state (or the factory default) is now restored. Close the namespace.
}

void unitsStringsInit()
{
  switch (speedUnit)
    {
      case 0:
        strcat(strSpeedUnit, "Km/h");
        toModuleData.speedConversionFactor = 1.0;
        break;
      case 1:
        strcat(strSpeedUnit, "Kts");
        toModuleData.speedConversionFactor = 1.852;
        break;
      case 2:
        strcat(strSpeedUnit, "MPH");
        toModuleData.speedConversionFactor = 1.609;
        break;
      default:
        break;
    }
  switch (baroSetUnit)
    {
      case 0:
        strcat(strBaroSetUnit, "hPA");
        break;
      case 1:
        strcat(strBaroSetUnit, "InHg");
        break;
      default:
        break;
    }
  switch (tempUnit)
    {
      case 0:
        strcat(strTempUnit, "C");
        break;
      case 1:
        strcat(strTempUnit, "F");
        break;
      default:
        break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                Function used to filter raw data from sensors
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Fonction de filtre à Réponse Impulsionnelle Infinie (RII)
float iirFilter(float previousFilteredValue, float currentValue, float iirFilterCoefficient)
{
  return previousFilteredValue * (1 - iirFilterCoefficient) + currentValue * iirFilterCoefficient;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                ESP-NOW callback functions
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// This callback function is executed when new data is sent
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
  if (status != ESP_NOW_SEND_SUCCESS)
    {
      //Serial.println("Delivery Fail");
    }
}

// This callback function is executed when new data is received, so every 50 ms
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len)
{
  memcpy(&fromRDAMdata, incomingData, sizeof(fromRDAMdata));
  // Here we need to capture the extreme values ​​of AccZ, then filter AccZ. AccZ is received inverted (from the AHRS and the RDAM), negative downwards.
  if ((fromRDAMdata.AccZ < accZmax) && (fromRDAMdata.groundSpeedGNSS >= 20))  // Transient G-forces related to potential jolts during slow taxiing (<20Km/h) are ignored.
    {
      accZmax = fromRDAMdata.AccZ;
      efisPrefs.begin("EFISPrefs", false);  // Open the flash memory namespace in RW mode to save the new value
      efisPrefs.putFloat("accZmax", accZmax);
      efisPrefs.end();
    }
  if ((fromRDAMdata.AccZ > accZmin) && (fromRDAMdata.groundSpeedGNSS >= 20))
    {
      accZmin = fromRDAMdata.AccZ;
      efisPrefs.begin("EFISPrefs", false);  // Open the flash memory namespace in RW mode to save the new value
      efisPrefs.putFloat("accZmin", accZmin);
      efisPrefs.end();
    }
  fromRDAMdata.AccZ = iirFilter(previousFilteredAccZValue, fromRDAMdata.AccZ, 0.05);  
  previousFilteredAccZValue = fromRDAMdata.AccZ;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                             Miscellaneous functions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void fps()  // refresh rate measurement and display
{
  totalLoop = totalLoop + (millis() - topLoop);  //
  if (cptLoop == 500)
    {
      tft.setCursor(761, 465);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setFont(&Font2);
      totalLoop = totalLoop / cptLoop;
      tft.println(1.0 / (totalLoop / 1000.0), 2);
      totalLoop = 0;
      cptLoop = 0;
    }
  cptLoop++;
}
void displayEngineTime()
{
  uint8_t nbHour = fromRDAMdata.totalNbSec / 3600;
  uint8_t nbMin = (fromRDAMdata.totalNbSec % 3600) / 60;
  uint8_t nbSec = (fromRDAMdata.totalNbSec % 60);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFont(&Font4);
  tft.setCursor(160, 461);
  tft.printf("%01u:%02u:%02u", nbHour, nbMin, nbSec);
}

void displayFuel()
{
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(153, 375);
  tft.printf("F used %04.1f", fromRDAMdata.fuelUsed);
  tft.setCursor(150, 395);
  tft.printf("Tank %02u - %02u", fromRDAMdata.lowerLimit, fromRDAMdata.upperLimit);
}

void displayCurrent()
{
  tft.setCursor(165, 420);
  tft.printf("Bat %c%04.1f",((fromRDAMdata.iBat >= 0) ? '+' : '-') , abs(fromRDAMdata.iBat));
  tft.setCursor(165, 437);
  tft.printf("Bus  %04.1f", fromRDAMdata.iBus);
}

void drawTemp(float tmprtr, uint16_t x, uint16_t y, uint8_t n)
{
  tmprtr = (tempUnit ? tmprtr * 1.8 + 32 : tmprtr);  // Celsius to Farenheit conversion if necessary
  spriteSkyGround.setFont(&Font4);
  spriteSkyGround.setTextDatum(top_right);
  spriteSkyGround.drawFloat(tmprtr, 1, x, y - SHIFT_VAL_SG * n);
  spriteSkyGround.setTextDatum(top_center);
  spriteSkyGround.setCursor(x + 2, y - 5 - SHIFT_VAL_SG * n);
  spriteSkyGround.setFont(&Font2);
  spriteSkyGround.print("o");
  spriteSkyGround.setCursor(x + 12, y - SHIFT_VAL_SG * n);
  spriteSkyGround.setFont(&Font4);
  spriteSkyGround.print(tempUnit ? 'F' : 'C');
}
