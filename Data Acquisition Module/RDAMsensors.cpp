#include "RDAMsensors.h"

void sensorsInit()
{
  // *************************************************************************** Initializing the DS18B20 inside air temperature sensor ********************************************************************************************
  sensors.begin();

  //***************************************************************** Initializing the I2C bus ************************************************************************
  i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
  i2c_bus_config.i2c_port = I2C_MASTER_NUM;
  i2c_bus_config.scl_io_num = I2C_MASTER_SCL_IO;
  i2c_bus_config.sda_io_num = I2C_MASTER_SDA_IO;
  i2c_bus_config.glitch_ignore_cnt = 7;
  i2c_bus_config.flags.enable_internal_pullup = true;
  if (i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle) != ESP_OK)
    {
      Serial.println("I2C bus initialization failed");
      while (1) { ; }
    }
  else
    {
      Serial.println("I2C bus initialized");
    }

  //**************************************************************************** Initialisation des devices I2C ***************************************************************
  i2c_device_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  i2c_device_config.scl_speed_hz = I2C_MASTER_FREQ_HZ_LOW;
  i2c_device_config.device_address = BARODEV_ADDRESS;
  i2c_master_bus_add_device(i2c_bus_handle, &i2c_device_config, &baroDev_handle);
  i2c_device_config.device_address = DIFFDEV_ADDRESS;
  i2c_master_bus_add_device(i2c_bus_handle, &i2c_device_config, &diffDev_handle);
  i2c_device_config.scl_speed_hz = I2C_MASTER_FREQ_HZ_HIGH;
  i2c_device_config.device_address = MAPDEV_ADDRESS;
  i2c_master_bus_add_device(i2c_bus_handle, &i2c_device_config, &mapDev_handle);
  i2c_device_config.device_address = ADC1DEV_ADDRESS;
  i2c_master_bus_add_device(i2c_bus_handle, &i2c_device_config, &adc1Dev_handle);
  i2c_device_config.device_address = ADC2DEV_ADDRESS;
  i2c_master_bus_add_device(i2c_bus_handle, &i2c_device_config, &adc2Dev_handle);
  delay(1000);

  //********************************************************** Verification of the correct operation of all I2C devices ****************************************************************
  if (i2c_master_probe(i2c_bus_handle, BARODEV_ADDRESS, 1000) == ESP_OK) Serial.println("I2C device AMS5915_1500_A added and initialized");
  else
    {
      Serial.println("I2C device AMS5915_1500_A initialization failed");
      while (1) { ; }
    }

  if (i2c_master_probe(i2c_bus_handle, DIFFDEV_ADDRESS, 1000) == ESP_OK) Serial.println("I2C device AMS5915_0050_D added and initialized");
  else
    {
      Serial.println("I2C device AMS5915_0050_D initialization failed");
      while (1) { ; }
    }

  if (i2c_master_probe(i2c_bus_handle, MAPDEV_ADDRESS, 1000) == ESP_OK) Serial.println("I2C device MPRLS sensor added and initialized");
  else
    {
      Serial.println("I2C MPRLS sensor initialization failed");
      while (1) { ; }
    }

  if (i2c_master_probe(i2c_bus_handle, ADC1DEV_ADDRESS, 1000) == ESP_OK) Serial.println("I2C device ADS7828 #1 added and initialized");
  else
    {
      Serial.println("I2C device ADS7828 #1 initialization failed");
      while (1) { ; }
    }

  if (i2c_master_probe(i2c_bus_handle, ADC2DEV_ADDRESS, 1000) == ESP_OK) Serial.println("I2C device ADS7828 #2 added and initialized");
  else
    {
      Serial.println("I2C device ADS7828 #2 initialization failed");
      while (1) { ; }
    }
  delay(500);
}

