#include "RDAMtimers.h"

void timersInit()
{
  //************************************************************************ Initialize one shot timers parameters ******************************************************************
  // oneshot_timer_5ms : This timer is used to query the MPRLS sensor 5 ms after sending it a command.
  const esp_timer_create_args_t oneshot_timer_args_5ms = {
    .callback = &oneshot_timer_5ms_callback,  // link the call back
    .arg = nullptr,                           // not passing in anything
    .name = "one-shot-5ms"                    // name
  };
  ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args_5ms, &oneshot_timer_5ms));  //create timer, not running it yet.


  // one_shot_timer_50ms : this timer is used to trigger reading CANmailbox, air data computation, and sending ESP-NOW data to the display module
  const esp_timer_create_args_t oneshot_timer_args_50ms = {
    .callback = &oneshot_timer_50ms_callback,  
    .arg = nullptr,
    .name = "one-shot-50ms"
  };
  ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args_50ms, &oneshot_timer_50ms));


  // one_shot_timer_200ms : this timer is used to trigger sending data to tne CAN Bus
  const esp_timer_create_args_t oneshot_timer_args_200ms = {
    .callback = &oneshot_timer_200ms_callback,
    .arg = nullptr,
    .name = "one-shot-200ms"
  };
  ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args_200ms, &oneshot_timer_200ms));


  // one_shot_timer_250ms : this timer is used to trigger barometric variometer integration timeout
  const esp_timer_create_args_t oneshot_timer_args_250ms = {
    .callback = &oneshot_timer_250ms_callback,
    .arg = nullptr,
    .name = "one-shot-250ms"
  };
  ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args_250ms, &oneshot_timer_250ms));


  // one_shot_timer_500ms : this timer is used to trigger debug display to serial monitor
  const esp_timer_create_args_t oneshot_timer_args_500ms = {
    .callback = &oneshot_timer_500ms_callback,
    .arg = nullptr,
    .name = "one-shot-500ms"
  };
  ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args_500ms, &oneshot_timer_500ms));

  // one_shot_timer_5000ms : this timer is used to query the DS18B20 Temperature Sensor (for IAT) every 5 seconds
  const esp_timer_create_args_t oneshot_timer_args_5000ms = {
    .callback = &oneshot_timer_5000ms_callback,
    .arg = nullptr,
    .name = "one-shot-5000ms"
  };
  ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args_5000ms, &oneshot_timer_5000ms));
}

static void oneshot_timer_5ms_callback(void *arg)
{
  timeout5ms = true;
}

static void oneshot_timer_50ms_callback(void *arg)
{
  timeout50ms = true;
}

static void oneshot_timer_200ms_callback(void *arg)
{
  timeout200ms = true;
}

static void oneshot_timer_250ms_callback(void *arg)
{
  timeout250ms = true;
}

static void oneshot_timer_500ms_callback(void *arg)
{
  timeout500ms = true;
}

static void oneshot_timer_5000ms_callback(void *arg)
{
  timeout5000ms = true;
}