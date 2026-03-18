#ifndef GLOBALS_H
#define GLOBALS_H

// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                  External libraries inclusion
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <LGFX_ESP32_EFIS_EMS_Display_Module_Driver.h>

#include <LovyanGFX.hpp>
#include <Preferences.h>  // Espressif library for storing permanent data in flash non volatile memory. For the ESP32 family, a more modern equivalent of the EEPROM library, significantly easier to use.

#include <esp_now.h>
#include <WiFi.h>

// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                            EFIS Objects instantiation
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
extern LGFX tft;
inline LGFX_Sprite spriteSkyGround(&tft);
inline LGFX_Sprite spriteEMS(&tft);
inline LGFX_Sprite spriteVario(&tft);
inline LGFX_Sprite spriteArcRoll(&spriteSkyGround);
inline LGFX_Sprite spriteMaquette(&spriteSkyGround);
inline LGFX_Sprite spriteGradPitch(&spriteSkyGround);
inline LGFX_Sprite spriteIndexRoll(&spriteSkyGround);
inline LGFX_Sprite spriteAlti1(&spriteSkyGround);   // Sprite for displaying hundreds, thousands, and tens of thousands of feet
inline LGFX_Sprite spriteAlti2(&spriteSkyGround);   // Sprite displaying dozens of feet scrolling vertically
inline LGFX_Sprite spriteSpeed1(&spriteSkyGround);  // Sprite for displaying the tens digits of speed units
inline LGFX_Sprite spriteSpeed2(&spriteSkyGround);  // Sprite displaying speed units digits scrolling vertically
inline LGFX_Sprite spriteMagHead(&spriteSkyGround);
inline LGFX_Sprite spriteFlapSpeed(&spriteSkyGround);  // white arc display sprite for the speed indicator White arc – The flap operating range
inline LGFX_Sprite spriteLimitSpeeds(&spriteSkyGround);

inline Preferences efisPrefs;



// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                            Classes definitions
// --------------------------------------------------------------------------------------------------------------------------------------------------------------


class CircularGauge
{
  // ---------- Class attributes ----------
private:
  int centerX, centerY, radius, thicknessArc;  // the coordinates of the center of the circular gauge, its outer radius, and the thickness of the arc
  float value[6];                              // the 6 limit values ​​passed to the class begin function
  float range;                                 // the total angular extent of the arc passed to the class constructor
  float angle[6];                              // the 6 angular values ​​that correspond to the values ​​passed to the class begin function. They will be calculated by this same begin function
  char strCaption[5];                          // a short string literal passed to the constructor to indicate the measured value.
  bool intRound;                               // if true, the float parameter of the class drawGauge function is converted to an integer and rounded to the nearest ten
  int nbDec;                                   // the number of decimal places of a float
  float needleAngle;
  float previousAngle;
  int color[5] = { TFT_RED, TFT_YELLOW, TFT_GREEN, TFT_YELLOW, TFT_RED };

  LGFX_Sprite spriteCanvasCirc;   // main sprite
  LGFX_Sprite spriteNeedleBlack;  // this sprite used to erase the old needle position
  LGFX_Sprite spriteNeedleWhite;  // the needle sprite
  LGFX_Sprite spriteLineCirc;     // the small linear sprite that allows a clear delimitation of the different colored sectors of the arc
  LGFX* ptr;                      // LGFX type pointer which is used to retrieve, via the constructor, the global display LGFX screen object

  //  ------------ Classe methods --------------
public:
  CircularGauge(LGFX* _ptrtft, uint16_t _centerX, uint16_t _centerY, uint16_t _radius, float _startAngle, float _range, int _thicknessArc, char* _strCaption, bool _intRound, int _nbDec);  // Conctructor
  void begin(float* _arr);
  void drawGauge(float _value);
};

class LinearGauge
{
  // ---------- Class attributes ----------
private:
  int centerX, centerY, gaugeLength, gaugeThickness;  // the coordinates of the center of the linear gauge, its length and thickness in pixels
  int indexLength;                                    // Length of the index
  char orientation;                                   // 'V' for hertical, 'H' for horizontal
  char strCaption[7];                                 // a short string literal passed to the constructor to indicate the measured value.
  bool intRound;                                      // if true, the float parameter of the class drawGauge function is converted to an integer and rounded to the nearest ten
  int nbDec;                                          // the number of decimal places of a float
  float value[6];                                     // the 6 limit values ​​passed to the class begin function
  float valuePix[6];                                  // the 6 limits in pixels ​​that correspond to the values ​​passed to the class begin function. They will be calculated by this same begin function
  float posIndex;                                     // Position of the index in pixels
  int color[5] = { TFT_RED, TFT_YELLOW, TFT_GREEN, TFT_YELLOW, TFT_RED };

