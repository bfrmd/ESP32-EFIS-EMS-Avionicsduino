#ifndef RDAMGLOBALS_H
#define RDAMGLOBALS_H

// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                  External libraries inclusions
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <driver/twai.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_now.h>
#include <WiFi.h>
#include "esp_timer.h"  // We need high-resolution ESP software timers (1 µs resolution) rather than FreeRTOS software timers whose resolution are "only" one millisecond.
#include "driver/i2c_master.h"

// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                           RDAM object instantiation
// --------------------------------------------------------------------------------------------------------------------------------------------------------------
inline OneWire oneWire(GPIO_NUM_7);
inline DallasTemperature sensors(&oneWire);

//--------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                             Variables et constantes globales
// -------------------------------------------------------------------------------------------------------------------------------------------------

//***************************************** Timer handles and flags *********************************************************************
inline esp_timer_handle_t oneshot_timer_5ms;  // This timer is used in one-shot mode to query the MPRLS sensor 5 ms after sending it a command.
inline esp_timer_handle_t oneshot_timer_50ms;  // This timer is used in one-shot mode to trigger reading CAN mailbox, air data computation, sending ESP-NOW messages to dysplay module
inline esp_timer_handle_t oneshot_timer_200ms;  // This timer is used in one-shot mode trigger sending data to the CAN bus
inline esp_timer_handle_t oneshot_timer_250ms;  // This timer is used in one-shot mode to trigger barometric variometer integration timeout
inline esp_timer_handle_t oneshot_timer_500ms;  // This timer is used in one-shot mode to trigger sending debugging messages to the serial monitor
inline esp_timer_handle_t oneshot_timer_5000ms;  // This timer is used in one-shot mode to query the DS18B20 Temperature Sensor (for IAT) every 5 seconds

inline bool timeout5ms = false;
inline bool timeout50ms = true;
inline bool timeout200ms = true;
inline bool timeout250ms = true;
inline bool timeout500ms = true;
inline bool timeout5000ms = true;

//**********timeout5ms**************** Variables and constants relating to MPRLS pressure sensors **************************************************
#define PRESSURESPAN 50.9005
#define OUTPUTMIN 1677722
#define OUTPUTSPAN 0xCCCCCC
inline float nonFilteredManifoldPressure = 29.92, previousFilteredManifoldPressureValue = 29.92;
inline bool mprReadyToReceiveCommand = true;
inline const uint8_t cmd_mprls[3] = { 0xAA, 0x00, 0x00 };  // command to be sent to the MPRLS sensor
inline uint8_t data_mprls[4];                              // buffer to receive data from MPRLS pressure sensor

// ************************************************* I2C configuration ****************************************************************************
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO GPIO_NUM_5
#define I2C_MASTER_SCL_IO GPIO_NUM_6
#define I2C_MASTER_FREQ_HZ_HIGH 1000000
#define I2C_MASTER_FREQ_HZ_LOW 300000  // The I2C bus needs to slow down for the AMS pressure sensors.
#define BARODEV_ADDRESS 0x31
#define DIFFDEV_ADDRESS 0x28
#define MAPDEV_ADDRESS 0x18
#define ADC1DEV_ADDRESS 0x4A
#define ADC2DEV_ADDRESS 0x48
inline static i2c_master_bus_handle_t i2c_bus_handle = NULL;
inline i2c_master_bus_config_t i2c_bus_config;
inline static i2c_master_dev_handle_t baroDev_handle = NULL;
inline static i2c_master_dev_handle_t diffDev_handle = NULL;
inline static i2c_master_dev_handle_t mapDev_handle = NULL;
inline static i2c_master_dev_handle_t adc1Dev_handle = NULL;
inline static i2c_master_dev_handle_t adc2Dev_handle = NULL;
inline i2c_device_config_t i2c_device_config;

// ****************************************************** Constants and intermediate variables used when calculating air data : altitudes, vario, speeds *****************************************************
inline uint8_t data_ams[4];
inline const uint16_t digoutpmin = 1638, digoutpmax = 14745, digoutrange = 13107;
inline const float pminbaro = 0, pmaxbaro = 1500, prangebaro = 1500;
inline const float pmindiff = 0, pmaxdiff = 50, prangediff = 50;

inline float altitudeCorrection = 1;  // To correct for any error proportional to altitude. To be determined during flight tests by comparison with another altimeter.
inline float pressure = 101300;
inline float nonFilteredPressure = 101300;
inline float previousFilteredPressureValue = 101300;
inline float differentialPressure = 0;
inline float nonFilteredDifferentialPressure;
inline float previousFilteredDiffPressureValue = 0;

inline int16_t nonFilteredIndicatedAirSpeed, nonFilteredTrueAirSpeed;
inline float altitudeVario, altitudeVarioOld;

inline float previousFilteredBaroVarioValue;

//****************************************** External sensors related variables **************************************************
inline float prevFiltOilP;
inline float prevFiltOilT;
inline float prevFiltCht2;
inline float prevFiltCht3;
inline float prevFiltIBus;
inline float prevFiltIBat;
inline float prevFiltEgt3 = 20.0;
inline float prevFiltEgt4 = 20.0;
inline float prevFiltAfr = 10.0;