void sensorsRead()
{
  // ************************************************************** Reading from the inside air temperature sensor every 5 seconds ****************************************************
  if (timeout5000ms)
    {
      timeout5000ms = false;
      sensors.requestTemperatures();
      moduleData.iat = sensors.getTempCByIndex(0);

      if (esp_timer_is_active(oneshot_timer_5000ms))  //check state of timer and stop it if needed..
        {
          ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer_5000ms));
        }
      ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer_5000ms, 5000000));  //restart our one shot timer for 5 seconds
    }
  // *********************************************************************** baro pressure calculation **********************************************************************
  // static pressure calculation
  if (i2c_master_receive(baroDev_handle, data_ams, sizeof(data_ams), 1000) == ESP_OK)
    {
      uint16_t output = (data_ams[0] << 8 | data_ams[1]);
      //Serial.println(output);
      pressure = ((output - digoutpmin) / (digoutrange / prangebaro) + pminbaro) * 100;  // result in Pa
    }
  nonFilteredPressure = pressure;
  pressure = iirFilter(previousFilteredPressureValue, pressure, 0.005);
  previousFilteredPressureValue = pressure;

  // differential pressure calculation
  if (i2c_master_receive(diffDev_handle, data_ams, sizeof(data_ams), 1000) == ESP_OK)
    {
      uint16_t output = (data_ams[0] << 8 | data_ams[1]);
      differentialPressure = ((output - digoutpmin) / (digoutrange / prangediff) + pmindiff) * 100;  // result in Pa
    }
  nonFilteredDifferentialPressure = differentialPressure;
  differentialPressure = iirFilter(previousFilteredDiffPressureValue, differentialPressure, 0.005);
  previousFilteredDiffPressureValue = differentialPressure;

  //MAP calculation
  if (mprReadyToReceiveCommand)
    {
      i2c_master_transmit(mapDev_handle, cmd_mprls, sizeof(cmd_mprls), -1);
      mprReadyToReceiveCommand = false;
      if (esp_timer_is_active(oneshot_timer_5ms))  //check state of timer and stop it if needed..
        {
          ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer_5ms));
        }
      ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer_5ms, 5000));  //restart our one shot timer for 5 milliseconds
    }

  if (timeout5ms)
    {
      if (i2c_master_receive(mapDev_handle, data_mprls, sizeof(data_mprls), -1) == ESP_OK)
        {
          timeout5ms = false;
          mprReadyToReceiveCommand = true;
          if (data_mprls[0] == 0x40)  // if status byte OK
            {
              uint32_t output = (data_mprls[1] << 16 | data_mprls[2] << 8 | data_mprls[3]);
              moduleData.manifoldPressure = (output - OUTPUTMIN) * (PRESSURESPAN) / (OUTPUTSPAN);
              nonFilteredManifoldPressure = moduleData.manifoldPressure;
              moduleData.manifoldPressure = iirFilter(previousFilteredManifoldPressureValue, moduleData.manifoldPressure, 0.01);
              previousFilteredManifoldPressureValue = moduleData.manifoldPressure;
            }
        }
    }

  float digitalValue;

  // Oil pressure calculation, adc1 channel 0
  cmd_adc[0] = 0b10000100;
  i2c_master_transmit_receive(adc1Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  float tnvovd = (digitalValue / oilPgain) + numOilPoffset;                // tnvovd = Theoretical Numerical Value at the Output of the Voltage Divider, cf. mon document "Projet EFIS-EMS ESP32.docx"
  float rSensor = tnvovd * rSeriesOilPSensor / (4095 - tnvovd);            // Calculation of the oil pressure sensor resistance value
  moduleData.oilP = 0.1418 * pow(rSensor, 2) + 38.806 * rSensor - 383.25;  // See the explanation of this formula: https://avionicsduino.com/index.php/fr/le-capteur-de-pression-dhuile-rotax/
  moduleData.oilP = iirFilter(prevFiltOilP, moduleData.oilP, 0.01);
  prevFiltOilP = moduleData.oilP;

  // Oil température calculation, adc1 channel 1
  cmd_adc[0] = 0b11000100;
  i2c_master_transmit_receive(adc1Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  tnvovd = (digitalValue / tGain) + numTOffset;
  rSensor = tnvovd * rSeriesTsensor / (4095 - tnvovd);                            // Calculation of the resistance value of the oil temperature sensor
  moduleData.oilT = 3.193 * pow(log(rSensor), 2) - 61.72 * log(rSensor) + 302.2;  // See the explanation of this formula: : https://avionicsduino.com/index.php/fr/les-capteurs-de-temperature-des-moteurs-rotax/
  moduleData.oilT = iirFilter(prevFiltOilT, moduleData.oilT, 0.05);
  prevFiltOilT = moduleData.oilT;

  // cht2 calculation, adc1 channel 2
  cmd_adc[0] = 0b10010100;
  i2c_master_transmit_receive(adc1Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  tnvovd = (digitalValue / tGain) + numTOffset;
  rSensor = tnvovd * rSeriesTsensor / (4095 - tnvovd);  // calculation of the resistance value of the CHT2 sensor
  moduleData.cht2 = 3.193 * pow(log(rSensor), 2) - 61.72 * log(rSensor) + 302.2;
  moduleData.cht2 = iirFilter(prevFiltCht2, moduleData.cht2, 0.05);
  prevFiltCht2 = moduleData.cht2;

  // cht3 calculation, adc1 channel 3
  cmd_adc[0] = 0b11010100;
  i2c_master_transmit_receive(adc1Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  tnvovd = (digitalValue / tGain) + numTOffset;
  rSensor = tnvovd * rSeriesTsensor / (4095 - tnvovd);  // calculation of the resistance value of the CHT3 sensor
  moduleData.cht3 = 3.193 * pow(log(rSensor), 2) - 61.72 * log(rSensor) + 302.2;
  moduleData.cht3 = iirFilter(prevFiltCht3, moduleData.cht3, 0.05);
  prevFiltCht3 = moduleData.cht3;

  // Bus current calculation, adc2 channel 1
  // (if iBus input stage output = 0 -> iBus = 0, if iBus input stage output = 2.5 volts -> output ADS7828 = OUTPUTVAL2_5 -> iBus = 20)
  //digitalValue = (float)adc2.read(1);
  cmd_adc[0] = 0b11001100;
  i2c_master_transmit_receive(adc2Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  digitalValue = digitalValue * 4095 / OUTPUTVAL2_5;  // Correction for Vref = 2.514 instead of 2.5 volts
  moduleData.iBus = digitalValue * 20 / OUTPUTVAL2_5;
  moduleData.iBus = iirFilter(prevFiltIBus, moduleData.iBus, 0.01);
  prevFiltIBus = moduleData.iBus;

  // Battery current calculation, adc2 channel 2
  // if VShunt = +33 mV -> Ibat = +13.2A -> output INA296 = 1.65 + 0.033*50 = 3.3 -> input ADC = 2.5 -> output ADS7828 = OUTPUTVAL2_5 -> iBat = +13.2
  // if VShunt = -33 mV -> Ibat = -13.2A -> output INA296 = 1.65 - 0.033*50 = 0   -> input ADC = 0   -> output ADS7828 = 0            -> iBat = -13.2
  //digitalValue = (float)adc2.read(2);
  cmd_adc[0] = 0b10011100;
  i2c_master_transmit_receive(adc2Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  digitalValue = digitalValue * 4095 / OUTPUTVAL2_5;  // Correction for Vref = 2.514 instead of 2.5 volts
  moduleData.iBat = (digitalValue * 26.4 / OUTPUTVAL2_5) - 13.2;
  moduleData.iBat = iirFilter(prevFiltIBat, moduleData.iBat, 0.01);
  prevFiltIBat = moduleData.iBat;

  // AFR calculation, adc2 channel 3
  // (if AFR controller output = 0 -> AFR = 10, if controller output = 5 volts -> output ADS7828 = OUTPUTVAL2_5 -> AFR = 20)
  //digitalValue = (float)adc2.read(3);
  cmd_adc[0] = 0b11011100;
  i2c_master_transmit_receive(adc2Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  moduleData.afr = digitalValue / (OUTPUTVAL2_5 / 10.0) + 10;
  moduleData.afr = iirFilter(prevFiltAfr, moduleData.afr, 0.01);
  prevFiltAfr = moduleData.afr;

  // EGT3 calculation, adc2 channel 6
  // If EGT controller ouput = 0 -> EGT = 0°, if controller output = 5 volts -> output ADS7828 = 4095 -> EGT = 1000°
  //digitalValue = (float)adc2.read(6);
  cmd_adc[0] = 0b10111100;
  i2c_master_transmit_receive(adc2Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  moduleData.egt3 = (1000.0 / OUTPUTVAL2_5) * digitalValue;  // egt3 = (1000/4095) * digitalValue
  moduleData.egt3 = iirFilter(prevFiltEgt3, moduleData.egt3, 0.05);
  prevFiltEgt3 = moduleData.egt3;

  //EGT4 calculation, adc2 channel 7
  //digitalValue = (float)adc2.read(7);
  cmd_adc[0] = 0b11111100;
  i2c_master_transmit_receive(adc2Dev_handle, cmd_adc, sizeof(cmd_adc), data_adc, 2, -1);
  digitalValue = (float)(data_adc[0] << 8 | data_adc[1]);
  moduleData.egt4 = (1000.0 / OUTPUTVAL2_5) * digitalValue;
  moduleData.egt4 = iirFilter(prevFiltEgt4, moduleData.egt4, 0.05);
  prevFiltEgt4 = moduleData.egt4;
}

void airDataComputation()
{
  if (xSemaphoreTake(airDataMutex, MUTEX_TIMEOUT))
    {
      // --------- calcul des altitudes --------- (Pressures are expressed in Pascals and filtered at the sensor reading stage, but QNH/QFE are expressed in hundredths of an inch of mercury)
      moduleData.qnhAltitude = (1 - pow((((pressure + fromDisplayData.pressureCorr * 10) / 100) / (fromDisplayData.QNH * 0.338646)), 0.190284)) * 145366.45 * altitudeCorrection;  // 0.01 inHg = 0.338646 hPa
      float qfeAltitude = (1 - pow((((pressure + fromDisplayData.pressureCorr * 10) / 100) / (fromDisplayData.QFE * 0.338646)), 0.190284)) * 145366.45 * altitudeCorrection;
      moduleData.qfeAltitudeInt = int((qfeAltitude + 5) / 10.0) * 10;
      moduleData.pressureAltitude = (1 - pow((((pressure + fromDisplayData.pressureCorr * 10) / 100) / 1013.25), 0.190284)) * 145366.45 * altitudeCorrection;
      altitudeVario = moduleData.pressureAltitude;
      moduleData.pressureAltitude = int((moduleData.pressureAltitude + 5) / 10.0) * 10;
      //Serial.println(pressure);

      float pressionSatH20 = 6.1078 * pow(10, ((7.5 * canDataSnapshot.oat) / (237.3 + canDataSnapshot.oat))) * 100;
      float pressionPartielleH2O = pressionSatH20 * canDataSnapshot.rh / 100;
      float densiteAir = ((pressure + fromDisplayData.pressureCorr * 10) / (287.05 * (canDataSnapshot.oat + 273.15)) * (1 - (0.378 * pressionPartielleH2O / (pressure + fromDisplayData.pressureCorr * 10))));
      float densityAltitude = (44.3308 - (42.2665 * (pow(densiteAir, 0.234969)))) * 1000 * 3.28084;
      moduleData.densityAltitudeInt = int((densityAltitude + 5) / 10.0) * 10;
      moduleData.dewPoint = 243.12 * (log(canDataSnapshot.rh / 100) + 17.62 * canDataSnapshot.oat / (243.12 + canDataSnapshot.oat)) / (17.62 - (log(canDataSnapshot.rh / 100) + 17.62 * canDataSnapshot.oat / (243.12 + canDataSnapshot.oat)));

      // ----------- Calculation of the rise/fall rate from the static pressure sensor -------------------
      if (timeout250ms)
        {
          if (esp_timer_is_active(oneshot_timer_250ms))  //check state of timer and stop it if needed..
            {
              ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer_250ms));
            }
          ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer_250ms, 250000));  //restart our one shot timer for 250 milliseconds
          timeout250ms = false;
          moduleData.baroVario = (altitudeVario - altitudeVarioOld) * 60 * (1000 / 250);
          altitudeVarioOld = altitudeVario;
          moduleData.baroVario = iirFilter(previousFilteredBaroVarioValue, moduleData.baroVario, 0.1);
          previousFilteredBaroVarioValue = moduleData.baroVario;
          if (moduleData.baroVario >= 0) moduleData.baroVario = (int(moduleData.baroVario + 5) / 10) * 10;
          else moduleData.baroVario = (int(moduleData.baroVario - 5) / 10) * 10;
        }


      // ----------------------- Calculation of aircraft speeds, results in the unit selected by the user --------------------------------------------
      if (differentialPressure > 4.5)
        {
          moduleData.indicatedAirSpeed = (sqrtf(21.159183 * differentialPressure) / fromDisplayData.speedConversionFactor);                                                        
          moduleData.trueAirSpeed = int16_t((86.257392 * sqrtf(differentialPressure * (273.15 + canDataSnapshot.oat) / pressure) / fromDisplayData.speedConversionFactor) + 0.5);  

          nonFilteredIndicatedAirSpeed = int16_t(sqrtf(21.159183 * nonFilteredDifferentialPressure) + 0.5);                                         // Unfiltered value in km/h for sending to the CAN bus, and therefore to the FDR
          nonFilteredTrueAirSpeed = int16_t(86.257392 * sqrtf(nonFilteredDifferentialPressure * (273.15 + canDataSnapshot.oat) / pressure) + 0.5);  // Unfiltered value in km/h for sending to the CAN bus, and therefore to the FDR
        }
      else
        {
          moduleData.indicatedAirSpeed = 0;
          moduleData.trueAirSpeed = 0;
        }

      // ----------------------- Calculating wind direction and speed (requires a perfectly calibrated and aligned magnetometer!!!) --------------------------------------------
      float GSE = sin(PI / 180 * canDataSnapshot.trk) * canDataSnapshot.groundSpeedGNSS;  // groundSpeedGNSS est exprimée en Km/h
      float GSN = cos(PI / 180 * canDataSnapshot.trk) * canDataSnapshot.groundSpeedGNSS;
      float TASE = sin(PI / 180 * (canDataSnapshot.magHeading + fromDisplayData.magDev)) * moduleData.trueAirSpeed;
      float TASN = cos(PI / 180 * (canDataSnapshot.magHeading + fromDisplayData.magDev)) * moduleData.trueAirSpeed;
      float deltaE = TASE - GSE;
      float deltaN = TASN - GSN;
      moduleData.windSpeedInt = int16_t(sqrt(pow(deltaE, 2) + pow(deltaN, 2)) + 0.5);  // result in km/h
      moduleData.windDirectionInt = int16_t((atan2(deltaE, deltaN) * 180 / PI) + 0.5);
      if (moduleData.windDirectionInt < 0) moduleData.windDirectionInt = 360 + moduleData.windDirectionInt;

      xSemaphoreGive(airDataMutex);
    }
}

void displayDebuggingData()
{
  Serial.printf("Roll : % 3.1f   Pitch : % 3.1f    %c", canDataSnapshot.roll, canDataSnapshot.pitch, '\t');
  Serial.printf("baro : %.1f%c", pressure / 100, '\t');
  Serial.printf("diffP : %.1f%c", differentialPressure, '\t');
  Serial.printf("MAP : %.2f%c", nonFilteredManifoldPressure, '\t');
  Serial.printf("OilP : %.1f%c", moduleData.oilP / 1000, '\t');
  Serial.printf("OilT : %.1f%c", moduleData.oilT, '\t');
  Serial.printf("CHT2 : %.1f%c", moduleData.cht2, '\t');
  Serial.printf("CHT3 : %.1f%c", moduleData.cht3, '\t');
  Serial.printf("Ibus : %.1f%c", moduleData.iBus, '\t');
  Serial.printf("iBat : %.1f%c", moduleData.iBat, '\t');
  Serial.printf("AFR : %.1f%c", moduleData.afr, '\t');
  Serial.printf("EGT3 : %.0f%c", moduleData.egt3, '\t');
  Serial.printf("EGT4 : %.0f%c", moduleData.egt4, '\n');
}

// Infinite Impulse Response (IIR) filter functions
float iirFilter(float previousFilteredValue, float currentValue, float iirFilterCoefficient)
{
  return previousFilteredValue * (1 - iirFilterCoefficient) + currentValue * iirFilterCoefficient;
}