  LGFX_Sprite spriteCanvasLinear;   // main sprite
  LGFX_Sprite spriteIndex;          // the index sprite
  LGFX_Sprite spriteCaptionLinear;  // a small sprite to display the caption of the linear gauge
  LGFX_Sprite spriteValueLinear;    // a small sprite to display the value of the linear gauge
  LGFX* ptr;                        // LGFX type pointer which is used to retrieve, via the constructor, the global display LGFX screen object


#define SPRITECAPTION_WIDTH 60
#define SPRITECAPTION_HEIGHT 18
#define SPRITEVALUE_WIDTH 60
#define SPRITEVALUE_HEIGHT 18


  //  ------------ Classe methods --------------
public:
  LinearGauge(LGFX* _ptrtft, uint16_t _centerX, uint16_t _centerY, uint16_t _gaugeLength, uint16_t _gaugeThickness, char _orientation, char* _strCaption, bool _intRound, int _nbDec);  // Conctructor
  void begin(float* _arr);
  void drawGauge(float _value);
};



// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                            Global EMS limits
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
// For each below lines, the 6 limit values are expressed in their own unit.
inline float rpmLimits[6] = { 0, 1200, 3500, 5500, 5800, 6000 };
inline float fuelLevelLimits[6] = { 0, 10, 15, 80, 80, 80 };
inline float fuelFlowLimits[6] = { 0, 0, 0, 25, 25, 25 };
inline float oilPressureLimits[6] = { 0, 1, 2, 5, 7, 8 };
inline float oilTempLimits[6] = { 0, 50, 90, 110, 140, 150 };
inline float vBatLimits[6] = { 11.5, 12.2, 13.3, 14.3, 15, 16 };
inline float mapLimits[6] = { 10, 10, 10, 35, 35, 35 };
inline float cht2Limits[6] = { 0, 50, 80, 110, 125, 140 };
inline float cht3Limits[6] = { 0, 50, 80, 110, 125, 140 };
inline float egt3Limits[6] = { 600, 600, 600, 850, 880, 900 };
inline float egt4Limits[6] = { 600, 600, 600, 850, 880, 900 };
inline float afrLimits[6] = { 10, 10, 10, 14, 17, 20 };



// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                            Global EFIS and EMS variables and constants declaration
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
// ********************************************************************* Menu tree ********************************************************************************
// Declaration of the typedef of a structure, then of an array of this structure. The array will play the role of a database table to contain all the items (options) of the menus/submenus.
// Each row is the equivalent of a record of this "table". Each variable name of the structure is a field name of this table.
typedef struct
{
  char* menuItemLabel;           // Name of the menu option.
  uint32_t menuItemNumber;       // Number of the menu option. This number is unique, acting like a primary key.
  char menuItemAction;           // Type of action expected if this option is validated: Q->Quit or exit menu, S->open child submenu, B->Back to parent submenu, P->Procedure for adjusting a value, A-> other Action
  uint8_t numberOfSiblingItems;  // Number of sibling items, i.e. all options on the same menu/submenu level.
  uint8_t indexChildParentItem;  // After sorting the menu array, this is the index of the first child option if action='S', or of the first sibling parent option if action = 'B'.
                                 // The last two fields are calculated and initialized when sorting during setup.
} OptionMenu;


// Declaration and initialization of an array, named "menu", of type "OptionMenu". Each line of the array characterizes a menu/submenu option.
// The first three fields ("menuItemLabel", "menuItemNumber" and "menuItemAction") of all the lines of this array are initialized "manually" below. The remaining 2 fields will be automatically initialized during setup.
// The indented presentation below, sorted to clearly display the menu tree, allows easy further management of menu options (modification, addition or deletion of options).
// But before use, this array will be sorted and indexed in the setup() function in ascending order of the "menuItemNumber" field. This sorting and indexation will allow for the straightforward operation of the menu/submenus.
// After setup() :
//               - Each first option in any menu/submenu (i.e. all those whose menuItemNumber ends with a zero) provides the number of sibling options in its menu/submenu.
//               - Each option with a menuItemAction letter 'S' provides the index (not the menuItemNumber!) of the first child option,
//               - Each option with a menuItemAction letter 'B' provides the index of the first parent option.
// The setup() function displays the results before and after sorting and fields initialization on the Arduino IDE serial monitor.