//****************************************** Constants related to certain PCB resistance values  **************************************************
inline const float numOilPoffset = 585.0, oilPgain = 1.6324;  // numOilPoffset = 4095 * R31 / (R31 + R32) = 4095*3000(3000+18000)= 585. oilPgain = 1 + (R37/R35) = 1+(43000/68000)= 1.6324 -> see KICAD diagram of the RDAM and the component datasheet.
inline const float rSeriesOilPSensor = 50;                    // = R38, which is the value of the resistance in series with the oil pressure sensor, i.e. 50 ohms.
inline const float numTOffset = 729.2466, tGain = 1.5806;     // The digital offset and gain are identical for all 3 temperature sensors (oil, cht2, cht3). The resistance values ​​are the same for all 3 sensors.
inline const float rSeriesTsensor = 91;

//****************************************** Miscellaneous variables and constants *********************************************************
// The ADC2 is responsible for the non-ratiometric sensors. Its internal reference voltage is supposed to be exactly 2.5 volts. However, this voltage was measured at 2.514 volts with the ADS7828 used. This should be verified with a different ADS7828..
// All electronic calculations (resistance values, etc.) are based on a 2.5-volt reference. This means that 2.5 volts input to the ADS7828 should result in 4095 bits output.
// However, 2.5 volts at the ADC input, while ref = 2.514, only yields 4072.2 bits. This must be taken into account in the program. Hence the constant OUTPUTVAL2_5 = 4072.2, to be used for all non-ratiometric sensors.
#define OUTPUTVAL2_5 4072.2
inline uint8_t cmd_adc[1];   // command to sent to the ADS7828 ADC
inline uint8_t data_adc[2];  // buffer to receive data from ADS7828 ADC

// ******************************************************************** FreeRTOS variables : task handle, queue, mutex ... ***********************************************************************
inline TaskHandle_t TaskCAN = NULL;
inline QueueHandle_t CANmailbox = NULL;
inline BaseType_t rc;
#define MUTEX_TIMEOUT 5  // 5ms timeout
inline SemaphoreHandle_t airDataMutex = NULL;

// ******************************************************************** Constants, variables and structure for the CAN bus ***********************************************************************
#define RX_GPIO_PIN GPIO_NUM_8  // CAN transceiver pins
#define TX_GPIO_PIN GPIO_NUM_9
inline bool canControllerReady = false;

// To send messages ID 46, 48, 50, 52, 54, 56, 58, 60 to the CAN bus
inline twai_message_t msgQNHaltitudesVario;  
inline twai_message_t msgSpeeds_Wind;
inline twai_message_t msgPA;
inline twai_message_t msgCHT;
inline twai_message_t msgHuile;
inline twai_message_t msgEGT;
inline twai_message_t msgAFR_Vbus;
inline twai_message_t msgCurrents;

typedef struct  // Structure for the the reception and storage of CAN bus data
{
  float oat;
  float rh = 50;
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
} CanDataStructure;

inline CanDataStructure canDataRealTime, canDataSnapshot;  // structureA
inline float previousFilteredAccYValue = 0;
inline float previousFilteredAccZValue = 1;

// ***************************** Structure for storing all values calculated by the RDAM **************************************************

typedef struct
{
  float manifoldPressure;
  float cht2;
  float cht3;
  float egt3;
  float egt4;
  float oilT;
  float oilP;
  float afr;
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

} ModuleDataStructure;

inline ModuleDataStructure moduleData;  // structureB

// ******************************** Structure grouping the two previous ones. Used to send all these variables to the display via ESP-NOW *********************************************

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
  float vBat = 13.7;
  float manifoldPressure;
  float cht2;
  float cht3;
  float egt3;
  float egt4;
  float oilT;
  float oilP;
  float afr;
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
} ToDisplayDataStructure;

inline ToDisplayDataStructure toDisplayData;  // structureFusion A et B

// ************************************************** Structure for storing data received from the display via ESP-NOW ******************************************************
typedef struct
{
  int16_t QNH;
  int16_t QFE;
  float speedConversionFactor;
  int16_t pressureCorr;  // Correction of an absolute pressure offset, expressed in Pa tenth. It can be adjusted in the menus to correct an altimetric offset error.
                         // Use with caution, see https://avionicsduino.com/index.php/en/efis-pressure-sensors/
  int16_t magDev;
} FromDisplayDataStructure;

inline FromDisplayDataStructure fromDisplayData;

// ******************************************************** ESP-NOW variables ***************************************************************
inline uint8_t broadcastAddress[] = { 0x30, 0xED, 0xA0, 0x34, 0xC1, 0xB8 };  // Flight test aircraft display Module MAC Adress : 30:ed:a0:34:c1:b8 // Bench prototype display Module MAC Adress : 10:20:ba:02:8a:30
inline esp_now_peer_info_t peerInfo;
inline bool flagAffiche = false;

// *********************************************************** Debugging flags ***************************************************************
inline bool debug, debugEspNow, debugCAN;

#endif