inline OptionMenu menu[] = {
  /*  */ { "FLIGHT", 10, 'S' },
  /*      */ { "BACK", 100, 'B' },  // Mandatory : the "menuItemNumber" fields of the same menu/submenu must follow each other without any gap greater than one unit.
  /*      */ { "QNH", 101, 'P' },
  /*      */ { "QFE", 102, 'P' },
  /*      */ { "TIMERS", 103, 'S' },
  /*          */ { "BACK", 1030, 'B' },
  /*          */ { "TIMER_UP", 1031, 'A' },
  /*          */ { "TIMER_DOWN", 1032, 'P' },
  /*          */ { "QUIT", 1033, 'Q' },
  /*      */ { "QUIT", 104, 'Q' },
  /*  */ { "SETUP", 11, 'S' },
  /*      */ { "BACK", 110, 'B' },
  /*      */ { "UNITS", 111, 'S' },
  /*          */ { "BACK", 1110, 'B' },
  /*          */ { "SPEED", 1111, 'S' },
  /*              */ { "BACK", 11110, 'B' },
  /*              */ { "Km/h", 11111, 'A' },
  /*              */ { "Kts", 11112, 'A' },
  /*              */ { "MPH", 11113, 'A' },
  /*              */ { "QUIT", 11114, 'Q' },
  /*          */ { "PRESS.", 1112, 'S' },
  /*              */ { "BACK", 11120, 'B' },
  /*              */ { "hPa", 11121, 'A' },
  /*              */ { "InHg", 11122, 'A' },
  /*              */ { "QUIT", 11123, 'Q' },
  /*          */ { "TEMP.", 1113, 'S' },
  /*              */ { "BACK", 11130, 'B' },
  /*              */ { "DEG._C", 11131, 'A' },
  /*              */ { "DEG._F", 11132, 'A' },
  /*              */ { "QUIT", 11133, 'Q' },
  /*          */ { "QUIT", 1114, 'Q' },
  /*      */ { "MAG._DEV.", 112, 'P' },
  /*      */ { "LOCAL_TIME", 113, 'P' },
  /*      */ { "MORE", 114, 'S' },
  /*          */ { "BACK", 1140, 'B' },
  /*          */ { "BACK_LIGHT", 1141, 'P' },  // it was for backlight adjust
  /*          */ { "PRESS._CORR.", 1142, 'P' },
  /*          */ { "RESET_G", 1143, 'A' },
  /*          */ { "MORE", 1144, 'S' },
  /*              */ { "BACK", 11440, 'B' },
  /*              */ { "DISPLAY_QFE ?", 11441, 'A' },
  /*              */ { "QUIT", 11442, 'Q' },
  /*          */ { "QUIT", 1145, 'Q' },
  /*      */ { "QUIT", 115, 'Q' },
  /*  */ { "QUIT", 12, 'Q' },
  { " ", 1000000, '\0' }  // End of array marker, allowing to easyly count its elements
};

// ********************************************************************* Constants for backlight setting **************************************************************************
#define BACKLIGHT_PWM_PIN GPIO_NUM_7
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
#define PWM_FREQ 75000

// ********************************************************************* Other menu constants and variables **************************************************************************
#define MENU_X_COORDINATE 260
#define MENU_Y_COORDINATE 400
#define MENU_WIDTH 540
#define MENU_HEIGHT 80
#define BUTTON_WIDTH 88
#define BUTTON_HEIGHT 70
#define BUTTON_MARGIN 2
#define DEBOUNCE_TIME 300

inline uint32_t numTouchedButton = 6;
inline volatile uint32_t numPushedButton = 0;
inline bool menuOpen = false;
inline bool setVal = false;
inline uint8_t indexCurrentMenuOption = 0;
inline uint32_t lastTimeButtonTouched;
inline int16_t* ptrGen = nullptr;
inline char efisPrefsKey[16];

// ***************************************************** EFIS variables stored in flash memory. Initialized in the setup() function ***************************************************************
inline int16_t locTimeCorr;
inline float accZmax;
inline float accZmin;
inline uint8_t speedUnit;    // 0 -> Km/h   1 -> Kts     2 -> MPH // Speeds are always calculated in Km/h, then possibly converted to mph or kts, depending on the user's choice
inline uint8_t baroSetUnit;  // 0 -> hPa    1 -> InHg             // QNH and QFE are always calculated in hundredths of an inch of mercury, then possibly converted to hPa, depending on the user's choice.
inline uint8_t tempUnit;     // 0 -> °C     1 -> °F               // Temperatures are always calculated in °C, then possibly converted to °F, depending on the user's choice
inline bool displayQFE;      // Display or not the QFE
inline int16_t backLight;    // Range : 1 to 16

// ********************************************* EFIS variables. Possibly in the future stored in flash SD card and initialized in the setup() function ******************************************************
// Vs0 = 90  : The lower limit of white arc : stalling speed in the landing configuration, at the maximum landing weight.
// Vs1 = 120 : The lower limit of green arc : stalling speed at the maximum takeoff weight, power off and flaps up.
// VFE = 170 : The upper limit of the white arc : maximum speed with the flaps extended.
// VNO = 265 : The upper limit of green arc and lower limit of yellow arc : maximum structural cruising speed. Do not exceed this speed except in smooth air, and only with caution.
// VNE = 320 : Red line : Never exceed speed. Operating above this speed is prohibited since it may result in damage or structural failure.

inline uint16_t whiteArc[2] = { 90, 170 };
inline uint16_t greenArc[2] = { 120, 265 };
inline uint16_t yellowArc[2] = { 266, 320 };
inline uint16_t redArc[2] = { 321, 500 };
inline int16_t upperLimWhiteArc;
inline int16_t lengthWhiteArc;
inline int16_t upperLimYellowArc;
inline int16_t lengthYellowArc;
inline int16_t upperLimGreenArc;
inline int16_t lengthGreenArc;
inline int16_t upperLimRedArc;
inline int16_t lengthRedArc;


// ******************************************************************** Sprites constants ***********************************************************************
#define ARCROLL_WIDTH 252
#define ARCROLL_HEIGHT 81
#define GRADPITCH_HEIGHT 302
#define GRADPITCH_WIDTH 80
#define INDEXROLL_HEIGHT 20
#define INDEXROLL_WIDTH 21
#define MAQUETTE_HEIGHT 36
#define MAQUETTE_WIDTH 100
#define BOUTON_HEIGHT 77
#define BOUTON_WIDTH 92
#define SPRITE_VARIO_WIDTH 12
#define SPRITE_VARIO_HEIGHT 376
#define SPRITE_ALTI1_WIDTH 61
#define SPRITE_ALTI1_HEIGHT 29
#define SPRITE_ALTI2_WIDTH 41
#define SPRITE_ALTI2_HEIGHT 57
#define SPRITE_SPEED1_WIDTH 41
#define SPRITE_SPEED1_HEIGHT 29
#define SPRITE_SPEED2_WIDTH 21
#define SPRITE_SPEED2_HEIGHT 57
#define SPRITE_MAGHEAD_WIDTH 61
#define SPRITE_MAGHEAD_HEIGHT 31

#define SPRITESPEEDKMH_EXTENT 100
#define SPRITESPEEDPIX_EXTENT 260
#define SPRITESPEED_WIDTH 10

#define HORIZON_X 262
#define HORIZON_Y 0
#define HORIZON_WIDTH 504
#define HORIZON_HEIGHT 396
#define VARIO_X 768
#define VARIO_Y 10


#define NB_SLICES_SG 9   // Used for spriteSkyGround (SG) slice rendering. Displaying a small sliced 16-bit sprite is much faster than using a large 8-bit one, when a large 16-bit sprite doesn't fit in the available memory. \
                         // minimum 2, can be 2, 3, 4, 6, 9, 12, to suit the required speed and remaining memory needed, particularly for other sprites. More slices --> less FPS --> more remaining memory
#define SHIFT_VAL_SG 44  // NB_SLICES_SG x SHIFT_VAL_SG : must be equal to HORIZON_HEIGHT ; 9 x 44 = 396 = HORIZON_HEIGHT


// ******************************************************************** Constantes EFIS ***********************************************************************
#define GROUND_COLOR 0xBAC3  // 0b 1011 1010 1100 0011 ou en RGB888 (184, 88, 24) -> ocre
//#define GROUND_COLOR 0x8160
#define SKY_COLOR 0x041F  // 0b 0000 0100 0001 1111 ou en RGB888 (0, 128, 248) -> bleu ciel
//#define SKY_COLOR TFT_BLUE
#define largeur 504
#define hauteur 396
#define CX 252
#define CY 198
#define DL 252
#define DH 198
#define LD 504  // CX + DL
#define LG 0    //CX - DL
#define LH 0    //CY - DH
#define LB 396  //CY + DH
#define BALL_RADIUS 15
#define PIX_PER_DEG 6

// ***************************************************** Constants for displaying speed and altitude in their respective sprites *******************************************************************
#define COORD_YS 1
#define COORD_YL 15
#define HEIGHT_CHAR 32
#define WIDTH_CHAR 20

#define COORDY_SPEED 185


// ******************************************** Other EFIS variable that depend on saved menus options. Initialized in the setup() function ***************************************************************************
inline char strSpeedUnit[5];
inline char strBaroSetUnit[5];
inline char strTempUnit[2];

// ********************************************************************** Timers related variables **********************************************************************************************************
#define TIMEOUT 1000
inline uint32_t startTime;
inline uint32_t seconds = 0;
inline uint32_t prevSecondsUp = 0;
inline uint32_t prevSecondsDown = 0;
inline uint32_t timerUpStartTime;
inline uint32_t timerUpValueSec = 0;
inline int16_t timerDownPeriod;
inline int16_t timerDownValueSec;
inline bool timerDownFlag = false, timerUpFlag = false;
inline uint32_t period5000msStartTime;
#define PERIOD_5000MS_TIMEOUT 5000


// ******************************************************************** EFIS variables ***********************************************************************
//  Variables used to calculate the position and then display the horizon
inline float g, d, h, b;
inline uint8_t casG, casD;

// Variables relating to the aircraft's attitude, flight symmetry, and acceleration
inline int16_t ballOffset = 0;
inline float previousFilteredAccYValue = 0;
inline float previousFilteredAccZValue = -9.80665;

// Variables diverses
inline uint16_t color1, color2;

// ******************************************************************** Variables ou constantes diverses ***********************************************************************
//#define DEG_TO_RAD 0.01745329251
inline uint32_t topLoop;  // 3 variables used to calculate FPS
inline uint16_t cptLoop = 1;
inline float totalLoop = 0;


// ******************************** Structure for storing data received from the RDAM via ESP-NOW *********************************************

typedef struct
{
  float oat;
  float rh;
  float magHeading;
  float roll;
  float pitch;
  float AccY;
  float AccZ;
  float Vz;
  float trk;
  uint8_t hourGNSS;
  uint8_t minuteGNSS;
  uint8_t secondGNSS;
  uint8_t satInView;
  float altitudeGNSS;
  float groundSpeedGNSS;
  uint8_t lowerLimit;
  uint8_t upperLimit;
  uint16_t totalNbSec;
  float fuelUsed;
  float fuelFlow;
  float calcFuelLevel;
  uint16_t rpmInt;
  float vBat;
  float manifoldPressure = 29.92;
  float cht2;
  float cht3;
  float egt3;
  float egt4;
  float oilT;
  float oilP;
  float afr = 10.0;
  float iBus;
  float iBat;
  float iat;
  float dewPoint;
  float qnhAltitude;
  float pressureAltitude;
  float baroVario;
  float indicatedAirSpeed;
  int16_t trueAirSpeed;
  int16_t qfeAltitudeInt;
  int16_t densityAltitudeInt;
  int16_t windSpeedInt;
  int16_t windDirectionInt;
} FromRDAMdataStructure;

inline FromRDAMdataStructure fromRDAMdata;


// ************************************************** Structure for storing data to send to the RDAM via ESP-NOW ******************************************************

typedef struct
{
  int16_t QNH;
  int16_t QFE;
  float speedConversionFactor;
  int16_t pressureCorr;  // Correction of an absolute pressure offset, expressed in Pa tenth. It can be adjusted in the menus to correct an altimetric offset error.
                         // Use with caution, see https://avionicsduino.com/index.php/en/efis-pressure-sensors/
  int16_t magDev;
} ToModuleDataStructure;

inline ToModuleDataStructure toModuleData;

// ******************************************************** ESP-NOW variables ***************************************************************
inline uint8_t broadcastAddress[] = { 0xB8, 0xF8, 0x62, 0xFA, 0x2C, 0xCC };  // Flight test aircraft RDAM MAC Adress : B8:F8:62:FA:2C:CC // Bench prototype RDAM MAC Adress : 64:e8:33:50:b9:00
inline esp_now_peer_info_t peerInfo;
inline bool flagAffiche = false;


#endif